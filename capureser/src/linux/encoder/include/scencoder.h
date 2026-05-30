#ifndef SCENCODER_H
#define SCENCODER_H
#define ENCODEDE_SIZE 1024*800
#define ENCODER_TARGET_FRAMETRATE 60
#include <stdint.h>

typedef struct {
    struct AVCodecContext *codec_ctx;
    struct SwsContext *sws_ctx;
    struct AVFrame *avframe;
    struct AVFrame* hwframe;
    struct AVPacket *pkt;
    int state;
} VideoEncoder;

int encoder_init(VideoEncoder *enc, int w, int h, uint16_t pw_fmt);
uint32_t encoder_encode(VideoEncoder *enc, uint8_t *raw_data,int32_t raw_stride,uint8_t** output);
void encoder_cleanup(VideoEncoder *enc);

#endif