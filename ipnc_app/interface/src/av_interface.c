/* ===========================================================================
* @file Appro_interface.c
*
* @path $(IPNCPATH)/util/
*
* @desc Interface for getting Audio/Video data between processes
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <avDrvMsg.h>
#include "av_interface.h"
/**
 * @brief Interface for getting Audio/Video data between processes
 *
 *
 * @param   type   video type defined at approinput.hh
 * @param   ptr    frame information data structure defined at av_interface.h
 *
 * @return the value should be RET_XXXXX defined at av_interface.h
 *
 * @pre Must have called AVDrvInit()
 *
 */
#define STREAM_PATH		"/tmp/stream"

FILE *pFile = NULL;
FILE *pCloseFile = NULL;
//FILE *pAudioFile = NULL;
//FILE *pCloseAudioFile = NULL;
//int curAudioSerialNo = 0, preAudioSerialNo = 0;
//unsigned long long preTimestamp;
unsigned char FrameBuff[1024*1024];
//unsigned char AFrameBuff[256];
int dbg_frameCnt;
int preFrameCnt;

void OpenFileHdl(FrameFormat_t format, int chn, int serial)
{
	char filename[60];
#if 1
	if (format == FMT_H264)
		sprintf(filename, "%s/%d/%d.h264", STREAM_PATH, chn, serial);
	else if (format == FMT_H265)
		sprintf(filename, "%s/%d/%d.h265", STREAM_PATH, chn, serial);
	else if (format == FMT_H264_SD)
		sprintf(filename, "%s/%d/%d.h264", STREAM_PATH, chn, serial);
	else if (format == FMT_H265_SD)
		sprintf(filename, "%s/%d/%d.h265", STREAM_PATH, chn, serial);
	else if (format == FMT_H264_CIF)
		sprintf(filename, "%s/%d/%d.h264", STREAM_PATH, chn, serial);
	else if (format == FMT_H265_CIF)
		sprintf(filename, "%s/%d/%d.h265", STREAM_PATH, chn, serial);
	else if (format == FMT_MJPEG || format == FMT_MJPEG2)
		sprintf(filename, "%s/%d/%d.mjp", STREAM_PATH, chn, serial);
#else
	if (format == FMT_H265 || format == FMT_H265_CIF || format == FMT_H265_SD)
		sprintf(filename, "%s/%d/%ld.h265", STREAM_PATH, format, serial);

	if (format == FMT_H264 || format == FMT_H264_CIF || format == FMT_H264_SD)
		sprintf(filename, "%s/%d/%ld.h264", STREAM_PATH, format, serial);

	if (format == FMT_MJPEG || format == FMT_MJPEG2)
		sprintf(filename, "%s/%d/%ld.mjp", STREAM_PATH, format, serial);
#endif
	pFile = fopen(filename, "rb");
	if( pFile == NULL )
		fprintf(stderr,"open file %s fail!!\n", filename);
	else {
		fseek(pFile, 0, SEEK_SET);	// reset position
	}
}

void CloseFileHdl(void)
{
	if (pCloseFile != NULL)
	{
		fclose(pCloseFile);
		pCloseFile = NULL;
	}

	if( pFile != NULL )
	{
		fclose(pFile);
		pFile = NULL;
	}
}

int GetAVData(FrameFormat_t format, int chn, AV_DATA *av_ptr)
{
	static int last_serial = 0;
	int serial_no = 0;
	int offset = 0;
	int framesize = 0;
	int frame_cnt=0;
	int read_bytes;
	FrameInfo_t frame;
	int i;

_TRYAGAIN:
	if (GetAVData1(format, chn, &frame) < 0)
		return -1;

	serial_no = frame.fileName;
	if (!last_serial || serial_no != last_serial) {
		pCloseFile = pFile;
		OpenFileHdl(format, chn, serial_no);
		if (pFile == NULL) {
fprintf(stderr, "format %d %d(%d): pack cnt %d offset %d len %d $$$\n", format,  frame.fileName, last_serial, frame.packCnt, frame.packInfo[0].offset, frame.packInfo[0].size);
			last_serial = 0;
			goto _TRYAGAIN;
			return -1;
		}
		
		last_serial = serial_no;
		dbg_frameCnt = 0;
		preFrameCnt = frame.frameCnt;
	}
		
	if (format == FMT_MJPEG || format == FMT_MJPEG2)
	{
		framesize = frame.packInfo[0].size + frame.packInfo[1].size;
		offset = frame.packInfo[0].offset;
//fprintf(stderr, "%p\n", pFile);
		fseek(pFile, offset, SEEK_SET);
		if ((read_bytes = fread(FrameBuff, framesize, 1, pFile)) <= 0) { 
fprintf(stderr, "MJPEG err read len %d fail %d \n", framesize, offset);
fprintf(stderr, "%d read %d errno %d %p\n", serial_no, read_bytes, errno, pFile);
			return -1;	// define ERRNO
		}
		av_ptr->size = framesize;
		av_ptr->ptr = (unsigned char *)FrameBuff;
		av_ptr->frameType = AV_FRAME_TYPE_I_FRAME;
		if (pCloseFile != NULL)
		{
			fclose(pCloseFile);
			pCloseFile = NULL;
		}
	}
	else {
		if (frame.packCnt > 1) {
			int pack_size;
			int pack_offset = 0;

			av_ptr->numVolNal = frame.packCnt;
			for (i = 0; i < frame.packCnt; i++) {
				offset = frame.packInfo[i].offset;
				pack_size = frame.packInfo[i].size;
				fseek(pFile, offset + 4, SEEK_SET);
				av_ptr->volOffsets[i] = pack_offset;
				av_ptr->volNalLens[i] = pack_size - 4;
				if ((read_bytes=fread(FrameBuff + pack_offset, pack_size - 4, 1, pFile)) <= 0) {
fprintf(stderr, "err read for frame type %0x len %d fail \n",FrameBuff[pack_offset], pack_size);
fprintf(stderr, "packCnt %d read %d errno %d \n", frame.packCnt, read_bytes, errno);
					return -1;	// define ERRNO

				} else {
//fprintf(stderr, "read nal type %x pack_offset %d\n", FrameBuff[pack_offset], pack_offset);
					pack_offset += (pack_size - 4);
				}
			}
			av_ptr->size = frame.packInfo[0].size - 4;
			av_ptr->ptr = (unsigned char *)FrameBuff;
			av_ptr->frameType = AV_FRAME_TYPE_I_FRAME;
			dbg_frameCnt = 0;
			preFrameCnt = frame.frameCnt;
		}
		else {
			framesize = frame.packInfo[0].size;
			offset = frame.packInfo[0].offset;
//fprintf(stderr, "%p\n", pFile);
			fseek(pFile, offset, SEEK_SET);
			if ((read_bytes = fread(FrameBuff, framesize, 1, pFile)) <= 0) { 
fprintf(stderr, "err read for frame type %0x len %d fail %d type %d\n",FrameBuff[4], framesize, offset, format);
fprintf(stderr, "%d read %d errno %d %p\n", serial_no, read_bytes, errno, pFile);
				return -1;	// define ERRNO
			}
			else {
//fprintf(stderr, "read buff %x:%x:%x:%x:%x\n", FrameBuff[0], FrameBuff[1], FrameBuff[2], FrameBuff[3], FrameBuff[4]);
			}
			av_ptr->size = frame.packInfo[0].size;
			av_ptr->ptr = (unsigned char *)FrameBuff;
			av_ptr->frameType = AV_FRAME_TYPE_P_FRAME;
			if (pCloseFile != NULL)
			{
				fclose(pCloseFile);
				pCloseFile = NULL;
			}
		}
	}

	dbg_frameCnt++;
	av_ptr->serial = serial_no;
	av_ptr->width = frame.width;
	av_ptr->height = frame.height;
	av_ptr->timestamp = frame.timestamp;
	av_ptr->framecnt = frame.frameCnt;
	frame_cnt = frame.frameCnt;
//fprintf(stderr, "%s: %lld\n", __func__, frame.timestamp);
//if (frame_cnt > 1 && av_ptr->size < 500) fprintf(stderr, "frame serial %d framesize %ld frame_cnt %d:%d type %d \n", av_ptr->serial, av_ptr->size, frame_cnt, dbg_frameCnt, type);
#if 0 //defined(dbg)
if (frame_cnt - preFrameCnt > 1 && format != FMT_MJPEG && format != FMT_MJPEG2)
fprintf(stderr, "get frame serial %d framesize %ld frame_cnt %d preFrameCnt %d\n", av_ptr->serial, av_ptr->size, frame_cnt, preFrameCnt);
//if (dbg_frameCnt != frame_cnt && type != VIDEO_TYPE_MJPEG && type != VIDEO_TYPE_MJPEG2)
//fprintf(stderr, "get frame serial %d framesize %ld frame_cnt %d:%d type %d\n", av_ptr->serial, av_ptr->size, frame_cnt, dbg_frameCnt, type);
#endif
	preFrameCnt = frame.frameCnt;
	return 0;	// success
}

int GetVideoStreamInfo(int numStr, FrameInfo_t *frame)
{
	FrameInfo_t cur;
	
	cur = GetCurrentFrameInfo(numStr);
	if (cur.packCnt == -1) {
		fprintf(stderr, "GetVideoStreamInfo fail\n");
		return -1;	// define ERRNO
	} else {
		frame->fileName = cur.fileName;
		frame->frameCnt = cur.frameCnt;
		frame->packCnt = cur.packCnt;
		frame->width = cur.width;
		frame->height = cur.height;
//		frame->format = cur.format;
		frame->timestamp = cur.timestamp;
		frame->numStr = numStr;
		frame->framerate = cur.framerate;
		frame->bitrate = cur.bitrate;
		//*frame = cur;
		return 0;	// success
	}	
}

int GetMjpegFrame(int numStr, FrameInfo_t *frame)
{
	FrameInfo_t cur;
	int i = 0;
	
	cur = GetLastMjpegFrame(numStr);
	
	if (cur.packCnt == -1) {
		fprintf(stderr, "GetMjpegFrame fail\n");
		return -1;	// define ERRNO
	} else {
		frame->fileName = cur.fileName;
		frame->frameCnt = cur.frameCnt;
		frame->packCnt = cur.packCnt;
		frame->width = cur.width;
		frame->height = cur.height;
//		frame->format = cur.format;
		frame->timestamp = cur.timestamp;
		for (i = 0; i < cur.packCnt; i++) {
			frame->packInfo[i].offset = cur.packInfo[i].offset;
			frame->packInfo[i].size = cur.packInfo[i].size;
		}
		frame->numStr = numStr;
		frame->framerate = cur.framerate;
		frame->bitrate = cur.bitrate;
		//*frame = cur;
		return 0;	// success
	}	
}
 
int GetAVData1(FrameFormat_t format, int chn, FrameInfo_t *frame)
{
	int i = 0;
	FrameInfo_t cur;
GETAVDATA:
	cur = GetCurrentFrame(format, chn);
	if (cur.packCnt == -1)
	{
		fprintf(stderr, "GetCurrentFrame fail\n");
		return -1;	// define ERRNO
	}
	else if (cur.frameCnt == 0) {
		usleep(10000);
		goto GETAVDATA;
	}
//fprintf(stderr, "format %d %d: pack cnt %d offset %d len %d ===\n", format,  cur.fileName, cur.packCnt, cur.packInfo[0].offset, cur.packInfo[0].size);
	if (getpid() != cur.pid) {
		fprintf(stderr, "%s: %d %d\n", __func__, cur.pid, getpid());
	}
//	if (format != cur.format) {
//		fprintf(stderr, "%s: format = %d cur.format=%d\n", __func__, format, cur.format);
//	}
	frame->fileName = cur.fileName;
	frame->frameCnt = cur.frameCnt;
	frame->packCnt = cur.packCnt;
	frame->width = cur.width;
	frame->height = cur.height;
//	frame->format = cur.format;
	frame->timestamp = cur.timestamp;
	for (i = 0; i < cur.packCnt; i++) {
		frame->packInfo[i].offset = cur.packInfo[i].offset;
		frame->packInfo[i].size = cur.packInfo[i].size;
	}
//	*frame = cur;
	return 0;	// success
}

//int ResetAVDataBuffer(FrameFormat_t format, FrameInfo_t *frame)
int ResetAVDataBuffer(int chn, FrameInfo_t *frame)
{
	FrameInfo_t cur;
	
	cur = ResetFrameBuffer(chn);
	*frame = cur;
	return 0;
}

int avInterfaceInit(int id)
{
	return AVDrvInit(id);
}

/**
 * @brief Release the resource of the av interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int avInterfaceExit()
{
	return AVDrvExit();
}

