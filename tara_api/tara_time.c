#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_time_priv.h"

static DateTimeConfig *pshm=NULL;

DateTimeConfig* Tara_time_attach(s32 flags)
{
	DateTimeConfig *ret=NULL;
	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey=TIME_KEY;
	shmsize = sizeof(s32) + sizeof(DateTimeConfig_t);
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
			ret = (DateTimeConfig*)malloc(sizeof(DateTimeConfig));
			ret->semid = ptr[0];
			ret->dt = (DateTimeConfig_t*)&(ptr[1]);
		}
	}

	return ret;
}

void Tara_time_detach(DateTimeConfig *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

/*
 * get the status of video channel
 * return: on error negative value is returned; on success returns 0
 * chan: channel index, zero-based
 * pvs: where video channel status will be stored
 */
s32 Tara_time_getConfig(TaraDateTimeConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_time_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(ptr, &pshm->dt->config, sizeof(TaraDateTimeConfig));
		LeaveCritical(pshm->semid, 0);
	}

	return 0;
}

s32 Tara_time_setConfig(TaraDateTimeConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (ptr->type < 0 || ptr->type >= DATETIME_TYPE_MAX) return ERROR_INVALARG;
	
	if (!pshm) pshm = Tara_time_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		memcpy(&pshm->dt->config, ptr, sizeof(pshm->dt->config));
		pshm->dt->needsupdate++;
		LeaveCritical(pshm->semid, 0);
	}
	
	return 0;
}
