#ifndef SCCAP_H
#define SCCAP_H
#include <stdint.h>
#define PPW_SAMPLE_FRAMERATE 80
#define STREAM_NAME "SC2ca4"

typedef struct PwCaptureContext PwCaptureContext; 
typedef struct PortalContext PortalContext;
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
    PwCaptureContext* pwcap;
    PortalContext* ppwportal;
    void (*call_stream)(void*);
    void (*call_para)(void*);
    struct frame_info info;
    int fd;
    uint32_t node_id;
    void* callback_param[3];
};

int sccap_init(sccap_context *ctx);
void sccap_loop(sccap_context *ctx);
void sccap_stop(sccap_context *ctx);
void sccap_stream_toggle(sccap_context *ctx,int state);
int sccap_stream_get(sccap_context* ctx);
int portal_init(sccap_context *ctx);
void portal_destroy(sccap_context *ctx);
#endif