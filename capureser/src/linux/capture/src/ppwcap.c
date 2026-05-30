#include <stdint.h>
#include <pipewire/pipewire.h>
#include <spa/param/video/format-utils.h>
#include "../include/sccap.h"

void stream_process(void *data);
void param_changed(void *data, uint32_t id, const struct spa_pod *param);
void state_changed(void *data, enum pw_stream_state old,enum pw_stream_state state, const char *error);

struct PwCaptureContext{
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_stream *stream;
    struct spa_hook* sl;
    struct pw_properties* pwp;
} ;
static const struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .process = stream_process,//callback1
    .param_changed=param_changed,//callback2
    .state_changed=state_changed//callback3
};

static uint8_t counter_=0;

static uint8_t buffer[1024];
    
inline static void build_pod(sccap_context* ctx,const struct spa_pod** pod){  
    counter_=0;memset(buffer,0,sizeof(buffer));
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    pod[counter_++]=spa_pod_builder_add_object(&b,
        SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
        SPA_FORMAT_mediaType,            SPA_POD_Id(SPA_MEDIA_TYPE_video),
        SPA_FORMAT_mediaSubtype,         SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
        SPA_FORMAT_VIDEO_framerate,      SPA_POD_Fraction(&SPA_FRACTION(0, 1)),
        SPA_FORMAT_VIDEO_maxFramerate,   SPA_POD_CHOICE_RANGE_Fraction(&SPA_FRACTION(PPW_SAMPLE_FRAMERATE, 1),&SPA_FRACTION(1, 1),&SPA_FRACTION(PPW_SAMPLE_FRAMERATE, 1)),
        SPA_FORMAT_VIDEO_size,           SPA_POD_CHOICE_RANGE_Rectangle(&SPA_RECTANGLE(ctx->info.width, ctx->info.height),&SPA_RECTANGLE(ctx->info.width, ctx->info.height),&SPA_RECTANGLE(ctx->info.width, ctx->info.height))
    );
    pod[counter_++] = spa_pod_builder_add_object(&b,
        SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
        SPA_PARAM_BUFFERS_buffers, SPA_POD_CHOICE_RANGE_Int(8, 4, 32),
        SPA_PARAM_BUFFERS_blocks,  SPA_POD_Int(1),
        SPA_PARAM_BUFFERS_size,    SPA_POD_Int(ctx->info.width * ctx->info.height * 4),
        SPA_PARAM_BUFFERS_dataType, SPA_POD_CHOICE_FLAGS_Int((1<<SPA_DATA_MemPtr)|(1<<SPA_DATA_DmaBuf))
    );
}

inline static void build_property(sccap_context* c_,PwCaptureContext* ctx){
    char id[16]={0}; 
    snprintf(id, 16, "%u", c_->node_id);
    ctx->pwp=pw_properties_new(
            PW_KEY_MEDIA_TYPE, "Video",
            PW_KEY_MEDIA_CATEGORY, "Capture",
            PW_KEY_MEDIA_ROLE, "Screen",
            "stream.dont-remap", "true",
            "node.always-process", "true",
            PW_KEY_MEDIA_CLASS, "Stream/Input/Video",
            "object.serial", id,
            NULL
    );
}

int sccap_init(sccap_context *c_) {
    {
        if(c_->node_id==0){
            fprintf(stderr,"Refuse to Initiate pipewire context obj which doesnt own a proper node id\n");
            return 0;
        }
        if(c_->fd<=0)printf("Warning, this pipewire context obj has a invaild fd\n");
    }
    
    PwCaptureContext* ctx=(PwCaptureContext*)malloc(sizeof(PwCaptureContext));
    ctx->sl=(struct spa_hook*)malloc(sizeof(struct spa_hook));
    c_->pwcap=ctx;
    const struct spa_pod* pod1[2];    
    pw_init(NULL, NULL);
    build_property(c_,ctx);
    build_pod(c_,pod1);
    ctx->loop = pw_main_loop_new(NULL);
    ctx->context = pw_context_new(pw_main_loop_get_loop(ctx->loop), NULL, 0);
    if(!(ctx->core=c_->fd>0?pw_context_connect_fd(ctx->context,c_->fd,NULL,0):pw_context_connect(ctx->context,NULL,0))){
        printf("Cant assign a new Pipewire core obj to this pipewire context obj,exiting..\n");
        return 0;
    };
    ctx->stream=pw_stream_new(ctx->core,STREAM_NAME,ctx->pwp);
    pw_stream_add_listener(ctx->stream,ctx->sl,&stream_events,c_);
    if (pw_stream_connect(ctx->stream,
                    PW_DIRECTION_INPUT,
                    c_->node_id,
                    PW_STREAM_FLAG_AUTOCONNECT|PW_STREAM_FLAG_MAP_BUFFERS,
                    pod1,counter_)< 0) 
    {
        fprintf(stderr, "Cant connect to PipeWire stream\n");
        return 0;
    }
    // pw_stream_set_active(ctx->stream, false);

    return 1;
}

void sccap_loop(sccap_context *c_) {
    PwCaptureContext* ctx=c_->pwcap;
    if (!ctx || !ctx->loop) return;
    printf("Starting pipewire loop..\n");
    pw_main_loop_run(ctx->loop);
    if (ctx->stream) {
        pw_stream_destroy(ctx->stream);
        ctx->stream = NULL;
    }
    if (ctx->core) {
        pw_core_disconnect(ctx->core);
        ctx->core = NULL;
    }
 
    if (ctx->context) {
        pw_context_destroy(ctx->context);
        ctx->context = NULL;
    }
    if (ctx->loop) {
        pw_main_loop_destroy(ctx->loop);
        ctx->loop = NULL;
    }
    free(ctx->sl);
    free(ctx);
    printf("PipeWire context obj Released\n");

}
int sccap_stream_get(sccap_context* ctx){
    int g= pw_stream_get_state(ctx->pwcap->stream, NULL);
    switch (g) {
    case PW_STREAM_STATE_STREAMING:return 1;
        break;
    case PW_STREAM_STATE_PAUSED:return 0;
        // pw_stream_set_active(ctx->stream, true); 
    case PW_STREAM_STATE_ERROR:return -1;
        // printf("pwcap_toggle(): failed: stream in error state: %s\n", error ? error : "");
    default:return -1;
        // printf("pwcap_toggle(): failed: stream not ready (state=%d)\n", state);
    }
}
void sccap_stream_toggle(sccap_context *c_,int state){
    switch (state)
    {
    case 1:
        
        pw_stream_set_active(c_->pwcap->stream, true);
        printf("pwcap_stream_toggle(): PAUSED -> STREAMING\n");
        break;
    case 0:

        pw_stream_set_active(c_->pwcap->stream, false);
        printf("pwcap_stream_toggle(): STREAMING -> PAUSED\n");
        break;
    default:
        printf("pwcap_stream_toggle(): Invaild state\n");
        break;
    }
}

void sccap_stop(sccap_context *c_) {
    PwCaptureContext* ctx=c_->pwcap;
    if (!ctx) return;
    if (ctx->loop) {
        pw_main_loop_quit(ctx->loop);
    }   
    printf("PipeWire stream exiting signal sent..\n");

}