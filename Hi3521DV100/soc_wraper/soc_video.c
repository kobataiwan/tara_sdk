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

const TaraVideoSource vs_default[MAX_CAM_CHN] = 
{
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1920, 1080, 25}
};

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

void soc_syncCodecPTS(void)
{
#if 1
	struct timespec ts;
	
	clock_gettime(CLOCK_REALTIME, &ts);
#if 1
	if (dbgflag == 1) {
		HI_U64 uPTS;
		struct tm *tmNow;
		time_t tNow;
		
		time(&tNow);
		tmNow = localtime(&tNow);
		HI_MPI_SYS_GetCurPts(&uPTS);
		printf("=== %04d-%02d-%02d %02d:%02d:%02d ===\n", tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday,
						tmNow->tm_hour, tmNow->tm_min, tmNow->tm_sec);
		printf("syncCodecPTS systime=%llu %llu, hi=%llu %llu, [%ld]\n", 
			((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000, ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000,
			uPTS/1000000, uPTS%1000000, 
			(long)((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000) - uPTS);
	}
#endif
	HI_MPI_SYS_SyncPts((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);

#else
	HI_U64 uPTS;
	uPTS = GetSysTimeByUsec();
	HI_MPI_SYS_SyncPts(uPTS);
#endif
}

void soc_initCodecPTS(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	printf("%s: %llu\n", __func__, (ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);
	HI_MPI_SYS_InitPtsBase((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);
}

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
* funciton : save mjpeg stream.
******************************************************************************/
HI_S32 video_platform_saveMjpeg(FILE* fpMJpegFile, VENC_STREAM_S* pstStream, STR_CTRL_S *pstrCtrl)
{
	VENC_PACK_S*  pstData;
	HI_U32 i = 0;
	STREAM_FILE_INFO_S pStr;
	VFrameInfo vframe;
	int channel = pstrCtrl->channelNo / MAX_STREAM_NUM;
	
	memset(&vframe, 0, sizeof(VFrameInfo));
	vframe.codec = VIDEO_CODEC_MJPG;
    vframe.type = VIDEO_KEY_FRAME;
	for (i = 0; i < pstStream->u32PackCount; i++) {
		pstData = &pstStream->pstPack[i];
		fseek(fpMJpegFile, 0L, SEEK_END);
		fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpMJpegFile);
		fflush(fpMJpegFile);

		pStr.packCnt = pstStream->u32PackCount;
		pStr.packInfo[i].offset = pstrCtrl->lastOffset + pstrCtrl->lastSize;
		//pstrCtrl->frameCnt++;		
		pstrCtrl->lastSize = pStr.packInfo[i].size = pstStream->pstPack[i].u32Len;
		pstrCtrl->lastOffset = pStr.packInfo[i].offset;
		pstrCtrl->timestamp = pStr.timestamp = pstStream->pstPack[i].u64PTS;
	}
	pstrCtrl->frameCnt++;
	vframe.pos = pStr.packInfo[0].offset;
	vframe.size = pstStream->pstPack[0].u32Len + pstStream->pstPack[1].u32Len;
	vframe.timestamp = pstStream->pstPack[0].u64PTS;

	vframe.dirid = pstrCtrl->channelNo;
	vframe.fileid = pstrCtrl->serialNo; 
	vframe.seqno = pstStream->u32Seq; 
	vframe.width = mjpeg1[channel].width;
	vframe.height = mjpeg1[channel].height;
	
	if (VQueueIsFull(pstrCtrl->vq)) {
		VQueueRemove(pstrCtrl->vq);
	}
	VQueueInsert(pstrCtrl->vq, (void*)&vframe);
#if 0
	if (channel == 0) {
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		printf("systime=%llu %llu, seqno=%u, timestamp=%llu %llu\n", ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000,
			((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000, vframe.seqno, vframe.timestamp/1000000, vframe.timestamp%1000000);
	}
#endif
//	if (pstrCtrl->channelNo == 2) printf("frameCnt=%d\n", pstrCtrl->frameCnt);
	pStr.serialNo = pstrCtrl->serialNo;
	pStr.frameCnt = pstrCtrl->frameCnt;
	RBufWrite(pstrCtrl->channelNo, &pStr);
	
	return HI_SUCCESS;
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
//		if (((pstrCtrl->channelNo) % 3) == 0)
//printf("video[%d] PTS: %llu\n", pstrCtrl->channelNo, pStr.timestamp);
		pstrCtrl->lastSize = pStr.packInfo[i].size = pstStream->pstPack[i].u32Len;
		pstrCtrl->lastOffset = pStr.packInfo[i].offset;

//			printf("===chn%d %d,%d 0ffset %d, size %d %d last %d %d===\n", pstrCtrl->channelNo, pStr.packCnt, 
//				pstStream->pstPack[i].DataType.enH264EType, pStr.packInfo[i].offset, 
//				pStr.packInfo[i].size, pstrCtrl->frameCnt, pstrCtrl->lastOffset, pstrCtrl->lastSize);			
	}
	vframe.dirid = pstrCtrl->channelNo;
	vframe.fileid = pstrCtrl->serialNo; 
	vframe.seqno = pstStream->u32Seq; 
	vframe.width = (stype == 0) ? video1[channel].width : video2[channel].width;
	vframe.height = (stype == 0) ? video1[channel].height : video2[channel].height;
	if (VQueueIsFull(pstrCtrl->vq)) {
		VQueueRemove(pstrCtrl->vq);
	}
	VQueueInsert(pstrCtrl->vq, (void*)&vframe);
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
//	int idx = pstrCtrl->channelNo % MAX_STREAM_NUM;
		
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
	if (VQueueIsFull(pstrCtrl->vq)) {
		VQueueRemove(pstrCtrl->vq);
	}
	VQueueInsert(pstrCtrl->vq, (void*)&vframe);

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
	else if (PT_MJPEG == enType)
		s32Ret = video_platform_saveMjpeg(strCtrl->pFile, pstStream, strCtrl);
	else if (PT_H265 == enType)
		s32Ret = video_platform_saveH265(strCtrl->pFile, pstStream, strCtrl);        
	else
		return HI_FAILURE;
	return s32Ret;
}

int video_platform_loadConfigFile(void)
{
	dictionary *cfgdictionary = NULL;
	int s32Temp = 0, i, ii;
	char acTempStr[128];
	char *pcTempStr = NULL;

	memcpy(&vs_hw, &vs_hw_default, sizeof(vs_hw));

	cfgdictionary = iniparser_load(VIDEO_PLATFORM_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", VIDEO_PLATFORM_CFG);
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "global:dbg");
	pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	if (pcTempStr == NULL)
		DBG("%s failed\n", acTempStr);
	else if (strcmp(pcTempStr, "On") == 0)
		dbgflag = 1;
	DBG("%s:%s, %d\n",acTempStr, pcTempStr, dbgflag);

	for (i = 0; i < 1; i++) {
		for (ii = 0; ii < VPSS_MAX_PHY_CHN_NUM; ii++) {
			/**************phyChn:phyChn_En**************/
			s32Temp = 0;
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:phyChn_En", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].enPhyChn = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].enPhyChn);

			/**************phyChn:vbConf_BlkCnt**************/
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vbConf_BlkCnt", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].vbBlkCnt = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vbBlkCnt);
	
			/**************phyChn:vpssMode_W**************/
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_W", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].vpssModeWidth = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vpssModeWidth);
		
			/**************phyChn:vpssMode_H**************/
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_H", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].vpssModeHeight = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vpssModeHeight);
	
			/**************phyChn:vpssMode_CompressMode**************/
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_CompressMode", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].vpssModeCompressMode = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vpssModeCompressMode);

			/**************phyChn:vpssAttr_Srcfps**************/
			s32Temp = 0;
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssAttr_Srcfps", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			vs_hw[i].vpsschn[ii].vpssAttrSrcfps = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vpssAttrSrcfps);

			/**************phyChn:vpssAttr_Dstfps**************/
			s32Temp = 0;
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssAttr_Dstfps", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			vs_hw[i].vpsschn[ii].vpssAttrDstfps = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].vpssAttrDstfps);
		
			/**************phyChn:UseUiResolution**************/
			s32Temp = 0;
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:UseUiResolution", ii);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
			if (FAILURE != s32Temp)
				vs_hw[i].vpsschn[ii].UseUiResolution = s32Temp;
			DBG("%s=%d, %d\n",acTempStr, s32Temp, vs_hw[i].vpsschn[ii].UseUiResolution);
		
			snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:NR", ii);
			pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
			if (pcTempStr == NULL)
				DBG("%s failed\n", acTempStr);
			else if (strcmp(pcTempStr, "On") == 0)
				vs_hw[i].vpsschn[ii].NR = 1;
			else if (strcmp(pcTempStr, "Off") == 0)
				vs_hw[i].vpsschn[ii].NR = 0;
			DBG("%s:%s, %d\n",acTempStr, pcTempStr, vs_hw[i].vpsschn[ii].NR);
		}
		
		for (i = 0; i < MAX_STREAM_NUM; i++) {		
			snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:phyChn", i+1);
			pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
			if (pcTempStr == NULL)
				DBG("%s failed\n", acTempStr);
			else if (strcmp(pcTempStr, "0")==0) vs_hw[0].str[i].vpssphychn = 0;
			else if (strcmp(pcTempStr, "1")==0) vs_hw[0].str[i].vpssphychn = 1;
			else if (strcmp(pcTempStr, "2")==0) vs_hw[0].str[i].vpssphychn = 2;
			else if (strcmp(pcTempStr, "3")==0) vs_hw[0].str[i].vpssphychn = 3;
			DBG("%s=%s\n",acTempStr, pcTempStr);
			DBG("vs_hw[0].str[%d].vpssphychn=%d\n", i, vs_hw[0].str[ii].vpssphychn);

			snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:BindVenc", i+1);
			s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);		
			if (FAILURE != s32Temp)
				vs_hw[i].str[ii].bindvpss = s32Temp;
			DBG("%s:%d, %d\n",acTempStr, s32Temp, vs_hw[i].str[ii].bindvpss);

			snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:gopMode", i+1);
			pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
			if (pcTempStr == NULL)
				DBG("%s failed\n", acTempStr);
			else if (strcmp(pcTempStr, "dualp") == 0)
				vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_DUALP;
			else if (strcmp(pcTempStr, "smartp") == 0)
				vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_SMARTP;
			else if (strcmp(pcTempStr, "bipredb") == 0)
				vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_BIPREDB;
			else if (strcmp(pcTempStr, "lowdelayb") == 0)
				vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_LOWDELAYB;
			DBG("%s:%s, %d\n",acTempStr, pcTempStr, vs_hw[i].str[ii].enGopMode);
		}
	}	
	
	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}

/* sp420 to p420 ; sp422 to p422  */
void sample_yuv_dump(VIDEO_FRAME_S * pVBuf, FILE *pfd)
{
  unsigned int w, h;
  char * pVBufVirt_Y;
  char * pVBufVirt_C;
  char * pMemContent;
  unsigned char TmpBuff[MAX_WIDTH];
  HI_U32 phy_addr,y_size,c_size;
  HI_CHAR *pUserPageAddr[2];
  PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
  HI_U32 u32UvHeight;/* uv height when saved for planar type */

  y_size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
  if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat) {
    c_size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)/2;
    u32UvHeight = pVBuf->u32Height/2;
  } else {
    c_size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
    u32UvHeight = pVBuf->u32Height;
  }

  phy_addr = pVBuf->u32PhyAddr[0];

  //printf("phy_addr:%x, size:%d\n", phy_addr, size);
  pUserPageAddr[0] = (HI_CHAR *) HI_MPI_SYS_Mmap(phy_addr, y_size);
  if (NULL == pUserPageAddr[0]) {
	printf("get userPageAddr0 error\n");
    return;
  }
  
  phy_addr = pVBuf->u32PhyAddr[1];
  pUserPageAddr[1] = (HI_CHAR *) HI_MPI_SYS_Mmap(phy_addr, c_size);
  if (NULL == pUserPageAddr[1]) {
	printf("get userPageAddr1 error\n");
    return;
  }
  //printf("stride: %d,%d\n",pVBuf->u32Stride[0],pVBuf->u32Stride[1] );

  pVBufVirt_Y = pUserPageAddr[0];
  pVBufVirt_C = pUserPageAddr[1];//pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

  if (rawfileformat == 1) {
  /* save Y ----------------------------------------------------------------*/
    for(h=0; h<pVBuf->u32Height; h++) {
      pMemContent = pVBufVirt_Y + h*pVBuf->u32Stride[0];
      fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
    }
    fflush(pfd);

  /* save U ----------------------------------------------------------------*/
    for(h=0; h<u32UvHeight; h++) {
      pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

      pMemContent += 1;

      for(w=0; w<pVBuf->u32Width/2; w++) {
        TmpBuff[w] = *pMemContent;
        pMemContent += 2;
      }
      fwrite(TmpBuff, pVBuf->u32Width/2, 1, pfd);
    }
    fflush(pfd);

  /* save V ----------------------------------------------------------------*/
    for(h=0; h<u32UvHeight; h++) {
      pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

      for(w=0; w<pVBuf->u32Width/2; w++) {
        TmpBuff[w] = *pMemContent;
        pMemContent += 2;
      }
      fwrite(TmpBuff, pVBuf->u32Width/2, 1, pfd);
    }
    fflush(pfd);
  } else {
    fwrite(pVBufVirt_Y, y_size, 1, pfd);
    fflush(pfd);
    fwrite(pVBufVirt_C, c_size, 1, pfd);
    fflush(pfd);
  }

//  printf("done %d!\n", pVBuf->u32TimeRef);
  HI_MPI_SYS_Munmap(pUserPageAddr[0], y_size);
  HI_MPI_SYS_Munmap(pUserPageAddr[1], c_size);
}

void *getRawFrameProc(void *p)
{
  HI_S32 s32Ret;
  ThreadInfo_s *pstPara;
  VIDEO_FRAME_INFO_S stFrmInfo;
  VPSS_GRP VpssGrp = 0;
  VPSS_CHN VpssChn = VPSS_CHN2;
  HI_S32 s32MilliSec = 5000; //10000;
  int i;
  HI_CHAR yuvName[MAX_CAM_CHN][FILE_NAME_LEN];
  HI_CHAR pathName[MAX_CAM_CHN][64];
  FILE *pfd;
  HI_U32 fileid = 0;
  s32 qsize = 1;
  VFrameInfo vframe;
  int onoff[MAX_CAM_CHN];
  int framecnt=0;

  pstPara = (ThreadInfo_s *)p;
  VpssChn = vs_hw[0].str[raw_stream-1].vpssphychn;  
  memset(&vframe, 0, sizeof(VFrameInfo));
  for (i = 0; i < MAX_CAM_CHN; i++) {
    sprintf(pathName[i], "%s/%d", STREAM_PATH, ((i * MAX_STREAM_NUM) + VpssChn));
    memset(yuvName[i], 0, sizeof(yuvName[i]));
    rq[i] = NULL;

    if (VpssChn == 1) {
			rq[i] = VQueueGetPtr(VIDEO_SUBRAW_STREAM_KEY(i), qsize);
			onoff[i] = video2[i].video;
		} else if (VpssChn == 2) {
			rq[i] = VQueueGetPtr(VIDEO_MJPGRAW_STREAM_KEY(i), qsize);
			onoff[i] = mjpeg1[i].mjpeg;
		} else {
			rq[i] = VQueueGetPtr(VIDEO_MAINRAW_STREAM_KEY(i), qsize);
			onoff[i] = video1[i].video;
		}
  }
  
  fileid = GetSysTimeBySec();
  
  printf("get raw frame from  stream%d\n", VpssChn);
  pthread_detach(pthread_self());
  VpssGrp = 0;
  while (pstPara->bRun == HI_TRUE) {
#if 0
	memset(&stFrmInfo,0,sizeof(stFrmInfo));
	s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrmInfo, s32MilliSec);
	if(HI_SUCCESS != s32Ret) {
		printf("HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d), Error(%#x)!\n", VpssGrp, VpssChn, s32Ret);
	} 
	if (VpssGrp == 0 && ++framecnt >= 25) {
		struct timespec ts;
//		HI_U64 tv=0;
		framecnt=0;
		clock_gettime(CLOCK_REALTIME, &ts);
//		tv = GetSysTimeByUsec();
		printf("[vpss]:systime=%llu %llu, pts=%llu %llu [%ld.%ld]\n", 
			(((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000LLU), 
			((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000LLU, 
			stFrmInfo.stVFrame.u64pts/1000000LLU, stFrmInfo.stVFrame.u64pts%1000000LLU,
			(long)(((ts.tv_sec * 1000000LLU) + (ts.tv_nsec / 1000)) - stFrmInfo.stVFrame.u64pts)/1000,
			(long)(((ts.tv_sec * 1000000LLU) + (ts.tv_nsec / 1000)) - stFrmInfo.stVFrame.u64pts)%1000);
	}
	HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrmInfo);

#else
    for (VpssGrp = 0; VpssGrp < MAX_CAM_CHN; VpssGrp++) {
			if (onoff[VpssGrp] == ENCODER_OFF) continue;
      memset(&stFrmInfo,0,sizeof(stFrmInfo));
      s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrmInfo, s32MilliSec);
      if(HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d), Error(%#x)!\n", VpssGrp, VpssChn, s32Ret);
      } 
      else {
				if (strlen(yuvName[VpssGrp]))
					unlink(yuvName[VpssGrp]);
//		else printf(">> no file delete <<\n");
				memset(yuvName[VpssGrp], 0, sizeof(yuvName[VpssGrp]));
        sprintf(yuvName[VpssGrp], "%s/%d.yuv", pathName[VpssGrp], fileid);
        pfd = fopen(yuvName[VpssGrp], "wb");
        if (pfd) {
          memset(&vframe, 0, sizeof(VFrameInfo));
//          printf("%s, VpssGrp%d:%d, %llu\n", yuvName[VpssGrp], VpssGrp, VpssChn, stFrmInfo.stVFrame.u64pts);
          sample_yuv_dump(&stFrmInfo.stVFrame, pfd);
          fseek(pfd, 0, SEEK_END);
          vframe.pos = 0;
          vframe.size = ftell(pfd);
          if (fclose(pfd))
						printf("fclose %s failed\n", yuvName[VpssGrp]);
					vframe.codec = VIDEO_CODEC_NONE;
					vframe.type = VIDEO_KEY_FRAME;
          vframe.ctimestamp = stFrmInfo.stVFrame.u64pts;
          vframe.timestamp = stFrmInfo.stVFrame.u64pts;
          vframe.dirid = ((VpssGrp * MAX_STREAM_NUM) + VpssChn);
          vframe.fileid = fileid;
          vframe.width = stFrmInfo.stVFrame.u32Width;
          vframe.height = stFrmInfo.stVFrame.u32Height;

          if (VQueueIsFull(rq[VpssGrp])) {
            VQueueRemove(rq[VpssGrp]);
          }
          VQueueInsert(rq[VpssGrp], (void*)&vframe);
				} else DBG("open %s file failed!\n", yuvName[VpssGrp]);
			}
      HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrmInfo);
		}
		fileid++;
#endif
		usleep(5000);
  }
  pstPara->bRun = HI_FALSE;
  printf("%s bye~\n", __func__);
  return NULL;
}

void *getVIFrameProc(void *p)
{
  HI_S32 s32Ret;
  ThreadInfo_s *pstPara;
  VIDEO_FRAME_INFO_S stFrmInfo;
  VI_CHN ViChn = 0;
  HI_S32 s32MilliSec = 5000; //10000;
  int i;
  int framecnt=0;
	HI_U32 u32Depth = -1U;
	
  pstPara = (ThreadInfo_s *)p;
  
  printf("get VI frame from  stream%d\n", ViChn);
  pthread_detach(pthread_self());
	if (HI_MPI_VI_GetFrameDepth(ViChn, &u32Depth))
		printf("HI_MPI_VI_GetFrameDepth err, vi chn %d \n", ViChn);
	printf("HI_MPI_VI_GetFrameDepth %d\n", u32Depth);
	if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
		printf("HI_MPI_VI_SetFrameDepth err, vi chn %d \n", ViChn);
        
  while (pstPara->bRun == HI_TRUE) {
//    for (VpssGrp = 0; VpssGrp < MAX_CAM_CHN; VpssGrp++)
		memset(&stFrmInfo,0,sizeof(stFrmInfo));
		s32Ret = HI_MPI_VI_GetFrame(ViChn, &stFrmInfo, s32MilliSec);
    if(HI_SUCCESS != s32Ret)
      printf("HI_MPI_VI_GetFrame failed, ViChn(%d), Error(%#x)!\n", ViChn, s32Ret);
    
	#if 1
//		if (ViChn == 0 && ++framecnt >= 5) {
		if (ViChn == 0) {
			struct timespec ts;
			HI_U64 tv=0;
//			framecnt=0;
			clock_gettime(CLOCK_REALTIME, &ts);
			tv = GetSysTimeByUsec();
			printf("[vi]:  systime=%llu %llu, pts=%llu %llu [[%ld.%ld]]\n", 
				(((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000LLU), 
				((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000LLU, 
				stFrmInfo.stVFrame.u64pts/1000000LLU, stFrmInfo.stVFrame.u64pts%1000000LLU,
				(long)(((ts.tv_sec * 1000000LLU) + (ts.tv_nsec / 1000)) - stFrmInfo.stVFrame.u64pts)/1000,
				(long)(((ts.tv_sec * 1000000LLU) + (ts.tv_nsec / 1000)) - stFrmInfo.stVFrame.u64pts)%1000);
	
		}
	#endif
    HI_MPI_VI_ReleaseFrame(ViChn, &stFrmInfo);
		usleep(10000);
  }
  pstPara->bRun = HI_FALSE;
  printf("%s bye~\n", __func__);
  return NULL;
}

int video_platform_getRawFrameStart(int type)
{
printf("%s:%d\n", __func__, type);
	if (type >= RAWFRAME_TYPE_MAX || type < 0) raw_stream = RAWFRAME_TYPE_OFF;
	else raw_stream = type;
	if (raw_stream == RAWFRAME_TYPE_OFF) return -1;
	raw.bRun = HI_TRUE;
	pthread_create(&raw.hThread, 0, getRawFrameProc, (HI_VOID*)&raw);
#if 0
	viframe.bRun = HI_TRUE;
	pthread_create(&viframe.hThread, 0, getVIFrameProc, (HI_VOID*)&viframe);
#endif
	return 0;
}

int video_platform_getRawFrameStop(void)
{
	DBG("%s\n", __func__);

	if (HI_TRUE == raw.bRun) {
		raw.bRun = HI_FALSE;
		pthread_join(raw.hThread, 0);
	}
#if 0
	if (HI_TRUE == viframe.bRun) {
		viframe.bRun = HI_FALSE;
		pthread_join(viframe.hThread, 0);
	}
#endif
	return SUCCESS;	
}

/******************************************************************************
 * 1. creat venc channel (h264, h265) 
 * 2. Start Recv Venc Pictures
******************************************************************************/
int video_platform_initVideoEncoder(VENC_CHN VencChn, TaraVideoEncoder *p, VENC_GOP_MODE_E enGopMode, int vs_framerate)
{
	HI_S32 s32Ret;
	TaraVideoEncoder venc = *p;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_GOP_ATTR_S stGopAttr;
	HI_U32	u32StatTime;
	
	if (!p) return -1;
	if (venc.type != VIDEO_CODEC_H264 && venc.type != VIDEO_CODEC_HEVC) return -1;
	
	s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr, gs_enNorm);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("Get GopAttr failed!\n");
		stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
		stGopAttr.stNormalP.s32IPQpDelta = 2;
	}
DBG("VencChn%d: codec=%s %dx%d vsfrate=%d gopMode=%d\n", VencChn, venc.type == VIDEO_CODEC_H264 ? "H264" : "H265",
		venc.width, venc.height, vs_framerate, stGopAttr.enGopMode);
	u32StatTime = 1;

	/******************************************
	step 1:  Create Venc Channel
	******************************************/
	if (venc.type == VIDEO_CODEC_HEVC) {
		stVencChnAttr.stVeAttr.enType = PT_H265;
		stVencChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth		= venc.width;
		stVencChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight	= venc.	height;
		stVencChnAttr.stVeAttr.stAttrH265e.u32PicWidth			= venc.width;
		stVencChnAttr.stVeAttr.stAttrH265e.u32PicHeight			= venc.height;
		stVencChnAttr.stVeAttr.stAttrH265e.u32BufSize				= venc.width * venc.height * 2;
		stVencChnAttr.stVeAttr.stAttrH265e.u32Profile 			= 0;
		stVencChnAttr.stVeAttr.stAttrH265e.bByFrame 				= HI_TRUE;/*get stream mode is slice mode or frame mode?*/
		memcpy(&stVencChnAttr.stGopAttr, &stGopAttr, sizeof(VENC_GOP_ATTR_S));
		
		if (venc.ratecontrol == VIDEO_RATE_CONTROL_CBR) {
			VENC_ATTR_H265_CBR_S    stH265Cbr;
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
			stH265Cbr.u32Gop            = venc.gopsize;	//(VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
			stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
			stH265Cbr.u32SrcFrmRate      = vs_framerate;
//			stH265Cbr.fr32DstFrmRate = venc.framerate;
			stH265Cbr.fr32DstFrmRate = (venc.framerate > vs_framerate) ? vs_framerate : venc.framerate;
			stH265Cbr.u32BitRate = venc.bitrate;
			stH265Cbr.u32FluctuateLevel = 1;
			memcpy(&stVencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
		} else if (venc.ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP) {
			VENC_ATTR_H265_FIXQP_S    stH265FixQp;
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
			stH265FixQp.u32Gop = venc.gopsize;
			stH265FixQp.u32SrcFrmRate = vs_framerate;
//			stH265FixQp.fr32DstFrmRate = venc.framerate;
			stH265FixQp.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
			stH265FixQp.u32IQp = venc.qp;	//venc.iqp;
			stH265FixQp.u32PQp = venc.qp; //venc.pqp;
			stH265FixQp.u32BQp = venc.qp; //venc.pqp;
			memcpy(&stVencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
		} else if (venc.ratecontrol == VIDEO_RATE_CONTROL_VBR) {
			VENC_ATTR_H265_VBR_S    stH265Vbr;
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
			stH265Vbr.u32Gop = venc.gopsize;
			stH265Vbr.u32StatTime = u32StatTime;
			stH265Vbr.u32SrcFrmRate = vs_framerate;
//			stH265Vbr.fr32DstFrmRate = venc.framerate;
			stH265Vbr.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
			stH265Vbr.u32MaxBitRate = venc.bitrate;
			stH265Vbr.u32MinQp	= venc.qp; //venc.minqp;
			stH265Vbr.u32MinIQp	= venc.qp; //venc.minqp;
			stH265Vbr.u32MaxQp	= 51; //venc.maxqp;
			memcpy(&stVencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
		} else
			return -1;
	} else if (venc.type == VIDEO_CODEC_H264) {
		stVencChnAttr.stVeAttr.enType = PT_H264;
		stVencChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth 	= venc.width;
		stVencChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight 	= venc.height;
		stVencChnAttr.stVeAttr.stAttrH264e.u32PicWidth			= venc.width;
		stVencChnAttr.stVeAttr.stAttrH264e.u32PicHeight			= venc.height;
		stVencChnAttr.stVeAttr.stAttrH264e.u32BufSize				= venc.width * venc.height * 2;
		/*0: baseline; 1:MP; 2:HP;  3:svc_t */
		if (venc.profile == VIDEO_PROFILE_MAIN)
			stVencChnAttr.stVeAttr.stAttrH264e.u32Profile = 1;
		else if (venc.profile == VIDEO_PROFILE_BASELINE)
			stVencChnAttr.stVeAttr.stAttrH264e.u32Profile = 0;
		else stVencChnAttr.stVeAttr.stAttrH264e.u32Profile = 2;
		stVencChnAttr.stVeAttr.stAttrH264e.bByFrame 				=  HI_TRUE;/*get stream mode is slice mode or frame mode?*/
		memcpy(&stVencChnAttr.stGopAttr, &stGopAttr, sizeof(VENC_GOP_ATTR_S));
		
		if (venc.ratecontrol == VIDEO_RATE_CONTROL_CBR) {
			VENC_ATTR_H264_CBR_S		stH264Cbr;	
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
			stH264Cbr.u32Gop            = venc.gopsize;
			stH264Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
			stH264Cbr.u32SrcFrmRate      = vs_framerate; //SrcFrameRate;
//			stH264Cbr.fr32DstFrmRate = venc.framerate;
			stH264Cbr.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate; /* target frame rate */
			stH264Cbr.u32BitRate = venc.bitrate;
			stH264Cbr.u32FluctuateLevel = 1; /* average bit rate */
			memcpy(&stVencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
		} else if (venc.ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP) {
			VENC_ATTR_H264_FIXQP_S		stH264FixQp;
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
			stH264FixQp.u32Gop = venc.gopsize;
			stH264FixQp.u32SrcFrmRate = vs_framerate;
//			stH264FixQp.fr32DstFrmRate = venc.framerate;
			stH264FixQp.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
			stH264FixQp.u32IQp = venc.qp; //venc.iqp;
			stH264FixQp.u32PQp = venc.qp; //venc.pqp;
			stH264FixQp.u32BQp = venc.qp; //venc.pqp;
			memcpy(&stVencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
		} else if (venc.ratecontrol == VIDEO_RATE_CONTROL_VBR) {
			VENC_ATTR_H264_VBR_S		stH264Vbr;
			stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
			stH264Vbr.u32Gop = venc.gopsize;
			stH264Vbr.u32StatTime = u32StatTime;
			stH264Vbr.u32SrcFrmRate = vs_framerate;
//			stH264Vbr.fr32DstFrmRate = venc.framerate;
			stH264Vbr.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
			stH264Vbr.u32MaxBitRate = venc.bitrate;
			stH264Vbr.u32MinQp = venc.qp; //venc.minqp;
			stH264Vbr.u32MinIQp = venc.qp; //venc.minqp;
			stH264Vbr.u32MaxQp = 51; //venc.maxqp;
			memcpy(&stVencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
		#if 0
			printf("VBR: gop=%d statTime=%d src=%dfps dst=%dfps %dbps %d %d %d\n", stH264Vbr.u32Gop,
				stH264Vbr.u32StatTime, stH264Vbr.u32SrcFrmRate, stH264Vbr.fr32DstFrmRate, stH264Vbr.u32MaxBitRate, 
				stH264Vbr.u32MinQp, stH264Vbr.u32MinIQp, stH264Vbr.u32MaxQp);
		#endif
		} else return -1;
	} else return -1;

	/******************************************
	step 2:  Start Recv Venc Pictures
	******************************************/
	s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", VencChn, s32Ret);
		return FAILURE;
	}

	s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VENC_StartRecvPic %d faild with%#x!\n", VencChn, s32Ret);
		return FAILURE;
	}

	return SUCCESS;
}

/******************************************************************************
 * 1. creat venc channel (mjpeg)
 * 2. Start Recv Venc Pictures
******************************************************************************/
int video_platform_initMjpegEncoder(VENC_CHN VencChn, TaraMjpegEncoder *p, VENC_GOP_MODE_E enGopMode, int vs_framerate)
{
	HI_S32 s32Ret;
	TaraMjpegEncoder venc = *p;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_GOP_ATTR_S stGopAttr;
	HI_U32	u32StatTime;
	VIDEO_RATE_CONTROL_MODE ratectrl = VIDEO_RATE_CONTROL_FIXEDQP;
	
	if (!p) return -1;
	s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr, gs_enNorm);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("Get GopAttr failed!\n");
		stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
		stGopAttr.stNormalP.s32IPQpDelta = 2;
	}
DBG("VencChn%d: codec=MJPEG %dx%d quality=%d\n", VencChn, venc.width, venc.height, venc.quality);
	u32StatTime = 1;

	/******************************************
	step 1:  Create Venc Channel
	******************************************/
	stVencChnAttr.stVeAttr.enType = PT_MJPEG;
	stVencChnAttr.stVeAttr.stAttrMjpege.u32MaxPicWidth	= venc.width;
	stVencChnAttr.stVeAttr.stAttrMjpege.u32MaxPicHeight	= venc.height;
	stVencChnAttr.stVeAttr.stAttrMjpege.u32PicWidth			= venc.width;
	stVencChnAttr.stVeAttr.stAttrMjpege.u32PicHeight		= venc.height;
	stVencChnAttr.stVeAttr.stAttrMjpege.u32BufSize			= venc.width * venc.height * 3;
	stVencChnAttr.stVeAttr.stAttrMjpege.bByFrame				= HI_TRUE;  /*get stream mode is field mode  or frame mode*/
		
	stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
	stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;

	if (ratectrl == VIDEO_RATE_CONTROL_FIXEDQP) {
		VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;	
		stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
		stMjpegeFixQp.u32Qfactor        = venc.quality;
		stMjpegeFixQp.u32SrcFrmRate      = vs_framerate;
//		stMjpegeFixQp.fr32DstFrmRate = venc.framerate;
		stMjpegeFixQp.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
		memcpy(&stVencChnAttr.stRcAttr.stAttrMjpegeFixQp, &stMjpegeFixQp, sizeof(VENC_ATTR_MJPEG_FIXQP_S));
	} else if (ratectrl == VIDEO_RATE_CONTROL_CBR) {
		VENC_ATTR_MJPEG_CBR_S stMjpegeCbr;
		stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
		stMjpegeCbr.u32StatTime = u32StatTime;
		stMjpegeCbr.u32SrcFrmRate = vs_framerate;
//		stMjpegeCbr.fr32DstFrmRate = venc.framerate;
		stMjpegeCbr.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
		stMjpegeCbr.u32BitRate = 2000; //venc.bitrate;
		stMjpegeCbr.u32FluctuateLevel = 1;
		memcpy(&stVencChnAttr.stRcAttr.stAttrMjpegeCbr, &stMjpegeCbr, sizeof(VENC_ATTR_MJPEG_CBR_S));
	} else if (ratectrl == VIDEO_RATE_CONTROL_VBR) {
		VENC_ATTR_MJPEG_VBR_S   stMjpegVbr;
		stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
		stMjpegVbr.u32StatTime = u32StatTime;
		stMjpegVbr.u32SrcFrmRate = vs_framerate;
//		stMjpegVbr.fr32DstFrmRate = venc.framerate;
		stMjpegVbr.fr32DstFrmRate = venc.framerate > vs_framerate ? vs_framerate : venc.framerate;
		stMjpegVbr.u32MaxBitRate = 2000; //venc.bitrate;
		stMjpegVbr.u32MinQfactor = 50;
		stMjpegVbr.u32MaxQfactor = 95;
		memcpy(&stVencChnAttr.stRcAttr.stAttrMjpegeVbr, &stMjpegVbr, sizeof(VENC_ATTR_MJPEG_VBR_S));
	} else return -1;
		
	/******************************************
	step 2:  Start Recv Venc Pictures
	******************************************/
	s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", VencChn, s32Ret);
		return FAILURE;
	}

	s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VENC_StartRecvPic %d faild with%#x!\n", VencChn, s32Ret);
		return FAILURE;
	}

	return SUCCESS;
}

/******************************************************************************
 * modify venc channel attr. (h264, h265)
 * stop recv venc picture -> modify venc attr. -> start recv venc picture
******************************************************************************/
//int video_platform_setVideoEncoder(int channel, TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *pvideo)
int setVideoEncoder(int channel, TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *pvideo)
{
	HI_S32 ret = 0;
	VENC_CHN_ATTR_S VencChnAttr;
	HI_U32	u32StatTime;	
	VENC_CHN VencChn = (channel * MAX_STREAM_NUM) + stream_type;

	DBG("setVideoEncoder-VENC%d:chn%d-%d %dx%d, %d fps, %d bps, gop=%d, rcmode=%d\n", 
		VencChn, channel, stream_type, pvideo->width, pvideo->height, pvideo->framerate,
		pvideo->bitrate, pvideo->gopsize, pvideo->ratecontrol);

	ret = HI_MPI_VENC_GetChnAttr(VencChn, &VencChnAttr);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_GetChnAttr(%d) faild with%#x!\n", VencChn, ret);
		return -1;
	}

	u32StatTime = 1;
	
	if (VencChnAttr.stVeAttr.enType == PT_H264) {
		if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_CBR)  {
			VENC_ATTR_H264_CBR_S		stH264Cbr;
//printf("%s: srcFramerate=%d\n", __func__, VencChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate);
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
			stH264Cbr.u32Gop            = pvideo->gopsize;
			stH264Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
			stH264Cbr.u32SrcFrmRate      = vs[channel].framerate;
			stH264Cbr.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate; /* target frame rate */
			stH264Cbr.u32BitRate = pvideo->bitrate;
			stH264Cbr.u32FluctuateLevel = 1; /* average bit rate */
			memcpy(&VencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
			DBG("H264 CBR: src:%d fps dst:%d fps\n", VencChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate, VencChnAttr.stRcAttr.stAttrH264Cbr.fr32DstFrmRate);
		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_VBR)  {
			VENC_ATTR_H264_VBR_S		stH264Vbr;
//printf("%s: srcFramerate=%d\n", __func__, VencChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate);
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
			stH264Vbr.u32Gop = pvideo->gopsize;
			stH264Vbr.u32StatTime = u32StatTime;
			stH264Vbr.u32SrcFrmRate = vs[channel].framerate;
			stH264Vbr.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate;
			stH264Vbr.u32MaxBitRate = pvideo->bitrate;
			stH264Vbr.u32MinQp = pvideo->qp; //pvideo->minqp;
			stH264Vbr.u32MinIQp = pvideo->qp; //pvideo->minqp;
			stH264Vbr.u32MaxQp = 51; //pvideo->maxqp;
			memcpy(&VencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
			DBG("H264 VBR: src:%d fps dst:%d fps, GOP=%d\n", VencChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate, VencChnAttr.stRcAttr.stAttrH264Vbr.fr32DstFrmRate, stH264Vbr.u32Gop);
		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP)  {
			VENC_ATTR_H264_FIXQP_S		stH264FixQp;
//printf("%s: srcFramerate=%d\n", __func__, VencChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate);			
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
			stH264FixQp.u32Gop = pvideo->gopsize;
			stH264FixQp.u32SrcFrmRate = vs[channel].framerate;
			stH264FixQp.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate;
			stH264FixQp.u32IQp = pvideo->qp; //pvideo->iqp;
			stH264FixQp.u32PQp = pvideo->qp; //pvideo->pqp;
			stH264FixQp.u32BQp = pvideo->qp; //pvideo->pqp;
			memcpy(&VencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
			DBG("H264 FixQP: src:%d fps dst:%d fps\n", VencChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate, VencChnAttr.stRcAttr.stAttrH264FixQp.fr32DstFrmRate);
		} else return -1;
	} else if (VencChnAttr.stVeAttr.enType == PT_H265) {
		if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_CBR) {
			VENC_ATTR_H265_CBR_S    stH265Cbr;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
			stH265Cbr.u32Gop            = pvideo->gopsize;
			stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
			stH265Cbr.u32SrcFrmRate      = vs[channel].framerate;
			stH265Cbr.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate;
			stH265Cbr.u32BitRate = pvideo->bitrate;
			stH265Cbr.u32FluctuateLevel = 1;
			memcpy(&VencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_VBR)  {
			VENC_ATTR_H265_VBR_S    stH265Vbr;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
			stH265Vbr.u32Gop = pvideo->gopsize;
			stH265Vbr.u32StatTime = u32StatTime;
			stH265Vbr.u32SrcFrmRate = vs[channel].framerate;
			stH265Vbr.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate;
			stH265Vbr.u32MaxBitRate = pvideo->bitrate;
			stH265Vbr.u32MinQp	= pvideo->qp; //pvideo->minqp;
			stH265Vbr.u32MinIQp	= pvideo->qp; //pvideo->minqp;
			stH265Vbr.u32MaxQp	= 51; //pvideo->maxqp;
			memcpy(&VencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP) {
			VENC_ATTR_H265_FIXQP_S    stH265FixQp;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
			stH265FixQp.u32Gop = pvideo->gopsize;
			stH265FixQp.u32SrcFrmRate = vs[channel].framerate;
			stH265FixQp.fr32DstFrmRate = pvideo->framerate > vs[channel].framerate ? vs[channel].framerate : pvideo->framerate;
			stH265FixQp.u32IQp = pvideo->qp; //pvideo->iqp;
			stH265FixQp.u32PQp = pvideo->qp; //pvideo->pqp;
			stH265FixQp.u32BQp = pvideo->qp; //pvideo->pqp;
			memcpy(&VencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
		} else return -1;
	} else return -1;
		
	ret = HI_MPI_VENC_StopRecvPic(VencChn);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_StopRecvPic(%d) faild with%#x!\n", VencChn, ret);
		return FAILURE;
	} 	
	usleep(10000);	//10ms
	
	ret = HI_MPI_VENC_SetChnAttr(VencChn, &VencChnAttr);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_SetChnAttr(%d) faild with%#x!\n", VencChn, ret);
		return FAILURE;
	} 	

	usleep(10000);	//10ms
	ret = HI_MPI_VENC_StartRecvPic(VencChn);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_StartRecvPic(%d) faild with%#x!\n", VencChn, ret);
		return FAILURE;
	} 	

//	usleep(100000);	//100ms
	return SUCCESS;
}

// only mjpeg1 support
/******************************************************************************
 * modify venc channel attr. (mjpeg)
 * stop recv venc picture -> modify venc attr. -> start recv venc picture
******************************************************************************/
//int video_platform_setMjpegEncoder(int channel, TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *pmj)
int setMjpegEncoder(int channel, TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *pmj)
{
	HI_S32 ret = 0;
	VENC_CHN_ATTR_S VencChnAttr;
	HI_U32	u32StatTime;
	VIDEO_RATE_CONTROL_MODE ratectrl = VIDEO_RATE_CONTROL_FIXEDQP;
	VENC_CHN VencChn = (channel * MAX_STREAM_NUM) + stream_type;

	DBG("setMjpegEncoder-VENC%d:chn%d-%d %dx%d, %d fps, quality=%d\n", 
		VencChn, channel, stream_type, pmj->width, pmj->height, pmj->framerate, pmj->quality);

	ret = HI_MPI_VENC_GetChnAttr(VencChn, &VencChnAttr);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_GetChnAttr faild with%#x!\n", ret);
		return -1;
	}

	u32StatTime = 1;
	VencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
	VencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
	
	if (VencChnAttr.stVeAttr.enType == PT_MJPEG) {
//		if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_CBR) {
		if (ratectrl == VIDEO_RATE_CONTROL_CBR) {
			VENC_ATTR_MJPEG_CBR_S stMjpegeCbr;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
			stMjpegeCbr.u32StatTime = u32StatTime;
			stMjpegeCbr.u32SrcFrmRate = vs[channel].framerate;
			stMjpegeCbr.fr32DstFrmRate = pmj->framerate > vs[channel].framerate ? vs[channel].framerate : pmj->framerate;
			stMjpegeCbr.u32BitRate = 2000; //pmj->bitrate;
			stMjpegeCbr.u32FluctuateLevel = 1;
			memcpy(&VencChnAttr.stRcAttr.stAttrMjpegeCbr, &stMjpegeCbr, sizeof(VENC_ATTR_MJPEG_CBR_S));
//		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_VBR) {
		} else if (ratectrl == VIDEO_RATE_CONTROL_VBR) {
			VENC_ATTR_MJPEG_VBR_S   stMjpegVbr;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
			stMjpegVbr.u32StatTime = u32StatTime;
			stMjpegVbr.u32SrcFrmRate = vs[channel].framerate;
			stMjpegVbr.fr32DstFrmRate = pmj->framerate > vs[channel].framerate ? vs[channel].framerate : pmj->framerate;
			stMjpegVbr.u32MaxBitRate = 2000; //pmj->bitrate;
			stMjpegVbr.u32MinQfactor = 50;
			stMjpegVbr.u32MaxQfactor = 95;
			memcpy(&VencChnAttr.stRcAttr.stAttrMjpegeVbr, &stMjpegVbr, sizeof(VENC_ATTR_MJPEG_VBR_S));
//		} else if (pvideo->ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP) {
		} else if (ratectrl == VIDEO_RATE_CONTROL_FIXEDQP) {
			VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;
			VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
			stMjpegeFixQp.u32Qfactor        = pmj->quality;
			stMjpegeFixQp.u32SrcFrmRate      = vs[channel].framerate;;
			stMjpegeFixQp.fr32DstFrmRate = pmj->framerate > vs[channel].framerate ? vs[channel].framerate : pmj->framerate;
			memcpy(&VencChnAttr.stRcAttr.stAttrMjpegeFixQp, &stMjpegeFixQp, sizeof(VENC_ATTR_MJPEG_FIXQP_S));
		} else return -1;
	} else return -1;
		
	ret = HI_MPI_VENC_StopRecvPic(VencChn);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_StopRecvPic(%d) faild with%#x!\n", VencChn, ret);
		return -1;
	} 	
	usleep(10000);	//10ms

	ret = HI_MPI_VENC_SetChnAttr(VencChn, &VencChnAttr);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_SetChnAttr(%d) faild with%#x!\n", VencChn, ret);
		return -1;
	} 	
	
	usleep(10000);	//10ms
	ret = HI_MPI_VENC_StartRecvPic(VencChn);
	if (HI_SUCCESS != ret) {
		SAMPLE_PRT("HI_MPI_VENC_StartRecvPic(%d) faild with%#x!\n", VencChn, ret);
		return -1;
	} 	
	return SUCCESS;
}
int video_platform_setVideoEncoder(int chan, TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *venc)
{
	TaraVideoEncoder *pv;
	int rst = 0;
DBG("%s: stream_type=%d\n", __func__, stream_type);
	if (chan >= num_channel) return -1;
	if (venc == NULL) return -1;
	
	if (stream_type == VIDEO_MAIN_STREAM) pv = &video1[chan];
	else pv = &video2[chan];

	rst |= pv->video ^ venc->video;
	rst |= pv->profile ^ venc->profile;
	rst |= pv->ratecontrol ^ venc->ratecontrol;
	rst |= pv->type ^ venc->type;
	rst |= pv->width ^ venc->width;
	rst |= pv->height ^ venc->height;
	
	memcpy(pv, venc, sizeof(TaraVideoEncoder));
	printf("chn%d:%s %dx%d, %d fps, %d bps, gop=%d, rcmode=%d\n", chan,
			pv->video == 0 ? "Off" : "On",
			pv->width, pv->height, pv->framerate, pv->bitrate, pv->gopsize, pv->ratecontrol);
	if (rst == 0) setVideoEncoder(chan, stream_type, pv);
	else video_platform_restart();
	return SUCCESS;
}

int video_platform_setAllVideoEncoder(TARA_VIDEO_STREAM_TYPE stream_type, TaraVideoEncoder *venc)
{
	TaraVideoEncoder *pv;
	int rst = 0, i;
DBG("%s: stream_type=%d\n", __func__, stream_type);

	if (venc == NULL) return -1;
	if (stream_type == VIDEO_MAIN_STREAM) {
		pv = &video1[0];
		for (i = 0; i < MAX_CAM_CHN-1; i++) {
			rst |= video1[i].video ^ video1[i+1].video;
			rst |= video1[i].profile ^ video1[i+1].profile;
			rst |= video1[i].ratecontrol ^ video1[i+1].ratecontrol;
			rst |= video1[i].type ^ video1[i+1].type;
			rst |= video1[i].width ^ video1[i+1].width;
			rst |= video1[i].height ^ video1[i+1].height;
			if (rst == 1) break;
		}
	} else {
		pv = &video2[0];
		for (i = 0; i < MAX_CAM_CHN-1; i++) {
			rst |= video2[i].video ^ video2[i+1].video;
			rst |= video2[i].profile ^ video2[i+1].profile;
			rst |= video2[i].ratecontrol ^ video2[i+1].ratecontrol;
			rst |= video2[i].type ^ video2[i+1].type;
			rst |= video2[i].width ^ video2[i+1].width;
			rst |= video2[i].height ^ video2[i+1].height;
			if (rst == 1) break;
		}
	}
	printf("video1 %s %dx%d, %d fps, %d bps, gop=%d, rcmode=%d ==\n", pv->video == 0 ? "Off" : "On",
			pv->width, pv->height, pv->framerate, pv->bitrate, pv->gopsize, pv->ratecontrol);
	printf("venc %dx%d, %d fps, %d bps, gop=%d, rcmode=%d ==\n",
			venc->width, venc->height, venc->framerate, venc->bitrate, venc->gopsize, venc->ratecontrol);
			
	rst |= pv->video ^ venc->video;
	rst |= pv->profile ^ venc->profile;
	rst |= pv->ratecontrol ^ venc->ratecontrol;
	rst |= pv->type ^ venc->type;
	rst |= pv->width ^ venc->width;
	rst |= pv->height ^ venc->height;
//printf("============= %d ==================================\n", rst);	
	for(i = 0; i < MAX_CAM_CHN; i++) {
		pv = (stream_type == VIDEO_MAIN_STREAM) ? &video1[i] : &video2[i];
		memcpy(pv, venc, sizeof(TaraVideoEncoder));
		printf("chn%d: %dx%d, %d fps, %d bps, gop=%d, rcmode=%d\n", i,
			pv->width, pv->height, pv->framerate, pv->bitrate, pv->gopsize, pv->ratecontrol);
		if (rst == 0) {
			setVideoEncoder(i, stream_type, pv);
		}
	}
printf("================================================\n");	
	if (rst > 0) {
		video_platform_restart();
	}
	return SUCCESS;
}

int video_platform_setMjpegEncoder(int chan, TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *mj)
{
	TaraMjpegEncoder *pm;
	int rst = 0;

	if (chan >= num_channel) return -1;
	if (mj == NULL) return -1;
	
	pm = &mjpeg1[chan];

	rst |= mj->mjpeg ^ pm->mjpeg;
	rst |= mj->width ^ pm->width;
	rst |= mj->height ^ pm->height;
	
	memcpy(pm, mj, sizeof(TaraMjpegEncoder));
	if (rst == 0)
		setMjpegEncoder(chan, stream_type, pm);
	else video_platform_restart();
	return 0;
}

int video_platform_setAllMjpegEncoder(TARA_VIDEO_STREAM_TYPE stream_type, TaraMjpegEncoder *mj)
{
	TaraMjpegEncoder *pm;
	int rst = 0, i;
	
	if (mj == NULL) return -1;
	for (i = 0; i < MAX_CAM_CHN-1; i++) {
		rst |= mjpeg1[i].mjpeg ^ mjpeg1[i+1].mjpeg;
		rst |= mjpeg1[i].width ^ mjpeg1[i+1].width;
		rst |= mjpeg1[i].height ^ mjpeg1[i+1].height;
//		if (rst == 1) break;
	}
	
	rst |= mj->mjpeg ^ mjpeg1[0].mjpeg;
	rst |= mj->width ^ mjpeg1[0].width;
	rst |= mj->height ^ mjpeg1[0].height;

	for(i = 0; i < MAX_CAM_CHN; i++) {
		pm = &mjpeg1[i];
		memcpy(pm, mj, sizeof(TaraMjpegEncoder));
		if (rst == 0) {
			setMjpegEncoder(i, stream_type, pm);
		}
	}
	
	if (rst > 0) {
		video_platform_restart();
	}
	return SUCCESS;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* getStreamProc(HI_VOID* p)
{
	HI_S32 i;
	AVSERVER_VENC_GETSTREAM_PARA_S* pstPara;
	HI_S32 maxfd = 0;
	struct timeval TimeoutVal;
	fd_set read_fds;
	HI_S32 VencFd[MAX_CAM_CHN];
	HI_CHAR aszFileName[MAX_CAM_CHN][FILE_NAME_LEN];
	FILE *pCloseFile[MAX_CAM_CHN];
	char szFilePostfix[MAX_CAM_CHN][10];
	PAYLOAD_TYPE_E enPayLoadType[MAX_CAM_CHN];
	VENC_CHN_STAT_S stStat;
	VENC_STREAM_S stStream;
	HI_S32 s32Ret;
	VENC_CHN VencChn;
	HI_CHAR aszPathName[MAX_CAM_CHN][64];
	HI_U32 closeFlag[MAX_CAM_CHN]={0};
	HI_CHAR delFileName[64];		
	HI_CHAR path[15] = STREAM_PATH;
	int delFile = 0;
	int stype;
	int qsize = 300;
	int framerate, gopsize;
	int onoff[MAX_CAM_CHN];
	
	pstPara = (AVSERVER_VENC_GETSTREAM_PARA_S*)p;
	stype = pstPara->strtype;	
	for (i = 0; i < MAX_CAM_CHN; i++) 
		onoff[i] = (stype == STREAM_TYPE_VIDEO1) ? video1[i].video : video2[i].video;
	/******************************************
	step 1:  check & prepare save-file & venc-fd
	******************************************/
	memset(&VencFd, 0, sizeof(VencFd));
	mkdir(path, 0777);
DBG("==== %s: %d %s\n", __func__, stype, stype == STREAM_TYPE_VIDEO1 ? "Video1" : "Video2");
	for (i = 0; i < MAX_CAM_CHN; i++) {
		VencChn = (i * MAX_STREAM_NUM) + stype;
		VencFd[i] = 0;
		pCloseFile[i] = NULL;
//		StreamCtrl[VencChn].vq = NULL;
		StreamCtrl[VencChn].channelNo = VencChn;
		StreamCtrl[VencChn].pFile = NULL;
		StreamCtrl[VencChn].serialNo = 0;
		StreamCtrl[VencChn].frameCnt = 0;
		StreamCtrl[VencChn].rb_rdidx = 0;
		StreamCtrl[VencChn].rb_wtidx = 0;
		StreamCtrl[VencChn].lastOffset = 0;
		StreamCtrl[VencChn].lastSize = 0;
		if (stype == STREAM_TYPE_VIDEO1) {
			if (onoff[i] == ENCODER_OFF) continue;
//			StreamCtrl[VencChn].vq = VQueueGetPtr(VIDEO_MAIN_STREAM_KEY(i), qsize);
			enPayLoadType[i] = video1[i].type == VIDEO_CODEC_HEVC ? PT_H265 : PT_H264;
		} else {
			if (onoff[i] == ENCODER_OFF) continue;
//			StreamCtrl[VencChn].vq = VQueueGetPtr(VIDEO_SUB_STREAM_KEY(i), qsize);
			enPayLoadType[i] = video2[i].type == VIDEO_CODEC_HEVC ? PT_H265 : PT_H264;
		}
		snprintf(szFilePostfix[i], sizeof(szFilePostfix[i]), "%s", enPayLoadType[i] == PT_H265 ? ".h265" : ".h264");
		snprintf(aszPathName[i], sizeof(aszPathName[i]), "%s/%d", STREAM_PATH, VencChn);
		rmdir(aszPathName[i]);
		mkdir(aszPathName[i], 0777);
		/* Set Venc Fd. */
		VencFd[i] = HI_MPI_VENC_GetFd(VencChn);
		if (VencFd[i] < 0) {
			SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
			return NULL ;
		}
		if (maxfd <= VencFd[i]) maxfd = VencFd[i];		
	}

	DBG("start to get stream%d %d\n", stype, pstPara->bThreadStart);
	/******************************************
	step 2:  Start to get streams of each channel.
	******************************************/
	pthread_detach(pthread_self());
	if (stype == STREAM_TYPE_VIDEO1) getstrflag = 1;
	else getv2flag = 1;
	
	while (HI_TRUE == pstPara->bThreadStart)
	{
		FD_ZERO(&read_fds);
		
		for (i = 0; i < MAX_CAM_CHN; i++) {
			if (VencFd[i] == 0) continue;
			FD_SET(VencFd[i], &read_fds);
		}

		TimeoutVal.tv_sec  = 0;
		TimeoutVal.tv_usec = 300000;
		s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
		if (s32Ret < 0) {
			SAMPLE_PRT("select failed!\n");
			break;
		} else if (s32Ret == 0) {
			if (VencFd[0] != 0)
				SAMPLE_PRT("stype=%d, get venc stream time out\n", stype);
			continue;
		} else {
			for (i = 0; i < MAX_CAM_CHN; i++) {
				VencChn = (i * MAX_STREAM_NUM) + stype;
				if (VencFd[i] == 0) continue;
				if (FD_ISSET(VencFd[i], &read_fds)) {
					/*******************************************************
					step 2.1 : query how many packs in one-frame stream.
					*******************************************************/
					memset(&stStream, 0, sizeof(stStream));
					s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
					if (HI_SUCCESS != s32Ret) {
						SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", VencChn, s32Ret);
						break;
					}

					/*******************************************************
					step 2.2 : malloc corresponding number of pack nodes.
					*******************************************************/
					stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
					if (NULL == stStream.pstPack) {
						SAMPLE_PRT("malloc stream pack failed!\n");
						break;
					}
                    
					/*******************************************************
					step 2.3 : call mpi to get one-frame stream
					*******************************************************/
					stStream.u32PackCount = stStat.u32CurPacks;
					s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, HI_TRUE);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
						goto release_stream;
					}
					if (stStream.u32PackCount > 3) {
						StreamCtrl[VencChn].frameCnt = 0;
						if (StreamCtrl[VencChn].serialNo == 0) StreamCtrl[VencChn].serialNo = GetSysTimeBySec();
						else {
							closeFlag[i] = 1;
							pCloseFile[i] = StreamCtrl[VencChn].pFile;
							StreamCtrl[VencChn].lastOffset = 0;
							StreamCtrl[VencChn].lastSize = 0;
							StreamCtrl[VencChn].serialNo++;							
						}
						snprintf(aszFileName[i], sizeof(aszFileName[i]), "%s/%d%s", aszPathName[i], StreamCtrl[VencChn].serialNo, szFilePostfix[i]);
						StreamCtrl[VencChn].pFile = fopen(aszFileName[i], "wb");
						if (!StreamCtrl[VencChn].pFile) {
							SAMPLE_PRT("open file[%s] failed!\n", aszFileName[i]);
							return NULL;
						}
					} else if (closeFlag[i] == 1) {
						closeFlag[i] = 0;											
						fclose(pCloseFile[i]);
						pCloseFile[i] = NULL;
						framerate = (stype == STREAM_TYPE_VIDEO1) ? video1[i].framerate : video2[i].framerate;
						gopsize = (stype == STREAM_TYPE_VIDEO1) ? video1[i].gopsize : video2[i].gopsize;
													
						delFile = (int)(((10 * framerate) + ((double)gopsize / 2)) / gopsize);
						if (delFile == 0) delFile = 1;
						snprintf(delFileName, sizeof(delFileName), "%s/%d%s", aszPathName[i], (StreamCtrl[VencChn].serialNo - delFile), szFilePostfix[i]);
						unlink(delFileName);
					}

					/*******************************************************
					step 2.4 : save frame to file
					*******************************************************/
					s32Ret = video_platform_saveStream(enPayLoadType[i], &stStream, &StreamCtrl[VencChn]);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						SAMPLE_PRT("save stream failed!\n");
						break;
					}
//					printf("video[%d] PTS = %llu\n", i, StreamCtrl[i].timestamp);

release_stream:
					/*******************************************************
					step 2.5 : release stream
					*******************************************************/
					s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						break;
					}
					/*******************************************************
					step 2.6 : free pack nodes
					*******************************************************/
					free(stStream.pstPack);
					stStream.pstPack = NULL;
				}
			}
		}
	}
	/*******************************************************
	* step 3 : close save-file
	*******************************************************/
DBG("close save file\n");
//	pstPara->bThreadStart = HI_FALSE;

	for (i = 0; i < MAX_CAM_CHN; i++) {
		VencChn = (i * MAX_STREAM_NUM) + stype;
		if (StreamCtrl[VencChn].pFile != NULL) {
			DBG("venc%d close str%d \n", VencChn, i);
			fclose(StreamCtrl[VencChn].pFile);
			StreamCtrl[VencChn].pFile = NULL;
		}
		if (pCloseFile[i] != NULL) {
			DBG("venc%d close file\n", VencChn);
			fclose(pCloseFile[i]);
			pCloseFile[i] = NULL;
		}
		CleanRBuf(VencChn);
	}
DBG("%s bye~\n", __func__);
	//system("rm -rf /tmp/stream");
	//sync();
	if (stype == STREAM_TYPE_VIDEO1) getstrflag = 0;
	else getv2flag = 0;
	return NULL;
}

HI_VOID* getMjpegProc(HI_VOID* p)
{
	HI_S32 i;
	AVSERVER_VENC_GETSTREAM_PARA_S* pstPara;
	HI_S32 maxfd = 0;
	struct timeval TimeoutVal;
	fd_set read_fds;
	HI_S32 VencFd[MAX_CAM_CHN];
	HI_CHAR aszFileName[MAX_CAM_CHN][FILE_NAME_LEN];
	FILE *pCloseFile[MAX_CAM_CHN];
	char szFilePostfix[]=".mjp";
	PAYLOAD_TYPE_E enPayLoadType=PT_MJPEG;
	VENC_CHN_STAT_S stStat;
	VENC_STREAM_S stStream;
	HI_S32 s32Ret;
	VENC_CHN VencChn;
	HI_CHAR aszPathName[MAX_CAM_CHN][64];
	HI_U32 closeFlag[MAX_CAM_CHN]={0};
	HI_CHAR delFileName[64];		
	HI_CHAR path[15] = STREAM_PATH;
	int delFile = 0;
//	HI_S32 s32ChnTotal, s32ViTotal;
	int qsize = 300;
	int framerate, gopsize;
	int stype = 2;
	int onoff[MAX_CAM_CHN];
	int framecnt=0;
	
	pstPara = (AVSERVER_VENC_GETSTREAM_PARA_S*)p;
	for (i = 0; i < MAX_CAM_CHN; i++) 
		onoff[i] = mjpeg1[i].mjpeg;

	/******************************************
	step 1:  check & prepare save-file & venc-fd
	******************************************/
	memset(&VencFd, 0, sizeof(VencFd));
	mkdir(path, 0777);
	for (i = 0; i < MAX_CAM_CHN; i++) {
		VencChn = (i * MAX_STREAM_NUM) + stype;
		VencFd[i] = 0;
		pCloseFile[i] = NULL;
//		StreamCtrl[VencChn].vq = NULL;
		StreamCtrl[VencChn].channelNo = VencChn;
		StreamCtrl[VencChn].pFile = NULL;
		StreamCtrl[VencChn].serialNo = 0;
		StreamCtrl[VencChn].frameCnt = 0;
		StreamCtrl[VencChn].rb_rdidx = 0;
		StreamCtrl[VencChn].rb_wtidx = 0;
		StreamCtrl[VencChn].lastOffset = 0;
		StreamCtrl[VencChn].lastSize = 0;
		
		if (onoff[i] == ENCODER_OFF) continue;
//		StreamCtrl[VencChn].vq = VQueueGetPtr(VIDEO_MJPG_STREAM_KEY(i), qsize);
		snprintf(aszPathName[i], sizeof(aszPathName[i]), "%s/%d", STREAM_PATH, VencChn);
		rmdir(aszPathName[i]);
		mkdir(aszPathName[i], 0777);
//			printf("path: %s\n", aszPathName[i]);
		/* Set Venc Fd. */
		VencFd[i] = HI_MPI_VENC_GetFd(VencChn);
		if (VencFd[i] < 0) {
			SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
			return NULL ;
		}		
		if (maxfd <= VencFd[i]) maxfd = VencFd[i];
	}

	DBG("start to get stream %d\n", pstPara->bThreadStart);
	/******************************************
	step 2:  Start to get streams of each channel.
	******************************************/
	pthread_detach(pthread_self());
	getmjpflag = 1;
	while (HI_TRUE == pstPara->bThreadStart)
	{
		FD_ZERO(&read_fds);
		
		for (i = 0; i < MAX_CAM_CHN; i++) {
			if (VencFd[i] == 0)	continue;
			FD_SET(VencFd[i], &read_fds);
		}

		TimeoutVal.tv_sec  = 0;
		TimeoutVal.tv_usec = 500000;
		s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
		if (s32Ret < 0) {
			SAMPLE_PRT("select failed!\n");
			break;
		} else if (s32Ret == 0) {
			if (VencFd[i] != 0)
				SAMPLE_PRT("get mjpeg stream time out\n");
			continue;
		} else {
			for (i = 0; i < MAX_CAM_CHN; i++) {
				VencChn = (i * MAX_STREAM_NUM) + stype;
				if (VencFd[i] == 0) continue;
				if (FD_ISSET(VencFd[i], &read_fds)) {
					/*******************************************************
					step 2.1 : query how many packs in one-frame stream.
					*******************************************************/
					memset(&stStream, 0, sizeof(stStream));
					s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
					if (HI_SUCCESS != s32Ret) {
						SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", VencChn, s32Ret);
						break;
					}

					/*******************************************************
					step 2.2 : malloc corresponding number of pack nodes.
					*******************************************************/
					stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
					if (NULL == stStream.pstPack) {
						SAMPLE_PRT("malloc stream pack failed!\n");
						break;
					}
                    
					/*******************************************************
					step 2.3 : call mpi to get one-frame stream
					*******************************************************/
					stStream.u32PackCount = stStat.u32CurPacks;
					s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, HI_TRUE);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
						goto release_mjpeg;
					}
					//goto release_mjpeg;

					if(dbgflag == 1) {
						if (VencChn == 2 && ++framecnt >= 4) {
							struct timespec ts;
							framecnt = 0;
							clock_gettime(CLOCK_REALTIME, &ts);
							printf("[venc]:systime=%llu %llu, ", ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000, ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000);
							printf("pts=%llu %llu [%ld.%ld]\n", stStream.pstPack[0].u64PTS/1000000, stStream.pstPack[0].u64PTS%1000000, 
								(long)(((ts.tv_sec * 1000000LLU)+ts.tv_nsec / 1000) - stStream.pstPack[0].u64PTS)/1000,
								(long)(((ts.tv_sec * 1000000LLU)+ts.tv_nsec / 1000) - stStream.pstPack[0].u64PTS)%1000);
						}
					}

					if (StreamCtrl[VencChn].frameCnt >= mjpeg1[i].framerate || StreamCtrl[VencChn].serialNo == 0) {
						if (StreamCtrl[VencChn].serialNo == 0) StreamCtrl[VencChn].serialNo = GetSysTimeBySec();
						else {
							StreamCtrl[VencChn].frameCnt = 0;
							closeFlag[i] = 1;
							pCloseFile[i] = StreamCtrl[VencChn].pFile;
							StreamCtrl[VencChn].lastOffset = 0;
							StreamCtrl[VencChn].lastSize = 0;
							StreamCtrl[VencChn].serialNo++;
						}
						snprintf(aszFileName[i], sizeof(aszFileName[i]), "%s/%d.mjp", aszPathName[i], StreamCtrl[VencChn].serialNo);
						StreamCtrl[VencChn].pFile = fopen(aszFileName[i], "wb");
						if (!StreamCtrl[VencChn].pFile) {
							SAMPLE_PRT("open file[%s] failed!\n", aszFileName[i]);
							return NULL;
						}
					} else if (closeFlag[i] == 1) {
						closeFlag[i] = 0;											
						fclose(pCloseFile[i]);
						pCloseFile[i] = NULL;
						snprintf(delFileName, sizeof(delFileName), "%s/%d.mjp", aszPathName[i], (StreamCtrl[VencChn].serialNo - 6));
						unlink(delFileName);
					}

					/*******************************************************
					step 2.4 : save frame to file
					*******************************************************/
					s32Ret = video_platform_saveStream(enPayLoadType, &stStream, &StreamCtrl[VencChn]);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						SAMPLE_PRT("save stream failed!\n");
						break;
					}
//					printf("video[%d] PTS = %llu\n", i, StreamCtrl[i].timestamp);

release_mjpeg:
					/*******************************************************
					step 2.5 : release stream
					*******************************************************/
					s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
					if (HI_SUCCESS != s32Ret) {
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						break;
					}
					/*******************************************************
					step 2.6 : free pack nodes
					*******************************************************/
					free(stStream.pstPack);
					stStream.pstPack = NULL;
				}
			}
		}
	}
	/*******************************************************
	* step 3 : close save-file
	*******************************************************/
DBG("close save file\n");
//	pstPara->bThreadStart = HI_FALSE;

	for (i = 0; i < MAX_CAM_CHN; i++) {
		VencChn = (i * MAX_STREAM_NUM) + stype;
		if (StreamCtrl[VencChn].pFile != NULL) {
			DBG("close str%d \n", i);
			fclose(StreamCtrl[VencChn].pFile);
			StreamCtrl[VencChn].pFile = NULL;
		}
		if (pCloseFile[i] != NULL) {
			fclose(pCloseFile[i]);
			pCloseFile[i] = NULL;
		}
		CleanRBuf(StreamCtrl[VencChn].channelNo);
	}
DBG("%s bye~\n", __func__);
//	system("rm -rf /tmp/stream");
//	sync();
	getmjpflag = 0;
	return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
int video_platform_getStreamStart(int vi_num, int stream_num)
{
	gs_stParam.bThreadStart = HI_TRUE;
	gs_stParam.strtype = STREAM_TYPE_VIDEO1;
	gs_v2Param.bThreadStart = HI_TRUE;
	gs_v2Param.strtype = STREAM_TYPE_VIDEO2;
	gs_mjParam.bThreadStart = HI_TRUE;
	gs_mjParam.strtype = STREAM_TYPE_MJPEG1;
	pthread_create(&gs_VencPid, 0, getStreamProc, (HI_VOID*)&gs_stParam);
	pthread_create(&gs_Video2Pid, 0, getStreamProc, (HI_VOID*)&gs_v2Param);
	pthread_create(&gs_MjpgPid, 0, getMjpegProc, (HI_VOID*)&gs_mjParam);
	return 0;
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
int video_platform_getStreamStop(void)
{
	DBG("%s\n", __func__);
	if (HI_TRUE == gs_stParam.bThreadStart) {
		gs_stParam.bThreadStart = HI_FALSE;
		pthread_join(gs_VencPid, 0);
	}
	if (HI_TRUE == gs_v2Param.bThreadStart) {
		gs_v2Param.bThreadStart = HI_FALSE;
		pthread_join(gs_Video2Pid, 0);
	}
	if (HI_TRUE == gs_mjParam.bThreadStart) {
		gs_mjParam.bThreadStart = HI_FALSE;
		pthread_join(gs_MjpgPid, 0);
	}
	return SUCCESS;
}

// need video source status ?
int video_platform_init(void)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VB_CONF_S stVbConf;
	HI_U32 u32BlkSize;
	SIZE_S stSize;	
	SAMPLE_VI_MODE_E enViMode =  SAMPLE_VI_MODE_8_1080P;
	PIC_SIZE_E enSize[MAX_STREAM_NUM] = {PIC_HD1080, PIC_HD720, PIC_960H};
	HI_S32 s32VpssGrpCnt = MAX_CAM_CHN;
	HI_S32 s32ChnNum = MAX_STREAM_NUM;
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
	VPSS_CHN_MODE_S stVpssChnMode;
	VI_USERPIC_ATTR_S stUsrPic;
	int i;
  HI_U64 uPTS;

	memcpy(&vs, &vs_default, sizeof(vs));
	memset(&video1, 0, sizeof(video1));
	memset(&video2, 0, sizeof(video2));
	memset(&mjpeg1, 0, sizeof(mjpeg1));
	video_platform_loadConfigFile();

	uPTS = GetSysTimeByUsec();
	HI_MPI_SYS_InitPtsBase(uPTS);

	/******************************************
		step  1: init sys variable
	******************************************/
	memset(&stVbConf,0,sizeof(VB_CONF_S));
	for (i = 0; i < s32ChnNum; i++) {
//		if (vs_hw[0].vpsschn[i].enPhyChn == 0)
//			continue;
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
                enSize[i], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH, COMPRESS_MODE_SEG);
		stVbConf.astCommPool[i].u32BlkSize = u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt = vs_hw[0].vpsschn[i].vbBlkCnt;
DBG("vb%d: %llu %u\n", i, stVbConf.astCommPool[i].u32BlkSize, stVbConf.astCommPool[i].u32BlkCnt);
	}
	stVbConf.u32MaxPoolCnt = s32ChnNum;

	/******************************************
		step 2: mpp system init.
	******************************************/
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != HI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x!\n", s32Ret);
		goto END_VENC_1080P_CLASSIC_0;
	}
  
  /******************************************
		step 3: start vi dev & chn to capture
	******************************************/
	s32Ret = SAMPLE_COMM_VI_Start(enViMode, gs_enNorm);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("start vi failed with %#x!\n", s32Ret);
		goto END_VENC_1080P_CLASSIC_1;
	}
	
	/******************************************
		step 4: start vpss and vi bind vpss
	******************************************/
	s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		goto END_VENC_1080P_CLASSIC_1;
	}

	stSize.u32Width = vs_hw[0].maxWidth;
	stSize.u32Height = vs_hw[0].maxHeight;
	if (vs_hw[0].vpsschn[3].enPhyChn == 1)
		s32ChnNum += 1;
	memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
	stVpssGrpAttr.u32MaxW 	= stSize.u32Width;
	stVpssGrpAttr.u32MaxH 	= stSize.u32Height;
	stVpssGrpAttr.bNrEn 	= HI_TRUE;
	stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	stVpssGrpAttr.enPixFmt 	= SAMPLE_PIXEL_FORMAT;
	s32Ret = SAMPLE_COMM_VPSS_Start(s32VpssGrpCnt, &stSize, s32ChnNum , &stVpssGrpAttr);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("Start Vpss failed!\n");
		goto END_VENC_1080P_CLASSIC_2;
	}
	
	s32Ret = SAMPLE_COMM_VI_BindVpss(enViMode);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("Vi bind Vpss failed!\n");
		goto END_VENC_1080P_CLASSIC_3;
	}

	for (VpssGrp = 0; VpssGrp < s32VpssGrpCnt; VpssGrp++) {
	#if 1
		for (VpssChn = 0; VpssChn < VPSS_MAX_PHY_CHN_NUM; VpssChn++) {
			if (vs_hw[0].vpsschn[VpssChn].enPhyChn == 0 && VpssChn != 3) {
				DBG("Disable Vpss%d:Chn%d\n", VpssGrp, VpssChn);
				HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
			} else {
				if (VpssChn != 3) continue;
				memset(&stVpssChnMode, 0, sizeof(stVpssChnMode));
				stVpssChnMode.bDouble 	     = HI_FALSE;
				stVpssChnMode.enChnMode	= VPSS_CHN_MODE_USER;
				stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
				stVpssChnMode.stFrameRate.s32SrcFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrSrcfps;
				stVpssChnMode.stFrameRate.s32DstFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrDstfps;
				stVpssChnMode.u32Width = vs_hw[0].vpsschn[VpssChn].vpssModeWidth;
				stVpssChnMode.u32Height = vs_hw[0].vpsschn[VpssChn].vpssModeHeight;
				stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
				s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
				if (HI_SUCCESS != s32Ret) {
					DBG("%s: HI_MPI_VPSS_SetChnMode failed, Error(%#x),VpssGrp(%d),VpssChn(%d)!\n", __func__, s32Ret, VpssGrp, VpssChn); 
				}
			}
		}
	#else
		VpssChn = 3;
		if (vs_hw[0].vpsschn[VpssChn].enPhyChn == 1) {
			memset(&stVpssChnMode, 0, sizeof(stVpssChnMode));
			stVpssChnMode.bDouble 	     = HI_FALSE;
			stVpssChnMode.enChnMode	= VPSS_CHN_MODE_USER;
			stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
			stVpssChnMode.stFrameRate.s32SrcFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrSrcfps;
			stVpssChnMode.stFrameRate.s32DstFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrDstfps;
			stVpssChnMode.u32Width = vs_hw[0].vpsschn[VpssChn].vpssModeWidth;
			stVpssChnMode.u32Height = vs_hw[0].vpsschn[VpssChn].vpssModeHeight;
			stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
			s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
			if (HI_SUCCESS != s32Ret) {
				DBG("%s: HI_MPI_VPSS_SetChnMode failed, Error(%#x),VpssGrp(%d),VpssChn(%d)!\n", __func__, s32Ret, VpssGrp, VpssChn); 
			}
		}
	#endif
	}
	
	stUsrPic.bPub = HI_TRUE;
	stUsrPic.enUsrPicMode = VI_USERPIC_MODE_BGC;
	stUsrPic.unUsrPic.stUsrPicBg.u32BgColor = 0x0000ff;
	s32Ret = HI_MPI_VI_SetUserPic(0, &stUsrPic);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VI_SetUserPic failed with %#x!\n", s32Ret);
	}
	int qsize=300;
	for(i=0; i<MAX_CAM_CHN; i++) {
		StreamCtrl[i*MAX_STREAM_NUM].vq = VQueueGetPtr(VIDEO_MAIN_STREAM_KEY(i), qsize);
		StreamCtrl[(i*MAX_STREAM_NUM)+1].vq = VQueueGetPtr(VIDEO_SUB_STREAM_KEY(i), qsize);
		StreamCtrl[(i*MAX_STREAM_NUM)+2].vq = VQueueGetPtr(VIDEO_MJPG_STREAM_KEY(i), qsize);
	}
	return s32Ret;
	
END_VENC_1080P_CLASSIC_3:    //unbind vpss and vi
	SAMPLE_COMM_VI_UnBindVpss(enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
	SAMPLE_COMM_VPSS_Stop(s32VpssGrpCnt,s32ChnNum);
END_VENC_1080P_CLASSIC_1:	//vi stop
	SAMPLE_COMM_VI_Stop(enViMode);
END_VENC_1080P_CLASSIC_0:	//system exit
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

static int videoEncoderStart(int stream_type, TaraVideoEncoder *pvideo)
{
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0;
	VPSS_CHN_MODE_S stVpssChnMode;
	HI_S32 VpssGrpCnt;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32Depth = 2;

	if (!pvideo) return -1;
	VpssGrpCnt = MAX_CAM_CHN;
//printf("%s: stream_type = %d\n", __func__, stream_type);
#if 0
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (stream_type == VIDEO_MAIN_STREAM)
			memcpy(&video1[VpssGrp], &pvideo[VpssGrp], sizeof(video1[VpssGrp]));
		else if (stream_type == VIDEO_SUB_STREAM)
			memcpy(&video2[VpssGrp], &pvideo[VpssGrp], sizeof(video2[VpssGrp]));
		else return -1;
	}
#endif
	VpssChn = vs_hw[0].str[stream_type].vpssphychn;	
DBG("%s:%d VpssChn=%d\n", __func__, stream_type, VpssChn);
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (pvideo[VpssGrp].video == ENCODER_OFF)
			continue;
		VencChn = (VpssGrp * MAX_STREAM_NUM) + stream_type;
		memset(&stVpssChnMode, 0, sizeof(stVpssChnMode));
		stVpssChnMode.bDouble 	     = HI_FALSE;
		stVpssChnMode.enChnMode	= VPSS_CHN_MODE_USER;
		stVpssChnMode.enCompressMode = vs_hw[0].vpsschn[VpssChn].vpssModeCompressMode;
		stVpssChnMode.stFrameRate.s32SrcFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrSrcfps;
		stVpssChnMode.stFrameRate.s32DstFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrDstfps;
		stVpssChnMode.u32Width = vs_hw[0].vpsschn[VpssChn].vpssModeWidth;
		stVpssChnMode.u32Height = vs_hw[0].vpsschn[VpssChn].vpssModeHeight;
		stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		if (vs_hw[0].vpsschn[VpssChn].UseUiResolution == 1) {
			stVpssChnMode.u32Width			 = pvideo[VpssGrp].width;
			stVpssChnMode.u32Height 		 = pvideo[VpssGrp].height;
		}
		DBG("%s: VpssChn%d %dx%d\n", __func__, VpssChn, stVpssChnMode.u32Width, stVpssChnMode.u32Height);
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
		if (HI_SUCCESS != s32Ret) {
			DBG("%s: HI_MPI_VPSS_SetChnMode failed, Error(%#x),VpssGrp(%d),VpssChn(%d)!\n", __func__, s32Ret, VpssGrp, VpssChn); 
		}
			
		if (HI_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32Depth) != HI_SUCCESS)
			DBG("set VpssGrp%d VpssChn%d depth error\n", VpssGrp, VpssChn);

		s32Ret = video_platform_initVideoEncoder(VencChn, &pvideo[VpssGrp], vs_hw[0].str[stream_type].enGopMode, vs[VpssGrp].framerate);
		if (SUCCESS != s32Ret) {
			SAMPLE_PRT("Start Venc[%d] failed!\n", VencChn);
			goto ERR_VIDEO_ENCODER_START;		// ??? Need to initialize the next one ???
		}

		if (vs_hw[0].str[stream_type].bindvpss == 1) {
			DBG("Venc %d bind vpssGrp%d %d\n", VencChn, VpssGrp, VpssChn);
			s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
			if (HI_SUCCESS != s32Ret) {
				SAMPLE_PRT("Venc[%d] bind vpssGrp[%d][%d] failed! %#x\n", VencChn, VpssGrp, VpssChn, s32Ret);
				goto ERR_VIDEO_ENCODER_START;		// ??? Need to initialize the next one ???
			}
		}
	}
#if 0
	printf("====================================================\n");
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (stream_type == 0) {
			printf("chn%d: video1:%s, %s, %dx%d, %d fps, %d bps\n", VpssGrp, 
				video1[VpssGrp].video == ENCODER_ON ? "On" : "Off", 
				video1[VpssGrp].type == VIDEO_CODEC_HEVC ? "H265" : "H264",
				video1[VpssGrp].width, video1[VpssGrp].height, video1[VpssGrp].framerate, video1[VpssGrp].bitrate);
		} else {
			printf("chn%d: video2:%s, %s, %dx%d, %d fps, %d bps\n", VpssGrp, 
				video2[VpssGrp].video == ENCODER_ON ? "On" : "Off", 
				video2[VpssGrp].type == VIDEO_CODEC_HEVC ? "H265" : "H264",
				video2[VpssGrp].width, video2[VpssGrp].height, video2[VpssGrp].framerate, video2[VpssGrp].bitrate);
		}
	}
	printf("==================================================\n");
#endif
	return SUCCESS;

ERR_VIDEO_ENCODER_START:
	SAMPLE_PRT("Start Venc failed!\n");
	// ??? close all videoN stream ???
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (pvideo[VpssGrp].video == ENCODER_OFF)
			continue;
		VencChn = (VpssGrp * MAX_STREAM_NUM) + stream_type;
		if (vs_hw[0].str[stream_type].bindvpss == 1)		
			SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		SAMPLE_COMM_VENC_Stop(VencChn);
	}
	return s32Ret;
}

int video_platform_videoEncoderStart(TARA_VIDEO_STREAM_TYPE stream_type, VideoEncoder_t *pvideo)
{
	int i, ret;
//	TaraVideoEncoder venc[num_channel];
	
	if (!pvideo) return -1;
	if (stream_type != VIDEO_MAIN_STREAM && stream_type != VIDEO_SUB_STREAM) return -1;
	DBG("============================================\n");
	for (i = 0; i < num_channel; i++) {
		if (stream_type == VIDEO_MAIN_STREAM) {
			memcpy(&video1[i], &pvideo[i].venc, sizeof(TaraVideoEncoder));
			DBG("VS%dVideo1 %s %s %dx%d %dfps %d bps qp=%d\n", i, video1[i].video == ENCODER_ON ? "On" : "Off",
				video1[i].type == VIDEO_CODEC_HEVC ? "H265" : "H264", video1[i].width, video1[i].height, video1[i].framerate, video1[i].bitrate, video1[i].qp);
		} else if (stream_type == VIDEO_SUB_STREAM) {
			memcpy(&video2[i], &pvideo[i].venc, sizeof(TaraVideoEncoder));
			DBG("VS%dVideo2 %s %s %dx%d %dfps %d bps qp=%d\n", i, video2[i].video == ENCODER_ON ? "On" : "Off",
				video2[i].type == VIDEO_CODEC_HEVC ? "H265" : "H264", video2[i].width, video2[i].height, video2[i].framerate, video2[i].bitrate, video2[i].qp);
		}
	}
	DBG("============================================\n");
	if (stream_type == VIDEO_MAIN_STREAM) ret = videoEncoderStart(stream_type, video1);
	else ret = videoEncoderStart(stream_type, video2);

	return ret;
}

static int mjpegEncoderStart(int stream_type, TaraMjpegEncoder *pmj)
{
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0;
	VPSS_CHN_MODE_S stVpssChnMode;
	HI_S32 VpssGrpCnt;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32Depth = 2;

	if (stream_type == VIDEO_MJPEG_STREAM) {
		if (!pmj) return -1;
		VpssGrpCnt = MAX_CAM_CHN;
	} else return -1;
	
	VpssChn = vs_hw[0].str[stream_type].vpssphychn;	
DBG("%s:%d VpssChn=%d\n", __func__, stream_type, VpssChn);

	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
//		memcpy(&mjpeg1[VpssGrp], &pmj[VpssGrp], sizeof(mjpeg1[VpssGrp]));
		
		if (pmj[VpssGrp].mjpeg == ENCODER_OFF) 
			continue;
		VencChn = (VpssGrp * MAX_STREAM_NUM) + stream_type;
		memset(&stVpssChnMode, 0, sizeof(stVpssChnMode));
		stVpssChnMode.bDouble 	     = HI_FALSE;
		stVpssChnMode.enChnMode	= VPSS_CHN_MODE_USER;
		stVpssChnMode.enCompressMode = vs_hw[0].vpsschn[VpssChn].vpssModeCompressMode;
		stVpssChnMode.stFrameRate.s32SrcFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrSrcfps;
		stVpssChnMode.stFrameRate.s32DstFrmRate = vs_hw[0].vpsschn[VpssChn].vpssAttrDstfps;
		stVpssChnMode.u32Width = vs_hw[0].vpsschn[VpssChn].vpssModeWidth;
		stVpssChnMode.u32Height = vs_hw[0].vpsschn[VpssChn].vpssModeHeight;
		stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		if (vs_hw[0].vpsschn[VpssChn].UseUiResolution == 1) {
			stVpssChnMode.u32Width			 = pmj[VpssGrp].width;
			stVpssChnMode.u32Height 		 = pmj[VpssGrp].height;
		}
DBG("VPSS_SetChnMode: %d:%d %dx%d %dfps\n", VpssGrp, VpssChn, stVpssChnMode.u32Width, stVpssChnMode.u32Height, stVpssChnMode.stFrameRate.s32SrcFrmRate);
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
		if (HI_SUCCESS != s32Ret) {
			DBG("%s: HI_MPI_VPSS_SetChnMode failed, Error(%#x),VpssGrp(%d),VpssChn(%d)!\n", __func__, s32Ret, VpssGrp, VpssChn); 
		}

    if (HI_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32Depth) != HI_SUCCESS)
			DBG("set VpssGrp%d VpssChn%d depth error\n", VpssGrp, VpssChn);

		s32Ret = video_platform_initMjpegEncoder(VencChn, &pmj[VpssGrp], vs_hw[0].str[stream_type].enGopMode, vs[VpssGrp].framerate);
		if (HI_SUCCESS != s32Ret) {
			SAMPLE_PRT("Start Venc[%d] failed!\n", VencChn);
			goto ERR_MJPEG_ENCODER_START;		// ??? Need to initialize the next one ???
		}

		if (vs_hw[0].str[stream_type].bindvpss == 1) {
			DBG("Venc %d bind vpssGrp%d %d\n", VencChn, VpssGrp, VpssChn);
			s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
			if (HI_SUCCESS != s32Ret) {
				SAMPLE_PRT("Venc[%d] bind vpssGrp[%d][%d] failed! %#x\n", VencChn, VpssGrp, VpssChn, s32Ret);
				goto ERR_MJPEG_ENCODER_START;		// ??? Need to initialize the next one ???
			}
		}
	} // end of VpssGrp >= VpssGrpCnt
#if 0
	printf("==================================================\n");
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		printf("chn%d: mjpeg1:%s, %dx%d, %d fps, %d\n", VpssGrp, 
		mjpeg1[VpssGrp].mjpeg == ENCODER_ON ? "On" : "Off", 
		mjpeg1[VpssGrp].width, mjpeg1[VpssGrp].height, mjpeg1[VpssGrp].framerate, mjpeg1[VpssGrp].quality);
	}
	printf("==================================================\n");
#endif
	return SUCCESS;

ERR_MJPEG_ENCODER_START:
	SAMPLE_PRT("Start Venc%d failed!\n", VencChn);
	// ??? close all mjpeg1 stream ???
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (pmj[VpssGrp].mjpeg == ENCODER_OFF) 
			continue;
		VencChn = (VpssGrp * MAX_STREAM_NUM) + stream_type;
		if (vs_hw[0].str[stream_type].bindvpss == 1)		
			SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		SAMPLE_COMM_VENC_Stop(VencChn);
	}
	return s32Ret;
}

int video_platform_mjpegEncoderStart(TARA_VIDEO_STREAM_TYPE stream_type, MjpegEncoder_t *pmj)
{
	int i, ret;
//	TaraMjpegEncoder mjenc[num_channel];
	
	if (!pmj) return -1;
	if (stream_type != VIDEO_MJPEG_STREAM) return -1;
	
	for (i = 0; i < num_channel; i++) {
		memcpy(&mjpeg1[i], &pmj[i].mjenc, sizeof(TaraMjpegEncoder));
//		memcpy(&mjenc[i], &pmj[i].mjenc, sizeof(TaraMjpegEncoder));
	}
	ret = mjpegEncoderStart(stream_type, mjpeg1);
	return ret;
}

int video_platform_exit(void)
{
	SAMPLE_VI_MODE_E enViMode =  SAMPLE_VI_MODE_8_1080P;
	VPSS_GRP VpssGrp = 0;
	HI_S32 VpssGrpCnt = MAX_CAM_CHN;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0; 
	HI_S32 s32ChnNum = 0;
//	int i;

	video_platform_getRawFrameStop();
	video_platform_getStreamStop(); 
	
	system("rm -rf /tmp/stream");   

	for (VpssChn = 0; VpssChn < VPSS_MAX_PHY_CHN_NUM; VpssChn++) {
		if (vs_hw[0].vpsschn[VpssChn].enPhyChn == 1) s32ChnNum++;
	}
	
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		if (video1[VpssGrp].video == ENCODER_ON) {
			VpssChn = vs_hw[0].str[0].vpssphychn;
			VencChn = VpssGrp * MAX_STREAM_NUM;
			VQueueClean(StreamCtrl[VencChn].vq);
			if (vs_hw[0].str[0].bindvpss == 1)
				SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
		}

		if (video2[VpssGrp].video == ENCODER_ON) {
			VpssChn = vs_hw[0].str[1].vpssphychn;
			VencChn = (VpssGrp * MAX_STREAM_NUM) + 1;
			VQueueClean(StreamCtrl[VencChn].vq);
			if (vs_hw[0].str[1].bindvpss == 1)
				SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
		}

		if (mjpeg1[VpssGrp].mjpeg == ENCODER_ON) {
			VpssChn = vs_hw[0].str[2].vpssphychn;
			VencChn = (VpssGrp * MAX_STREAM_NUM) + 2;
			VQueueClean(StreamCtrl[VencChn].vq);
			if (vs_hw[0].str[2].bindvpss == 1)
				SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
		}
	}
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		VQueueClean(StreamCtrl[VpssGrp * MAX_STREAM_NUM].vq);
		VQueueClean(StreamCtrl[(VpssGrp * MAX_STREAM_NUM)+1].vq);
		VQueueClean(StreamCtrl[(VpssGrp * MAX_STREAM_NUM)+2].vq);
	}

	SAMPLE_COMM_VI_UnBindVpss(enViMode);
	SAMPLE_COMM_VPSS_Stop(VpssGrpCnt,s32ChnNum);
	SAMPLE_COMM_VI_Stop(enViMode);
	SAMPLE_COMM_SYS_Exit();
	return SUCCESS;
}

int video_platform_restart(void)
{
	int i, tmp;
	int s32Ret = SUCCESS;
	VPSS_GRP VpssGrp = 0;
	HI_S32 VpssGrpCnt = MAX_CAM_CHN;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0; 

	system("killall -2 streaming_server");
	usleep(250000);
	system("killall -9 streaming_server");	
	usleep(250000);
	system("killall -9 streaming_server");	
//printf("==== getStrCnt=%d ====\n", getStrCnt);
	// rebuild pipleline
	video_platform_getStreamStop();	
	video_platform_getRawFrameStop();
#if 1
	while(getstrflag == 1 || getmjpflag == 1 || getv2flag == 1) {
		printf("%d %d %d\n", getstrflag, getv2flag, getmjpflag);
		usleep(500000);
	}
#else
	tmp=1;
	while(tmp) {
		tmp=0;
		for(i=0; i<MAX_CAM_CHN;i++) {
			printf("%d ", getStrThrCtrl[i].getstr);
			tmp+=getStrThrCtrl[i].getstr;
		}
		printf("\n");
		usleep(33000);
//		printf("getStrCnt=%d\n", getStrCnt);
	}
#endif
	system("rm -rf /tmp/stream");
	sync();
	
	for (VpssGrp = 0; VpssGrp < VpssGrpCnt; VpssGrp++) {
		for (i = 0; i < MAX_STREAM_NUM; i++) {
			VpssChn = vs_hw[0].str[i].vpssphychn;
			if (vs_hw[0].str[i].bindvpss == 1)
				SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
DBG("=== stop Venc%d ===\n", VencChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
			VencChn++;
			usleep(50000);
		}
	}

	videoEncoderStart(VIDEO_MAIN_STREAM, video1);
	videoEncoderStart(VIDEO_SUB_STREAM, video2);
	mjpegEncoderStart(VIDEO_MJPEG_STREAM, mjpeg1);

	video_platform_getRawFrameStart(raw_stream);
	video_platform_getStreamStart(VpssGrpCnt, MAX_STREAM_NUM);
	system("/srv/bin/streaming_server &");
	return s32Ret;
}

int video_platform_setVideoSourceBlueScreen(int channel, int on)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (on == 1) s32Ret = HI_MPI_VI_EnableUserPic(channel*2);
	else s32Ret = HI_MPI_VI_DisableUserPic(channel*2);
	
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VI_EnableUserPic failed with %#x!\n", s32Ret);
	}
	return (int)s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
