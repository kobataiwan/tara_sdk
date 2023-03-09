#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_info.h"
#include "tara_info_priv.h"

static InfoConfig *pshm=NULL;

InfoConfig* Tara_info_attach(s32 flags)
{
	InfoConfig *ret=NULL;
	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey=INFO_KEY;
	shmsize = sizeof(s32) + sizeof(InfoConfig_t);
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
			ret = (InfoConfig*)malloc(sizeof(InfoConfig));
			ret->semid = ptr[0];
			ret->info = (InfoConfig_t*)&(ptr[1]);
		}
	}

	return ret;
}

void Tara_info_detach(InfoConfig *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

/*
 * get the status of video channel
 * return: on error negative value is returned; on success returns 0
 * chan: channel index, zero-based
 * pvs: where video channel status will be stored
 */
s32 Tara_info_getConfig(TaraInfoConfig *pinfo)
{
	if (!pinfo) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_info_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(pinfo, &pshm->info->config, sizeof(TaraInfoConfig));
		LeaveCritical(pshm->semid, 0);
	}

	return 0;
}

s32 Tara_info_setBrand(s8 *brand)
{
	if (!brand) return ERROR_INVALARG;
	if (strlen(brand) > BRAND_NAME_SIZE) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_info_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(pshm->info->config.brand, brand, sizeof(pshm->info->config.brand));
		pshm->info->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}

s32 Tara_info_setSerialNumber(s8 *num)
{
	if (!num) return ERROR_INVALARG;
	if (strlen(num) > SERIAL_NUMBER_SIZE) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_info_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(pshm->info->config.serial_number, num, sizeof(pshm->info->config.serial_number));
		pshm->info->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}

s32 Tara_info_setName(s8 *name)
{
	if (!name) return ERROR_INVALARG;
	if (strlen(name) > NAME_SIZE) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_info_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(pshm->info->config.name, name, sizeof(pshm->info->config.name));
		pshm->info->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}

s32 Tara_info_setLocation(s8 *location)
{
	if (!location) return ERROR_INVALARG;
	if (strlen(location) > LOCATION_SIZE) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_info_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(pshm->info->config.location, location, sizeof(pshm->info->config.location));
		pshm->info->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}
