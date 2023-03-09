#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_persistence.h"
#include "tara_persistence_priv.h"

static persistence_data *pshm=NULL;
static TaraPersistenceData *puser=NULL;

persistence_data* Tara_persistence_attach(s32 flags)
{
	persistence_data *ret=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey=PERSISTENCE_KEY;
	shmsize = sizeof(persistence_data);
	if ((flags&O_RDWR) != 0) {
		shmid = shmget(shmkey, shmsize, IPC_CREAT|IPC_EXCL|0666);
		if (shmid != -1) is_create = 1;
		if (shmid==-1 && errno==EEXIST) shmid = shmget(shmkey, shmsize, 0666);
	} else shmid = shmget(shmkey, shmsize, 0444);

	if (shmid == -1) errno = ERROR_FAIL;
	else {
		ret = (persistence_data*)shmat(shmid, 0, 0);
		if (ret == (persistence_data*)-1) ret = NULL;
		else if (is_create == 1) {
			memset((void*)ret, 0, shmsize);
			ret->semid = SemInit(shmkey, 1);
		}
	}

	return ret;
}

void Tara_persistence_detach(persistence_data *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

TaraPersistenceData* Tara_persistence_getData()
{
	if (!puser) puser = malloc(sizeof(TaraPersistenceData));
	if (!puser) return NULL;
	// attach to shared memory
	if (!pshm) pshm = Tara_persistence_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		s32 i=0;
		boot_env_256 *p256;
		memcpy(puser->data2048, pshm->data2048.data, sizeof((pshm->data2048).data));
		for (i=0; i<PERSISTENCE_256_NUM; i++) {
			p256 = &(pshm->data256[i]);
			memcpy(puser->data256[i], p256->data, sizeof(p256->data));
		}
////		boot_env_2048 *data2048=&(pshm->data2048);
//		puser->data2048 = (pshm->data2048).data;
//		for (i=0; i<PERSISTENCE_256_NUM; i++) {
//			p256 = &(pshm->data256[i]);
//			puser->data256[i] = p256->data;
//		}

		LeaveCritical(pshm->semid, 0);
		return puser;
	} else return NULL;
}

s32 Tara_persistence_setData256(s32 id, s8 *data)
{
	if (!data) return ERROR_INVALARG;
	if (id < 0 || id >= PERSISTENCE_256_NUM) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_persistence_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		boot_env_256 *p256;
		s32 len;
		p256 = &(pshm->data256[id]);
		len = strlen(data);
		if (len > sizeof(p256->data)) len = sizeof(p256->data);
		memset(p256->data, 0, sizeof(p256->data));
		memcpy(p256->data, data, sizeof(s8)*len);
		p256->needupdate++;
		LeaveCritical(pshm->semid, 0);
	}

	return ERROR_NONE;
}

s32 Tara_persistence_setData2048(s8 *data)
{
	if (!data) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_persistence_attach(O_RDWR);
	if (pshm) {
		EnterCritical(pshm->semid, 0);
		boot_env_2048 *p2048;
		s32 len;
		p2048 = &(pshm->data2048);
		len = strlen(data);
		if (len > sizeof(p2048->data)) len = sizeof(p2048->data);
		memset(p2048->data, 0, sizeof(p2048->data));
		memcpy(p2048->data, data, sizeof(s8)*len);
		p2048->needupdate++;
		LeaveCritical(pshm->semid, 0);
	}

	return ERROR_NONE;
}
