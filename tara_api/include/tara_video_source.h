#ifndef _TARA_VIDEO_SOURCE_H_
#define _TARA_VIDEO_SOURCE_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

typedef enum {
	VS_TYPE_NOSIGNAL=0,
	VS_TYPE_CVBS,
	VS_TYPE_AHD,
	VS_TYPE_TVI,
	VS_TYPE_HDCVI,
	VS_TYPE_UNKNOWN
} VIDEO_SOURCE_TYPE;

typedef struct {
	s32 type;             // VIDEO_SOURCE_TYPE
	s32 width;
	s32 height;
	s32 framerate;
} TaraVideoSource;


#ifdef CPLUSPLUS
extern "C" {
#endif

// global system environment variable of channel number
//extern u32 num_channel;
	
/*
 * get the status of video channel
 * return: on error negative value is returned; on success returns 0
 * chan: channel index, zero-based
 * pvs: where video channel status will be stored
 */
extern s32 Tara_video_source_getStatus(s32 chan, TaraVideoSource *pvs);

//extern s32 Tara_video_source_setVideoInputControl(s32 chan, s32 on);

//extern s32 Tara_video_source_getVideoInputControl(s32 chan, s32 *on);

#ifdef CPLUSPLUS
}
#endif
#endif

