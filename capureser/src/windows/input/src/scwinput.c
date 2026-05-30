#include "../include/scinput.h"
int scinput_init(scinput_context *ctx, int width, int height,const char* name){
    return 1;
};

void scinput_input(scinput_context *ctx, int x, int y, int is_down){};

void scinput_sync(scinput_context *ctx){};

void scinput_close(scinput_context *ctx){};
