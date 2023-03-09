/*
 * nc_hi_audio.h
 *
 *  Created on: 2017. 4. 10.
 *      Author: kwanho2
 */

#ifndef _HI3531_NC_HI_AUDIO_H_
#define _HI3531_NC_HI_AUDIO_H_

#include "nc_hi_common.h"
#include "../soc_api.h"


typedef struct tagSAMPLE_AI_S
{
    HI_BOOL bStart;
    HI_S32  AiDev;
    HI_S32  AiChn;
    HI_S32  AencChn;
    HI_S32  AoDev;
    HI_S32  AoChn;
    HI_BOOL bSendAenc;
    HI_BOOL bSendAo;
    pthread_t stAiPid;
} SAMPLE_AI_S;

void NC_HI_AUDIO_INIT(AI_CHN AiChn, AUDIO_SAMPLE_RATE_E sample_rate);
void NC_HI_AUDIO_DESTROY(void);

HI_S32 NC_HI_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType);

HI_S32 NC_HI_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType);

HI_S32 NC_HI_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);

HI_S32 NC_HI_AUDIO_StartHdmi(AIO_ATTR_S *pstAioAttr);

void *NC_HI_AUDIO_AiProc(void *parg);

#endif /* HI3531A_AHD_APP_AHD_APP_0314_SOC_HI3531_NC_HI_AUDIO_H_ */
