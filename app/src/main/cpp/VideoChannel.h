//
// Created by Administrator on 2020/6/3.
//

#ifndef PLAYER_VIDEOCHANNEL_H
#define PLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
extern "C"{
#include <libswscale/swscale.h>
}
/**
 * 1.解码
 * 2.播放
 */
 typedef void (*RenderFrameCallback)(uint8_t *,int,int,int);
class VideoChannel : public BaseChannel{
public:
    VideoChannel(int id,AVCodecContext *avCodecContext, int fps);
    ~VideoChannel();
    void play();
    void decode();
    void render();
    void setRenderCallback(RenderFrameCallback callback);

private:
    pthread_t pid_decode;
    pthread_t pid_render;

    SwsContext *swsContext;
    RenderFrameCallback callback;
    int fps;
};


#endif //PLAYER_VIDEOCHANNEL_H