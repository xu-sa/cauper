#include <string>
#include <sstream>
#include "./client.h"
#include "./client_api.h"
#define LOG_TAG "CauperNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static ScClient g_client;
ANativeWindow* g_native_window = NULL;
static void update_frame(struct frameif* f) {
    if (!g_native_window) return;
    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(g_native_window, &buffer, NULL) < 0) return;
    int copy_height = (f->height < buffer.height) ? f->height : buffer.height;
    int bytes_per_pixel = 4;
    int copy_width_bytes = (f->width < buffer.width ? f->width : buffer.width) * bytes_per_pixel;

    int src_stride = f->linesize[0];
    int dst_stride = buffer.stride * bytes_per_pixel;

    uint8_t* dst_ptr = (uint8_t*)buffer.bits;
    uint8_t* src_ptr = (uint8_t*)f->data[0];

    for (int y = 0; y < copy_height; y++)memcpy(dst_ptr + y * dst_stride, src_ptr + y * src_stride, copy_width_bytes);
    ANativeWindow_unlockAndPost(g_native_window);
}
extern "C" JNIEXPORT jint JNICALL
Java_com_example_cauper_CauperNativeLib_initClient(JNIEnv* env,jobject) {
    LOGI("Initializing ScClient...");
    g_client.state = 0;
    g_client.Us.port = 9990;
    g_client.Ur.port = 9991;
    return scclient_init(&g_client,update_frame);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_cauper_CauperNativeLib_connectClient(JNIEnv* env, jobject ,jstring ip,jint port) {
    // if(g_client.state!=CLIENT_STATE_IDLE)return -2;
    if(g_client.state==CLIENT_STATE_OPERATING)scclient_stop(&g_client);
    const char *native_ip = env->GetStringUTFChars(ip, 0);
    LOGI("Connecting to %s:%d", native_ip, port);
    
    int result = 0;
    if (udp_connect(&g_client.Us, NULL, port, native_ip)) {
        result = scclient_start(&g_client);
    } else {
        LOGE("UDP Sync failed");
        result = -1;
    }

    env->ReleaseStringUTFChars(ip, native_ip);
    return result;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_deinitClient(JNIEnv* env,jobject) {
    LOGI("Deinitializing ScClient...");
    sclient_deinit(&g_client);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_cauper_CauperNativeLib_setNativeSurface(JNIEnv *env, jobject, jobject surface) {
    if (g_native_window) {
        ANativeWindow_release(g_native_window);
        g_native_window = NULL;
    }
    if(g_client.V.info.width==0||g_client.V.info.height==0)return env->NewStringUTF("0");
    
    if(surface==NULL)return env->NewStringUTF("Surface is null");
   
    g_native_window = ANativeWindow_fromSurface(env, surface);
   
    if(ANativeWindow_setBuffersGeometry(g_native_window, g_client.V.info.width, g_client.V.info.height, WINDOW_FORMAT_RGBA_8888)==0){

        std::string a = "Initialized window : "+std::to_string(g_client.V.info.width)+"x"+std::to_string(g_client.V.info.height);
        jstring jstr = env->NewStringUTF(a.c_str());
        return jstr;

    }
    return env->NewStringUTF("Failed to set buffers geometry for window");
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_touch(JNIEnv* env,jobject,jint x,jint y,jint is_down) {
    uint8_t bf[5];
    float ratio_x=x/1000.0f;
    float ratio_y=y/1000.0f;
    int X=g_client.V.info.width*ratio_x;
    int Y=g_client.V.info.height*ratio_y;
    bf[0]=X>>8;
    bf[1]=X&0xff;
    bf[2]=Y>>8;
    bf[3]=Y&0xff;
    bf[4]=is_down==1;
    udp_send(&g_client.Us,UDPDATA_TOUCH,5,bf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_toggleStream(JNIEnv* env,jobject ) {
    LOGI("Sending toggle command...");
    udp_send(&g_client.Us, UDPDATA_TOGGLE_STREAM, 0, NULL);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cauper_CauperNativeLib_stopClient(JNIEnv* env,jobject ) {
    LOGI("Stopping ScClient...");
    scclient_stop(&g_client);
}

extern "C" JNIEXPORT void JNICALL 
Java_com_example_cauper_CauperNativeLib_checkState(JNIEnv* env,jobject){
    std::stringstream ss;
    ss<<g_client.state;
    ss<<g_client.V.info.width;
    ss<<g_client.V.info.height;
    std::string a = ss.str();
    jstring jstr = env->NewStringUTF(a.c_str());
}