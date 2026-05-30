#include "../include/scudp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>  
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#define SHUT_RD SD_RECEIVE
#define WSA_GUARD if(!ensure_wsa()) return 0;
#define CLOSE_SOCKET closesocket
#define SHUTDOWN_SOCKET(sock, how) shutdown(sock, how)
#define SENDTO(fd,buf,size,flag,addr,len) sendto(fd,(const char*)buf,size,flag,addr,len)
#define RECVFROM(fd,buf,size,flag,addr,len) recvfrom(fd,(char*)buf,size,flag,addr,len)
static int wsa_init_done = 0;
static int ensure_wsa() {
    if (!wsa_init_done) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            printf("WSAStartup failed\n");
            return 0;
        }
        wsa_init_done = 1;
    }
    return 1;
}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h> 
#define WSA_GUARD ;
#define CLOSE_SOCKET close
#define SHUTDOWN_SOCKET(sock, how) shutdown(sock, how)
#define SENDTO(fd,buf,size,flag,addr,len) sendto(fd,buf,size,flag,addr,len)
#define RECVFROM(fd,buf,size,flag,addr,len) recvfrom(fd,buf,size,flag,addr,len)
#endif
 
// #define BUF_SIZE 1024*700

struct udpsocket{
    int fd;
    uint8_t buf[UDP_CHUNK_SIZE_LIM+30];
    uint16_t bufsz;
};


void udp_recv_loop(udpcontext* V) {
    uint8_t* p=NULL;
    if(!V->p){
        p=(uint8_t*)malloc(UDP_PKG_SIZE_LIM);
        V->p=&p;
        printf("udp_recv_loop(): receiver buffer Not specified, allocating default buffer\n");
        memset(*V->p,0,UDP_PKG_SIZE_LIM);
    }
    V->p_sz=0;
    V->p_type=0;
    socklen_t len=sizeof(*(V->target));
    int pass=0;
    int chunk_Num=0;
    int chunk_Now=0;
    while (V->state==CONNECTED)
    {
        ssize_t s =RECVFROM(V->socketcf->fd,V->socketcf->buf, UDP_CHUNK_SIZE_LIM+10, 0, (struct sockaddr *)(V->target), &len);
        ssize_t N=(ssize_t)(V->socketcf->buf[UDPPK_CHUNK_SIZE_1]<<8)|V->socketcf->buf[UDPPK_CHUNK_SIZE_2];
        int cc=(V->socketcf->buf[UDPPK_CHUNK_COUNT_1]<<8)| V->socketcf->buf[UDPPK_CHUNK_COUNT_2];
        if(s!=N||(chunk_Num!=0&&cc!=chunk_Num)){//incorrect package
            printf("udp_recv(): skipping a False package..%d %d\n",(int)s,(int)N);
            V->p_sz=0;
            pass=2;
        }
        if(V->p_sz+N-UDPPK_DATA_START>=UDP_PKG_SIZE_LIM){//package buffer Overflowing
            printf("udp_recv(): Exceed package size limit..\n");
            pass=1;
            continue;
        }
        chunk_Now=(V->socketcf->buf[UDPPK_CHUNK_THIS_1]<<8) | V->socketcf->buf[UDPPK_CHUNK_THIS_2];
        chunk_Num=(V->socketcf->buf[UDPPK_CHUNK_COUNT_1]<<8)| V->socketcf->buf[UDPPK_CHUNK_COUNT_2];
        
        if(s>0&&pass==0){//write to buffer
            memcpy(*(V->p)+V->p_sz,V->socketcf->buf+UDPPK_DATA_START,s-UDPPK_DATA_START);
            V->p_sz+=N-UDPPK_DATA_START;
        }
        if(chunk_Now==chunk_Num){//finial chunk received,Handle package here
            V->p_type=V->socketcf->buf[UDPPK_TYPE];
            if(pass!=1)V->callback((void*)V);
            pass=0;
            V->p_sz=0;
            chunk_Now=0;
            chunk_Num=0;
        } 
    }
    if(p)free(p);
    V->p=NULL;
    printf("udp_recv_loop(): UDP recevier stopped listening\n");
    return;
}

int udp_init(udpcontext* V){
  
    WSA_GUARD
 

    if(V->port==0){
        printf("udp_init():Refuse to Initialize a incompleted upd context\n");
        return 0;
    } 
    V->socketcf=(struct udpsocket*)malloc(sizeof(struct udpsocket));
    V->target=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    V->target->sin_port=1;    
    struct udpsocket* v=V->socketcf;
    v->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (v->fd < 0) {
        printf("socket failed to create\n");
        return 0;
    }
    struct sockaddr_in info={
        .sin_family=AF_INET,
        .sin_addr.s_addr=INADDR_ANY,
        .sin_port=htons(V->port)
    };
    if (bind(v->fd, (const struct sockaddr *)&info, sizeof(info)) < 0) {
        printf("udp_init():UDP socket Binding failed\n");
        return 0;
    }
    V->p_sz=0;
    V->p_type=0;
    printf("udp_init():UDP socket listening on: %d\n", V->port);
    V->state=DISCONNECTED;
    return 1;
}

int udp_connect(udpcontext* V,struct sockaddr_in* socketcf,int server_port,const char* server_ip){
    if(socketcf){//for server to locate the client
        V->socketcf->buf[UDPPK_TYPE]=UDPDATA_CONNECT;
        V->socketcf->bufsz=UDPPK_DATA_START;
        if(addr_check(V->target,socketcf)){
            SENDTO(V->socketcf->fd, V->socketcf->buf,V->socketcf->bufsz, 0, (struct sockaddr * )socketcf, sizeof(*(socketcf)));
            return 1;
        }
        if(V->target->sin_port!=1)udp_send(V,UDPDATA_DISCONNECT,0,NULL);
        memcpy(V->target,socketcf,sizeof(*(socketcf)));
        V->target->sin_port = htons(ntohs(socketcf->sin_port)+1);
        V->socketcf->buf[UDPPK_CHUNK_COUNT_1]=0;
        V->socketcf->buf[UDPPK_CHUNK_COUNT_2]=1;
        V->socketcf->buf[UDPPK_CHUNK_THIS_1]=0;
        V->socketcf->buf[UDPPK_CHUNK_THIS_2]=1;
        V->socketcf->buf[UDPPK_CHUNK_SIZE_1]=0;
        V->socketcf->buf[UDPPK_CHUNK_SIZE_2]=UDPPK_DATA_START;
        V->socketcf->buf[UDPPK_TYPE]=UDPDATA_CONNECT;
        V->socketcf->bufsz=UDPPK_DATA_START;
        if(SENDTO(V->socketcf->fd, V->socketcf->buf,V->socketcf->bufsz, 0, (struct sockaddr * )socketcf, sizeof(*(socketcf)))>0)
        {
            printf("udp_connect(): client Synchronized ,IP: %s:%d\n",inet_ntoa(V->target->sin_addr),ntohs(V->target->sin_port));
            return 1;
        }
        printf("udp_connect(): Failed to Synchronize ,IP: %s:%d\n",inet_ntoa(V->target->sin_addr),ntohs(V->target->sin_port));
    }
    else{//for client to connect the server 
        V->target->sin_family=AF_INET;
        V->target->sin_port=htons(server_port);
        inet_pton(AF_INET,server_ip,&(V->target->sin_addr));
        V->socketcf->buf[UDPPK_TYPE]=0;
        printf("udp_connect(): connecting server at %s:%d ...\n ",server_ip,server_port);
        udp_send(V,UDPDATA_CONNECT,0,NULL);//request 
        V->socketcf->buf[UDPPK_TYPE]=0;
        RECVFROM(V->socketcf->fd,V->socketcf->buf,UDP_CHUNK_SIZE_LIM+10, 0, (struct sockaddr *)(V->target), NULL);
        if(V->socketcf->buf[UDPPK_TYPE]==UDPDATA_CONNECT){
            printf("udp_connect(): Server connected\n");
            return 1;
        }   
        printf("udp_connect(): Failed to connect to server: code %d\n",V->socketcf->buf[UDPPK_TYPE]);
    }
    return 0;
}
void udp_disconnect(udpcontext* sender,udpcontext* receiver){
    sender->socketcf->buf[UDPPK_TYPE]=UDPDATA_DISCONNECT;
    sender->socketcf->buf[UDPPK_CHUNK_COUNT_1]=0;
    sender->socketcf->buf[UDPPK_CHUNK_COUNT_2]=1;
    sender->socketcf->buf[UDPPK_CHUNK_THIS_1]=0;
    sender->socketcf->buf[UDPPK_CHUNK_THIS_2]=1;
    sender->socketcf->buf[UDPPK_CHUNK_SIZE_1]=0;
    sender->socketcf->buf[UDPPK_CHUNK_SIZE_2]=UDPPK_DATA_START;
    sender->socketcf->bufsz=UDPPK_DATA_START;
    struct sockaddr_in receiver_addr;
    receiver_addr.sin_family=AF_INET;
    receiver_addr.sin_port=htons(receiver->port);
    inet_pton(AF_INET,"127.0.0.1",&(receiver_addr.sin_addr));
    socklen_t len=sizeof(receiver_addr);
    SENDTO(sender->socketcf->fd, sender->socketcf->buf,sender->socketcf->bufsz, 0, (struct sockaddr *)&receiver_addr,len);
    
}
int addr_check(struct sockaddr_in* b,struct sockaddr_in* a){
    return ntohs(b->sin_port)==(ntohs(a->sin_port)+1)&&
            b->sin_addr.s_addr==a->sin_addr.s_addr&&
            b->sin_family==a->sin_family;
}
int udp_send(udpcontext* V,uint8_t tp,uint32_t sz,uint8_t* data){
    if(V->state==DISCONNECTED){
        printf("udp_send(): DISCONNECTED\n");
        return 0;
    }
    struct udpsocket* v=V->socketcf;
    socklen_t len=sizeof(*(V->target));
    uint16_t chunk_count=(sz+UDP_CHUNK_SIZE_LIM-1)/UDP_CHUNK_SIZE_LIM;
    v->buf[UDPPK_TYPE]=tp;
    v->buf[UDPPK_CHUNK_COUNT_1]=chunk_count>>8;
    v->buf[UDPPK_CHUNK_COUNT_2]=chunk_count&0xff;
    uint32_t offset=0;
    if(sz==0||!data){//command without data chunks 
        v->bufsz=7;
        v->buf[UDPPK_CHUNK_COUNT_2]=1;
        v->buf[UDPPK_CHUNK_THIS_1]=0;
        v->buf[UDPPK_CHUNK_THIS_2]=1;
        v->buf[UDPPK_CHUNK_SIZE_1]=0;
        v->buf[UDPPK_CHUNK_SIZE_2]=UDPPK_DATA_START;
        SENDTO(v->fd,v->buf,v->bufsz, 0, (struct sockaddr *)(V->target), len);
    }else for (uint16_t i = 1; i <= chunk_count; i++)//command /data with chunks of bytes
    {
        offset = (i-1) * UDP_CHUNK_SIZE_LIM;
        v->bufsz =UDP_CHUNK_SIZE_LIM<sz-offset?UDP_CHUNK_SIZE_LIM+UDPPK_DATA_START:sz-offset+UDPPK_DATA_START;
        v->buf[UDPPK_CHUNK_SIZE_1]=v->bufsz>>8;
        v->buf[UDPPK_CHUNK_SIZE_2]=v->bufsz&0xff;
        v->buf[UDPPK_CHUNK_THIS_1]=i>>8;
        v->buf[UDPPK_CHUNK_THIS_2]=i&0xff; 
        memcpy(v->buf+UDPPK_DATA_START,data+offset,v->bufsz-UDPPK_DATA_START);
        SENDTO(v->fd,v->buf,v->bufsz, 0, (struct sockaddr *)(V->target), len);
    }
    return 1;
}

void udp_close(udpcontext *v) {
    if(v->socketcf&&v->socketcf->fd >= 0){
        v->state=!DISCONNECTED;
        SHUTDOWN_SOCKET(v->socketcf->fd, SHUT_RD);
        CLOSE_SOCKET(v->socketcf->fd);
        v->socketcf->fd = -1;
        free(v->target);
        v->target=NULL;
        v->state=DISCONNECTED;
        printf("udp_close(): UDP port closed\n");
    }
    #ifdef _WIN32
    WSACleanup();
    #endif
}

void udp_set_timeout(udpcontext* V,int Miliseconds){
    struct timeval tv={Miliseconds/1000,0};
    setsockopt(V->socketcf->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}