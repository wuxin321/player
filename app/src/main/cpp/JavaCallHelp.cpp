//
// Created by Administrator on 2020/6/3.
//

#include "JavaCallHelp.h"
#include "macro.h"

JavaCallHelp::
JavaCallHelp(JavaVM *vm, JNIEnv *env, jobject instance) {
    this->vm = vm;
    //如果在主线程 回调
    this->env = env;

    //一旦涉及到jobject：跨方法 线程就需要创建全局引用
    this->instance = env->NewGlobalRef(instance);
    jclass clazz = env->GetObjectClass(instance);
    onErrorId = env->GetMethodID(clazz,"onError","(I)V");
    onPrepareId = env->GetMethodID(clazz,"onPrepare","()V");
}


JavaCallHelp::~JavaCallHelp() {
    env->DeleteGlobalRef(instance);
}


void JavaCallHelp::onError(int thread, int errorCode) {
    if (thread == THREAD_MAIN){
        env->CallVoidMethod(instance,onErrorId,errorCode);
    } else{

        //子线程
        JNIEnv *env1;
        //获得属于我这一个线程的jnienv
        vm->AttachCurrentThread(&env1,0);
        env1->CallVoidMethod(instance,onErrorId,errorCode);
        vm->DetachCurrentThread();
    }
}

void JavaCallHelp::onPrepare(int thread) {
    if (thread == THREAD_MAIN){
        env->CallVoidMethod(instance,onPrepareId);
    } else{

        //子线程
        JNIEnv *env1;
        //获得属于我这一个线程的jnienv
        vm->AttachCurrentThread(&env1,0);
        env1->CallVoidMethod(instance,onPrepareId);
        vm->DetachCurrentThread();
    }
}
