#ifndef _TARA_MEDIA_STREAM_H_
#define _TARA_MEDIA_STREAM_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/time.h>
#include <limits.h>
#include "myglobal.h"
#include "myerr.h"
#define NEW_IMPLEMENT                         1

#if NEW_IMPLEMENT
#include "tara_video_frame.h"
#else
#endif


#if NEW_IMPLEMENT
#define VIDEO_MAIN_STREAM                   0
#define VIDEO_SUB_STREAM                    1
#define VIDEO_MJPEG_STREAM                  2
#define VIDEO_RAW_STREAM                    3
#define VIDEO_STREAM_NUM                    4

#define	VIDEO_CODEC_NONE						0
#define	VIDEO_CODEC_MJPG						1
#define	VIDEO_CODEC_H264						2
#define	VIDEO_CODEC_HEVC						3
#define	VIDEO_CODEC_NUM							4

#define VIDEO_PREDICT_FRAME                 2         // P-Frame
#define VIDEO_KEY_FRAME                     1         // I-Frame ( Key frame)
#define VIDEO_YUV420_FRAME                  1         // Raw Frame ( YUV data )
#define VIDEO_FRAMETYPE_NUM                 3

#else
typedef enum {
	VIDEO_MAIN_STREAM=0,
	VIDEO_SUB_STREAM=1,
	VIDEO_MJPEG_STREAM=2,
	VIDEO_RAW_STREAM=3,
	VIDEO_STREAM_NUM
} TARA_VIDEO_STREAM_TYPE;

typedef enum {
	VIDEO_CODEC_NONE=0,
	VIDEO_CODEC_H264=1,
	VIDEO_CODEC_HEVC=2,
	VIDEO_CODEC_MJPG=3,
	VIDEO_CODEC_NUM
} TARA_VIDEO_CODEC_TYPE;

typedef enum {
	VIDEO_PREDICT_FRAME=0,         // P-Frame
	VIDEO_KEY_FRAME=1,             // I-Frame ( Key frame)
	VIDEO_YUV420_FRAME=2,          // Raw Frame ( YUV data )
	VIDEO_FRAMETYPE_NUM
} TARA_VIDEO_FRAME_TYPE;
#endif

#if NEW_IMPLEMENT
//#define VIDEO_NEXT_FRAME                    0        // get coming frame ( I or P )
//#define VIDEO_NEXT_KEY_FRAME                1        // get coming key frame
//#define VIDEO_LAST_FRAME                    2        // get last recorded frame ( I or P )
//#define VIDEO_LAST_KEY_FRAME                3        // get last recorded key frame
//#define VIDEO_NEXT_GOP                      4        // get coming GOP
//#define VIDEO_LAST_GOP                      5        // get last recorded GOP
//#define VIDEO_NEXT_GOPS                     6        // get coming GOPs
//#define VIDEO_LAST_GOPS                     7        // get last recorded GOPs
//#define VIDEO_NEXT_FRAMES_TIME              8        // get a specific period of video frames whose timestamps are after the specified timestamp
//#define VIDEO_LAST_FRAMES_TIME              9        // get a specific period of video frames whose timestamps are before the specified timestamp
//#define VIDEO_NEXT_FRAMES_TIMEC             10       // get a number of video frames whose timestamps are after the specified timestamp
//#define VIDEO_LAST_FRAMES_TIMEC             11       // get a number of video frames whose timestamps are before the specified timestamp
//#define VIDEO_COMMAND_MAX                   12
#else
typedef enum {
	VIDEO_NEXT_FRAME=0,            // get coming frame ( I or P )
	VIDEO_NEXT_KEY_FRAME=1,        // get coming key frame
	VIDEO_LAST_FRAME=2,            // get last recorded frame ( I or P )
	VIDEO_LAST_KEY_FRAME=3,        // get last recorded key frame
	VIDEO_NEXT_GOP=4,              // get coming GOP
	VIDEO_LAST_GOP=5,              // get last recorded GOP
	VIDEO_NEXT_GOPS=6,             // get coming GOPs
	VIDEO_LAST_GOPS=7,             // get last recorded GOPs
	VIDEO_NEXT_FRAMES_TIME=8,      // get a specific period of video frames whose timestamps are after the specified timestamp
	VIDEO_LAST_FRAMES_TIME=9,      // get a specific period of video frames whose timestamps are before the specified timestamp
	VIDEO_NEXT_FRAMES_TIMEC=10,    // get a number of video frames whose timestamps are after the specified timestamp
	VIDEO_LAST_FRAMES_TIMEC=11,    // get a number of video frames whose timestamps are before the specified timestamp
	VIDEO_COMMAND_MAX
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
	TARA_VIDEO_CODEC_TYPE ctype;   // video codec of this frame ( TARA_VIDEO_CODEC_TYPE )
	TARA_VIDEO_FRAME_TYPE ftype;   // frame type of this frame ( TARA_VIDEO_FRAME_TYPE )
	u8 metadata[4];
	u32 reserved[16];
} VideoFrameInfo;

typedef struct {
	VideoFrameInfo info;           // frame info
	s8 *data;                      // encoded frame data
} VideoFrame;

typedef struct {
	int num;                       // frame number in current GOP
	VideoFrame *frame;             // frame data
	void *next;                    // pointer to next GOP if available
} VideoGroup;
#endif


#if ENABLE_DEPRECATED
struct video_handle_struct;
typedef struct video_handle_struct video_handle_t;

struct video_handle_struct {
	u32 channel_idx;
	TARA_VIDEO_STREAM_TYPE stype;
	void *privdata;
	/*
	 * get one video frame
	 * return: on error negative value is returned; on success returns 0
	 * handle: video handle
	 * frame: where encoded frame will be stored
	 * cmd: one of VIDEO_NEXT_FRAME/VIDEO_LAST_FRAME/VIDEO_NEXT_KEY_FRAME/VIDEO_LAST_KEY_FRAME
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	s32 (*get_video_frame)(video_handle_t *handle, VideoFrame *frame, u32 cmd, u32 to);

	/*
	 * get one video raw frame, only do VIDEO_NEXT_FRAME? Is it doable for VIDEO_LAST_FRAME?
	 * return: on error negative value is returned; on success always return 1
	 * handle: video handle
	 * frame: where raw frame will be stored
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	s32 (*get_video_rawframe)(video_handle_t *handle, VideoFrame *frame, u32 cmd, u32 to);

	/* get group of frames
	 * return: on error negative value is returned; on success return 0
	 * handle: video handle
	 * gop: where group of frames will be stored
	 * cmd: one of VIDEO_NEXT_GOP/VIDEO_LAST_GOP
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	// In the case of MJPEG, get_video_gop = get_video_frame.
	//s32 (*get_video_gop)(video_handle_t *handle, VideoGroup *gop, u32 cmd, u32 to);

	/* get groups of frames
	 * return: on error negative value is returned; on success return gop number
	 * handle: video handle
	 * gop: where group of frames will be stored
	 * cmd: one of VIDEO_NEXT_GOP/VIDEO_LAST_GOP/VIDEO_NEXT_GOPS/VIDEO_LAST_GOPS
	 * to: timeout in milliseconds to wait, 0 means forever
	 * period: in seconds
	 */
	s32 (*get_video_gops)(video_handle_t *handle, VideoGroup **gop, u32 cmd, u32 to, u32 period);

	/* get group of frames
	 * return: on error negative value is returned; on success return 0
	 * handle: video handle
	 * frame: where encoded frames will be stored
	 * cmd: one of VIDEO_NEXT_GOP/VIDEO_LAST_GOP
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	// In the case of MJPEG, get_video_gop = get_video_frame.
	//s32 (*get_video_gop)(video_handle_t *handle, VideoFrame **frame, u32 cmd, u32 to);

	/* get coming GOPs in spedified period
	 * return: on error negative value is returned; on success return video gop number
	 * handle: video handle
	 * frame: where encoded frames will be stored
	 * period: in seconds
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	//s32 (*get_next_gops)(video_handle_t *handle, VideoFrame **frame, u32 period, u32 to);

	/* get last GOPs in specified period
	 * return: on error negative value is returned; on success return video gop number
	 * handle: video handle
	 * frame: where encoded frames will be stored
	 * period: in seconds
	 * to: timeout in milliseconds to wait, 0 means forever
	 */
	//s32 (*get_last_gops)(video_handle_t *handle, VideoFrame **frame, u32 period, u32 to);

	/* release resource allocated by get_video_frame call */
	s32 (*free_video_frame)(VideoFrame *frame);

	/* release resource allocated by get_videos call */
	s32 (*free_video_gops)(VideoGroup **gop);
	//s32 (*free_video_gops)(VideoFrame **frame);

	/* set threshold for motion detect. Threshold is based on SAD */
	s32 (*set_motion_threshold)(video_handle_t *handle, u32 threshold);

	/*
	 * to: timeout in seconds to wait, 0 means forever
	 * motion_cb: callback function when motion event detected or timeout
	 * motion detection is not for MJPEG?
	 */
	//s32 (*set_motion_event_cb)(video_handle_t *handle, TARA_VIDEO_STREAM_TYPE type, u32 to,
	//                           s32 (*motion_cb)(bool result, void *user_priv));


	/*
	 * return: has video ==> ERROR_NONE
	 *         no signal ==> ERROR_NOVIDEO
	 *         negative value ==> operation failed
	 */
	s32 (*get_video_status)(video_handle_t *handle);

	/* get groups of frames
	 * return: on error negative value is returned; on success return gop number
	 * handle: video handle
	 * gop: where video frames will be stored
	 * cmd: one of VIDEO_NEXT_FRAMES_TIME/VIDEO_LAST_FRAMES_TIME
	 * to: timeout in milliseconds to wait, 0 means forever
	 * option: video length in milliseconds for cmd VIDEO_NEXT_FRAMES_TIME and VIDEO_LAST_FRAMES_TIME
	 *         video frame counts for cmd VIDEO_NEXT_FRAMES_TIMEC and VIDEO_LAST_FRAMES_TIMEC
	 */
	s32 (*get_video_frames_by_time)(video_handle_t *handle, VideoGroup **gop, u32 cmd, u32 to, s64 ts, u32 option);
};
#endif

#ifdef CPLUSPLUS
extern "C" {
#endif

#if NEW_IMPLEMENT
#else
//extern video_handle_t *Tara_video_init(s32 cam);

// start the stream with the configured Image Settings in config file
extern s32 Tara_video_start(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type);

// stop the stream
extern s32 Tara_video_stop(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type);

// Only attach when stream already started
extern video_handle_t *Tara_video_attach(u32 channel, TARA_VIDEO_STREAM_TYPE stream_type);

// detach motion_cb and all related methods
extern s32 Tara_video_detach(video_handle_t *vh);

// release resource allocated by get_video_frame call
extern s32 Tara_video_free_video_frame(VideoFrame *frame);
#endif

// release resource allocated by get_videos call
extern s32 Tara_video_free_video_gops(VideoGroup *gop);

#ifdef CPLUSPLUS
}
#endif
#endif
