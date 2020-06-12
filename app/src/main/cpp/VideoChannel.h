//
// Created by Administrator on 2020/6/3.
//

#ifndef PLAYER_VIDEOCHANNEL_H
#define PLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include "AudioChannel.h"

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
    VideoChannel(int id,AVCodecContext *avCodecContext, AVRational time_base,int fps);
    ~VideoChannel();

    void setAudioChannel(AudioChannel* audioChannel);
    void play();
    void decode();
    void render();
    void stop();
    void setRenderCallback(RenderFrameCallback callback);

private:
    pthread_t pid_decode = 0;
    pthread_t pid_render = 0;

    SwsContext *swsContext;
    RenderFrameCallback callback;

    AudioChannel *audioChannel = 0;
    int fps;

};


#endif //PLAYER_VIDEOCHANNEL_H
