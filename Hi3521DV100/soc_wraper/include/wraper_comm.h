/******************************************************************************
  for Hisilicon Hi35xx head file.

 ******************************************************************************
    Modification:  2015-10 Created
******************************************************************************/

#ifndef __WRAPER_COMM_H__
#define __WRAPER_COMM_H__

#include "av_env_type.h"

#define MAX_VENC_CHN				24
#define MAX_CAM_CHN					8
#define MAX_STREAM_NUM				3
#define SUCCESS  0
#define FAILURE  (-1)

//#if defined(dev)
#if _DBG_MSG_
#define DBG(fmt, args...)	fprintf(stderr, "HiSDKDebug " fmt, ##args)
#define _RGN_NOT_PASS(err)\
    do {\
        SAMPLE_PRT("\033[0;31mtest case <%s>not pass at line:%d err:%x\033[0;39m\n",\
               __FUNCTION__,__LINE__,err);\
    }while(0)
#define SAMPLE_DBG(s32Ret)\
    do{\
        fprintf(stderr, "s32Ret=%#x,fuc:%s,line:%d\n", s32Ret, __FUNCTION__, __LINE__);\
    }while(0)
#else
#define DBG(fmt, args...)
#define _RGN_NOT_PASS(err)
#define SAMPLE_DBG(s32Ret)
#endif

/*******************************************************
    function announce  
*******************************************************/
void _ResetFrameBuffer(int type);
void _GetCurrentFrame(int type, FrameInfo_t *frame);
void _GetCurrentFrameInfo(FrameInfo_t *frame);
void _GetLastMjpegFrame(FrameInfo_t *frame);

#endif /* End of #ifndef __WRAPER_COMM_H__ */
