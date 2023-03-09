#ifndef _TARA_PERSISTENCE_PRIV_H_
#define _TARA_PERSISTENCE_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#ifndef PERSISTENCE_256_NUM
#define PERSISTENCE_256_NUM                       32
#endif

typedef struct {
	s32 needupdate;
	s8 data[256];
} boot_env_256;

typedef struct {
	s32 needupdate;
	s8 data[2048];
} boot_env_2048;

typedef struct {
	s32 semid;
	boot_env_256 data256[PERSISTENCE_256_NUM];
	boot_env_2048 data2048;
} persistence_data;

#ifdef CPLUSPLUS
extern "C" {
#endif


// attach to shared memory
extern persistence_data* Tara_persistence_attach(s32 flags);

extern void Tara_persistence_detach(persistence_data *ptr);

#ifdef CPLUSPLUS
}
#endif

#endif /*_TARA_PERSISTENCE_PRIV_H_*/
