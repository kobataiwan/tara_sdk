#ifndef _TARA_TIME_PRIV_H_
#define _TARA_TIME_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_time.h"

typedef struct {
	s32 needsupdate;
	TaraDateTimeConfig config;
} DateTimeConfig_t;

typedef struct {
	s32 semid;
	DateTimeConfig_t *dt;
} DateTimeConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern DateTimeConfig* Tara_time_attach(s32 flags);

extern void Tara_time_detach(DateTimeConfig *ptr);


#ifdef CPLUSPLUS
}
#endif
#endif

