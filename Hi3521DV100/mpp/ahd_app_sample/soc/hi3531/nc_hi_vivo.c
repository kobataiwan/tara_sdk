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


#include "nc_hi_vivo.h"
#include "nc_hi_sys.h"

#include <_debug_macro.h>

static VO_DEV VoDev_main = SAMPLE_VO_DEV_DHD0;
static VO_LAYER VoLayer_main = 0;

#ifdef HI_CHIP_HI3531D

typedef struct hiHDMI_ARGS_S
{
    HI_HDMI_ID_E  enHdmi;
}HDMI_ARGS_S;

#define CHECK_POINTER_NO_RET(p) \
do{                      \
    if(HI_NULL == p){\
        printf("The pointer is null\n");       \
        return; \
    } \
}while(0)


#define CHECK_RET_SUCCESS_NO_RET(val) \
do{                      \
    if(HI_SUCCESS != val){\
        printf("return value is not HI_SUCCESS, 0x%x!\n", val);       \
        return ; \
    } \
}while(0)

#endif

#define DEF_VI_CH_MAX (32)
#define DEF_VO_CH_MAX (32)

//#ifdef SUPPORT_3520D_6158C
//#define GRAPHICS_LAYER_HC0 2
//#else
#define GRAPHICS_LAYER_HC0 3
//#endif
#define GRAPHICS_LAYER_G0  0
#define GRAPHICS_LAYER_G1  1

static int _VO_Init_f=0;

static int _FB_Width;
static int _FB_Height;
static void *_FB_pBitmapBuffer;

typedef struct MuxRectStrc
{
	int x,y,w,h;
	int enable;
} MuxRectStrc;

static MuxRectStrc CurVoMuxRect[ DEF_VO_CH_MAX ];
static NC_HI_MUX_ATTR_S CurMuxAttrs;

static inline int _CollideRectRect( int l1, int t1, int r1, int b1, int x, int y )
{
	int l2 = x;
	int t2 = y;
	int r2 = x+1;
	int b2 = y+1;;

	if(r1 < l2 || l1 > r2 || t1 > b2 || b1 < t2)
	{
        return 0;
    }

	return 1;
}

int NC_HI_VO_MuxCollideChCheck( int x, int y )
{
	int u32WndNum = CurMuxAttrs.w * CurMuxAttrs.h;

	int ii;
	for(ii=0;ii<u32WndNum;ii++)
	{
		MuxRectStrc *pMuxRect = &CurVoMuxRect[ii];
		if(pMuxRect->enable)
		{
			if( _CollideRectRect( pMuxRect->x,pMuxRect->y,
					pMuxRect->x+pMuxRect->w,
					pMuxRect->y+pMuxRect->h,
					x,y) )
			{
				DBG_INFO("CH : %d\n",ii);
				return ii;
			}
		}
	}
	return -1;
}

int NC_HI_VO_MuxRectGet( int ch, MuxRectStrc *pMuxRectDst )
{
	if(pMuxRectDst)
	{
		memcpy( pMuxRectDst, &CurVoMuxRect[ch], sizeof( MuxRectStrc ) );
	}
	return 0;
}

int NC_HI_VO_MuxAttrGet( NC_HI_MUX_ATTR_S *pMuxAttrDst )
{
	if( pMuxAttrDst )
	{
		memcpy( pMuxAttrDst, &CurMuxAttrs, sizeof( NC_HI_MUX_ATTR_S ) );
	}
	return 0;
}

int NC_HI_VO_MuxAttrSet( NC_HI_MUX_ATTR_S *pMuxAttr )
{
	HI_S32 i;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32WndNum = 0;
	HI_U32 u32Square = 0;
	HI_U32 u32Width = 0;
	HI_U32 u32Height = 0;
	VO_CHN_ATTR_S stChnAttr;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;

	VO_LAYER VoLayer = VoLayer_main;

	if(!pMuxAttr)
	{
		DBG_ERR("pMuxAttr Null\n");
	}

	memcpy( &CurMuxAttrs, pMuxAttr, sizeof( NC_HI_MUX_ATTR_S ) );

	if(CurMuxAttrs.w<=0) CurMuxAttrs.w=1;
	if(CurMuxAttrs.h<=0) CurMuxAttrs.h=1;

	int nCol = CurMuxAttrs.w;
	int nRow = CurMuxAttrs.h;

	if(CurMuxAttrs.single_en || CurMuxAttrs.single_mid_screen)	// fix 170209 13:42
	{
		nCol = 1;
		nRow = 1;
	}

	u32WndNum = nCol * nRow;
	//u32Square = u32WndNum 3;

	s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	u32Width = stLayerAttr.stImageSize.u32Width;
	u32Height = stLayerAttr.stImageSize.u32Height;

	HI_MPI_VO_SetAttrBegin(VoLayer);

	for (i=0; i< DEF_VO_CH_MAX ; i++)
	{
		CurVoMuxRect[i].enable = 0;
		int vo_ch = i;
		HI_MPI_VO_DisableChn(VoLayer, vo_ch);
	}

	for (i=0; i<u32WndNum; i++)
	{
		int vo_ch = i;
		int single_en = CurMuxAttrs.single_en;
		if(single_en)
		{
			vo_ch = CurMuxAttrs.single_ch;
		}

		int org_w;
		int org_h;
		int vo_ch_vpssrgp = vo_ch;

#ifdef VPSS_DISABLE
		
		org_w = 1920;
		org_h = 1080;

#else

		VPSS_GRP_ATTR_S VpssGrpAttr;

		s32Ret = HI_MPI_VPSS_GetGrpAttr(vo_ch_vpssrgp, &VpssGrpAttr);
#if 0

		if (s32Ret != HI_SUCCESS)
		{
			single_en = 0;
		}
#endif
		org_w = VpssGrpAttr.u32MaxW;
		org_h = VpssGrpAttr.u32MaxH;

#endif

		if(CurMuxAttrs.single_ch == vo_ch)
		{
			if(u32Width > org_w && u32Height > org_h)
				CurMuxAttrs.single_orgsize_possible = 1;
			else
				CurMuxAttrs.single_orgsize_possible = 0;
		}

		if(single_en && CurMuxAttrs.single_orgsize_en && CurMuxAttrs.single_orgsize_possible)
		{
			stChnAttr.stRect.s32X       = ALIGN_BACK( u32Width/2 - org_w/2, 2);
			stChnAttr.stRect.s32Y       = ALIGN_BACK( u32Height/2 - org_h/2, 2);
			stChnAttr.stRect.u32Width   = ALIGN_BACK(org_w, 2);
			stChnAttr.stRect.u32Height  = ALIGN_BACK(org_h, 2);
			stChnAttr.u32Priority       = 0;
			stChnAttr.bDeflicker        = HI_FALSE;
		}
		else if(CurMuxAttrs.single_mid_screen)			// add 170209 13:42
		{
				stChnAttr.stRect.s32X       = u32Width/4;
				stChnAttr.stRect.s32Y       = u32Height/4;
				stChnAttr.stRect.u32Width   = u32Width/2;
				stChnAttr.stRect.u32Height  = u32Height/2;
				stChnAttr.u32Priority       = 0;
				stChnAttr.bDeflicker        = HI_FALSE;

				CurMuxAttrs.single_mid_screen = 0;
				CurMuxAttrs.single_mid_screen_on = 0;
		}
		else
		{
			stChnAttr.stRect.s32X       = ALIGN_BACK((u32Width/nCol) * (i%nCol), 2);
			stChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height/nRow) * (i/(nCol)), 2);
			stChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width/nCol, 2);
			stChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height/nRow, 2);
			stChnAttr.u32Priority       = 0;
			stChnAttr.bDeflicker        = HI_FALSE;

			if(CurMuxAttrs.single_orgsize_possible)
				CurMuxAttrs.single_mid_screen_on = 0;
			else
				CurMuxAttrs.single_mid_screen_on = 1;
		}

		CurVoMuxRect[vo_ch].x = stChnAttr.stRect.s32X;
		CurVoMuxRect[vo_ch].y = stChnAttr.stRect.s32Y;
		CurVoMuxRect[vo_ch].w = stChnAttr.stRect.u32Width;
		CurVoMuxRect[vo_ch].h = stChnAttr.stRect.u32Height;
		CurVoMuxRect[vo_ch].enable = 1;

		s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, vo_ch, &stChnAttr);
		if (s32Ret != HI_SUCCESS)
		{
			DBG_ERR("failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}

		s32Ret = HI_MPI_VO_EnableChn(VoLayer, vo_ch);
		if (s32Ret != HI_SUCCESS)
		{
			DBG_ERR("failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
	}

	s32Ret = HI_MPI_VO_SetAttrEnd(VoLayer);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}



	return 0;
}

int NC_HI_VO_Init( VO_PUB_ATTR_S *pPubAttr )
{
	HI_S32 s32Ret = HI_SUCCESS;
    pthread_t phifb0 = -1;
	pthread_t phifb1 = -1;

    HI_U32 u32PicWidth;
    HI_U32 u32PicHeight;
    SIZE_S  stSize;

	VO_LAYER VoLayer = 0;
    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    HI_U32 u32VoFrmRate;

    VB_CONF_S       stVbConf;
	HI_U32 u32BlkSize;

	memcpy( &stPubAttr, pPubAttr, sizeof( VO_PUB_ATTR_S ));

	// Get Resolution First!!
	memset(&stSize,0,sizeof(SIZE_S));
	s32Ret = NC_HI_VO_GetWH(stPubAttr.enIntfSync,&stSize.u32Width,\
		&stSize.u32Height,&u32VoFrmRate);
	if (HI_SUCCESS != s32Ret)
	{
		DBG_ERR("get vo wh failed with %d!\n", s32Ret);
		goto SAMPLE_HIFB_NoneBufMode_0;
	}

	u32PicWidth = stSize.u32Width;
	u32PicHeight = stSize.u32Height;
#ifdef SUPPORT_3520D_6158C
	if((stPubAttr.enIntfSync == VO_OUTPUT_3840x2160_30) || (stPubAttr.enIntfSync == VO_OUTPUT_2560x1440_30))
	{
		_FB_Width = u32PicWidth / 2;
		_FB_Height = u32PicHeight / 2;
	}
	else
	{
		_FB_Width = u32PicWidth;
		_FB_Height = u32PicHeight;
	}
#else

    _FB_Width = u32PicWidth;
	_FB_Height = u32PicHeight;

#endif
	DBG_INFO("VO format Size : w %d x  h %d\n",stSize.u32Width,stSize.u32Height);

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    //u32BlkSize = CEILING_2_POWER(u32PicWidth,SAMPLE_SYS_ALIGN_WIDTH)\
    //    * CEILING_2_POWER(u32PicHeight,SAMPLE_SYS_ALIGN_WIDTH) *2;
#ifdef SUPPORT_3520D_6158C
    u32BlkSize = CEILING_2_POWER(2048,SAMPLE_SYS_ALIGN_WIDTH)\
        * CEILING_2_POWER(1536,SAMPLE_SYS_ALIGN_WIDTH) *2;

    stVbConf.u32MaxPoolCnt = 64;

    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;

    stVbConf.astCommPool[0].u32BlkCnt =  32;

#else
    u32BlkSize = CEILING_2_POWER(3840,SAMPLE_SYS_ALIGN_WIDTH)\
        * CEILING_2_POWER(2160,SAMPLE_SYS_ALIGN_WIDTH) *2;

    stVbConf.u32MaxPoolCnt = 128;

    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;

    stVbConf.astCommPool[0].u32BlkCnt =  64;
//   stVbConf.astCommPool[0].u32BlkCnt =  128;
	memset(stVbConf.astCommPool[0].acMmzName,0,sizeof(stVbConf.astCommPool[0].acMmzName));

#endif
    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = NC_HI_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    /******************************************
     step 3:  start vo hd0.
    *****************************************/
    s32Ret = HI_MPI_VO_UnBindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev_main);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("UnBindGraphicLayer(%d) failed with %x!\n", GRAPHICS_LAYER_HC0, s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    s32Ret = HI_MPI_VO_BindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev_main);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("BindGraphicLayer failed with %x!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    stLayerAttr.bClusterMode = HI_FALSE;
	stLayerAttr.bDoubleFrame = HI_FALSE;
	stLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;

    s32Ret = NC_HI_VO_GetWH(stPubAttr.enIntfSync,&stSize.u32Width,\
        &stSize.u32Height,&u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("get vo wh failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }
    memcpy(&stLayerAttr.stImageSize,&stSize,sizeof(stSize));

	s32Ret = NC_HI_VO_StartDev(VoDev_main, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("start vo dev failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
	}

    stLayerAttr.u32DispFrmRt = u32VoFrmRate ;
	stLayerAttr.stDispRect.s32X = 0;
	stLayerAttr.stDispRect.s32Y = 0;
	stLayerAttr.stDispRect.u32Width = stSize.u32Width;
	stLayerAttr.stDispRect.u32Height = stSize.u32Height;

    s32Ret = NC_HI_VO_StartLayer(VoLayer, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("start vo layer failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_1;
	}

    VoLayer_main = VoLayer;

    if (stPubAttr.enIntfType & VO_INTF_HDMI)
    {
        s32Ret = NC_HI_VO_HdmiStart(stPubAttr.enIntfSync);
        if (HI_SUCCESS != s32Ret)
        {
        	DBG_ERR("start HDMI failed with %d!\n", s32Ret);
            goto SAMPLE_HIFB_NoneBufMode_2;
    	}
    }

    DBG_INFO("VO Initialized\n");

    _VO_Init_f = 1;

    return 0;

SAMPLE_HIFB_NoneBufMode_2:
	NC_HI_VO_StopLayer(VoLayer);
SAMPLE_HIFB_NoneBufMode_1:
	NC_HI_VO_StopDev(VoDev_main);
SAMPLE_HIFB_NoneBufMode_0:
	NC_HI_SYS_Exit();

	return s32Ret;
}

int NC_HI_FB_Get_Width(void)
{
	return _FB_Width;
}

int NC_HI_FB_Get_Height(void)
{
	return _FB_Height;
}

void *NC_HI_FB_Get_BitmapBuffer(void)
{
	return _FB_pBitmapBuffer;
}

HI_S32 NC_HI_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32 *pu32W,HI_U32 *pu32H, HI_U32 *pu32Frm)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL       :  *pu32W = 720;  *pu32H = 576;  *pu32Frm = 25; break;
        case VO_OUTPUT_NTSC      :  *pu32W = 720;  *pu32H = 480;  *pu32Frm = 30; break;
        case VO_OUTPUT_576P50    :  *pu32W = 720;  *pu32H = 576;  *pu32Frm = 50; break;
        case VO_OUTPUT_480P60    :  *pu32W = 720;  *pu32H = 480;  *pu32Frm = 60; break;
        case VO_OUTPUT_800x600_60:  *pu32W = 800;  *pu32H = 600;  *pu32Frm = 60; break;
        case VO_OUTPUT_720P50    :  *pu32W = 1280; *pu32H = 720;  *pu32Frm = 50; break;
        case VO_OUTPUT_720P60    :  *pu32W = 1280; *pu32H = 720;  *pu32Frm = 60; break;
        case VO_OUTPUT_1080I50   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 50; break;
        case VO_OUTPUT_1080I60   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 60; break;
        case VO_OUTPUT_1080P24   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 24; break;
        case VO_OUTPUT_1080P25   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 25; break;
        case VO_OUTPUT_1080P30   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 30; break;
        case VO_OUTPUT_1080P50   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 50; break;
        case VO_OUTPUT_1080P60   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 60; break;
        case VO_OUTPUT_1024x768_60:  *pu32W = 1024; *pu32H = 768;  *pu32Frm = 60; break;
        case VO_OUTPUT_1280x1024_60: *pu32W = 1280; *pu32H = 1024; *pu32Frm = 60; break;
        case VO_OUTPUT_1366x768_60:  *pu32W = 1366; *pu32H = 768;  *pu32Frm = 60; break;
        case VO_OUTPUT_1440x900_60:  *pu32W = 1440; *pu32H = 900;  *pu32Frm = 60; break;
        case VO_OUTPUT_1280x800_60:  *pu32W = 1280; *pu32H = 800;  *pu32Frm = 60; break;
        case VO_OUTPUT_1600x1200_60: *pu32W = 1600; *pu32H = 1200; *pu32Frm = 60; break;
        case VO_OUTPUT_1680x1050_60: *pu32W = 1680; *pu32H = 1050; *pu32Frm = 60; break;
        case VO_OUTPUT_1920x1200_60: *pu32W = 1920; *pu32H = 1200; *pu32Frm = 60; break;
        case VO_OUTPUT_2560x1440_30: *pu32W = 2560; *pu32H = 1440; *pu32Frm = 30; break;
        case VO_OUTPUT_3840x2160_30: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 30; break;
        case VO_OUTPUT_3840x2160_60: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 60; break;
        case VO_OUTPUT_USER    :     *pu32W = 720;  *pu32H = 576;  *pu32Frm = 25; break;
        default:
            DBG_ERR("vo enIntfSync not support!\n");
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 NC_HI_VO_MemConfig(VO_DEV VoDev, HI_CHAR *pcMmzName)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stMppChnVO;

    /* config vo dev */
    stMppChnVO.enModId  = HI_ID_VOU;
    stMppChnVO.s32DevId = VoDev;
    stMppChnVO.s32ChnId = 0;
    s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVO, pcMmzName);
    if (s32Ret)
    {
        DBG_INFO("HI_MPI_SYS_SetMemConf ERR !\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 NC_HI_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_SetPubAttr(VoDev, pstPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_Enable(VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 NC_HI_VO_StopDev(VO_DEV VoDev)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_Disable(VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 NC_HI_VO_StartLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 NC_HI_VO_StopLayer(VO_LAYER VoLayer)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 NC_HI_VO_StartChn(VO_LAYER VoLayer, VO_MODE_MUX enMode)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;
    HI_U32 u32Square = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    switch (enMode)
    {
        case VO_MODE_1MUX:
            u32WndNum = 1;
            u32Square = 1;
            break;
        case VO_MODE_4MUX:
            u32WndNum = 4;
            u32Square = 2;
            break;
        case VO_MODE_9MUX:
            u32WndNum = 9;
            u32Square = 3;
            break;
        case VO_MODE_16MUX:
            u32WndNum = 16;
            u32Square = 4;
            break;
        default:
            DBG_INFO("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    u32Width = stLayerAttr.stImageSize.u32Width;
    u32Height = stLayerAttr.stImageSize.u32Height;

    for (i=0; i<u32WndNum; i++)
    {
        stChnAttr.stRect.s32X       = ALIGN_BACK((u32Width/u32Square) * (i%u32Square), 2);
        stChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height/u32Square) * (i/u32Square), 2);
        stChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width/u32Square, 2);
        stChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height/u32Square, 2);
        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = HI_FALSE;

        s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n",\
                   __FUNCTION__,__LINE__,  s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_INFO("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

HI_S32 NC_HI_VO_StopChn(VO_LAYER VoLayer, VO_MODE_MUX enMode)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;

    switch (enMode)
    {
        case VO_MODE_1MUX:
        {
            u32WndNum = 1;
            break;
        }
        case VO_MODE_4MUX:
        {
            u32WndNum = 4;
            break;
        }
        case VO_MODE_9MUX:
        {
            u32WndNum = 9;
            break;
        }
        case VO_MODE_16MUX:
        {
            u32WndNum = 16;
            break;
        }

        default:
            DBG_INFO("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
    }

    for (i=0; i<u32WndNum; i++)
    {
        s32Ret = HI_MPI_VO_DisableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_INFO("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return s32Ret;
}

#ifdef HI_CHIP_HI3531D

HI_HDMI_CALLBACK_FUNC_S g_stCallbackFunc;
HDMI_ARGS_S      		g_stHdmiArgs;

static HI_VOID HDMI_HotPlug_Proc(HI_VOID *pPrivateData)
{
    HDMI_ARGS_S    stArgs;
    HI_S32         s32Ret = HI_FAILURE;
    HI_HDMI_ATTR_S stAttr;
    HI_HDMI_SINK_CAPABILITY_S stCaps;

    //printf("EVENT: HPD\n");

    CHECK_POINTER_NO_RET(pPrivateData);

    memset(&stAttr, 0, sizeof(HI_HDMI_ATTR_S));
    memset(&stArgs, 0, sizeof(HDMI_ARGS_S));
    memcpy(&stArgs, pPrivateData, sizeof(HDMI_ARGS_S));
    memset(&stCaps, 0, sizeof(HI_HDMI_SINK_CAPABILITY_S));


    s32Ret = HI_MPI_HDMI_GetSinkCapability(stArgs.enHdmi, &stCaps);
    if(s32Ret != HI_SUCCESS)
    {
        printf("get sink caps failed!\n");
    }
    else
    {
        printf("get sink caps success!\n");
    }

    s32Ret = HI_MPI_HDMI_GetAttr(stArgs.enHdmi, &stAttr);
    CHECK_RET_SUCCESS_NO_RET(s32Ret);

    s32Ret = HI_MPI_HDMI_SetAttr(stArgs.enHdmi, &stAttr);
    CHECK_RET_SUCCESS_NO_RET(s32Ret);

    HI_MPI_HDMI_Start(stArgs.enHdmi);

    return;
}

static HI_VOID HDMI_UnPlug_Proc(HI_VOID *pPrivateData)
{
    HDMI_ARGS_S  stArgs;

    //printf("EVENT: UN-HPD\n");

    CHECK_POINTER_NO_RET(pPrivateData);

    memset(&stArgs, 0, sizeof(HDMI_ARGS_S));
    memcpy(&stArgs, pPrivateData, sizeof(HDMI_ARGS_S));

    HI_MPI_HDMI_Stop(stArgs.enHdmi);

    return;
}

static HI_VOID HDMI_EventCallBack(HI_HDMI_EVENT_TYPE_E event, HI_VOID *pPrivateData)
{
    switch ( event )
    {
        case HI_HDMI_EVENT_HOTPLUG:
            HDMI_HotPlug_Proc(pPrivateData);
            break;
        case HI_HDMI_EVENT_NO_PLUG:
            HDMI_UnPlug_Proc(pPrivateData);
            break;
        default:
            break;
    }

    return;
}

#endif

HI_S32 NC_HI_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync)
{
    HI_HDMI_ATTR_S      stAttr;
    HI_HDMI_VIDEO_FMT_E enVideoFmt;

#ifdef HI_CHIP_HI3531D
        HDMI_ARGS_S      __stHdmiArgs;
	HI_HDMI_CALLBACK_FUNC_S __stCallbackFunc;
    
	NC_HI_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);

	HI_MPI_HDMI_Init();

	g_stHdmiArgs.enHdmi = HI_HDMI_ID_0;
    g_stCallbackFunc.pfnHdmiEventCallback = HDMI_EventCallBack;
  	g_stCallbackFunc.pPrivateData = &__stHdmiArgs;

        HI_MPI_HDMI_Open(HI_HDMI_ID_0);

	HI_MPI_HDMI_RegCallbackFunc(HI_HDMI_ID_0, &g_stCallbackFunc);
#else
    HI_HDMI_INIT_PARA_S stHdmiPara;

    NC_HI_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);

    stHdmiPara.pfnHdmiEventCallback = NULL;
    stHdmiPara.pCallBackArgs = NULL;
    stHdmiPara.enForceMode = HI_HDMI_FORCE_HDMI;
    HI_MPI_HDMI_Init(&stHdmiPara);

    HI_MPI_HDMI_Open(HI_HDMI_ID_0);
#endif
 
	VO_HDMI_PARAM_S     stHdmiPara_Luma;

    stHdmiPara_Luma.stCSC.u32Luma = 50;
    stHdmiPara_Luma.stCSC.u32Hue = 50;
    stHdmiPara_Luma.stCSC.u32Saturation = 50;
    stHdmiPara_Luma.stCSC.u32Contrast = 50;
    stHdmiPara_Luma.stCSC.enCscMatrix = 0;

    HI_MPI_HDMI_GetAttr(HI_HDMI_ID_0, &stAttr);

    stAttr.bEnableHdmi = HI_TRUE;

    stAttr.bEnableVideo = HI_TRUE;
    stAttr.enVideoFmt = enVideoFmt;

    stAttr.enVidOutMode = HI_HDMI_VIDEO_MODE_YCBCR444;
    stAttr.enDeepColorMode = HI_HDMI_DEEP_COLOR_OFF;
    stAttr.bxvYCCMode = HI_FALSE;

    stAttr.bEnableAudio = HI_FALSE;
    stAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S;
    stAttr.bIsMultiChannel = HI_FALSE;

    stAttr.enBitDepth = HI_HDMI_BIT_DEPTH_16;

    stAttr.bEnableAviInfoFrame = HI_TRUE;
    stAttr.bEnableAudInfoFrame = HI_TRUE;
    stAttr.bEnableSpdInfoFrame = HI_FALSE;
    stAttr.bEnableMpegInfoFrame = HI_FALSE;

    stAttr.bDebugFlag = HI_FALSE;
    stAttr.bHDCPEnable = HI_FALSE;

    stAttr.b3DEnable = HI_FALSE;

    HI_MPI_HDMI_SetAttr(HI_HDMI_ID_0, &stAttr);

    HI_MPI_VO_SetHdmiParam( 0, &stHdmiPara_Luma);

    HI_MPI_HDMI_Start(HI_HDMI_ID_0);

    printf("HDMI start success.\n");
    return HI_SUCCESS;
}

HI_S32 NC_HI_VO_HdmiStop(HI_VOID)
{
    HI_MPI_HDMI_Stop(HI_HDMI_ID_0);
    HI_MPI_HDMI_Close(HI_HDMI_ID_0);
    HI_MPI_HDMI_DeInit();

    return HI_SUCCESS;
}


typedef struct hiPTHREAD_HIFB_SAMPLE
{
    HI_S32 fd;
    HI_S32 layer;
    HI_S32 ctrlkey;
}PTHREAD_HIFB_SAMPLE_INFO;

HIFB_BUFFER_S stCCanvasBuf;
PTHREAD_HIFB_SAMPLE_INFO stInfo;
PTHREAD_HIFB_SAMPLE_INFO stInfo2;
PTHREAD_HIFB_SAMPLE_INFO stMouseInfo;

int NC_HI_FB_Init( void )
{
    HI_S32 s32Ret = HI_SUCCESS;
    HIFB_LAYER_INFO_S stLayerInfo = {0};
    HI_U32 x, y;
	HI_BOOL Show;
	HI_BOOL bCompress = HI_TRUE;
    HIFB_POINT_S stPoint = {0};
    struct fb_var_screeninfo stVarInfo;
	char file[12] = "/dev/fb0";
    HIFB_BUFFER_S stCanvasBuf;

	PTHREAD_HIFB_SAMPLE_INFO *pstInfo = &stInfo;
	HIFB_CAPABILITY_S *pstCap;

	if(_VO_Init_f==0)
	{
		DBG_ERR("Not initialized VO\n");
		return 1;
	}


	stInfo.layer   =  0;
	stInfo.fd      = -1;
	stInfo.ctrlkey =  2; //2

	DBG_INFO("init_osd_Layer %x\n", pstInfo->layer);
	switch (pstInfo->layer)
	{
		case GRAPHICS_LAYER_G0 :
			strcpy(file, "/dev/fb0");
			break;
		case GRAPHICS_LAYER_HC0 :
#ifdef SUPPORT_3520D_6158C
			strcpy(file, "/dev/fb2");
#else
			strcpy(file, "/dev/fb3");
#endif
			break;
		default:
			strcpy(file, "/dev/fb0");
			break;
	}
		/* 1. open framebuffer device overlay 0 */
	pstInfo->fd = open(file, O_RDWR, 0);
	if(pstInfo->fd < 0)
	{
		DBG_ERR("open %s failed!\n",file);
		return HI_NULL;
	}

    s32Ret = ioctl(pstInfo->fd, FBIOGET_VSCREENINFO, &stVarInfo);
	if(s32Ret < 0)
	{
		DBG_ERR("GET_VSCREENINFO failed!\n");
		return HI_NULL;
	}

    if (ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
    	DBG_ERR("set screen original show position failed!\n");
        return HI_NULL;
    }

    stVarInfo.xres = _FB_Width;
    stVarInfo.yres = _FB_Height;
    stVarInfo.xres_virtual = _FB_Width;
    stVarInfo.yres_virtual = _FB_Height;
    stVarInfo.xoffset = (_FB_Width-stVarInfo.xres)/2;
    stVarInfo.yoffset = (_FB_Height-stVarInfo.yres)/2;

    //DBG_INFO("x[%d], y[%d], w[%d], h[%d]\n", stVarInfo.xres, stVarInfo.yres, stVarInfo.xres_virtual, stVarInfo.yres_virtual);

    s32Ret = ioctl(pstInfo->fd, FBIOPUT_VSCREENINFO, &stVarInfo);
    if(s32Ret < 0)
	{
    	DBG_ERR("PUT_VSCREENINFO failed!s32Ret %x\n", s32Ret);
		return HI_NULL;
	}
    DBG_INFO("xres[%d], yres[%d], w[%d], h[%d]\n", stVarInfo.xres, stVarInfo.yres, stVarInfo.xres_virtual, stVarInfo.yres_virtual);

	int BufMultiply = 1;

    switch (pstInfo->ctrlkey)
	{
		case 0 :
		{
			stLayerInfo.BufMode = HIFB_LAYER_BUF_ONE;
			stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
			BufMultiply = 2;
			break;
		}

		case 1 :
		{
			stLayerInfo.BufMode = HIFB_LAYER_BUF_DOUBLE;
		    stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
		    BufMultiply = 3;
			break;
		}

		default:
		{
			stLayerInfo.BufMode = HIFB_LAYER_BUF_NONE;
			stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
		}
	}

    //DBG_INFO("x[%d], y[%d], w[%d], h[%d]\n", stLayerInfo.u32ScreenWidth, stLayerInfo.u32ScreenHeight, \
	//	stLayerInfo.u32DisplayWidth, stLayerInfo.u32DisplayHeight);
    s32Ret = ioctl(pstInfo->fd, FBIOPUT_LAYER_INFO, &stLayerInfo);
    if(s32Ret < 0)
	{
    	DBG_ERR("PUT_LAYER_INFO failed!\n");
		return HI_NULL;
	}
	Show = HI_TRUE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &Show) < 0)
    {
    	DBG_ERR("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_NULL;
    }

    unsigned int BufferByteSize =  _FB_Width * _FB_Height * 4 * BufMultiply;

    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&(stCanvasBuf.stCanvas.u32PhyAddr), ((void**)&_FB_pBitmapBuffer),
            NULL, NULL, BufferByteSize))
    {
		DBG_INFO("allocate memory (maxW*maxH*2 bytes) failed\n");
        return HI_NULL;
    }
	memcpy(&stCCanvasBuf, &stCanvasBuf, sizeof(HIFB_BUFFER_S));
	memcpy(&stInfo, pstInfo, sizeof(PTHREAD_HIFB_SAMPLE_INFO));

	DBG_INFO("## Allocate OSD memory : (%x) (%d x %d x %d) : %d bytes\n",_FB_pBitmapBuffer ,_FB_Width,_FB_Height,BufMultiply,BufferByteSize);

    memset(_FB_pBitmapBuffer, 0x00, BufferByteSize);


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////// mouse layer
#if 0
    s32Ret = HI_MPI_VO_UnBindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev);
	if (HI_SUCCESS != s32Ret)
	{
		DBG_ERR("UnBindGraphicLayer failed with %d!\n", s32Ret);
		//goto SAMPLE_HIFB_NoneBufMode_0;
		return -1;
	}
	if (HI_SUCCESS !=(s32Ret= HI_MPI_VO_BindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev)))
	{
		DBG_ERR("Graphic Bind to VODev failed!, s32Ret = %x\n", s32Ret);
		SAMPLE_COMM_SYS_Exit();
		SAMPLE_HIFB_VO_Stop();
		return -1;
	}
#endif
#ifdef SUPPORT_3520D_6158C
	stInfo2.layer   =  2;
#else
	stInfo2.layer   =  3;
#endif
	stInfo2.fd      = -1;
	stInfo2.ctrlkey = 3;
//	mouse_layer_init(&stInfo2);

	return HI_NULL;
}

static HI_VOID NC_HI_VO_HdmiConvertSync(VO_INTF_SYNC_E enIntfSync, HI_HDMI_VIDEO_FMT_E *penVideoFmt)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_PAL;
            break;
        case VO_OUTPUT_NTSC:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_NTSC;
            break;
        case VO_OUTPUT_1080P24:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_24;
            break;
        case VO_OUTPUT_1080P25:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_25;
            break;
        case VO_OUTPUT_1080P30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_30;
            break;
        case VO_OUTPUT_720P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_720P_50;
            break;
        case VO_OUTPUT_720P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_720P_60;
            break;
        case VO_OUTPUT_1080I50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080i_50;
            break;
        case VO_OUTPUT_1080I60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080i_60;
            break;
        case VO_OUTPUT_1080P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_50;
            break;
        case VO_OUTPUT_1080P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_60;
            break;
        case VO_OUTPUT_576P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_576P_50;
            break;
        case VO_OUTPUT_480P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_480P_60;
            break;
        case VO_OUTPUT_800x600_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_800X600_60;
            break;
        case VO_OUTPUT_1024x768_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1024X768_60;
            break;
        case VO_OUTPUT_1280x1024_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1280X1024_60;
            break;
        case VO_OUTPUT_1366x768_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1366X768_60;
            break;
        case VO_OUTPUT_1440x900_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1440X900_60;
            break;
        case VO_OUTPUT_1280x800_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1280X800_60;
            break;
        case VO_OUTPUT_1600x1200_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1600X1200_60;
            break;
        case VO_OUTPUT_2560x1440_30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_2560x1440_30;
            break;
        case VO_OUTPUT_2560x1600_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_2560x1600_60;
            break;
        case VO_OUTPUT_3840x2160_30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_3840X2160P_30;
            break;
        case VO_OUTPUT_3840x2160_60:
        	*penVideoFmt = HI_HDMI_VIDEO_FMT_3840X2160P_60;
        	break;
        default :
            DBG_INFO("Unkonw VO_INTF_SYNC_E value!\n");
            break;
    }

    return;
}

static struct fb_bitfield g_r16 = {10, 5, 0};
static struct fb_bitfield g_g16 = {5, 5, 0};
static struct fb_bitfield g_b16 = {0, 5, 0};
static struct fb_bitfield g_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

#include "cursor.raw"

static int WriteMouseRawImage(  int x, int y, int w, int h, char* buf, char* fb)
{
	int j,i;
	HI_CHAR *pDst = NULL;
	HI_U16 *src = (HI_U16*)buf;
	HI_U16 *des = NULL;


	pDst = (unsigned char*)(fb + (32*y) + x);
	des = (HI_U16*)pDst;

	for(j=0; j<h; j++)
	{

		//if(colorkeyon)
		//{
		//	memcpy( pDst, (void*)buf+(w*j*2), w*2 );
		//	//pDst += g_u32FixScreenStride[layer];
		//	pDst += 1920;
		//}
		//else
		{
			i = w;
			while(i--)
			{
				if(src[(w*j) + i] != 0x7c1f)
					des[i] = src[(w*j) + i] | 0x8000;
			}
		}
			des += 32;
	}

	//g_var[MOUSE_LAYER].yoffset = 0;

///////////////////// Write HD End ///////////////////////////////////////////////////
    return 0;
}

int mouse_layer_init(PTHREAD_HIFB_SAMPLE_INFO *pData)
{
    HI_S32 i;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    HI_U32 u32FixScreenStride = 0;
    unsigned char *pShowScreen;
    unsigned char *pHideScreen;
    HIFB_ALPHA_S stAlpha;
    HIFB_POINT_S stPoint = {40, 112};
    char file[12] = "/dev/fb0";
	HI_BOOL g_bCompress = HI_TRUE;
    char image_name[128];
	HI_U8 *pDst = NULL;
    HI_BOOL bShow;
    PTHREAD_HIFB_SAMPLE_INFO *pstInfo;
	HIFB_COLORKEY_S stColorKey;

    if(HI_NULL == pData)
    {
        return HI_NULL;
    }
    pstInfo = (PTHREAD_HIFB_SAMPLE_INFO *)pData;
    switch (pstInfo->layer)
    {
        case 0 :
            strcpy(file, "/dev/fb0");
            break;
#ifdef SUPPORT_3520D_6158C
        case 2 :
            strcpy(file, "/dev/fb2");
            break;
#endif
        case 3 :
            strcpy(file, "/dev/fb3");
            break;
        default:
            strcpy(file, "/dev/fb0");
            break;
    }
	//printf("---------------------------MOUSE INIT-------------------------------\n");
    /* 1. open framebuffer device overlay 0 */
    pstInfo->fd = open(file, O_RDWR, 0);
    if(pstInfo->fd < 0)
    {
        printf("open %s failed!\n",file);
        return HI_NULL;
    }

    bShow = HI_FALSE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_NULL;
    }
    /* 2. set the screen original position */
	stPoint.s32XPos = _FB_Width/2;
    stPoint.s32YPos = _FB_Height/2;

    if (ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        printf("set screen original show position failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    /* 3.set alpha */
    stAlpha.bAlphaEnable = HI_TRUE;
    stAlpha.bAlphaChannel = HI_TRUE;
    stAlpha.u8Alpha0 = 0xff;
    stAlpha.u8Alpha1 = 0xff;
    stAlpha.u8GlobalAlpha = 0xff;
    if (ioctl(pstInfo->fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        printf("Set alpha failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    /** //if(pstInfo->layer == HIFB_LAYER_CURSOR_0) */
	/** { */
	/**   stColorKey.bKeyEnable = HI_TRUE; */
	/**   stColorKey.u32Key = 0x1f; */
	/**   if (ioctl(pstInfo->fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0) */
	/**   { */
	/**     printf("FBIOPUT_COLORKEY_HIFB!\n"); */
	/**     close(pstInfo->fd); */
    /**     return HI_NULL; */
	/**   } */
    /** } */
    /* 4. get the variable screen info */
    if (ioctl(pstInfo->fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        printf("Get variable screen info failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    /* 5. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    */

    //usleep(4*1000*1000);

    var.xres_virtual = 32;
    var.yres_virtual = 32;
    var.xres = 32;
    var.yres = 32;
    var.transp= g_a16;
    var.red = g_r16;
    var.green = g_g16;
    var.blue = g_b16;
    var.bits_per_pixel = 16;
    var.activate = FB_ACTIVATE_NOW;
    var.yoffset = 0;
    var.xoffset = 0;
    /* 6. set the variable screeninfo */
    if (ioctl(pstInfo->fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        printf("Put variable screen info failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    /* 7. get the fix screen info */
    if (ioctl(pstInfo->fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        printf("Get fix screen info failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    u32FixScreenStride = fix.line_length;   /*fix screen stride*/
    /* 8. map the physical video memory for user use */
    pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, pstInfo->fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        printf("mmap framebuffer failed!\n");
		close(pstInfo->fd);
        return HI_NULL;
    }
    memset(pShowScreen, 0x83E0, fix.smem_len);

    /* time to paly*/
    bShow = HI_TRUE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_HIFB failed!\n");
        munmap(pShowScreen, fix.smem_len);
        return HI_NULL;
    }

	/** WriteMouseRawImage( 0, 0, cursor_attrib[1], cursor_attrib[2],  cursor_Buf, pShowScreen); */
	if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY, &var) < 0)
	{
		printf("FBIOPAN_DISPLAY failed!\n");
		munmap(pShowScreen, fix.smem_len);
		close(pstInfo->fd);
		return HI_FALSE;
	}
	//pMouseFrameBuffer = pShowScreen;
	//MouseFrameBufferlen = fix.smem_len;
	memcpy(&stMouseInfo, pstInfo, sizeof(PTHREAD_HIFB_SAMPLE_INFO));
	munmap(pShowScreen, fix.smem_len);
    /* unmap the physical memory */
    return HI_NULL;
}

typedef struct
{
    HI_S32 s32XPos;         /**<  horizontal position */
    HI_S32 s32YPos;         /**<  vertical position */
}HIFB_POINT_S2;


int NC_HI_FB_Move_Cursor(unsigned int  x, unsigned int  y)
{
	//printf("Move_Cursor X:%x , Y:%x Do Notting...\n", x, y);

    HIFB_POINT_S2 stPoint = {0, 0};

    stPoint.s32XPos = x;
    stPoint.s32YPos = y;

    // 2. set the screen original position
    if (ioctl(stMouseInfo.fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        printf("set screen original show position failed!))))))))))))))))))))))))))))))))))))))))))))))))))))\n");
        return -1;
    }

}



void NC_HI_FB_UpdateRect( int x, int y, int w, int h)
{
	HI_S32 s32Ret = HI_SUCCESS;

	stCCanvasBuf.stCanvas.u32Height = _FB_Height;
	stCCanvasBuf.stCanvas.u32Width = _FB_Width;
	stCCanvasBuf.stCanvas.u32Pitch = _FB_Width*4;
	stCCanvasBuf.stCanvas.enFmt = HIFB_FMT_ARGB8888;
	stCCanvasBuf.UpdateRect.x = x;
	stCCanvasBuf.UpdateRect.y = y;
	stCCanvasBuf.UpdateRect.w = w;
	stCCanvasBuf.UpdateRect.h = h;

	s32Ret = ioctl(stInfo.fd, FBIO_REFRESH, &stCCanvasBuf);

	//DBG_INFO("x:%d y:%d w:%d h:%d , ret : %d\n",x,y,w,h,s32Ret);
}

void NC_HI_FB_Test( void)
{
	unsigned int *pDst = NC_HI_FB_Get_BitmapBuffer();

	int ii;
	for(ii=0;ii<(1920*1080/2);ii++)
	{
		*pDst++ = 0x00FF0000 | (ii <<24);
	}

	NC_HI_FB_UpdateRect(0,0,100,100);
}


typedef unsigned char BYTE;




HI_U32 NC_COMM_SYS_CalcPicVbBlkSize( int w, int h, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth,COMPRESS_MODE_E enCompFmt)
{
    HI_S32 s32Ret      		= HI_FAILURE;
    SIZE_S stSize;
    HI_U32 u32Width 		= 0;
    HI_U32 u32Height 		= 0;
    HI_U32 u32BlkSize 		= 0;
	HI_U32 u32HeaderSize 	= 0;

	stSize.u32Width = w;
	stSize.u32Height = h;

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
    	DBG_ERR("pixel format[%d] input failed!\n", enPixFmt);
            return HI_FAILURE;
    }

    if (16!=u32AlignWidth && 32!=u32AlignWidth && 64!=u32AlignWidth)
    {
    	DBG_ERR("system align width[%d] input failed!\n",u32AlignWidth);
            return HI_FAILURE;
    }
    if (704 == stSize.u32Width)
    {
        stSize.u32Width = 720;
    }
    //DBG_INFO("w:%d, u32AlignWidth:%d\n", CEILING_2_POWER(stSize.u32Width,u32AlignWidth), u32AlignWidth);

    u32Width  = CEILING_2_POWER(stSize.u32Width, u32AlignWidth);
    u32Height = CEILING_2_POWER(stSize.u32Height,u32AlignWidth);

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt)
    {
        u32BlkSize = u32Width * u32Height * 2;
    }
    else
    {
        u32BlkSize = u32Width * u32Height * 3 / 2;
    }


	if(COMPRESS_MODE_SEG == enCompFmt)
	{
		VB_PIC_HEADER_SIZE(u32Width,u32Height,enPixFmt,u32HeaderSize);
	}

	u32BlkSize += u32HeaderSize;

    return u32BlkSize;
}

HI_VOID NC_COMM_VI_SetMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{

#ifdef SUPPORT_3520D_6158C

    	pstDevAttr->au32CompMask[0] = 0x000000FF << ViDev * 8;
	   if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
	   {
		   pstDevAttr->au32CompMask[0] = 0x0000FF00 << ViDev * 8;
		   pstDevAttr->au32CompMask[1] = 0x000000FF << ViDev * 8;
	   }
	   else
	   {
		   pstDevAttr->au32CompMask[1] = 0x0;
	   }

#else
    switch (ViDev % 2)
      {
          case 0:
            pstDevAttr->au32CompMask[0] = 0x00FF0000;
            if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[0] = 0xFF000000;
                pstDevAttr->au32CompMask[1] = 0xFF000000>>8;
            }
            else
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;

          case 1:
             pstDevAttr->au32CompMask[0] = 0xFF000000;
             pstDevAttr->au32CompMask[1] = 0x0;
              break;
          default:
              HI_ASSERT(0);
      }
#endif


}


HI_S32 NC_COMM_VI_BindVpss(VI_PARAMS_S *pstViParam)
{
    HI_S32 j, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    VI_PARAMS_S stViParam;
    VI_CHN ViChn;

    memcpy( &stViParam, pstViParam, sizeof( VI_PARAMS_S ) );

    VpssGrp = 0;
    for (j=0; j<stViParam.s32ViChnCnt; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval;
        stSrcChn.enModId = HI_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;

        stDestChn.enModId = HI_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

        DBG_INFO("bind viu(%d)->vpss(%d)(%d)\n",ViChn,VpssGrp,0);

        s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_ERR("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        VpssGrp ++;
    }
    return HI_SUCCESS;
}

HI_S32 NC_COMM_VO_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    //DBG_INFO("Bind vpssgrp(%d)(ch %d) -> vo(%d)\n",VpssGrp,VpssChn,VoChn);

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 NC_COMM_VO_BindVI(VO_LAYER VoLayer,VO_CHN VoChn,VI_CHN ViChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

	DBG_INFO("bind viu(%d)->vou(%d)(ch %d)\n",ViChn,VoChn, ViChn);

    //DBG_INFO("Bind vpssgrp(%d)(ch %d) -> vo(%d)\n",VpssGrp,VpssChn,VoChn);

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        DBG_INFO("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
};


static VI_DEV_ATTR_S DEV_ATTR_DEFAULT = // DEV_ATTR_BT656_P_1MUX
{

    VI_MODE_BT656,

    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},


	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},

    VI_INPUT_DATA_YVYU,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,


    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },

    VI_PATH_BYPASS,

    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_1120_DEFAULT =
{
    /*interface mode*/
	VI_MODE_BT1120_STANDARD,
	//VI_MODE_BT1120_INTERLEAVED,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},

	/* for single/double edge, must be set when double edge*/
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    //{-1, -1, -1, -1},
    {0, 0, 0, 0},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_UVUV,
	//VI_INPUT_DATA_VUVU,

    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV

};

static VI_DEV_ATTR_S DEV_ATTR_656_2MUX = // DEV_ATTR_BT656_P_1MUX
{
   
    VI_MODE_BT656,

    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	
	VI_CLK_EDGE_DOUBLE,

    /*AdChnId*/
    {-1, -1, -1, -1},

    VI_INPUT_DATA_YVYU,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,


    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },

    VI_PATH_BYPASS,
	
    VI_DATA_TYPE_YUV
};

int NC_HI_VI_Port_SetFormat( VI_DEV ViDev, NC_HI_VI_PORT_ATTR_S *pPortAttr )
{
	HI_S32 s32Ret;
	VI_DEV_ATTR_S stViDevAttr;


	DBG_INFO("dev (%d)\n",ViDev);

	HI_MPI_VI_DisableDev(ViDev);

	memset(&stViDevAttr,0,sizeof(stViDevAttr));
	if( pPortAttr->Interface == VI_MODE_BT1120_STANDARD )
	{
		memcpy(&stViDevAttr, &DEV_ATTR_1120_DEFAULT, sizeof(stViDevAttr));
	}
	else
	{
		if( pPortAttr->Multiplex == VI_WORK_MODE_2Multiplex )
		{
			memcpy(&stViDevAttr, &DEV_ATTR_656_2MUX, sizeof(stViDevAttr));
		}
		else if( pPortAttr->Multiplex == VI_WORK_MODE_1Multiplex )
		{
			memcpy(&stViDevAttr, &DEV_ATTR_DEFAULT, sizeof(stViDevAttr));
		}

	}

	stViDevAttr.enClkEdge = pPortAttr->ClkEdge;
	stViDevAttr.enIntfMode = pPortAttr->Interface;
	stViDevAttr.enWorkMode = pPortAttr->Multiplex;

	NC_COMM_VI_SetMask(ViDev, &stViDevAttr);

	s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("vi_dev[%d] failed [%#x]!\n", ViDev,s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_EnableDev(ViDev);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("vi_dev[%d] failed [%#x]!\n", ViDev,s32Ret);
		return HI_FAILURE;
	}


	return 0;
}



#if 0
static NC_HI_VI_STAT_S  NCHI_VI_ChannelStatus[ DEF_VI_CH_MAX ];


static NC_HI_VI_STAT_S *NC_HI_VI_Channel_GetStatus( VI_CHN ViChn )
{
	return &NCHI_VI_ChannelStatus[ ViChn ];
}
#endif


int NC_HI_VI_Channel_SetFormat(VI_CHN ViChn, VO_CHN BindVpssVoChn, VI_WORK_MODE_E Mux, NC_VI_CH_FMT_S *pChFmt, unsigned int CableDist )
{
	HI_S32 s32Ret;
	VI_CHN_BIND_ATTR_S stChnBindAttr = {0,};
	NC_VIVO_CH_FORMATDEF FmtDef = NC_HI_VI_FindFormatDef( pChFmt->format_standard, pChFmt->format_resolution, pChFmt->format_fps );

	//NCHI_VI_ChannelStatus[ ViChn ].pChFmt = pChFmt;
	RAPTOR3_VI_Channel_SetStatus(ViChn, pChFmt);

	int width = pChFmt->width;
	int height = pChFmt->height;

	HI_MPI_VI_DisableChn( ViChn );

	s32Ret = HI_MPI_VI_GetChnBind(ViChn, &stChnBindAttr);
	if (HI_SUCCESS != s32Ret)
	{
		//DBG_ERR("HI_MPI_VI_GetChnBind failed with %#x\n", s32Ret);
	}

	// HI_MPI_VI_BindChn : vi ch <= vi dev
	if(Mux == VI_WORK_MODE_1Multiplex)
	{
		stChnBindAttr.ViDev = ViChn/4;
		stChnBindAttr.ViWay = 0;
	}
	else if(Mux == VI_WORK_MODE_4Multiplex)
	{
	stChnBindAttr.ViDev = ViChn/4;
	stChnBindAttr.ViWay = ViChn%4;
	}
	else // 2 Multiplex Setting
	{
		stChnBindAttr.ViDev = ViChn / 4;
//		stChnBindAttr.ViWay = ViChn%4;
		switch(ViChn % 4)
		{
		case 0 : stChnBindAttr.ViWay = 0;
				 break;
		case 1 :
		case 2 :
		case 3 : stChnBindAttr.ViWay = 1;
				 break;
		}
//		stChnBindAttr.ViDev = ViChn/4;
//		if(ViChn%4 == 2 || ViChn%4 == 0)
//			stChnBindAttr.ViWay = 0;
//		else
//			stChnBindAttr.ViWay = 1;
	}

	s32Ret = HI_MPI_VI_BindChn(ViChn, &stChnBindAttr);
	if (HI_SUCCESS != s32Ret)
	{
		DBG_ERR("ViChn:%d call HI_MPI_VI_BindChn failed with %#x\n",ViChn, s32Ret);
		return HI_FAILURE;
	}

	VI_CHN_ATTR_S stChnAttr;
	memset( &stChnAttr, 0, sizeof(VI_CHN_ATTR_S) );
	s32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= s32Ret)
	{
		//DBG_ERR( "HI_MPI_VI_GetChnAttr failed : %x\n",s32Ret);
		//return HI_FAILURE;
	}

	stChnAttr.s32SrcFrameRate = -1;
	stChnAttr.s32DstFrameRate = -1;

	if(pChFmt->format_fps == FMT_NT || pChFmt->format_fps == FMT_PAL)
		stChnAttr.enScanMode = VI_SCAN_INTERLACED;
	else
		stChnAttr.enScanMode = VI_SCAN_PROGRESSIVE;

	stChnAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;
	stChnAttr.enCapSel = VI_CAPSEL_BOTH;
	stChnAttr.stCapRect.s32X = 0;
	stChnAttr.stCapRect.s32Y = 0;

	stChnAttr.stCapRect.u32Width = width;
	stChnAttr.stCapRect.u32Height = height;
	stChnAttr.stDestSize.u32Width = width;
	stChnAttr.stDestSize.u32Height = height;

	/* 2mux-cif mode - over 3M(3M/4M/5M/8M RT)*/
	if( Mux == VI_WORK_MODE_2Multiplex )
	{
		if(	FmtDef == AHD30_4M_30P || FmtDef == AHD30_4M_25P || FmtDef == AHD30_3M_30P || FmtDef == AHD30_3M_25P || \
			FmtDef == AHD30_5M_20P || FmtDef == AHD30_5_3M_20P || FmtDef == AHD30_6M_18P || FmtDef == AHD30_6M_20P || \
			FmtDef == AHD30_8M_15P || FmtDef == AHD30_8M_12_5P || FmtDef == CVI_4M_25P || FmtDef == CVI_4M_30P || \
			FmtDef == CVI_8M_12_5P || FmtDef == CVI_8M_15P || FmtDef == TVI_4M_25P || FmtDef == TVI_4M_30P ||
			FmtDef == TVI_5M_20P   || FmtDef == TVI_8M_15P || FmtDef == TVI_8M_12_5P )
		{
			stChnAttr.stCapRect.u32Width = width/2;
			stChnAttr.stCapRect.u32Height = height;
			stChnAttr.stDestSize.u32Width = width/2;
			stChnAttr.stDestSize.u32Height = height;
			DBG_INFO("2MUX mode, CAP:Width(%d)xHeight(%d), DEST:Width(%d)xHeight(%d)\n", \
					stChnAttr.stCapRect.u32Width, stChnAttr.stCapRect.u32Height, stChnAttr.stDestSize.u32Width, stChnAttr.stDestSize.u32Height );
		}
#ifdef  SUPPORT_3520D_6158C
		else if ( FmtDef == AHD30_3M_18P || FmtDef == AHD30_4M_15P || FmtDef == AHD30_5M_12_5P || FmtDef == TVI_3M_18P || FmtDef == TVI_5M_12_5P )
		{
			stChnAttr.stCapRect.u32Width = width;
			stChnAttr.stCapRect.u32Height = height;
			stChnAttr.stDestSize.u32Width = width/2;
			stChnAttr.stDestSize.u32Height = height/2;
			DBG_INFO("Hi3520D NRT 2MUX mode, CAP:Width(%d)xHeight(%d), DEST:Width(%d)xHeight(%d)\n", \
					stChnAttr.stCapRect.u32Width, stChnAttr.stCapRect.u32Height, stChnAttr.stDestSize.u32Width, stChnAttr.stDestSize.u32Height );
		}
#endif
	}

	VI_SKIP_MODE_E skipmode = VI_SKIP_NONE;

	if( pChFmt->format_resolution == FMT_H960_2EX ||
	pChFmt->format_resolution == FMT_H960_Btype_2EX )
	{
		skipmode = VI_SKIP_YES;
		stChnAttr.stDestSize.u32Width = width / 4;
		stChnAttr.stDestSize.u32Height = height;
		DBG_INFO("Skip mode : VI_SKIP_YES\n");
	}
	else if( pChFmt->format_resolution == FMT_H960_EX )
	{
		stChnAttr.stDestSize.u32Width = width / 2;
		stChnAttr.stDestSize.u32Height = height;
	}
	else if( pChFmt->format_resolution == FMT_720P_Btype_EX || pChFmt->format_resolution == FMT_720P_EX )
	{
		//skipmode = VI_SKIP_YES;
		stChnAttr.stDestSize.u32Width = width / 2; //07.28 when 720PEX mode,VI skip 1/2 horizontal pixel
		stChnAttr.stDestSize.u32Height = height;
		DBG_INFO("H Size EX mode skip\n");
	}

	s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_SetSkipMode(ViChn, skipmode);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_EnableChn(ViChn);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}


#ifdef VPSS_DISABLE

#else
	/// BIND
	// VI to vpss
	{
		HI_S32 j, s32Ret;
		VPSS_GRP VpssGrp;
		MPP_CHN_S stSrcChn;
		MPP_CHN_S stDestChn;

		if(Mux == VI_WORK_MODE_1Multiplex)
			VpssGrp = BindVpssVoChn / 4;
		else if(Mux == VI_WORK_MODE_2Multiplex)
			VpssGrp = BindVpssVoChn / 2;
		else
			VpssGrp = BindVpssVoChn;

		NC_HI_VpssGrp_Create( VpssGrp, width, height, CableDist );
		//for (j=0; j<stViParam.s32ViChnCnt; j++)
		{
			//ViChn = j * stViParam.s32ViChnInterval;
			stSrcChn.enModId = HI_ID_VIU;
			stSrcChn.s32DevId = 0;
			stSrcChn.s32ChnId = ViChn;

			stDestChn.enModId = HI_ID_VPSS;
			stDestChn.s32DevId = VpssGrp;
			stDestChn.s32ChnId = 0;

			DBG_INFO("bind viu(%d)->vpssgrp(%d)(ch %d)\n",ViChn,VpssGrp,ViChn);

			HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);

			s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
			if (s32Ret != HI_SUCCESS)
			{
				DBG_ERR("failed with %#x!\n", s32Ret);
				return HI_FAILURE;
			}

			//VpssGrp ++;
		}
	}
#endif

	return 0;
}

HI_S32 NC_COMM_VPSS_Start(HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    VPSS_CHN_ATTR_S stChnAttr = {0};
    VPSS_GRP_PARAM_S stVpssParam = {0};
    HI_S32 s32Ret;
    HI_S32 i, j;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

        stGrpAttr.bIeEn = HI_TRUE;
        stGrpAttr.bNrEn = HI_TRUE;
        stGrpAttr.bDciEn = HI_FALSE;
        stGrpAttr.bHistEn = HI_FALSE;
        stGrpAttr.bEsEn = HI_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_AUTO;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }


    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        /*** create vpss group ***/
        s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_ERR("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** set vpss param ***/
        s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_ERR("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        stVpssParam.u32IeStrength = 0;
        s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_ERR("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** enable vpss chn, with frame ***/
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bUVInvert = HI_FALSE;
            stChnAttr.bBorderEn = HI_FALSE;
            stChnAttr.stBorder.u32Color = 0xFFEC8B;
            stChnAttr.stBorder.u32LeftWidth = 2;
            stChnAttr.stBorder.u32RightWidth = 2;
            stChnAttr.stBorder.u32TopWidth = 2;
            stChnAttr.stBorder.u32BottomWidth = 2;

            s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != HI_SUCCESS)
            {
                DBG_ERR("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                DBG_ERR("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
        }

        /*** start vpss group ***/
        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            DBG_ERR("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

    }
    return HI_SUCCESS;
}

HI_S32 NC_COMM_VPSS_SetZOOM(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S *pstVpssCropInfo,HI_BOOL zoomFlag)
{
	VPSS_CROP_INFO_S stVpssCropInfo;
	HI_S32 s32Ret = HI_SUCCESS;

	if(HI_TRUE == zoomFlag)
	{
		/*** enable vpss group clip ***/
	    stVpssCropInfo.bEnable = HI_TRUE;
	    stVpssCropInfo.enCropCoordinate = pstVpssCropInfo->enCropCoordinate;
	    stVpssCropInfo.stCropRect.s32X = ALIGN_BACK(pstVpssCropInfo->stCropRect.s32X,2);
	    stVpssCropInfo.stCropRect.s32Y = ALIGN_BACK(pstVpssCropInfo->stCropRect.s32Y,2);
	    stVpssCropInfo.stCropRect.u32Width = ALIGN_BACK(pstVpssCropInfo->stCropRect.u32Width,16);
	    stVpssCropInfo.stCropRect.u32Height = ALIGN_BACK(pstVpssCropInfo->stCropRect.u32Height,16);
		//printf("stVpssCropInfo.stCropRect.s32X 		----> %d\n",stVpssCropInfo.stCropRect.s32X);
		//printf("stVpssCropInfo.stCropRect.s32Y 		----> %d\n",stVpssCropInfo.stCropRect.s32Y);
		//printf("stVpssCropInfo.stCropRect.u32Height	----> %d\n",stVpssCropInfo.stCropRect.u32Height);
		//printf("stVpssCropInfo.stCropRect.u32Width 	----> %d\n",stVpssCropInfo.stCropRect.u32Width);
	}
	else
	{
		/*** enable vpss group clip ***/
	    stVpssCropInfo.bEnable = HI_FALSE;
	    stVpssCropInfo.enCropCoordinate = pstVpssCropInfo->enCropCoordinate;
	    stVpssCropInfo.stCropRect.s32X = 0;
	    stVpssCropInfo.stCropRect.s32Y = 0;
	    stVpssCropInfo.stCropRect.u32Height = 0;
	    stVpssCropInfo.stCropRect.u32Width = 0;
	}

	s32Ret = HI_MPI_VPSS_SetGrpCrop(VpssGrp, &stVpssCropInfo);
    if (HI_SUCCESS != s32Ret)
    {
        DBG_ERR("HI_MPI_VPSS_SetGrpCrop failed with %#x!\n", s32Ret);
        return -1;
    }
	return s32Ret;
}

int NC_HI_VpssGrp_Create( VPSS_GRP VpssGrp, int width, int height, int cable_dist )
{
	HI_S32 s32Ret;

	HI_S32 s32ChnCnt = VPSS_MAX_CHN_NUM;

	SIZE_S stSize;

	stSize.u32Width = width;
	stSize.u32Height = height;

	VPSS_GRP_ATTR_S stGrpAttr;
	memset(&stGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
	stGrpAttr.u32MaxW = stSize.u32Width;
	stGrpAttr.u32MaxH = stSize.u32Height;

#ifdef __FOR_CHINA_HYUNDAI_180319
	stGrpAttr.bIeEn = HI_TRUE;		// Enable IE(Image Enhance)
	stGrpAttr.bNrEn = HI_TRUE;
#else
	#ifdef __FOR_DEMO_171024
	stGrpAttr.bIeEn = HI_FALSE;		// Enable IE(Image Enhance)
	stGrpAttr.bNrEn = HI_FALSE;
	#else
	stGrpAttr.bIeEn = HI_TRUE;		// Enable IE(Image Enhance)
	stGrpAttr.bNrEn = HI_TRUE;
	#endif
#endif

stGrpAttr.bDciEn = HI_FALSE;
	stGrpAttr.bHistEn = HI_FALSE;
	stGrpAttr.bEsEn = HI_FALSE;
	stGrpAttr.enDieMode = VPSS_DIE_MODE_AUTO;//VPSS_DIE_MODE_AUTO;//VPSS_DIE_MODE_NODIE;
	stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;


	VPSS_CHN_ATTR_S stChnAttr = {0};
	VPSS_GRP_PARAM_S stVpssParam = {0};
	VPSS_NR_ADVANCED_PARAM_S stVpssAdvanParam = {0};

	VPSS_GRP_ATTR_S *pstVpssGrpAttr = &stGrpAttr;

	HI_MPI_VPSS_StopGrp( VpssGrp );
	HI_MPI_VPSS_DestroyGrp( VpssGrp );

	DBG_INFO("Create Vpssgrp(%d)\n",VpssGrp);

	/*** create vpss group ***/
	s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}


#if 0 // TODO : 170223 Read VPSS Group Attribute
	memset(&stGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
	s32Ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, &stGrpAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("HI_MPI_VPSS_GetGrpAttr failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	{
		DBG_INFO("u32MaxW 	= %d\n",stGrpAttr.u32MaxW);
		DBG_INFO("u32MaxH 	= %d\n",stGrpAttr.u32MaxH);
		DBG_INFO("bIeEn   	= %d\n",stGrpAttr.bIeEn);
		DBG_INFO("bNrEn   	= %d\n",stGrpAttr.bNrEn);
		DBG_INFO("enDieMode = %d\n",stGrpAttr.enDieMode);
		DBG_INFO("bHistEn   = %d\n",stGrpAttr.bHistEn);
		DBG_INFO("bEsEn   	= %d\n",stGrpAttr.bEsEn);
		DBG_INFO("bDciEn  	= %d\n",stGrpAttr.bDciEn);
		DBG_INFO("enPixFmt  = %d\n",stGrpAttr.enPixFmt);

		switch(stGrpAttr.enPixFmt)
		{
		case PIXEL_FORMAT_YUV_PLANAR_422 :
		case PIXEL_FORMAT_YUV_SEMIPLANAR_422 : DBG_INFO("Pixel Format : YUV 422\n");
			 break;
		case PIXEL_FORMAT_YUV_PLANAR_420 :
		case PIXEL_FORMAT_YUV_SEMIPLANAR_420 : DBG_INFO("Pixel Format : YUV 420\n");
			break;
		case PIXEL_FORMAT_YUV_PLANAR_444 :
		case PIXEL_FORMAT_YUV_SEMIPLANAR_444 : DBG_INFO("Pixel Format : YUV 444\n");
			break;

		default : break;
		}
	}
#endif

	/*** set vpss param ***/
	s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

#ifdef __FOR_HIK_DEMO_180208
    // TODO : 170223 Add VPSS Setting, same RAPTOR2 Init Setting Value
    stVpssParam.u32Contrast = 8;
    stVpssParam.u32DieStrength = 0;
    stVpssParam.u32SfStrength = 64;
    stVpssParam.u32TfStrength = 15;

    if(cable_dist < 4)
    {
    	stVpssParam.u32IeStrength = 4;
		stVpssParam.u32CfStrength = 12;
		stVpssParam.u32CTfStrength = 16;
    }
    else
    {
    	stVpssParam.u32IeStrength = 32;
		stVpssParam.u32CfStrength = 255;
		stVpssParam.u32CTfStrength = 32;
    }

    //  stVpssParam.u32CvbsStrength = 0;
    //  stVpssParam.u32DeMotionBlurring = 40;

	s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VPSS_GetGrpAdvancedParam(VpssGrp, &stVpssAdvanParam);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}	

	stVpssAdvanParam.u32Mdz = 63;
	stVpssAdvanParam.u32Edz = 63;
	stVpssAdvanParam.u32HTfRelaStrength = 96;
	stVpssAdvanParam.u32WTfRelaStrength = 50;

	s32Ret = HI_MPI_VPSS_SetGrpAdvancedParam(VpssGrp, &stVpssAdvanParam);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
#else
    // TODO : 170223 Add VPSS Setting, same RAPTOR2 Init Setting Value
    stVpssParam.u32Contrast = 8;
    stVpssParam.u32DieStrength = 0;
    stVpssParam.u32SfStrength = 15;
    stVpssParam.u32TfStrength = 63;

    if(cable_dist < 4)
    {
    	stVpssParam.u32IeStrength = 4;
    stVpssParam.u32CfStrength = 12;
    stVpssParam.u32CTfStrength = 16;
    }
    else
    {
    	stVpssParam.u32IeStrength = 32;
		stVpssParam.u32CfStrength = 255;
		stVpssParam.u32CTfStrength = 32;
    }

    //  stVpssParam.u32CvbsStrength = 0;
    //  stVpssParam.u32DeMotionBlurring = 40;

	s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);

	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

//	VPSS_FRAME_RATE_S stVpssFrmRate = {15, 30};
//	s32Ret = HI_MPI_VPSS_SetGrpFrameRate(VpssGrp, &stVpssFrmRate);
/*
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
*/

#endif
#if 0 // TODO : 170223 Read VPSS Param
	s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	{
		DBG_INFO("u32Contrast     	  = %d\n",stVpssParam.u32Contrast);
		DBG_INFO("u32DieStrength  	  = %d\n",stVpssParam.u32DieStrength);
		DBG_INFO("u32IeStrength   	  = %d\n",stVpssParam.u32IeStrength);
		DBG_INFO("u32SfStrength   	  = %d\n",stVpssParam.u32SfStrength);
		DBG_INFO("u32TfStrength   	  = %d\n",stVpssParam.u32TfStrength);
		DBG_INFO("u32CfStrength   	  = %d\n",stVpssParam.u32CfStrength);
		DBG_INFO("u32CTfStrength   	  = %d\n",stVpssParam.u32CTfStrength);
		DBG_INFO("u32DeMotionBlurring = %d\n",stVpssParam.u32DeMotionBlurring);
	}
#endif
	int j;
	/*** enable vpss chn, with frame ***/
	for(j=0; j<s32ChnCnt; j++)
	{
		VPSS_CHN VpssChn = j;
		/* Set Vpss Chn attr */
		stChnAttr.bSpEn = HI_FALSE;
		stChnAttr.bUVInvert = HI_FALSE;
		stChnAttr.bBorderEn = HI_FALSE;
		stChnAttr.stBorder.u32Color = 0xFFEC8B;
		stChnAttr.stBorder.u32LeftWidth = 2;
		stChnAttr.stBorder.u32RightWidth = 2;
		stChnAttr.stBorder.u32TopWidth = 2;
		stChnAttr.stBorder.u32BottomWidth = 2;

		s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
		if (s32Ret != HI_SUCCESS)
		{
			DBG_ERR("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
			return HI_FAILURE;
		}

		s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
		if (s32Ret != HI_SUCCESS)
		{
			DBG_ERR("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
			return HI_FAILURE;
		}
	}


#if 0 // TODO : 170223 Read VPSS Channel Attribute
	VPSS_CHN VpssChn = 1;
	memset(&stChnAttr,0,sizeof(VPSS_CHN_ATTR_S));
	s32Ret = HI_MPI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stChnAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("HI_MPI_VPSS_GetChnAttr failed with %#x\n", s32Ret);
		return HI_FAILURE;
	}

	{
		DBG_INFO("bSpEn	= %d\n",stChnAttr.bSpEn);
		DBG_INFO("bUVInvert	= %d\n",stChnAttr.bUVInvert);
		DBG_INFO("bBorderEn	= %d\n",stChnAttr.bBorderEn);
		DBG_INFO("stBorder.u32Color = %08x\n",stChnAttr.stBorder.u32Color);
		DBG_INFO("stBorder.u32LeftWidth = %d\n",stChnAttr.stBorder.u32LeftWidth);
		DBG_INFO("stBorder.u32RightWidth = %d\n",stChnAttr.stBorder.u32RightWidth);
		DBG_INFO("stBorder.u32TopWidth = %d\n",stChnAttr.stBorder.u32TopWidth);
		DBG_INFO("stBorder.u32BottomWidth = %d\n",stChnAttr.stBorder.u32BottomWidth);
	}
#endif

	/*** start vpss group ***/
	s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
		return HI_FAILURE;
	}

	return 0;
}

int NC_HI_VI_Init(void)
{
	int i;
	HI_U32 u32BlkSize;
	VB_CONF_S stVbConf;
	HI_U32 u32ViChnCnt   = 16;
	HI_S32 s32VpssGrpCnt = 16;
	HI_S32 s32Ret;

	memset(&stVbConf,0,sizeof(VB_CONF_S));
	u32BlkSize = NC_COMM_SYS_CalcPicVbBlkSize( 1920, 1080, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);
	stVbConf.u32MaxPoolCnt = 128;
	/*ddr0 video buffer*/
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = u32ViChnCnt * 8;  //tony test 01.14
	memset(stVbConf.astCommPool[0].acMmzName,0,sizeof(stVbConf.astCommPool[0].acMmzName));

	//SAMPLE_COMM_SYS_Init(&stVbConf);

	// Vi param
#if 0
	SAMPLE_VI_PARAM_S stViParam;
	SAMPLE_VI_PARAM_S *pstViParam = &stViParam;
	pstViParam->s32ViDevCnt = 8;
	pstViParam->s32ViDevInterval = 1;
	pstViParam->s32ViChnCnt = 8;
	pstViParam->s32ViChnInterval = 4;
#endif
	/*** Start VI Dev ***/
	/*** Start VI Chn ***/

	//NC_COMM_VI_BindVpss( pstViParam );

	// prebind vpss

#ifdef VPSS_DISABLE

	for(i=0;i<32;i++)
	{
		VO_DEV VoDev = VoDev_main;
		VO_CHN VoChn = i;
		VI_CHN ViChn = i;
		//NC_HI_Vpss_Start( VpssGrp, 1280, 720 ); // -> go to when SetChannelFormat

		s32Ret = NC_COMM_VO_BindVI( VoDev, VoChn , ViChn);
		if (HI_SUCCESS != s32Ret)
		{
			DBG_ERR("failed!\n");
			return HI_FAILURE;
		}
	}
	printf(" VPSS Function Disable \n");

#else

	for(i=0;i<32;i++)
	{
		VO_DEV VoDev = VoDev_main;
		VO_CHN VoChn = i;
		VPSS_GRP VpssGrp = i;
		VPSS_CHN VpssChn_VoHD0 = VPSS_CHN3;

		//NC_HI_Vpss_Start( VpssGrp, 1280, 720 ); // -> go to when SetChannelFormat

		s32Ret = NC_COMM_VO_BindVpss( VoDev, VoChn, VpssGrp, VpssChn_VoHD0 );
		if (HI_SUCCESS != s32Ret)
		{
			DBG_ERR("failed!\n");
			return HI_FAILURE;
		}
	}

#endif

	DBG_INFO("#2\n");

	return HI_SUCCESS;
}

//HI3531A All Device, Channel Clear
void NC_HI_VI_Dev_Chn_Clear(void)
{
	int i;

	for(i=0;i<32;i++)
		HI_MPI_VI_DisableChn(i);

	for(i=0;i<8;i++)
		HI_MPI_VI_DisableDev(i);
}

int NC_HI_VO_Init_SK_TEST( VO_PUB_ATTR_S *pPubAttr )
{
	HI_S32 s32Ret = HI_SUCCESS;
    pthread_t phifb0 = -1;
	pthread_t phifb1 = -1;

    HI_U32 u32PicWidth;
    HI_U32 u32PicHeight;
    SIZE_S  stSize;

	VO_LAYER VoLayer = 0;
    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    HI_U32 u32VoFrmRate;

    VB_CONF_S       stVbConf;
	HI_U32 u32BlkSize;

	memcpy( &stPubAttr, pPubAttr, sizeof( VO_PUB_ATTR_S ));

	// Get Resolution First!!
	memset(&stSize,0,sizeof(SIZE_S));
	s32Ret = NC_HI_VO_GetWH(stPubAttr.enIntfSync,&stSize.u32Width,\
		&stSize.u32Height,&u32VoFrmRate);
	if (HI_SUCCESS != s32Ret)
	{
		DBG_ERR("get vo wh failed with %d!\n", s32Ret);
		goto SAMPLE_HIFB_NoneBufMode_0;
	}

	u32PicWidth = stSize.u32Width;
	u32PicHeight = stSize.u32Height;

	if((stPubAttr.enIntfSync == VO_OUTPUT_3840x2160_30) || (stPubAttr.enIntfSync == VO_OUTPUT_2560x1600_60))
	{
		_FB_Width = u32PicWidth;
		_FB_Height = u32PicHeight;
	}
	else
	{
		_FB_Width = 1920;
		_FB_Height = 1080;
	}

	DBG_INFO("VO format Size : w %d x  h %d\n",stSize.u32Width,stSize.u32Height);

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    //u32BlkSize = CEILING_2_POWER(u32PicWidth,SAMPLE_SYS_ALIGN_WIDTH)\
    //    * CEILING_2_POWER(u32PicHeight,SAMPLE_SYS_ALIGN_WIDTH) *2;
    u32BlkSize = CEILING_2_POWER(3840,SAMPLE_SYS_ALIGN_WIDTH)\
        * CEILING_2_POWER(2160,SAMPLE_SYS_ALIGN_WIDTH) *2;

    stVbConf.u32MaxPoolCnt = 128;

    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt =  32;

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = NC_HI_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    /******************************************
     step 3:  start vo hd0.
    *****************************************/
    s32Ret = HI_MPI_VO_UnBindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev_main);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("UnBindGraphicLayer failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    s32Ret = HI_MPI_VO_BindGraphicLayer(GRAPHICS_LAYER_HC0, VoDev_main);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("BindGraphicLayer failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }
    //stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    //stPubAttr.enIntfType = VO_INTF_HDMI;
	//stPubAttr.u32BgColor = 0x0000FF;

    stLayerAttr.bClusterMode = HI_FALSE;
	stLayerAttr.bDoubleFrame = HI_FALSE;
//	stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;

    s32Ret = NC_HI_VO_GetWH(stPubAttr.enIntfSync,&stSize.u32Width,\
        &stSize.u32Height,&u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("get vo wh failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }
    memcpy(&stLayerAttr.stImageSize,&stSize,sizeof(stSize));

	s32Ret = NC_HI_VO_StartDev(VoDev_main, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("start vo dev failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
	}

    stLayerAttr.u32DispFrmRt = 30 ;
	stLayerAttr.stDispRect.s32X = 0;
	stLayerAttr.stDispRect.s32Y = 0;
	stLayerAttr.stDispRect.u32Width = stSize.u32Width;
	stLayerAttr.stDispRect.u32Height = stSize.u32Height;

    s32Ret = NC_HI_VO_StartLayer(VoLayer, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
    	DBG_ERR("start vo layer failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_1;
	}

    VoLayer_main = VoLayer;

    if (stPubAttr.enIntfType & VO_INTF_HDMI)
    {
        s32Ret = NC_HI_VO_HdmiStart(stPubAttr.enIntfSync);
        if (HI_SUCCESS != s32Ret)
        {
        	DBG_ERR("start HDMI failed with %d!\n", s32Ret);
            goto SAMPLE_HIFB_NoneBufMode_2;
    	}
    }

    DBG_INFO("VO Initialized\n");

    _VO_Init_f = 1;

    return 0;

SAMPLE_HIFB_NoneBufMode_2:
	NC_HI_VO_StopLayer(VoLayer);
SAMPLE_HIFB_NoneBufMode_1:
	NC_HI_VO_StopDev(VoDev_main);
SAMPLE_HIFB_NoneBufMode_0:
	NC_HI_SYS_Exit();

	return s32Ret;
}

int NC_HI_VI_Init_SK_TEST(void)
{
	int i;
	HI_U32 u32BlkSize;
	VB_CONF_S stVbConf;
	HI_U32 u32ViChnCnt   = 16;
	HI_S32 s32VpssGrpCnt = 16;
	HI_S32 s32Ret;

	memset(&stVbConf,0,sizeof(VB_CONF_S));
	u32BlkSize = NC_COMM_SYS_CalcPicVbBlkSize( 1280, 720, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);
	stVbConf.u32MaxPoolCnt = 128;
	/*ddr0 video buffer*/
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = u32ViChnCnt * 8;  //tony test 01.14
	memset(stVbConf.astCommPool[0].acMmzName,0,sizeof(stVbConf.astCommPool[0].acMmzName));

	//SAMPLE_COMM_SYS_Init(&stVbConf);

	// Vi param
#if 0
	SAMPLE_VI_PARAM_S stViParam;
	SAMPLE_VI_PARAM_S *pstViParam = &stViParam;
	pstViParam->s32ViDevCnt = 8;
	pstViParam->s32ViDevInterval = 1;
	pstViParam->s32ViChnCnt = 8;
	pstViParam->s32ViChnInterval = 4;
#endif
	/*** Start VI Dev ***/
	/*** Start VI Chn ***/

	//NC_COMM_VI_BindVpss( pstViParam );

	// prebind vpss
	for(i=0;i<32;i++)
	{
		VO_DEV VoDev = VoDev_main;
		VO_CHN VoChn = i;
		VPSS_GRP VpssGrp = i;
		VPSS_CHN VpssChn_VoHD0 = VPSS_CHN3;

		//NC_HI_Vpss_Start( VpssGrp, 1280, 720 ); // -> go to when SetChannelFormat

		s32Ret = NC_COMM_VO_BindVpss( VoDev, VoChn, VpssGrp, VpssChn_VoHD0 );
		if (HI_SUCCESS != s32Ret)
		{
			DBG_ERR("failed!\n");
			return HI_FAILURE;
		}
	}

	DBG_INFO("#2\n");

	return HI_SUCCESS;
}

static VI_DEV_ATTR_S DEV_ATTR_DEFAULT_SK_TEST = // DEV_ATTR_BT656_P_1MUX
{

    VI_MODE_BT656,

    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* 崗占쏙옙占쏙옙占쏙옙珂占쏙옙占쏙옙占쏙옙占쏙옙 */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 占쏙옙連占쏙옙YUV占쏙옙駕*/
    VI_INPUT_DATA_YVYU,
    /*谿占쏙옙占쏙옙口占쏙옙占쏙옙壇reg占쌍뀐옙占쏙옙占쏙옙占쏙옙占쏙옙占�? --bt1120珂占쏙옙占쏙옙槻*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,


    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*賈占쏙옙占쌘뀐옙ISP*/
    VI_PATH_BYPASS,
    /*占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙*/
    VI_DATA_TYPE_YUV
};

int NC_HI_VI_Port_SetFormat_SK_TEST( VI_DEV ViDev, NC_HI_VI_PORT_ATTR_S *pPortAttr )
{
	HI_S32 s32Ret;
	VI_DEV_ATTR_S stViDevAttr;

	DBG_INFO("dev (%d)\n",ViDev);

	memset(&stViDevAttr,0,sizeof(stViDevAttr));
	if( pPortAttr->Interface == VI_MODE_BT1120_STANDARD )
	{
		memcpy(&stViDevAttr, &DEV_ATTR_1120_DEFAULT, sizeof(stViDevAttr));
	}
	else
	{
		memcpy(&stViDevAttr, &DEV_ATTR_DEFAULT_SK_TEST, sizeof(stViDevAttr));
	}

	stViDevAttr.enClkEdge = pPortAttr->ClkEdge;
	stViDevAttr.enIntfMode = pPortAttr->Interface;
	stViDevAttr.enWorkMode = pPortAttr->Multiplex;

	NC_COMM_VI_SetMask(ViDev,&stViDevAttr);

	s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_EnableDev(ViDev);
	if (s32Ret != HI_SUCCESS)
	{
		DBG_ERR("failed %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	return 0;
}
