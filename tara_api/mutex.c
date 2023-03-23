/*
 * [20200205] - replace semop with semtimedop in EnterCritical to fix lock not released issue.
 *            - if semtimedop timeout, force reset semaphore
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include "mutex.h"
#include "myglobal.h"

// ---------------------------------------------------------------
// Semphore

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

int semtimedop(int  semid, struct sembuf *sops, unsigned nsops, struct timespec *timeout);

void SemRemove(int semid)
{
	int rtn;
	union semun sem_un;

	if (semid != -1) {
		rtn = semctl(semid, 0, IPC_RMID, sem_un);
#ifdef DEBUG
		printf("SemRemove: semid=%d: %s\n", semid, (rtn==0 ? "Ok" : "Error"));
#endif
		tolog(TARA_LOG_INFO, "SemRemove: semid=%d: %s\n", semid, (rtn==0 ? "Ok" : "Error"));
	}
}

void EnterCritical(int semid, int semnum)
{
	struct sembuf sops;
	int rtn;
	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	if (semid == -1) return;
	while (1) {
		sops.sem_num = semnum;
		sops.sem_op = -1;   /* semaphore operation */
		sops.sem_flg = 0;
		//sops.sem_flg = SEM_UNDO;
#if 0
		rtn = semop(semid, &sops, 1);
		if (rtn==-1 && errno==EINTR) printf("EnterCritical: EINTR, semid=%d\n", semid);
		else break;
#else
		rtn = semtimedop(semid, &sops, 1, &ts);
		if (rtn == 0) break;
		else {
			if (rtn==-1 && errno==EAGAIN) {
#ifdef DEBUG
				printf("%s:%d pid:%d\n", __FUNCTION__, __LINE__, (int)getpid());
#endif
				SemReset(semid, 0);
			}
		}
#endif
	}
#if 0//DEBUG
	printf("EnterCritical: semid=%d, semnum=%d, rtn=%d\n", semid, semnum, rtn);
#endif
}

void LeaveCritical(int semid, int semnum)
{
	struct sembuf sops;
	if (semid == -1) return;
	sops.sem_num = semnum;
	sops.sem_op = 1;   /* semaphore operation */
	sops.sem_flg = 0;
	//sops.sem_flg = SEM_UNDO;
	semop(semid, &sops, 1);
#if 0//DEBUG
	printf("LeaveCritical: semid=%d, semnum=%d, rtn=%d\n", semid, semnum, rtn);
#endif
}

void SemReset(int semid, int semnum)
{
	union semun sem_un;
	int rtn;
	sem_un.val = 1;
	rtn = semctl(semid, semnum, SETVAL, sem_un);
	tolog(TARA_LOG_INFO, "SemReset: semid=%d return %d\n", semid, rtn);
}

int SemGetVal(int semid, int semnum)
{
	union semun sem_un;
	int rtn;
	rtn = semctl(semid, semnum, GETVAL, sem_un);
	return rtn;
}

int SemInit(int semkey, int nsems)
{
	struct sembuf sops;
	int rtn, semnum, semid=-1, is_create=0;
	semid = semget (semkey, nsems, IPC_CREAT|IPC_EXCL|0600);
	if (semid != -1) is_create = 1;
	if (semid==-1 && errno==EEXIST) semid = semget(semkey, nsems, 0600);
	if (is_create) {
		for (semnum=0; semnum<nsems; semnum++) {
			sops.sem_num = semnum;
			sops.sem_op = 1;   /* semaphore operation */
			sops.sem_flg = 0;
			//sops.sem_flg = SEM_UNDO;
			rtn = semop(semid, &sops, 1);
			if (rtn == -1) {
				SemRemove(semkey);
				semid = -1;
				break; 
			}
		}
	}

	return semid;
}
