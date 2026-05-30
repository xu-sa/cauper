#include <thread>
#include "./server.h"
#include <stdlib.h>
extern void cbk_ppw_2(void* this_);
extern void cbk_ppw_1(void* this_);
extern void outqueue_loop(udpcontext* sender);
extern void udpr_handle(void* this_);
extern void server_utils_stop();

void ScServer_init(ScServer* s){
    for(size_t i = 0; i < RAW_FRAME_QUEUE; i++)s->pkg.frames[i]=(uint8_t*)malloc(RAW_FRAME_SIZE_LIM);
    
    s->pkg.index_in=0;
    s->pkg.index_out=0;
    
    s->P.call_para=cbk_ppw_2;
    s->P.call_stream=cbk_ppw_1;
    s->P.callback_param[0]=(void*)&s->pkg;
    s->P.callback_param[1]=(void*)&s->Us;
    s->P.callback_param[2]=(void*)&s->V;//Customizable pointer
    
    s->Ur.port=SERVER_PORT+1;
    s->Ur.callback=udpr_handle;
    s->Ur.callback_param[0]=(void*)&s->P;
    s->Ur.callback_param[1]=(void*)&s->Us;
    s->Ur.callback_param[2]=(void*)&s->I;//Customizable pointer
    s->Ur.p=NULL;
    
    s->Us.port=SERVER_PORT;
    s->Us.callback=NULL;
    s->Us.callback_param[0]=(void*)&s->pkg;
    s->Us.callback_param[1]=(void*)&s->Ur;
    s->Us.callback_param[2]=(void*)&s->V;//Customizable pointer
    
    s->V.state=0;
    
    udp_init(&s->Us);
    udp_init(&s->Ur);
    #ifndef _WIN32
    portal_init(&s->P);
    #endif
    sccap_init(&s->P);
    scinput_init(&s->I,s->P.info.width,s->P.info.height,"cccc1");

}

void ScServer_run(ScServer* s){
    s->Ur.state=CONNECTED;
    s->Us.state=DISCONNECTED;
    s->thread_pool[2]=new std::thread(outqueue_loop,&s->Us);
    s->thread_pool[0]=new std::thread(udp_recv_loop,&s->Ur);
    s->thread_pool[1]=new std::thread(sccap_loop,&s->P);
}

void ScServer_stop(ScServer* s){
    sccap_stop(&s->P);
    #ifndef _WIN32
    portal_destroy(&s->P);
    #endif
    udp_close(&s->Ur);
    udp_close(&s->Us);
    scinput_close(&s->I);
    server_utils_stop();
    ((std::thread*)s->thread_pool[0])->join();
    ((std::thread*)s->thread_pool[1])->join();
    ((std::thread*)s->thread_pool[2])->join();

    delete (std::thread*)s->thread_pool[0];
    delete (std::thread*)s->thread_pool[1];
    delete (std::thread*)s->thread_pool[2];
    
    for(size_t i = 0; i < RAW_FRAME_QUEUE; i++)free(s->pkg.frames[i]);    
}
