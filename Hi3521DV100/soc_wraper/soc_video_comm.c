/******************************************************************************
  Hisilicon Hi3531 video encode functions.

 ******************************************************************************
    Modification:  2015-10 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"
#include <time.h>
#include "iniparser.h"
#include "soc_video_priv.h"
#include "soc_video.h"
#include "tara_video_source.h"
#include "shmkey.h"
#include "vqueue.h"

#define VIDEO_PLATFORM_CFG	 "/var/cfg/video_platform.cfg"
#define STREAM_PATH		"/tmp/stream"

#define MIN_WIDTH              64
#define MIN_HEIGHT             64
#define MAX_WIDTH              4096
#define MAX_HEIGHT             4096

#define VALUE_BETWEEN(x,min,max) (((x)>=(min)) && ((x) <= (max)))

STR_CTRL_S StreamCtrl[MAX_VENC_CHN];
//STR_CTRL_S MjpegCtrl[MAX_CAM_CHN];
//STR_CTRL_S video1Ctrl[MAX_CAM_CHN], video2Ctrl[MAX_CAM_CHN];
void *rq[MAX_CAM_CHN];

VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_NTSC;
VpssGrpInfo vs_hw[1];
TaraVideoSource vs[MAX_CAM_CHN];
ThreadInfo_s raw, viframe;
int raw_stream = 0;
int rawfileformat = 0;
int getstrflag = 0, getmjpflag = 0, getv2flag = 0;
int getStrCnt = 0;
int dbgflag=0;

static pthread_t gs_VencPid, gs_MjpgPid, gs_Video2Pid;//, gs_AencPid;
static AVSERVER_VENC_GETSTREAM_PARA_S gs_stParam, gs_mjParam, gs_v2Param;
static GetStreamThreadCtrl getStrThrCtrl[MAX_CAM_CHN];
static TaraVideoEncoder video1[MAX_CAM_CHN], video2[MAX_CAM_CHN];
static TaraMjpegEncoder mjpeg1[MAX_CAM_CHN];

const VpssGrpInfo vs_hw_default[1] = 
{
	{
		1920, 
		1080,
		{
			{1, 128, 1920, 1080, 0, 30, 30, 0, 0},
			{1, 64, 1280, 720, 0, 30, 30, 1, 0},
			{1, 64, 1920, 1080, 0, 30, 30, 0, 1},
			{1, 32, 720, 576, 0, 30, 10, 0, 0}
		}, 
		{
			{0, 1, VENC_GOPMODE_NORMALP},
			{1, 1, VENC_GOPMODE_NORMALP},
			{2, 1, VENC_GOPMODE_NORMALP}
		}
	}
};

static int video_platform_restart(void);

HI_BOOL isRBufFull(HI_S32 chn)
{
	return ((StreamCtrl[chn].rb_wtidx + 1) % STR_RB_MAX == StreamCtrl[chn].rb_rdidx) ?
		HI_TRUE : HI_FALSE;
}

HI_BOOL isRBufEmpty(HI_S32 chn)
{
	return (StreamCtrl[chn].rb_rdidx == StreamCtrl[chn].rb_wtidx) ? HI_TRUE : HI_FALSE;
}

void CleanRBuf(HI_S32 chn)
{
	StreamCtrl[chn].rb_rdidx = StreamCtrl[chn].rb_wtidx;
}

void RBufWrite(HI_S32 chn, STREAM_FILE_INFO_S *p)
{
	HI_S32 wtidx = StreamCtrl[chn].rb_wtidx;
	HI_S32 rdidx = StreamCtrl[chn].rb_rdidx;
	STREAM_FILE_INFO_S str = *p;
//	int i;

	if (isRBufFull(chn))
	{
//		printf("RBuffer is full\n");
		rdidx = (rdidx + 1) % STR_RB_MAX;
		StreamCtrl[chn].rb_rdidx = rdidx;
	}
		
	StreamCtrl[chn].rbuf[wtidx] = str;
	StreamCtrl[chn].rb_wtidx = (StreamCtrl[chn].rb_wtidx + 1) % STR_RB_MAX;
}

void RBufRead(HI_S32 chn, STREAM_FILE_INFO_S *p)
{
	int i;
	HI_S32 rdidx = StreamCtrl[chn].rb_rdidx;

	p->serialNo = StreamCtrl[chn].rbuf[rdidx].serialNo;
	p->frameCnt = StreamCtrl[chn].rbuf[rdidx].frameCnt;
	p->packCnt = StreamCtrl[chn].rbuf[rdidx].packCnt;

	for (i = 0; i < p->packCnt; i++)
		p->packInfo[i] = StreamCtrl[chn].rbuf[rdidx].packInfo[i];

	p->timestamp = StreamCtrl[chn].rbuf[rdidx].timestamp;
	StreamCtrl[chn].rb_rdidx = (StreamCtrl[chn].rb_rdidx + 1) % STR_RB_MAX;
}

void _ResetFrameBuffer(int chn)
{
printf("%s:%d\n", __func__, chn);
	CleanRBuf(chn);
}

void _GetCurrentFrame(int type, FrameInfo_t *frame)
{
	STREAM_FILE_INFO_S Str;
	int chn = 0, cnt = 0, i = 0;
	HI_BOOL nodata = HI_TRUE;

	chn = frame->vencChn;
	if (type == BOA_MSG_TYPE || type == FCGI_MSG_TYPE)
		chn = 2;

	if (chn >= MAX_VENC_CHN) {
		frame->format = FMT_MAX_NUM;
		frame->fileName = -1;
		frame->frameCnt = -1;
		frame->timestamp = -1;
		frame->packCnt = -1;
		return;
	}
 	for (cnt = 0; cnt < 1; cnt++) {
		nodata = isRBufEmpty(chn);
		if (nodata == HI_FALSE) 
			break;
//		usleep(10000);
	}
	
//	frame->format = StreamCtrl[chn].format;			
	if (nodata == HI_TRUE) {
		frame->fileName = 0;
		frame->frameCnt = 0;
		frame->timestamp = 0;
		frame->packCnt = 0;
	} else {
		RBufRead(chn, &Str);
		frame->fileName = Str.serialNo;
		frame->frameCnt = Str.frameCnt;
		frame->timestamp = Str.timestamp;
		frame->packCnt = Str.packCnt;
		for (i = 0; i < Str.packCnt; i++) {
			frame->packInfo[i].offset = Str.packInfo[i].offset;
			frame->packInfo[i].size = Str.packInfo[i].size;
//printf("ofset %d size %d\n", Str.packInfo[i].offset, Str.packInfo[i].size);
		}
	}
}

void _GetCurrentFrameInfo(FrameInfo_t *frame)
{
	int chn = 1;
	int channel = frame->numStr / MAX_STREAM_NUM;
	int stype = frame->numStr % MAX_STREAM_NUM;
	chn = frame->numStr;
	
	if ((stype == 0 && video1[channel].video == 0) ||
			(stype == 1 && video2[channel].video == 0)) {
//	if (stream[chn].encAttr.enable == 0) {
		frame->format = FMT_MAX_NUM;
		frame->fileName = -1;
		frame->frameCnt = -1;
		frame->timestamp = -1;
		frame->packCnt = -1;
	} else {
//		frame->format = StreamCtrl[chn].format;			
		frame->fileName = StreamCtrl[chn].serialNo;
		frame->frameCnt = StreamCtrl[chn].frameCnt;
		frame->timestamp = StreamCtrl[chn].timestamp;
		frame->packCnt = 0;
		frame->width = (stype == 1) ? video2[channel].width : video2[channel].width;
		frame->height = (stype == 1) ? video2[channel].height : video2[channel].height;
		frame->framerate = (stype == 1) ? video2[channel].framerate : video2[channel].framerate;
		frame->bitrate = (stype == 1) ? video2[channel].bitrate : video2[channel].bitrate;
	}
}

void _GetLastMjpegFrame(FrameInfo_t *frame)
{
	int chn = 2;
	int i;
	HI_S32 rdidx; 
	int	channel = frame->numStr / MAX_STREAM_NUM;
	chn = frame->numStr;

//	printf("%s: %d\n", __func__, chn);
	rdidx = (StreamCtrl[chn].rb_wtidx == 0) ? STR_RB_MAX - 1 : StreamCtrl[chn].rb_wtidx - 1;

//	if (stream[chn].encAttr.enable == 0) {
	if (mjpeg1[channel].mjpeg == 0) {
		frame->format = FMT_MAX_NUM;
		frame->fileName = -1;
		frame->frameCnt = -1;
		frame->timestamp = -1;
		frame->packCnt = -1;
	} else {
//		frame->format = StreamCtrl[chn].format;			
		frame->fileName = StreamCtrl[chn].rbuf[rdidx].serialNo;
		frame->frameCnt = StreamCtrl[chn].rbuf[rdidx].frameCnt;
		frame->timestamp = StreamCtrl[chn].rbuf[rdidx].timestamp;
		frame->packCnt = StreamCtrl[chn].rbuf[rdidx].packCnt;
		for (i = 0; i < frame->packCnt; i++) {
			frame->packInfo[i].offset = StreamCtrl[chn].rbuf[rdidx].packInfo[i].offset;
			frame->packInfo[i].size = StreamCtrl[chn].rbuf[rdidx].packInfo[i].size;
//printf("ofset %d size %d\n", Str.packInfo[i].offset, Str.packInfo[i].size);
		}
		frame->width = mjpeg1[channel].width;
		frame->height = mjpeg1[channel].height;
		frame->framerate = mjpeg1[channel].framerate;
		frame->bitrate = 2000;
	}
}

/******************************************************************************
* funciton : save H264 stream
******************************************************************************/
HI_S32 video_platform_saveH264(FILE* fpH264File, VENC_STREAM_S *pstStream, STR_CTRL_S *pstrCtrl)
{
	HI_S32 i;
        STREAM_FILE_INFO_S pStr;
        VFrameInfo vframe;
        int channel = pstrCtrl->channelNo / MAX_STREAM_NUM;
        int stype = pstrCtrl->channelNo % MAX_STREAM_NUM;

        memset(&vframe, 0, sizeof(VFrameInfo));
        vframe.codec = VIDEO_CODEC_H264;
        for (i = 0; i < pstStream->u32PackCount; i++) {
                fseek(fpH264File, 0L, SEEK_END);
                fwrite(pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,
                                                pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset, 1, fpH264File);
                fflush(fpH264File);

		fflush(fpH264File);

		pStr.packCnt = pstStream->u32PackCount;
		pStr.packInfo[i].offset = pstrCtrl->lastOffset + pstrCtrl->lastSize;

		if (pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_SPS) {
			pStr.packInfo[i].offset = 0;
			vframe.sps = pstStream->pstPack[i].u32Len;
		} else if (pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_PPS) {
			vframe.pps = pstStream->pstPack[i].u32Len;
		} else if (pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_SEI) {
			vframe.sei = pstStream->pstPack[i].u32Len;
		} else if (pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_ISLICE || pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_IDRSLICE) {
			pstrCtrl->frameCnt = 1;
			vframe.type = VIDEO_KEY_FRAME;
			vframe.pos = 0;
			vframe.size = pstStream->pstPack[i].u32Len + vframe.sps + vframe.pps + vframe.sei;
		} else if (pstStream->pstPack[i].DataType.enH264EType == H264E_NALU_PSLICE) {
			pstrCtrl->frameCnt++;
			vframe.type = VIDEO_PREDICT_FRAME;
			vframe.pos = pStr.packInfo[i].offset;
			vframe.size = pstStream->pstPack[i].u32Len;
                }
		vframe.timestamp = pstStream->pstPack[i].u64PTS;
		pstrCtrl->timestamp = pStr.timestamp = pstStream->pstPack[i].u64PTS;
                pstrCtrl->lastSize = pStr.packInfo[i].size = pstStream->pstPack[i].u32Len;
                pstrCtrl->lastOffset = pStr.packInfo[i].offset;

        }
        vframe.dirid = pstrCtrl->channelNo;
        vframe.fileid = pstrCtrl->serialNo;
        vframe.seqno = pstStream->u32Seq;
        vframe.width = (stype == 0) ? video1[channel].width : video2[channel].width;
        vframe.height = (stype == 0) ? video1[channel].height : video2[channel].height;
//        if (VQueueIsFull(pstrCtrl->vq)) {
//                VQueueRemove(pstrCtrl->vq);
//        }
//        VQueueInsert(pstrCtrl->vq, (void*)&vframe);
        pStr.serialNo = pstrCtrl->serialNo;
        pStr.frameCnt = pstrCtrl->frameCnt;
        RBufWrite(pstrCtrl->channelNo, &pStr);

        return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H265 stream
******************************************************************************/
HI_S32 video_platform_saveH265(FILE* fpH265File, VENC_STREAM_S *pstStream, STR_CTRL_S *pstrCtrl)
{
	HI_S32 i;
	STREAM_FILE_INFO_S pStr;
	VFrameInfo vframe;
	int channel = pstrCtrl->channelNo / MAX_STREAM_NUM;
	int stype = pstrCtrl->channelNo % MAX_STREAM_NUM;

	memset(&vframe, 0, sizeof(VFrameInfo));
	vframe.codec = VIDEO_CODEC_HEVC;
	for (i = 0; i < pstStream->u32PackCount; i++) {
		fseek(fpH265File, 0L, SEEK_END);
		fwrite(pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,
                                                pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset, 1, fpH265File);
                fflush(fpH265File);

                pStr.packCnt = pstStream->u32PackCount;
                pStr.packInfo[i].offset = pstrCtrl->lastOffset + pstrCtrl->lastSize;

                if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_VPS) {
                        pStr.packInfo[i].offset = 0;
                        vframe.vps = pstStream->pstPack[i].u32Len;
                } else if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_SPS) {
                        vframe.sps = pstStream->pstPack[i].u32Len;
                } else if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_PPS) {
                        vframe.pps = pstStream->pstPack[i].u32Len;
                } else if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_SEI) {
                        vframe.sei = pstStream->pstPack[i].u32Len;
                } else if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_ISLICE ||
                                                                pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_IDRSLICE) {
                        pstrCtrl->frameCnt = 1;
                        vframe.type = VIDEO_KEY_FRAME;
                        vframe.pos = 0;
                        vframe.size = pstStream->pstPack[i].u32Len + vframe.sps + vframe.pps + vframe.sei;
                } else if (pstStream->pstPack[i].DataType.enH265EType == H265E_NALU_PSLICE) {
                        pstrCtrl->frameCnt++;
                        vframe.type = VIDEO_PREDICT_FRAME;
                        vframe.pos = pStr.packInfo[i].offset;
                        vframe.size = pstStream->pstPack[i].u32Len;
                }
                vframe.timestamp = pstStream->pstPack[i].u64PTS;
                pstrCtrl->timestamp = pStr.timestamp = pstStream->pstPack[i].u64PTS;
                pstrCtrl->lastSize = pStr.packInfo[i].size = pstStream->pstPack[i].u32Len;
                pstrCtrl->lastOffset = pStr.packInfo[i].offset;
        }

        vframe.dirid = pstrCtrl->channelNo;
        vframe.fileid = pstrCtrl->serialNo;
        vframe.seqno = pstStream->u32Seq;
        vframe.width = (stype == 0) ? video1[channel].width : video2[channel].width;
        vframe.height = (stype == 0) ? video1[channel].height : video2[channel].height;
//        if (VQueueIsFull(pstrCtrl->vq)) {
//                VQueueRemove(pstrCtrl->vq);
//        }
//        VQueueInsert(pstrCtrl->vq, (void*)&vframe);

        pStr.serialNo = pstrCtrl->serialNo;
        pStr.frameCnt = pstrCtrl->frameCnt;
        RBufWrite(pstrCtrl->channelNo, &pStr);

        return HI_SUCCESS;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
HI_S32 video_platform_saveStream(PAYLOAD_TYPE_E enType, VENC_STREAM_S *pstStream, STR_CTRL_S *strCtrl)
{
        HI_S32 s32Ret;

        if (PT_H264 == enType)
                s32Ret = video_platform_saveH264(strCtrl->pFile, pstStream, strCtrl);
//        else if (PT_MJPEG == enType)
//                s32Ret = video_platform_saveMjpeg(strCtrl->pFile, pstStream, strCtrl);
        else if (PT_H265 == enType)
                s32Ret = video_platform_saveH265(strCtrl->pFile, pstStream, strCtrl);
        else
                return HI_FAILURE;
        return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
