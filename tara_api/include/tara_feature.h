#ifndef _TARA_FEATURE_H_
#define _TARA_FEATURE_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

enum{
	Feature_UnencryptedUpdate_Off = 0,
	Feature_UnencryptedUpdate_On
};

enum {
	Feature_Stream_Off = 0,
	Feature_Stream_On
};

enum {
	Feature_OSD_Off = 0,
	Feature_OSD_On
};

typedef struct {
	s32 ubootenv_offset;
	s32 UnencryptedUpdate;
} TaraFeatureSys;

typedef struct {
	s32 video1;
	s32 video2;
	s32 video3;
	s32 mjpeg1;
	s32 mjpeg2;
} TaraFeatureVideo;

typedef struct {
	s32 osd;
	s32 debug;
} TaraFeatureOsd;

typedef struct {
	TaraFeatureSys sys;
	TaraFeatureVideo stream;
	TaraFeatureOsd osd;
} TaraFeatureConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * get feature
 * return: 0 on success, otherwise error code returned
 * feature: on success, TaraFeature will be stored
 */
extern s32 Tara_feature_getConfig(TaraFeatureConfig *feature);

#ifdef CPLUSPLUS
}
#endif
#endif

