//
// Created by Administrator on 2020/6/3.
//

#include "VideoChannel.h"
#include "macro.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

int fpt;

void *decode_task(void *args) {
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->decode();
    return 0;
}

void *render_task(void *args) {
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->render();
    return 0;
}

/**
 * 丢包直到下一个关键帧
 * @param q
 */
void dropAvPacket(queue<AVPacket*> &q){
    while (!q.empty()){
        AVPacket *packet = q.front();
        //如果不属于I帧
        if (packet->flags != AV_PKT_FLAG_KEY){
            BaseChannel::releaseAvPacket(&packet);
            q.pop();
        } else{
            break;
        }
    }
}

void dropAvFrame(queue<AVFrame*> &q){
    double dif = 1.0/ fpt;
    int num = 0.05 / dif;
    if (num < 1){
        num = 1;
    }
//    LOGE("num = %d,,dif=%f   ,,length=%d",num,dif,q.size());
    while (!q.empty() && num > 0){
        AVFrame *frame = q.front();
        BaseChannel::releaseAVFrame(&frame);
        q.pop();
        num--;

    }
}


VideoChannel::VideoChannel(int id, AVCodecContext *avCodecContext, AVRational time_base, int fps)
        : BaseChannel(id, avCodecContext, time_base) {
    fpt = fps;
    this->fps = fps;
    //用于 设置一个  同步操作  队列的一个函数指针

    frames.setSyncHandle(dropAvFrame);

}


VideoChannel::~VideoChannel() {

}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}

void VideoChannel::play() {
    //1、解码
    isPlaying = 1;
    frames.setWork(1);
    packets.setWork(1);
    pthread_create(&pid_decode, 0, decode_task, this);
    //2、播放
    pthread_create(&pid_render, 0, render_task, this);

}

//解码
void VideoChannel::decode() {
    AVPacket *packet = 0;
    while (isPlaying) {
        //取出一个数据包
        int ret = packets.pop(packet);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        //把包丢给解码器
        ret = avcodec_send_packet(avCodecContext, packet);
        releaseAvPacket(&packet);
        //重试
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret != 0) {
            break;
        }
        //代表了一个图像
        AVFrame *frame = av_frame_alloc();
        //从解码器中读取 解码后的数据包 AVFrame
        ret = avcodec_receive_frame(avCodecContext, frame);
        //需要更多的数据才能够进行解码
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret != 0) {
            break;
        }
        //再开一个线程 来播放（流畅度）
        frames.push(frame);

    }
    if (packet) {
        releaseAvPacket(&packet);
    }

}

//播放
void VideoChannel::render() {
    //目标：RGBA
    swsContext = sws_getContext(avCodecContext->width,
                                avCodecContext->height, avCodecContext->pix_fmt,
                                avCodecContext->width,
                                avCodecContext->height,
                                AV_PIX_FMT_RGBA,
                                SWS_BILINEAR, 0, 0, 0);
    //每个画面刷新的间隔
    double frame_delays = 1.0 / fps;

    AVFrame *avFrame = 0;
    //指针数组
    uint8_t *dst_data[4];

    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize, avCodecContext->width,
                   avCodecContext->height, AV_PIX_FMT_RGBA, 1);
    while (isPlaying) {

        int ret = frames.pop(avFrame);
        if (!isPlaying) {
            break;
        }

        //src_linesize:表示每一行存放的字节长度

        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(avFrame->data),
                  avFrame->linesize, 0, avCodecContext->height,
                  dst_data, dst_linesize
        );
        //获得当前这一个画面 播放的相对时间(大多数和pts值是一样的)

        double clock = avFrame->best_effort_timestamp * av_q2d(time_base);

        double extra_delay = avFrame->repeat_pict /(2 *fps);
        //真实需要的延时的时间
        double delays = frame_delays+extra_delay;
        if (!audioChannel) {
            LOGE("audioChannel为null");
            av_usleep(delays * 1000000);
        } else {

            if (clock == 0) {
                //休眠
                av_usleep(delays * 1000000);
            } else {
                //比较音频与视频
                double audioClock = audioChannel->clock;
                //音视频相差的间隔
                double  diff = clock - audioClock;
                if (diff > 0){//表示视频比较快
                    LOGE("视频快了%f",diff);
                   av_usleep((diff + delays)*1000000);
                } else if (diff < 0){ //表示音频比较快
                    LOGE("音频快了%f",diff);
                    //不睡了  快点赶上音频
                    //视频包挤压的太多了（丢包）
                    if(fabs(diff) > 0.05){
                        releaseAVFrame(&avFrame);
                        //丢包
                        frames.sync();
                        continue;
                    }

                }


            }
        }

        //回调出去播放
        callback(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);
        releaseAVFrame(&avFrame);
    }
    av_freep(&dst_data[0]);
    releaseAVFrame(&avFrame);
    isPlaying = 0;
    sws_freeContext(swsContext);
    swsContext = 0;
}

void VideoChannel::setRenderCallback(RenderFrameCallback callback) {
    this->callback = callback;
}

void VideoChannel::stop() {
    isPlaying = 0;
    frames.setWork(0);
    packets.setWork(0);
    if (pid_render){
        pthread_join(pid_decode,0);
    }
    if (pid_render){
        pthread_join(pid_render,0);
    }




}