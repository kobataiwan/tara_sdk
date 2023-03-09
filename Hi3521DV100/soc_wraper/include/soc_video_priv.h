#ifndef __SOC_VIDEO_PRIV_H__
#define __SOC_VIDEO_PRIV_H__
#include "wraper_comm.h"

#define STR_RB_MAX 	8

typedef struct PACK_INFO_s
{
	HI_S32 offset;
	HI_S32 size;	
}PACK_INFO_S;

typedef struct STREAM_FILE_INFO_s
{
	HI_S32 serialNo;
	HI_S32 frameCnt;
	HI_S32 packCnt;
	PACK_INFO_S packInfo[5];
	HI_U64 timestamp;
}STREAM_FILE_INFO_S;

typedef struct STR_CTRL_s
{
	HI_S32 channelNo;
	HI_S32 serialNo;
	HI_S32 frameCnt;
	FILE *pFile;
	STREAM_FILE_INFO_S rbuf[STR_RB_MAX];
	HI_S32 rb_rdidx;
	HI_S32 rb_wtidx;
	HI_S32 lastOffset;
	HI_S32 lastSize;
	HI_U64 timestamp;
	FrameFormat_t format;
	void *vq;
}STR_CTRL_S;

typedef struct {
	HI_BOOL enPhyChn;
	HI_U32 vbBlkCnt;
	HI_U32 vpssModeWidth;
	HI_U32 vpssModeHeight;
	COMPRESS_MODE_E vpssModeCompressMode;
	HI_S32 vpssAttrSrcfps;
	HI_S32 vpssAttrDstfps;
	HI_BOOL UseUiResolution;
	HI_BOOL NR;	
} VpssChnAttr;

typedef struct {
	HI_U32 vpssphychn;
	HI_BOOL bindvpss;
	VENC_GOP_MODE_E enGopMode;
} StreamAttr;

typedef struct {
	int maxWidth;
	int maxHeight;
	VpssChnAttr vpsschn[4]; 
	StreamAttr str[MAX_STREAM_NUM];
} VpssGrpInfo;

typedef struct avserver_venc_getstream_s
{
	HI_BOOL bThreadStart;
	int  		strtype;
//	int			s32MaxVi;
}AVSERVER_VENC_GETSTREAM_PARA_S;

typedef struct GetStreamThreadCtrl_s{
	int chnidx;
	int thrStart;
	int getstr;
	pthread_t threadPid;
} GetStreamThreadCtrl;

#endif /* End of #ifndef __SOC_VIDEO_PRIV_H__ */

