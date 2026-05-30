#include <libavcodec/avcodec.h>
#include "../include/scdecoder.h"
#include <libswscale/swscale.h>
int decoder_init(VideoDecoder *dec) {
    printf("decoder_init(): Attempting to Initialize decoder\n");
    memset(dec, 0, sizeof(VideoDecoder));

    dec->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!dec->codec) {
        printf("decoder_init(): cant find H.264 decoder\n");
        return -1;
    }

    dec->ctx = avcodec_alloc_context3(dec->codec);
    if (!dec->ctx) return -2;

    
    dec->ctx->thread_count = 2;// multi thread
    dec->ctx->thread_type = FF_THREAD_FRAME;

    if (avcodec_open2(dec->ctx, dec->codec, NULL) < 0) {
        printf("decoder_init(): cant open decoder\n");
        return -3;
    }
    
    dec->frame = av_frame_alloc();
    dec->pkt = av_packet_alloc();

    printf("decoder_init(): Decoder Initialized\n");
    return 0;
}

int decoder_decode(VideoDecoder *dec, uint8_t *input_buf, int compressed_size) {
    if(!dec)return -1;
    dec->pkt->data =input_buf;
    dec->pkt->size = compressed_size;     
    int ret = avcodec_send_packet(dec->ctx, dec->pkt);
    if (ret < 0) {
        char err[1024]={0}; 
        av_strerror(ret, err, sizeof(err));
        printf("decoder_decode():Failed when decoding %s\n", err);
        return ret;
    }
    ret = avcodec_receive_frame(dec->ctx, dec->frame);
    if (ret == 0) {
        // dec->info.size = dec->frame->width * dec->frame->height * 4;
        if(
            dec->info.width!=dec->frame->width||
            dec->info.height!=dec->frame->height||
            dec->info.format!=dec->frame->format){
            printf("\
                decoder_decode(): frame changed:\n\
                                                 from: %dx%d,fmt=%d\n\
                                                 to:   %dx%d,fmt=%d\n",
                                                 dec->info.width,dec->info.height,dec->info.format,
                                                 dec->frame->width,dec->frame->height,dec->frame->format);

            dec->info.size=dec->frame->width*dec->frame->height*4;
            dec->info.width=dec->frame->width;
            dec->info.height=dec->frame->height;
            dec->info.format=dec->frame->format;
            if(dec->info.data[0])free(dec->info.data[0]);
            dec->info.data[0]=(uint8_t*)malloc(dec->info.size);
            dec->info.linesize[0]=dec->info.width*4;
            memset(dec->info.data[0],0,dec->info.size);
            if(dec){
                sws_freeContext(dec->sws_ctx);
                dec->sws_ctx=NULL;
            }
        }
        
        if(!dec->sws_ctx){
            dec->sws_ctx=sws_getContext(
                dec->info.width,dec->info.height,dec->info.format, 
                dec->info.width,dec->info.height,AV_PIX_FMT_RGBA, 
                SWS_FAST_BILINEAR, NULL, NULL, NULL
            ); 
            printf("\
                decoder_decode(): sws context changed:\n\
                                                 from: %dx%d,fmt=%d\n\
                                                 to:   %dx%d,fmt=%d\n",
                                                 dec->frame->width,dec->frame->height,dec->info.format,
                                                 dec->info.width,dec->info.height,AV_PIX_FMT_RGBA);
        }
        sws_scale(dec->sws_ctx,
            (const uint8_t* const*)dec->frame->data,
            (const int*) dec->frame->linesize,0, dec->info.height,
            dec->info.data, dec->info.linesize
        );
        return 1;
    } else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        printf("decoder_decode():cant decode frame\n");
        return 0;  
    } else {
        printf("decoder_decode():Unknown fault,cant decode frame\n");
        return ret;
    }
}

int decoder_close(VideoDecoder* dec){
    if (!dec) return 1;
    if (dec->frame) av_frame_free(&dec->frame);
    if (dec->pkt)     av_packet_free(&dec->pkt);
    if (dec->ctx) avcodec_free_context(&dec->ctx);
    if(dec->sws_ctx)sws_freeContext(dec->sws_ctx);
    if(dec->info.data[0])free(dec->info.data[0]);
    dec->state=0;
	printf("decoder_close(): Decoder has beed DeInitiated\n");
    return 0;
}