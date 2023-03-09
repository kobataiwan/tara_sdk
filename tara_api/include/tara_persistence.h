#ifndef _TARA_PERSISTENCE_H_
#define _TARA_PERSISTENCE_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#define PERSISTENCE_256_NUM                       32

typedef struct {
	s8 data256[PERSISTENCE_256_NUM][256];
	s8 data2048[2048];
} TaraPersistenceData;

#ifdef CPLUSPLUS
extern "C" {
#endif

/* 
 * get persistence data
 */
extern TaraPersistenceData* Tara_persistence_getData();

// set persistence data256
extern s32 Tara_persistence_setData256(s32 id, s8 *data);

// set persistence data2048
extern s32 Tara_persistence_setData2048(s8 *data);

#ifdef CPLUSPLUS
}
#endif

#endif /*_TARA_PERSISTENCE_H_*/
