#ifndef SCDECODER_H
#define SCDECODER_H 
#include <stdint.h>
struct frameif{
    int width;
    int height;
    int format;

    int size;
    int linesize[8];
    uint8_t* data[8];
};

typedef struct {
    const struct AVCodec *codec;
    struct AVCodecContext *ctx;
    struct AVFrame *frame;
    struct AVPacket *pkt;
    struct SwsContext *sws_ctx;
    struct frameif info;//the result information and data
    int state;
} VideoDecoder;

int decoder_init(VideoDecoder *dec) ;
int decoder_decode(VideoDecoder *dec, uint8_t* input_buf, int compressed_size);
int decoder_close(VideoDecoder* dec);
#endif