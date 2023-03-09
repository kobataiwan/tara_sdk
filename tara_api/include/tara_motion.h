#ifndef _TARA_MOTION_H_
#define _TARA_MOTION_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

typedef enum {
	MOTION_OFF = 0,
	MOTION_ON
} MOTION_ONOFF;

typedef enum {
	MOTION_OSD_OFF = 0,
	MOTION_OSD_AREA,
	MOTION_OSD_MAX
} MOTION_OSD_TYPE;

typedef enum {
	MOTION_OSDON_VIDEO1=0,
	MOTION_OSDON_VIDEO2,
	MOTION_OSDON_MJPEG1,
	MOTION_OSDON_ALL,
	MOTION_OSDON_MAX
} MOTION_OSDON_TYPE;

typedef enum {
	MOTION_STATE_FALSE=0,
	MOTION_STATE_TRUE
} MOTION_STATE;

typedef struct {
	MOTION_ONOFF motion;
	s32 sensitivity;
	MOTION_OSD_TYPE mdosd;
	MOTION_OSDON_TYPE mdosd_on;
	MOTION_STATE mdstate;
} TaraMotionConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif


/* 
 * get motion configuration
 * return: 0 on success, otherwise error code returned
 * chan: channel number to be retrieved
 * ptr: filled with TaraMotionConfig
 */
extern s32 Tara_motion_getConfig(s32 chan, TaraMotionConfig *ptr);

/* 
 * set motion configuration
 * return: 0 on success, otherwise error code returned
 * chan: channel number to be apply
 * ptr: motion configuration
 */
extern s32 Tara_motion_setConfig(s32 chan, TaraMotionConfig *ptr);

/* 
 * get all motion's configuration
 * return: on success, returns a pointer to TaraMotionConfig[] for all configuration,
 *         on fail, returns null.
 */
extern TaraMotionConfig* Tara_motion_getAll();

/* 
 * apply configuration to all motion
 * return: 0 on success, otherwise error code returned
 * ptr: on success, returns a TaraMotionConfig array
 */
extern s32 Tara_motion_setAll(TaraMotionConfig *ptr);

#ifdef CPLUSPLUS
}
#endif
#endif

