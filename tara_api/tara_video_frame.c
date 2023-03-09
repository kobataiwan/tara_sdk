#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "shmkey.h"
#include "vqueue.h"
#include "mutex.h"
#include "tara_video_source_priv.h"
#include "tara_log.h"


#define TARA_SDK_MAKE_VERSION(a,b,c)           ((((a)&0xff)<<16) | (((b)&0xff)<<8) | ((c)&0xff))
#define TARA_VIDEO_SDK_VERSION_MAJOR(a)        ((s8)(((a)>>16) & 0xff))
#define TARA_VIDEO_SDK_VERSION_MINOR(a)        ((s8)(((a)>> 8) & 0xff))
#define TARA_VIDEO_SDK_VERSION_PATCH(a)        ((s8)(((a)>> 0) & 0xff))

#define TARA_VIDEO_MAKE_API(a,b)               ((((a)&0xff)<<16) | (((b)&0xff)<<0))
#define TARA_VIDEO_API_MAJOR(a)                (((a)>>16) & 0xff)
#define TARA_VIDEO_API_MINOR(a)                (((a)>> 0) & 0xff)

#define TARA_SDK_VERSION                       TARA_SDK_MAKE_VERSION(2,1,0)
#define TARA_VIDEO_API                         TARA_VIDEO_MAKE_API(2, 0)

#define DEF_QUEUE_SIZE        300
#define DEF_RAWQUEUE_SIZE     1


typedef struct {
	u32 seq_no;
	void *queue;
	u32 reserved[16];
} VideoStream;

typedef struct {
	VideoStream video[TARA_VIDEO_ENCODER_STREAM_NUM];
	VideoStream jpeg[TARA_VIDEO_JPEG_STREAM_NUM];
	VideoStream raw[TARA_VIDEO_RAW_STREAM_NUM];
	u32 reserved[16];
} VideoPool;

typedef struct {
	VideoFrameInfo info;
	s8 *data;
} VideoFrameInternal;

static s32 initialized=0;
static s32 is_first=1;
static s32 chan_stat[MAX_CHANNEL_NUM][MAX_VIDEO_STREAM_NUM];
static VideoSource *vsptr=NULL;

static s32 get_coming_video_frame(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream,
                                  s32 frametype, struct timeval *to, VideoFrame *frame);
static s32 get_jpeg_frame_by_time(video_handle_t *handle, struct timeval tv,
                                  TARA_VIDEO_COMMAND command,
                                  struct timeval *to, VideoFrame *frame);

static void private_exit()
{
	Tara_video_source_detach(vsptr);
}

static s32 check_channel_stream(u32 channel, TARA_VIDEO_STREAM_TYPE stype)
{
	return 0;
}

static s32 _get_frame(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, s32 cmd, u32 to, VideoFrame *frame)
{
	s32 size=0;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	if (!handle || !frame || (cmd!=VIDEO_NEXT_FRAME && cmd!=VIDEO_LAST_FRAME)) return ERROR_INVALARG;

	vpool = (VideoPool*)handle->privdata;
	if (stream_type == TARA_VIDEO_JPEG_STREAM) {
		strm = &(vpool->jpeg[TARA_VIDEO_JPEG_STREAM_1]);
	} else if (stream_type == TARA_VIDEO_RAW_STREAM) {
		strm = &(vpool->raw[TARA_VIDEO_RAW_STREAM_1]);
	}

	if (strm != NULL) {
		vq = strm->queue;
		return VQueueRead(vq, (void*)frame, &size, &strm->seq_no, cmd, to);
	}

	return ERROR_FAIL;
}

static s32 get_coming_frame(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, VideoFrame *frame)
{
	if (stream_type == TARA_VIDEO_JPEG_STREAM)
		return _get_frame(handle, stream_type, stream, VIDEO_NEXT_FRAME, TIMEOUT_INFINITE, frame);
	else return ERROR_UNSUPPORTED;
}

static s32 get_last_frame(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, VideoFrame *frame)
{
	if (stream_type == TARA_VIDEO_JPEG_STREAM)
		return _get_frame(handle, stream_type, stream, VIDEO_LAST_FRAME, TIMEOUT_INFINITE, frame);
	else return ERROR_UNSUPPORTED;
}

static s32 get_frame_by_time(video_handle_t *handle, struct timeval tv, TARA_VIDEO_STREAM_TYPE stream_type,
                             s32 stream, TARA_VIDEO_COMMAND command, struct timeval *to, VideoFrame *frame)
{
	if (stream_type == TARA_VIDEO_JPEG_STREAM)
		return get_jpeg_frame_by_time(handle, tv, command, to, frame);
	else return ERROR_UNSUPPORTED;
}

static s32 get_coming_raw_frame(video_handle_t *handle, VideoFrame *frame)
{
	return _get_frame(handle, TARA_VIDEO_RAW_STREAM, TARA_VIDEO_RAW_STREAM_1, VIDEO_NEXT_FRAME, TIMEOUT_INFINITE, frame);
}

static s32 get_last_raw_frame(video_handle_t *handle, VideoFrame *frame)
{
	return _get_frame(handle, TARA_VIDEO_RAW_STREAM, TARA_VIDEO_RAW_STREAM_1, VIDEO_LAST_FRAME, TIMEOUT_INFINITE, frame);
}

static s32 get_coming_jpeg_frame(video_handle_t *handle, VideoFrame *frame)
{
	return _get_frame(handle, TARA_VIDEO_JPEG_STREAM, TARA_VIDEO_JPEG_STREAM_1, VIDEO_NEXT_FRAME, TIMEOUT_INFINITE, frame);
}

static s32 get_last_jpeg_frame(video_handle_t *handle, VideoFrame *frame)
{
	return _get_frame(handle, TARA_VIDEO_JPEG_STREAM, TARA_VIDEO_JPEG_STREAM_1, VIDEO_LAST_FRAME, TIMEOUT_INFINITE, frame);
}

static s32 get_jpeg_frame_by_time(video_handle_t *handle, struct timeval tv,
                                  TARA_VIDEO_COMMAND command,
                                  struct timeval *to, VideoFrame *frame)
{
	s32 ret=0;
	u32 tots=TIMEOUT_INFINITE;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	s64 ts=0;
	if (!handle || !frame) return ERROR_INVALARG;
	vpool = (VideoPool*)handle->privdata;
	strm = &(vpool->jpeg[TARA_VIDEO_JPEG_STREAM_1]);
	vq = strm->queue;
	ts = (s64)(tv.tv_sec) * 1000000L + (s64)tv.tv_usec;
	if (to != NULL) tots = to->tv_sec * 1000000 + to->tv_usec;
	ret = VQueueReadFramesByTime(vq, (void**)&frame, command, ts, 0, tots);
	if (ret != 0) {
		struct timeval systv;
		gettimeofday(&systv, NULL);
		Tara_log(TARA_LOG_ERROR, "Error: %s return %d, command %d, requested ts: %d.%d, system ts: %d.%d\n",
		        __FUNCTION__, ret, command, (s32)tv.tv_sec, (s32)tv.tv_usec, (s32)systv.tv_sec, (s32)systv.tv_usec);
	}

	return ret;
}

static s32 get_jpeg_frames_by_time(video_handle_t *handle, struct timeval tv,
                                   TARA_VIDEO_COMMAND command, s32 num,
                                   struct timeval *to, VideoFrame **frames)
{
	s32 ret=0;
	u32 tots=TIMEOUT_INFINITE;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	s64 ts=0;
	if (!handle || num<=0) return ERROR_INVALARG;
	if (*frames) {
		VideoFrame *pframes=(*frames);
		VideoFrame *ptr=&pframes[0];
		do {
			if (ptr->data) {
				free(ptr->data);
				ptr->data = NULL;
			}

			if (ptr->is_end == 1) break;
			ptr++;
		} while (1);

		free(*frames);
		*frames = NULL;
	}

	vpool = (VideoPool*)handle->privdata;
	strm = &(vpool->jpeg[TARA_VIDEO_JPEG_STREAM_1]);
	vq = strm->queue;
	ts = (s64)(tv.tv_sec) * 1000000L + (s64)tv.tv_usec;
	if (to != NULL) tots = to->tv_sec * 1000000 + to->tv_usec;
	ret = VQueueReadFramesByTime(vq, (void**)frames, command, ts, num, tots);
	return ret;
}

static s32 get_coming_video_frame(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream,
                                  s32 frametype, struct timeval *to, VideoFrame *frame)
{
	s32 cmd=VIDEO_NEXT_FRAME;
	u32 tots=TIMEOUT_INFINITE;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	s32 size=0;
	if (!handle || !frame) return ERROR_INVALARG;
	vpool = (VideoPool*)handle->privdata;
	if (stream == TARA_VIDEO_ENCODER_STREAM_2) strm = &(vpool->video[TARA_VIDEO_ENCODER_STREAM_2]);
	else strm = &(vpool->video[TARA_VIDEO_ENCODER_STREAM_1]);

	if (strm != NULL) {
		vq = strm->queue;
		if (frametype == TARA_VIDEO_FRAME_TYPE_I) cmd = VIDEO_NEXT_KEY_FRAME;
		if (to != NULL) {
			s64 ts=(s64)(to->tv_sec) * 1000000L + (s64)to->tv_usec;
			if (ts > UINT_MAX) tots = TIMEOUT_INFINITE;
			else tots = (u32)ts;
		}

		return VQueueRead(vq, (void*)frame, &size, &strm->seq_no, cmd, tots);
	}

	return ERROR_FAIL;
}

static s32 get_video_frame_by_seqno(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream,
                                    u32 seqno, struct timeval *to, VideoFrame *frame)
{
	u32 tots=TIMEOUT_INFINITE;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	if (!handle || !frame) return ERROR_INVALARG;
	vpool = (VideoPool*)handle->privdata;
	if (stream == TARA_VIDEO_ENCODER_STREAM_2) strm = &(vpool->video[TARA_VIDEO_ENCODER_STREAM_2]);
	else strm = &(vpool->video[TARA_VIDEO_ENCODER_STREAM_1]);

	if (strm != NULL) {
		vq = strm->queue;
		return VQueueReadFrameBySeq(vq, (void*)frame, seqno, tots);
	}

	return ERROR_FAIL;
}

static s32 get_video_frames_by_time(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream,
                                    struct timeval start, struct timeval end, struct timeval *to, VideoFrame **frames)
{
	u32 tots=TIMEOUT_INFINITE;
	VideoPool *vpool=NULL;
	VideoStream *strm=NULL;
	void *vq=NULL;
	s64 ts=0, start_ts=0, end_ts=0;
	if (!handle || stream<TARA_VIDEO_ENCODER_STREAM_1 || stream>=TARA_VIDEO_ENCODER_STREAM_NUM) return ERROR_INVALARG;
	if (*frames) {
		VideoFrame *pframes=(*frames);
		VideoFrame *ptr=&pframes[0];
		do {
			if (ptr->data) {
				free(ptr->data);
				ptr->data = NULL;
			}

			if (ptr->is_end == 1) break;
			ptr++;
		} while (1);

		free(*frames);
		*frames = NULL;
	}

	vpool = (VideoPool*)handle->privdata;
	strm = &(vpool->video[stream]);
	vq = strm->queue;
	start_ts = (s64)(start.tv_sec) * 1000000L + (s64)start.tv_usec;
	end_ts = (s64)(end.tv_sec) * 1000000L + (s64)end.tv_usec;
	if (to != NULL) {
		ts = (s64)(to->tv_sec) * 1000000L + (s64)to->tv_usec;
		if (ts > UINT_MAX) tots = TIMEOUT_INFINITE;
		else tots = (u32)ts;
	}

	return VQueueReadFramesByTimeRange(vq, (void**)frames, start_ts, end_ts, tots);
}

s32 Tara_video_start(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type, int stream)
{
	if (initialized == 0) {
		initialized = 1;
	}

	if (check_channel_stream(channel, stream_type)) return ERROR_INVALARG;

	// check input type
	vsptr = Tara_video_source_attach(0);
	if (channel > num_channel) return ERROR_INVALARG;
	if (!vsptr) return ERROR_FAIL;
	if (vsptr && vsptr->vsrc[channel].type == 0) return ERROR_NOVIDEO;

	if (chan_stat[channel][stream_type] == 0) {
		chan_stat[channel][stream_type] = 1;
	}

	// register abnormall exit
	atexit(private_exit);

	return ERROR_NONE;
}

s32 Tara_video_stop(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type, int stream)
{
	if (is_first == 1) {
		is_first = 0;
		return ERROR_FAIL;
	}

	if (check_channel_stream(channel, stream_type)) return ERROR_INVALARG;

	if (chan_stat[channel][stream_type] == 0) return ERROR_FAIL;
	chan_stat[channel][stream_type] = 0;
	return ERROR_NONE;
}

video_handle_t *Tara_video_attach(u32 channel)
{
	s32 vqsize=DEF_QUEUE_SIZE;
	s32 vqrawsize=DEF_RAWQUEUE_SIZE;
	video_handle_t *ret=NULL;
	VideoPool *vpool=NULL;
	if (initialized == 0) {
		initialized = 1;
		return NULL;
	}

	ret=(video_handle_t*)malloc(sizeof(video_handle_t));
	if (!ret) goto fail;

	memset(ret, 0, sizeof(video_handle_t));
	vpool=(VideoPool*)malloc(sizeof(VideoPool));
	if (!vpool) goto fail;

	memset(vpool, 0, sizeof(VideoPool));
	vpool->video[TARA_VIDEO_ENCODER_STREAM_1].queue = VQueueGetPtr(VIDEO_ENCODER_STREAM1_KEY(channel), vqsize);
	vpool->video[TARA_VIDEO_ENCODER_STREAM_2].queue = VQueueGetPtr(VIDEO_ENCODER_STREAM2_KEY(channel), vqsize);
	vpool->jpeg[TARA_VIDEO_JPEG_STREAM_1].queue = VQueueGetPtr(VIDEO_JPEG_STREAM1_KEY(channel), vqsize);
	vpool->raw[TARA_VIDEO_RAW_STREAM_1].queue = VQueueGetPtr(VIDEO_RAW_STREAM1_KEY(channel), vqrawsize);

	ret->channel_idx = channel;
	ret->privdata = (void*)vpool;
	ret->get_coming_raw_frame = &get_coming_raw_frame;
	ret->get_last_raw_frame = &get_last_raw_frame;
	ret->get_coming_jpeg_frame = &get_coming_jpeg_frame;
	ret->get_last_jpeg_frame = &get_last_jpeg_frame;
	ret->get_jpeg_frame_by_time = &get_jpeg_frame_by_time;
	ret->get_jpeg_frames_by_time = &get_jpeg_frames_by_time;
	ret->get_coming_video_frame = &get_coming_video_frame;
	ret->get_video_frame_by_seqno = &get_video_frame_by_seqno;
	ret->get_video_frames_by_time = &get_video_frames_by_time;
	ret->get_coming_frame = &get_coming_frame;
	ret->get_last_frame = &get_last_frame;
	ret->get_frame_by_time = &get_frame_by_time;

	goto ok;

fail:
	if (vpool) free(vpool);
	if (ret) free(ret);
	vpool = NULL;
	ret = NULL;

ok:
	return (void*)ret;
}

s32 Tara_video_detach(video_handle_t *vh)
{
	if (!vh) return ERROR_INVALARG;
	VideoPool *vpool=(VideoPool*)vh->privdata;
	free(vpool);
	free(vh);
	return ERROR_NONE;
}

s32 Tara_video_free_video_frame(VideoFrame *frame)
{
	if (!frame) return ERROR_INVALARG;
	if (frame->data) free(frame->data);
	frame->data = NULL;
	return ERROR_NONE;
}

s32 Tara_video_free_video_frames(VideoFrame **frames)
{
	if (*frames) {
		VideoFrame *pframes=(*frames);
		VideoFrame *ptr=&pframes[0];
		do {
			if (ptr->data) {
				free(ptr->data);
				ptr->data = NULL;
			}

			if (ptr->is_end == 1) break;
			ptr++;
		} while (1);

		free(*frames);
		*frames = NULL;
	}

	return ERROR_NONE;
}
