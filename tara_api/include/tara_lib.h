#ifndef __TARA_LIB_H__
#define __TARA_LIB_H__

#ifdef CPLUSPLUS
extern "C" {
#endif

#include "myglobal.h"
#include "myerr.h"
#include "tara_video_source.h"
#include "tara_video_frame.h"

/*
 * init sdk
 * return: 0 on success, otherwise error code returned
 */
extern s32 Tara_init();


#ifdef CPLUSPLUS
}
#endif
#endif
