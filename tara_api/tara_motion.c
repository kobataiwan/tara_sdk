#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_motion_priv.h"

static MotionConfig *pshmmd = NULL;
static TaraMotionConfig *mdptr = NULL;

MotionConfig* Tara_motion_attach(s32 flags)
{
	MotionConfig *ret=NULL;
	if (num_channel <=0 ) num_channel = 8;

	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey = MOTION_KEY;
	shmsize = sizeof(s32) + sizeof(MotionConfig_t) * num_channel;

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
			ret = (MotionConfig*)malloc(sizeof(MotionConfig));
			ret->semid = ptr[0];
			ret->md = (MotionConfig_t*)&(ptr[1]);
		}
	}

	return ret;
}

void Tara_motion_detach(MotionConfig *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

s32 Tara_motion_getConfig(s32 chan, TaraMotionConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (chan > num_channel) return ERROR_INVALARG;
	if (!pshmmd) pshmmd = Tara_motion_attach(O_RDWR);
	
	if (pshmmd) {
		EnterCritical(pshmmd->semid, 0);
		memcpy(ptr, &pshmmd->md[chan].config, sizeof(TaraMotionConfig));
		LeaveCritical(pshmmd->semid, 0);
	}
	return ERROR_NONE;
}

s32 Tara_motion_setConfig(s32 chan, TaraMotionConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	if (chan > num_channel) return ERROR_INVALARG;

	if (!pshmmd) pshmmd = Tara_motion_attach(O_RDWR);
	if (pshmmd) {
		EnterCritical(pshmmd->semid, 0);
		memcpy(&pshmmd->md[chan].config, ptr, sizeof(TaraMotionConfig));
		pshmmd->md[chan].needsupdate++;
		LeaveCritical(pshmmd->semid, 0);
	}

	return ERROR_NONE;
}

TaraMotionConfig* Tara_motion_getAll()
{
	s32 i;

	if (!mdptr) mdptr = malloc(sizeof(TaraMotionConfig)*num_channel);
	if (!mdptr) return NULL;
	// attach to shared memory	
	if (!pshmmd) pshmmd = Tara_motion_attach(O_RDWR);
	if (pshmmd) {
		EnterCritical(pshmmd->semid, 0);
		for (i = 0; i < num_channel; i++)
			memcpy(&mdptr[i], &pshmmd->md[i].config, sizeof(TaraMotionConfig));
		LeaveCritical(pshmmd->semid, 0);
	}

	return mdptr;
}

s32 Tara_motion_setAll(TaraMotionConfig *ptr)
{
	s32 i;
	if (!ptr) return ERROR_INVALARG;
	if (!pshmmd) pshmmd = Tara_motion_attach(O_RDWR);
	if (pshmmd) {
		EnterCritical(pshmmd->semid, 0);
		for (i = 0; i < num_channel; i++) {
			memcpy(&pshmmd->md[i].config, ptr, sizeof(TaraMotionConfig));
			pshmmd->md[i].needsupdate++;
		}
		LeaveCritical(pshmmd->semid, 0);
	}
	return ERROR_NONE;
}

