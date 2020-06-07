#include <jni.h>
#include <string>
#include "DNFFmpeg.h"
#include <android/native_window_jni.h>
#include <pthread.h>

extern "C" {
#include <libavcodec/avcodec.h>
}

DNFFmpeg *ffmpeg = 0;
JavaVM *javaVm = 0;
ANativeWindow *window = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int JNI_OnLoad(JavaVM *vm, void *r) {
    javaVm = vm;
    return JNI_VERSION_1_6;
}

//画画
void render(uint8_t *data,int linesize,int w,int h){
    pthread_mutex_lock(&mutex);
    if (!window){
        return;
    }
    //设置窗口属性
    ANativeWindow_setBuffersGeometry(window, w,
                                     h,
                                     WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        return;
    }
//填充rgb数据给dst_data
    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);

    //stride :一行有多少个数据(RGBA)*4
    int dst_linesize = window_buffer.stride*4;
    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst_data+i*dst_linesize,data+i*linesize,dst_linesize);
    }
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_axin_player_DNPlayer_native_1prepare(JNIEnv *env, jobject instance, jstring data_source) {

    const char *dataSource = env->GetStringUTFChars(data_source, 0);
    JavaCallHelp *help = new JavaCallHelp(javaVm,env,instance);
    ffmpeg = new DNFFmpeg(help,dataSource);
    ffmpeg->setRenderCallback(render);
    ffmpeg->prepare();

    env->ReleaseStringUTFChars(data_source, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_axin_player_DNPlayer_native_1start(JNIEnv *env, jobject instance) {
    ffmpeg->start();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_axin_player_DNPlayer_native_1setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex);
    if(window){
        //把老的释放
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env,surface);
    pthread_mutex_unlock(&mutex);

}