#ifndef _TARA_VIDEO_FRAME_H_
#define _TARA_VIDEO_FRAME_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/time.h>
#include <limits.h>
#include "myglobal.h"
#include "myerr.h"

#define VIDEO_NEXT_FRAME                    0        // get coming frame
#define VIDEO_NEXT_KEY_FRAME                1        // get coming key-frame
#define VIDEO_LAST_FRAME                    2        // get last frame

typedef enum {
	TARA_VIDEO_ENCODER_STREAM=0,        // ==> video encoder
	TARA_VIDEO_JPEG_STREAM=1,           // ==> mjpeg encoder
	TARA_VIDEO_RAW_STREAM=2,            // ==> raw data
	TARA_VIDEO_STREAM_TYPE_NUM=3
} TARA_VIDEO_STREAM_TYPE;

typedef enum {
	TARA_VIDEO_ENCODER_STREAM_1=0,      // video encoder 1
	TARA_VIDEO_ENCODER_STREAM_2=1,      // video encoder 2
	//TARA_VIDEO_ENCODER_STREAM_3=2,      // video encoder 3
	TARA_VIDEO_ENCODER_STREAM_NUM
} TARA_VIDEO_ENCODER_STREAM_TYPE;

typedef enum {
	TARA_VIDEO_JPEG_STREAM_1=0,         // mjpeg encoder 1
	//TARA_VIDEO_JPEG_STREAM_2=1,         // mjpeg encoder 2
	TARA_VIDEO_JPEG_STREAM_NUM
} TARA_VIDEO_JPEG_STREAM_TYPE;

typedef enum {
	TARA_VIDEO_RAW_STREAM_1=0,          // raw data
	TARA_VIDEO_RAW_STREAM_NUM
} TARA_VIDEO_RAW_STREAM_TYPE;

#define MAX_VIDEO_STREAM_NUM            (TARA_VIDEO_ENCODER_STREAM_NUM+TARA_VIDEO_JPEG_STREAM_NUM+TARA_VIDEO_RAW_STREAM_NUM)

typedef enum {
	TARA_VIDEO_ENCODE_ID_NONE=0,
	TARA_VIDEO_ENCODE_ID_JPEG=1,
	TARA_VIDEO_ENCODE_ID_H264=2,
	TARA_VIDEO_ENCODE_ID_H265=3,
	TARA_VIDEO_ENCODE_ID_NUM
} TARA_VIDEO_ENCODE_ID;

typedef enum {
	TARA_VIDEO_FRAME_TYPE_NONE=0,     // Undefined
	TARA_VIDEO_FRAME_TYPE_I=1,        // I frame ( Key frame )
	TARA_VIDEO_FRAME_TYPE_P=2         // P frame ( non-Key frame )
} TARA_VIDEO_FRAME_TYPE;

typedef enum {
	TARA_VIDEO_GETPREVFRAME=0,
	TARA_VIDEO_GETNEXTFRAME=1
} TARA_VIDEO_COMMAND;

typedef struct {
	u32 framesize;                 // size of this frame
	u32 seq_no;                    // sequence number of this frame
	u32 latest_seq_no;             // the latest sequence number of frame in video pool
	u32 width;                     // video width
	u32 height;                    // video height
	u32 capture_ts_sec;            // the captured timestamp of this frame ( Seconds )
	u32 capture_ts_usec;           // the captured timestamp of this frame ( Microseconds )
	u32 encode_ts_sec;             // the encoded timestamp of this frame ( Seconds )
	u32 encode_ts_usec;            // the encoded timestamp of this frame ( Microseconds )
	TARA_VIDEO_ENCODE_ID ctype;    // encoder type of this frame ( TARA_VIDEO_ENCODE_TYPE )
	TARA_VIDEO_FRAME_TYPE ftype;   // frame type ( TARA_VIDEO_FRAME_TYPE )
	u8 metadata[4];
	u32 reserved[16];
} VideoFrameInfo;

typedef struct {
	VideoFrameInfo info;           // frame info
	s8 is_end;                     // to indicate if it is the last element while releasing the memory
	s8 *data;                      // encoded frame data
} VideoFrame;

typedef struct {
	int num;                       // frame number in current GOP
	VideoFrame *frame;             // frame data
	void *next;                    // pointer to next GOP if available
} VideoGroup;

struct video_handle_struct;
typedef struct video_handle_struct video_handle_t;

struct video_handle_struct {
	u32 channel_idx;
	void *privdata;

	/* raw stream */
	/*
	 * get the coming video raw frame
	 * frame: where the video raw frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_coming_raw_frame)(video_handle_t *handle, VideoFrame *frame);

	/*
	 * get the last video raw frame
	 * frame: where the video raw frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_last_raw_frame)(video_handle_t *handle, VideoFrame *frame);

	/* jpeg stream */
	/*
	 * get the coming jpeg frame
	 * frame: where the jpeg frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_coming_jpeg_frame)(video_handle_t *handle, VideoFrame *frame);

	/*
	 * get the last jpeg frame
	 * frame: where the jpeg frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_last_jpeg_frame)(video_handle_t *handle, VideoFrame *frame);

	/*
	 * get jpeg frame by timestamp
	 * tv: timeval structure
	 * command: TARA_VIDEO_GETPREVFRAME / TARA_VIDEO_GETNEXTFRAME
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the jpeg frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_jpeg_frame_by_time)(video_handle_t *handle, struct timeval tv, TARA_VIDEO_COMMAND command, struct timeval *to, VideoFrame *frame);

	/*
	 * get jpeg frames by timestamp
	 * tv: timeval structure
	 * command: TARA_VIDEO_GETPREVFRAME / TARA_VIDEO_GETNEXTFRAME
	 * num: the number of jpeg frames to get
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the jpeg frames will be stored
	 * If the operation succeeds, the number of video frames received is returned. On error, a negative value is returned.
	 */
	s32 (*get_jpeg_frames_by_time)(video_handle_t *handle, struct timeval tv, TARA_VIDEO_COMMAND command, s32 num, struct timeval *to, VideoFrame **frames);

	/* video stream */
	/*
	 * get the coming video frame
	 * stream: TARA_VIDEO_ENCODER_STREAM_1, TARA_VIDEO_ENCODER_STREAM_2 or TARA_VIDEO_ENCODER_STREAM_3
	 * frametype: TARA_VIDEO_FRAME_TYPE_NONE or TARA_VIDEO_FRAME_TYPE_I
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the video frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_coming_video_frame)(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream, s32 frametype, struct timeval *to, VideoFrame *frame);

	/* !!! To be removed !!!
	 * get the last video frame
	 * stream: TARA_VIDEO_ENCODER_STREAM_1, TARA_VIDEO_ENCODER_STREAM_2 or TARA_VIDEO_ENCODER_STREAM_3
	 * type: TARA_VIDEO_FRAME_TYPE_I
	 * frame: where the video frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	//s32 (*get_last_video_frame)(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM stream, s32 frametype, VideoFrame *frame);

	/*
	 * get the video frame by sequence number
	 * stream: TARA_VIDEO_ENCODER_STREAM_1, TARA_VIDEO_ENCODER_STREAM_2 or TARA_VIDEO_ENCODER_STREAM_3
	 * seqno: the specified sequence number of the video frame to get
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the video frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_video_frame_by_seqno)(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream, u32 seqno, struct timeval *to, VideoFrame *frame);

	/*
	 * get the video frames by timestamp
	 *     the first frame will be the I-Frame whose timestamp is before the specified start timestamp;
	 *     if there is no I-Frame whose timestamp is before the specified start timestamp,
	 *     will get the first I-Frame whose timestamp is after the specified start timestamp
	 * stream: TARA_VIDEO_ENCODER_STREAM_1, TARA_VIDEO_ENCODER_STREAM_2 or TARA_VIDEO_ENCODER_STREAM_3
	 * start: the timeval structure of the specified start time
	 * end: the timeval structure of the specified end time
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the video frames will be stored
	 * If the operation succeeds, the number of video frames received is returned. On error, a negative value is returned.
	 */
	s32 (*get_video_frames_by_time)(video_handle_t *handle, TARA_VIDEO_ENCODER_STREAM_TYPE stream, struct timeval start, struct timeval end, struct timeval *to, VideoFrame **frames);

	/**/
	/*
	 * get the coming jpeg frame
	 * the wrapper function of get_coming_jpeg_frame
     * stream_type: support TARA_VIDEO_JPEG_STREAM only
     * stream: support TARA_VIDEO_JPEG_STREAM_1 only
	 * frame: where the frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_coming_frame)(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, VideoFrame *frame);

	/*
	 * get the last jpeg frame
	 * the wrapper function of get_last_jpeg_frame
     * stream_type: support TARA_VIDEO_JPEG_STREAM only
     * stream: support TARA_VIDEO_JPEG_STREAM_1 only
	 * frame: where the frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_last_frame)(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, VideoFrame *frame);

	/*
	 * get jpeg frame by timestamp
	 * the wrapper function of get_jpeg_frame_by_time
	 * tv: timeval structure
	 * stream_type: support TARA_VIDEO_JPEG_STREAM only
	 * stream: support TARA_VIDEO_JPEG_STREAM_1 only
	 * command: TARA_VIDEO_GETPREVFRAME / TARA_VIDEO_GETNEXTFRAME
	 * to: the timeval structure to specify the timeout to wait, ignored if NULL.
	 * frame: where the jpeg frame will be stored
	 * If the operation succeeds, zero is returned. On error, a negative value is returned.
	 */
	s32 (*get_frame_by_time)(video_handle_t *handle, struct timeval tv, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream, TARA_VIDEO_COMMAND command, struct timeval *to, VideoFrame *frame);

};

#ifdef CPLUSPLUS
extern "C" {
#endif

// start the stream with the configured Image Settings in config file
extern s32 Tara_video_start(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream);

// stop the stream
extern s32 Tara_video_stop(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type, s32 stream);

// Only attach when stream already started
extern video_handle_t *Tara_video_attach(u32 channel);

// detach motion_cb and all related methods
extern s32 Tara_video_detach(video_handle_t *vh);

// release resource
extern s32 Tara_video_free_video_frame(VideoFrame *frame);

// release resource
extern s32 Tara_video_free_video_frames(VideoFrame **frames);

#ifdef CPLUSPLUS
}
#endif
#endif
