#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_lighting_priv.h"

static LightingConfig *pshm = NULL;

LightingConfig* Tara_lighting_attach(s32 flags)
{
	LightingConfig *ret=NULL;
	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey = LIGHTING_KEY;
	shmsize = sizeof(s32) + sizeof(LightingConfig_t) + sizeof(TaraLightingDevice);

	if ((flags&O_RDWR) != 0) {
		shmid = shmget(shmkey, shmsize, IPC_CREAT|IPC_EXCL|0666);
		if (shmid != -1) is_create = 1;
		if (shmid==-1 && errno==EEXIST) shmid = shmget(shmkey, shmsize, 0666);
	} else shmid = shmget(shmkey, shmsize, 0444);

	if (shmid == -1) errno = ERROR_FAIL;
	else {
		ptr = (s32*)shmat(shmid, 0, 0);
		if (ptr == (s32*)-1) ptr = NULL;
		else if (is_create == 1) {
			memset((void*)ptr, 0, shmsize);
			ptr[0] = SemInit(shmkey, 1);
		}

		if (ptr) {
			ret = (LightingConfig*)malloc(sizeof(LightingConfig));
			ret->semid = ptr[0];
			ret->lighting = (LightingConfig_t*)&(ptr[1]);
			ret->dev = (TaraLightingDevice*)&(ret->lighting[1]);
		}
	}

	return ret;
}

void Tara_lighting_detach(LightingConfig *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

/*
 * get current TaraLightingConfig
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_getConfig(TaraLightingConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(ptr, &pshm->lighting->config, sizeof(TaraLightingConfig));
		LeaveCritical(pshm->semid, 0);
	}

	return 0;
}

/*
 * set cool white level.
 * level: configuration to set, the valid value is 0~100, 0 is to turn OFF the cool white.
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_setCoolWhiteLevel(u32 level)
{
	if (level < TARA_LIGHTING_LEVEL_MIN || level > TARA_LIGHTING_LEVEL_MAX) 
		return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(&pshm->lighting->config.cool.level, &level, sizeof(pshm->lighting->config.cool.level));
		pshm->lighting->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}

/*
 * set cool white level and frequency.
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_setCoolWhite(TaraLightingConfig_t *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (ptr->freq < TARA_LIGHTING_FREQ_MIN || ptr->freq > TARA_LIGHTING_FREQ_MAX)
		return ERROR_INVALARG;
	if (ptr->level < TARA_LIGHTING_LEVEL_MIN || ptr->level > TARA_LIGHTING_LEVEL_MAX)
		return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(&pshm->lighting->config.cool, ptr, sizeof(pshm->lighting->config.cool));
		pshm->lighting->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;	
}

/*
 * set warm white level.
 * level: configuration to set, the valid value is 0~100, 0 is to turn OFF the cool white.
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_setWarmWhiteLevel(u32 level)
{
	if (level < TARA_LIGHTING_LEVEL_MIN || level > TARA_LIGHTING_LEVEL_MAX) 
		return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(&pshm->lighting->config.warm.level, &level, sizeof(pshm->lighting->config.warm.level));
		pshm->lighting->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}

/*
 * set warm white level and frequency.
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_setWarmWhite(TaraLightingConfig_t *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (ptr->freq < TARA_LIGHTING_FREQ_MIN || ptr->freq > TARA_LIGHTING_FREQ_MAX)
		return ERROR_INVALARG;
	if (ptr->level < TARA_LIGHTING_LEVEL_MIN || ptr->level > TARA_LIGHTING_LEVEL_MAX)
		return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(&pshm->lighting->config.warm, ptr, sizeof(pshm->lighting->config.warm));
		pshm->lighting->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;	
}

/*
 * get PWM device
 * ptr: filled with current pwm device status
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_lighting_getPWMDevice(TaraLightingDevice *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_lighting_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(ptr, pshm->dev, sizeof(TaraLightingDevice));
		LeaveCritical(pshm->semid, 0);
	}
	return 0;	
}
