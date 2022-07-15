//
// Created by waitwalker on 2022/7/14.
//

#include <jni.h>
#include <stdlib.h>
#include <string>
#include <android/log.h>
#include "MP3Encoder.h"
#define TAG    "JNI_LOG"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

//定义Java文件的位置,注意是'/'分开
static const char *className = "cn/waitwalker/ffmpeg_flutter/JNI";

//返回一个string
jstring stringFromJNI(JNIEnv *env,jclass clz) {
    std::string hello = "Hello from C++";
    MP3Encoder *encoder = new MP3Encoder();
    encoder->encode();

    int value = encoder->getValue();
    LOGE("%s", "Say Hello and value"+value);
    return env->NewStringUTF(hello.c_str());
}

//返回一个string 并把这句话打印到logcat中去
jstring sayHello(JNIEnv* env,jclass clz) {
    std::string hello = "Say Hello from C++";
    LOGE("Say Hello and value");
    MP3Encoder *encoder = new MP3Encoder();
    encoder->encode();
    int value = encoder->getValue();
    LOGE("%s", "Say Hello and value"+value);
    return env->NewStringUTF(hello.c_str());
}

//获取所有方法名
static JNINativeMethod gJniMethods[] = {
        {"sayHello", "()Ljava/lang/String;", (void*)sayHello},
        {"stringFromJNI", "()Ljava/lang/String;", (void*)stringFromJNI},
};
//其中{"MethodName","(入参)返回值",()Method}

//动态注册本地方法
static int jniRegisterNativeMethods(JNIEnv* env, const char* className,
                                    const JNINativeMethod* gMethods, int numMethods) {
    jclass clazz;
    clazz = (env)->FindClass( className);
    if (clazz == NULL) {
        return -1;
    }

    int result = 0;
    if ((env)->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        result = -1;
    }

    (env)->DeleteLocalRef(clazz);
    return result;
}

//JNI_OnLoad回调
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){

    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jniRegisterNativeMethods(env, className, gJniMethods, sizeof(gJniMethods) / sizeof(JNINativeMethod));

    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif