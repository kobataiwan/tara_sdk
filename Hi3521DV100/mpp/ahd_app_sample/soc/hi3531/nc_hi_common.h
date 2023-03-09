#ifndef __NC_HI_COMMON_H__
#define __NC_HI_COMMON_H__

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vda.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_ao.h"
#include "hi_comm_aio.h"
#include "hi_comm_hdmi.h"
#include "hi_defines.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "mpi_vdec.h"
#include "mpi_vda.h"
#include "mpi_region.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_hdmi.h"

#ifdef SUPPORT_3520D_6158C
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420
#else
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_422
#endif

#define TW2865_FILE "/dev/tw2865dev"
#define TP2823_FILE "/dev/tp2823dev"
#define TLV320_FILE "/dev/tlv320aic31"

#define SAMPLE_VO_DEV_DHD0 0
#define SAMPLE_VO_DEV_DHD1 1
#define SAMPLE_VO_DEV_DSD0 2
#define SAMPLE_VO_DEV_VIRT0 3
#define SAMPLE_VO_DEV_DSD1 -1

#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VPIP 2
#define SAMPLE_VO_LAYER_VSD0 3
#define SAMPLE_VO_LAYER_VIRT0 4


#define VO_LAYER_PIP      2
#define VO_LAYER_PIP_STA	2
#define VO_LAYER_PIP_END	2
#define VO_DEV_HD_END	2


#define SAMPLE_VO_WBC_BASE  0
#define SAMPLE_VO_LAYER_PRIORITY_BASE 0
#define SAMPLE_VO_LAYER_PRIORITY_PIP 1
//#define GRAPHICS_LAYER_HC0 3

#define SAMPLE_AUDIO_PTNUMPERFRM   320
#define SAMPLE_AUDIO_TLV320_AI_DEV 2
#define SAMPLE_AUDIO_TLV320_AO_DEV 0


#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 2
#define SAMPLE_AUDIO_HDMI_AO_DEV 1
#define SAMPLE_AUDIO_AI_DEV 1
#define SAMPLE_AUDIO_AO_DEV 0

#define SAMPLE_CIF_H264_PATH "../common/CIF.h264"
#define SAMPLE_1080P_H264_PATH "../common/1080P.h264"
#define SAMPLE_1080P_H265_PATH "../common/1080P.h265"
#define SAMPLE_4K_H264_PATH "../common/tmp1"
#define SAMPLE_4K_H265_PATH "../common/tmp2"
#define SAMPLE_1080P_MPEG4_PATH "../common/1080P.mpeg4"
#define SAMPLE_FIELD_H264_PATH "../common/D1_field.h264"
#define SAMPLE_1080P_JPEG_PATH "../common/1080P.jpg"
#define SAMPLE_4K_JPEG_PATH "../common/tmp3"

#define SAMPLE_MAX_VDEC_CHN_CNT 8


#define ALIGN_UP(x, a)              ((x+a-1)&(~(a-1)))
#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))

#define SAMPLE_SYS_ALIGN_WIDTH  16
#define VO_BKGRD_BLUE           0x0000FF

#define HD_WIDTH                1920
#define HD_HEIGHT               1080
#define _4K_WIDTH                3840
#define _4K_HEIGHT               2160

#define D1_WIDTH                720
#define D1_HEIGHT               576

#define _720P_WIDTH              1280
#define _720P_HEIGHT             720

typedef enum _VO_MODE_MUX
{
    VO_MODE_1MUX  = 0,
    VO_MODE_4MUX = 1,
    VO_MODE_9MUX = 2,
    VO_MODE_16MUX = 3,
    VO_MODE_BUTT
}VO_MODE_MUX;

typedef struct _VI_PARAMS_S
{
    HI_S32 s32ViDevCnt;        // VI Dev Total Count
    HI_S32 s32ViDevInterval;   // Vi Dev Interval
    HI_S32 s32ViChnCnt;        // Vi Chn Total Count
    HI_S32 s32ViChnInterval;   // VI Chn Interval
}VI_PARAMS_S;


#endif
