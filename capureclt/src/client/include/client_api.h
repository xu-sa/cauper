#ifndef CLIENT_API_H
#define CLIENT_API_H
#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
//run at the starting up 
extern "C" JNIEXPORT jint JNICALL
Java_com_example_cauper_CauperNativeLib_initClient(JNIEnv* env,jobject);
//button On Main Activity
extern "C" JNIEXPORT jint JNICALL
Java_com_example_cauper_CauperNativeLib_connectClient(JNIEnv* env, jobject ,jstring ip,jint port);
//button on Main Activity
extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_deinitClient(JNIEnv* env,jobject);
//Surface View On Secondary(Video) Activity
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_cauper_CauperNativeLib_setNativeSurface(JNIEnv *env, jobject, jobject surface);
//Event On Sec
extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_touch(JNIEnv* env,jobject,jint x,jint y,jint is_down);
//Button on sec
extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_toggleStream(JNIEnv* env,jobject );
//button on sec
extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_stopClient(JNIEnv* env,jobject);
extern "C" JNIEXPORT void JNICALL 
Java_com_example_cauper_CauperNativeLib_checkState(JNIEnv* env,jobject);
#endif
