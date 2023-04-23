#include "sample_comm.h"
#include <time.h>
#include "iniparser.h"
#include "soc_video_priv.h"
#include "soc_video.h"
#include "soc_video_api.h"
#include "tara_video_source.h"
#include "hi3521d.h"

#define VIDEO_PLATFORM_CFG       "/var/cfg/video_platform.cfg"

static const VpssGrpInfo vs_hw_default[1] =
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

void hicomm_init_codec_pts(void)
{
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        printf("%s: %llu\n", __func__, (ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);
        //HI_MPI_SYS_InitPtsBase((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);
}

void hicomm_sync_codec_pts(struct hi3521d_priv *priv)
{
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);

        if (priv->dbgflag == 1) {
                HI_U64 uPTS;
                struct tm *tmNow;
                time_t tNow;

                time(&tNow);
                tmNow = localtime(&tNow);
                //HI_MPI_SYS_GetCurPts(&uPTS);
                printf("=== %04d-%02d-%02d %02d:%02d:%02d ===\n", tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday,
                                                tmNow->tm_hour, tmNow->tm_min, tmNow->tm_sec);
                printf("syncCodecPTS systime=%llu %llu, hi=%llu %llu, [%ld]\n",
                        ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)/1000000, ((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000)%1000000,
                        uPTS/1000000, uPTS%1000000,
                        (long)((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000) - uPTS);
        }
        //HI_MPI_SYS_SyncPts((ts.tv_sec * 1000000LLU) + ts.tv_nsec / 1000);
}

int hicomm_load_config(struct hi3521d_priv *priv)
{
	dictionary *cfgdictionary = NULL;
        int s32Temp = 0, i, ii;
        char acTempStr[128];
        char *pcTempStr = NULL;

        memcpy(&priv->vs_hw, &vs_hw_default, sizeof(priv->vs_hw));

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
                priv->dbgflag = 1;
        DBG("%s:%s, %d\n",acTempStr, pcTempStr, priv->dbgflag);

	for (i = 0; i < 1; i++) {
                for (ii = 0; ii < VPSS_MAX_PHY_CHN_NUM; ii++) {
                        /**************phyChn:phyChn_En**************/
                        s32Temp = 0;
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:phyChn_En", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].enPhyChn = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].enPhyChn);

                        /**************phyChn:vbConf_BlkCnt**************/
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vbConf_BlkCnt", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].vbBlkCnt = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vbBlkCnt);

			/**************phyChn:vpssMode_W**************/
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_W", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].vpssModeWidth = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vpssModeWidth);

                        /**************phyChn:vpssMode_H**************/
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_H", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].vpssModeHeight = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vpssModeHeight);

			/**************phyChn:vpssMode_CompressMode**************/
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssMode_CompressMode", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].vpssModeCompressMode = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vpssModeCompressMode);

                        /**************phyChn:vpssAttr_Srcfps**************/
                        s32Temp = 0;
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssAttr_Srcfps", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        priv->vs_hw[i].vpsschn[ii].vpssAttrSrcfps = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vpssAttrSrcfps);

                        /**************phyChn:vpssAttr_Dstfps**************/
                        s32Temp = 0;
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:vpssAttr_Dstfps", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        priv->vs_hw[i].vpsschn[ii].vpssAttrDstfps = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].vpssAttrDstfps);

                       /**************phyChn:UseUiResolution**************/
                        s32Temp = 0;
                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:UseUiResolution", ii);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].vpsschn[ii].UseUiResolution = s32Temp;
                        DBG("%s=%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].vpsschn[ii].UseUiResolution);

                        snprintf(acTempStr, sizeof(acTempStr), "phyChn%d:NR", ii);
                        pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
                        if (pcTempStr == NULL)
                                DBG("%s failed\n", acTempStr);
                        else if (strcmp(pcTempStr, "On") == 0)
                                priv->vs_hw[i].vpsschn[ii].NR = 1;
                        else if (strcmp(pcTempStr, "Off") == 0)
                                priv->vs_hw[i].vpsschn[ii].NR = 0;
                        DBG("%s:%s, %d\n",acTempStr, pcTempStr, priv->vs_hw[i].vpsschn[ii].NR);
		}

                for (i = 0; i < MAX_STREAM_NUM; i++) {
                        snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:phyChn", i+1);
                        pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
                        if (pcTempStr == NULL)
                                DBG("%s failed\n", acTempStr);
                        else if (strcmp(pcTempStr, "0")==0) priv->vs_hw[0].str[i].vpssphychn = 0;
                        else if (strcmp(pcTempStr, "1")==0) priv->vs_hw[0].str[i].vpssphychn = 1;
                        else if (strcmp(pcTempStr, "2")==0) priv->vs_hw[0].str[i].vpssphychn = 2;
                        else if (strcmp(pcTempStr, "3")==0) priv->vs_hw[0].str[i].vpssphychn = 3;
                        DBG("%s=%s\n",acTempStr, pcTempStr);
                        DBG("vs_hw[0].str[%d].vpssphychn=%d\n", i, priv->vs_hw[0].str[ii].vpssphychn);

                        snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:BindVenc", i+1);
                        s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
                        if (FAILURE != s32Temp)
                                priv->vs_hw[i].str[ii].bindvpss = s32Temp;
                        DBG("%s:%d, %d\n",acTempStr, s32Temp, priv->vs_hw[i].str[ii].bindvpss);

                        snprintf(acTempStr, sizeof(acTempStr), "stream%dInfo:gopMode", i+1);
                        pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
                        if (pcTempStr == NULL)
                                DBG("%s failed\n", acTempStr);
                        else if (strcmp(pcTempStr, "dualp") == 0)
                                priv->vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_DUALP;
                        else if (strcmp(pcTempStr, "smartp") == 0)
                                priv->vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_SMARTP;
			else if (strcmp(pcTempStr, "bipredb") == 0)
                                priv->vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_BIPREDB;
                        else if (strcmp(pcTempStr, "lowdelayb") == 0)
                                priv->vs_hw[i].str[ii].enGopMode = VENC_GOPMODE_LOWDELAYB;
                        DBG("%s:%s, %d\n",acTempStr, pcTempStr, priv->vs_hw[i].str[ii].enGopMode);
                }
        }

        iniparser_freedict(cfgdictionary);
        return SUCCESS;
}
