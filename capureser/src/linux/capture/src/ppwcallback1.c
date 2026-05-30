#include <pipewire/pipewire.h>
#include <spa/param/video/format-utils.h>
#include <stdint.h>
#include "../include/sccap.h"
struct PwCaptureContext
{
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_stream *stream;
    struct spa_hook *sl;
    struct pw_properties *pwp;
};

void stream_process(void *data)
{

    sccap_context *ctx = (sccap_context *)data;
    struct pw_buffer *b=pw_stream_dequeue_buffer(ctx->pwcap->stream);
    if (!b)
    {
        printf("stream_process(): cant dequeue a buffer\n");
        return;
    };
    struct spa_buffer *buf = b->buffer;
    if (buf->datas[0].data &&buf->n_datas == 1)
    { // BGRx RGBx RGBA
        ctx->info.stride = buf->datas[0].chunk->stride;
        ctx->info.size = buf->datas[0].chunk->size;
        ctx->info.data = buf->datas[0].data;
        ctx->call_stream((void*)ctx);// here will hanlde encoding and sending
    }else printf("stream_process(): n_datas != 1 , skipped\n");
     
    pw_stream_queue_buffer(ctx->pwcap->stream, b);
}
   
void param_changed(void *data, uint32_t id, const struct spa_pod *param)
{
    printf("param_changed(): with%s spa_pod ,enum ID : %d", param == NULL ? "out" : "", id);
    sccap_context *ctx = (sccap_context *)data;
    switch (id)
    {
    case SPA_PARAM_Props:
    {
        printf("-> SPA_PARAM_Props\n");
        break;
    }
    case SPA_PARAM_EnumFormat:
    {
        printf("-> SPA_PARAM_EnumFormat\n");
        break;
    }
    case SPA_PARAM_Format:
    {
        printf("-> SPA_PARAM_Format\n");
        struct spa_video_info_raw fmt = {0};
        if (!param)
            return;
        if (spa_format_video_raw_parse(param, &fmt) < 0)
        {
            printf("param_changed(): Failed to parse spa_pod");
            return;
        };
        ctx->info.height = fmt.size.height;
        ctx->info.width = fmt.size.width;
        ctx->info.format = fmt.format;
        printf("param_changed(): ready for streaming , current configuration:\n\
                                            Width=%d\n\
                                            Height=%d\n\
                                            Format=%d\n\
                                            Framerate=<(%d,%d),(%d,%d)>\n",
               fmt.size.width, fmt.size.height, fmt.format, fmt.framerate.num, fmt.framerate.denom, fmt.max_framerate.num, fmt.max_framerate.denom);
        ctx->call_para((void *)ctx);
        break;
    }
    case SPA_PARAM_Latency:
    {
        
        printf("-> SPA_PARAM_Latency\n");
        break;
    }
    default:
    {
        printf("-> Unknown\n");
        break;
    }
    }
}

void state_changed(void *data,enum pw_stream_state old,enum pw_stream_state state, const char *error)
{
    static uint8_t i=0;
    PwCaptureContext *ctx = ((sccap_context *)data)->pwcap;
    printf("state_changed(): stream state change from %s to %s,Node ID: %u. %s\n",
           pw_stream_state_as_string(old),
           pw_stream_state_as_string(state),
           pw_stream_get_node_id(ctx->stream),
           state == PW_STREAM_STATE_ERROR ? error : "");
    return;
};
