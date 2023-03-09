#ifndef __NC_HI_VIVO_H__
#define __NC_HI_VIVO_H__

#include "nc_hi_common.h"

#include <api/video_fmt_info.h>

typedef struct NC_HI_MUX_ATTR_S
{
	int w; // column
	int h; // row
	int single_en; //
	int single_ch; //
	int single_orgsize_en; //
	int single_orgsize_possible; //
	int single_mid_screen;		//add 170209 13:38
	int single_mid_screen_on;	//add 170209 13:38
	int flag;
} NC_HI_MUX_ATTR_S;

enum
{
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
};

enum
{
	DEV_0 = 0,
	DEV_1,
	DEV_2,
	DEV_3,
	DEV_4,
	DEV_5,
};

typedef struct NC_HI_VI_STAT_S
{
	int status;

	int vd_dev;
	int vd_ch;

	NC_VI_CH_FMT_S *pChFmt;

} NC_HI_VI_STAT_S;

/*
 * bt656, bt1120
 * edge : single , dual
 * vclk freq
 * multiplex
 */
typedef struct NC_HI_VI_PORT_ATTR_S
{
	VI_INTF_MODE_E Interface; // bt656, bt1120
	VI_WORK_MODE_E Multiplex; // multiplex
	VI_CLK_EDGE_E ClkEdge; // single, dual edge
	int ClkFreq; // VCLK freq.
} NC_HI_VI_PORT_ATTR_S;

#define SoC_MAX_PORT_NUM 8

void NC_HI_VI_Dev_Chn_Clear(void);
NC_HI_VI_STAT_S *NC_HI_VI_Channel_GetStatus( VI_CHN ViChn );
HI_S32 NC_HI_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32 *pu32W,HI_U32 *pu32H, HI_U32 *pu32Frm);
HI_S32 NC_HI_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
HI_S32 NC_HI_VO_StopDev(VO_DEV VoDev);
HI_S32 NC_HI_VO_StartLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 NC_HI_VO_StopLayer(VO_LAYER VoLayer);
HI_S32 NC_HI_VO_StartChn(VO_LAYER VoLayer, VO_MODE_MUX enMode);
HI_S32 NC_HI_VO_StopChn(VO_LAYER VoLayer, VO_MODE_MUX enMode);
HI_S32 NC_HI_VO_HdmiStop(HI_VOID);
HI_S32 NC_HI_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
static HI_VOID NC_HI_VO_HdmiConvertSync(VO_INTF_SYNC_E enIntfSync, HI_HDMI_VIDEO_FMT_E *penVideoFmt);

HI_S32 NC_COMM_VO_BindVI(VO_LAYER VoLayer,VO_CHN VoChn,VI_CHN ViChn);

#endif
