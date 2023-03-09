/* ===========================================================================
* @file AVDrvMsg.c
*
* @path $(IPNCPATH)/util/
*
* @desc Message driver for av_interface
* .
* Copyright (c) Pacidal Inc.
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <avDrvMsg.h>
#include "sem_util.h"

static int gProcId = MSG_TYPE_MSG1;
static int qid;
static SemHandl_t hndlAVDrvSem = NULL;

#define PROC_MSG_ID	gProcId

#if defined(dev)
#define __D(fmt, args...) fprintf(stderr, "avDrvMsg " fmt, ## args)
#else
#define __D(fmt, args...)
#endif


/**
 * @brief Initiliztion of the message driver for AV interface
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ./Stream_Msg_Def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int AVDrvInit(int proc_id)
{
	__D("%s: %d\n", __func__, proc_id);
/*
	if(proc_id < MSG_TYPE_MSG1 || proc_id > 20){
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
*/
	gProcId = proc_id;

	if(hndlAVDrvSem == NULL)
		hndlAVDrvSem = MakeSem();

	if(hndlAVDrvSem == NULL){
		gProcId = MSG_TYPE_MSG1;
__D("MakeSem fail\n");
		return -1;
	}

	if((qid=Msg_Init(MSG_KEY)) < 0){
		DestroySem(hndlAVDrvSem);
		hndlAVDrvSem = NULL;
		gProcId = MSG_TYPE_MSG1;
__D("Msg_Init fail\n");
		return -1;
	}
	return 0;
}
/**
 * @brief Resource releasing of the message driver for AV interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int AVDrvExit()
{
	__D("%s: %d\n", __func__, gProcId);
	gProcId = MSG_TYPE_MSG1;
	DestroySem(hndlAVDrvSem);
	hndlAVDrvSem = NULL;
	return 0;
}

/**
 * @brief  Message driver for getting current frame information
 *
 *
* @param   fmt    stucture of frame information, defined at Msg_def.h
 *
 *
 *
 */
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt, int chn)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_CUR_FRAME;
	msgbuf.frame_info.format = fmt;
	msgbuf.frame_info.pid = getpid();
	msgbuf.frame_info.vencChn = chn;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
//	if (fmt != msgbuf.frame_info.format)
//		fprintf(stderr, "different Current FrameType!\n");
	SemRelease(hndlAVDrvSem);
//printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	if(msgbuf.ret != 0)
		msgbuf.frame_info.packCnt = -1;
	return msgbuf.frame_info;
}

FrameInfo_t GetCurrentFrameInfo(int num)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_CUR_FRAME_INFO;
	msgbuf.frame_info.numStr = num;
	msgbuf.frame_info.pid = getpid();
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/

#if 1
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	if (msgbuf.frame_info.pid != getpid()) {
		printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());		
	}
#else
	for(;;) {
		msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
		if (msgbuf.frame_info.pid == getpid()) {
			break;
		}
		printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	}
#endif
	SemRelease(hndlAVDrvSem);
//printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	if(msgbuf.ret != 0)
		msgbuf.frame_info.packCnt = -1;
	return msgbuf.frame_info;
}

FrameInfo_t GetLastMjpegFrame(int num)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_LAST_MJPEG_FRAME;
	msgbuf.frame_info.numStr = num;
	msgbuf.frame_info.pid = getpid();
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/

	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	if (msgbuf.frame_info.pid != getpid()) {
		printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());		
	}

	SemRelease(hndlAVDrvSem);
//printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	if(msgbuf.ret != 0)
		msgbuf.frame_info.packCnt = -1;
	return msgbuf.frame_info;
}

//FrameInfo_t ResetFrameBuffer(FrameFormat_t fmt)
FrameInfo_t ResetFrameBuffer(int chn)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_RESET_FRAMEBUF;
	msgbuf.frame_info.vencChn = chn;
//	msgbuf.frame_info.format = fmt;
	msgbuf.frame_info.pid = getpid();
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	for(;;) {
		msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
		if (msgbuf.frame_info.pid == getpid())
			break;
		printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	}
	SemRelease(hndlAVDrvSem);
//printf("%s: %d %d\n", __func__, msgbuf.frame_info.pid, getpid());
	return msgbuf.frame_info;
//	return msgbuf.ret;
}
#if 0
int SetISPImagingSettings(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IMAGINGSETTINGS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

void SendQuitCmd()
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_QUIT;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	SemRelease(hndlAVDrvSem);

}

int SetBrightness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BRIGHTNESS;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = (int)nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetContrast(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CONTRAST;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetSaturation(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SATURATION;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetHue(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_HUE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetSharpness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHARPNESS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int Set2DNoiseReductionMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_2DNOISEREDUCTION_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int Set2DNoiseReductionLevel(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_2DNOISEREDUCTION_LEVEL;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int Set3DNoiseReductionMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_3DNOISEREDUCTION_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int Set3DNoiseReductionLevel(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_3DNOISEREDUCTION_LEVEL;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetIRCutFilter(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IRCUTFILTER;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetIRCutFilterResponseTime(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IRCUTFILTER_RESPONSETIME;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetIRCutFilterThreshold(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IRCUTFILTER_THRESHOLD;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWhiteBalanceMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWBOnePushTriggerMsg(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WB_ONEPUSH_TRIGGER;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWhiteBalanceCrGain(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE_CRGAIN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWhiteBalanceCbGain(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE_CBGAIN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetBacklightCompensationMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BLC_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetBacklightCompensationLevel(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BLC_LEVEL;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWideDynRangeMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WIDEDYNRANGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetWideDynRangeLevel(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WIDEDYNRANGE_LEVEL;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetFlickerControl(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FLICKER_CONTROL;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposureCompensationMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXP_COMPENSATION_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposureCompensationLevel(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXP_COMPENSATION_LEVEL;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposureMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXP_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposurePriority(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXP_PRIORITY;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMaxExposureTime(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MAX_EXPTIME;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMinExposureTime(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MIN_EXPTIME;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposureTime(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXPTIME;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMaxExposureGain(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MAX_EXPGAIN;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMinExposureGain(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MIN_EXPGAIN;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetExposureGain(unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXPGAIN;
	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAGain(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AGAIN;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetDGain(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DGAIN;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetISPGain(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ISPGAIN;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetFocusMsg(FocusSettings* pPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FOCUS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, pPrm, sizeof(FocusSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMaxIris(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MAX_IRIS;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMinIris(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MIN_IRIS;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetIris(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IRIS;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetFocusTrigger(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FOCUS_TRIGGER;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetFocusPosition(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FOCUS_POSITION;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int GetFocusPosition(void)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_FOCUS_POSITION;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return (int)msgbuf.ret;
}


int SetPictureEffect(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PICTURE_EFFECT;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetDefoggingMode(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DEFOGGING_MODE;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetDefoggingLevel(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DEFOGGING_LEVEL;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetGammaMode(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_GAMMA_MODE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetGammaOffset(float nValue)
{
	MSG_BUF msgbuf;
	float* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_GAMMA_OFFSET;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetFlipMirror(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FLIPMIRROR;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetDIActiveMode(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DI_ACTIV_EMODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

unsigned char GetMotionStatus(void)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MOTION_STATUS;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return (unsigned char)msgbuf.ret;
}

unsigned char GetDIVoltageLevel(void)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_DI_VOLTAGE_LEVEL;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return (unsigned char)msgbuf.ret;
}

unsigned char GetRecoveryVoltageLevel(void)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_RECOVERY_VOLTAGE_LEVEL;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return (unsigned char)msgbuf.ret;
}

int SetDOActiveMode(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DO_ACTIV_EMODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetDOLogicalState(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DO_LOGICAL_STATE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

#if 0
int SetClipSnapName(char* strText, int nLength)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_CLICKNAME;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = nLength;
	memcpy(++ptr, strText, nLength);
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetClipSnapLocation(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CLICKSTORAGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}


#endif
int SetWideDynRangePrmMsg(DynRangePrm* dynRangePrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WIDEDYNRANGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, dynRangePrm, sizeof(DynRangePrm));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetVencConfMsg(CodecParam *codecPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VENC_CONFIG;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecPrm, sizeof(CodecParam));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAndRestartVencMsg(CodecParam *codecPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_RESTART_VENC;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecPrm, sizeof(CodecParam));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAndRestartAudioMsg(int idx, AudioEncParam *codecPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_RESTART_AUDIO;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecPrm, sizeof(AudioEncParam));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAudioSourceMsg(int idx, AudioSourceParam *codecPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_SOURCE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecPrm, sizeof(AudioSourceParam));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAudioOutputGainMsg(int gain)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_OUTPUT_GAIN;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = gain;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAudioOutputFileMsg(AudioOutputParam *outPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_OUTPUT_FILE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, outPrm, sizeof(AudioOutputParam));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMotionDetectMsg(MotionSettings_tmp *outPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOTION_SETTINGS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, outPrm, sizeof(MotionSettings_tmp));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetPTZSettingsMsg(ptzSettings *outPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZ_SETTINGS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, outPrm, sizeof(ptzSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetCodecPTSMsg(void)
{
	MSG_BUF msgbuf;
//	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CODEC_PTS;
//	ptr = (unsigned char*)&msgbuf.mem_info;
//	memcpy(ptr, outPrm, sizeof(MotionSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetAVRestartMsg(void)
{
	MSG_BUF msgbuf;
//	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_RESTART;
//	ptr = (unsigned char*)&msgbuf.mem_info;
//	memcpy(ptr, outPrm, sizeof(MotionSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetOSDNameMsg(OSD_PARAMS *osdparm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSD_NAME;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, osdparm, sizeof(OSD_PARAMS));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetOSDTimeMsg(OSD_PARAMS *osdparm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSD_TIME;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, osdparm, sizeof(OSD_PARAMS));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}
#if 0
int SetVideoOutputMsg(VideoOutputSettings *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VIDEOOUTPUT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(VideoOutputSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMaskMsg(maskSettings *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MASK;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(maskSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int SetMaskRectMsg(maskSettings *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ALL_MASK_RECT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(maskSettings));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}
#endif
int av_drawMaskRectMsg(RectParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DRAW_MASK_RECT;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(RectParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

RectParam_s av_getMaskAttrMsg(int id)
{
	RectParam_s prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MASK_ATTR;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = id;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setMasksVisibilityMsg(int on)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MASKS_VISIBILITY;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = on;	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getMasksVisibilityMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MASKS_VISIBILITY;
	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setMasksColorMsg(int color)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MASKS_COLOR;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = color;	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getMasksColorMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MASKS_COLOR;
	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_createRectMsg(int osdid)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_ADD_RECT_ID;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = osdid;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return (unsigned char)msgbuf.ret;
}

int av_deleteRectMsg(int osdid)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DEL_RECT_ID;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = osdid;
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_drawRectMsg(RectParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DRAW_RECT;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(RectParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

RectParam_s av_getRectAttrMsg(int id)
{
	RectParam_s prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_RECT_ATTR;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = id;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_drawNameMsg(TextParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DRAW_NAME;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(TextParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

TextParam_s av_getNameMsg(void)
{
	TextParam_s prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_NAME_ATTR;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setNameVisibilityMsg(int on)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_NAME_VISIBILITY;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = on;	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_drawTimeMsg(TextParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DRAW_TIME;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(TextParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

TextParam_s av_getTimeMsg(void)
{
	TextParam_s prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_TIME_ATTR;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_drawTextMsg(TextParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_DRAW_TEXT;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(TextParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

TextParam_s av_getTextAttrMsg(int id)
{
	TextParam_s prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_TEXT_ATTR;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = id;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setTextVisibilityMsg(TextParam_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TEXT_VISIBILITY;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(TextParam_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}


wideDynamicRange_attr av_getImagingWDRMsg(void)
{
	wideDynamicRange_attr prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_WDR;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setImagingWDRMsg(wideDynamicRange_attr *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IMG_WDR;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(wideDynamicRange_attr));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

float av_getImagingGainMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_GAIN;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

float av_getImagingAGainMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_AGAIN;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

float av_getImagingDGainMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_DGAIN;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

float av_getImagingISPGainMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_ISPGAIN;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

unsigned long av_getImagingExposureTimeMsg(void)
{
	unsigned long prm;
	MSG_BUF msgbuf;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_IMG_EXPTIME;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setPtzfDigitalZoomModeMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZF_ZOOM_MODE;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = prm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getPtzfDigitalZoomModeMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZF_ZOOM_MODE;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setPtzfZoomMsg(float prm)
{
	MSG_BUF msgbuf;
	float* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZF_ZOOM;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = prm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

float av_getPtzfZoomMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZF_ZOOM;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setPtzfDigitalZoomMsg(float prm)
{
	MSG_BUF msgbuf;
	float* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZF_DIGITAL_ZOOM;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = prm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

float av_getPtzfDigitalZoomMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZF_DIGITAL_ZOOM;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setPtzfOpticalZoomMsg(float prm)
{
	MSG_BUF msgbuf;
	float* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZF_OPTICAL_ZOOM;
	ptr = (float*)&msgbuf.mem_info;
	*ptr = prm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

float av_getPtzfOpticalZoomMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZF_OPTICAL_ZOOM;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

float av_getPtzfOpticalZoomStateMsg(void)
{
	float prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZF_OPTICAL_ZOOM_STATE;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setVideoSourceFrameRateMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VS_FRAMERATE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getVideoSourceFrameRateMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VS_FRAMERATE;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setVideoFreezeFrameMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VIDEO_FREEZE_FRAME;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getVideoFreezeFrameMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VIDEO_FREEZE_FRAME;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

videoOutput_data av_getVideoOutputAttrMsg(void)
{
	videoOutput_data prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VO_ATTR;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setVideoOutputAttrMsg(videoOutput_data *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VO_ATTR;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(videoOutput_data));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}
#if 0
int av_setVideoOnOffMsg(VencPrm *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_VIDEO_ONOFF;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(VencPrm));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getVideoOnOffMsg(int id)
{
	int prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VIDEO_ONOFF;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = id;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}
#endif
int av_loadPresetMsg(int parm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_LOAD_PRESET;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = parm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_savePresetMsg(preset_settings_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SAVE_PRESET;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(preset_settings_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_clearPresetMsg(int parm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_CLEAR_PRESET;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = parm;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setPowerOnPreset(int on)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PWRON_PRESET;
	
	ptr = (int*)&msgbuf.mem_info;
	*ptr = on;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getPowerOnPreset(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PWRON_PRESET;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_setPresetNameMsg(preset_settings_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PRESET_NAME;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(preset_settings_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_getPresetFirstNumMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PRESET_FIRST_NUMBER;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_getPresetLastNumMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PRESET_LAST_NUMBER;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_getPresetPWROnNumMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PRESET_PWRON_NUMBER;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_getPresetNumMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PRESET_NUMBER;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_getPresetCurrentNumMsg(void)
{
	int prm;
	MSG_BUF msgbuf;
		
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PRESET_CURRENT_NUMBER;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

PRESET_S av_presetLTILoadMsg(int num)
{
	PRESET_S prm;
	MSG_BUF msgbuf;
	int *ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SAVE_LTI;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = num;

	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);

	memcpy(&prm, &msgbuf.mem_info, sizeof(prm));
	return prm;
}

int av_presetLTISaveMsg(presetData_s *parm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PRESET_NAME;
	
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, parm, sizeof(presetData_s));
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setUtcEnterMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_UTC_ENTER;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setUtcUpMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_UTC_UP;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setUtcDownMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_UTC_DOWN;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setUtcRightMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_UTC_RIGHT;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}

int av_setUtcLeftMsg(int prm)
{
	MSG_BUF msgbuf;
	int* ptr;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_UTC_LEFT;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = prm;
printf("%s %d\n", __func__, prm);	
	SemWait(hndlAVDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlAVDrvSem);
	return msgbuf.ret;
}
#endif
