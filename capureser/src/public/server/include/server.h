#ifndef SERVER_H
#define SERVER_H
#define SERVER_PORT 9970

#define RAW_FRAME_SIZE_LIM 2560*1440*4.1

#define RAW_FRAME_QUEUE 7
#ifdef _WIN32
extern "C"{
    #include "../../network/include/scudp.h"
    #include "../../../windows/capture/include/sccap.h"
    #include "../../../windows/input/include/scinput.h"
    #include "../../../windows/encoder/include/scencoder.h"
}
#else
extern "C"{
    #include "../../network/include/scudp.h"
    #include "../../../linux/capture/include/sccap.h"
    #include "../../../linux/input/include/scinput.h"
    #include "../../../linux/encoder/include/scencoder.h"
}
#endif
struct frame_buffer{
    uint8_t* frames[RAW_FRAME_QUEUE];
    int stride[RAW_FRAME_QUEUE];
    uint8_t index_in;
    uint8_t index_out;
};
typedef struct 
{
    sccap_context P;
    VideoEncoder V;
    scinput_context I;
    udpcontext Us;//sender
    udpcontext Ur;//receiver
    void* thread_pool[3];//thread 0 is for receiver loop,  thread 1 is for pipewire main loop ,thread 2 is for outqueue loop
    struct frame_buffer pkg;
}ScServer;

void ScServer_init(ScServer* s);
void ScServer_run(ScServer* s);
void ScServer_stop(ScServer* s);
#endif