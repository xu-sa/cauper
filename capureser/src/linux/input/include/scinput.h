#ifndef SCUINPUT_H
#define SCUINPUT_H

typedef struct {
    int fd;
    int screen_width;
    int screen_height;
} scinput_context;

int scinput_init(scinput_context *ctx, int width, int height,const char* name);

void scinput_input(scinput_context *ctx, int x, int y, int is_down);

void scinput_sync(scinput_context *ctx);

void scinput_close(scinput_context *ctx);

#endif