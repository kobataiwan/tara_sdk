#ifndef _VQUEUE_H_
#define _VQUEUE_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/time.h>
#include "myglobal.h"
#include "tara_video_frame.h"

#define TIMEOUT_INFINITE            UINT_MAX

#pragma pack(1)

typedef struct {
	u32 seqno;
	s32 dirid;
	s32 fileid;
	s32 pos;
	s32 size;
	s32 codec;
	s32 type;
	s32 width;
	s32 height;
	u32 sps;         // size of SPS
	u32 pps;         // size of PPS
	u32 sei;         // size of SEI
	u32 vps;         // size of VPS
	u64 timestamp;   // encoded timestamp in micro seconds
	u64 ctimestamp;  // capture timestamp in micro seconds
} VFrameInfo;


#ifdef CPLUSPLUS
extern "C" {
#endif

extern void *VQueueGetPtr(key_t shm_key, s32 qsize);
extern s32 VQueueIsEmpty(void *vq);
extern s32 VQueueIsFull(void *vq);
extern s32 VQueueInsert(void *vq, void *buf);
extern s32 VQueueRemove(void *vq);

// return: 0:ok, 1:queue empty, 2:no match, <0:error
// *seq_no: in and out
// for flag
#define VQ_MATCH_IFR   1
#define VQ_MATCH_SEQNO 2
extern s32 VQueueRead(void *vq, void *buf, s32 *bufsize, u32 *seq_no, u32 cmd, u32 to);
extern void VQueueClean(void *vq);

extern s32 VQueueReadGOP(void *vq, void *gop, u32 cmd, u32 to);
extern s32 VQueueReadGOPs(void *vq, void **gop, u32 cmd, u32 to, u32 period);

extern s32 VQueueReadFramesByTime(void *vq, void **frames, u32 cmd, s64 ts, s32 num, u32 to);

extern s32 VQueueReadFrameBySeq(void *vq, void *frame, u32 seq, u32 to);

extern s32 VQueueReadFramesByTimeRange(void *vq, void **frames, s64 start, s64 end, u32 to);
#ifdef CPLUSPLUS
}
#endif
#endif
