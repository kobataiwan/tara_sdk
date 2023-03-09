#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_video_source.h"
#include "tara_video_source_priv.h"

//u32 num_channel=8;
static VideoSource *pshm=NULL;

VideoSource* Tara_video_source_attach(s32 flags)
{
	VideoSource *ret=NULL;
	if (num_channel <=0 ) num_channel = 8;

	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey=VIDEO_SOURCE_KEY;
	shmsize = sizeof(s32) + sizeof(videosource_t) * num_channel;
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
			ret = (VideoSource*)malloc(sizeof(VideoSource));
			ret->semid = ptr[0];
			ret->vsrc = (videosource_t*)&(ptr[1]);
		}

		//ret = (VideoSource*)shmat(shmid, 0, 0);
		//if (ret == (VideoSource*)-1) ret = NULL;
		//else if (is_create == 1) memset((void*)ret, 0, shmsize);
	}

	return ret;
}

void Tara_video_source_detach(VideoSource *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

s32 Tara_video_source_setVideoInputControl(s32 num, s32 on)
{
	return 0;
}

s32 Tara_video_source_getVideoInputControl(s32 num, s32 *on)
{
	return 0;
}

/*
 * get the status of video channel
 * return: on error negative value is returned; on success returns 0
 * chan: channel index, zero-based
 * pvs: where video channel status will be stored
 */
s32 Tara_video_source_getStatus(s32 chan, TaraVideoSource *pvs)
{
	if (!pvs) return ERROR_INVALARG;
	if (!pshm) pshm = Tara_video_source_attach(O_RDWR);
	if (pshm) {
		//pvs->type = pshm[chan].type;
		//pvs->width = pshm[chan].width;
		//pvs->height = pshm[chan].height;
		//pvs->framerate = pshm[chan].framerate;

		EnterCritical(pshm->semid, 0);
		pvs->type = pshm->vsrc[chan].type;
		pvs->width = pshm->vsrc[chan].width;
		pvs->height = pshm->vsrc[chan].height;
		pvs->framerate = pshm->vsrc[chan].framerate;
		LeaveCritical(pshm->semid, 0);
	}

	return 0;
}
