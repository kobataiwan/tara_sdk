#ifndef __SOC_API_H__
#define __SOC_API_H__

#include "nc_hi_common.h"

typedef unsigned char uint8;
typedef unsigned short  uint16;
typedef unsigned int uint32;

typedef signed char int8;
typedef signed short  int16;
typedef signed int int32;

typedef struct NC_HI_HDMI_ATTR_S
{
	int u32Luma;
	int u32Hue;
	int u32Saturation;
	int u32Contrast;
	int enCscMatrix;

} NC_HI_HDMI_ATTR_S;

void SOC_Display_Mux_SetDiv( int w, int h  );

void SOC_Audio_Init( );
void SOC_Audio_Init_NewCh(int ch, int sample_rate);
void SOC_Audio_Destroy(void);

HI_S32 SOC_HDMI_Output_Color_Set( NC_HI_HDMI_ATTR_S pParam );
HI_S32 SOC_HDMI_Output_Color_Get( NC_HI_HDMI_ATTR_S *pParam );
HI_S32 SOC_HDMI_Output_SelGRP_Color_Set( int vo_dev, NC_HI_HDMI_ATTR_S *pParam );
HI_S32 SOC_HDMI_Output_SelGRP_Color_Get( int vo_dev, NC_HI_HDMI_ATTR_S *pParam );

HI_S32 SOC_VPSS_SetGrp_Param_Set( VPSS_GRP_PARAM_S *pParam );
HI_S32 SOC_VPSS_SelGRP_Param_Set( int Grp, VPSS_GRP_PARAM_S *pParam );
HI_S32 SOC_VPSS_SetGrp_Param_Get( VPSS_GRP_PARAM_S *pParam );
HI_S32 SOC_VPSS_SelGRP_Param_Get( int Grp, VPSS_GRP_PARAM_S *pParam );
HI_S32 SOC_VPSS_SelGRP_Attr_Get( int grp, VPSS_GRP_ATTR_S *pParam );
HI_S32 SOC_VPSS_SelGRP_Attr_Set( int grp, VPSS_GRP_ATTR_S *pParam );

#endif
