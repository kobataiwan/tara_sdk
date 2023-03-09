#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/time.h>

#ifdef CPLUSPLUS
extern "C" {
#endif

extern int SemInit(int semkey, int nsems);
extern void SemReset(int semid, int semnum);
extern int SemGetVal(int semid, int semnum);
extern void EnterCritical(int semid, int semnum);
extern void LeaveCritical(int semid, int semnum);
extern void SemRemove(int semid);

#ifdef CPLUSPLUS
}
#endif
#endif
