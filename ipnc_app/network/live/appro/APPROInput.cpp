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
// An interface to the WIS GO7007 capture device.
// Implementation

#include "APPROInput.hh"
#include "Err.hh"
#include "Base64.hh"
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include "sem_util.h"
#include "av_interface.h"
//#include <linux/soundcard.h>
#include "H264or5VideoStreamFramer.hh"
#include <error.h>


#define RTSP_MEMCPY	memcpy

#define STREAM_DETAIL_OUTPUT
#define TIME_GET_WAY (0)
#define STREAM_CHECK_DURATION_MS	(5000)
#define GOP_SIZE	30		// for 30 fps, if not change this val
//#define STREAM_PATH		"/tmp/stream"

extern unsigned audioSamplingFrequency;
extern unsigned audioNumChannels;
extern int audio_enable;
extern unsigned audioType;
extern void sigterm(int dummy);

//int WaitVideoStart(FrameFormat_t format, AV_DATA *av_ptr);
int WaitVideoStart(int vType, int vNum, AV_DATA *av_ptr);

////////// OpenFileSource definition //////////

// A common "FramedSource" subclass, used for reading from an open file:

class OpenFileSource: public FramedSource {
public:
  int  uSecsToDelay;
  int  uSecsToDelayMax;
  int  srcType;
protected:
  OpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~OpenFileSource();

  virtual int readFromFile() = 0;

private: // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  static void incomingDataHandler(OpenFileSource* source);
  void incomingDataHandler1();

protected:
  APPROInput& fInput;

//  int fFileNo;
};


////////// VideoOpenFileSource definition //////////

class VideoOpenFileSource: public OpenFileSource {
public:
  VideoOpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~VideoOpenFileSource();

protected: // redefined virtual functions:
  virtual int readFromFile();
  virtual int readFromFile264();
  unsigned int SerialBook;
  unsigned int SerialLock;
  int StreamFlag;
  struct timeval fPresentationTimePre;
  int IsStart;
  int nal_state;
  int num_vol_nal;
  int vol_nal_offsets[MAX_VOL_NAL];
  int vol_nal_lens[MAX_VOL_NAL];
  int Checkduration;
	unsigned long long BackupTimestamp;
};

#define STREAM_GET_VOL    0x0001
#define STREAM_NEW_GOP    0x0002

////////// AudioOpenFileSource definition //////////

class AudioOpenFileSource: public OpenFileSource {
public:
  AudioOpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~AudioOpenFileSource();

protected: // redefined virtual functions:
  virtual int readFromFile();
  int getAudioData();

  unsigned int AudioBook;
  unsigned int AudioLock;
  struct timeval fPresentationTimePre;
  int IsStart;

};

long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  long msec;
  msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
  msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
  return msec;
}

void printErr(UsageEnvironment& env, char const* str = NULL) {
  if (str != NULL) err(env) << str;
  env << ": " << strerror(env.getErrno()) << "\n";
}

////////// APPROInput implementation //////////

APPROInput* APPROInput::createNew(UsageEnvironment& env, int vType, int vNum) {
  return new APPROInput(env, vType, vNum);
}

FramedSource* APPROInput::videoSource() {

  if (fOurVideoSource == NULL) {
    fOurVideoSource = new VideoOpenFileSource(envir(), *this);
  }

  return fOurVideoSource;
}

FramedSource* APPROInput::audioSource() {

  if (fOurAudioSource == NULL) {
    fOurAudioSource = new AudioOpenFileSource(envir(), *this);
  }

  return fOurAudioSource;
}

APPROInput::APPROInput(UsageEnvironment& env, int vType, int vNum)
  : Medium(env), videoType(vType), videoChn(vNum), fOurVideoSource(NULL), fOurAudioSource(NULL) {
}

APPROInput::~APPROInput() {
 if( fOurVideoSource )
 {
 	delete (VideoOpenFileSource *)fOurVideoSource;
 	fOurVideoSource = NULL;
 }

 if( fOurAudioSource )
 {
 	delete (AudioOpenFileSource *)fOurAudioSource;
 	fOurAudioSource = NULL;
 }
//fprintf(stderr,"%s bye!\n", __func__);
}

#include <stdio.h>
#include <stdlib.h>

unsigned long long preTimestamp;
FILE *pAudioFile = NULL;
FILE *pCloseAudioFile = NULL;
int curAudioSerialNo = 0, preAudioSerialNo = 0;
unsigned char AFrameBuff[256];
unsigned char FrameBuff[1024*1024];

#if defined(_MODIFY_)
FILE *pFile = NULL;
FILE *pCloseFile = NULL;
unsigned char FrameBuff[1024*1024];
int dbg_frameCnt;
int preFrameCnt;

void OpenFileHdl(FrameFormat_t format, int serial)
{
	char filename[60];

	if (format == FMT_H265 || format == FMT_H265_CIF || format == FMT_H265_SD)
		sprintf(filename, "%s/%d/%ld.h265", STREAM_PATH, format, serial);

	if (format == FMT_H264 || format == FMT_H264_CIF || format == FMT_H264_SD)
		sprintf(filename, "%s/%d/%ld.h264", STREAM_PATH, format, serial);

	if (format == FMT_MJPEG || format == FMT_MJPEG2)
		sprintf(filename, "%s/%d/%ld.mjp", STREAM_PATH, format, serial);

	pFile = fopen(filename, "rb");
	if( pFile == NULL )
		fprintf(stderr,"open file %s fail!!\n", filename);
	else {
//		fprintf(stderr,"open file %s/%p success!!\n", filename, pFile);
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

int GetAVData(int type, AV_DATA *av_ptr)
{
	static int last_serial = 0;
	int serial_no = 0;
	int offset = 0;
	int framesize = 0, frame_cnt;
	int read_bytes;
	FrameInfo_t frame;
	int i;
	FrameFormat_t format;

	switch(type)
	{
	case VIDEO_TYPE_H265:
		format = FMT_H265; break;	
	case VIDEO_TYPE_H265_SD:
		format = FMT_H265_SD; break;
	case VIDEO_TYPE_H265_CIF:
		format = FMT_H265_CIF; break;
	case VIDEO_TYPE_H264:
		format = FMT_H264; break;
	case VIDEO_TYPE_H264_SD:
		format = FMT_H264_SD; break;
	case VIDEO_TYPE_H264_CIF:
		format = FMT_H264_CIF; break;
	case VIDEO_TYPE_MJPEG:
		format = FMT_MJPEG; break;
	case VIDEO_TYPE_MJPEG2:
		format = FMT_MJPEG2; break;
	default:		
		return -1;
	}
_TRYAGAIN:
	if (GetAVData1(format, &frame) < 0)
	{
		printf("Stream %d is not avaliable~~~~~~~~\n",type);
		sigterm(0);
		return -1;
	}
	
	if (frame.format != format) {
fprintf(stderr, "%s: format = %d, serial = %d\n", __func__, frame.format, frame.fileName);
		goto _TRYAGAIN;
	}

//fprintf(stderr, "type %d %d: pack cnt %d offset %d len %d $$$\n", type,  frame.fileName, frame.packCnt, frame.packInfo[0].offset, frame.packInfo[0].size);

	serial_no = frame.fileName;
	if (!last_serial || serial_no != last_serial) {
		pCloseFile = pFile;
		OpenFileHdl(format, serial_no);
		if (pFile == NULL) {
fprintf(stderr, "type %d %d(%d): pack cnt %d offset %d len %d format=%d $$$\n", type,  frame.fileName, last_serial, frame.packCnt, frame.packInfo[0].offset, frame.packInfo[0].size, frame.format);
			last_serial = 0;
			goto _TRYAGAIN;
			return -1;
		}
		
		last_serial = serial_no;
		dbg_frameCnt = 0;
		preFrameCnt = frame.frameCnt;
	}
		
	if (type == VIDEO_TYPE_MJPEG || type == VIDEO_TYPE_MJPEG2)
	{
		framesize = frame.packInfo[0].size + frame.packInfo[1].size;
		offset = frame.packInfo[0].offset;
//fprintf(stderr, "%p\n", pFile);
		fseek(pFile, offset, SEEK_SET);
		if (read_bytes = fread(FrameBuff, framesize, 1, pFile) <= 0) { 
fprintf(stderr, "MJPEG err read len %ld fail %ld \n", framesize, offset);
fprintf(stderr, "%d read %d errno %d %p\n", serial_no, read_bytes, errno, pFile);
			return -1;	// define ERRNO
		}
#if 0	
		av_ptr->numVolNal = frame.packCnt;
		av_ptr->volOffsets[0] = 0;
		av_ptr->volNalLens[0] = frame.packInfo[0].size;
		av_ptr->volOffsets[1] = frame.packInfo[0].size;
		av_ptr->volNalLens[1] = frame.packInfo[1].size;
#endif
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
				if ( read_bytes=fread(FrameBuff + pack_offset, pack_size - 4, 1, pFile) <= 0) {
fprintf(stderr, "err read for frame type %0x len %ld fail \n",FrameBuff[pack_offset], pack_size);
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
			if (read_bytes = fread(FrameBuff, framesize, 1, pFile) <= 0) { 
fprintf(stderr, "err read for frame type %0x len %ld fail %ld type %d\n",FrameBuff[4], framesize, offset, type);
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
	av_ptr->timestamp = frame.timestamp;
	av_ptr->framecnt = frame.frameCnt;
	frame_cnt = frame.frameCnt;
//fprintf(stderr, "%s: %lld\n", __func__, frame.timestamp);
//if (frame_cnt > 1 && av_ptr->size < 500) fprintf(stderr, "frame serial %d framesize %ld frame_cnt %d:%d type %d \n", av_ptr->serial, av_ptr->size, frame_cnt, dbg_frameCnt, type);
#if 0 //defined(dbg)
if (frame_cnt - preFrameCnt > 1 && type != VIDEO_TYPE_MJPEG && type != VIDEO_TYPE_MJPEG2)
fprintf(stderr, "get frame serial %d framesize %ld frame_cnt %d preFrameCnt %d\n", av_ptr->serial, av_ptr->size, frame_cnt, preFrameCnt);
//if (dbg_frameCnt != frame_cnt && type != VIDEO_TYPE_MJPEG && type != VIDEO_TYPE_MJPEG2)
//fprintf(stderr, "get frame serial %d framesize %ld frame_cnt %d:%d type %d\n", av_ptr->serial, av_ptr->size, frame_cnt, dbg_frameCnt, type);
#endif
	preFrameCnt = frame.frameCnt;
	return 0;	// success
}
#endif


////////// OpenFileSource implementation //////////

OpenFileSource
::OpenFileSource(UsageEnvironment& env, APPROInput& input)
  : FramedSource(env),
    fInput(input) {
}

OpenFileSource::~OpenFileSource() {
fprintf(stderr, "%s\n", __func__);
//	CloseFileHdl();
}

void OpenFileSource::doGetNextFrame() {
	incomingDataHandler(this);
}

void OpenFileSource
::incomingDataHandler(OpenFileSource* source) {
  source->incomingDataHandler1();
}

void OpenFileSource::incomingDataHandler1() {
	int ret;

	if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

	ret = readFromFile();
	if (ret < 0) {
		handleClosure(this);
		fprintf(stderr,"In Grab Image, the source stops being readable!!!!\n");
	}
	else if (ret == 0)
	{

//fprintf(stderr, "%s ret 0 call incomingDataHandler...\n", __func__);
		if( uSecsToDelay >= uSecsToDelayMax )
		{
			uSecsToDelay = uSecsToDelayMax;
		}else{
			uSecsToDelay *= 2;
		}
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)incomingDataHandler, this);
	}
	else {

//fprintf(stderr, "%s ret %d schedule afterGetting...\n", __func__, ret);
		nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)afterGetting, this);
	}
}

////////// VideoOpenFileSource implementation //////////

VideoOpenFileSource
::VideoOpenFileSource(UsageEnvironment& env, APPROInput& input)
  : OpenFileSource(env, input), SerialBook(0), SerialLock(0), StreamFlag(STREAM_GET_VOL),IsStart(1),nal_state(0),Checkduration(0) {
 uSecsToDelay = 1000;
 uSecsToDelayMax = 1666;
 srcType = 0;
}

VideoOpenFileSource::~VideoOpenFileSource() {

  fInput.fOurVideoSource = NULL;
  SerialLock = 0;
fprintf(stderr, "%s bye~~ \n",__func__);

}

int VideoOpenFileSource::readFromFile264()
{
	void *pframe = NULL;
	int framesize = 0;
	int i, ret = 0;
	int offset = 0;
	int serial = 0;
	FrameFormat_t format;
	AV_DATA av_data;

	switch(fInput.videoType)
	{
	case VIDEO_TYPE_H265:
	case VIDEO_TYPE_H265_SD:
	case VIDEO_TYPE_H265_CIF:
		format = FMT_H265; break;	
//		format = FMT_H265_SD; break;
//		format = FMT_H265_CIF; break;
	case VIDEO_TYPE_H264:
	case VIDEO_TYPE_H264_SD:
	case VIDEO_TYPE_H264_CIF:
		format = FMT_H264; break;
//		format = FMT_H264_SD; break;
//		format = FMT_H264_CIF; break;
	case VIDEO_TYPE_MJPEG:
	case VIDEO_TYPE_MJPEG2:
		format = FMT_MJPEG; break;
//		format = FMT_MJPEG2; break;
	default:		
		return -1;
	}
	
	if (fInput.videoType == VIDEO_TYPE_MJPEG || fInput.videoType == VIDEO_TYPE_MJPEG2)
	{
		if (GetAVData(format, fInput.videoChn, &av_data) < 0) {
			printf("Stream %d is not avaliable~~~~~~~~\n",fInput.videoType);
			sigterm(0);
			return -1;
		}
		pframe = av_data.ptr;
		fFrameSize = framesize = av_data.size;

		if (fFrameSize > fMaxSize) {
			printf("Frame Truncated\n");
			fNumTruncatedBytes = fFrameSize - fMaxSize;
			fFrameSize = fMaxSize;
		}
		else {
			fNumTruncatedBytes = 0;
		}
		RTSP_MEMCPY(fTo, av_data.ptr, fFrameSize);
//fprintf(stderr, "offset %d len %d\n", av_data.volOffsets[1], framesize);
		fPresentationTime.tv_sec = av_data.timestamp / 1000000;
		fPresentationTime.tv_usec = av_data.timestamp % 1000000;
		fDurationInMicroseconds = STREAM_CHECK_DURATION_MS >> 1;
		return 1;
	}
 	else
	{
		if (StreamFlag & STREAM_GET_VOL)
		{
		// TODO: only 264/265 need to WaitVideoStart...
			WaitVideoStart(fInput.videoType, fInput.videoChn, &av_data);		// must be I frame
			pframe = av_data.ptr;
			framesize = av_data.size;
			offset = framesize;
			BackupTimestamp = av_data.timestamp;
			
			StreamFlag &= ~(STREAM_GET_VOL);
			num_vol_nal = av_data.numVolNal;

//fprintf(stderr, "nal vol type %02x len %d\n", fTo[0], framesize);
			for (i = 1; i < num_vol_nal ; i ++) {
				vol_nal_offsets[i] = av_data.volOffsets[i];
				vol_nal_lens[i] = av_data.volNalLens[i];
			}

			nal_state ++;
			StreamFlag |= STREAM_NEW_GOP;
			RTSP_MEMCPY((char *)FrameBuff, av_data.ptr, av_data.size);
		}
		else {
			if (StreamFlag & STREAM_NEW_GOP) {
				//RTSP_MEMCPY(fTo, (char *)pframe+vol_nal_offsets[nal_state], vol_nal_lens[nal_state]);
				RTSP_MEMCPY(fTo, (char *)FrameBuff+vol_nal_offsets[nal_state], vol_nal_lens[nal_state]);
				offset = vol_nal_lens[nal_state];
//fprintf(stderr, "nal_state %d nal vol type %02x offset %d len %d\n", nal_state, fTo[0], vol_nal_offsets[nal_state], vol_nal_lens[nal_state]);
				nal_state ++;
				if (nal_state == num_vol_nal) {
					StreamFlag &= ~(STREAM_NEW_GOP);
					nal_state = 0;
				}
			} 
			else {
				GetAVData(format, fInput.videoChn, &av_data);	// msg loop, open close file
				pframe = av_data.ptr;
				framesize = av_data.size;
				BackupTimestamp = av_data.timestamp;

				if (av_data.frameType == AV_FRAME_TYPE_I_FRAME) {
					RTSP_MEMCPY(fTo, (char *)pframe, framesize);
					offset = framesize;
				
					num_vol_nal = av_data.numVolNal;

//fprintf(stderr, "nal vol type %02x len %d\n", fTo[0], framesize);
					for (i = 1; i < num_vol_nal ; i ++) {
						vol_nal_offsets[i] = av_data.volOffsets[i];
						vol_nal_lens[i] = av_data.volNalLens[i];
					}
					nal_state ++;
					StreamFlag |= STREAM_NEW_GOP;
				} else {
					RTSP_MEMCPY(fTo, (char *)pframe+4, framesize-4);	// dont copy start code 00 00 00 01 or length
					offset = framesize - 4;
				}
			}
		}
	} //fInput.videoType == VIDEO_TYPE_H264 || fInput.videoType == VIDEO_TYPE_H264_CIF



	if (1) {
		fFrameSize = offset;
		if (fFrameSize > fMaxSize) {
			printf("Frame Truncated\n");
			printf("fFrameSize = %d\n",fFrameSize);
			printf("fMaxSize = %d\n",fMaxSize);
			fNumTruncatedBytes = fFrameSize - fMaxSize;
			fFrameSize = fMaxSize;
		}
		else {
			fNumTruncatedBytes = 0;
		}

		// re-arrange the fPresentationTime
		fPresentationTime.tv_sec = BackupTimestamp / 1000000;
		fPresentationTime.tv_usec = BackupTimestamp % 1000000;
		
		// Note the timestamp and size:
		fDurationInMicroseconds = 5000;
		return 1;
	}
}

int WaitVideoStart(int vType, int vNum, AV_DATA *av_ptr)
//int WaitVideoStart( FrameFormat_t format, AV_DATA *av_ptr)
{
	FrameFormat_t format;
	FrameInfo_t frameInfo;

	switch(vType) {
	case VIDEO_TYPE_H265:
		format = FMT_H265; break;	
	case VIDEO_TYPE_H265_SD:
		format = FMT_H265_SD; break;
	case VIDEO_TYPE_H265_CIF:
		format = FMT_H265_CIF; break;
	case VIDEO_TYPE_H264:
		format = FMT_H264; break;
	case VIDEO_TYPE_H264_SD:
		format = FMT_H264_SD; break;
	case VIDEO_TYPE_H264_CIF:
		format = FMT_H264_CIF; break;
	case VIDEO_TYPE_MJPEG:
		format = FMT_MJPEG; break;
	case VIDEO_TYPE_MJPEG2:
		format = FMT_MJPEG2; break;
	default:		
		return -1;
	}
//	ResetAVDataBuffer(format, &frameInfo);
	ResetAVDataBuffer(vNum, &frameInfo);
	
	while(1) {
		if (GetAVData(format, vNum, av_ptr) < 0) {
			printf("Stream %d is not avaliable~~~~~~~~\n",format);
			sigterm(0);
			return -1;
		}

		if (av_ptr->frameType == AV_FRAME_TYPE_I_FRAME)
			break;
		else {
			continue;
		}
	}

	return 0;
}

#ifdef STREAM_DETAIL_OUTPUT
#define	RUN_FRAME_NUM	(1000)
void PrintStreamDetails(int type)
{
	static int strmfrmCnt = 0;
	static int strmfrmSkip = 0;
	static struct timeval startTime, endTime, passTime;
	double calcTime;

	if (strmfrmSkip < 150) {
		strmfrmSkip++;
	} else {
		if (strmfrmCnt == 0)
			gettimeofday(&startTime, NULL);
		if (strmfrmCnt == RUN_FRAME_NUM) {
			gettimeofday(&endTime, NULL);
			printf("\n==================== STREAMING DETAILS ====================\n");
			printf("Start Time : %ldsec %06ldusec\n", (long)startTime.tv_sec, (long)startTime.tv_usec);
			printf("End Time   : %ldsec %06ldusec\n", (long)endTime.tv_sec, (long)endTime.tv_usec);
			timersub(&endTime, &startTime, &passTime);
			calcTime = (double)passTime.tv_sec*1000.0 + (double)passTime.tv_usec/1000.0;
			printf("Total Time to stream %d frames: %.3f msec\n", RUN_FRAME_NUM , calcTime);
			printf("Time per frame: %3.4f msec\n", calcTime/RUN_FRAME_NUM);
			printf("Streaming Performance in FPS: %3.4f\n", RUN_FRAME_NUM/(calcTime/1000));
			printf("===========================================================\n");
			strmfrmCnt 	= 0;
			strmfrmSkip = 0;
		} else {
			strmfrmCnt++;
		}
	}
}
#endif

int VideoOpenFileSource::readFromFile()
{
	return readFromFile264();
}


////////// AudioOpenFileSource implementation //////////

AudioOpenFileSource
::AudioOpenFileSource(UsageEnvironment& env, APPROInput& input)
  : OpenFileSource(env, input), AudioBook(0), AudioLock(0), IsStart(1) {
  uSecsToDelay = 5000;
  uSecsToDelayMax = 125000;
  srcType = 1;
}

AudioOpenFileSource::~AudioOpenFileSource() {
  fInput.fOurAudioSource = NULL;
  if (AudioLock > 0) {
    AudioLock = 0;
  }

}

int AudioOpenFileSource::getAudioData() {
	AV_DATA av_data;
	int ret;
	FrameInfo_t frame;
	FrameFormat_t format = FMT_AUDIO;
	int read_bytes;
	int offset = 0, framesize = 0;
	char filename[60];
	char ptToStr[5];
__TRYGAGIN:	
	if (GetAVData1(FMT_AUDIO, 0, &frame) < 0)
	{
		printf("Audio is not avaliable~~~~~~~~\n");
		sigterm(0);
		return -1;
	}
	if (frame.format != FMT_AUDIO) {
		fprintf(stderr, "%s: format = %d, serial = %d\n", __func__, frame.format, frame.fileName);
		goto __TRYGAGIN;
	}
	switch (audioType) {
	case AUDIO_G711U:
		sprintf(ptToStr, "g711u"); break;
	case AUDIO_G711A:
		sprintf(ptToStr, "g711a"); break;
	case AUDIO_G726:
		sprintf(ptToStr, "g726"); break;
	case AUDIO_ADPCM_32:
	case AUDIO_ADPCM_64:
		sprintf(ptToStr, "adpcm"); break;
	case AUDIO_LPCM:
		sprintf(ptToStr, "pcm"); break;
	default:
		sprintf(ptToStr, "g711u"); break;
	}
	
	curAudioSerialNo = frame.fileName;
	if (!preAudioSerialNo || curAudioSerialNo != preAudioSerialNo) {
		pCloseAudioFile = pAudioFile;
		pAudioFile = NULL;
		sprintf(filename, "/tmp/audio/a%ld.%s", curAudioSerialNo, ptToStr);
		pAudioFile = fopen(filename, "rb");
		if (pAudioFile == NULL ) {
			fprintf(stderr,"open file %s fail!!\n", filename);
			return -1;
		}
		preAudioSerialNo = curAudioSerialNo;
	}
	
	framesize = frame.packInfo[0].size;
	offset = frame.packInfo[0].offset;
	fseek(pAudioFile, offset, SEEK_SET);
	if (read_bytes = fread(AFrameBuff, framesize, 1, pAudioFile) <= 0) { 
fprintf(stderr, "Audio err read len %ld fail %ld \n", frame.packInfo[0].size, frame.packInfo[0].offset);
fprintf(stderr, "%d read %d errno %d %p\n", curAudioSerialNo, read_bytes, errno, pAudioFile);
		return -1;	// define ERRNO
	}

	if (pCloseAudioFile != NULL) {
		fclose(pCloseAudioFile);
		pCloseAudioFile = NULL;
	}
	av_data.size = framesize;
	av_data.ptr = (unsigned char *)AFrameBuff;
	av_data.timestamp = frame.timestamp;
//fprintf(stderr, "%s: %lld %lld\n", __func__, av_data.timestamp, av_data.timestamp - preTimestamp);
	preTimestamp = av_data.timestamp;
	RTSP_MEMCPY(fTo, av_data.ptr, av_data.size);
	fPresentationTime.tv_sec = av_data.timestamp/1000000;
	fPresentationTime.tv_usec = av_data.timestamp%1000000;
		
 	return av_data.size;
}


int AudioOpenFileSource::readFromFile() {
  int timeinc;

//  if (!audio_enable)  return 0;

  // Read available audio data:
  int ret = getAudioData();

  if (ret <= 0) return 0;
  if (ret < 0) ret = 0;
  fFrameSize = (unsigned)ret;
  fNumTruncatedBytes = 0;

#if (TIME_GET_WAY)
/* PR#2665 fix from Robin
   * Assuming audio format = AFMT_S16_LE
   * Get the current time
   * Substract the time increment of the audio oss buffer, which is equal to
   * buffer_size / channel_number / sample_rate / sample_size ==> 400+ millisec
   */
 timeinc = fFrameSize * 1000 / audioNumChannels / (audioSamplingFrequency/1000) ;/// 2;
 while (fPresentationTime.tv_usec < timeinc)
 {
   fPresentationTime.tv_sec -= 1;
   timeinc -= 1000000;

 }

 fPresentationTime.tv_usec -= timeinc;

#else
  timeinc = fFrameSize*1000 / audioNumChannels / (audioSamplingFrequency/1000);
  if( IsStart )
  {
  	IsStart = 0;
  	fPresentationTimePre = fPresentationTime;
  	fDurationInMicroseconds = timeinc;
  }else{
	fDurationInMicroseconds = timevaldiff(&fPresentationTimePre, &fPresentationTime )*1000;
//	fDurationInMicroseconds = timevaldiff(&fPresentationTimePre, &fPresentationTime);
	fPresentationTimePre = fPresentationTime;
  }
#endif
  return 1;
}

int GetSprop(void *pBuff, int *vsize, int *ssize, int *psize, char vType, char vNum)
{
	AV_DATA av_data;
	int vps_size = 0, sps_size = 0, pps_size = 0;
	void *pframe = NULL;
	int framesize = 0, totalsize;

	WaitVideoStart(vType, vNum, &av_data);		// must be I frame
	pframe = av_data.ptr;
	framesize = av_data.size;

	if (vType == VIDEO_TYPE_H265 || vType == VIDEO_TYPE_H265_CIF || vType == VIDEO_TYPE_H265_SD) {
		vps_size = framesize;
		sps_size = av_data.volNalLens[1];
		pps_size = av_data.volNalLens[2];
		totalsize = vps_size + sps_size + pps_size;
		RTSP_MEMCPY((unsigned char *)pBuff, (unsigned char *)pframe, totalsize);
	}

	if (vType == VIDEO_TYPE_H264 || vType == VIDEO_TYPE_H264_CIF || vType == VIDEO_TYPE_H264_SD) {
		sps_size = framesize;
		pps_size = av_data.volNalLens[1];
		totalsize = sps_size + pps_size;
		RTSP_MEMCPY((unsigned char *)pBuff, (unsigned char *)pframe, totalsize);
	}
	*vsize = vps_size;
	*ssize = sps_size;
	*psize = pps_size;
	return 1;
}

