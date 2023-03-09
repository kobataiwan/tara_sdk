#ifndef _TARA_VIDEO_ENCODER_H_
#define _TARA_VIDEO_ENCODER_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

enum {
	Tara_Video_Encoder_1=1,
	Tara_Video_Encoder_2=2,
	Tara_Mjpeg_Encoder=3
};

typedef enum {
	ENCODER_OFF=0,
	ENCODER_ON
} ENCODER_ONOFF;

typedef enum {
	VIDEO_PROFILE_HIGH=0,
	VIDEO_PROFILE_MAIN,
	VIDEO_PROFILE_BASELINE,
	VIDEO_PROFILE_MAX
} VIDEO_PROFILE;

typedef enum {
	VIDEO_RATE_CONTROL_CBR=0,
	VIDEO_RATE_CONTROL_VBR,
	VIDEO_RATE_CONTROL_FIXEDQP,
	VIDEO_RATE_CONTROL_MAX
} VIDEO_RATE_CONTROL_MODE;

typedef struct {
	ENCODER_ONOFF video;
	s32 type;
	VIDEO_PROFILE profile;
	s32 width;
	s32 height;
	s32 framerate;
	VIDEO_RATE_CONTROL_MODE ratecontrol;
	s32 qp;
	s32 bitrate;
	s32 gopsize;
} TaraVideoEncoder;

typedef struct {
	ENCODER_ONOFF mjpeg;
	s32 width;
	s32 height;
	s32 framerate;
	s32 quality;
} TaraMjpegEncoder;

#ifdef CPLUSPLUS
extern "C" {
#endif


/* 
 * get video encoder configuration
 * return: 0 for success, otherwise error number returned
 * type: can be Tara_Video_Encoder_1 / Tara_Video_Encoder_2 / Tara_Mjpeg_Encoder
 * chan: channel number to be retrieved
 * pve : if type is Tara_Mjpeg_Encoder, pve must be TaraMjpegEncoder; 
 *       otherwise pve must be TaraVideoEncoder.
 */
extern s32 Tara_venc_getVideoEncoder(s32 type, s32 chan, void *pve);

/* 
 * set video encoder configuration
 * return: 0 for success, otherwise error number returned
 * type: can be Tara_Video_Encoder_1 / Tara_Video_Encoder_2 / Tara_Mjpeg_Encoder
 * chan: channel number to be apply
 * pve : if type is Tara_Mjpeg_Encoder, pve must be TaraMjpegEncoder; 
 *       otherwise pve must be TaraVideoEncoder.
 */
extern s32 Tara_venc_setVideoEncoder(s32 type, s32 chan, void *pve);

/* 
 * get all video encoders' configuration
 * return: on success, returns a pointer to TaraMjpegEncoder[] for type Tara_Mjpeg_Encoder, 
 *         or a pointer to TaraVideoEncoder[] for type Tara_Video_Encoder_1 and Tara_Video_Encoder_2. 
 *         on fail, returns null.
 * type: can be Tara_Video_Encoder_1 / Tara_Video_Encoder_2 / Tara_Mjpeg_Encoder
 */
extern void* Tara_venc_getAllVideoEncoder(s32 type);

/* 
 * apply configuration to all videos
 * return: 0 for success, otherwise error number returned
 * type: can be Tara_Video_Encoder_1 / Tara_Video_Encoder_2 / Tara_Mjpeg_Encoder\
 * pve : if type is Tara_Mjpeg_Encoder, pve must be TaraMjpegEncoder; 
 *       otherwise pve must be TaraVideoEncoder.
 */
extern s32 Tara_venc_setAllVideoEncoder(s32 type, void *pve);

#ifdef CPLUSPLUS
}
#endif
#endif

