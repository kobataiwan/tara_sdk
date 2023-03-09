#ifndef _TARA_SYS_H_
#define _TARA_SYS_H_

#include <errno.h>
#include <time.h>
#include "myglobal.h"
#include "myerr.h"

typedef enum {
	SYS_RESET_SOFT=0,	// network, time and accounts settings remain unchanged by soft reset.
	SYS_RESET_HART,		// all parameters are reset to factory default values.
	SYS_RESET_ACCOUNTS,	// reset all accounts and passwords. 
	SYS_RESET_MAX
} SYS_RESET_TYPE;

#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * reset system
 * return: 0 on success, otherwise error code returned
 * type: 
 */
extern s32 Tara_sys_reset(SYS_RESET_TYPE type);


#ifdef CPLUSPLUS
}
#endif
#endif

