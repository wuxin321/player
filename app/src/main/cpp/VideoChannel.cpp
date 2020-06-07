//
// Created by Administrator on 2020/6/3.
//

#include "VideoChannel.h"
extern "C"{
#include <libavutil/imgutils.h>
}


void* decode_task(void* args){
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->decode();
    return 0;
}
void* render_task(void* args){
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->render();
    return 0;
}



VideoChannel::VideoChannel(int id,AVCodecContext *avCodecContext,int fps) : BaseChannel(id,avCodecContext) {

    this->fps = fps;
}

VideoChannel::~VideoChannel() {

}

void VideoChannel::play(){
    //1、解码
    isPlaying = 1;
    frames.setWork(1);
    packets.setWork(1);
    pthread_create(&pid_decode,0,decode_task,this);
    //2、播放
    pthread_create(&pid_render,0,render_task,this);

}

//解码
void VideoChannel::decode() {
    AVPacket *packet = 0;
    while (isPlaying){
        //取出一个数据包
        int ret = packets.pop(packet);
        if (!isPlaying){
            break;
        }
        if (!ret){
            continue;
        }
        //把包丢给解码器
        ret = avcodec_send_packet(avCodecContext,packet);
        releaseAvPacket(&packet);
        //重试
        if (ret == AVERROR(EAGAIN)){
            continue;
        } else if (ret != 0){
            break;
        }
        //代表了一个图像
        AVFrame *frame = av_frame_alloc();
        //从解码器中读取 解码后的数据包 AVFrame
        ret = avcodec_receive_frame(avCodecContext,frame);
        //需要更多的数据才能够进行解码
        if (ret == AVERROR(EAGAIN)){
            continue;
        } else if (ret!= 0){
            break;
        }
        //再开一个线程 来播放（流畅度）
        frames.push(frame);

    }
    if (packet){
        releaseAvPacket(&packet);
    }

}

//播放
void VideoChannel::render() {
    //目标：RGBA
    swsContext = sws_getContext(avCodecContext->width,
            avCodecContext->height,avCodecContext->pix_fmt,
                                            avCodecContext->width,
                                            avCodecContext->height,
                                            AV_PIX_FMT_RGBA,
                                            SWS_BILINEAR,0,0,0);
    AVFrame* avFrame = 0;
    //指针数组
    uint8_t *dst_data[4];

    int dst_linesize[4];
    av_image_alloc(dst_data,dst_linesize,avCodecContext->width,
            avCodecContext->height,AV_PIX_FMT_RGBA,1);
    while (isPlaying){

        int ret = frames.pop(avFrame);
        if (!isPlaying){
            break;
        }

        //src_linesize:表示每一行存放的字节长度

        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(avFrame->data),
                avFrame->linesize,0,avCodecContext->height,
                  dst_data,dst_linesize
                );
        //回调出去播放
        callback(dst_data[0],dst_linesize[0],avCodecContext->width,avCodecContext->height);
        releaseAVFrame(&avFrame);
    }
    av_freep(&dst_data[0]);
    releaseAVFrame(&avFrame);
}

void VideoChannel::setRenderCallback(RenderFrameCallback callback) {
    this->callback = callback;
}