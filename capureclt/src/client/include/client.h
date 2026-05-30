#ifndef SCCLIENT_H
#define SCCLIENT_H
#ifdef __cplusplus
extern "C" {
#endif
    #include "../../network/include/scudp.h"
    #include "../../decoder/include/scdecoder.h"
#ifdef __cplusplus
}
#endif
#define SCPACAKGE_SIZE_LIM (1024*804 + 64)
#define SCPACAKGE_COUNT_LIM 7
enum CLIENT_STATE{
    CLIENT_STATE_UNINITIALIZED,
    CLIENT_STATE_IDLE,
    CLIENT_STATE_RECONNECTING,
    CLIENT_STATE_OPERATING
};
struct frame_buffer{
    uint8_t* bf_data[SCPACAKGE_COUNT_LIM];
    uint32_t bf_sz[SCPACAKGE_COUNT_LIM];
    uint8_t index_in;
    uint8_t index_out;
    // int count;
    int fps;
};

typedef struct
{
    udpcontext Us;//sender UDP
    udpcontext Ur;//recevier UDP
    VideoDecoder V;
    void* thread_pool[2];
    struct frame_buffer pkg;
    void (*update_frame)(struct frameif*);
    int state;
} ScClient;
           
int scclient_init(ScClient*,void (*update_frame)(struct frameif* info));

int scclient_start(ScClient*);//start threads

int scclient_stop(ScClient*);//stop threads

int sclient_deinit(ScClient* clt);

#endif