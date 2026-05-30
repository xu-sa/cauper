#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#define SLEEP(x) usleep(1000 * x) 
#include <sys/time.h>
#include "../include/client.h"
static sem_t sem_dequeue;
static sem_t sem_inqueue;
void client_utils_stop(){
    sem_post(&sem_inqueue);
    sem_wait(&sem_dequeue);
}
void client_utils_exit(){
    client_utils_stop();
    sem_destroy(&sem_dequeue);
    sem_destroy(&sem_inqueue);
}
void play_stream(ScClient* clt){//the main loop of streaming
    sem_init(&sem_dequeue, 0, SCPACAKGE_COUNT_LIM);
    sem_init(&sem_inqueue, 0, 0);
    static int counter=0;
    static int64_t last_stamp=0;
    {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        last_stamp=tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
    while(clt->Ur.state==CONNECTED){
        sem_wait(&sem_inqueue);
        
        if(decoder_decode(&clt->V,clt->pkg.bf_data[clt->pkg.index_out],(int)clt->pkg.bf_sz[clt->pkg.index_out])==1)clt->update_frame(&(clt->V.info));
        clt->pkg.index_out=(clt->pkg.index_out+1)%SCPACAKGE_COUNT_LIM;
        counter++;
        if(counter==150){//every 100 frames Records one fps
            struct timeval tv;
            gettimeofday(&tv,NULL);
            int64_t current_stamp=tv.tv_sec * 1000 + tv.tv_usec / 1000;
            clt->pkg.fps=100*1000/(current_stamp-last_stamp);
            printf("play_stream(): current fps : %d\n",clt->pkg.fps);
            counter=0;
            last_stamp=current_stamp;
        }

        sem_post(&sem_dequeue);

    }
}

void udpr_handle(void* this_){//udp receiver received a package, handle it here
    udpcontext* ctx=(udpcontext*)this_;
    struct frame_buffer* pkg=(struct frame_buffer*)ctx->callback_param[2];
    switch (ctx->p_type)
    {
    case UDPDATA_FRAME:
        {
            if(ctx->p_sz==0)return;
            sem_wait(&sem_dequeue);
          
            pkg->bf_sz[pkg->index_in]=ctx->p_sz;
            memset(pkg->bf_data[pkg->index_in] + pkg->bf_sz[pkg->index_in], 0, 64);//add metadata of avcodec chunk ending
            pkg->index_in=(pkg->index_in+1)%SCPACAKGE_COUNT_LIM;
            ctx->p=&(pkg->bf_data[pkg->index_in]);

            sem_post(&sem_inqueue);

            break;
        }   
    case UDPDATA_DISCONNECT:
        ctx->state=DISCONNECTED;
        printf("\nudp_recv_loop(): Server disconnected!\n");
        break;
    case UDPDATA_SERVER_SEND_CONFIG:
        {
            VideoDecoder* dec=(VideoDecoder*)ctx->callback_param[0];
            struct frameif* info=&(dec->info);
            dec->info.height= (pkg->bf_data[pkg->index_in][0]<<8)  |pkg->bf_data[pkg->index_in][1];
            dec->info.width = (pkg->bf_data[pkg->index_in][2]<<8)|pkg->bf_data[pkg->index_in][3];
            dec->info.size=dec->info.height*dec->info.width*4;
            dec->info.linesize[0]=dec->info.width*4;
            dec->info.data[0]=(uint8_t*)malloc(dec->info.size);
            memset(dec->info.data[0],0,dec->info.size);
            printf("udpr_handle(): got current server configuration : %d x %d, pipewire format :%d\n",
                dec->info.width,
                dec->info.height,
                pkg->bf_data[pkg->index_in][4]
            );
        }
        break;
    default:
        break;
    }
    
}