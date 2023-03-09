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
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"
#include "iniparser.h"
#include "soc_video.h"
#include "soc_motion.h"

#define MOTION_PLATFORM_CFG	 "/var/cfg/motion_platform.cfg"

typedef struct vda_mdparam
{
  pthread_t pid;
  HI_BOOL threadrun;
  VDEC_CHN VdaChn;
  HI_BOOL detected;
} VDA_MDPARAM_S;

VDA_MD_ATTR_S gstVdaMdAttr;
VDA_MDPARAM_S mdparam[MAX_CAM_CHN];


void load_motion_vdamdConfigFile(void)
{
  dictionary* cfgdictionary = NULL;
  int s32Temp = 0;
  char acTempStr[128];
  char* pcTempStr = NULL;

	gstVdaMdAttr.enVdaAlg      = VDA_ALG_REF;
  gstVdaMdAttr.enMbSize      = VDA_MB_16PIXEL;
  gstVdaMdAttr.enMbSadBits   = VDA_MB_SAD_8BIT;
  gstVdaMdAttr.enRefMode     = VDA_REF_MODE_DYNAMIC;
  gstVdaMdAttr.u32MdBufNum   = 8;
  gstVdaMdAttr.u32VdaIntvl   = 4;
  gstVdaMdAttr.u32BgUpSrcWgt = 128;
  gstVdaMdAttr.u32SadTh      = 100;
  gstVdaMdAttr.u32ObjNumMax  = 128;

  cfgdictionary = iniparser_load(MOTION_PLATFORM_CFG);
  if (NULL == cfgdictionary) {
    DBG("load %s failed\n", MOTION_PLATFORM_CFG);
    return;
  }
  
  snprintf(acTempStr, sizeof(acTempStr), "md:enVdaAlg");
  pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
  if (pcTempStr == NULL) gstVdaMdAttr.enVdaAlg = VDA_ALG_REF;
  else if (strcmp(pcTempStr, "VDA_ALG_BG") == 0) gstVdaMdAttr.enVdaAlg = VDA_ALG_BG;
  else if (strcmp(pcTempStr, "VDA_ALG_REF") == 0) gstVdaMdAttr.enVdaAlg = VDA_ALG_REF;
  DBG("%s=%s\n",acTempStr, pcTempStr);

  snprintf(acTempStr, sizeof(acTempStr), "md:enMbSize");
  pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
  if (pcTempStr == NULL) gstVdaMdAttr.enMbSize = VDA_MB_16PIXEL;
  else if (strcmp(pcTempStr, "VDA_MB_8PIXEL") == 0) gstVdaMdAttr.enMbSize = VDA_MB_8PIXEL;
  else if (strcmp(pcTempStr, "VDA_MB_16PIXEL") == 0) gstVdaMdAttr.enMbSize = VDA_MB_16PIXEL;
  DBG("%s=%s\n",acTempStr, pcTempStr);

  snprintf(acTempStr, sizeof(acTempStr), "md:enMbSadBits");
  pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
  if (pcTempStr == NULL) gstVdaMdAttr.enMbSadBits = VDA_MB_SAD_8BIT;
  else if (strcmp(pcTempStr, "VDA_MB_SAD_16BIT") == 0) gstVdaMdAttr.enMbSadBits = VDA_MB_SAD_16BIT;
  else if (strcmp(pcTempStr, "VDA_MB_SAD_8BIT") == 0) gstVdaMdAttr.enMbSadBits = VDA_MB_SAD_8BIT;
  DBG("%s=%s\n",acTempStr, pcTempStr);

  snprintf(acTempStr, sizeof(acTempStr), "md:enRefMode");
  pcTempStr = (char*)iniparser_getstring(cfgdictionary, acTempStr, NULL);
  if (pcTempStr == NULL) gstVdaMdAttr.enRefMode = VDA_REF_MODE_DYNAMIC;
  else if (strcmp(pcTempStr, "VDA_REF_MODE_STATIC") == 0) gstVdaMdAttr.enRefMode = VDA_REF_MODE_STATIC;
  else if (strcmp(pcTempStr, "VDA_REF_MODE_DYNAMIC") == 0) gstVdaMdAttr.enRefMode = VDA_REF_MODE_DYNAMIC;
  DBG("%s=%s\n",acTempStr, pcTempStr);
  
  s32Temp = 0;
  snprintf(acTempStr, sizeof(acTempStr), "md:u32MdBufNum");
  s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
  if (FAILURE != s32Temp && s32Temp > 0 && s32Temp <= 16)
    gstVdaMdAttr.u32MdBufNum = s32Temp;
  DBG("%s=%d, %d\n",acTempStr, s32Temp, gstVdaMdAttr.u32MdBufNum);

  s32Temp = 0;
  snprintf(acTempStr, sizeof(acTempStr), "md:u32VdaIntvl");
  s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
  if (FAILURE != s32Temp && s32Temp >= 0 && s32Temp <= 256)
    gstVdaMdAttr.u32VdaIntvl = s32Temp;
  DBG("%s=%d, %d\n",acTempStr, s32Temp, gstVdaMdAttr.u32VdaIntvl);

  s32Temp = 0;
  snprintf(acTempStr, sizeof(acTempStr), "md:u32BgUpSrcWgt");
  s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
  if (FAILURE != s32Temp && s32Temp > 0 && s32Temp < 256)
    gstVdaMdAttr.u32BgUpSrcWgt = s32Temp;
  DBG("%s=%d, %d\n",acTempStr, s32Temp, gstVdaMdAttr.u32BgUpSrcWgt);

  s32Temp = 0;
  snprintf(acTempStr, sizeof(acTempStr), "md:u32SadTh");
  s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
  if (FAILURE != s32Temp)
    gstVdaMdAttr.u32SadTh = s32Temp;
  DBG("%s=%d, %d\n",acTempStr, s32Temp, gstVdaMdAttr.u32SadTh);

  s32Temp = 0;
  snprintf(acTempStr, sizeof(acTempStr), "md:u32ObjNumMax");
  s32Temp = iniparser_getint(cfgdictionary, acTempStr, FAILURE);
  if (FAILURE != s32Temp && s32Temp > 0 && s32Temp <= 128)
    gstVdaMdAttr.u32ObjNumMax = s32Temp;
  DBG("%s=%d, %d\n",acTempStr, s32Temp, gstVdaMdAttr.u32ObjNumMax);
}

HI_VOID *motion_threadProc(HI_VOID *pdata)
{
  HI_S32 s32Ret;
  VDA_CHN VdaChn;
  VDA_DATA_S stVdaData;
  VDA_MDPARAM_S *pMdparam;
  HI_S32 maxfd = 0;
  FILE *fp = stdout;
  HI_S32 VdaFd;
  fd_set read_fds;
  struct timeval TimeoutVal;
  int mdcnt = 0;

  prctl(PR_SET_NAME, "hi_MdGetResult", 0, 0, 0);

  pMdparam = (VDA_MDPARAM_S *)pdata;

  VdaChn   = pMdparam->VdaChn;

  /* decide the stream file name, and open file to save stream */
  /* Set Venc Fd. */
  VdaFd = HI_MPI_VDA_GetFd(VdaChn);
  if (VdaFd < 0) {
    SAMPLE_PRT("HI_MPI_VDA_GetFd(%d) failed with %#x!\n", VdaChn, VdaFd);
    return NULL;
  }
  if (maxfd <= VdaFd)
    maxfd = VdaFd;

	pMdparam->detected = HI_FALSE;
  while (HI_TRUE == pMdparam->threadrun) {
    FD_ZERO(&read_fds);
    FD_SET(VdaFd, &read_fds);

    TimeoutVal.tv_sec  = 2;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0) {
      SAMPLE_PRT("VDA%d select failed!\n", VdaChn);
      pMdparam->detected = HI_FALSE;
      break;
    } else if (s32Ret == 0) {
      SAMPLE_PRT("get vda[%d] result time out, exit thread\n", VdaChn);
      pMdparam->detected = HI_FALSE;
      break;
    } else {
      if (FD_ISSET(VdaFd, &read_fds)) {
        /*******************************************************
        step 2.3 : call mpi to get one-frame stream
        *******************************************************/
        s32Ret = HI_MPI_VDA_GetData(VdaChn, &stVdaData, -1);
        if(s32Ret != HI_SUCCESS) {
          SAMPLE_PRT("HI_MPI_VDA_GetData(%d) failed with %#x!\n", VdaChn, s32Ret);
          pMdparam->detected = HI_FALSE;
          return NULL;
        }
        /*******************************************************
        *step 2.4 : save frame to file
        *******************************************************/
       #if 1
			  if (HI_TRUE != stVdaData.unData.stMdData.bObjValid) {
          fprintf(fp, "VDA%d bMbObjValid = FALSE.\n", VdaChn);
          mdcnt = 0;
				}

//        fprintf(fp, "VDA%d ObjNum=%d, AlarmPixCnt=%d, pts=%lld\n", VdaChn,
//					stVdaData.unData.stMdData.stObjData.u32ObjNum,
//					stVdaData.unData.stMdData.u32AlarmPixCnt,
//					stVdaData.u64Pts / 1000);
		    
		    if (stVdaData.unData.stMdData.stObjData.u32ObjNum > 0 && mdcnt < 100) mdcnt++;
		    if (stVdaData.unData.stMdData.stObjData.u32ObjNum == 0) mdcnt = 0;
		    
		    if (mdcnt > 2)
					pMdparam->detected = HI_TRUE;
				else
					pMdparam->detected = HI_FALSE;
       #else
//        SAMPLE_COMM_VDA_MdPrtSad(fp, &stVdaData);
        SAMPLE_COMM_VDA_MdPrtObj(fp, &stVdaData);
        SAMPLE_COMM_VDA_MdPrtAp(fp, &stVdaData);
       #endif
        /*******************************************************
        *step 2.5 : release stream
        *******************************************************/
        s32Ret = HI_MPI_VDA_ReleaseData(VdaChn,&stVdaData);
        if(s32Ret != HI_SUCCESS) {
          SAMPLE_PRT("HI_MPI_VDA_ReleaseData failed with %#x!\n", s32Ret);
          return NULL;
        }
      }
    }
  }
  pMdparam->detected = HI_FALSE;
  return HI_NULL;
}

int motion_soc_vdaStart(int vs, int sadth)
{
  HI_S32 s32Ret = HI_SUCCESS;
  VDA_CHN_ATTR_S stVdaChnAttr;
  MPP_CHN_S stSrcChn, stDestChn;
  VPSS_CHN VpssChn = 3;
  VDA_CHN VdaChn = vs;
printf("start vda%d\n", VdaChn);

  if (VdaChn < 0 || VdaChn >= MAX_VS_NUM) {
		printf("vda%d channel is invalid\n", VdaChn);
		return -1;
	}
  /* step 1 create vda channel */
  stVdaChnAttr.enWorkMode = VDA_WORK_MODE_MD;
  stVdaChnAttr.u32Width   = 720; //videoPhyAttr[VpssChn].vpssMode_Width;
  stVdaChnAttr.u32Height  = 576; //videoPhyAttr[VpssChn].vpssMode_Height;

  stVdaChnAttr.unAttr.stMdAttr.enVdaAlg      = gstVdaMdAttr.enVdaAlg; //VDA_ALG_REF;
  stVdaChnAttr.unAttr.stMdAttr.enMbSize      = gstVdaMdAttr.enMbSize; //VDA_MB_16PIXEL;
  stVdaChnAttr.unAttr.stMdAttr.enMbSadBits   = gstVdaMdAttr.enMbSadBits; //VDA_MB_SAD_8BIT;
  stVdaChnAttr.unAttr.stMdAttr.enRefMode     = gstVdaMdAttr.enRefMode; //VDA_REF_MODE_DYNAMIC;
  stVdaChnAttr.unAttr.stMdAttr.u32MdBufNum   = gstVdaMdAttr.u32MdBufNum; //8;
  stVdaChnAttr.unAttr.stMdAttr.u32VdaIntvl   = gstVdaMdAttr.u32VdaIntvl; //4;
  stVdaChnAttr.unAttr.stMdAttr.u32BgUpSrcWgt = gstVdaMdAttr.u32BgUpSrcWgt; //128;
  stVdaChnAttr.unAttr.stMdAttr.u32SadTh      = sadth; //100;
  stVdaChnAttr.unAttr.stMdAttr.u32ObjNumMax  = gstVdaMdAttr.u32ObjNumMax; //128;

  s32Ret = HI_MPI_VDA_CreateChn(VdaChn, &stVdaChnAttr);
  if(s32Ret != HI_SUCCESS) {
    SAMPLE_PRT("HI_MPI_VDA_CreateChn err!\n");
    return s32Ret;
  }

  /* step 2: vda chn bind vi chn */
  stSrcChn.enModId = HI_ID_VPSS;
  stSrcChn.s32ChnId = VpssChn;
  stSrcChn.s32DevId = vs;

  stDestChn.enModId = HI_ID_VDA;
  stDestChn.s32ChnId = VdaChn;
  stDestChn.s32DevId = 0;

  s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if(s32Ret != HI_SUCCESS) {
    SAMPLE_PRT("bind err!\n");
    return s32Ret;
  }

  /* step 3: vda chn start recv picture */
  s32Ret = HI_MPI_VDA_StartRecvPic(VdaChn);
  if(s32Ret != HI_SUCCESS) {
    SAMPLE_PRT("HI_MPI_VDA_StartRecvPic err!\n");
    return s32Ret;
  }
  
  /* step 4: create thread to get result */
		mdparam[VdaChn].threadrun = HI_TRUE;
		mdparam[VdaChn].VdaChn = VdaChn;
		pthread_create(&mdparam[VdaChn].pid, 0, motion_threadProc, (HI_VOID *)&mdparam[VdaChn]);
    return (int)HI_SUCCESS;
}

int motion_soc_vdaStop(int vs)
{
  HI_S32 s32Ret = HI_SUCCESS;
  MPP_CHN_S stSrcChn, stDestChn;
  VPSS_CHN VpssChn = 3;
  VDA_CHN VdaChn = vs;
  
  if (VdaChn < 0 || VdaChn >= MAX_VS_NUM) {
		printf("vda%d channel is invalid\n", VdaChn);
		return -1;
	}
  
  /* join thread */
  if (mdparam[VdaChn].threadrun == HI_TRUE) {
	  mdparam[VdaChn].threadrun = HI_FALSE;
	  pthread_join(mdparam[VdaChn].pid, 0);
		mdparam[VdaChn].detected = HI_FALSE;
		
		/* vda stop recv picture */
		s32Ret = HI_MPI_VDA_StopRecvPic(VdaChn);
		if(s32Ret != HI_SUCCESS)
			SAMPLE_PRT("HI_MPI_VDA_StopRecvPic err(0x%x)!!!!\n",s32Ret);

		/* unbind vda chn & vi chn */

		stSrcChn.enModId = HI_ID_VPSS;
		stSrcChn.s32ChnId = VpssChn;
		stSrcChn.s32DevId = 0;
		stDestChn.enModId = HI_ID_VDA;
		stDestChn.s32ChnId = VdaChn;
		stDestChn.s32DevId = 0;

		s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
		if(s32Ret != HI_SUCCESS)
			SAMPLE_PRT("unBind err(0x%x)!!!!\n", s32Ret);

  /* destroy vda chn */
		s32Ret = HI_MPI_VDA_DestroyChn(VdaChn);
		if(s32Ret != HI_SUCCESS)
			SAMPLE_PRT("HI_MPI_VDA_DestroyChn err(0x%x)!!!!\n", s32Ret);
	}
  return s32Ret;
}

int motion_getStatus(int vs)
{
	int ret = 0, i;
	
//	printf("%s(%d): ", __func__, vs);
	if (vs >= 0 && vs < MAX_CAM_CHN) {
//		printf("%d ", mdparam[vs].detected);
		ret = mdparam[vs].detected;
	} else {
		for (i = 0; i < MAX_CAM_CHN; i++) {
			printf("%d ", mdparam[i].detected);
			ret |= (mdparam[i].detected & 0x01) << i;
		}
	}
//	printf("\n");
	return ret;
}

int motion_setSensitivity(int vs, int value)
{
  HI_S32 s32Ret = HI_SUCCESS;
  VDA_CHN_ATTR_S stVdaAttr;
	VDA_CHN VdaChn = vs;
	
	if (VdaChn < 0 || VdaChn >= MAX_VS_NUM) {
    printf("vda%d channel is invalid\n", VdaChn);
	  return -1;
	}
	
	s32Ret = HI_MPI_VDA_GetChnAttr(VdaChn, &stVdaAttr);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VDA_GetChnAttr(%d) faild, error(%#x)!\n", VdaChn, s32Ret);
	}
	stVdaAttr.unAttr.stMdAttr.u32SadTh = value;
	s32Ret = HI_MPI_VDA_SetChnAttr(VdaChn, &stVdaAttr);
	if (HI_SUCCESS != s32Ret) {
		SAMPLE_PRT("HI_MPI_VDA_SetChnAttr(%d) faild, error(%#x)!\n",VdaChn, s32Ret);
	}
	return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


