#ifndef SCUDP_H
#define SCUDP_H
#include <stdint.h>
#define UDP_CHUNK_SIZE_LIM 1440
#define UDP_PKG_SIZE_LIM 1024*800
// #define UDP_PACKAGE_COUNT_LIM 30 
enum UDP_STATE{
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

enum UDPPK{
    UDPPK_CHUNK_SIZE_1,
    UDPPK_TYPE,//high bits of the chunk count
    UDPPK_CHUNK_SIZE_2,//low bits of the chunk count
    UDPPK_CHUNK_COUNT_1,
    UDPPK_CHUNK_COUNT_2,
    UDPPK_CHUNK_THIS_1,
    UDPPK_CHUNK_THIS_2,
    UDPPK_DATA_START
};

enum UDPDATA{
    UDPDATA_FRAME,//data
    UDPDATA_TOUCH,//data
    UDPDATA_CONNECT,//command
    UDPDATA_DISCONNECT,
    UDPDATA_TOGGLE_STREAM,//client to server
    UDPDATA_SERVER_SEND_CONFIG,//server to client
    UDPDATA_CLIENT_REQUEST_CONFIG//client to server
};

typedef struct {
    struct udpsocket* socketcf;
    struct sockaddr_in* target;//only for sender
    int port;
    void (*callback)(void*);//only for receiver
    void* callback_param[3];//encoder,Another UDPcontext..(depends on how you Implement callback function)
    uint8_t** p;//all data package received will be wrote here 
    uint32_t p_sz;//the size of last UDP received package
    uint8_t p_type;//the type of the last package
    int state;
} udpcontext;

int udp_connect(udpcontext* V,struct sockaddr_in* target,int server_port,const char* server_ip);//can be used for client to connect with server, or with TARGET parameter for server to Synchronize client
void udp_disconnect(udpcontext* sender,udpcontext* receiver);//use local sender to send a stop Singal to receiver to Trigger receiver to stop, used only when client cant receive stop signal after requesting  

int udp_init(udpcontext*);
void udp_recv_loop(udpcontext*); 
void udp_recv(udpcontext*,uint8_t**);
int udp_send(udpcontext* V,uint8_t tp,uint32_t sz,uint8_t* data);
void udp_close(udpcontext*);
void udp_set_timeout(udpcontext* V,int Miliseconds);//used for sender
int addr_check(struct sockaddr_in* sender_target,struct sockaddr_in* rece_target); 
#endif 