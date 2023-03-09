#ifndef _TARA_INFO_PRIV_H_
#define _TARA_INFO_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_info.h"

typedef struct {
	s32 needsupdate;
	TaraInfoConfig config;
} InfoConfig_t;

typedef struct {
	s32 semid;
	InfoConfig_t *info;
} InfoConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern InfoConfig* Tara_info_attach(s32 flags);

extern void Tara_info_detach(InfoConfig *ptr);


#ifdef CPLUSPLUS
}
#endif
#endif

