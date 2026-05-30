#ifndef SCCAP_H
#define SCCAP_H
#include <stdint.h>
#define STREAM_NAME "SC2ca4"
typedef struct frame_info frame_info;
typedef struct sccap_context sccap_context;

struct frame_info{
    uint16_t width;
    uint16_t height;
    uint16_t format;
    int32_t stride;
    uint32_t size;
    void* data;
};

struct sccap_context
{
    void (*call_stream)(void*);
    void (*call_para)(void*);
    struct frame_info info;
    int fd;
    void* callback_param[3];
};

int sccap_init(sccap_context *ctx);
void sccap_loop(sccap_context *ctx);
void sccap_stop(sccap_context *ctx);
void sccap_stream_toggle(sccap_context *ctx,int state);
int sccap_stream_get(sccap_context* ctx);
#endif