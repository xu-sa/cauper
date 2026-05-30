#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include "../include/scinput.h"

static void uinput_emit(int fd, int type, int code, int val) {
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = val;
    write(fd, &ie, sizeof(ie));
}

int scinput_init(scinput_context *ctx, int width, int height,const char* name) {
    ctx->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (ctx->fd < 0) {
        perror("cant open /dev/uinput");
        return 0;
    }

    ctx->screen_width = width;
    ctx->screen_height = height;
    ioctl(ctx->fd, UI_SET_EVBIT, EV_KEY);
    ioctl(ctx->fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl(ctx->fd, UI_SET_EVBIT, EV_ABS);
    ioctl(ctx->fd, UI_SET_ABSBIT, ABS_X);
    ioctl(ctx->fd, UI_SET_ABSBIT, ABS_Y);
    struct uinput_abs_setup abs_setup;
    
    // X 
    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_X;
    abs_setup.absinfo.minimum = 0;
    abs_setup.absinfo.maximum = width;
    ioctl(ctx->fd, UI_ABS_SETUP, &abs_setup);

    // Y
    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_Y;
    abs_setup.absinfo.minimum = 0;
    abs_setup.absinfo.maximum = height;
    ioctl(ctx->fd, UI_ABS_SETUP, &abs_setup);

    // device
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1111;
    usetup.id.product = 0x2222;
    strcpy(usetup.name, name);

    ioctl(ctx->fd, UI_DEV_SETUP, &usetup);
    ioctl(ctx->fd, UI_DEV_CREATE);

    return 1;
}

void scinput_input(scinput_context *ctx, int x, int y, int is_down) {
    uinput_emit(ctx->fd, EV_ABS, ABS_X, x);
    uinput_emit(ctx->fd, EV_ABS, ABS_Y, y);
    uinput_emit(ctx->fd, EV_KEY, BTN_TOUCH, is_down);
    // virtouchsc_sync(ctx);
}

void scinput_sync(scinput_context *ctx) {
    uinput_emit(ctx->fd, EV_SYN, SYN_REPORT, 0);
}

void scinput_close(scinput_context *ctx) {
    if (ctx->fd >= 0) {
        ioctl(ctx->fd, UI_DEV_DESTROY);
        close(ctx->fd);
        ctx->fd = -1;
    }
}