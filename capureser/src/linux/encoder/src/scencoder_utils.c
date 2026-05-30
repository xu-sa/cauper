#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

enum pipewire_spa_video_format_ {
	SPA_VIDEO_FORMAT_UNKNOWN,
	SPA_VIDEO_FORMAT_ENCODED,
	SPA_VIDEO_FORMAT_I420,
	SPA_VIDEO_FORMAT_YV12,
	SPA_VIDEO_FORMAT_YUY2,
	SPA_VIDEO_FORMAT_UYVY,
	SPA_VIDEO_FORMAT_AYUV,
	SPA_VIDEO_FORMAT_RGBx,
	SPA_VIDEO_FORMAT_BGRx,
	SPA_VIDEO_FORMAT_xRGB,
	SPA_VIDEO_FORMAT_xBGR,
	SPA_VIDEO_FORMAT_RGBA,
	SPA_VIDEO_FORMAT_BGRA,
	SPA_VIDEO_FORMAT_ARGB,
	SPA_VIDEO_FORMAT_ABGR,
	SPA_VIDEO_FORMAT_RGB,
	SPA_VIDEO_FORMAT_BGR,
	SPA_VIDEO_FORMAT_Y41B,
	SPA_VIDEO_FORMAT_Y42B,
	SPA_VIDEO_FORMAT_YVYU,
	SPA_VIDEO_FORMAT_Y444,
	SPA_VIDEO_FORMAT_v210,
	SPA_VIDEO_FORMAT_v216,
	SPA_VIDEO_FORMAT_NV12,
	SPA_VIDEO_FORMAT_NV21,
	SPA_VIDEO_FORMAT_GRAY8,
	SPA_VIDEO_FORMAT_GRAY16_BE,
	SPA_VIDEO_FORMAT_GRAY16_LE,
	SPA_VIDEO_FORMAT_v308,
	SPA_VIDEO_FORMAT_RGB16,
	SPA_VIDEO_FORMAT_BGR16,
	SPA_VIDEO_FORMAT_RGB15,
	SPA_VIDEO_FORMAT_BGR15,
	SPA_VIDEO_FORMAT_UYVP,
	SPA_VIDEO_FORMAT_A420,
	SPA_VIDEO_FORMAT_RGB8P,
	SPA_VIDEO_FORMAT_YUV9,
	SPA_VIDEO_FORMAT_YVU9,
	SPA_VIDEO_FORMAT_IYU1,
	SPA_VIDEO_FORMAT_ARGB64,
	SPA_VIDEO_FORMAT_AYUV64,
	SPA_VIDEO_FORMAT_r210,
	SPA_VIDEO_FORMAT_I420_10BE,
	SPA_VIDEO_FORMAT_I420_10LE,
	SPA_VIDEO_FORMAT_I422_10BE,
	SPA_VIDEO_FORMAT_I422_10LE,
	SPA_VIDEO_FORMAT_Y444_10BE,
	SPA_VIDEO_FORMAT_Y444_10LE,
	SPA_VIDEO_FORMAT_GBR,
	SPA_VIDEO_FORMAT_GBR_10BE,
	SPA_VIDEO_FORMAT_GBR_10LE,
	SPA_VIDEO_FORMAT_NV16,
	SPA_VIDEO_FORMAT_NV24,
	SPA_VIDEO_FORMAT_NV12_64Z32,
	SPA_VIDEO_FORMAT_A420_10BE,
	SPA_VIDEO_FORMAT_A420_10LE,
	SPA_VIDEO_FORMAT_A422_10BE,
	SPA_VIDEO_FORMAT_A422_10LE,
	SPA_VIDEO_FORMAT_A444_10BE,
	SPA_VIDEO_FORMAT_A444_10LE,
	SPA_VIDEO_FORMAT_NV61,
	SPA_VIDEO_FORMAT_P010_10BE,
	SPA_VIDEO_FORMAT_P010_10LE,
	SPA_VIDEO_FORMAT_IYU2,
	SPA_VIDEO_FORMAT_VYUY,
	SPA_VIDEO_FORMAT_GBRA,
	SPA_VIDEO_FORMAT_GBRA_10BE,
	SPA_VIDEO_FORMAT_GBRA_10LE,
	SPA_VIDEO_FORMAT_GBR_12BE,
	SPA_VIDEO_FORMAT_GBR_12LE,
	SPA_VIDEO_FORMAT_GBRA_12BE,
	SPA_VIDEO_FORMAT_GBRA_12LE,
	SPA_VIDEO_FORMAT_I420_12BE,
	SPA_VIDEO_FORMAT_I420_12LE,
	SPA_VIDEO_FORMAT_I422_12BE,
	SPA_VIDEO_FORMAT_I422_12LE,
	SPA_VIDEO_FORMAT_Y444_12BE,
	SPA_VIDEO_FORMAT_Y444_12LE,

	SPA_VIDEO_FORMAT_RGBA_F16,
	SPA_VIDEO_FORMAT_RGBA_F32,

	SPA_VIDEO_FORMAT_xRGB_210LE,	/**< 32-bit x:R:G:B 2:10:10:10 little endian */
	SPA_VIDEO_FORMAT_xBGR_210LE,	/**< 32-bit x:B:G:R 2:10:10:10 little endian */
	SPA_VIDEO_FORMAT_RGBx_102LE,	/**< 32-bit R:G:B:x 10:10:10:2 little endian */
	SPA_VIDEO_FORMAT_BGRx_102LE,	/**< 32-bit B:G:R:x 10:10:10:2 little endian */
	SPA_VIDEO_FORMAT_ARGB_210LE,	/**< 32-bit A:R:G:B 2:10:10:10 little endian */
	SPA_VIDEO_FORMAT_ABGR_210LE,	/**< 32-bit A:B:G:R 2:10:10:10 little endian */
	SPA_VIDEO_FORMAT_RGBA_102LE,	/**< 32-bit R:G:B:A 10:10:10:2 little endian */
	SPA_VIDEO_FORMAT_BGRA_102LE,	/**< 32-bit B:G:R:A 10:10:10:2 little endian */

	/* Aliases */
	SPA_VIDEO_FORMAT_DSP_F32 = SPA_VIDEO_FORMAT_RGBA_F32,
};

const static char* hardware_encoders[] = {
	"h264_vaapi", // Linux Genetic
	"h264_qsv",   // Intel 
	"h264_amf",   // Windows AMD , No longer Supported on latest linux GPU driver
	"h264_nvenc", // NVIDIA 
	"libx264"     // software
};

enum AVPixelFormat pw_format_to_av(uint16_t pw_fmt) {
    switch (pw_fmt) {
        case SPA_VIDEO_FORMAT_BGRx: return AV_PIX_FMT_BGR0;
        case SPA_VIDEO_FORMAT_BGRA: return AV_PIX_FMT_BGR0;
		case SPA_VIDEO_FORMAT_xRGB: return AV_PIX_FMT_BGR0;

		case SPA_VIDEO_FORMAT_xBGR: return AV_PIX_FMT_RGB0;
        case SPA_VIDEO_FORMAT_RGBx: return AV_PIX_FMT_RGB0;
        case SPA_VIDEO_FORMAT_RGBA: return AV_PIX_FMT_RGB0;

		case SPA_VIDEO_FORMAT_NV12: return AV_PIX_FMT_NV12;
        case SPA_VIDEO_FORMAT_YUY2: return AV_PIX_FMT_YUYV422;
        case SPA_VIDEO_FORMAT_NV16: return AV_PIX_FMT_NV16;
        case SPA_VIDEO_FORMAT_I420: return AV_PIX_FMT_YUV420P;
        // case SPA_VIDEO_FORMAT_P010_10BE: return AV_PIX_FMT_P010;
        default: return AV_PIX_FMT_NONE;
    }
}

enum AVHWDeviceType hardware_device_type(const char* tp){
	if (strcmp(tp, "h264_qsv") == 0) return AV_HWDEVICE_TYPE_QSV;
	else if (strcmp(tp, "h264_vaapi") == 0) return AV_HWDEVICE_TYPE_VAAPI;
	else if(strcmp(tp,"h264_nvenc")==0)return AV_HWDEVICE_TYPE_CUDA;
	#ifdef _WIN32
	else if(strcmp(tp,"h264_amf")==0)return AV_HWDEVICE_TYPE_D3D11VA;
	#endif
	else return AV_HWDEVICE_TYPE_NONE;
}

enum AVPixelFormat hardware_pixel_type(uint8_t tp){
	switch (tp) {
		case AV_HWDEVICE_TYPE_VAAPI: return AV_PIX_FMT_VAAPI; 
		case AV_HWDEVICE_TYPE_CUDA:  return AV_PIX_FMT_CUDA;  
		case AV_HWDEVICE_TYPE_QSV:   return AV_PIX_FMT_QSV;   
		case AV_HWDEVICE_TYPE_D3D11VA: return AV_PIX_FMT_D3D11; 
		default: return AV_PIX_FMT_NONE;
	}
}

int create_hw_frames_ctx(AVCodecContext *ctx, int width, int height, enum AVPixelFormat hw_format) {
    if (!ctx->hw_device_ctx) return -1;

    AVBufferRef *hw_frames_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
    if (!hw_frames_ref) return -1;

    AVHWFramesContext *frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
    //Procedure 4
	frames_ctx->format    = hw_format;        //AVPixelFormat of hardware
	//Procedure 3
    frames_ctx->sw_format = AV_PIX_FMT_NV12;  //General Transmission Type from RAM to Graphic RAM
    frames_ctx->width     = width;
    frames_ctx->height    = height;
    frames_ctx->initial_pool_size = 20;     

    int ret = av_hwframe_ctx_init(hw_frames_ref);
    if (ret < 0) {
        av_buffer_unref(&hw_frames_ref);
        return ret;
    }

    ctx->hw_frames_ctx = hw_frames_ref; 
    return 0;
}

AVCodecContext* get_hardware_encoder(int width, int height, int framerate) {
    for (int i = 0; i < sizeof(hardware_encoders) / sizeof(char*); i++) {
        const AVCodec *codec = avcodec_find_encoder_by_name(hardware_encoders[i]);
        if (!codec) continue;
        AVCodecContext *ctx = avcodec_alloc_context3(codec);
        {//build AVCodecContext
			ctx->width = width; ctx->height = height;
			ctx->time_base = (AVRational){1, framerate}; 
			ctx->framerate = (AVRational){framerate, 1};
			ctx->bit_rate = 5.3 * 1000 * 1000;
			ctx->rc_max_rate = 10 * 1000 * 1000;
			ctx->rc_buffer_size = 6.7 * 1000 * 1000;
			ctx->gop_size = 60; 
			ctx->max_b_frames = 0;
			ctx->flags &= ~AV_CODEC_FLAG_GLOBAL_HEADER;
		}
		enum AVHWDeviceType tp = hardware_device_type(hardware_encoders[i]);
        enum AVPixelFormat hw_pix_fmt=hardware_pixel_type(tp);
        if (tp!=AV_HWDEVICE_TYPE_NONE && hw_pix_fmt != AV_PIX_FMT_NONE) {
            ctx->pix_fmt = hw_pix_fmt;
			av_hwdevice_ctx_create(&ctx->hw_device_ctx, tp, NULL, NULL, 0);
            if (create_hw_frames_ctx(ctx, width, height, hw_pix_fmt) < 0)printf("Warning: Failed to create hw_frames_ctx for %s\n", hardware_encoders[i]);
        } else ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        
        //settings for Specific Encoder Optimization
		if (tp == AV_HWDEVICE_TYPE_QSV) av_opt_set(ctx->priv_data, "async_depth", "1", 0);
		if (tp==AV_HWDEVICE_TYPE_CUDA) {
			av_opt_set(ctx->priv_data, "repeat_headers", "1", 0);
			av_opt_set(ctx->priv_data, "forced-idr", "1", 0);
		}
	
		av_opt_set(ctx->priv_data, "preset", "ultrafast", 0);
		av_opt_set(ctx->priv_data, "tune", "zerolatency", 0);
		
		if (avcodec_open2(ctx, codec, NULL) >= 0) return ctx;
        avcodec_free_context(&ctx);	
	}
    return NULL;

}
