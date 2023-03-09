#ifndef _TARA_MOTION_PRIV_H_
#define _TARA_MOTION_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_motion.h"


typedef struct {
	s32 needsupdate;
	TaraMotionConfig config;
} MotionConfig_t;

typedef struct {
	s32 semid;
	MotionConfig_t *md;
} MotionConfig;


#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern MotionConfig* Tara_motion_attach(s32 flags);
extern void Tara_motion_detach(MotionConfig *ptr);

#ifdef CPLUSPLUS
}
#endif
#endif

