#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_video_encoder.h"
#include "tara_video_encoder_priv.h"

//u32 num_channel=8;
static VideoEncoder *pshmv1=NULL;
static VideoEncoder *pshmv2=NULL;
static MjpegEncoder *pshmm1=NULL;
static TaraVideoEncoder *v1ptr=NULL;
static TaraVideoEncoder *v2ptr=NULL;
static TaraMjpegEncoder *mj1ptr=NULL;


void* Tara_video_encoder_attach(s32 type, s32 flags)
{
	void *ret=NULL;
	if (num_channel <=0 ) num_channel = 8;

	s32 *ptr=NULL;
	s32 shmid;
	s32 shmsize=0, is_create=0;
	key_t shmkey;
	if (type==Tara_Video_Encoder_1 || type==Tara_Video_Encoder_2) {
		shmkey = (type==Tara_Video_Encoder_1) ? VIDEO1_ENCODER_KEY : VIDEO2_ENCODER_KEY;
		shmsize = sizeof(s32) + sizeof(VideoEncoder_t) * num_channel;
	} else if (type == Tara_Mjpeg_Encoder) {
		shmkey = MJPEG1_ENCODER_KEY;
		shmsize = sizeof(s32) + sizeof(MjpegEncoder) * num_channel;
	} else {
		return ret;
	}

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
			if (type == Tara_Mjpeg_Encoder) {
				ret = (MjpegEncoder*)malloc(sizeof(MjpegEncoder));
				((MjpegEncoder*)ret)->semid = ptr[0];
				((MjpegEncoder*)ret)->enc = (MjpegEncoder_t*)&(ptr[1]);
			} else {
				ret = (VideoEncoder*)malloc(sizeof(VideoEncoder));
				((VideoEncoder*)ret)->semid = ptr[0];
				((VideoEncoder*)ret)->enc = (VideoEncoder_t*)&(ptr[1]);
			}
		}
	}

	return ret;
}

void Tara_video_encoder_detach(void *ptr)
{
	if (ptr != NULL) shmdt(ptr);
}

s32 Tara_venc_getVideoEncoder(s32 type, s32 chan, void *pve)
{
	if (type<Tara_Video_Encoder_1 || type>Tara_Mjpeg_Encoder) return ERROR_INVALARG;
	if (!pve) return ERROR_INVALARG;
	if (chan > num_channel) return ERROR_INVALARG;

	if (type == Tara_Video_Encoder_1) {
		if (!pshmv1) pshmv1 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_1, O_RDWR);
		if (pshmv1) {
			EnterCritical(pshmv1->semid, 0);
			memcpy(pve, &pshmv1->enc[chan].venc, sizeof(TaraVideoEncoder));
			LeaveCritical(pshmv1->semid, 0);
		}
	} else if (type == Tara_Video_Encoder_2) {
		if (!pshmv2) pshmv2 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_2, O_RDWR);
		if (pshmv2) {
			EnterCritical(pshmv2->semid, 0);
			memcpy(pve, &pshmv2->enc[chan].venc, sizeof(TaraVideoEncoder));
			LeaveCritical(pshmv2->semid, 0);
		}
	} else if (type == Tara_Mjpeg_Encoder) {
		if (!pshmm1) pshmm1 = (MjpegEncoder*)Tara_video_encoder_attach(Tara_Mjpeg_Encoder, O_RDWR);
		if (pshmm1) {
			EnterCritical(pshmm1->semid, 0);
			memcpy(pve, &pshmm1->enc[chan].mjenc, sizeof(TaraMjpegEncoder));
			LeaveCritical(pshmm1->semid, 0);
		}
	}

	return ERROR_NONE;
}

s32 Tara_venc_setVideoEncoder(s32 type, s32 chan, void *pve)
{
	if (type<Tara_Video_Encoder_1 || type>Tara_Mjpeg_Encoder) return ERROR_INVALARG;
	if (!pve) return ERROR_INVALARG;
	if (chan > num_channel) return ERROR_INVALARG;

	if (type == Tara_Video_Encoder_1) {
		if (!pshmv1) pshmv1 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_1, O_RDWR);
		if (pshmv1) {
			EnterCritical(pshmv1->semid, 0);
			memcpy(&pshmv1->enc[chan].venc, pve, sizeof(TaraVideoEncoder));
			pshmv1->enc[chan].needsupdate++;
			LeaveCritical(pshmv1->semid, 0);
		}
	} else if (type == Tara_Video_Encoder_2) {
		if (!pshmv2) pshmv2 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_2, O_RDWR);
		if (pshmv2) {
			EnterCritical(pshmv2->semid, 0);
			memcpy(&pshmv2->enc[chan].venc, pve, sizeof(TaraVideoEncoder));
			pshmv2->enc[chan].needsupdate++;
			LeaveCritical(pshmv2->semid, 0);
		}
	} else if (type == Tara_Mjpeg_Encoder) {
		if (!pshmm1) pshmm1 = (MjpegEncoder*)Tara_video_encoder_attach(Tara_Mjpeg_Encoder, O_RDWR);
		if (pshmm1) {
			EnterCritical(pshmm1->semid, 0);
			memcpy(&pshmm1->enc[chan].mjenc, pve, sizeof(TaraMjpegEncoder));
			pshmm1->enc[chan].needsupdate++;
			LeaveCritical(pshmm1->semid, 0);
		}
	}

	return ERROR_NONE;
}

void* Tara_venc_getAllVideoEncoder(s32 type)
{
	if (type<Tara_Video_Encoder_1 || type>Tara_Mjpeg_Encoder) return NULL;

	s32 i;
	if (type == Tara_Video_Encoder_1) {
		if (!v1ptr) v1ptr = malloc(sizeof(TaraVideoEncoder)*num_channel);
		if (!v1ptr) return NULL;
		// attach to shared memory	
		//if (!pshmv1) pshmv1 = Tara_video1_encoder_attach(O_RDWR);
		if (!pshmv1) pshmv1 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_1, O_RDWR);
		if (pshmv1) {
			EnterCritical(pshmv1->semid, 0);
			for (i = 0; i < num_channel; i++)
				memcpy(&v1ptr[i], &pshmv1->enc[i].venc, sizeof(TaraVideoEncoder));
			LeaveCritical(pshmv1->semid, 0);
		}

		return (void*)v1ptr;
	} else if (type == Tara_Video_Encoder_2) {
		if (!v2ptr) v2ptr = malloc(sizeof(TaraVideoEncoder)*num_channel);
		if (!v2ptr) return NULL;
		// attach to shared memory	
		//if (!pshmv2) pshmv2 = Tara_video2_encoder_attach(O_RDWR);
		if (!pshmv2) pshmv2 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_2, O_RDWR);
		if (pshmv2) {
			EnterCritical(pshmv2->semid, 0);
			for (i = 0; i < num_channel; i++)
				memcpy(&v2ptr[i], &pshmv2->enc[i].venc, sizeof(TaraVideoEncoder));
			LeaveCritical(pshmv2->semid, 0);
		}

		return (void*)v2ptr;
	} else if (type == Tara_Mjpeg_Encoder) {
		if (!mj1ptr) mj1ptr = malloc(sizeof(TaraMjpegEncoder)*num_channel);
		if (!mj1ptr) return NULL;
		// attach to shared memory	
		//if (!pshmm1) pshmm1 = Tara_mjpeg1_encoder_attach(O_RDWR);
		if (!pshmm1) pshmm1 = (MjpegEncoder*)Tara_video_encoder_attach(Tara_Mjpeg_Encoder, O_RDWR);
		if (pshmm1) {
			EnterCritical(pshmm1->semid, 0);
			for (i = 0; i < num_channel; i++)
				memcpy(&mj1ptr[i], &pshmm1->enc[i].mjenc, sizeof(TaraMjpegEncoder));
			LeaveCritical(pshmm1->semid, 0);
		}

		return (void*)mj1ptr;
	}

	return NULL;
}

s32 Tara_venc_setAllVideoEncoder(s32 type, void *pve)
{
	if (type<Tara_Video_Encoder_1 || type>Tara_Mjpeg_Encoder) return ERROR_INVALARG;
	if (!pve) return ERROR_INVALARG;

	s32 i;
	if (type == Tara_Video_Encoder_1) {
		if (!pshmv1) pshmv1 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_1, O_RDWR);
		if (pshmv1) {
			EnterCritical(pshmv1->semid, 0);
			for (i = 0; i < num_channel; i++) {
				memcpy(&pshmv1->enc[i].venc, pve, sizeof(TaraVideoEncoder));
				pshmv1->enc[i].needsupdate++;
			}

			LeaveCritical(pshmv1->semid, 0);
		}
	} else if (type == Tara_Video_Encoder_2) {
		if (!pshmv2) pshmv2 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_2, O_RDWR);
		if (pshmv2) {
			EnterCritical(pshmv2->semid, 0);
			for (i = 0; i < num_channel; i++) {
				memcpy(&pshmv2->enc[i].venc, pve, sizeof(TaraVideoEncoder));
				pshmv2->enc[i].needsupdate++;
			}

			LeaveCritical(pshmv2->semid, 0);
		}
	} else if (type == Tara_Mjpeg_Encoder) {
		if (!pshmm1) pshmm1 = (MjpegEncoder*)Tara_video_encoder_attach(Tara_Mjpeg_Encoder, O_RDWR);
		if (pshmm1) {
			EnterCritical(pshmm1->semid, 0);
			for (i = 0; i < num_channel; i++) {
				memcpy(&pshmm1->enc[i].mjenc, pve, sizeof(TaraMjpegEncoder));
				pshmm1->enc[i].needsupdate++;
			}

			LeaveCritical(pshmm1->semid, 0);
		}
	}

	return ERROR_NONE;
}

