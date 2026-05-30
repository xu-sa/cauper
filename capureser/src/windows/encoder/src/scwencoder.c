#include "../include/scencoder.h"
int encoder_init(VideoEncoder *enc, int w, int h, uint16_t pw_fmt){
    return 1;
};
uint32_t encoder_encode(VideoEncoder *enc, uint8_t *raw_data,int32_t raw_stride,uint8_t** output){
    return 1;
};
void encoder_cleanup(VideoEncoder *enc){};