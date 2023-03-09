#ifndef _TARA_FEATURE_PRIV_H_
#define _TARA_FEATURE_PRIV_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_feature.h"

typedef struct {
	s32 semid;
	TaraFeatureConfig *feature;
} FeatureConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif

// attach to shared memory
extern FeatureConfig* Tara_feature_attach(s32 flags);

extern void Tara_feature_detach(FeatureConfig *ptr);


#ifdef CPLUSPLUS
}
#endif
#endif

