#ifndef _TARA_VIDEO_ENCODER_PRIV_H_
#define _TARA_VIDEO_ENCODER_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_video_encoder.h"


typedef struct {
	s32 needsupdate;
	TaraVideoEncoder venc;
} VideoEncoder_t;

typedef struct {
	s32 needsupdate;
	TaraMjpegEncoder mjenc;
} MjpegEncoder_t;

typedef struct {
	s32 semid;
	VideoEncoder_t *enc;
} VideoEncoder;

typedef struct {
	s32 semid;
	MjpegEncoder_t *enc;
} MjpegEncoder;

#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern void* Tara_video_encoder_attach(s32 type, s32 flags);
extern void Tara_video_encoder_detach(void *ptr);

#ifdef CPLUSPLUS
}
#endif
#endif

