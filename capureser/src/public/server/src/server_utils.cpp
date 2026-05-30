#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #define sem_t                HANDLE
  #define sem_init(sem, pshared, val)  (*(sem) = CreateSemaphoreA(NULL, val, val, NULL))
  #define sem_wait(sem)        WaitForSingleObject(*(sem), INFINITE)
  #define sem_post(sem)        ReleaseSemaphore(*(sem), 1, NULL)
  #define sem_destroy(sem)     CloseHandle(*(sem))

  static inline int gettimeofday(struct timeval *tv, void *tz) {
      FILETIME ft;
      ULARGE_INTEGER li;
      GetSystemTimeAsFileTime(&ft);
      li.LowPart  = ft.dwLowDateTime;
      li.HighPart = ft.dwHighDateTime;
      // 1601-01-01 to 1970-01-01 offset in 100ns intervals
      li.QuadPart -= 116444736000000000ULL;
      tv->tv_sec  = (long)(li.QuadPart / 10000000ULL);
      tv->tv_usec = (long)(li.QuadPart % 10000000ULL / 10ULL);
      return 0;
  }
#else
  #include <sys/time.h>
  #include <semaphore.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "./server.h"

static sem_t sem_dequeue;
static sem_t sem_inqueue;

void server_utils_stop(){
    sem_post(&sem_inqueue);
    sem_wait(&sem_dequeue);
    sem_destroy(&sem_dequeue);
    sem_destroy(&sem_inqueue);
    return;
}

void cbk_ppw_1(void* this_){
    sem_wait(&sem_dequeue);
    
    sccap_context* ctx=(sccap_context*)this_;
    udpcontext* sender=(udpcontext*)ctx->callback_param[1];
    struct frame_buffer* buffer=(struct frame_buffer*)ctx->callback_param[0];
    if(sender->state!=CONNECTED){
        if(sccap_stream_get(ctx)==1)sccap_stream_toggle(ctx,0);
        return;
    }
    
    memcpy(buffer->frames[buffer->index_in],(uint8_t*)(ctx->info.data),ctx->info.size);
    buffer->stride[buffer->index_in]=ctx->info.stride;
    buffer->index_in=(buffer->index_in+1)%RAW_FRAME_QUEUE;
    
    sem_post(&sem_inqueue);
    return;
}

void outqueue_loop(udpcontext* sender){
    sem_init(&sem_dequeue, 0, RAW_FRAME_QUEUE);
    sem_init(&sem_inqueue, 0, 0);

    struct frame_buffer* buffer=(struct frame_buffer*)sender->callback_param[0];
    udpcontext* ts=(udpcontext*)sender->callback_param[1];
    VideoEncoder* enc=(VideoEncoder*)sender->callback_param[2];
    int counter=0;
    int64_t last_stamp=0;
    uint8_t* h246buffer=(uint8_t*)malloc(UDP_PKG_SIZE_LIM);

    while (ts->state==CONNECTED)
    {
        sem_wait(&sem_inqueue);
        
        uint32_t sz=encoder_encode(enc,buffer->frames[buffer->index_out],buffer->stride[buffer->index_out],&h246buffer);
        if(sz>1&&udp_send(sender,UDPDATA_FRAME,sz,h246buffer)){
            counter++;
            if(counter==130){
                struct timeval tv;
                gettimeofday(&tv,NULL);
                int64_t current_stamp=tv.tv_sec * 1000 + tv.tv_usec / 1000;
                float fps=130.0f*1000.0f/(current_stamp-last_stamp);
                printf("ppw_outqueue(): current fps : %f\n",fps);
                counter=0;
                last_stamp=current_stamp;
            }
        }
        else printf("ppw_outqueue(): failed to send, check whether client is Synchronized\n");
        buffer->index_out=(buffer->index_out+1)%RAW_FRAME_QUEUE;
        
        sem_post(&sem_dequeue);
    }
    free(h246buffer);

}

void cbk_ppw_2(void* this_){//pipewire parameter change callback
    printf("ppw_param_change():Trying to Initiate encoder..\n");
    sccap_context* ctx=(sccap_context*)this_;
    VideoEncoder* enc=(VideoEncoder*)ctx->callback_param[2];
    encoder_init(enc,ctx->info.width,ctx->info.height,ctx->info.format);
    return;
};

void udpr_handle(void* this_){
    udpcontext* ts=(udpcontext*)this_;
    sccap_context* P=(sccap_context*)ts->callback_param[0];
    udpcontext* Us=(udpcontext*)ts->callback_param[1];
    if(ts->p_type==UDPDATA_CONNECT){
        if(udp_connect(Us,ts->target,0,NULL))Us->state=CONNECTED;
        sccap_stream_toggle(P,0);
        return;
    }else
    if(Us->state==CONNECTED&&addr_check(Us->target,ts->target))
    switch (ts->p_type)
    {
        
    case UDPDATA_DISCONNECT:
        udp_send(Us,UDPDATA_DISCONNECT,0,NULL);   
        Us->state=DISCONNECTED;
        sccap_stream_toggle(P,0);
        sem_post(&sem_inqueue);
        printf("udp_recv_loop(): Client Request disconnecting!\n");
        break;
    case UDPDATA_TOGGLE_STREAM:
        {        
            int i =sccap_stream_get(P);
            sccap_stream_toggle(P,i==1?0:1);
        }
        break;
    case UDPDATA_CLIENT_REQUEST_CONFIG:
        {
            uint8_t bf[30]={0};
            int i =0;
            bf[i++]=P->info.height>>8;
            bf[i++]=P->info.height&0xff;
            bf[i++]=P->info.width>>8;
            bf[i++]=P->info.width&0xff;
            bf[i++]=P->info.format;
            udp_send(Us,UDPDATA_SERVER_SEND_CONFIG,i,bf);
            printf("udpr_handle(): sending configuration to client\n");
        }
        break;

    case UDPDATA_TOUCH:
        {
            scinput_context* I=(scinput_context*)ts->callback_param[2];
            int x=ts->p[0][0]<<8|ts->p[0][1]&0xff;
            int y=ts->p[0][2]<<8|ts->p[0][3]&0xff;
            int is_down=ts->p[0][4];
            scinput_input(I,x,y,is_down);
            scinput_sync(I);
            //printf("%d %d %d\n",x,y,is_down);
        }    
        break;
    default:
        break;
    }
}
