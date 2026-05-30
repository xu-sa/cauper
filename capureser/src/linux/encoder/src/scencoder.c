#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

#include <stdio.h>
#include <string.h>
#include "../include/scencoder.h"

extern enum AVPixelFormat pw_format_to_av(uint16_t pw_fmt);
extern enum AVHWDeviceType hardware_device_type(const char* tp);
extern enum AVPixelFormat hardware_pixel_type(uint8_t tp);
extern int create_hw_frames_ctx(AVCodecContext *ctx, int width, int height, enum AVPixelFormat hw_format);
extern AVCodecContext* get_hardware_encoder(int width, int height, int framerate);
int encoder_init(VideoEncoder *enc, int w, int h,uint16_t pw_fmt) {
	if(!enc){
		printf("encoder_init(): encoder is null\n");
		return -1;
	}
	memset(enc, 0, sizeof(VideoEncoder));
	if(enc->state==1){
		printf("encoder_init(): Encoder is Initialized Already!\n");
		
		return 0;
	}
	//AVCodecContext
	enc->codec_ctx = get_hardware_encoder(w, h, ENCODER_TARGET_FRAMETRATE);
    if (!enc->codec_ctx) return -1;
	printf("encoder_init():AVCodecContext Created , configuration:\n\
				encoder  =%s\n\
				format   =%d\n", enc->codec_ctx->codec->name,enc->codec_ctx->pix_fmt
	);
    //SwsContext
	enc->sws_ctx = sws_getContext(
        w, h, pw_format_to_av(pw_fmt),//Generally one plane for RGB
		w, h, AV_PIX_FMT_NV12,//two planes
        SWS_FAST_BILINEAR, NULL, NULL, NULL
    );
	if(!enc->sws_ctx)return -2;
	printf("encoder_init():SwsContext created, configuration:\n\
				input_format   =%d\n\
				target_format =%d\n",pw_fmt,AV_PIX_FMT_NV12);
	//AVFrame
	enc->hwframe = av_frame_alloc();
	enc->avframe = av_frame_alloc();
	if(!enc->avframe)return -3;
    enc->avframe->format = AV_PIX_FMT_NV12;
    enc->avframe->width = w;
    enc->avframe->height = h;
    if (av_frame_get_buffer(enc->avframe, 32) < 0) return -3;
		
    //AVPacket
    enc->pkt = av_packet_alloc();
	if (!enc->pkt) return -1;
	printf("encoder_init():Successfully loaded encoder\n");
    enc->state=1;
	return 0;
}

uint32_t encoder_encode(VideoEncoder *enc, uint8_t *raw_frame,int32_t stride,uint8_t** output){
	if(!enc)return 0;
	uint32_t compress=0;
	static uint64_t ts=0;
	av_frame_make_writable(enc->avframe);//av_frame_make_writable(enc->avframe);
	sws_scale(enc->sws_ctx, 
              (const uint8_t* const*)&raw_frame, &stride, 
              0, enc->codec_ctx->height,
              enc->avframe->data, enc->avframe->linesize);//convert raw frame format to avframe desired format and store frame buffer with member pointer 
	if(av_hwframe_get_buffer(enc->codec_ctx->hw_frames_ctx, enc->hwframe, 0)==-1)return 0;
	if(av_hwframe_transfer_data(enc->hwframe, enc->avframe, 0)==-1)return 0;
	enc->hwframe->pts = ts++;
	if (avcodec_send_frame(enc->codec_ctx, enc->hwframe)< 0) {//the encoding process shall be Processed and Accomplished at this moment
        printf("encoder_encode(): Error sending frame to encoder\n");
        av_frame_unref(enc->hwframe);
		return 0;
    }
	while (1) {
		int ret = avcodec_receive_packet(enc->codec_ctx, enc->pkt);
		if (ret == 0) 
		{
			if (compress + enc->pkt->size <= ENCODEDE_SIZE) {
				memcpy(*output + compress, enc->pkt->data, enc->pkt->size);
				compress += enc->pkt->size;
			} else {
				printf("encoder_encode(): Warning, output buffer full, dropping packet\n");
				return 0;
			}
			av_packet_unref(enc->pkt);
		} 
		else if (ret == AVERROR(EAGAIN))break;
		else {
			printf("encoder_encode(): Error receiving packet: %d\n", ret);
			return 0;
    	}
	}
	av_frame_unref(enc->hwframe);
    return compress;
};

void encoder_cleanup(VideoEncoder *enc) {
    if (!enc) return;
    if (enc->sws_ctx) sws_freeContext(enc->sws_ctx);
    if (enc->avframe) av_frame_free(&enc->avframe);
    if (enc->pkt)     av_packet_free(&enc->pkt);
    if (enc->codec_ctx) avcodec_free_context(&enc->codec_ctx);
	enc->state=0;
	printf("encoder has beed DeInitiated\n");
}
