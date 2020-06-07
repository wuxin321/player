//
// Created by Administrator on 2020/6/2.
//

#ifndef PLAYER_DNFFMPEG_H

#define PLAYER_DNFFMPEG_H

extern "C"{
#include <libavformat/avformat.h>
};

#include "JavaCallHelp.h"
#include "AudioChannel.h"
#include "VideoChannel.h"

class DNFFmpeg {
public:
    DNFFmpeg(JavaCallHelp* callHelp,const char* dataSource);
    ~DNFFmpeg();
    void prepare();
    void _prepare();
    void start();
    void _start();
    void setRenderCallback(RenderFrameCallback callback);


private:
    char *dataSource;
    pthread_t pid;
    pthread_t pid_play;
    AVFormatContext *formatContext;
    JavaCallHelp* callHelp;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    bool  isPlaying;
    RenderFrameCallback callback;
};


#endif //PLAYER_DNFFMPEG_H
