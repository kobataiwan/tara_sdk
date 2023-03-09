#include <stdio.h>
#include <string.h>

#include <sys/poll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <linux/limits.h>				// PATH_MAX
#include <fcntl.h>                      // O_RDWR , O_NOCTTY 占쏙옙占쏙옙 占쏙옙占� 占쏙옙占쏙옙
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <linux/kdev_t.h>				// MKDEV
#include <sys/types.h>					// dev_t
#include <sys/stat.h>

#include <stdbool.h>
#include <math.h>

#include <nc_hi_common.h>
#include <nc_hi_audio.h>

#include <_debug_macro.h>

static HI_U32 g_u32AiCnt = -1;
static HI_U32 g_u32AoCnt = -1;
static HI_U32 g_u32AiChn = -1;
static HI_U32 g_u32AoChn = -1;
static HI_U32 g_u32AencCnt = -1;
static HI_U32 g_u32Adec = -1;
static HI_U32 g_u32AiDev = -1;
static HI_U32 g_u32AoDev = -1;
static PAYLOAD_TYPE_E gs_enPayloadType = PT_ADPCMA;
//static HI_BOOL gs_bMicIn = HI_FALSE;
static HI_BOOL gs_bAiVqe= HI_FALSE;
static HI_BOOL gs_bAioReSample = HI_FALSE;
static HI_BOOL gs_bUserGetMode = HI_FALSE;
static HI_BOOL gs_bAoVolumeCtrl = HI_TRUE;
static AUDIO_RESAMPLE_ATTR_S *gs_pstAiReSmpAttr;
static AUDIO_RESAMPLE_ATTR_S *gs_pstAoReSmpAttr;

static SAMPLE_AI_S gs_stSampleAi[AI_DEV_MAX_NUM*AIO_MAX_CHN_NUM];

HI_S32 NC_HI_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
        AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType)
{
	HI_S32 i;
	HI_S32 s32Ret;

	if (pstAioAttr->u32ClkChnCnt == 0)
	{
		pstAioAttr->u32ClkChnCnt = 16;
	}

	s32Ret = HI_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
	if (s32Ret)
	{
		printf("%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	s32Ret = HI_MPI_AI_Enable(AiDevId);
	if (s32Ret)
	{
		printf("%s: HI_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for (i = 0; i < s32AiChnCnt; i++)
	{
		s32Ret = HI_MPI_AI_EnableChn(AiDevId, i);
		if (s32Ret)
		{
			printf("%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
			return s32Ret;
		}

		if (HI_TRUE == bResampleEn)
		{
			s32Ret = HI_MPI_AI_EnableReSmp(AiDevId, i, enOutSampleRate);
			if (s32Ret)
			{
				printf("%s: HI_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}
		}

		if (NULL != pstAiVqeAttr)
		{
			HI_BOOL bAiVqe = HI_TRUE;
			switch (u32AiVqeType)
			{
				case 0:
					s32Ret = HI_SUCCESS;
					bAiVqe = HI_FALSE;
					break;
				case 1:
					s32Ret = HI_MPI_AI_SetVqeAttr(AiDevId, i, SAMPLE_AUDIO_AO_DEV, i, (AI_VQE_CONFIG_S *)pstAiVqeAttr);
					break;
				default:
					s32Ret = HI_FAILURE;
					break;
			}
			if (s32Ret)
			{
				printf("%s: HI_MPI_AI_SetVqeAttr(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}

			if (bAiVqe)
			{
				s32Ret = HI_MPI_AI_EnableVqe(AiDevId, i);
				if (s32Ret)
				{
					printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
					return s32Ret;
				}
			}
		}
	}

	return HI_SUCCESS;
}

HI_S32 NC_HI_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
		HI_BOOL bResampleEn, HI_BOOL bVqeEn)
{
	HI_S32 i;
	HI_S32 s32Ret;

	for (i = 0; i < s32AiChnCnt; i++)
	{
		if (HI_TRUE == bResampleEn)
		{
			s32Ret = HI_MPI_AI_DisableReSmp(AiDevId, i);
			if (HI_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		if (HI_TRUE == bVqeEn)
		{
			s32Ret = HI_MPI_AI_DisableVqe(AiDevId, i);
			if (HI_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		s32Ret = HI_MPI_AI_DisableChn(AiDevId, i);
		if (HI_SUCCESS != s32Ret)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
			return s32Ret;
		}
	}

	s32Ret = HI_MPI_AI_Disable(AiDevId);
	if (HI_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	return HI_SUCCESS;
}

HI_S32 NC_HI_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
        AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType)
{
	HI_S32 i;
	HI_S32 s32Ret;

	if (pstAioAttr->u32ClkChnCnt == 0)
	{
		pstAioAttr->u32ClkChnCnt = 16;
	}

	if (SAMPLE_AUDIO_INNER_HDMI_AO_DEV == AoDevId || AoDevId == SAMPLE_AUDIO_HDMI_AO_DEV)
	{
		pstAioAttr->u32ClkSel = 0;

		NC_HI_AUDIO_StartHdmi(pstAioAttr);
	}

	printf("[%x] [%x] [%x] [%x] \n",
			pstAioAttr->enBitwidth,
			pstAioAttr->enSamplerate,
			pstAioAttr->enSoundmode,
			pstAioAttr->enWorkmode);

	s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
			   AoDevId, s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_AO_Enable(AoDevId);
	if (HI_SUCCESS != s32Ret)
	{
		printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
		return HI_FAILURE;
	}

	for (i = 0; i < s32AoChnCnt; i++)
	{
		s32Ret = HI_MPI_AO_EnableChn(AoDevId, i);
		if (HI_SUCCESS != s32Ret)
		{
			printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
			return HI_FAILURE;
		}

		if (HI_TRUE == bResampleEn)
		{
			s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
			s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
			if (HI_SUCCESS != s32Ret)
			{
				printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
				return HI_FAILURE;
			}
		}

		if (NULL != pstAoVqeAttr)
		{
			HI_BOOL bAoVqe = HI_TRUE;
			switch (u32AoVqeType)
			{
				case 0:
					s32Ret = HI_SUCCESS;
					bAoVqe = HI_FALSE;
					break;
				case 1:
					s32Ret = HI_MPI_AO_SetVqeAttr(AoDevId, i, (AO_VQE_CONFIG_S *)pstAoVqeAttr);
					break;
				default:
					s32Ret = HI_FAILURE;
					break;
			}

			if (s32Ret)
			{
				printf("%s: SetAoVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AoVqeType, AoDevId, i, s32Ret);
				return s32Ret;
			}

			if (bAoVqe)
			{
				s32Ret = HI_MPI_AO_EnableVqe(AoDevId, i);
				if (s32Ret)
				{
					printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AoDevId, i, s32Ret);
					return s32Ret;
				}
			}
		}
	}

	return HI_SUCCESS;
}

HI_S32 NC_HI_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AoChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
                return s32Ret;
            }
        }

        if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AO_DisableVqe(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AO_DisableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AO_Disable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    if (SAMPLE_AUDIO_HDMI_AO_DEV == AoDevId)
    {
        s32Ret = SAMPLE_COMM_AUDIO_StopHdmi();
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: SAMPLE_COMM_AUDIO_StopHdmi failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 NC_HI_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    SAMPLE_AI_S *pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev*AIO_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = HI_FALSE;
    pstAi->bSendAo = HI_TRUE;
    pstAi->bStart= HI_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AoDev = AoDev;
    pstAi->AoChn = AoChn;

    pthread_create(&pstAi->stAiPid, 0, NC_HI_AUDIO_AiProc, pstAi);

    return HI_SUCCESS;
}

HI_S32 NC_HI_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn)
{
    SAMPLE_AI_S *pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev*AIO_MAX_CHN_NUM + AiChn];

    if (pstAi->bStart)
    {
        pstAi->bStart= HI_FALSE;
        //pthread_cancel(pstAi->stAiPid);
        pthread_join(pstAi->stAiPid, 0);

    }

    return HI_SUCCESS;
}

void *NC_HI_AUDIO_AiProc(void *parg)
{
    HI_S32 s32Ret;
    HI_S32 AiFd;
    SAMPLE_AI_S *pstAiCtl = (SAMPLE_AI_S *)parg;
    AUDIO_FRAME_S stFrame;
    fd_set read_fds;
    struct timeval TimeoutVal;
    AI_CHN_PARAM_S stAiChnPara;

    s32Ret = HI_MPI_AI_GetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    stAiChnPara.u32UsrFrmDepth = 30;

    s32Ret = HI_MPI_AI_SetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    FD_ZERO(&read_fds);
    AiFd = HI_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd,&read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd,&read_fds);

        s32Ret = select(AiFd+1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            s32Ret = HI_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AI_GetFrame(%d, %d), failed with %#x!\n",\
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
            }

            /* send frame to encoder */
            if (HI_TRUE == pstAiCtl->bSendAenc)
            {
                s32Ret = HI_MPI_AENC_SendFrame(pstAiCtl->AencChn, &stFrame, NULL);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AENC_SendFrame(%d), failed with %#x!\n",\
                           __FUNCTION__, pstAiCtl->AencChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }
            }

            /* send frame to ao */
            if (HI_TRUE == pstAiCtl->bSendAo)
            {
                s32Ret = HI_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 1000);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",\
                           __FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }

            }

            /* finally you must release the stream */
            s32Ret = HI_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n",\
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = HI_FALSE;

	printf("%s destroyed!\n",__FUNCTION__);

    return NULL;
}

HI_S32 NC_HI_AUDIO_StartHdmi(AIO_ATTR_S *pstAioAttr)
{
    HI_S32 s32Ret;
    HI_HDMI_ATTR_S stHdmiAttr;
    HI_HDMI_ID_E enHdmi = HI_HDMI_ID_0;
    VO_PUB_ATTR_S stPubAttr;
    VO_DEV VoDev = 0;

    stPubAttr.u32BgColor = 0x000000ff;
    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P30;

//    if(HI_SUCCESS != SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr))
//    {
//        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
//        return HI_FAILURE;
//    }
//
//    s32Ret = SAMPLE_COMM_VO_HdmiStart(stPubAttr.enIntfSync);
//    if(HI_SUCCESS != s32Ret)
//    {
//        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
//        return HI_FAILURE;
//    }

    s32Ret = HI_MPI_HDMI_SetAVMute(enHdmi, HI_TRUE);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_GetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    stHdmiAttr.bEnableAudio = HI_TRUE;        /**< if enable audio */
    stHdmiAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S; /**< source of HDMI audio, HI_HDMI_SND_INTERFACE_I2S suggested.the parameter must be consistent with the input of AO*/
    stHdmiAttr.enSampleRate = pstAioAttr->enSamplerate;        /**< sampling rate of PCM audio,the parameter must be consistent with the input of AO */
    stHdmiAttr.u8DownSampleParm = HI_FALSE;    /**< parameter of downsampling  rate of PCM audio, default :0 */

    stHdmiAttr.enBitDepth = 8 * (pstAioAttr->enBitwidth+1);   /**< bitwidth of audio,default :16,the parameter must be consistent with the config of AO */
    stHdmiAttr.u8I2SCtlVbit = 0;        /**< reserved, should be 0, I2S control (0x7A:0x1D) */

    stHdmiAttr.bEnableAviInfoFrame = HI_TRUE; /**< if enable  AVI InfoFrame*/
    stHdmiAttr.bEnableAudInfoFrame = HI_TRUE;; /**< if enable AUDIO InfoFrame*/

    s32Ret = HI_MPI_HDMI_SetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_SetAVMute(enHdmi, HI_FALSE);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

inline HI_S32 SAMPLE_COMM_AUDIO_StopHdmi(HI_VOID)
{
	HI_S32 s32Ret;
	VO_DEV VoDev = 0;

	//s32Ret =  SAMPLE_COMM_VO_HdmiStop();
	s32Ret |= HI_MPI_VO_Disable(VoDev);
	if(HI_SUCCESS != s32Ret)
	{
		printf("%s: HI_MPI_VO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
		return HI_FAILURE;
	}

	return s32Ret;
}

void NC_HI_AUDIO_INIT(AI_CHN AiChn, AUDIO_SAMPLE_RATE_E sample_rate)
{
	HI_S32 s32Ret, s32AiChnCnt;
	AUDIO_DEV   AiDev = SAMPLE_AUDIO_AI_DEV;
	AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;
	AO_CHN      AoChn = 0;

	AIO_ATTR_S stAioAttr;
	AIO_ATTR_S stHdmiAoAttr;
	AUDIO_RESAMPLE_ATTR_S stAoReSampleAttr;

	stAioAttr.enSamplerate   = sample_rate;
	stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
	stAioAttr.enWorkmode     = AIO_MODE_I2S_SLAVE;
	stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32EXFlag      = 1;
	stAioAttr.u32FrmNum      = 30;
	stAioAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
	stAioAttr.u32ChnCnt      = 16;
	stAioAttr.u32ClkChnCnt   = 16;
	stAioAttr.u32ClkSel      = 0;

	stHdmiAoAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
	stHdmiAoAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
	stHdmiAoAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
	stHdmiAoAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
	stHdmiAoAttr.u32EXFlag      = 1;
	stHdmiAoAttr.u32FrmNum      = 30;
	//stHdmiAoAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
	stHdmiAoAttr.u32PtNumPerFrm = stAioAttr.u32PtNumPerFrm*6;
	stHdmiAoAttr.u32ChnCnt      = 1;
	stHdmiAoAttr.u32ClkChnCnt   = 1;
	stHdmiAoAttr.u32ClkSel      = 0;

	printf("%s: Set Ain:%d - Framerate:%d\n", __FUNCTION__, AiChn, sample_rate);

	/* ao 8k -> 48k */
	stAoReSampleAttr.u32InPointNum  = SAMPLE_AUDIO_PTNUMPERFRM;
	stAoReSampleAttr.enInSampleRate = sample_rate;
	stAoReSampleAttr.enOutSampleRate = AUDIO_SAMPLE_RATE_48000;
	gs_pstAoReSmpAttr = &stAoReSampleAttr;
	gs_bAioReSample = HI_TRUE;
	/* resample and anr should be user get mode */
	gs_bUserGetMode = (HI_TRUE == gs_bAioReSample || HI_TRUE == gs_bAiVqe) ? HI_TRUE : HI_FALSE;

	/* enable AI channle */
	s32AiChnCnt = stAioAttr.u32ChnCnt;
	g_u32AiCnt = s32AiChnCnt;
	g_u32AiDev = AiDev;
	g_u32AiChn = AiChn;
	// s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_FALSE, NULL, 0);
	 s32Ret = NC_HI_AUDIO_StartAi(SAMPLE_AUDIO_AI_DEV, s32AiChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_FALSE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* enable AO channle */
	stAioAttr.u32ChnCnt = 1;
	stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	s32Ret = NC_HI_AUDIO_StartAo(SAMPLE_AUDIO_AO_DEV, stAioAttr.u32ChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate,HI_FALSE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* enable HDMI AO channle */
	g_u32AoCnt = stHdmiAoAttr.u32ChnCnt;
	g_u32AoDev = AoDev;
	g_u32AoChn = AoChn;
	s32Ret = NC_HI_AUDIO_StartAo(AoDev, stHdmiAoAttr.u32ChnCnt, &stHdmiAoAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_TRUE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* AI to AO channel */
	s32Ret = NC_HI_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AoChn);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	DBG_INFO("  HDMI Audio Output Init Success !!!! \n ");

}

void NC_HI_AUDIO_DESTROY(void)
{
	HI_S32 s32Ret;

	if((g_u32AoDev == -1) && (g_u32AoCnt == -1) &&
		(g_u32AiDev == -1) && (g_u32AiCnt == -1))
	{
		printf("%s: no AIO-devices opened!\n", __FUNCTION__);
		return;
	}

	if (HI_TRUE == gs_bUserGetMode)
	{
		s32Ret = NC_HI_AUDIO_DestoryTrdAi(g_u32AiDev, g_u32AiChn);
		if (s32Ret != HI_SUCCESS)
		{
			printf("%s: NC_HI_AUDIO_DestoryTrdAi failed with %#x!\n", __FUNCTION__, s32Ret);
			return;
		}
		else
		{
			g_u32AiChn = -1;
			printf("NC_HI_AUDIO_DestoryTrdAi destroyed!\n");
		}
	}

	s32Ret = NC_HI_AUDIO_StopAo(g_u32AoDev, g_u32AoCnt, gs_bAioReSample, HI_FALSE);
	if (s32Ret != HI_SUCCESS)
	{
		printf("%s: NC_HI_AUDIO_StopAo failed with %#x!\n", __FUNCTION__, s32Ret);
		return;
	}
	else
	{
		g_u32AoDev = -1;
		g_u32AoCnt = -1;
		gs_bAioReSample = HI_FALSE;
		printf("NC_HI_AUDIO_StopAo destroyed!\n");
	}

	s32Ret = NC_HI_AUDIO_StopAi(g_u32AiDev, g_u32AiCnt, HI_FALSE, HI_FALSE);
	if (s32Ret != HI_SUCCESS)
	{
		printf("%s: NC_HI_AUDIO_StopAi failed with %#x!\n", __FUNCTION__, s32Ret);
		return;
	}
	else
	{
		g_u32AiDev = -1;
		g_u32AiCnt = -1;
		printf("NC_HI_AUDIO_StopAi destroyed!\n");
	}

	printf("%s Success!\n", __FUNCTION__);
}

#if 0
void NC_HI_AUDIO_INIT(void)
{
	static PAYLOAD_TYPE_E gs_enPayloadType = PT_ADPCMA;
	//static HI_BOOL gs_bMicIn = HI_FALSE;
	static HI_BOOL gs_bAiVqe= HI_FALSE;
	static HI_BOOL gs_bAioReSample = HI_FALSE;
	static HI_BOOL gs_bUserGetMode = HI_FALSE;
	static HI_BOOL gs_bAoVolumeCtrl = HI_TRUE;
	static AUDIO_RESAMPLE_ATTR_S *gs_pstAiReSmpAttr = NULL;
	static AUDIO_RESAMPLE_ATTR_S *gs_pstAoReSmpAttr = NULL;
	static HI_U32 g_u32AiCnt = 0;
	static HI_U32 g_u32AoCnt = 0;
	static HI_U32 g_u32AencCnt = 0;
	static HI_U32 g_u32Adec = 0;
	static HI_U32 g_u32AiDev = 0;
	static HI_U32 g_u32AoDev = 0;


	HI_S32 s32Ret, s32AiChnCnt;
	AUDIO_DEV   AiDev = SAMPLE_AUDIO_AI_DEV;
	AI_CHN      AiChn = 0;
	AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;
	AO_CHN      AoChn = 0;

	AIO_ATTR_S stAioAttr;
	AIO_ATTR_S stHdmiAoAttr;
	AUDIO_RESAMPLE_ATTR_S stAoReSampleAttr;

	stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
	stAioAttr.enWorkmode     = AIO_MODE_I2S_SLAVE;
	stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32EXFlag      = 1;
	stAioAttr.u32FrmNum      = 30;
	stAioAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
	stAioAttr.u32ChnCnt      = 16;
	stAioAttr.u32ClkChnCnt   = 16;
	stAioAttr.u32ClkSel      = 0;

	stHdmiAoAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
	stHdmiAoAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
	stHdmiAoAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
	stHdmiAoAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
	stHdmiAoAttr.u32EXFlag      = 1;
	stHdmiAoAttr.u32FrmNum      = 30;
	//stHdmiAoAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
	stHdmiAoAttr.u32PtNumPerFrm = stAioAttr.u32PtNumPerFrm*6;
	stHdmiAoAttr.u32ChnCnt      = 1;
	stHdmiAoAttr.u32ClkChnCnt   = 1;
	stHdmiAoAttr.u32ClkSel      = 0;

	/* ao 8k -> 48k */
	stAoReSampleAttr.u32InPointNum  = SAMPLE_AUDIO_PTNUMPERFRM;
	stAoReSampleAttr.enInSampleRate = AUDIO_SAMPLE_RATE_8000;
	stAoReSampleAttr.enOutSampleRate = AUDIO_SAMPLE_RATE_48000;
	gs_pstAoReSmpAttr = &stAoReSampleAttr;
	gs_bAioReSample = HI_TRUE;
	/* resample and anr should be user get mode */
	gs_bUserGetMode = (HI_TRUE == gs_bAioReSample || HI_TRUE == gs_bAiVqe) ? HI_TRUE : HI_FALSE;

	/* enable AI channle */
	s32AiChnCnt = stAioAttr.u32ChnCnt;
	g_u32AiCnt = s32AiChnCnt;
	g_u32AiDev = AiDev;
	// s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_FALSE, NULL, 0);
	 s32Ret = NC_HI_AUDIO_StartAi(SAMPLE_AUDIO_AI_DEV, s32AiChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_FALSE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* enable AO channle */
	stAioAttr.u32ChnCnt = 1;
	stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	s32Ret = NC_HI_AUDIO_StartAo(SAMPLE_AUDIO_AO_DEV, stAioAttr.u32ChnCnt, &stAioAttr, gs_pstAoReSmpAttr->enInSampleRate,HI_FALSE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* enable HDMI AO channle */
	g_u32AoCnt = stHdmiAoAttr.u32ChnCnt;
	g_u32AoDev = AoDev;
	s32Ret = NC_HI_AUDIO_StartAo(AoDev, stHdmiAoAttr.u32ChnCnt, &stHdmiAoAttr, gs_pstAoReSmpAttr->enInSampleRate, HI_TRUE, NULL, 0);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	/* AI to AO channel */
	s32Ret = NC_HI_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AoChn);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("%x\n",s32Ret);
		//return HI_FAILURE;
	}

	DBG_INFO("  HDMI Audio Output Init Success !!!! \n ");

}

void NC_HI_AUDIO_DESTROY(void)
{
	HI_S32 s32Ret;

	if((g_u32AoDev == -1) && (g_u32AoCnt == -1) &&
		(g_u32AiDev == -1) && (g_u32AiCnt == -1))
	{
		printf("%s: no AIO-devices opened!\n", __FUNCTION__);
		return;
	}

	if (HI_TRUE == gs_bUserGetMode)
	{
		s32Ret = NC_HI_AUDIO_DestoryTrdAi(g_u32AiDev, g_u32AiChn);
		if (s32Ret != HI_SUCCESS)
		{
			printf("%s: NC_HI_AUDIO_DestoryTrdAi failed with %#x!\n", __FUNCTION__, s32Ret);
			return;
		}
		else
		{
			g_u32AiChn = -1;
			printf("NC_HI_AUDIO_DestoryTrdAi destroyed!\n");
		}
	}

	s32Ret = NC_HI_AUDIO_StopAo(g_u32AoDev, g_u32AoCnt, gs_bAioReSample, HI_FALSE);
	if (s32Ret != HI_SUCCESS)
	{
		printf("%s: NC_HI_AUDIO_StopAo failed with %#x!\n", __FUNCTION__, s32Ret);
		return;
	}
	else
	{
		g_u32AoDev = -1;
		g_u32AoCnt = -1;
		gs_bAioReSample = HI_FALSE;
		printf("NC_HI_AUDIO_StopAo destroyed!\n");
	}

	s32Ret = NC_HI_AUDIO_StopAi(g_u32AiDev, g_u32AiCnt, HI_FALSE, HI_FALSE);
	if (s32Ret != HI_SUCCESS)
	{
		printf("%s: NC_HI_AUDIO_StopAi failed with %#x!\n", __FUNCTION__, s32Ret);
		return;
	}
	else
	{
		g_u32AiDev = -1;
		g_u32AiCnt = -1;
		printf("NC_HI_AUDIO_StopAi destroyed!\n");
	}

	printf("%s Success!\n", __FUNCTION__);
}
#endif



