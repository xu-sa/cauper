#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
extern "C" {
#include "../include/scuinput.h"

}
int main() {
    UinputContext ctx;
    if (!virtouchsc_init(&ctx, 1920, 1080,"cccc1")) {
        fprintf(stderr, "virtouchsc_init failed!\n");
        return 1;
    }
    int start=0;
    scanf("%d\n",&start);
    if(start!=1)return 0;

    virtouchsc_touch(&ctx, 500, 500, 1);  // 按下
    sleep(1);

    virtouchsc_touch(&ctx, 1000, 600, 1); // 移动（保持按下）
    sleep(1);

    virtouchsc_touch(&ctx, 1500, 800, 1);
    sleep(1);

    virtouchsc_touch(&ctx, 1500, 800, 0); // 抬起
    sleep(1);

    virtouchsc_touch(&ctx, 100, 100, 1);
    virtouchsc_sync(&ctx);
    usleep(100000); // 100ms
    virtouchsc_touch(&ctx, 100, 100, 0);
    virtouchsc_sync(&ctx);
    sleep(1);

    virtouchsc_close(&ctx);
    return 0;
}