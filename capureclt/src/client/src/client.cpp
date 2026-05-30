#include <thread>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/client.h"
#include <unistd.h>
#define SLEEP(x) usleep(1000 * x)
void client_utils_stop();
void client_utils_exit();
static void udps_handle(void* this_){ //udp sender would not handle package or recevied from any ip  
};
static void give_frame(struct frameif* info){//being applied as a debugger when a NULL is passed to the 2d parameter of scclient_init()
    static uint8_t i=0;
    printf("give_frame(): client received a frame from server %d: %d x %d , size : %d Bytes , stride : %d\n",i,info->width,info->height,info->size,info->linesize[0]);
    i=(i+1)%30;
    return;
}
void udpr_handle(void* this_);//callback function which handle package from UDP
void play_stream(ScClient* clt);//the main loop of streaming

int scclient_init(ScClient* clt,void (*update_frame)(struct frameif* info)){//0->1
    if(clt->state!=CLIENT_STATE_UNINITIALIZED){
        printf("scclient_init(): please do not attempt to Initialize a operating object\n");
        return 0;
    }
    if(update_frame==NULL){
        printf("scclient_init(): Warning: no callback specified, using the debugger function\n");
        clt->update_frame=give_frame;
    }
    else clt->update_frame=update_frame;
    clt->pkg.index_in=0;
    clt->pkg.index_out=0;
    for (size_t i = 0; i < SCPACAKGE_COUNT_LIM; i++)clt->pkg.bf_data[i]=(uint8_t*)malloc(SCPACAKGE_SIZE_LIM);
    clt->Ur.p=&(clt->pkg.bf_data[clt->pkg.index_in]);
    udp_init(&clt->Us);
    udp_init(&clt->Ur);
    int i = decoder_init(&clt->V);
    if(i!=0)return i;

    udp_set_timeout(&clt->Us,4000);
    
    clt->Us.callback=udps_handle;
    clt->Us.state=CONNECTED;
    
    clt->Ur.callback=udpr_handle;
    clt->Ur.callback_param[0]=(void*)&(clt->V);
    clt->Ur.callback_param[1]=(void*)&(clt->Us);
    clt->Ur.callback_param[2]=(void*)&(clt->pkg);
    
    clt->thread_pool[0]=NULL;
    clt->thread_pool[1]=NULL;
    clt->state=CLIENT_STATE_IDLE;    
    printf("scclient_init(): Initialized\n");
    return 1;
}

int scclient_start(ScClient* clt){//1->2
    if(clt->state!=CLIENT_STATE_IDLE){
        printf("scclient_start(): this object need to be idling Prior to starting , you may need to stop it first\n");
        return 2;
    }

    clt->pkg.index_in=0;
    clt->pkg.index_out=0;    
    clt->Ur.state=CONNECTED;
    clt->Ur.p=&(clt->pkg.bf_data[clt->pkg.index_in]);
    if(clt->thread_pool[0]!=NULL||clt->thread_pool[1]!=NULL){//this shall not happen as long as the API is called in a proper way
        ((std::thread*)clt->thread_pool[0])->join();
        ((std::thread*)clt->thread_pool[1])->join();
        delete (std::thread*)clt->thread_pool[0];
        delete (std::thread*)clt->thread_pool[1];
        clt->thread_pool[0]=NULL;
        clt->thread_pool[1]=NULL;
        SLEEP(100);
    }
    clt->thread_pool[0]=(void*) new std::thread(udp_recv_loop,&clt->Ur);
    clt->thread_pool[1]=(void*) new std::thread(play_stream,clt);
    for (size_t i = 0; i < 5; i++)
    {
        udp_send(&clt->Us,UDPDATA_CLIENT_REQUEST_CONFIG,0,NULL);
        SLEEP(2000);
        if(clt->V.info.width!=0&&clt->V.info.height!=0)break;
    }
    clt->state=CLIENT_STATE_OPERATING;
    if(clt->V.info.width==0||clt->V.info.height==0){
        printf("scclient_start(): Failed to fetch data\n");
        return 0;
    }else{
        printf("scclient_start(): Operating..\n");
        return 1;
    }
};

int scclient_stop(ScClient* clt){//2->1
    if(clt->state!=CLIENT_STATE_OPERATING){
        printf("scclient_stop(): cant stop a non-operating object\n");
        return 0;
    }

    if(clt->Ur.state==CONNECTED){//sometimes the client disconnect due to server closing connection, in which case the state wont be CONNECTED
        udp_send(&clt->Us,UDPDATA_DISCONNECT,0,NULL);
        for(int i =0;i<10;i++){
            SLEEP(400);
            if(clt->Ur.state==DISCONNECTED)break;
            if(i==9)udp_disconnect(&clt->Us,&clt->Ur);//force to stop client receiver thread 
        }
    }   
    client_utils_stop();
    ((std::thread*)clt->thread_pool[0])->join();
    ((std::thread*)clt->thread_pool[1])->join();
    
    delete (std::thread*)clt->thread_pool[0];
    delete (std::thread*)clt->thread_pool[1];
    clt->thread_pool[0]=NULL;
    clt->thread_pool[1]=NULL;
    
    free(clt->V.info.data[0]);
    clt->V.info.data[0]=NULL;
    clt->V.info.size=0;
    clt->V.info.width=0;
    clt->V.info.height=0;

    clt->pkg.index_in=0;
    clt->pkg.index_out=0;
    for(int i=0;i<SCPACAKGE_COUNT_LIM;i++)memset(clt->pkg.bf_data[i],0,SCPACAKGE_SIZE_LIM);

    clt->state=CLIENT_STATE_IDLE;

    printf("scclient_stop(): Stopped\n");
    return 1;
}   

int sclient_deinit(ScClient* clt){//1->0
    if(clt->state!=CLIENT_STATE_IDLE){
        printf("sclient_deinit(): This object need to be idling Prior to deinitializing , you may to stop or initialize it first\n");
        return 0;
    }
    
    udp_close(&clt->Ur);
    udp_close(&clt->Us);
    
    decoder_close(&clt->V);
    
    for (size_t i = 0; i < SCPACAKGE_COUNT_LIM; i++)
    {
        free(clt->pkg.bf_data[i]);
        clt->pkg.bf_data[i]=NULL;
    }
    void client_utils_exit();
    // clt->pkg=NULL;
    clt->state=CLIENT_STATE_UNINITIALIZED;
    printf("sclient_deinit(): Deinitialized\n");
    return 1;
}