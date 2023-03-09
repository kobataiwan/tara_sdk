/******************************************************************************
  A simple program of Hisilicon Hi35xx video encode implementation.
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
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "sem_util.h"
//#include "avserver_comm.h"
#include "wraper_comm.h"
#include "iniparser.h"

#ifdef _OSD_USED
#include "SDL.h"
#include "SDL_ttf.h"
#endif

#include "soc_video.h"
#include "soc_osd.h"
#include "nvp6158_main_thread.h"

#include "mutex.h"
#include "tara_media_stream.h"
#include "tara_video_source_priv.h"
#include "tara_video_encoder_priv.h"
#include "tara_lib.h"
#include "tara_feature.h"
#include "tara_feature_priv.h"
#include "tara_motion_priv.h"
#include "tara_utc.h"

#define VIDEO_CFG	 "/var/cfg/video.cfg"
#define FEATURE_CFG "/var/cfg/feature.cfg"
#define MOTION_CFG "/var/cfg/motion.cfg"
#define	OSD_CFG	"/var/cfg/osd.cfg"
#define	VO_CFG	"/var/cfg/videooutput.cfg"
#define	MASK_CFG	"/var/cfg/mask.cfg"

static TaraFeatureConfig feature;
static VideoEncoder_t video1[MAX_CAM_CHN], video2[MAX_CAM_CHN];
static MjpegEncoder_t mjpeg1[MAX_CAM_CHN];
static MotionConfig_t motion[MAX_CAM_CHN];

int rawframe = RAWFRAME_TYPE_OFF;
VideoSource  *pshm_vs = NULL;
VideoEncoder *pshm_v1 = NULL;
VideoEncoder *pshm_v2 = NULL;
MjpegEncoder *pshm_mj1 = NULL;
MotionConfig *pshm_md = NULL;

int utcThreadrun = 0, encThreadrun=0, mdThreadrun;
pthread_t utcThread = NULL, encThread = NULL, mdThread = NULL;


#ifdef _OSD_USED
mask_cfg_data mask;
osdText_cfg_data osd;
#endif

//VideoOutputSettings gVO;
int gMsgThreadRun = 0;

//videoOutput_data videoOutput;
int gVencId;
int gAvServerRun = 0;

const TaraVideoEncoder venc1_default = {ENCODER_ON, VIDEO_CODEC_H264, VIDEO_PROFILE_HIGH, 1920, 1080, 15, VIDEO_RATE_CONTROL_VBR, 28, 4000, 30};
const TaraVideoEncoder venc2_default = {ENCODER_ON, VIDEO_CODEC_H264, VIDEO_PROFILE_HIGH, 960, 540, 15, VIDEO_RATE_CONTROL_VBR, 22, 2000, 30};
const TaraMjpegEncoder mjenc1_default = {ENCODER_ON, 960, 536, 15, 80};
const TaraMotionConfig motion_default = {MOTION_OFF, 100, MOTION_OSD_OFF, MOTION_OSD_ON_VIDEO2, MOTION_STATE_FALSE};

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void AVSERVER_VENC_HandleSig(int signo)
{
	int i;
	
	if (SIGINT == signo || SIGTERM == signo)
	{
		for (i = 0; i < MAX_VS_NUM; i++)
			motion_soc_vdaStop(i);
		
		DBG("%s exit \n", __func__);
		gAvServerRun = 0;
	}
}

#ifdef _OSD_USED
int msgctrl_osd_drawMaskRect(RectParam_s *rectParam)
{
	int idx, ret;
	RectParam_s prm;
				
	memcpy(&prm, &rectParam[0], sizeof(prm));
	DBG("%s[%d]\n", __func__, prm.id);
				
	idx = prm.id;
	mask.maskColor = mask.rect[idx].color = prm.color;
	mask.rect[idx].stRect.x = prm.stRect.x;
	mask.rect[idx].stRect.y = prm.stRect.y;
	mask.rect[idx].stRect.width = prm.stRect.width;
	mask.rect[idx].stRect.height = prm.stRect.height;
	ret = fSetMaskRect(idx, &mask);
	if (ret < 0) {
		DBG("fSetMaskRect[%d] fail\n", idx);
	}
	mask.update = 1;
	return ret;
}

int msgctrl_osd_getMaskAttr(int idx, RectParam_s *param)
{
	int ret = 0;
//	DBG("%s[%d]\n", __func__, idx);
				
	param->id = mask.rect[idx].id;
	param->color = mask.rect[idx].color;
	param->stRect.x = mask.rect[idx].stRect.x;
	param->stRect.y = mask.rect[idx].stRect.y;
	param->stRect.width = mask.rect[idx].stRect.width;
	param->stRect.height = mask.rect[idx].stRect.height;
	
	return ret;
}

int msgctrl_osd_getMaskVisibility(int *value)
{
	int ret = 0;
				
	*value = mask.maskOn;
	DBG("%s(%d)\n", __func__, *value);
	return ret;
}

int msgctrl_osd_setMaskVisibility(int *value)
{
	int prm;
	int ret = 0;
				
	memcpy(&prm, &value[0], sizeof(prm));
	DBG("%s (%d)\n", __func__, prm);
	mask.maskOn = prm;
	ret = fSetMask(&mask);
	if (ret < 0) {
		DBG("fSetMask fail\n");
	}
	mask.update = 1;
	return ret;
}

int msgctrl_osd_getMaskColor(int *value)
{
	int ret = 0;
				
	*value = mask.maskColor;
	DBG("%s(%d)\n", __func__, *value);
	return ret;
}

int msgctrl_osd_setMaskColor(int *value)
{
	int prm;
	int ret = 0;
				
	memcpy(&prm, &value[0], sizeof(prm));
	DBG("%s (0x%6x)\n", __func__, prm);
	mask.maskColor = prm;
	ret = fSetMask(&mask);
	if (ret < 0) {
		DBG("fSetMask fail\n");
	}
	mask.update = 1;
	return ret;
}

int msgctrl_osd_getNameAttr(TextParam_s *param)
{
	int ret = 0;
				
	DBG("%s\n", __func__);
				
	param->show = osd.Name.show;
	param->x = osd.Name.x;
	param->y = osd.Name.y;
	param->fontSize = osd.Name.fontSize;
	param->fontColor = osd.Name.fontColor;
	snprintf(param->text, sizeof(param->text), "%s", osd.Name.text);
	return ret;
}

int msgctrl_osd_drawName(TextParam_s *textParam)
{
	TextParam_s prm;
	int ret = 0;
	unsigned char update = 0;

	memcpy(&prm, &textParam[0], sizeof(prm));
	DBG("%s: show=%d, type=%d, size=%d, color=%x, (%d, %d)\n", __func__, 
		prm.show, prm.type, prm.fontSize, prm.fontColor, prm.x, prm.y);
					
	if (prm.show != -1 && prm.show != osd.Name.show) {
		osd.Name.show = prm.show;
		update = 1;
	}
	
	if (prm.type != -1 && prm.type != osd.Name.type) {
		osd.Name.type = prm.type;
		update = 1;
	}
	
	if (prm.x != -1 && prm.x != osd.Name.x) {
		osd.Name.x = prm.x;
		update = 1;
	}
	
	if (prm.y != -1 && prm.y != osd.Name.y) {
		osd.Name.y = prm.y;
		update = 1;
	}
	
	if (prm.fontSize != -1 && prm.fontSize != osd.Name.fontSize) {
		osd.Name.fontSize = prm.fontSize;
		update = 1;
	}
	
	if (prm.fontColor != -1 && prm.fontColor != osd.Name.fontColor) {
		osd.Name.fontColor = prm.fontColor;
		update = 1;
	}
	
	if (strcmp(prm.text, osd.Name.text) != 0) {
		snprintf(osd.Name.text, sizeof(osd.Name.text), "%s", prm.text);
		update = 1;
	}
	ret = fSetOSDName(&osd.Name);
	osd.Name.update = update;
	return ret;
}

int msgctrl_osd_setNameVisibility(int *value)
{
	int prm;
	int ret = 0;
				
	memcpy(&prm, &value[0], sizeof(prm));
	DBG("%s (%d)\n", __func__, prm);
	osd.Name.show = prm;
	ret = fSetOSDName(&osd.Name);
	if (ret < 0) {
		DBG("fSetOSDName fail\n");
	}
	osd.Name.update = 1;
	return ret;
}

int msgctrl_osd_getTimeAttr(TextParam_s *param)
{
	int ret = 0;
				
	DBG("%s\n", __func__);
	param->show = osd.Time.show;
	param->type = osd.Time.type;
	param->x = osd.Time.x;
	param->y = osd.Time.y;
	param->fontSize = osd.Time.fontSize;
	param->fontColor = osd.Time.fontColor;
	return ret;
}

int msgctrl_osd_drawTime(TextParam_s *textParam)
{
	TextParam_s prm;
	int ret = 0;
	unsigned char update = 0;
	
	osd_platform_cleanTime();
	
	memcpy(&prm, &textParam[0], sizeof(prm));
	DBG("%s: show=%d, type=%d, size=%d, color=%x, (%d, %d)\n", __func__,
		prm.show, prm.type, prm.fontSize, prm.fontColor, prm.x, prm.y);
				
	if (prm.show != -1 && prm.show != osd.Time.show) {
		osd.Time.show = prm.show;
		update = 1;
	}
	
	if (prm.type != -1 && prm.type != osd.Time.type) {
		osd.Time.type = prm.type;
		update = 1;
	}
	
	if (prm.x != -1 && prm.x != osd.Time.x) {
		osd.Time.x = prm.x;
		update = 1;
	}
	
	if (prm.y != -1 && prm.y != osd.Time.y) {
		osd.Time.y = prm.y;
		update = 1;
	}
	
	if (prm.fontSize != -1 && prm.fontSize != osd.Time.fontSize) {
		osd.Time.fontSize = prm.fontSize;
		update = 1;
	}
	
	if (prm.fontColor != -1 && prm.fontColor != osd.Time.fontColor) {
		osd.Time.fontColor = prm.fontColor;
		update = 1;
	}
	
	ret = fSetOSDTime(&osd.Time);
	osd.Time.update = update;
	return ret;
}

int msgctrl_osd_getTextAttr(int idx, TextParam_s *param)
{
	int ret = 0;
	
	param->show = osd.Text[idx].show;
	param->x = osd.Text[idx].x;
	param->y = osd.Text[idx].y;
	param->fontSize = osd.Text[idx].fontSize;
	param->fontColor = osd.Text[idx].fontColor;
	snprintf(param->text, sizeof(param->text), "%s", osd.Text[idx].text);
	return ret;
}

int msgctrl_osd_drawText(TextParam_s *textParam)
{
	TextParam_s prm;
	int ret = 0, idx;
	unsigned char update = 0;
			
	memcpy(&prm, &textParam[0], sizeof(prm));
	DBG("%s[%d]: show=%d, type=%d, size=%d, color=%x, (%d, %d)\n", __func__,  
		prm.id, prm.show, prm.type, prm.fontSize, prm.fontColor, prm.x, prm.y);
					
	if (prm.fontSize == 0)
		return -1;
				
	idx = prm.id;
	if (prm.show != -1 && prm.show != osd.Text[idx].show) {
		osd.Text[idx].show = prm.show;
		update = 1;
	}
	osd.Text[idx].type = OSD_TEXT_TYPE_TEXT;
	
	if (prm.x != -1 && prm.x != osd.Text[idx].x) {
		osd.Text[idx].x = prm.x;
		update = 1;
	}
	
	if (prm.y != -1 && prm.y != osd.Text[idx].y) {
		osd.Text[idx].y = prm.y;
		update = 1;
	}
	
	if (prm.fontSize != -1 && prm.fontSize != osd.Text[idx].fontSize) {
		osd.Text[idx].fontSize = prm.fontSize;
		update = 1;
	}
	
	if (prm.fontColor != -1 && prm.fontColor != osd.Text[idx].fontColor) {
		osd.Text[idx].fontColor = prm.fontColor;
		update = 1;
	}
	
	if (strcmp(prm.text, osd.Text[idx].text) != 0) {
		snprintf(osd.Text[idx].text, sizeof(osd.Text[idx].text), "%s", prm.text);
		update = 1;
	}

	ret = fSetOSDText(prm.id, &osd.Text[prm.id]);
	osd.Text[0].update = update;
	return ret;
}

int msgctrl_osd_setTextVisibility(TextParam_s *textParam)
{
	TextParam_s prm;
	int ret = 0;
				
	memcpy(&prm, &textParam[0], sizeof(prm));
	DBG("%s[%d]: show=%d\n", __func__, prm.id, prm.show);
					
	osd.Text[prm.id].show = prm.show;
	ret = fSetOSDText(prm.id, &osd.Text[prm.id]);
	if (ret < 0) {
		DBG("fSetOSDText[%d] fail\n", prm.id);
	}
	osd.Text[0].update = 1;
	return ret;
}
#endif

void *Msg_CTRL(void* args)
{
	int msg_size = 0;
	MSG_BUF msgbuf;

	pthread_detach(pthread_self());
	while(gMsgThreadRun)
	{		
		msg_size = msgrcv( gVencId, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG1, 0);

		if( msg_size < 0 ) {
			DBG("Receive msg fail \n");
			break;
		} else {
			switch(msgbuf.cmd)
			{
			case MSG_CMD_RESET_FRAMEBUF:
				_ResetFrameBuffer(msgbuf.Src);
				break;
		
			case MSG_CMD_GET_CUR_FRAME:
				_GetCurrentFrame(msgbuf.Src, &msgbuf.frame_info);
				break;
				
			case MSG_CMD_GET_CUR_FRAME_INFO :
				_GetCurrentFrameInfo(&msgbuf.frame_info);
				break;
				
			case MSG_CMD_GET_LAST_MJPEG_FRAME:
				_GetLastMjpegFrame(&msgbuf.frame_info);
				break;
		#ifdef _OSD_USED
			case MSG_CMD_DRAW_MASK_RECT:
				msgbuf.ret = msgctrl_osd_drawMaskRect((RectParam_s *)&msgbuf.mem_info);
				break;				

			case MSG_CMD_GET_MASK_ATTR:
			{
				int idx;
				memcpy(&idx, &msgbuf.mem_info, sizeof(idx));
				DBG("MSG_CMD_GET_MASK_RECT[%d]\n", idx);
				msgbuf.ret = msgctrl_osd_getMaskAttr(idx, (RectParam_s *)&msgbuf.mem_info);
				break;
			}

			case MSG_CMD_SET_MASKS_VISIBILITY:
				msgbuf.ret = msgctrl_osd_setMaskVisibility((int *)&msgbuf.mem_info);
				break;
			
			case MSG_CMD_GET_MASKS_VISIBILITY:
				msgbuf.ret = msgctrl_osd_getMaskVisibility((int *)&msgbuf.mem_info);
				break;
			
			case MSG_CMD_SET_MASKS_COLOR:
				msgbuf.ret = msgctrl_osd_setMaskColor((int *)&msgbuf.mem_info);
				break;

			case MSG_CMD_GET_MASKS_COLOR:
				msgbuf.ret = msgctrl_osd_getMaskColor((int *)&msgbuf.mem_info);
				break;
			
			case MSG_CMD_DRAW_NAME:
				msgbuf.ret = msgctrl_osd_drawName((TextParam_s *)&msgbuf.mem_info);
				break;

			case MSG_CMD_GET_NAME_ATTR:
				msgbuf.ret = msgctrl_osd_getNameAttr((TextParam_s *)&msgbuf.mem_info);
				break;

			case MSG_CMD_SET_NAME_VISIBILITY:
				msgbuf.ret = msgctrl_osd_setNameVisibility((int *)&msgbuf.mem_info);
				break;

			case MSG_CMD_DRAW_TIME:
				msgbuf.ret = msgctrl_osd_drawTime((TextParam_s *)&msgbuf.mem_info);
				break;

			case MSG_CMD_GET_TIME_ATTR:
				msgbuf.ret = msgctrl_osd_getTimeAttr((TextParam_s *)&msgbuf.mem_info);
				break;
			case MSG_CMD_DRAW_TEXT:
				msgbuf.ret = msgctrl_osd_drawText((TextParam_s *)&msgbuf.mem_info);
				break;

			case MSG_CMD_GET_TEXT_ATTR:
			{
				int idx;
				memcpy(&idx, &msgbuf.mem_info, sizeof(idx));
				DBG("MSG_CMD_GET_TEXT_ATTR: %d\n", idx);
				msgbuf.ret = msgctrl_osd_getTextAttr(idx, (TextParam_s *)&msgbuf.mem_info);
				break;
			}

			case MSG_CMD_SET_TEXT_VISIBILITY:
				msgbuf.ret = msgctrl_osd_setTextVisibility((TextParam_s *)&msgbuf.mem_info);
				break;
		#endif	//_OSD_USED
			default: 
				DBG("MSG_CMD_XXX = %d\n", msgbuf.cmd);
			break;
			}

			if (msgbuf.Src != 0)
			{	/* response */
				msgbuf.Des = msgbuf.Src;
				msgbuf.Src = STREAM_MSG_TYPE;
				msgsnd(gVencId, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);
			}
		}
	}
	return NULL;
}

static void *utcThreadCtrl(void *arg)
{
	int sockfd=-1;
	char *sockname=UTC_CMD_SOCKET;
	struct sockaddr_un addr;
	fd_set rfds;
	struct timeval tv;
	s32 sockbuf[2], *pu32=NULL;
	int ret;
	tv.tv_sec = 0;
	tv.tv_usec = 500000;

	pthread_detach(pthread_self());
	unlink(sockname);	
	do {
		// initialize
		if (sockfd == -1) {
			sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
			if (sockfd != -1) {
				fcntl(sockfd, F_SETFL, O_NONBLOCK);
				addr.sun_family = AF_UNIX;
				strcpy(addr.sun_path, sockname);
				if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != -1) {
					if (listen(sockfd, 1) == -1) {
						close(sockfd);
						sockfd = -1;
					}
				} else {
					close(sockfd);
					sockfd = -1;
				}
			}
		}

		if (sockfd == -1) {
			usleep(100000);
			continue;
		}

		// check incoming connections
//		DBG("%s:%d check incoming connection\n", __FUNCTION__, __LINE__);
		struct sockaddr_un caddr;
		socklen_t clen=sizeof(struct sockaddr_un);
		int clifd=accept(sockfd, (struct sockaddr *)&caddr, &clen);
		if (clifd > 0) {
//			DBG("%s:%d incoming connection fd=%d\n", __FUNCTION__, __LINE__, clifd);
			fcntl(clifd, F_SETFL, O_NONBLOCK);
			FD_ZERO(&rfds);
			FD_SET(clifd, &rfds);
			ret = select(clifd+1, &rfds, NULL, NULL, &tv);
			if (ret > 0) {
				memset(sockbuf, 0, sizeof(sockbuf));
				pu32 = sockbuf;
				ret = read(clifd, pu32, sizeof(sockbuf));
				if (ret < 0)
					DBG("%s:%d read fail returned %d, errno %d\n", __FUNCTION__, __LINE__, ret, errno);
				printf("@@@ utc socket read: %d, %d @@@\n", pu32[0], pu32[1]);
				switch(pu32[1]){
					case UTC_CMD_UP: nvp6158_control_coax_up(pu32[0]); break;
					case UTC_CMD_DOWN: nvp6158_control_coax_down(pu32[0]); break;
					case UTC_CMD_LEFT: nvp6158_control_coax_left(pu32[0]); break;
					case UTC_CMD_RIGHT: nvp6158_control_coax_right(pu32[0]); break;
					case UTC_CMD_ENTER: nvp6158_control_coax_enter(pu32[0]); break;
					default: break;
				}
			}
			close(clifd);
			clifd = -1;
		}

		// check workers

		usleep(100000);
	} while (utcThreadrun);
	printf("%s exit!\n", __func__);
	return 0;
}

int loadVideoConfig(void)
{
	dictionary* cfgdictionary = NULL;
	int s32Temp = 0, i;
	char acTempStr[128];
	char* pcTempStr = NULL;
	
printf("====== %s ======\n", __func__);
	rawframe = RAWFRAME_TYPE_OFF;
	for (i = 0; i < MAX_CAM_CHN; i++) {
		video1[i].needsupdate = 0;
		memcpy(&video1[i].venc, &venc1_default, sizeof(video1[i].venc));
		video2[i].needsupdate = 0;
		memcpy(&video2[i].venc, &venc2_default, sizeof(video2[i].venc));
		mjpeg1[i].needsupdate = 0;
		memcpy(&mjpeg1[i].mjenc, &mjenc1_default, sizeof(mjpeg1[i].mjenc));
	}
	
	cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", VIDEO_CFG);
		return FAILURE;
	}
	
	snprintf(acTempStr, sizeof(acTempStr), "global:RawFrame");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	printf("%s:%s\n",acTempStr, pcTempStr);
	if (pcTempStr) {
		if(strcmp(pcTempStr, "Off") == 0) rawframe = RAWFRAME_TYPE_OFF;
		else if(strcmp(pcTempStr, "Video1") == 0) rawframe = RAWFRAME_TYPE_VIDEO1;
		else if(strcmp(pcTempStr, "Video2") == 0) rawframe = RAWFRAME_TYPE_VIDEO2;
		else if(strcmp(pcTempStr, "MJPEG1") == 0) rawframe = RAWFRAME_TYPE_MJPEG1;
		else rawframe = RAWFRAME_TYPE_OFF;
	}
printf("===rawframe=%d\n", rawframe);
	for (i = 0; i < MAX_CAM_CHN; i++) {		
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "On") == 0) video1[i].venc.video = ENCODER_ON;
			else video1[i].venc.video = ENCODER_OFF;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Encoding", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if (strcmp(pcTempStr, "H264") == 0)
				video1[i].venc.type = VIDEO_CODEC_H264;
			else if (strcmp(pcTempStr, "H265") == 0)
				video1[i].venc.type = VIDEO_CODEC_HEVC;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Profile", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "Baseline") == 0)
				video1[i].venc.profile = VIDEO_PROFILE_BASELINE;
			else if(strcmp(pcTempStr, "Main") == 0)
				video1[i].venc.profile = VIDEO_PROFILE_MAIN;
			else if(strcmp(pcTempStr, "High") == 0)
				video1[i].venc.profile = VIDEO_PROFILE_HIGH;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Width", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video1[i].venc.width = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Height", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video1[i].venc.height = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1FrameRate", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video1[i].venc.framerate = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1RateControlMode", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "VBR") == 0)
				video1[i].venc.ratecontrol = VIDEO_RATE_CONTROL_VBR;
			else if (strcmp(pcTempStr, "CBR") == 0)
				video1[i].venc.ratecontrol = VIDEO_RATE_CONTROL_CBR;
			else if (strcmp(pcTempStr, "FIXQP") == 0)
				video1[i].venc.ratecontrol = VIDEO_RATE_CONTROL_FIXEDQP;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1QP", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE); 	
		if (FAILURE != s32Temp)
			video1[i].venc.qp = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Bitrate", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video1[i].venc.bitrate = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1GOPSize", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video1[i].venc.gopsize = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "On") == 0) video2[i].venc.video = ENCODER_ON;
			else video2[i].venc.video = ENCODER_OFF;
		}
		
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Encoding", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if (strcmp(pcTempStr, "H264") == 0)
				video2[i].venc.type = VIDEO_CODEC_H264;
			else if (strcmp(pcTempStr, "H265") == 0)
				video2[i].venc.type = VIDEO_CODEC_HEVC;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Profile", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "Baseline") == 0)
				video2[i].venc.profile = VIDEO_PROFILE_BASELINE;			
			else if(strcmp(pcTempStr, "Main") == 0)
				video2[i].venc.profile = VIDEO_PROFILE_MAIN;
			else if(strcmp(pcTempStr, "High") == 0)
				video2[i].venc.profile = VIDEO_PROFILE_HIGH;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Width", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video2[i].venc.width = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Height", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video2[i].venc.height = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2FrameRate", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video2[i].venc.framerate = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2RateControlMode", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "VBR") == 0)
				video2[i].venc.ratecontrol = VIDEO_RATE_CONTROL_VBR;
			else if (strcmp(pcTempStr, "CBR") == 0)
				video2[i].venc.ratecontrol = VIDEO_RATE_CONTROL_CBR;
			else if (strcmp(pcTempStr, "FIXQP") == 0)
				video2[i].venc.ratecontrol = VIDEO_RATE_CONTROL_FIXEDQP;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2QP", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE); 	
		if (FAILURE != s32Temp)
			video2[i].venc.qp = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Bitrate", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video2[i].venc.bitrate = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2GOPSize", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			video2[i].venc.gopsize = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "On") == 0) mjpeg1[i].mjenc.mjpeg = ENCODER_ON;
			else mjpeg1[i].mjenc.mjpeg = ENCODER_OFF;
		}

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Width", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			mjpeg1[i].mjenc.width = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Height", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			mjpeg1[i].mjenc.height = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1FrameRate", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			mjpeg1[i].mjenc.framerate = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);			

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Quality", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		if (FAILURE != s32Temp)
			mjpeg1[i].mjenc.quality = s32Temp;
		DBG("%s:%d\n",acTempStr, s32Temp);
		
		printf("channel%d: video1:%s, %s, %dx%d, %d fps, %d bps\n", i, 
			video1[i].venc.video == ENCODER_ON ? "On" : "Off", 
			video1[i].venc.type == VIDEO_CODEC_HEVC ? "H265" : "H264",
			video1[i].venc.width, video1[i].venc.height, video1[i].venc.framerate, video1[i].venc.bitrate);
		printf("channel%d: video2:%s, %s, %dx%d, %d fps, %d bps\n", i, 
			video2[i].venc.video == ENCODER_ON ? "On" : "Off", 
			video2[i].venc.type == VIDEO_CODEC_HEVC ? "H265" : "H264",
			video2[i].venc.width, video2[i].venc.height, video2[i].venc.framerate, video2[i].venc.bitrate);
		printf("channel%d: mjpeg1:%s, %dx%d, %d fps, %d\n", i, 
			mjpeg1[i].mjenc.mjpeg == ENCODER_ON ? "On" : "Off", 
			mjpeg1[i].mjenc.width, mjpeg1[i].mjenc.height, mjpeg1[i].mjenc.framerate, mjpeg1[i].mjenc.quality);
	}
	
	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}

#ifdef _OSD_USED
int load_osdConfigFile(void)
{
	dictionary* cfgdictionary = NULL;
	int s32Temp = 0, i;
	char acTempStr[128];
	char* pcTempStr = NULL;

	cfgdictionary = iniparser_load(OSD_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", OSD_CFG);
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "OSD:Font");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	memset(&osd.Font, '\0', strlen(osd.Font));
	memcpy(&osd.Font, pcTempStr, strlen(pcTempStr));
	DBG("%s:%s\n",acTempStr, osd.Font);
	
	snprintf(acTempStr, sizeof(acTempStr), "Name:CameraName");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	snprintf(osd.Name.text, sizeof(osd.Name.text), "%s", pcTempStr);
	DBG("%s:%s\n",acTempStr, osd.Name.text);

	snprintf(acTempStr, sizeof(acTempStr), "Name:NameOSD");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (pcTempStr == NULL)
		osd.Name.show = 0;
	else if(strcmp(pcTempStr, "On") == 0) 
		osd.Name.show = 1;
	else 
		osd.Name.show = 0;

	osd.Name.type = OSD_Type_Text;
	
	snprintf(acTempStr, sizeof(acTempStr), "Name:NameOSDX");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Name.x = 30;
	else
		osd.Name.x = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Name:NameOSDY");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Name.y = 30;
	else
		osd.Name.y = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Name:NameOSDFontSize");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Name.fontSize = 32;
	else
		osd.Name.fontSize = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Name:NameOSDFontColor");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:#%x\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Name.fontColor = 0xffffff;
	else
		osd.Name.fontColor = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSD");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (pcTempStr == NULL)
		osd.Time.show = 0;
	else if(strcmp(pcTempStr, "On") == 0) 
		osd.Time.show = 1;
	else 
		osd.Time.show = 0;

	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSDType");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (pcTempStr == NULL)
		osd.Time.type = OSD_Type_DateTime;
	else if(strcmp(pcTempStr, "Date") == 0) 
		osd.Time.type = OSD_Type_Date;
	else if(strcmp(pcTempStr, "Time") == 0) 
		osd.Time.type = OSD_Type_Time;
	else
		osd.Time.type = OSD_Type_DateTime;
	
	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSDX");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Time.x = 30;
	else
		osd.Time.x = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSDY");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Time.y = 30;
	else
		osd.Time.y = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSDFontSize");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, HI_FAILURE);
	DBG("%s:%d\n",acTempStr, s32Temp);
	if (s32Temp == HI_FAILURE)
		osd.Time.fontSize = 32;
	else
		osd.Time.fontSize = s32Temp;

	snprintf(acTempStr, sizeof(acTempStr), "Time:TimeOSDFontColor");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
	DBG("%s:0x%x\n",acTempStr, s32Temp);
	if (s32Temp == FAILURE)
		osd.Time.fontColor = 0xffffff;
	else
		osd.Time.fontColor = s32Temp;
	
	for (i = 0; i < OSD_TEXT_MAX; i++) {
		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextStr", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		if (pcTempStr != NULL) {
			snprintf(osd.Text[i].text, sizeof(osd.Text[i].text), "%s", pcTempStr);
			DBG("%s:%s\n",acTempStr, osd.Text[i].text);
		} else {
			memset(osd.Text[i].text, ' ', 64);
			osd.Text[i].text[1] = '\0';
			DBG("%s:%s\n",acTempStr, osd.Text[i].text);
		}

		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextOSD", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr == NULL)
			osd.Text[i].show = 0;
		else if(strcmp(pcTempStr, "On") == 0) 
			osd.Text[i].show = 1;
		else 
			osd.Text[i].show = 0;

		osd.Text[i].type = OSD_Type_Text;
	
		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextOSDX", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAILURE)
			osd.Text[i].x = 30;
		else
			osd.Text[i].x = s32Temp;

		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextOSDY", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAILURE)
			osd.Text[i].y = 30;
		else
			osd.Text[i].y = s32Temp;

		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextOSDFontSize", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAILURE)
			osd.Text[i].fontSize = 32;
		else
			osd.Text[i].fontSize = s32Temp;

		snprintf(acTempStr, sizeof(acTempStr), "Text%d:TextOSDFontColor", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		DBG("%s:#%x\n",acTempStr, s32Temp);
		if (s32Temp == FAILURE)
			osd.Text[i].fontColor = 0xffffff;
		else
			osd.Text[i].fontColor = s32Temp;
		osd.Text[i].update = 0;
	}
	
	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}

int load_maskConfigFile(void)
{
	dictionary* cfgdictionary = NULL;
	int s32Temp = 0, i;
	char acTempStr[128];
	char* pcTempStr = NULL;

	cfgdictionary = iniparser_load(MASK_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", MASK_CFG);
		mask.maskOn = 0;
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "mask:Mask");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (pcTempStr == NULL)
		mask.maskOn = 0;
	else if(strcmp(pcTempStr, "On") == 0)
		mask.maskOn = 1;
	else
		mask.maskOn = 0;

	snprintf(acTempStr, sizeof(acTempStr), "mask:MaskColor");
	s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAIL);
	DBG("%s:#%x\n",acTempStr, s32Temp);
	if (s32Temp == FAIL)
		mask.maskColor = 0xffffff;
	else
		mask.maskColor = s32Temp;
	
	for (i = 0; i < MASK_RECT_MAX; i++) {
		mask.rect[i].id = -1;
		mask.rect[i].show = 0;
		mask.rect[i].type = OSD_RECT_TYPE_FILL;
		mask.rect[i].color = mask.maskColor;
		
		snprintf(acTempStr, sizeof(acTempStr), "mask:Mask%dX", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAIL);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAIL) 
			mask.rect[i].stRect.x = 0;
		else
			mask.rect[i].stRect.x = s32Temp;

		snprintf(acTempStr, sizeof(acTempStr), "mask:Mask%dY", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAIL);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAIL)
			mask.rect[i].stRect.y = 0;
		else
			mask.rect[i].stRect.y = s32Temp;
			
		snprintf(acTempStr, sizeof(acTempStr), "mask:Mask%dWidth", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAIL);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAIL)
			mask.rect[i].stRect.width = 0;
		else
			mask.rect[i].stRect.width = s32Temp;

		snprintf(acTempStr, sizeof(acTempStr), "mask:Mask%dHeight", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAIL);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp == FAIL)
			mask.rect[i].stRect.height = 0;
		else
			mask.rect[i].stRect.height = s32Temp;
	}

	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}
#endif

int loadMotionConfigFile(void)
{
	dictionary* cfgdictionary = NULL;
	char acTempStr[128];
	char* pcTempStr = NULL;
	int i, s32Temp;

	load_motion_vdamdConfigFile();

	for (i = 0; i < MAX_CAM_CHN; i++) {
		motion[i].needsupdate = 0;
		memcpy(&motion[i].config, &motion_default, sizeof(motion[i].config));
	}
	
	cfgdictionary = iniparser_load(MOTION_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", MOTION_CFG);
		return FAILURE;
	}

	for (i = 0; i < MAX_VS_NUM; i++) {
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionDetection", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if (strcmp(pcTempStr, "On") == 0) motion[i].config.motion = MOTION_ON;
			else if (strcmp(pcTempStr, "Off") == 0) motion[i].config.motion = MOTION_OFF;
		}
	
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:Sensitivity", i+1);
		s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
		DBG("%s:%d\n",acTempStr, s32Temp);
		if (s32Temp != FAILURE) motion[i].config.sensitivity = s32Temp;
	
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSD", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if (strcmp(pcTempStr, "Area") == 0) motion[i].config.mdosd = MOTION_OSD_AREA;
			else if (strcmp(pcTempStr, "Off") == 0) motion[i].config.mdosd = MOTION_OSD_OFF;
		}
		
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSDOn", i+1);
		pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "Video1") == 0) motion[i].config.mdosd_on = MOTION_OSDON_VIDEO1;
			else if (strcmp(pcTempStr, "Video2") == 0) motion[i].config.mdosd_on = MOTION_OSDON_VIDEO2;
			else if (strcmp(pcTempStr, "MJPEG1") == 0) motion[i].config.mdosd_on = MOTION_OSDON_MJPEG1;
			else if (strcmp(pcTempStr, "All") == 0) motion[i].config.mdosd_on = MOTION_OSDON_ALL;
		}
	}
	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}

void saveMotionConfig(int chan, TaraMotionConfig *pdata)
{
	dictionary* cfgdictionary = NULL;
	int ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(MOTION_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", MOTION_CFG);
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionDetection", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->motion == MOTION_ON ? "On" : "Off");
	printf("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);
	
	snprintf(acTempStr, sizeof(acTempStr), "VS%d:Sensitivity", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->sensitivity);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSD", chan+1);
	if (pdata->mdosd == MOTION_OSD_AREA) snprintf(pcTempStr, sizeof(pcTempStr), "Area");
	else snprintf(pcTempStr, sizeof(pcTempStr), "Off");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);
	
	snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSDOn", chan+1);
	if (pdata->mdosd_on == MOTION_OSDON_VIDEO1) snprintf(pcTempStr, sizeof(pcTempStr), "Video1");
	else if (pdata->mdosd_on == MOTION_OSDON_VIDEO2) snprintf(pcTempStr, sizeof(pcTempStr), "Video2");
	else if (pdata->mdosd_on == MOTION_OSDON_MJPEG1) snprintf(pcTempStr, sizeof(pcTempStr), "MJPEG1");
	else if (pdata->mdosd_on == MOTION_OSDON_ALL) snprintf(pcTempStr, sizeof(pcTempStr), "All");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);

	if((fp = fopen(MOTION_CFG, "wb")) == NULL) {
		printf("Can't open %s file\n", MOTION_CFG);
	} else {
		iniparser_dump_ini(cfgdictionary, fp);
		iniparser_freedict(cfgdictionary);
		cfgdictionary = NULL;
		fclose(fp);
		fp = NULL;
	}
}

void saveAllMotionConfig(TaraMotionConfig *pdata)
{
	dictionary* cfgdictionary = NULL;
	int chan, ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(MOTION_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", MOTION_CFG);
		return FAILURE;
	}
printf("%s\n", __func__);
	for (chan = 0; chan < num_channel; chan++) {
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionDetection", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->motion == MOTION_ON ? "On" : "Off");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);
	
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:Sensitivity", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->sensitivity);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSD", chan+1);
		if (pdata->mdosd == MOTION_OSD_AREA) snprintf(pcTempStr, sizeof(pcTempStr), "Area");
		else snprintf(pcTempStr, sizeof(pcTempStr), "Off");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);
	
		snprintf(acTempStr, sizeof(acTempStr), "VS%d:MotionOSDOn", chan+1);
		if (pdata->mdosd_on == MOTION_OSD_ON_VIDEO1) snprintf(pcTempStr, sizeof(pcTempStr), "Video1");
		else if (pdata->mdosd_on == MOTION_OSD_ON_VIDEO2) snprintf(pcTempStr, sizeof(pcTempStr), "Video2");
		else if (pdata->mdosd_on == MOTION_OSD_ON_MJPEG1) snprintf(pcTempStr, sizeof(pcTempStr), "MJPEG1");
		else if (pdata->mdosd_on == MOTION_OSD_ON_ALL) snprintf(pcTempStr, sizeof(pcTempStr), "All");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE) printf("save %s=%s failed\n", acTempStr, pcTempStr);
	}
	
	if((fp = fopen(MOTION_CFG, "wb")) == NULL) {
		printf("Can't open %s file\n", MOTION_CFG);
	} else {
		iniparser_dump_ini(cfgdictionary, fp);
		iniparser_freedict(cfgdictionary);
		cfgdictionary = NULL;
		fclose(fp);
		fp = NULL;
	}
}

#if 0
int load_videoOutputConfigFile(void)
{
	dictionary* cfgdictionary = NULL;
	char acTempStr[128];
	char* pcTempStr = NULL;
	int s32Temp = 0;

	videoOutput.on = 0;
	videoOutput.mode = VIDEO_OUT_MODE_BT1120;
	videoOutput.timing = VIDEO_OUT_TIMING_1080P30;
	
	cfgdictionary = iniparser_load(VO_CFG);
	if (NULL == cfgdictionary) {
		DBG("load %s failed\n", VO_CFG);
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "VideoOutput:VideoOutput");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (NULL == pcTempStr)
		videoOutput.on = 0;
	else if(strcmp(pcTempStr, "On") == 0) 
		videoOutput.on = 1;
	else
		videoOutput.on = 0;
	
	snprintf(acTempStr, sizeof(acTempStr), "VideoOutput:VideoOutputMode");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (NULL == pcTempStr) {
		videoOutput.mode = VideoOutput_Mode_BT1120;
	} else if(strcmp(pcTempStr, "BT1120") == 0) {
		videoOutput.mode = VIDEO_OUT_MODE_BT1120;
	} else if(strcmp(pcTempStr, "BT656") == 0) {
		videoOutput.mode = VIDEO_OUT_MODE_BT656;
	} else if(strcmp(pcTempStr, "CVBS") == 0) {
		videoOutput.mode = VIDEO_OUT_MODE_CVBS;
	} else {
		videoOutput.mode = VIDEO_OUT_MODE_BT1120;
	}

	snprintf(acTempStr, sizeof(acTempStr), "VideoOutput:VideoOutputTiming");
	pcTempStr = (char *)iniparser_getstring(cfgdictionary, acTempStr, NULL);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	if (NULL == pcTempStr)
		videoOutput.timing = VIDEO_OUT_TIMING_1080P30;
	else if(strcmp(pcTempStr, "1080p60") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080P60;
	else if(strcmp(pcTempStr, "1080p50") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080P50;
	else if(strcmp(pcTempStr, "1080p30") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080P30;
	else if(strcmp(pcTempStr, "1080p25") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080P25;
	else if(strcmp(pcTempStr, "1080i60") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080I60;
	else if(strcmp(pcTempStr, "1080i50") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_1080I50;
	else if(strcmp(pcTempStr, "720p60") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_720P60;
	else if(strcmp(pcTempStr, "720p50") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_720P50;
	else if(strcmp(pcTempStr, "NTSC") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_NTSC;
	else if(strcmp(pcTempStr, "PAL") == 0)
		videoOutput.timing = VIDEO_OUT_TIMING_PAL;
	else
		videoOutput.timing = VIDEO_OUT_TIMING_1080P30;	

	iniparser_freedict(cfgdictionary);
	return SUCCESS;
}
#endif

void saveVideoConfig(int chan, int stream_type, TaraVideoEncoder *pdata)
{
	dictionary* cfgdictionary = NULL;
	int stype, ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", VIDEO_CFG);
		return FAILURE;
	}
	stype = stream_type == VIDEO_MAIN_STREAM ? 1 : 2;

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%d", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->video == ENCODER_ON ? "On" : "Off");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);
	
	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dEncoding", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->type == VIDEO_CODEC_HEVC ? "H265" : "H264");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);
		
	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dProfile", chan+1, stype);
	if (pdata->profile == VIDEO_PROFILE_BASELINE)
		snprintf(pcTempStr, sizeof(pcTempStr), "Baseline");
	else if (pdata->profile == VIDEO_PROFILE_MAIN)
		snprintf(pcTempStr, sizeof(pcTempStr), "Main");
	else snprintf(pcTempStr, sizeof(pcTempStr), "High");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dWidth", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->width);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dHeight", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->height);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dFrameRate", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->framerate);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dRateControlMode", chan+1, stype);
	if (pdata->ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP)
		snprintf(pcTempStr, sizeof(pcTempStr), "FixedQP");
	else if (pdata->ratecontrol == VIDEO_RATE_CONTROL_CBR)
		snprintf(pcTempStr, sizeof(pcTempStr), "CBR");
	else snprintf(pcTempStr, sizeof(pcTempStr), "VBR");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dQP", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->qp);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dBitrate", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->bitrate);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dGOPSize", chan+1, stype);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->gopsize);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	if((fp = fopen(VIDEO_CFG, "wb")) == NULL) {
		printf("Can't open video_cfg.ini file\n");
	} else {
		iniparser_dump_ini(cfgdictionary, fp);
		iniparser_freedict(cfgdictionary);
		cfgdictionary = NULL;
		fclose(fp);
		fp = NULL;
	}
}

void saveAllVideoConfig(int stream_type, TaraVideoEncoder *pdata)
{
	dictionary* cfgdictionary = NULL;
	int chan, stype, ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", VIDEO_CFG);
		return FAILURE;
	}
	stype = stream_type == VIDEO_MAIN_STREAM ? 1 : 2;

	for (chan = 0; chan < num_channel; chan++) {
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%d", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->video == ENCODER_ON ? "On" : "Off");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);
	
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dEncoding", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->type == VIDEO_CODEC_HEVC ? "H265" : "H264");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);
		
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dProfile", chan+1, stype);
		if (pdata->profile == VIDEO_PROFILE_BASELINE)
			snprintf(pcTempStr, sizeof(pcTempStr), "Baseline");
		else if (pdata->profile == VIDEO_PROFILE_MAIN)
			snprintf(pcTempStr, sizeof(pcTempStr), "Main");
		else snprintf(pcTempStr, sizeof(pcTempStr), "High");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dWidth", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->width);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dHeight", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->height);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dFrameRate", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->framerate);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dRateControlMode", chan+1, stype);
		if (pdata->ratecontrol == VIDEO_RATE_CONTROL_FIXEDQP)
			snprintf(pcTempStr, sizeof(pcTempStr), "FixedQP");
		else if (pdata->ratecontrol == VIDEO_RATE_CONTROL_CBR)
			snprintf(pcTempStr, sizeof(pcTempStr), "CBR");
		else snprintf(pcTempStr, sizeof(pcTempStr), "VBR");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dQP", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->qp);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dBitrate", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->bitrate);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video%dGOPSize", chan+1, stype);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->gopsize);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);
	}

		if((fp = fopen(VIDEO_CFG, "wb")) == NULL) {
			printf("Can't open video_cfg.ini file\n");
		} else {
			iniparser_dump_ini(cfgdictionary, fp);
			iniparser_freedict(cfgdictionary);
			cfgdictionary = NULL;
			fclose(fp);
			fp = NULL;
		}
}

int saveMjpegConfig(int chan, int stream_type, TaraMjpegEncoder *pdata)
{
	dictionary* cfgdictionary = NULL;
	int stype, ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", VIDEO_CFG);
		return FAILURE;
	}

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->mjpeg == ENCODER_ON ? "On" : "Off");
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Width", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->width);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Height", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->height);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1FrameRate", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->framerate);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Quality", chan+1);
	snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->quality);
	DBG("%s:%s\n",acTempStr, pcTempStr);
	ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
	if (ret == FAILURE)
		printf("set %s=%s failed\n", acTempStr, pcTempStr);

	if((fp = fopen(VIDEO_CFG, "wb")) == NULL) {
		printf("Can't open video_cfg.ini file\n");
	} else {
		iniparser_dump_ini(cfgdictionary, fp);
		iniparser_freedict(cfgdictionary);
		cfgdictionary = NULL;
		fclose(fp);
		fp = NULL;
	}
}

int saveAllMjpegConfig(int stream_type, TaraMjpegEncoder *pdata)
{
	dictionary* cfgdictionary = NULL;
	int chan, stype, ret;
	char acTempStr[128];
	char pcTempStr[128];
	FILE *fp;
		
	cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == cfgdictionary) {
		DBG("open %s failed\n", VIDEO_CFG);
		return FAILURE;
	}

	for (chan = 0; chan < num_channel; chan++) {
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%s", pdata->mjpeg == ENCODER_ON ? "On" : "Off");
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Width", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->width);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Height", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->height);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1FrameRate", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->framerate);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);

		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1Quality", chan+1);
		snprintf(pcTempStr, sizeof(pcTempStr), "%d", pdata->quality);
		DBG("%s:%s\n",acTempStr, pcTempStr);
		ret = iniparser_set(cfgdictionary, acTempStr, pcTempStr);
		if (ret == FAILURE)
			printf("set %s=%s failed\n", acTempStr, pcTempStr);
	}
	if((fp = fopen(VIDEO_CFG, "wb")) == NULL) {
		printf("Can't open video_cfg.ini file\n");
	} else {
		iniparser_dump_ini(cfgdictionary, fp);
		iniparser_freedict(cfgdictionary);
		cfgdictionary = NULL;
		fclose(fp);
		fp = NULL;
	}
}

#define NC6158_FORMATDEF_MAX 	73
TaraVideoSource convVideoSourceStatus[NC6158_FORMATDEF_MAX] = {
	{VS_TYPE_NOSIGNAL, 0, 0, 0},
	{VS_TYPE_UNKNOWN, 0, 0, 0},
	{VS_TYPE_CVBS, 960, 540, 30},
	{VS_TYPE_CVBS, 960, 576, 25},
	{VS_TYPE_CVBS, 720, 480, 30},
	{VS_TYPE_CVBS, 720, 576, 25},
	{VS_TYPE_CVBS, 1280, 480, 30},
	{VS_TYPE_CVBS, 1280, 576, 25},
	{VS_TYPE_CVBS, 1440, 480, 30},
	{VS_TYPE_CVBS, 1440, 576, 25},
	{VS_TYPE_CVBS, 1920, 480, 30},
	{VS_TYPE_CVBS, 1920, 576, 25},
	{VS_TYPE_CVBS, 3840, 480, 30},
	{VS_TYPE_CVBS, 3840, 576, 25},
	{VS_TYPE_CVBS, 3840, 480, 30},
	{VS_TYPE_CVBS, 3840, 576, 25},

	{VS_TYPE_AHD, 1920, 1080, 30},
	{VS_TYPE_AHD, 1920, 1080, 25},
	{VS_TYPE_AHD, 1280, 720, 60},
	{VS_TYPE_AHD, 1280, 720, 50},
	{VS_TYPE_AHD, 1280, 720, 30},
	{VS_TYPE_AHD, 1280, 720, 25},
	{VS_TYPE_AHD, 2560, 720, 30},
	{VS_TYPE_AHD, 2560, 720, 25},
	{VS_TYPE_AHD, 2560, 720, 30},
	{VS_TYPE_AHD, 2560, 720, 25},
	{VS_TYPE_AHD, 2560, 1440, 30},
	{VS_TYPE_AHD, 2560, 1440, 25},
	{VS_TYPE_AHD, 2560, 1440, 15},
	{VS_TYPE_AHD, 2048, 1536, 30},
	{VS_TYPE_AHD, 2048, 1536, 25},
	{VS_TYPE_AHD, 2048, 1536, 18},
	{VS_TYPE_AHD, 2592, 1944, 12},
	{VS_TYPE_AHD, 2592, 1944, 20},
	{VS_TYPE_AHD, 3072, 1728, 20},
	{VS_TYPE_AHD, 3280, 1844, 18},
	{VS_TYPE_AHD, 3136, 1764, 20},
	{VS_TYPE_AHD, 1920, 2160, 30},
	{VS_TYPE_AHD, 1920, 2160, 25},
	{VS_TYPE_AHD, 3840, 2160, 8},	//7.5
	{VS_TYPE_AHD, 3840, 2160, 12},	//12.5
	{VS_TYPE_AHD, 3840, 2160, 15},

	{VS_TYPE_TVI, 1920, 1080, 30},
	{VS_TYPE_TVI, 1920, 1080, 25},
	{VS_TYPE_TVI, 1280, 720, 60},
	{VS_TYPE_TVI, 1280, 720, 50},
	{VS_TYPE_TVI, 1280, 720, 30},
	{VS_TYPE_TVI, 1280, 720, 25},
	{VS_TYPE_TVI, 2560, 720, 30},
	{VS_TYPE_TVI, 2560, 720, 25},
	{VS_TYPE_TVI, 1280, 720, 30},
	{VS_TYPE_TVI, 1280, 720, 25},
	{VS_TYPE_TVI, 2560, 720, 30},
	{VS_TYPE_TVI, 2560, 720, 25},
	{VS_TYPE_TVI, 1920, 1536, 18},
	{VS_TYPE_TVI, 2560, 1944, 12},	//12.5
	{VS_TYPE_TVI, 2560, 1944, 20},
	{VS_TYPE_TVI, 2560, 1440, 30},
	{VS_TYPE_TVI, 2560, 1440, 25},
	{VS_TYPE_TVI, 2560, 1440, 15},
	{VS_TYPE_TVI, 3840, 2160, 15},
	{VS_TYPE_TVI, 3840, 2160, 12},	//12.5

	{VS_TYPE_HDCVI, 1920, 1080, 30},
	{VS_TYPE_HDCVI, 1920, 1080, 25},
	{VS_TYPE_HDCVI, 1280, 720, 60},
	{VS_TYPE_HDCVI, 1280, 720, 55},
	{VS_TYPE_HDCVI, 1280, 720, 30},
	{VS_TYPE_HDCVI, 1280, 720, 25},
	{VS_TYPE_HDCVI, 2560, 720, 30},
	{VS_TYPE_HDCVI, 2560, 720, 25},
	{VS_TYPE_HDCVI, 2560, 1440, 30},
	{VS_TYPE_HDCVI, 2560, 1440, 25},
	{VS_TYPE_HDCVI, 3840, 2160, 15},
	{VS_TYPE_HDCVI, 3840, 2160, 12},	//12.5
	{VS_TYPE_UNKNOWN, 0, 0, 0}
};

static void *encPollingThread(void *arg)
{
	int i;
	VideoEncoder_t v1[num_channel], v2[num_channel];
	MjpegEncoder_t mj1[num_channel];
	int chkv1 = 0, chkv2 = 0, chkmj1 = 0;
	int updv1 = 0, updv2 = 0, updmj1 = 0;
	
	pthread_detach(pthread_self());
	while(encThreadrun) {
		chkv1 = chkv2 = chkmj1 = 0;
		updv1 = updv2 = updmj1 = 0;
		for (i = 0; i < num_channel; i++) {
			EnterCritical(pshm_v1->semid, 0);
			memcpy(&v1[i], &pshm_v1->enc[i], sizeof(VideoEncoder_t));
			LeaveCritical(pshm_v1->semid, 0);
		
			EnterCritical(pshm_v2->semid, 0);
			memcpy(&v2[i], &pshm_v2->enc[i], sizeof(VideoEncoder_t));
			LeaveCritical(pshm_v2->semid, 0);

			EnterCritical(pshm_mj1->semid, 0);
			memcpy(&mj1[i], &pshm_mj1->enc[i], sizeof(MjpegEncoder_t));
			LeaveCritical(pshm_mj1->semid, 0);			
		}

		for (i = 0; i < num_channel; i++) {
			if (video1[i].needsupdate != v1[i].needsupdate) {
				chkv1++;
				updv1 = i;
				memcpy(&video1[i], &v1[i], sizeof(video1[i]));
				DBG("update VS%dVideo1: %s %dx%d, %dfps, %dbps\n", i+1, video1[i].venc.video == ENCODER_ON ? "On" : "Off", 
					video1[i].venc.width, video1[i].venc.height, video1[i].venc.framerate, video1[i].venc.bitrate);		
			}
			
			if (video2[i].needsupdate != v2[i].needsupdate) {
				chkv2++;
				updv2 = i;
				memcpy(&video2[i], &v2[i], sizeof(video2[i]));
				DBG("update VS%dVideo2: %s %dx%d, %dfps, %dbps\n", i+1, video2[i].venc.video == ENCODER_ON ? "On" : "Off", 
					video2[i].venc.width, video2[i].venc.height, video2[i].venc.framerate, video2[i].venc.bitrate);		
			}
			
			if (mjpeg1[i].needsupdate != mj1[i].needsupdate) {
				chkmj1++;
				updmj1 = i;
				memcpy(&mjpeg1[i], &mj1[i], sizeof(mjpeg1[i]));
				DBG("update VS%dMJPEG1: %s %dx%d, %dfps, %d\n", i+1, mjpeg1[i].mjenc.mjpeg == ENCODER_ON ? "On" : "Off", 
					mjpeg1[i].mjenc.width, mjpeg1[i].mjenc.height, mjpeg1[i].mjenc.framerate, mjpeg1[i].mjenc.quality);		
			}
		}
		
		if (chkv1 == num_channel) {
			saveAllVideoConfig(VIDEO_MAIN_STREAM, &video1[0].venc);
			video_platform_setAllVideoEncoder(VIDEO_MAIN_STREAM, &video1[0].venc);
		} else if (chkv1 > 0) {
			saveVideoConfig(updv1, VIDEO_MAIN_STREAM, &video1[updv1].venc);
			video_platform_setVideoEncoder(updv1, VIDEO_MAIN_STREAM, &video1[updv1].venc);
		}
	
		if (chkv2 == num_channel) {
			saveAllVideoConfig(VIDEO_SUB_STREAM, &video2[0].venc);
			video_platform_setAllVideoEncoder(VIDEO_SUB_STREAM, &video2[0].venc);
		} else if (chkv2 > 0) {
			saveVideoConfig(updv2, VIDEO_SUB_STREAM, &video2[updv2].venc);
			video_platform_setVideoEncoder(updv2, VIDEO_SUB_STREAM, &video2[updv2].venc);
		}
		
		if (chkmj1 == num_channel) {
			saveAllMjpegConfig(VIDEO_MJPEG_STREAM, &mjpeg1[0].mjenc);
			video_platform_setAllMjpegEncoder(VIDEO_MJPEG_STREAM, &mjpeg1[0].mjenc);
		} else if (chkmj1 > 0) {
			saveMjpegConfig(updmj1, VIDEO_MJPEG_STREAM, &mjpeg1[updmj1].mjenc);
			video_platform_setMjpegEncoder(updmj1, VIDEO_MJPEG_STREAM, &mjpeg1[updmj1].mjenc);
		}
		usleep(200000);
	}
	
	return 0;
}

static void *mdPollingThread(void *arg)
{
	int i, status = MOTION_STATE_FALSE, update_num=0, update_cnt = 0;
	MotionConfig_t mdtmp[num_channel];

	pthread_detach(pthread_self());
	while(encThreadrun) {
		update_num = update_cnt = 0;
		for (i = 0; i < num_channel; i++) {
			EnterCritical(pshm_md->semid, 0);
			memcpy(&mdtmp[i], &pshm_md->md[i], sizeof(MotionConfig_t));
			LeaveCritical(pshm_md->semid, 0);

			if (motion[i].needsupdate != mdtmp[i].needsupdate) {
				motion[i].needsupdate = mdtmp[i].needsupdate;
				update_cnt++;
				update_num = i;
				if (motion[i].config.motion != mdtmp[i].config.motion) {
					motion[i].config.motion = mdtmp[i].config.motion;
					motion[i].config.sensitivity = mdtmp[i].config.sensitivity;
					if (motion[i].config.motion == MOTION_ON)
						motion_soc_vdaStart(i, motion[i].config.sensitivity);
					else motion_soc_vdaStop(i);
				}
				if (motion[i].config.sensitivity != mdtmp[i].config.sensitivity) {
					motion[i].config.sensitivity = mdtmp[i].config.sensitivity;
					motion_setSensitivity(i, motion[i].config.sensitivity);
				}
				printf("!!! update VS%d: MotionDetection %s, %d, %d, %d, update_cnt=%d, update_num=%d\n", i+1, motion[i].config.motion == MOTION_ON ? "On" : "Off",
							motion[i].config.sensitivity, motion[i].config.mdosd, motion[i].config.mdosd_on, update_cnt, update_num);
			}
			status = motion[i].config.motion == MOTION_ON ? motion_getStatus(i) : 0;
			EnterCritical(pshm_md->semid, 0);
			pshm_md->md[i].config.mdstate = status;
			LeaveCritical(pshm_md->semid, 0);
		}
		if (update_cnt == num_channel) saveAllMotionConfig(&motion[0].config);
		else if (update_cnt > 0) saveMotionConfig(update_num, &motion[update_num].config);
		
		usleep(200000);
	}
	
	return 0;
}

/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
int main(int argc, char *argv[])
{
	pthread_t threadControl;
	int i;
	int status[MAX_VS_NUM] = {1, 1, 1, 1, 1, 1, 1, 1};
	int last_status[MAX_VS_NUM] = {1, 1, 1, 1, 1, 1, 1, 1};
	struct timespec ts, ts_last;
	
	signal(SIGINT, AVSERVER_VENC_HandleSig);
	signal(SIGTERM, AVSERVER_VENC_HandleSig);
	
	Tara_init();

	soc_initCodecPTS();
	Tara_feature_getConfig(&feature);
	printf("sys:offset=0x%x\n", feature.sys.ubootenv_offset);
	printf("sys:UnencryptedUpdate=%s\n", 
		feature.sys.UnencryptedUpdate == Feature_UnencryptedUpdate_Off ? "Off" : "On");
	
	pshm_v1 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_1, O_RDWR);
	pshm_v2 = (VideoEncoder*)Tara_video_encoder_attach(Tara_Video_Encoder_2, O_RDWR);
	pshm_mj1 = (MjpegEncoder*)Tara_video_encoder_attach(Tara_Mjpeg_Encoder, O_RDWR);
	pshm_vs = Tara_video_source_attach(O_RDWR);
	pshm_md = Tara_motion_attach(O_RDWR);
		
	if (pshm_vs == NULL || pshm_v1 == NULL || pshm_v2 == NULL || pshm_mj1 == NULL || pshm_md == NULL) {
		printf("Tara API attach fail\n");
		goto _EXIT;
	}

	nvp6158_start();
	sleep(3);
		
	loadVideoConfig();
	loadMotionConfigFile();
//	load_videoOutputConfigFile();
//	gVO.VideoOutput = videoOutput.on;
//	gVO.VideoOutputMode = videoOutput.mode;
//	gVO.VideoOutputTiming = videoOutput.timing;

#ifdef _OSD_USED
	load_osdConfigFile();
	load_maskConfigFile();

	if (TTF_Init() < 0 )  {  
		printf("Couldn't initialize TTF: %s\n", SDL_GetError());  
		SDL_Quit();
	}
#endif
	nvp6158_getStatus(num_channel, status);
	printf(">>> getStatus %d <<<\n", MAX_VS_NUM);
	videosource_t vs_temp;
	for (i = 0; i < num_channel; i++) {
		memset(&vs_temp, 0, sizeof(vs_temp));
		if (status[i] >= NC6158_FORMATDEF_MAX) status[i] = NC6158_FORMATDEF_MAX-1;
		last_status[i] = status[i];
		vs_temp.type = convVideoSourceStatus[status[i]].type;
		vs_temp.width = convVideoSourceStatus[status[i]].width;
		vs_temp.height = convVideoSourceStatus[status[i]].height;
		vs_temp.framerate = convVideoSourceStatus[status[i]].framerate;
		EnterCritical(pshm_vs->semid, 0);
		memcpy(&pshm_vs->vsrc[i], &vs_temp, sizeof(pshm_vs->vsrc[i]));
		printf("%d: %d type=%d, width=%d, height=%d, framerate=%d \n", i, status[i], pshm_vs->vsrc[i].type,
				pshm_vs->vsrc[i].width, pshm_vs->vsrc[i].height, pshm_vs->vsrc[i].framerate);
		LeaveCritical(pshm_vs->semid, 0);
		
		EnterCritical(pshm_v1->semid, 0);
		memcpy(&pshm_v1->enc[i].venc, &video1[i].venc, sizeof(pshm_v1->enc[i].venc));
		LeaveCritical(pshm_v1->semid, 0);
		
		EnterCritical(pshm_v2->semid, 0);
		memcpy(&pshm_v2->enc[i].venc, &video2[i].venc, sizeof(pshm_v2->enc[i].venc));
		LeaveCritical(pshm_v2->semid, 0);
		
		EnterCritical(pshm_mj1->semid, 0);
		memcpy(&pshm_mj1->enc[i].mjenc, &mjpeg1[i].mjenc, sizeof(pshm_mj1->enc[i].mjenc));
		LeaveCritical(pshm_mj1->semid, 0);
		
		EnterCritical(pshm_md->semid, 0);
		memcpy(&pshm_md->md[i].config, &motion[i].config, sizeof(pshm_md->md[i].config));
		LeaveCritical(pshm_md->semid, 0);
	}
	
	video_platform_init();
	
#ifdef _OSD_USED
	osd_start();
#endif
	video_platform_videoEncoderStart(VIDEO_MAIN_STREAM, video1);
	video_platform_videoEncoderStart(VIDEO_SUB_STREAM, video2);
	video_platform_mjpegEncoderStart(VIDEO_MJPEG_STREAM, mjpeg1);
	
	video_platform_getRawFrameStart(rawframe);
	video_platform_getStreamStart(num_channel, MAX_STREAM_NUM);
	
	for (i = 0; i < MAX_VS_NUM; i++) {
		if (motion[i].config.motion == MOTION_ON) motion_soc_vdaStart(i, motion[i].config.sensitivity);
	}

	utcThreadrun = 1;
	pthread_create(&utcThread, 0, utcThreadCtrl, NULL);
	
	encThreadrun = 1;
	pthread_create(&encThread, 0, encPollingThread, NULL);
	
	mdThreadrun = 1;
	pthread_create(&mdThread, 0, mdPollingThread, NULL);
	
	if ((gVencId = Msg_Init(MSG_KEY)) < 0) {
		DBG("msg init fail\n");
	}	
	gMsgThreadRun = 1;
	pthread_create(&threadControl,0,Msg_CTRL,NULL);
	gAvServerRun = 1;
	
//sleep(1);
	for (i = 0; i < MAX_VS_NUM; i++) {
		if (status[i] == 0) video_platform_setVideoSourceBlueScreen(i, 1);
		else video_platform_setVideoSourceBlueScreen(i, 0);
	}

	clock_gettime(CLOCK_REALTIME, &ts);
	ts_last = ts;
	soc_syncCodecPTS();
	while(gAvServerRun)
	{
		clock_gettime(CLOCK_REALTIME, &ts);
		
		if ((ts.tv_sec - ts_last.tv_sec) >= 60 || ts.tv_sec < ts_last.tv_sec) {
			soc_syncCodecPTS();
			ts_last = ts;
		}

		nvp6158_getStatus(MAX_VS_NUM, status);
		for (i = 0; i < MAX_VS_NUM; i++) {
			if (status[i] == last_status[i]) continue;
			if (status[i] == 0) {
				video_platform_setVideoSourceBlueScreen(i, 1);
				printf("Channel %d Video Loss!\n", i+1);
			} else video_platform_setVideoSourceBlueScreen(i, 0);
			last_status[i] = status[i];

			if (status[i] >= NC6158_FORMATDEF_MAX) 
				status[i] = NC6158_FORMATDEF_MAX-1;
			
			EnterCritical(pshm_vs->semid, 0);
			pshm_vs->vsrc[i].type = convVideoSourceStatus[status[i]].type;
			pshm_vs->vsrc[i].width = convVideoSourceStatus[status[i]].width;
			pshm_vs->vsrc[i].height = convVideoSourceStatus[status[i]].height;
			pshm_vs->vsrc[i].framerate = convVideoSourceStatus[status[i]].framerate;
			LeaveCritical(pshm_vs->semid, 0);
		}
		usleep(200000);
	}
	if (utcThreadrun) {
		utcThreadrun = 0;
		pthread_join(utcThread, 0);
	}
	
	if (encThreadrun) {
		encThreadrun = 0;
		pthread_join(encThread, 0);
	}
	
	if (mdThreadrun) {
		mdThreadrun = 0;
		pthread_join(mdThread, 0);
	}
	
	gMsgThreadRun = 0;
	pthread_join(threadControl, 0);
	Msg_Kill(gVencId);

#ifdef _OSD_USED
	osd_stop();
#endif
//	imaging_scene_stop();
	for (i = 0; i < MAX_VS_NUM; i++) {
		if (motion[i].config.motion == MOTION_ON) motion_soc_vdaStop(i);
		EnterCritical(pshm_vs->semid, 0);
		memset(&pshm_vs->vsrc[i], 0, sizeof(pshm_vs->vsrc[i]));
		LeaveCritical(pshm_vs->semid, 0);
	}
	video_platform_exit();
_EXIT:
	if (pshm_v1) Tara_video_encoder_detach(pshm_v1);
	if (pshm_v2) Tara_video_encoder_detach(pshm_v2);
	if (pshm_mj1) Tara_video_encoder_detach(pshm_mj1);
	if (pshm_vs) Tara_video_source_detach(pshm_vs);
	
#ifdef _OSD_USED
	TTF_Quit();
#endif
	nvp6158_stop();
	exit(0);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
