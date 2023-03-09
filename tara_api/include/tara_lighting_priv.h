#ifndef _TARA_LIGHTING_PRIV_H_
#define _TARA_LIGHTING_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_lighting.h"


typedef struct {
	s32 needsupdate;
	TaraLightingConfig config;
} LightingConfig_t;

typedef struct {
	s32 semid;
	LightingConfig_t *lighting;
	TaraLightingDevice *dev;
} LightingConfig;


#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern LightingConfig* Tara_lighting_attach(s32 flags);
extern void Tara_lighting_detach(LightingConfig *ptr);

#ifdef CPLUSPLUS
}
#endif
#endif
