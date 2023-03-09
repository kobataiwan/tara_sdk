#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "nc_hi_common.h"
#include <linux/fb.h>
#include "hifb.h"

#include "soc_api.h"

#include "nc_hi_vivo.h"
#include <_debug_macro.h>

#include <video_decoder.h>
#include <raptor3_audio.h>


unsigned static int gSoC_Video_SetConfig_1Mux_Port_Flag[SoC_MAX_PORT_NUM] = {0, };
unsigned static int gSoC_Video_SetConfig_2Mux_Port_Flag[SoC_MAX_PORT_NUM] = {0, };
unsigned static int gSoC_Video_SetConfig_4Mux_Port_Flag[SoC_MAX_PORT_NUM] = {0, };
unsigned static int gSoC_Video_SetConfig_BT1120_Port_Set_Flag[SoC_MAX_PORT_NUM] = {0, };

unsigned static int gSoC_Video_SetConfig_Manual_Port_Flag[SoC_MAX_PORT_NUM] = {0, };
unsigned static int g_Manual_Port_Setting_Flag[SoC_MAX_PORT_NUM]= {0, };

int SOC_SAL_Video_SetFormat(unsigned char ch, NC_VIVO_CH_FORMATDEF *pVideofmt )
{
	int SoC_port;
	int soc_ch;
	int vd_ch;
	int port;
	int i;
	unsigned char oCahnnel;
	NC_VIVO_CH_FORMATDEF FmtDef;
	NC_HI_VI_PORT_ATTR_S SoC_portAttr = {0,};
	NC_VI_CH_FMT_S *pChFmt;


	oCahnnel = ch%4;
	port = ch%4;
	SoC_port = oCahnnel;
	soc_ch = (oCahnnel * 4) ;

	for( i = ch*4; i < ch*4+4; i++)
		HI_MPI_VI_DisableChn(i);

	HI_MPI_VI_DisableDev(port);

	SoC_portAttr.Interface = VI_INPUT_MODE_BT656;
	SoC_portAttr.Multiplex = VI_WORK_MODE_1Multiplex;

	FmtDef = pVideofmt[oCahnnel];
	if(	FmtDef == AHD30_4M_30P || \
		FmtDef == AHD30_4M_25P || \
		FmtDef == AHD30_3M_30P || \
		FmtDef == AHD30_3M_25P || \
		FmtDef == AHD30_5M_20P || \
		FmtDef == AHD30_5_3M_20P || \
		FmtDef == AHD30_6M_18P || \
		FmtDef == AHD30_6M_20P || \
		FmtDef == AHD30_8M_15P || \
		FmtDef == AHD30_8M_12_5P || \
		FmtDef == CVI_4M_25P || \
		FmtDef == CVI_4M_30P || \
		FmtDef == CVI_8M_12_5P || \
		FmtDef == CVI_8M_15P || \
		FmtDef == TVI_4M_25P || \
		FmtDef == TVI_4M_30P || \
		FmtDef == TVI_5M_20P
		)
		SoC_portAttr.ClkEdge = VI_CLK_EDGE_DOUBLE;

	NC_HI_VI_Port_SetFormat( SoC_port, &SoC_portAttr );

	// Do Setup with the Format!
	pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( FmtDef );
	if(pChFmt)
	{
		NC_HI_VI_Channel_SetFormat( soc_ch, soc_ch, SoC_portAttr.Multiplex, pChFmt, 0 );
	}

	return 0;
}

void SOC_SAL_Video_2MuxSetFormat(unsigned char ch, NC_VIVO_CH_FORMATDEF *pVideofmt)
{
    int i = 0;
    int SoC_port;
    int soc_ch;
    int port;
    unsigned char oCahnnel;
    NC_VIVO_CH_FORMATDEF FmtDef;
    NC_HI_VI_PORT_ATTR_S SoC_portAttr = {0,};
    NC_VI_CH_FMT_S *pChFmt;

    port = ch/2 ;
    SoC_port = ch/2;
    soc_ch = (ch*2);

    for( i = ch*2; i < ch*2+2; i++)
        HI_MPI_VI_DisableChn(i);

    if( ch % 2 == 0 )
        HI_MPI_VI_DisableDev(port);

    SoC_portAttr.Interface = VI_MODE_BT656;
    SoC_portAttr.Multiplex = VI_WORK_MODE_2Multiplex;
    SoC_portAttr.ClkEdge   = VI_CLK_EDGE_DOUBLE;
    NC_HI_VI_Port_SetFormat( SoC_port, &SoC_portAttr );

    // Do Setup with the Format!
    FmtDef = pVideofmt[ch];
    pChFmt = (NC_VI_CH_FMT_S *)NC_HI_VI_Get_ChannelFormat( FmtDef );
    if(pChFmt)
    {
        NC_HI_VI_Channel_SetFormat( soc_ch, soc_ch, SoC_portAttr.Multiplex, pChFmt, 0 );
    }
}


HI_S32 SOC_HDMI_Output_Color_Set( NC_HI_HDMI_ATTR_S pParam )
{
	HI_S32 s32Ret = 0;
    VO_HDMI_PARAM_S  stHdmiPara_Luma;
    int ii = 0;

    for(ii=0; ii<32; ii++)
    {
        s32Ret = HI_MPI_VO_GetHdmiParam(0, &stHdmiPara_Luma);
        if(s32Ret == HI_SUCCESS)
        {
            stHdmiPara_Luma.stCSC.u32Luma       = pParam.u32Luma;
            stHdmiPara_Luma.stCSC.u32Hue        = pParam.u32Hue;
            stHdmiPara_Luma.stCSC.u32Saturation = pParam.u32Saturation;
            stHdmiPara_Luma.stCSC.u32Contrast   = pParam.u32Contrast;
            stHdmiPara_Luma.stCSC.enCscMatrix   = 0;
            s32Ret = HI_MPI_VO_SetHdmiParam( 0, &stHdmiPara_Luma);
            if(s32Ret != HI_SUCCESS)
        		printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, ii, s32Ret);

        }
        else
    		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, ii, s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 SOC_HDMI_Output_Color_Get( NC_HI_HDMI_ATTR_S *pParam )
{
	HI_S32 s32Ret = 0;
	VPSS_GRP VpssGrp = 0;
    VO_HDMI_PARAM_S  stHdmiPara_Luma;

    s32Ret = HI_MPI_VO_GetHdmiParam( VpssGrp, &stHdmiPara_Luma);
    if(s32Ret == HI_SUCCESS)
    {
        pParam->u32Luma       = stHdmiPara_Luma.stCSC.u32Luma;
        pParam->u32Hue        = stHdmiPara_Luma.stCSC.u32Hue;
        pParam->u32Saturation = stHdmiPara_Luma.stCSC.u32Saturation;
        pParam->u32Contrast   = stHdmiPara_Luma.stCSC.u32Contrast;
    }
    else
    {
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 SOC_HDMI_Output_SelGRP_Color_Set( int vo_dev, NC_HI_HDMI_ATTR_S *pParam )
{
	HI_S32 s32Ret    = 0;
	VPSS_GRP VpssGrp = 0;
    VO_HDMI_PARAM_S  stHdmiPara_Luma;

    VpssGrp = 0;

	s32Ret = HI_MPI_VO_GetHdmiParam( VpssGrp, &stHdmiPara_Luma);

	if(s32Ret == HI_SUCCESS)
	{
		stHdmiPara_Luma.stCSC.u32Luma       = pParam->u32Luma;
		stHdmiPara_Luma.stCSC.u32Hue        = pParam->u32Hue;
		stHdmiPara_Luma.stCSC.u32Saturation = pParam->u32Saturation;
		stHdmiPara_Luma.stCSC.u32Contrast   = pParam->u32Contrast;
		stHdmiPara_Luma.stCSC.enCscMatrix   = 0;
		HI_MPI_VO_SetHdmiParam( 0, &stHdmiPara_Luma);
	}
	else
		printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);


    return HI_SUCCESS;
}

HI_S32 SOC_HDMI_Output_SelGRP_Color_Get( int vo_dev, NC_HI_HDMI_ATTR_S *pParam )
{
	HI_S32 s32Ret = 0;
	VPSS_GRP VpssGrp = 0;
    VO_HDMI_PARAM_S  stHdmiPara_Luma;

    VpssGrp = 0;

    s32Ret = HI_MPI_VO_GetHdmiParam(VpssGrp, &stHdmiPara_Luma);
    if(s32Ret == HI_SUCCESS)
    {
        pParam->u32Luma       = stHdmiPara_Luma.stCSC.u32Luma;
        pParam->u32Hue        = stHdmiPara_Luma.stCSC.u32Hue;
        pParam->u32Saturation = stHdmiPara_Luma.stCSC.u32Saturation;
        pParam->u32Contrast   = stHdmiPara_Luma.stCSC.u32Contrast;
    }
    else
    {
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SetGrp_Param_Set( VPSS_GRP_PARAM_S *pParam )
{
	VPSS_GRP          VpssGrp;
	VPSS_GRP_PARAM_S  stVpssParam;
	int ii        = 0;
	HI_S32 s32Ret = 0;

	for(ii=0; ii<32; ii++)
	{
		s32Ret = HI_MPI_VPSS_GetGrpParam( ii, &stVpssParam );
		if( s32Ret == HI_SUCCESS )
		{
			stVpssParam.u32Contrast         = pParam->u32Contrast;
			stVpssParam.u32DieStrength      = pParam->u32DieStrength;
			stVpssParam.u32IeStrength       = pParam->u32IeStrength;
			stVpssParam.u32SfStrength       = pParam->u32SfStrength;
			stVpssParam.u32TfStrength       = pParam->u32TfStrength;
			stVpssParam.u32CfStrength       = pParam->u32CfStrength;
			stVpssParam.u32CTfStrength      = pParam->u32CTfStrength;
			stVpssParam.u32CvbsStrength     = pParam->u32CvbsStrength;
			stVpssParam.u32DeMotionBlurring = pParam->u32DeMotionBlurring;

			s32Ret = HI_MPI_VPSS_SetGrpParam(ii, &stVpssParam);
			if(s32Ret != HI_SUCCESS)
			{
				printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, ii, s32Ret);
			}
#ifdef __DEBUG_MSG
			printf("========== VpssGrp: %d ==========\n");
			printf("%d::u32Contrast::%d\n", ii, stVpssParam.u32Contrast);
			printf("%d::u32DieStrength::%d\n",  ii, stVpssParam.u32DieStrength);
			printf("%d::u32IeStrength::%d\n",   ii, stVpssParam.u32IeStrength);
			printf("%d::u32SfStrength::%d\n",  ii, stVpssParam.u32SfStrength);
			printf("%d::u32CfStrength::%d\n",  ii,  stVpssParam.u32CfStrength);
			printf("%d::u32CTfStrength::%d\n", ii,  stVpssParam.u32CTfStrength);
			printf("%d::u32CvbsStrength::%d\n", ii,   stVpssParam.u32CvbsStrength);
			printf("%d::u32DeMotionBlurring::%d\n", ii,  stVpssParam.u32DeMotionBlurring);
#endif
		}
		else
			printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, ii, s32Ret);
	}

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Param_Set( int Grp, VPSS_GRP_PARAM_S *pParam )
{
	HI_S32      s32Ret  = 0;
	VPSS_GRP    VpssGrp = 0;
	VPSS_GRP_PARAM_S  stVpssParam;

	VpssGrp = Grp;

	s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
	if( s32Ret == HI_SUCCESS )
	{
		stVpssParam.u32Contrast         = pParam->u32Contrast;
		stVpssParam.u32DieStrength      = pParam->u32DieStrength;
		stVpssParam.u32IeStrength       = pParam->u32IeStrength;
		stVpssParam.u32SfStrength       = pParam->u32SfStrength;
		stVpssParam.u32TfStrength       = pParam->u32TfStrength;
		stVpssParam.u32CfStrength       = pParam->u32CfStrength;
		stVpssParam.u32CTfStrength      = pParam->u32CTfStrength;
		stVpssParam.u32CvbsStrength     = pParam->u32CvbsStrength;
		stVpssParam.u32DeMotionBlurring = pParam->u32DeMotionBlurring;
		s32Ret = HI_MPI_VPSS_SetGrpParam(s32Ret, &stVpssParam);
		if(s32Ret != HI_SUCCESS)
		{
			printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);
		}
	}
	else
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

    return HI_SUCCESS;
}
HI_S32 SOC_VPSS_SetGrp_Param_Get( VPSS_GRP_PARAM_S *pParam )
{
	HI_S32     s32Ret = 0;
	VPSS_GRP  VpssGrp = 0;
	VPSS_GRP_PARAM_S  stVpssParam;

	s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
	if( s32Ret == HI_SUCCESS )
	{
		pParam->u32Contrast         = stVpssParam.u32Contrast;
		pParam->u32DieStrength      = stVpssParam.u32DieStrength;
		pParam->u32IeStrength       = stVpssParam.u32IeStrength;
		pParam->u32SfStrength       = stVpssParam.u32SfStrength;
		pParam->u32TfStrength       = stVpssParam.u32TfStrength;
		pParam->u32CfStrength       = stVpssParam.u32CfStrength;
		pParam->u32CTfStrength      = stVpssParam.u32CTfStrength;
		pParam->u32CvbsStrength     = stVpssParam.u32CvbsStrength;
		pParam->u32DeMotionBlurring = stVpssParam.u32DeMotionBlurring;
	}
	else
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Param_Get( int Grp, VPSS_GRP_PARAM_S *pParam )
{
	VPSS_GRP          VpssGrp;
	VPSS_GRP_PARAM_S  stVpssParam;
	HI_S32 s32Ret = 0;

	VpssGrp = Grp;

	s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
	if( s32Ret == HI_SUCCESS )
	{
		pParam->u32Contrast         = stVpssParam.u32Contrast;
		pParam->u32DieStrength      = stVpssParam.u32DieStrength;
		pParam->u32IeStrength       = stVpssParam.u32IeStrength;
		pParam->u32SfStrength       = stVpssParam.u32SfStrength;
		pParam->u32TfStrength       = stVpssParam.u32TfStrength;
		pParam->u32CfStrength       = stVpssParam.u32CfStrength;
		pParam->u32CTfStrength      = stVpssParam.u32CTfStrength;
		pParam->u32CvbsStrength     = stVpssParam.u32CvbsStrength;
		pParam->u32DeMotionBlurring = stVpssParam.u32DeMotionBlurring;
#ifdef __DEBUG_MSG
		printf("[vpss:%d]u32Contrast::%d\n", VpssGrp, stVpssParam.u32Contrast);
		printf("[vpss:%d]u32Die::%d\n", VpssGrp, stVpssParam.u32DieStrength);
		printf("[vpss:%d]u32Ie::%d\n", VpssGrp, stVpssParam.u32IeStrength);
		printf("[vpss:%d]u32Sf::%d\n", VpssGrp, stVpssParam.u32SfStrength);
		printf("[vpss:%d]u32Tf::%d\n", VpssGrp, stVpssParam.u32TfStrength);
		printf("[vpss:%d]u32Cf::%d\n", VpssGrp, stVpssParam.u32CfStrength);
		printf("[vpss:%d]u32CTf::%d\n", VpssGrp, stVpssParam.u32CTfStrength);
		printf("[vpss:%d]u32Cvbs::%d\n", VpssGrp, stVpssParam.u32CvbsStrength);
		printf("[vpss:%d]u32DeMotionBlurring::%d\n", VpssGrp, stVpssParam.u32DeMotionBlurring);
#endif
	}
	else
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Attr_Get( int grp, VPSS_GRP_ATTR_S *pParam )
{
	HI_S32    s32Ret = 0;
	VPSS_GRP VpssGrp = 0;
	VPSS_GRP_ATTR_S stVpssGrpAttr;

	VpssGrp = grp;

	s32Ret = HI_MPI_VPSS_GetGrpAttr (VpssGrp, &stVpssGrpAttr);
	if( s32Ret == HI_SUCCESS )
	{
		pParam->bDciEn  = stVpssGrpAttr.bDciEn;
		pParam->bNrEn   = stVpssGrpAttr.bNrEn;
		pParam->bIeEn   = stVpssGrpAttr.bIeEn;
		pParam->bHistEn = stVpssGrpAttr.bHistEn;
		pParam->bEsEn   = stVpssGrpAttr.bEsEn;
	}
	else
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Attr_Set( int grp, VPSS_GRP_ATTR_S *pParam )
{
	HI_S32    s32Ret = 0;
	VPSS_GRP VpssGrp = 0;
	VPSS_GRP_ATTR_S stVpssGrpAttr;

	VpssGrp = grp;

	s32Ret = HI_MPI_VPSS_GetGrpAttr (VpssGrp, &stVpssGrpAttr);
	if( s32Ret == HI_SUCCESS )
	{
		stVpssGrpAttr.bDciEn  = pParam->bDciEn;
		stVpssGrpAttr.bNrEn   = pParam->bNrEn;
		stVpssGrpAttr.bIeEn   = pParam->bIeEn;
		stVpssGrpAttr.bHistEn = pParam->bHistEn;
		stVpssGrpAttr.bEsEn   = pParam->bEsEn;
		s32Ret = HI_MPI_VPSS_SetGrpAttr(VpssGrp, &stVpssGrpAttr);
		if( s32Ret != HI_SUCCESS )
			printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);
	}
	else
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

    return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Adv_Param_Get( int grp, VPSS_NR_ADVANCED_PARAM_S *pParam )
{
	HI_S32	s32Ret = 0;
	VPSS_NR_ADVANCED_PARAM_S  VPSS_Adv_Param;
	VPSS_GRP VpssGrp = 0;
	
	VpssGrp = grp;

	s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam(VpssGrp, &VPSS_Adv_Param);

	if( s32Ret == HI_SUCCESS )
	{
		pParam->u32Mdz = VPSS_Adv_Param.u32Mdz;
		pParam->u32Edz = VPSS_Adv_Param.u32Edz;
		pParam->u32HTfRelaStrength = VPSS_Adv_Param.u32HTfRelaStrength;
		pParam->u32WTfRelaStrength = VPSS_Adv_Param.u32WTfRelaStrength;
	}
	else
	{
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

		return -1;
	}
	return HI_SUCCESS;
}

HI_S32 SOC_VPSS_SelGRP_Adv_Param_Set( int grp, VPSS_NR_ADVANCED_PARAM_S *pParam )
{
	HI_S32	s32Ret = 0;
	VPSS_NR_ADVANCED_PARAM_S  VPSS_Adv_Param;
	VPSS_GRP VpssGrp = 0;
	
	VpssGrp = grp;

	s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam(VpssGrp, &VPSS_Adv_Param);

	if( s32Ret == HI_SUCCESS )
	{
		VPSS_Adv_Param.u32Mdz = pParam->u32Mdz;
		VPSS_Adv_Param.u32Edz = pParam->u32Edz;
		VPSS_Adv_Param.u32HTfRelaStrength = pParam->u32HTfRelaStrength;
		VPSS_Adv_Param.u32WTfRelaStrength = pParam->u32WTfRelaStrength;

		s32Ret = HI_MPI_VPSS_SetGrpAdvancedParam(VpssGrp, &VPSS_Adv_Param);
	}
	else
	{
		printf("[%s::%d]GRP%d Get Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

		return -1;
	}
	return HI_SUCCESS;

}

void SOC_TestForTVI_SET( int ch, int EqStage, int Fmt )
{
	HI_S32	s32Ret = 0;
	VPSS_GRP_PARAM_S  stVpssParam;
	VPSS_NR_ADVANCED_PARAM_S stVpssAdvParam;
	VPSS_GRP VpssGrp = ch;

	printf("%s::Channel(%d) FMT(%d) EqStage(%d) \n", __func__, ch, Fmt, EqStage);

	if( Fmt == TVI_HD_B_25P_EX || Fmt == TVI_HD_B_30P_EX )
	{
		if( EqStage >= 6 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 32;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}

		}
		else if( 0 < EqStage || EqStage < 6  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == TVI_FHD_25P || Fmt == TVI_FHD_30P || Fmt == TVI_HD_25P_EX || Fmt == TVI_HD_30P_EX )
	{
		if( EqStage >= 4 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 4;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 32;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 3  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == TVI_4M_25P || Fmt == TVI_4M_30P || Fmt == TVI_5M_12_5P || Fmt == TVI_5M_20P )
	{
		if( EqStage >= 3 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 4;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 32;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 2  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else
		printf("[%s]Unknown Format!!!\n", __func__);

}


void SOC_TestForCVI_SET( int ch, int EqStage, int Fmt )
{
	HI_S32	s32Ret = 0;
	VPSS_GRP_PARAM_S  stVpssParam;
	VPSS_NR_ADVANCED_PARAM_S stVpssAdvParam;
	VPSS_GRP VpssGrp = ch;

	printf("%s::Channel(%d) FMT(%d) EqStage(%d) \n", __func__, ch, Fmt, EqStage);

	if( Fmt == CVI_HD_25P_EX || Fmt == CVI_HD_30P_EX )
	{
		if( EqStage >= 6 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}

		}
		else if( 0 < EqStage || EqStage < 6  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == CVI_FHD_25P || Fmt == CVI_FHD_30P )
	{
		if( EqStage >= 5 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 4  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == CVI_4M_25P || Fmt == CVI_4M_30P )
	{
		if( EqStage >= 2 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 1  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else
		printf("[%s]Unknown Format!!!\n", __func__);

}


void SOC_TestForAHD_SET( int ch, int EqStage, int Fmt )
{
	HI_S32	s32Ret = 0;
	VPSS_GRP_PARAM_S  stVpssParam;
	VPSS_NR_ADVANCED_PARAM_S stVpssAdvParam;
	VPSS_GRP VpssGrp = ch;

	printf("%s::Channel(%d) FMT(%d) EqStage(%d) \n", __func__, ch, Fmt, EqStage);

	if( Fmt == AHD20_720P_25P_EX_Btype || Fmt == AHD20_720P_30P_EX_Btype )
	{
		if( EqStage >= 6 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}

		}
		else if( 0 < EqStage || EqStage < 6  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == AHD20_1080P_25P || Fmt == AHD20_1080P_30P )
	{
		if( EqStage >= 5 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 4  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
		{
			printf("[%s]No Change SOC Setting Value!!!\n", __func__);
		}
	}
	else if( Fmt == AHD30_4M_25P || Fmt == AHD30_4M_30P )
	{
		if( EqStage >= 3 )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 63;
				stVpssParam.u32CTfStrength  = 24;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);

			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else if( 0 <= EqStage || EqStage <= 2  )
		{
			s32Ret = HI_MPI_VPSS_GetGrpParam( VpssGrp, &stVpssParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssParam.u32IeStrength   = 24;
				stVpssParam.u32SfStrength   = 15;
				stVpssParam.u32TfStrength	= 16;
				stVpssParam.u32CfStrength	= 12;
				stVpssParam.u32CTfStrength  = 16;
				stVpssParam.u32CvbsStrength = 24;

				s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
				if( s32Ret != HI_SUCCESS )
					printf("[%s::%d]GRP%d Set Error!! >>> Error::%x\n", __func__, __LINE__, VpssGrp, s32Ret);
			}

			s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			if( s32Ret == HI_SUCCESS )
			{
				stVpssAdvParam.u32Mdz	= 6;
				stVpssAdvParam.u32Edz	= 4;
				stVpssAdvParam.u32HTfRelaStrength	= 52;
				stVpssAdvParam.u32WTfRelaStrength	= 50;
				HI_MPI_VPSS_SetGrpAdvancedParam( VpssGrp, &stVpssAdvParam );
			}
		}
		else
			printf("[%s]Unknown Format!!!\n", __func__);
	}

}

void SOC_Display_Mux_SetDiv( int w, int h  )
{
	DBG_INFO("w,h : %d,%d\n",w,h);

	NC_HI_MUX_ATTR_S muxStr={0,};
	muxStr.w = w;
	muxStr.h = h;
	NC_HI_VO_MuxAttrSet( &muxStr );
}


void SOC_Audio_Init(void)
{
	AUDIO_SAMPLE_RATE_E ain_sample_rate = 8000;

	NC_HI_AUDIO_INIT(0, ain_sample_rate);
}

void SOC_Audio_Init_NewCh(int ch, int sample_rate)
{
	AUDIO_SAMPLE_RATE_E ain_sample_rate;

	NC_HI_AUDIO_DESTROY();

	switch(sample_rate)
	{
		case SampleRate_8K :
			ain_sample_rate = 8000;    /* 8K samplerate*/
			break;
		case SampleRate_16K :
			ain_sample_rate= 16000;   /* 16K samplerate*/
			break;
		case SampleRate_32K :
			ain_sample_rate = 32000;   /* 32K samplerate*/
			break;
		case SampleRate_44_1K :
			ain_sample_rate = 44100;   /* 44.1K samplerate*/
			break;
		default :
			break;
	}

	NC_HI_AUDIO_INIT((AI_CHN)ch, ain_sample_rate);
}

void SOC_Audio_Destroy(void)
{
	NC_HI_AUDIO_DESTROY();
}

int SOC_SAL_Init( VDEC_DEV_INFORM_S* pDevInfo )
{
	int Ch;
	int DevNum;
	NC_HI_MUX_ATTR_S 	muxStr={0,};

	NC_VIVO_CH_FORMATDEF Videofmt[16] = {AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P
										,AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P
										,AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P
										,AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P, AHD20_1080P_25P};

	for( DevNum = 0; DevNum < pDevInfo->Total_Chip_Cnt; DevNum++ )
    {
		if( pDevInfo->chip_id[DevNum] == 0xA0/*RAPTOR3_2PORT_R0_ID*/ )
		{
			for(Ch = DevNum * 4; Ch < DevNum*4 + 4; Ch++)
            SOC_SAL_Video_2MuxSetFormat(Ch, &Videofmt[Ch]);

        muxStr.w = 4;
        muxStr.h = 4;
        NC_HI_VO_MuxAttrSet( &muxStr );
    }
    else
    {
			for(Ch = DevNum * 4; Ch < DevNum*4 + 4; Ch++)
            SOC_SAL_Video_SetFormat(Ch, &Videofmt[Ch]);
    }
    }
}

