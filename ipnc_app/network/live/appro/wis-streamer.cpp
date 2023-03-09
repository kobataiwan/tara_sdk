/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// An application that streams audio/video captured by a WIS GO7007,
// using a built-in RTSP server.
// main program

#include <signal.h>
#include <BasicUsageEnvironment.hh>
#include <getopt.h>
#include <liveMedia.hh>
#include "Err.hh"
#include "WISJPEGVideoServerMediaSubsession.hh"
#include "WISH264VideoServerMediaSubsession.hh"
#include "WISH265VideoServerMediaSubsession.hh"
#include "WISPCMAudioServerMediaSubsession.hh"

#include <sys/time.h>
#include <sys/resource.h>
#include <GroupsockHelper.hh>
#include "WISJPEGStreamSource.hh"

#if 1
#include <Msg_Def.h>
#endif	// GET_AVDATA
#include "iniparser.h"
#include <av_interface.h>
#include "sys_env_type.h"
#include "hi_common.h"
#include <system_default.h>
#include "tara_media_stream.h"


enum  StreamingMode
{
	STREAMING_UNICAST,
	STREAMING_UNICAST_THROUGH_DARWIN,
	STREAMING_MULTICAST_ASM,
	STREAMING_MULTICAST_SSM
};

portNumBits rtspServerPortNum = 554;
char rtspServerPortName[16] = "stream";
char const* MjpegStreamName = "mjpeg";
char const* H265StreamName = "stream";  //H265
char const* H264StreamName = "stream"; //sdstream
char const* streamDescription = "RTSP/RTP stream from IPNC";

int MjpegVideoBitrate = 12000000;
int H265VideoBitrate = 12000000;
int H264VideoBitrate = 12000000;
int audioOutputBitrate = 128000;

unsigned audioSamplingFrequency = 16000;
unsigned audioNumChannels = 1;
int audio_enable = 1;
unsigned audioType = AUDIO_G711U;
unsigned char audioPT = PT_G711U;
char watchVariable = 0;
char videoType = -1;
char videoChn = -1;
char IsAudioAlarm = 0;

int qid = 0;

void sigterm(int dummy)
{
	printf("caught SIGTERM: shutting down\n");
#ifdef GET_AVDATA
	ApproInterfaceExit();
#endif	// GET_AVDATA
	avInterfaceExit();
	exit(1);
}

void sigint(int dummy)
{
	if( IsAudioAlarm == 1 )
	{
		printf("Audio Alarm!!\n");
		IsAudioAlarm = 0;
		return;
	}

	printf("caught SIGINT: shutting down\n");
	watchVariable = 1;
	alarm(1);
}

void sig_audio_enable(int dummy)
{
	printf("Audio enabled!\n");
	audio_enable = 1;
	IsAudioAlarm = 1;
	alarm(2);
}

void sig_audio_disable(int dummy)
{
	printf("Audio disabled!\n");
	audio_enable = 0;
	IsAudioAlarm = 1;
	alarm(2);
}

void init_signals(void)
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sigaddset(&sa.sa_mask, SIGALRM);

	sa.sa_flags = 0;
	sa.sa_handler = sigterm;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sig_audio_enable;
	sigaction(SIGUSR1, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sig_audio_disable;
	sigaction(SIGUSR2, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sigint;
	sigaction( SIGALRM, &sa, NULL );
}

void share_memory_init(int msg_type)
{
#ifdef GET_AVDATA
  if(ApproDrvInit(msg_type))
    exit(1);
  if (func_get_mem(&qid)) {
	ApproDrvExit();
    exit(1);
  }
#endif	// GET_AVDATA
}
#include <fcntl.h>
int GetSampleRate( void )
{
	static int CurrentStatus = -255;
	int fd_proc = -1;
	int ret = -1;
	char StrBuffer[300];
	char TmpBuf[50];
	char *pStr = NULL;
	int samplerate = 0;
	char delima_buf[] = ":\r\n ";

	if( CurrentStatus >= -1 )
	{
		fprintf(stderr,"CurrentStatus is = %d \n", CurrentStatus);
		return CurrentStatus;
	}


	fd_proc = open("/proc/asound/card0/pcm0c/sub0/hw_params", O_RDONLY);
	if( fd_proc < 0 )
	{
		fprintf(stderr,"GetSampleRate open file fail \n");
		ret = -1;
		goto CHECK_CPU_END;

	}

	ret = read(fd_proc,  StrBuffer, sizeof(StrBuffer)-1);
	if( ret <= 0 )
	{
		fprintf(stderr,"read device error !!");
		ret = -1;
		goto CHECK_CPU_END;

	}

	ret = -1;
	StrBuffer[sizeof(StrBuffer)-1] = '\0';
	pStr = strtok(StrBuffer,delima_buf );

	while( pStr != NULL )
	{
		sscanf( pStr,"%s",TmpBuf);

		if( strncmp(TmpBuf, "rate", sizeof("rate")) == 0 )
		{

			pStr = strtok(NULL, delima_buf);

			sscanf( pStr,"%d",&samplerate);

			//fprintf(stderr,"samplerate = %d \n", samplerate);

			ret = samplerate;
			goto CHECK_CPU_END;


		}

		pStr = strtok(NULL, delima_buf);

	}

CHECK_CPU_END:

	if( fd_proc >= 0 )
		close(fd_proc);

	CurrentStatus = ret;

	return ret;
}


static dictionary* g_cfgdictionary = NULL;

int LoadStreamConfigFile(void)
{
	if (NULL != g_cfgdictionary)
	{
		iniparser_freedict(g_cfgdictionary);
		g_cfgdictionary = NULL;
	}

	g_cfgdictionary = iniparser_load(STREAMING_CFG);
	if (NULL == g_cfgdictionary)
	{
		fprintf(stderr, "%s load failed\n", STREAMING_CFG);
		return -1;
	}
	return 0;
}

int LoadVideoConfigFile(void)
{
	if (NULL != g_cfgdictionary)
	{
		iniparser_freedict(g_cfgdictionary);
		g_cfgdictionary = NULL;
	}

	g_cfgdictionary = iniparser_load(VIDEO_CFG);
	if (NULL == g_cfgdictionary)
	{
		fprintf(stderr, "%s load failed\n", VIDEO_CFG);
		return -1;
	}
	return 0;
}

int LoadAudioConfigFile(void)
{
	if (NULL != g_cfgdictionary)
	{
		iniparser_freedict(g_cfgdictionary);
		g_cfgdictionary = NULL;
	}

	g_cfgdictionary = iniparser_load(AUDIO_CFG);
	if (NULL == g_cfgdictionary)
	{
//		fprintf(stderr, "%s ini load failed\n", AUDIO_CFG);
		return -1;
	}
	return 0;
}

typedef struct _StreamInfo {
	int  video1Encoding;
	int  video1RTSP;
	int  video1Port;
	char video1Path[16];
	int  video2Encoding;
	int  video2RTSP;
	int  video2Port;
	char video2Path[16];
	int  mjpegRTSP;
	int  mjpegPort;
	char mjpegPath[16];
} StreamInfo;

int main(int argc, char** argv) {
  init_signals();
  setpriority(PRIO_PROCESS, 0, 0);
  int IsSilence = 0;
  int svcEnable = 0;
  int OverHTTPEnable = 1;
  int cnt=0;
  int ret = 0;
  int waitCnt = 0;
  FrameFormat_t frameFormat;
//  int activePortCnt=0;

#ifdef AUDIO_ENABLE
  if( GetSampleRate() == 16000 )
  {
	audioOutputBitrate = 128000;
	audioSamplingFrequency = 16000;
  }else{
	audioOutputBitrate = 64000;
	audioSamplingFrequency = 8000;
  }
#else
	audioOutputBitrate = 32000;//128000;
	audioSamplingFrequency = 8000; //16000;
#endif
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
  int msg_type, video_type, vencNum;
  APPROInput* MjpegInputDevice = NULL;
  APPROInput* H264InputDevice = NULL;
  APPROInput* H265InputDevice = NULL;
  static pid_t child[8] = {
	-1,-1,-1,-1,-1,-1,-1,-1
  };
StreamInfo strInfo[8] = {
	{VIDEO_CODEC_H264, 1, 551, "vs1_1", VIDEO_CODEC_H264, 0, 551, "vs1_2", 0, 551, "vs1_mjpeg"},
	{VIDEO_CODEC_H264, 1, 552, "vs2_1", VIDEO_CODEC_H264, 0, 552, "vs2_2", 0, 552, "vs2_mjpeg"},
	{VIDEO_CODEC_H264, 1, 553, "vs3_1", VIDEO_CODEC_H264, 0, 553, "vs3_2", 0, 553, "vs3_mjpeg"},
	{VIDEO_CODEC_H264, 1, 554, "vs4_1", VIDEO_CODEC_H264, 0, 554, "vs4_2", 0, 554, "vs4_mjpeg"},
	{VIDEO_CODEC_H264, 1, 555, "vs5_1", VIDEO_CODEC_H264, 0, 555, "vs5_2", 0, 555, "vs5_mjpeg"},
	{VIDEO_CODEC_H264, 1, 556, "vs6_1", VIDEO_CODEC_H264, 0, 556, "vs6_2", 0, 556, "vs6_mjpeg"},
	{VIDEO_CODEC_H264, 1, 557, "vs7_1", VIDEO_CODEC_H264, 0, 557, "vs7_2", 0, 557, "vs7_mjpeg"},
	{VIDEO_CODEC_H264, 1, 558, "vs8_1", VIDEO_CODEC_H264, 0, 558, "vs8_2", 0, 558, "vs8_mjpeg"}
};

  StreamingMode streamingMode = STREAMING_UNICAST;
  netAddressBits multicastAddress = 0;//our_inet_addr("224.1.4.6");
  portNumBits videoRTPPortNum = 0;
  portNumBits audioRTPPortNum = 0;
  portNumBits OverHTTPPortNum = 0;
	char acTempStr[128];
	const char* pcTempStr = NULL;
	int Temp;

  IsSilence = 0;
  svcEnable = 0;
  audioType = AUDIO_G711U;
  streamingMode = STREAMING_UNICAST;

  for( cnt = 1; cnt < argc ;cnt++ )
  {
		if (strcmp(argv[cnt],"-m") == 0)
			streamingMode = STREAMING_MULTICAST_SSM;

		if (strcmp(argv[cnt],"-s") == 0)
			IsSilence = 1;

		if (strcmp( argv[cnt],"-a") == 0)
			audioType = AUDIO_G726;

		if (strcmp(argv[cnt],"-v") == 0)
			svcEnable = 1;

		if (strcmp(argv[cnt],"-h") == 0)	//Use OverHTTP
			OverHTTPEnable = 1;
	}
	
	for (waitCnt = 0; waitCnt < 3; waitCnt++) {	
		ret = LoadVideoConfigFile();
		if (ret == SUCCESS) break;
		fprintf(stderr, "streaming_server: try again!\n");
		usleep(500);
	}
#if 1
	for (int i = 0; i < 8; i++) {
		if (waitCnt >= 2)
			continue;
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1Encoding", i+1);
		pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//		fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "H265") == 0)
				strInfo[i].video1Encoding = VIDEO_CODEC_HEVC;
		}
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2Encoding", i+1);
		pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//		fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "H265") == 0)
				strInfo[i].video2Encoding = VIDEO_CODEC_HEVC;
		}
	}
#else
	for (int i = 0; i < 8; i++) {
		videoCodec[i] = VIDEO_CODEC_H264;
		if (waitCnt >= 2)
			continue;
		snprintf(acTempStr, sizeof(acTempStr), "VideoSource1:Video%dEncoding", i+1);
		pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
		fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
		if (pcTempStr) {
			if(strcmp(pcTempStr, "H265") == 0)
				videoCodec[i] = VIDEO_CODEC_HEVC;
		}
	}
#endif
	iniparser_freedict(g_cfgdictionary);
	g_cfgdictionary = NULL;
	
	ret = LoadStreamConfigFile();
	if (ret == SUCCESS) {
		for (int i = 0; i < 8; i++) {
			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1RTSP", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr == NULL)
				strInfo[i].video1RTSP = 1;
			else if (strcmp("Off", pcTempStr) == 0)
				strInfo[i].video1RTSP = 0;
			else
				strInfo[i].video1RTSP = 1;

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1RTSPPort", i+1);
			Temp = iniparser_getint(g_cfgdictionary, acTempStr, FAIL);
//			fprintf(stderr, "%s:%d\n",acTempStr, Temp);
			if (Temp != FAIL)
				strInfo[i].video1Port = Temp;

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video1RTSPPath", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr != NULL)
				sprintf(strInfo[i].video1Path, "%s", pcTempStr);

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2RTSP", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr == NULL)
				strInfo[i].video2RTSP = 1;
			else if (strcmp("Off", pcTempStr) == 0)
				strInfo[i].video2RTSP = 0;
			else
				strInfo[i].video2RTSP = 1;
			
			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2RTSPPort", i+1);
			Temp = iniparser_getint(g_cfgdictionary, acTempStr, FAIL);
//			fprintf(stderr, "%s:%d\n",acTempStr, Temp);
			if (Temp != FAIL)
				strInfo[i].video2Port = Temp;

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:Video2RTSPPath", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr != NULL)
				sprintf(strInfo[i].video2Path, "%s", pcTempStr);

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1RTSP", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr == NULL)
				strInfo[i].mjpegRTSP = 1;
			else if (strcmp("Off", pcTempStr) == 0)
				strInfo[i].mjpegRTSP = 0;
			else
				strInfo[i].mjpegRTSP = 1;
			
			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1RTSPPort", i+1);
			Temp = iniparser_getint(g_cfgdictionary, acTempStr, FAIL);
//			fprintf(stderr, "%s:%d\n",acTempStr, Temp);
			if (Temp != FAIL)
				strInfo[i].mjpegPort = Temp;

			snprintf(acTempStr, sizeof(acTempStr), "VideoSource%d:MJPEG1RTSPPath", i+1);
			pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//			fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
			if (pcTempStr != NULL)
				sprintf(strInfo[i].mjpegPath, "%s", pcTempStr);				
		}
	}
	iniparser_freedict(g_cfgdictionary);
	g_cfgdictionary = NULL;
	
	ret = LoadAudioConfigFile();
	if (ret != SUCCESS) {
//		fprintf(stderr, "wis-stream: load audio ini fail!\n");
		IsSilence = 1;
	} else {
//fprintf(stderr, "wis-stream: load audio config file success!\n");
		IsSilence = 1;
		snprintf(acTempStr, sizeof(acTempStr), "audio1:Audio1");
		pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//		fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
		if(strcmp(pcTempStr, "On") == 0) 
			IsSilence = 0;
	
		snprintf(acTempStr, sizeof(acTempStr), "audio1:Encoding");
		pcTempStr = iniparser_getstring(g_cfgdictionary, acTempStr, NULL);
//		fprintf(stderr, "%s:%s\n",acTempStr, pcTempStr);
		if(strcmp(pcTempStr, "PCMU") == 0) 
			audioType = AUDIO_G711U;
		else if(strcmp(pcTempStr, "PCMA") == 0) 
			audioType = AUDIO_G711A;
		else if(strcmp(pcTempStr, "G726-32") == 0) 
			audioType = AUDIO_G726;
		else if(strcmp(pcTempStr, "G711A") == 0) 
			audioType = AUDIO_G711A;
		else if(strcmp(pcTempStr, "G711U") == 0) 
			audioType = AUDIO_G711U;
		else if(strcmp(pcTempStr, "DVI4-32") == 0) 
			audioType = AUDIO_ADPCM_32;
		else if(strcmp(pcTempStr, "DVI4-64") == 0) 
			audioType = AUDIO_ADPCM_64;
		else if(strcmp(pcTempStr, "L16") == 0) {
			audioType = AUDIO_LPCM;
			audioSamplingFrequency = 44100;
		}
		else 
			audioType = AUDIO_G711U;
	}
	iniparser_freedict(g_cfgdictionary);
	
#if 0
  printf("###########IsSilence = %d ################\n",IsSilence);
  printf("###########streamingMode = %d ################\n",streamingMode);
  printf("###########audioType = %d ################\n",audioType);
  printf("###########svcEnable = %d ################\n",svcEnable);
#endif
#if 0
	for (int i = 1; i < 8; i++) {
		if (strInfo[0].video1RTSP == 0 && strInfo[0].video2RTSP == 0 && strInfo[0].mjpegRTSP == 0)
			continue;
		if (strInfo[i].video1RTSP == 1 || strInfo[i].video2RTSP == 1 || strInfo[i].mjpegRTSP == 1) {
			if (child[i-1]!= 0)
			 child[i-1] = fork();
			printf("fork child %d\n", i);
		}
	}	
#else

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[1].video1RTSP == 1 || strInfo[1].video2RTSP == 1 || strInfo[1].mjpegRTSP == 1))
		child[0] = fork();
	
	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[2].video1RTSP == 1 || strInfo[2].video2RTSP == 1 || strInfo[2].mjpegRTSP == 1)) {
		if (child[0] != 0)
			child[1] = fork();
  }

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[3].video1RTSP == 1 || strInfo[3].video2RTSP == 1 || strInfo[3].mjpegRTSP == 1)) {
		if (child[0] != 0 && child[1] != 0)
			child[2] = fork();
  }

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[4].video1RTSP == 1 || strInfo[4].video2RTSP == 1 || strInfo[4].mjpegRTSP == 1)) {
		if (child[0] != 0 && child[1] != 0 && child[2] != 0)
			child[3] = fork();
  }

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[5].video1RTSP == 1 || strInfo[5].video2RTSP == 1 || strInfo[5].mjpegRTSP == 1)) {
		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0)
			child[4] = fork();
  }

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[6].video1RTSP == 1 || strInfo[6].video2RTSP == 1 || strInfo[6].mjpegRTSP == 1)) {
		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0)
			child[5] = fork();
  }

	if ((strInfo[0].video1RTSP == 1 || strInfo[0].video2RTSP == 1 || strInfo[0].mjpegRTSP == 1)
		&&(strInfo[7].video1RTSP == 1 || strInfo[7].video2RTSP == 1 || strInfo[7].mjpegRTSP == 1)) {
		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0 && child[5] != 0) 
		child[6] = fork();
  }
#endif

  if (svcEnable) 
	{
		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0)
	  {
			child[4] = fork();
	  }

		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0)
	  {
			child[5] = fork();
	  }

		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0 && child[5] != 0)
	  {
			child[6] = fork();
	  }

		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0 && child[5] != 0 && child[6] != 0)
	  {
			child[7] = fork();
	  }
  }

	if (child[0] == 0)
	{	
		/* parent, success */
		msg_type = LIVE_MSG_TYPE2;		

		if (strInfo[1].video1RTSP == 1) {
			video_type = (strInfo[1].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 3;
			rtspServerPortNum = strInfo[1].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[1].video1Path);
		} else if (strInfo[1].video2RTSP == 1) {
			video_type = (strInfo[1].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 4;
			rtspServerPortNum = strInfo[1].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[1].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 5;
			rtspServerPortNum = strInfo[1].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[1].mjpegPath);
		}
		videoRTPPortNum = 6004;
		audioRTPPortNum = 6006;
		OverHTTPPortNum = 8301;
  }
  if( child[1] == 0 )
  {
		/* parent, success */
		msg_type = LIVE_MSG_TYPE3;
		if (strInfo[2].video1RTSP == 1) {
			video_type = (strInfo[2].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 6;
			rtspServerPortNum = strInfo[2].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[2].video1Path);
		} else if (strInfo[2].video2RTSP == 1) {
			video_type = (strInfo[2].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 7;
			rtspServerPortNum = strInfo[2].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[2].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 8;
			rtspServerPortNum = strInfo[2].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[2].mjpegPath);
		}
		videoRTPPortNum = 6008;
		audioRTPPortNum = 6010;
		OverHTTPPortNum = 8302;
  }
  if( child[2] == 0 )
  {
	/* parent, success */
		msg_type = LIVE_MSG_TYPE4;
		if (strInfo[3].video1RTSP == 1) {
			video_type = (strInfo[3].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 9;
			rtspServerPortNum = strInfo[3].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[3].video1Path);
		} else if (strInfo[3].video2RTSP == 1) {
			video_type = (strInfo[3].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 10;
			rtspServerPortNum = strInfo[3].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[3].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 11;
			rtspServerPortNum = strInfo[3].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[3].mjpegPath);
		}
		videoRTPPortNum = 6012;
		audioRTPPortNum = 6014;
		OverHTTPPortNum = 8303;
  }
  if( child[3] == 0 )
  {
	/* parent, success */
		msg_type = LIVE_MSG_TYPE5;
		if (strInfo[4].video1RTSP == 1) {
			video_type = (strInfo[4].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 12;
			rtspServerPortNum = strInfo[4].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[4].video1Path);
		} else if (strInfo[4].video2RTSP == 1) {
			video_type = (strInfo[4].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 13;
			rtspServerPortNum = strInfo[4].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[4].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 14;
			rtspServerPortNum = strInfo[4].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[4].mjpegPath);
		}
		videoRTPPortNum = 6016;
		audioRTPPortNum = 6018;
		OverHTTPPortNum = 8304;
  }
  if( child[4] == 0 )
  {
	/* parent, success */
		msg_type = LIVE_MSG_TYPE6;
		if (strInfo[5].video1RTSP == 1) {
			video_type = (strInfo[5].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 15;
			rtspServerPortNum = strInfo[5].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[5].video1Path);
		} else if (strInfo[5].video2RTSP == 1) {
			video_type = (strInfo[5].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 16;
			rtspServerPortNum = strInfo[5].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[5].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 17;
			rtspServerPortNum = strInfo[5].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[5].mjpegPath);
		}
		videoRTPPortNum = 6020;
		audioRTPPortNum = 6022;
		OverHTTPPortNum = 8305;
  }
  if( child[5] == 0 )
  {
	/* parent, success */
		msg_type = LIVE_MSG_TYPE7;
		if (strInfo[6].video1RTSP == 1) {
			video_type = (strInfo[6].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 18;
			rtspServerPortNum = strInfo[6].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[6].video1Path);
		} else if (strInfo[6].video2RTSP == 1) {
			video_type = (strInfo[6].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 19;
			rtspServerPortNum = strInfo[6].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[6].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 20;
			rtspServerPortNum = strInfo[6].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[6].mjpegPath);
		}

		videoRTPPortNum = 6024;
		audioRTPPortNum = 6026;
		OverHTTPPortNum = 8306;
  }
  if( child[6] == 0 )
  {
	/* parent, success */
		msg_type = LIVE_MSG_TYPE8;
		if (strInfo[7].video1RTSP == 1) {
			video_type = (strInfo[7].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
			vencNum = 21;
			rtspServerPortNum = strInfo[7].video1Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[7].video1Path);
		} else if (strInfo[7].video2RTSP == 1) {
			video_type = (strInfo[7].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
			vencNum = 22;
			rtspServerPortNum = strInfo[7].video2Port;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[7].video2Path);
		} else {
			video_type = VIDEO_TYPE_MJPEG;
			vencNum = 23;
			rtspServerPortNum = strInfo[7].mjpegPort;
			snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[7].mjpegPath);
		}
		videoRTPPortNum = 6028;
		audioRTPPortNum = 6030;
		OverHTTPPortNum = 8307;
  }
  if(svcEnable) {
	  if (child[4] == 0)
	  {
		/* parent, success */
#ifdef GET_AVDATA
			msg_type = LIVE_MSG_TYPE5;
#endif	// GET_AVDATA
			video_type = VIDEO_TYPE_H264_SVC_30FPS;
			rtspServerPortNum = 8601;
			H264VideoBitrate = 12000000;
			videoRTPPortNum = 6016;
			audioRTPPortNum = 6018;
			OverHTTPPortNum = 8305;
	  }
	  if (child[5] == 0)
	  {
		/* parent, success */
#ifdef GET_AVDATA
			msg_type = LIVE_MSG_TYPE6;
#endif	// GET_AVDATA
			video_type = VIDEO_TYPE_H264_SVC_15FPS;
			rtspServerPortNum = 8602;
			H264VideoBitrate = 12000000;
			videoRTPPortNum = 6020;
			audioRTPPortNum = 6022;
			OverHTTPPortNum = 8306;
	  }
	  if (child[6] == 0)
	  {
		/* parent, success */
#ifdef GET_AVDATA
			msg_type = LIVE_MSG_TYPE7;
#endif	// GET_AVDATA
			video_type = VIDEO_TYPE_H264_SVC_7FPS;
			rtspServerPortNum = 8603;
			H264VideoBitrate = 12000000;
			videoRTPPortNum = 6024;
			audioRTPPortNum = 6026;
			OverHTTPPortNum = 8307;
	  }
	  if (child[7] == 0)
	  {
		/* parent, success */
#ifdef GET_AVDATA
			msg_type = LIVE_MSG_TYPE8;
#endif	// GET_AVDATA
			video_type = VIDEO_TYPE_H264_SVC_3FPS;
			rtspServerPortNum = 8604;
			H264VideoBitrate = 12000000;
			videoRTPPortNum = 6028;
			audioRTPPortNum = 6030;
			OverHTTPPortNum = 8308;
	  }
	  if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0 && child[5] != 0 && child[6] != 0 && child[7] != 0)
	  {
		/* parent, success */
#ifdef GET_AVDATA
			msg_type = LIVE_MSG_TYPE9;
#endif	// GET_AVDATA
			video_type = VIDEO_TYPE_H264;
			rtspServerPortNum = 553;	//8557;
			H264VideoBitrate = 12000000;
			videoRTPPortNum = 6032;
			audioRTPPortNum = 6034;
			OverHTTPPortNum = 8304;
	  }
	}
 	else {
		if (strInfo[0].video1RTSP == 0 && strInfo[0].video2RTSP == 0 && strInfo[0].mjpegRTSP == 0)
			return 0;
			
		if (child[0] != 0 && child[1] != 0 && child[2] != 0 && child[3] != 0 && child[4] != 0 && child[5] != 0 && child[6] != 0) {
		/* parent, success */
			msg_type = LIVE_MSG_TYPE;
			if (strInfo[0].video1RTSP == 1) {
				video_type = (strInfo[0].video1Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265 : VIDEO_TYPE_H264;
				vencNum = 0;
				rtspServerPortNum = strInfo[0].video1Port;
				snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[0].video1Path);
			} else if (strInfo[0].video2RTSP == 1) {
				video_type = (strInfo[0].video2Encoding == VIDEO_CODEC_HEVC) ? VIDEO_TYPE_H265_SD : VIDEO_TYPE_H264_SD;
				vencNum = 1;
				rtspServerPortNum = strInfo[0].video2Port;
				snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[0].video2Path);
			} else {
				video_type = VIDEO_TYPE_MJPEG;
				vencNum = 2;
				rtspServerPortNum = strInfo[0].mjpegPort;
				snprintf(rtspServerPortName, sizeof(rtspServerPortName), "%s", strInfo[0].mjpegPath);
			}
			videoRTPPortNum = 6000;
			audioRTPPortNum = 6002;
			OverHTTPPortNum = 8300;
	  } 
 }

  videoType = video_type;
  videoChn = vencNum;

  // Objects used for multicast streaming:
  static Groupsock* rtpGroupsockAudio = NULL;
  static Groupsock* rtcpGroupsockAudio = NULL;
  static Groupsock* rtpGroupsockVideo = NULL;
  static Groupsock* rtcpGroupsockVideo = NULL;
  static FramedSource* sourceAudio = NULL;
  static RTPSink* sinkAudio = NULL;
  static RTCPInstance* rtcpAudio = NULL;
  static FramedSource* sourceVideo = NULL;
  static RTPSink* sinkVideo = NULL;
  static RTCPInstance* rtcpVideo = NULL;
  FrameInfo_t frameInfo;

#ifdef GET_AVDATA
  share_memory_init(msg_type);
#endif	// GET_AVDATA
	avInterfaceInit(msg_type);

  //init_signals();

	switch(video_type)
	{
	case VIDEO_TYPE_MJPEG:
	case VIDEO_TYPE_MJPEG2:
		frameFormat = FMT_MJPEG; 
		break;
	case VIDEO_TYPE_H265:
	case VIDEO_TYPE_H265_SD:
	case VIDEO_TYPE_H265_CIF:
		frameFormat = FMT_H265;
		break;
	case VIDEO_TYPE_H264:
	case VIDEO_TYPE_H264_SD:
	case VIDEO_TYPE_H264_CIF:
	default:		
		frameFormat = FMT_H264; 
		break;
	}
_TRY_AGAIN:
	ResetAVDataBuffer(videoChn, &frameInfo);
	if (frameInfo.pid != getpid()) {
		err(*env) << "ResetAVDataBuffer again\n";
fprintf(stderr, "%s: %d %d\n", __func__, frameInfo.pid, getpid());
		goto _TRY_AGAIN;
	}

  *env << "Initializing...\n";
  // Initialize the WIS input device:
  if (video_type == VIDEO_TYPE_MJPEG || video_type == VIDEO_TYPE_MJPEG2)
  {
		MjpegInputDevice = APPROInput::createNew(*env, video_type, vencNum);
	  if (MjpegInputDevice == NULL) {
	    err(*env) << "Failed to create MJPEG input device\n";
	    exit(1);
	  }
  }

  if (video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF || video_type == VIDEO_TYPE_H264_SD 
		 || video_type == VIDEO_TYPE_H264_SVC_30FPS ||
		video_type == VIDEO_TYPE_H264_SVC_15FPS || video_type == VIDEO_TYPE_H264_SVC_7FPS || video_type == VIDEO_TYPE_H264_SVC_3FPS)
  {
	  H264InputDevice = APPROInput::createNew(*env, video_type, vencNum);
	  if (H264InputDevice == NULL) {
	    err(*env) << "Failed to create H264 input device\n";
	    exit(1);
	  }
  }

  if (video_type == VIDEO_TYPE_H265 || video_type == VIDEO_TYPE_H265_CIF || video_type == VIDEO_TYPE_H265_SD)
  {
	  H265InputDevice = APPROInput::createNew(*env, video_type, vencNum);
	  if (H265InputDevice == NULL) {
		err(*env) << "Failed to create H265 input device\n";
		exit(1);
	  }
  }

  // Create the RTSP server:
  RTSPServer* rtspServer = NULL;
  // Normal case: Streaming from a built-in RTSP server:
  rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, NULL);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "...done initializing\n";

  if (streamingMode == STREAMING_UNICAST)
  {
	  if (video_type == VIDEO_TYPE_MJPEG || video_type == VIDEO_TYPE_MJPEG2)
	  {
	    ServerMediaSession* sms;
			sms  = ServerMediaSession::createNew(*env, rtspServerPortName, rtspServerPortName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISJPEGVideoServerMediaSubsession
				 ::createNew(sms->envir(), *MjpegInputDevice, MjpegVideoBitrate));
	    if (IsSilence == 0)
	    {
			sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *MjpegInputDevice));
	    }

	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
			if (OverHTTPEnable == 0)
			{
		    *env << "Play this stream using the URL:\n\t" << url << "\n";
			}
			else
			{
				if (rtspServer->setUpTunnelingOverHTTP(OverHTTPPortNum)){
					*env << "Play this stream using the URL:\n\t" << url << "\n";
					*env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";  
				} else {    
			 		*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";  
				}
			}
		
	    delete[] url;
	  }

	  if (video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF || video_type == VIDEO_TYPE_H264_SD ||
				video_type == VIDEO_TYPE_H264_SVC_30FPS ||
			video_type == VIDEO_TYPE_H264_SVC_15FPS || video_type == VIDEO_TYPE_H264_SVC_7FPS || video_type ==VIDEO_TYPE_H264_SVC_3FPS)
	  {
    	ServerMediaSession* sms;
			sms = ServerMediaSession::createNew(*env, rtspServerPortName, rtspServerPortName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISH264VideoServerMediaSubsession
				 ::createNew(sms->envir(), *H264InputDevice, H264VideoBitrate));
	    if (IsSilence == 0)
	    {
	    	sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *H264InputDevice));

	    }
	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
			if(OverHTTPEnable == 0)
			{
		    *env << "Play this stream using the URL:\n\t" << url << "\n";
			}
			else
			{
				if (rtspServer->setUpTunnelingOverHTTP(OverHTTPPortNum)){
					*env << "Play this stream using the URL:\n\t" << url << "\n";
					*env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";  
				} else {    
				 	*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";  
				}
			}

			delete[] url;
		}

		// Create a record describing the media to be streamed:
	  if (video_type == VIDEO_TYPE_H265 || video_type == VIDEO_TYPE_H265_CIF || video_type == VIDEO_TYPE_H265_SD)
	  {
			ServerMediaSession* sms;
			sms = ServerMediaSession::createNew(*env, rtspServerPortName, rtspServerPortName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISH265VideoServerMediaSubsession
				 ::createNew(sms->envir(), *H265InputDevice, H265VideoBitrate));
	    if (IsSilence == 0)
	    {
	    	sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *H265InputDevice));
	    }

	    rtspServer->addServerMediaSession(sms);
fprintf(stderr,"outsize = %d\n", OutPacketBuffer::maxSize);
	    char *url = rtspServer->rtspURL(sms);
			if(OverHTTPEnable == 0)
			{
		    *env << "Play this stream using the URL:\n\t" << url << "\n";
			}
			else
			{
				if (rtspServer->setUpTunnelingOverHTTP(OverHTTPPortNum)){
					*env << "Play this stream using the URL:\n\t" << url << "\n";
					*env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";  
				} else {    
				 	*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";  
				}
			}	
			 
	    delete[] url;
	  }
  } 
	else {

		if (streamingMode == STREAMING_MULTICAST_SSM)
		{
#if 1 //stfu@20140715 Merged RDK_3.8 ONVIF
			*env << "Multicast :\n\t";
#endif
			if (multicastAddress == 0)
				multicastAddress = chooseRandomIPv4SSMAddress(*env);
		} else if (multicastAddress != 0) {
			streamingMode = STREAMING_MULTICAST_ASM;
		}

		struct in_addr dest; dest.s_addr = multicastAddress;
		const unsigned char ttl = 255;

	// For RTCP:
		const unsigned maxCNAMElen = 100;
		unsigned char CNAME[maxCNAMElen + 1];
		gethostname((char *) CNAME, maxCNAMElen);
		CNAME[maxCNAMElen] = '\0';      // just in case

		ServerMediaSession* sms=NULL;

		if(video_type == VIDEO_TYPE_MJPEG || video_type == VIDEO_TYPE_MJPEG2)
		{
			sms = ServerMediaSession::createNew(*env, MjpegStreamName, MjpegStreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);

			sourceAudio = MjpegInputDevice->audioSource();
			sourceVideo = WISJPEGStreamSource::createNew(MjpegInputDevice->videoSource());
		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
			setVideoRTPSinkBufferSize();
			sinkVideo = JPEGVideoRTPSink::createNew(*env, rtpGroupsockVideo);

		}

		if (video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF || video_type == VIDEO_TYPE_H264_SD ||
		video_type == VIDEO_TYPE_H264_SVC_30FPS || video_type == VIDEO_TYPE_H264_SVC_15FPS ||
			video_type == VIDEO_TYPE_H264_SVC_7FPS || video_type == VIDEO_TYPE_H264_SVC_3FPS)
		{
 			sms = ServerMediaSession::createNew(*env, H264StreamName, H264StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
			sourceAudio = H264InputDevice->audioSource();
			sourceVideo = H264VideoStreamDiscreteFramer::createNew(*env, 	H264InputDevice->videoSource());

		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
			setVideoRTPSinkBufferSize();
			{
				char BuffStr[200];
//			extern int GetSprop(void *pBuff, char vType);
//			GetSprop(BuffStr,video_type);
//			sinkVideo = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96, 0x64001F,BuffStr);
				sinkVideo = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96,BuffStr);
//			sinkVideo = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96 );
			}

		}

	// Create a record describing the media to be streamed:
		if (video_type == VIDEO_TYPE_H265 || video_type == VIDEO_TYPE_H265_CIF || video_type == VIDEO_TYPE_H265_SD)
		{
			sms = ServerMediaSession::createNew(*env, H265StreamName, H265StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);

			sourceAudio = H265InputDevice->audioSource();
			sourceVideo = H265VideoStreamDiscreteFramer::createNew(*env, H265InputDevice->videoSource());

		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
			setVideoRTPSinkBufferSize();
			sinkVideo = H265VideoRTPSink::createNew(*env, rtpGroupsockVideo,96);

		}
		/* VIDEO Channel initial */
		if(1)
		{
		// Create (and start) a 'RTCP instance' for this RTP sink:
			unsigned totalSessionBandwidthVideo = (H265VideoBitrate+500)/1000; // in kbps; for RTCP b/w share
			rtcpVideo = RTCPInstance::createNew(*env, rtcpGroupsockVideo,
					totalSessionBandwidthVideo, CNAME,
					sinkVideo, NULL /* we're a server */ ,
					streamingMode == STREAMING_MULTICAST_SSM);
	    // Note: This starts RTCP running automatically
			sms->addSubsession(PassiveServerMediaSubsession::createNew(*sinkVideo, rtcpVideo));

		// Start streaming:
			sinkVideo->startPlaying(*sourceVideo, NULL, NULL);
		}
		/* AUDIO Channel initial */
		if (IsSilence == 0)
		{
		// there's a separate RTP stream for audio
		// Create 'groupsocks' for RTP and RTCP:
			const Port rtpPortAudio(audioRTPPortNum);
			const Port rtcpPortAudio(audioRTPPortNum+1);

			rtpGroupsockAudio = new Groupsock(*env, dest, rtpPortAudio, ttl);
			rtcpGroupsockAudio = new Groupsock(*env, dest, rtcpPortAudio, ttl);

			if (streamingMode == STREAMING_MULTICAST_SSM)
			{
				rtpGroupsockAudio->multicastSendOnly();
				rtcpGroupsockAudio->multicastSendOnly();
			}
			if (audioSamplingFrequency == 16000)
			{

				if(audioType == AUDIO_G711U)
				{
					sinkAudio = SimpleRTPSink::createNew(*env, rtpGroupsockAudio, 96, audioSamplingFrequency, "audio", "PCMU", 1);
				}
				else
				{
					char const* encoderConfigStr = "1408";// (2<<3)|(8>>1) = 0x14 ; ((8<<7)&0xFF)|(1<<3)=0x08 ;
					sinkAudio = MPEG4GenericRTPSink::createNew(*env, rtpGroupsockAudio,
						       96,
						       audioSamplingFrequency,
						       "audio", "AAC-hbr",
						       encoderConfigStr, audioNumChannels);
				}
			}
			else{
				if(audioType == AUDIO_G711U)
				{
					sinkAudio = SimpleRTPSink::createNew(*env, rtpGroupsockAudio, 0, audioSamplingFrequency, "audio", "PCMU", 1);
				}
				else{
					char const* encoderConfigStr =  "1588";// (2<<3)|(11>>1) = 0x15 ; ((11<<7)&0xFF)|(1<<3)=0x88 ;
					sinkAudio = MPEG4GenericRTPSink::createNew(*env, rtpGroupsockAudio,
						       96,
						       audioSamplingFrequency,
						       "audio", "AAC-hbr",
						       encoderConfigStr, audioNumChannels);

				}
			}

		// Create (and start) a 'RTCP instance' for this RTP sink:
			unsigned totalSessionBandwidthAudio = (audioOutputBitrate+500)/1000; // in kbps; for RTCP b/w share
			rtcpAudio = RTCPInstance::createNew(*env, rtcpGroupsockAudio,
					  totalSessionBandwidthAudio, CNAME,
					  sinkAudio, NULL /* we're a server */,
					  streamingMode == STREAMING_MULTICAST_SSM);
		// Note: This starts RTCP running automatically
			sms->addSubsession(PassiveServerMediaSubsession::createNew(*sinkAudio, rtcpAudio));

		// Start streaming:
			sinkAudio->startPlaying(*sourceAudio, NULL, NULL);
    }

		rtspServer->addServerMediaSession(sms);
		{
			struct in_addr dest; dest.s_addr = multicastAddress;
			char *url = rtspServer->rtspURL(sms);
			//char *url2 = inet_ntoa(dest);
			*env << "Mulicast Play this stream using the URL:\n\t" << url << "\n";
			//*env << "2 Mulicast addr:\n\t" << url2 << "\n";
			delete[] url;
		}
  }
  // Begin the LIVE555 event loop:
  env->taskScheduler().doEventLoop(&watchVariable); // does not return


  if( streamingMode!= STREAMING_UNICAST )
  {
	Medium::close(rtcpAudio);
	Medium::close(sinkAudio);
	Medium::close(sourceAudio);
	delete rtpGroupsockAudio;
	delete rtcpGroupsockAudio;

	Medium::close(rtcpVideo);
	Medium::close(sinkVideo);
	Medium::close(sourceVideo);
	delete rtpGroupsockVideo;
	delete rtcpGroupsockVideo;

  }

  Medium::close(rtspServer); // will also reclaim "sms" and its "ServerMediaSubsession"s
  if( MjpegInputDevice != NULL )
  {
	Medium::close(MjpegInputDevice);
  }

  if( H264InputDevice != NULL )
  {
	Medium::close(H264InputDevice);
  }

  if( H265InputDevice != NULL )
  {
	Medium::close(H265InputDevice);
  }

  env->reclaim();
fprintf(stderr, "close all...\n");
  delete scheduler;

#ifdef GET_AVDATA
  ApproInterfaceExit();
#endif	// GET_AVDATA
	avInterfaceExit();

  return 0; // only to prevent compiler warning

}
