/* ===========================================================================
* @file av_interface.h
*
* @path $(IPNCPATH)/include/
*
* @desc av_interface common include file
* .
* Copyright (c) Pacidal
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
  \file av_interface.h

  \brief av_interface common include file
*/

#ifndef _AV_INTERFACE_
#define _AV_INTERFACE_

#if defined (__cplusplus)
extern "C" {
#endif
#include <Msg_Def.h>

//#ifdef DEBUG
//#define dbg(x...)		fprintf(stderr,x)
//#else
//#define dbg(x...)
//#endif


/* return value */
#define RET_SUCCESS 		0		/**< error code : no error */
#define RET_NO_VALID_DATA   -1		/**< error code : request data not ready */
#define RET_NEW_MJPEG       -2		/**< error code : new jpeg(not use now) */
#define RET_INVALID_PRM     -100	/**< error code : invalid parameter */
#define RET_OVERWRTE        -102	/**< error code : request data loss */
#define RET_NO_MEM          -103	/**< error code : not enough memory for locking frame */
#define RET_ERROR_OP		-104	/**< error code : error operation */
#define RET_INVALID_COMMAND -200	/**< error code : invalid command */
#define RET_UNKNOWN_ERROR	-400	/**< error code : unknow error */

/* definition for AV_DATA.flags */
#define AV_FLAGS_MP4_I_FRAME 			0x00001 /**< First frame of GOP */
#define AV_FLAGS_MP4_LAST_FRAME 		0x00002 /**< Last frame of GOP */
/* definition for AV_DATA.frameType */
#define AV_FRAME_TYPE_I_FRAME	0x1	/**< frame type is I frame */
#define AV_FRAME_TYPE_P_FRAME	0x2	/**< frame type is P frame */

#define MAX_VOL_NAL	5
/**
 * @brief frame information data structure for GetAVData()
 */
typedef struct _av_data
{
	unsigned int serial;	/**< frame serial number */
	unsigned int framecnt;
	unsigned int size;		/**< frame size */
	unsigned int width;		/**< frame width */
	unsigned int height;	/**< frame height */
	unsigned int framerate;
	unsigned int bitrate;		
	unsigned int quality;	/**< frame quality */
	unsigned int flags;		/**< frame serial number */
	unsigned int frameType;	/**< either AV_FRAME_TYPE_I_FRAME  or AV_FRAME_TYPE_P_FRAME */
	unsigned long long timestamp;	/**< get frame time stamp */
	unsigned int numVolNal;
	unsigned int volOffsets[MAX_VOL_NAL];
	unsigned int volNalLens[MAX_VOL_NAL];
	unsigned int temporalId; /**< get frame temporalId */
//	int			 ref_serial[FMT_MAX_NUM];	/**<  all frame serial record for reference */
	unsigned char * ptr;	/**<  pointer for data ouput */
} AV_DATA;

typedef struct _VideoStream_Info
{
	unsigned int serial;	/**< frame serial number */
	unsigned int framecnt;
	unsigned int width;		/**< frame width */
	unsigned int height;	/**< frame height */
	unsigned int framerate;
	unsigned int bitrate;		
	FrameFormat_t format;
} VideoStream_Info;

/* interface functions */
//int func_get_mem( void *pdata );
//int GetAVData( int qid, int type, AV_DATA * ptr );
int GetVideoStreamInfo(int numStr, FrameInfo_t *frame);
int GetMjpegFrame(int numStr, FrameInfo_t *frame);
int GetAVData(FrameFormat_t format, int chn, AV_DATA *av_ptr);
int GetAVData1(FrameFormat_t format, int chn, FrameInfo_t *frame);
//int ResetAVDataBuffer(FrameFormat_t format, FrameInfo_t *frame);
int ResetAVDataBuffer(int chn, FrameInfo_t *frame);
int avInterfaceInit(int id);
int avInterfaceExit();

/* @} */

#if defined (__cplusplus)
}
#endif



#endif /*_AV_INTERFACE_*/
