//
// Created by Administrator on 2020/6/3.
//

#ifndef PLAYER_JAVACALLHELP_H
#define PLAYER_JAVACALLHELP_H

#include <jni.h>

class JavaCallHelp {
public:
    JavaCallHelp(JavaVM *vm,JNIEnv *env,jobject instance);
    ~JavaCallHelp();
    void onError(int thread,int errorCode);
    void onPrepare(int thread);

private:
    JavaVM *vm;
    JNIEnv *env;
    jobject instance;
    jmethodID  onErrorId;
    jmethodID  onPrepareId;
};


#endif //PLAYER_JAVACALLHELP_H
