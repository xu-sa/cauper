#include "../include/sccap.h"

int sccap_init(sccap_context *ctx){
    
    return 1;
};
void sccap_loop(sccap_context *ctx){};
void sccap_stop(sccap_context *ctx){};
void sccap_stream_toggle(sccap_context *ctx,int state){};
int sccap_stream_get(sccap_context* ctx){
    return 1;
};
