#ifndef _TARA_VIDEO_SOURCE_PRIV_H_
#define _TARA_VIDEO_SOURCE_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

typedef struct {
	s32 needsupdate;
	s32 vicontrol;
	s32 type;
	s32 width;
	s32 height;
	s32 framerate;
} videosource_t;

typedef struct {
	s32 semid;
	videosource_t *vsrc;
} VideoSource;


#ifdef CPLUSPLUS
extern "C" {
#endif

// global system environment variable of channel number
//extern u32 num_channel;

// attach to shared memory
extern VideoSource* Tara_video_source_attach(s32 flags);

extern void Tara_video_source_detach(VideoSource *ptr);

//extern s32 Tara_video_source_setVideoInputControl(int num, int on);

//extern s32 Tara_video_source_getVideoInputControl(int num);

#ifdef CPLUSPLUS
}
#endif
#endif

