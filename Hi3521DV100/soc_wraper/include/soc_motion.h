#ifndef __SOC_MOTION_H__
#define __SOC_MOTION_H__

#include "wraper_comm.h"

int motion_soc_vdaStart(int vs, int sadth);
int motion_soc_vdaStop(int vs);
int motion_getStatus(int vs);
int motion_setSensitivity(int vs, int value);
void load_motion_vdamdConfigFile(void);


#endif 

