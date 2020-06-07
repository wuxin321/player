//
// Created by Administrator on 2020/6/2.
//
#include <cstring>
#include <pthread.h>
#include "DNFFmpeg.h"
#include "macro.h"



void* task_prepare(void *args){
    DNFFmpeg *dnfFmpeg = static_cast<DNFFmpeg *>(args);
    dnfFmpeg->_prepare();
    return 0;



}
DNFFmpeg::DNFFmpeg(JavaCallHelp* callHelp,const char *dataSource) {
    //防止datasource参数指向的内存被释放 悬空指针
    //strlen 获得的字符串长度  不包括\0
    this->dataSource = new char[strlen(dataSource)+1];
    this->callHelp = callHelp;
    strcpy(this->dataSource,dataSource);

}

DNFFmpeg::~DNFFmpeg() {
    //释放
    DELETE(dataSource);
    DELETE(callHelp);
}

void DNFFmpeg::prepare() {
    pthread_create(&pid,0,task_prepare,this);
}

void DNFFmpeg::_prepare() {
    //初始化网络 让ffmpeg能够使用网络
    avformat_network_init();
    //1.打开媒体地址(文件地址、直播地址)
    //AVFormatContext: 包含了 视频的信息(宽、高等)
    formatContext = 0;
    int ret = avformat_open_input(&formatContext,dataSource,0,0);
    //ret不为0表示 打开媒体失败
    if(ret){
        LOGE("打开媒体失败:%s",av_err2str(ret));
        callHelp->onError(THREAD_CHILD,FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }
    //2.查找媒体中的音视频流
    ret = avformat_find_stream_info(formatContext,0);
    //小于0则失败
    if (ret < 0){
        LOGE("查找流失败:%s",av_err2str(ret));
        callHelp->onError(THREAD_CHILD,FFMPEG_CAN_NOT_FIND_STREAMS);
        return;
    }

    //nb_streams:几个流（几段视频/音频）
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //可能代表是一个视频  也可能是代表一个音频
        AVStream *stream = formatContext->streams[i];
        //包含了解码 这段流的各种参数信息
        AVCodecParameters *codecpar = stream->codecpar;

        //无论视频还是音频都需要干的一些事情（获得解码器）
        //1、通过当前流 使用的编码方式，查找解码器
        AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
        if (dec == NULL){
            LOGE("查找解码器失败:%s",av_err2str(ret));
            callHelp->onError(THREAD_CHILD,FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        //2.获得解码器上下文
        AVCodecContext *context = avcodec_alloc_context3(dec);
        if (context == NULL){
            LOGE("创建上下文是失败:%s",av_err2str(ret));
            callHelp->onError(THREAD_CHILD,FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }
        //3.设置上下文内的一些参数
        ret = avcodec_parameters_to_context(context,codecpar);
        if (ret < 0){
            LOGE("设置解码上下文参数失败:%s",av_err2str(ret));
            callHelp->onError(THREAD_CHILD,FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        //4.打开解码器
        ret = avcodec_open2(context,dec,0);
        if (ret != 0){
            LOGE("打开解码器失败:%s",av_err2str(ret));
            callHelp->onError(THREAD_CHILD,FFMPEG_OPEN_DECODER_FAIL);
            return;
        }

        //音频
        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO){  //0
            audioChannel = new AudioChannel(i,context);
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO){//视频
            //1
            //帧率：单位时间内 需要显示多少个图像
            AVRational frame_rate = stream->avg_frame_rate;


            int fps = av_q2d(frame_rate);

            videoChannel = new VideoChannel(i,context,fps);
            videoChannel->setRenderCallback(callback);
        }

    }
    //没有音视频
    if (!audioChannel && !videoChannel){
        callHelp->onError(THREAD_CHILD,FFMPEG_NOMEDIA);
        return;
    }
    //准备完了 通知java 你随时可以播放
    callHelp->onPrepare(THREAD_CHILD);
}

void* play(void* args){

    DNFFmpeg *dnfFmpeg = static_cast<DNFFmpeg *>(args);
    dnfFmpeg->_start();
    return 0;
}

void DNFFmpeg::start() {
    //正在播放
    isPlaying = true;
    if (videoChannel){
        //设置为工作状态
        videoChannel->play();
    }
    //启动声音的播放
    if (audioChannel){
        audioChannel->play();
    }
    pthread_create(&pid_play,0,play,this);


}

void DNFFmpeg::_start() {
    //1.读取媒体数据包(音视频数据包)
    int ret;
    while (isPlaying){
        AVPacket *avPacket = av_packet_alloc();
        ret = av_read_frame(formatContext,avPacket);
        //0:成功 其他：失败
        if(ret == 0){
            //stream_index 这一个流的一个序号
            if (audioChannel && avPacket->stream_index == audioChannel->id){
                audioChannel->packets.push(avPacket);
            } else if (videoChannel && avPacket->stream_index == videoChannel->id){
                videoChannel->packets.push(avPacket);
            }

        } else if (ret == AVERROR_EOF){
            //读取完成  但是可能还没有播放完

        } else{

        }

    }

    //2.解码
}

void DNFFmpeg::setRenderCallback(RenderFrameCallback callback) {
    this->callback = callback;

}
