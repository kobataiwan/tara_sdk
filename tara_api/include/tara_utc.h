#ifndef _TARA_UTC_H_
#define _TARA_UTC_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#define UTC_CMD_SOCKET              "/tmp/.utc_cmd"

enum {
	UTC_CMD_UP=0,
	UTC_CMD_DOWN,
	UTC_CMD_LEFT,
	UTC_CMD_RIGHT,
	UTC_CMD_ENTER,
	UTC_CMD_MAX
};


#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * utc control
 * return: 0 on success, otherwise error code returned
 * chan: channel index
 * cmd: utc command
 * data: optional
 */
extern s32 Tara_utc_control(s32 chan, s32 cmd, void *data);

#ifdef CPLUSPLUS
}
#endif
#endif

