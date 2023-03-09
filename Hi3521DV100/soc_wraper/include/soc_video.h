#ifndef __SOC_VIDEO_H__
#define __SOC_VIDEO_H__
#include "tara_video_encoder_priv.h"
#include "tara_media_stream.h"

enum {
	RAWFRAME_TYPE_OFF=0,
	RAWFRAME_TYPE_VIDEO1=1,
	RAWFRAME_TYPE_VIDEO2=2,
	RAWFRAME_TYPE_MJPEG1=3,
	RAWFRAME_TYPE_MAX=4
};

enum {
	STREAM_TYPE_VIDEO1=0,
	STREAM_TYPE_VIDEO2,
	STREAM_TYPE_MJPEG1,
	STREAM_TYPE_MAX
} STREAM_TYPE_ENUM;

void soc_initCodecPTS(void);
void soc_syncCodecPTS(void);

int video_platform_init(void);
int video_platform_exit(void);
int video_platform_videoEncoderStart(TARA_VIDEO_STREAM_TYPE stream_type, VideoEncoder_t *pvideo);
int video_platform_mjpegEncoderStart(TARA_VIDEO_STREAM_TYPE stream_type, MjpegEncoder_t *pmj);

int video_platform_getStreamStart(int vi_num, int stream_num);
int video_platform_getStreamStop(void);
int video_platform_getRawFrameStart(int type);
int video_platform_getRawFrameStop(void);

int video_platform_setVideoEncoder(int chan, TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *venc);
int video_platform_setMjpegEncoder(int chan, TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *mj);

int video_platform_setAllVideoEncoder(TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *venc);
int video_platform_setAllMjpegEncoder(TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *mj);
int video_platform_setVideoOnOff(int channel, int on);
int video_platform_setMjpegOnOff(int channel, int on);
int video_platform_setVideoFrameRate(int chan, int sub, int framerate);
int video_platform_setMjpegFrameRate(int chan, int sub, int framerate);

//int video_platform_setVideoSourceFrameRate(int chan, int frate);
int video_platform_setVideoSourceBlueScreen(int channel, int on);
#endif /* End of #ifndef __SOC_VIDEO_H__ */

