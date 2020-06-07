//
// Created by Administrator on 2020/6/3.
//

#ifndef PLAYER_BASECHANNEL_H
#define PLAYER_BASECHANNEL_H


#include "safe_queue.h"
extern "C"{
#include <libavformat/avformat.h>
};

class BaseChannel {
public:
    BaseChannel(int id,AVCodecContext *avCodecContext,AVRational time_base):id(id),avCodecContext(avCodecContext),time_base(time_base){
        packets.setReleaseCallback(releaseAvPacket);
        frames.setReleaseCallback(releaseAVFrame);
    }
    //虚函数  子类继承父类
    virtual ~BaseChannel(){
        frames.clear();
        packets.clear();
    }
    /**
     * 释放
     * @param packet
     */
    static void releaseAvPacket(AVPacket** packet){
        if (packet){
            av_packet_free(packet);
            *packet = 0;
        }
    }

    static void releaseAVFrame(AVFrame** pAvFrame){
        if (pAvFrame){
            av_frame_free(pAvFrame);
            *pAvFrame = 0;
        }
    }

    //纯虚方法 相当于抽象方法
    virtual void play()=0;
    int id;
    //编码数据包队列
    SafeQueue<AVPacket*> packets;
    //解码数据包队列
    SafeQueue<AVFrame*> frames;
    bool isPlaying;
    AVCodecContext *avCodecContext;
    AVRational time_base;
public:
    double clock;
};


#endif //PLAYER_BASECHANNEL_H
