/******************************************************************************

  Copyright (C), 2001-2012, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_venc.h
  Version       : 
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2006/11/24
  Last Modified :
  Description   : common struct definition for VENC
  Function List :
  History       :
******************************************************************************/
#ifndef __HI_COMM_VENC_H__
#define __HI_COMM_VENC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"
#include "hi_comm_rc.h"



/* invlalid channel ID */
#define HI_ERR_VENC_INVALID_CHNID     HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_VENC_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_VENC_EXIST             HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define HI_ERR_VENC_UNEXIST           HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_VENC_NULL_PTR          HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_VENC_NOT_CONFIG        HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_VENC_NOT_SUPPORT       HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_VENC_NOT_PERM          HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_VENC_NOMEM             HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_VENC_NOBUF             HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_VENC_BUF_EMPTY         HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_VENC_BUF_FULL          HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HI_ERR_VENC_SYS_NOTREADY      HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define HI_ERR_VENC_BUSY              HI_DEF_ERR(HI_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)


/*the nalu type of H264E*/
typedef enum hiH264E_NALU_TYPE_E
{
     H264E_NALU_PSLICE = 1,                         /*PSLICE types*/
     H264E_NALU_ISLICE = 5,                         /*ISLICE types*/
     H264E_NALU_SEI    = 6,                         /*SEI types*/
     H264E_NALU_SPS    = 7,                         /*SPS types*/
     H264E_NALU_PPS    = 8,                         /*PPS types*/  
     H264E_NALU_IPSLICE = 9,
     H264E_NALU_BUTT        
} H264E_NALU_TYPE_E;

/*the nalu type of H265E*/
typedef enum hiH265E_NALU_TYPE_E
{
	 H265E_NALU_PSLICE = 1,                         /*P SLICE types*/
	 H265E_NALU_ISLICE = 19,                         /*I SLICE types*/
     H265E_NALU_VPS    = 32,                         /*VPS types*/
     H265E_NALU_SPS    = 33,                         /*SPS types*/
     H265E_NALU_PPS    = 34,                         /*PPS types*/
     H265E_NALU_SEI    = 39,                         /*SEI types*/
     
     H265E_NALU_BUTT        
} H265E_NALU_TYPE_E;

/*the reference type of H264E slice*/
typedef enum hiH264E_REFSLICE_TYPE_E
{
     H264E_REFSLICE_FOR_1X = 1,                     /*Reference slice for H264E_REF_MODE_1X*/
     H264E_REFSLICE_FOR_2X = 2,                     /*Reference slice for H264E_REF_MODE_2X*/
     H264E_REFSLICE_FOR_4X = 5,                     /*Reference slice for H264E_REF_MODE_4X*/
     H264E_REFSLICE_FOR_BUTT                        /* slice not for reference*/          
} H264E_REFSLICE_TYPE_E;

/*the pack type of JPEGE*/
typedef enum hiJPEGE_PACK_TYPE_E
{
     JPEGE_PACK_ECS = 5,                            /*ECS types*/
     JPEGE_PACK_APP = 6,                            /*APP types*/
     JPEGE_PACK_VDO = 7,                            /*VDO types*/
     JPEGE_PACK_PIC = 8,                            /*PIC types*/
     JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/*the pack type of MPEG4*/
typedef enum hiMPEG4E_PACK_TYPE_E
{
     MPEG4E_PACK_VOP_P = 1,                         /*P VOP packet types*/
     MPEG4E_PACK_VOP_I = 5,                         /*I VOP packet types*/
     MPEG4E_PACK_VOS   = 6,                         /*VOS types*/
     MPEG4E_PACK_VO    = 7,                         /*VO types*/
     MPEG4E_PACK_VOL   = 8,                         /*VOL types*/
     MPEG4E_PACK_GVOP  = 9                          /*GROUP of vop types */
} MPEG4E_PACK_TYPE_E;

/*the data type of VENC*/
typedef union hiVENC_DATA_TYPE_U
{
    H264E_NALU_TYPE_E    enH264EType;               /*H264E NALU types*/
    JPEGE_PACK_TYPE_E    enJPEGEType;               /*JPEGE pack types*/
    MPEG4E_PACK_TYPE_E   enMPEG4EType;              /*MPEG4E pack types*/
    H265E_NALU_TYPE_E    enH265EType;               /*H264E NALU types*/
}VENC_DATA_TYPE_U;

typedef struct hiVENC_PACK_INFO_S
{
	VENC_DATA_TYPE_U  u32PackType;
    HI_U32 u32PackOffset;                          
    HI_U32 u32PackLength;                        
}VENC_PACK_INFO_S;


typedef struct hiVENC_PACK_S
{
    HI_U32   u32PhyAddr;                         /*the physics address of stream*/
    HI_U8   *pu8Addr;                            /*the virtual address of stream*/
    HI_U32   u32Len;                             /*the length of stream*/
    
    HI_U64   u64PTS;                             /*PTS*/
    HI_BOOL  bFrameEnd;                          /*frame end*/
    
    VENC_DATA_TYPE_U  DataType;                  /*the type of stream*/
    HI_U32   u32Offset;

	HI_U32 u32DataNum;
	VENC_PACK_INFO_S stPackInfo[8];
}VENC_PACK_S;

typedef enum hiH264E_REF_TYPE_E
{
    BASE_IDRSLICE = 0,                              /*the Idr frame at Base layer*/
    BASE_REFTOIDR = 1,
    BASE_PSLICE_REFBYBASE,                          /*the P frame at Base layer, referenced by other frames at Base layer*/
    BASE_PSLICE_REFBYENHANCE,                       /*the P frame at Base layer, referenced by other frames at Enhance layer*/
    ENHANCE_PSLICE_REFBYENHANCE,               		/*the P frame at Enhance layer, referenced by other frames at Enhance layer*/
    ENHANCE_PSLICE_NOTFORREF,                       /*the P frame at Enhance layer ,not referenced*/
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum hiH264E_REF_TYPE_E H265E_REF_TYPE_E;

typedef struct hiVENC_STREAM_INFO_H264_S
{    
    HI_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    HI_U32 u32PSkipMbNum;                           /* the skip macroblock num */
    HI_U32 u32IpcmMbNum;                            /* the ipcm macroblock num */
    HI_U32 u32Inter16x8MbNum;                       /* the inter16x8 macroblock num */
    HI_U32 u32Inter16x16MbNum;                      /* the inter16x16 macroblock num */
    HI_U32 u32Inter8x16MbNum;                       /* the inter8x16 macroblock num */
    HI_U32 u32Inter8x8MbNum;                        /* the inter8x8 macroblock num */
    HI_U32 u32Intra16MbNum;                         /* the intra16x16 macroblock num */
    HI_U32 u32Intra8MbNum;                          /* the intra8x8 macroblock num */
    HI_U32 u32Intra4MbNum;                          /* the inter4x4 macroblock num */

    H264E_REFSLICE_TYPE_E enRefSliceType;           /*the reference type of H264E slice*/
    H264E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    HI_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/	
	HI_U32 u32StartQp;								/*StartQP Value*/
}VENC_STREAM_INFO_H264_S;

typedef struct hiVENC_STREAM_INFO_H265_S
{    
    HI_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    HI_U32 u32Inter64x64CuNum;                      /* the Inter64x64 Cu num */
    HI_U32 u32Inter32x32CuNum;						/* the Inter32x32 Cu num */
    HI_U32 u32Inter16x16CuNum;						/* the Inter16x16 Cu num */
    HI_U32 u32Inter8x8CuNum;						/* the Inter8x8 Cu num */
    HI_U32 u32Intra32x32CuNum;						/* the Intra32x32 Cu num */
    HI_U32 u32Intra16x16CuNum;						/* the Intra16x16 Cu num */
    HI_U32 u32Intra8x8CuNum;						/* the Intra8x8 Cu num */
    HI_U32 u32Intra4x4CuNum;						/* the Intra4x4 Cu num */

    H265E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    HI_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/	
	HI_U32 u32StartQp;								/*StartQP Value*/
}VENC_STREAM_INFO_H265_S;

typedef struct hiVENC_STREAM_INFO_JPEG_S
{
    HI_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    HI_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
    HI_U32 u32Qfactor;                              /* image quality */
}VENC_STREAM_INFO_JPEG_S;

typedef struct hiVENC_STREAM_INFO_MPEG4_S
{
    HI_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    HI_U32 u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
}VENC_STREAM_INFO_MPEG4_S;

typedef struct hiVENC_STREAM_S
{
    VENC_PACK_S *pstPack;                           /*stream pack attribute*/
    HI_U32      u32PackCount;                       /*the pack number of one frame stream*/
    HI_U32      u32Seq;                             /*the list number of stream*/

    union
    {
        VENC_STREAM_INFO_H264_S  stH264Info;         /*the stream info of h264*/
        VENC_STREAM_INFO_JPEG_S  stJpegInfo;         /*the stream info of jpeg*/
        VENC_STREAM_INFO_MPEG4_S stMpeg4Info;       /*the stream info of mpeg4*/
        VENC_STREAM_INFO_H265_S  stH265Info;        /*the stream info of h265*/
    };
}VENC_STREAM_S;

typedef struct hiVENC_ATTR_H264_S
{
    HI_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    HI_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/

    HI_U32  u32BufSize;                             /*stream buffer size*/
    HI_U32  u32Profile;                             /*0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3]; */          
    HI_BOOL bByFrame;                               /*get stream mode is slice mode or frame mode*/
	

    HI_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    HI_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/    

    HI_U32  u32BFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    HI_U32  u32RefNum;                              /* 0: default; number of refrence frame*/

}VENC_ATTR_H264_S;

typedef struct hiVENC_ATTR_H265_S
{
    HI_U32  u32MaxPicWidth;     					/*maximum width of a picture to be encoded, in pixel*/
    HI_U32  u32MaxPicHeight;    					/*maximum height of a picture to be encoded, in pixel*/
    
    HI_U32  u32BufSize;         					/*stream buffer size*/
    HI_U32  u32Profile;         					/*0: MP */          
    HI_BOOL bByFrame;           					/*get stream mode is slice mode or frame mode*/

    
    HI_U32  u32PicWidth;        					/*width of a picture to be encoded, in pixel*/
    HI_U32  u32PicHeight;       					/*height of a picture to be encoded, in pixel*/
    
    HI_U32  u32BFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    HI_U32  u32RefNum;                              /* 0: default; number of refrence frame*/
}VENC_ATTR_H265_S;

typedef struct hiVENC_ATTR_MJPEG_S
{
    HI_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    HI_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    HI_U32  u32BufSize;                             /*stream buffer size*/
    HI_BOOL bByFrame;                               /*get stream mode is field mode  or frame mode*/

	
    HI_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    HI_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/ 
    
}VENC_ATTR_MJPEG_S;

typedef struct hiVENC_ATTR_JPEG_S 
{
    HI_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    HI_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    HI_U32  u32BufSize;                             /*stream buffer size*/
    HI_BOOL bByFrame;                               /*get stream mode is field mode  or frame mode*/
	

    HI_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    HI_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
    HI_BOOL  bSupportDCF;                           /*support dcf*/
    
}VENC_ATTR_JPEG_S;

/* attribute of MPEG4*/
typedef struct  hiVENC_ATTR_MPEG4_S
{
    HI_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    HI_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/    

    HI_U32  u32BufSize;                             /*buffer size*/
    HI_BOOL bByFrame;                               /*get stream mode is pack or frame*/
	

    HI_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    HI_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
}VENC_ATTR_MPEG4_S;

typedef struct hiVENC_ATTR_S
{
    PAYLOAD_TYPE_E  enType;                         /*the type of payload*/
    union
    {
        VENC_ATTR_H264_S  stAttrH264e;              /*attributes of h264*/
        VENC_ATTR_MJPEG_S stAttrMjpeg;              /*attributes of mjpeg*/
        VENC_ATTR_JPEG_S  stAttrJpeg;               /*attributes of jpeg*/
        VENC_ATTR_MPEG4_S stAttrMpeg4;              /*attributes of mpeg4*/
        VENC_ATTR_H265_S  stAttrH265e;              /*attributes of h265*/
    };    
}VENC_ATTR_S;

typedef struct hiVENC_CHN_ATTR_S
{
    VENC_ATTR_S stVeAttr;                           /*the attribute of video encoder*/
    VENC_RC_ATTR_S stRcAttr;                        /*the attribute of rate  ctrl*/
}VENC_CHN_ATTR_S;

typedef struct hiVENC_CHN_STAT_S
{
    HI_U32 u32LeftPics;                             /*left picture number */
    HI_U32 u32LeftStreamBytes;                      /*left stream bytes*/
    HI_U32 u32LeftStreamFrames;                     /*left stream frames*/
    HI_U32 u32CurPacks;                             /*pack number of current frame*/
    HI_U32 u32LeftRecvPics;                         /*Number of frames to be received. This member is valid after HI_MPI_VENC_StartRecvPicEx is called.*/
    HI_U32 u32LeftEncPics;                          /*Number of frames to be encoded. This member is valid after HI_MPI_VENC_StartRecvPicEx is called.*/
}VENC_CHN_STAT_S;



typedef struct hiVENC_PARAM_H264_SLICE_SPLIT_S
{
    HI_BOOL bSplitEnable;                           /*slice split enable, HI_TRUE:enable, HI_FALSE:diable, default value:HI_FALSE*/
    HI_U32  u32SplitMode;                           /*0:bit number, 1:mb line number, >=2:no meaning*/
    HI_U32  u32SliceSize;                           /*when the splitmode is 0, this value presents the bitsnum of one slice average, 
                                                      when the splitmode is 1, this value presents the mb num of one slice*/
} VENC_PARAM_H264_SLICE_SPLIT_S;

typedef struct hiVENC_PARAM_H264_INTER_PRED_S
{
    /* search window */
    HI_U32 u32HWSize;                              /* size of horizontal search window.
                                                      default value: differ with the picture size */
    HI_U32 u32VWSize;                              /* size of vertical search window.
                                                    default value: differ with the picture size */
                                    
    /* inter pred, one of the following 4 kinds of inter prediction mode must be enable */
    HI_BOOL bInter16x16PredEn;                     /*default: HI_TRUE, enable 16x16 prediction*/
    HI_BOOL bInter16x8PredEn;                      /*default: HI_TRUE*/
    HI_BOOL bInter8x16PredEn;                      /*default: HI_TRUE*/
    HI_BOOL bInter8x8PredEn;                       /*default: HI_TRUE*/
    HI_BOOL bExtedgeEn;                            /*default: HI_TRUE*/
} VENC_PARAM_H264_INTER_PRED_S;

typedef struct hiVENC_PARAM_H264_INTRA_PRED_S
{
    /* intra pred, one of following 2 kinds of intra pred mode must be enable */                            
    HI_BOOL bIntra16x16PredEn;                     /*default: HI_TRUE, enable 16x16 intra prediction*/           
    HI_BOOL bIntraNxNPredEn;                       /*default: HI_TRUE, enable 4x4 and/or 8x8 prediction
                                                     Notes: this value must work with IntraTransMode*/
    HI_U32 constrained_intra_pred_flag;            /*default: HI_FALSE, see the H.264 protocol*/
    HI_BOOL bIpcmEn;                               /*default: HI_TRUE, enable ipcm*/
}VENC_PARAM_H264_INTRA_PRED_S;

typedef struct hiVENC_PARAM_H264_TRANS_S
{
    HI_U32 u32IntraTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    HI_U32 u32InterTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    
    HI_BOOL bScalingListValid;                     /* default: HI_FALSE */
                                                   /* Notes: Enable IntraScalingList8X8 and InterScalingList8X8 while bScalingListValid is HI_TRUE */
    HI_U8  InterScalingList8X8[64];                
    HI_U8  IntraScalingList8X8[64];                
    
    HI_S32 chroma_qp_index_offset;                 /* [-12,12],default value: 0*/
}VENC_PARAM_H264_TRANS_S;

typedef struct hiVENC_PARAM_H264_ENTROPY_S
{
    HI_U32 u32EntropyEncModeI;                     /* 0:cavlc, 1:cabac */
    HI_U32 u32EntropyEncModeP;                     /* 0:cavlc, 1:cabac */
    HI_U32 cabac_stuff_en;                         /* default: 0*/
    HI_U32 Cabac_init_idc;                         /* 0~2 */
}VENC_PARAM_H264_ENTROPY_S;

typedef struct hiVENC_PARAM_H264_POC_S
{
    HI_U32 pic_order_cnt_type;                     /* default value: 2. {0,1,2} */

}VENC_PARAM_H264_POC_S;

typedef struct hiVENC_PARAM_H264_DBLK_S
{
    HI_U32 disable_deblocking_filter_idc;          /* default value: 0. {0,1,2} */
    HI_S32 slice_alpha_c0_offset_div2;             /* default value: 5. [-6,+6] */
    HI_S32 slice_beta_offset_div2;                 /* default value: 5. [-6,+6] */
}VENC_PARAM_H264_DBLK_S;


typedef struct hiVENC_PARAM_H264_VUI_TIME_INFO_S
{
    HI_U8  timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    HI_U8  fixed_frame_rate_flag;                  /* default value: n/a. {0,1} */
    HI_U32 num_units_in_tick;                      /* default value: n/a. > 0. */
    HI_U32 time_scale;                             /* default value: n/a. > 0. */

}VENC_PARAM_VUI_H264_TIME_INFO_S;

typedef struct hiVENC_PARAM_VUI_ASPECT_RATIO_S
{
    HI_U8  aspect_ratio_info_present_flag;        /* default value: 0. If 1, aspectratio info belows will be encoded into vui. {0,1} */
    HI_U8  aspect_ratio_idc;                      /* default value: n/a. [0,255],17~254 is reserved. */
	HI_U8  overscan_info_present_flag;			  /* default value: 0,just be 0.If 1, oversacan info belows will be encoded into vui. {0,1} */
	HI_U8  overscan_appropriate_flag;			  /* default value: n/a. */
	HI_U16 sar_width;                             /* default value: n/a. sar_width  and  sar_height  shall  be  relatively  prime. */
    HI_U16 sar_height ;                  		  /* default value: n/a.  */
}VENC_PARAM_VUI_ASPECT_RATIO_S;


typedef struct hiVENC_PARAM_VUI_VIDEO_SIGNAL_S
{
    HI_U8  video_signal_type_present_flag ;        /* default value: 0. If 1, video singnal info will be encoded into vui. {0,1} */
    HI_U8  video_format ;                          /* default value: n/a. >= 0. */
    HI_U8  video_full_range_flag;                  /* default value: n/a. {0,1}. */
    HI_U8  colour_description_present_flag ;       /* default value: n/a. {0,1} */
	HI_U8  colour_primaries ;					   /* default value: n/a. [0,255],0/9~255 is reserved */
	HI_U8  transfer_characteristics;			   /* default value: n/a. [0,255],0/13~255 is reserved */
	HI_U8  matrix_coefficients; 				   /* default value: n/a. [0,255],9~255 is reserved */		
}VENC_PARAM_VUI_VIDEO_SIGNAL_S;



typedef struct hiVENC_PARAM_H264_VUI_S
{
	VENC_PARAM_VUI_ASPECT_RATIO_S 	   stVuiAspectRatio; 	/* the param of aspect ratio */		
	VENC_PARAM_VUI_H264_TIME_INFO_S    stVuiTimeInfo;		/* the param of time info */	
	VENC_PARAM_VUI_VIDEO_SIGNAL_S      stVuiVideoSignal;	/* the param of video signal */
}VENC_PARAM_H264_VUI_S;


typedef struct hiVENC_PARAM_JPEG_S
{
    HI_U32 u32Qfactor;                             /*image quality :[1,99]*/

    HI_U8  u8YQt[64];                              /* y qt value */
    HI_U8  u8CbQt[64];                             /* cb qt value */
    HI_U8  u8CrQt[64];                             /* cr qt value */

    HI_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_JPEG_S;

typedef struct hiVENC_PARAM_MJPEG_S
{
    HI_U8 u8YQt[64];                               /* y qt value */
    HI_U8 u8CbQt[64];                              /* cb qt value */
    HI_U8 u8CrQt[64];                              /* cr qt value */

    HI_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_MJPEG_S;




/* ROI struct */
typedef struct hiVENC_ROI_CFG_S
{
    HI_U32  u32Index;                              /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
    HI_BOOL bEnable;                               /* Whether to enable this ROI */
    HI_BOOL bAbsQp;                                /* QP mode of an ROI.HI_FALSE: relative QP.HI_TURE: absolute QP.*/
    HI_S32  s32Qp;                                 /* QP value. */
    RECT_S  stRect;                                /* Region of an ROI*/
}VENC_ROI_CFG_S;


typedef struct hiVENC_ROIBG_FRAME_RATE_S
{
    HI_S32 s32SrcFrmRate;                          /* Input frame rate of   Roi backgroud*/
    HI_S32 s32DstFrmRate;                          /* Output frame rate of  Roi backgroud */
}VENC_ROIBG_FRAME_RATE_S;


typedef struct hiVENC_COLOR2GREY_S
{
    HI_BOOL bColor2Grey;                           /* Whether to enable Color2Grey.*/
}VENC_COLOR2GREY_S;

typedef struct hiVENC_CROP_CFG_S
{
    HI_BOOL bEnable;                               /* Crop region enable */
    RECT_S  stRect;                                /* Crop region, note: s32X must be multi of 16 */
}VENC_CROP_CFG_S;


typedef struct hiVENC_FRAME_RATE_S
{
    HI_S32 s32SrcFrmRate;                          /* Input frame rate of a  channel*/
    HI_S32 s32DstFrmRate;                          /* Output frame rate of a channel*/
} VENC_FRAME_RATE_S;

typedef struct hiVENC_PARAM_REF_S
{
    HI_U32       u32Base;                          /*Base layer period*/
    HI_U32       u32Enhance;                       /*Enhance layer period*/
    HI_BOOL      bEnablePred;                      /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is HI_FALSE, all frames at the base layer reference IDR frames.*/
} VENC_PARAM_REF_S;


typedef struct hiVENC_PARAM_REF_EX_S
{
    HI_U32 u32Base;                          /*Base layer period*/
    HI_U32 u32Enhance;                       /*Enhance layer period*/
    HI_BOOL bEnablePred;                      /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is HI_FALSE, all frames at the base layer reference IDR frames.*/

    HI_BOOL bVirtualIEnable;
    HI_U32 u32VirtualIInterval;
    HI_S32 s32VirtualIQpDelta;
} VENC_PARAM_REF_EX_S;


typedef enum hiH264E_IDR_PIC_ID_MODE_E
{
    H264E_IDR_PIC_ID_MODE_AUTO = 0,                /*auto mode */
    H264E_IDR_PIC_ID_MODE_USR,                     /*user mode */
    H264E_IDR_PIC_ID_MODE_BUTT,
}H264E_IDR_PIC_ID_MODE_E;

typedef struct hiVENC_H264_IDRPICID_CFG_S
{
    H264E_IDR_PIC_ID_MODE_E enH264eIdrPicIdMode;   /*the mode of idr_pic_id that is set*/
    HI_U32 u32H264eIdrPicId;                       /*idr_pic_id value*/
} VENC_H264_IDRPICID_CFG_S;     

typedef struct hiVENC_RECV_PIC_PARAM_S
{
    HI_S32 s32RecvPicNum;                          /*Number of frames received and encoded by the encoding channel*/
} VENC_RECV_PIC_PARAM_S;

typedef struct hiVENC_STREAM_BUF_INFO_S
{
    HI_U32   u32PhyAddr;
    HI_VOID *pUserAddr;
    HI_U32   u32BufSize;    
} VENC_STREAM_BUF_INFO_S;

typedef enum hiVENC_MODTYPE_E
{
    MODTYPE_VENC = 1,               
    MODTYPE_H264E,                     
    MODTYPE_H265E,
    MODTYPE_JPEGE,
    MODTYPE_BUTT
} VENC_MODTYPE_E;

typedef struct hiVENC_PARAM_MOD_H264E_S
{
    HI_U32  u32OneStreamBuffer;
    HI_U32  u32H264eVBSource;
    HI_U32  u32H264eRcnEqualRef;
    HI_U32  u32H264eMiniBufMode;  
} VENC_PARAM_MOD_H264E_S;

typedef struct hiVENC_PARAM_MOD_H265E_S
{
    HI_U32  u32OneStreamBuffer;
    HI_U32  u32H265eMiniBufMode;    
} VENC_PARAM_MOD_H265E_S;

typedef struct hiVENC_PARAM_MOD_JPEGE_S
{
    HI_U32  u32OneStreamBuffer;
    HI_U32  u32JpegeMiniBufMode;
    
} VENC_PARAM_MOD_JPEGE_S;


typedef struct hiVENC_PARAM_MOD_VENC_S
{
    HI_U32 u32VencBufferCache;    
} VENC_PARAM_MOD_VENC_S;


typedef struct hiVENC_MODPARAM_S
{
  VENC_MODTYPE_E enVencModType;
  union
  {
     VENC_PARAM_MOD_VENC_S stVencModParam;
     VENC_PARAM_MOD_H264E_S stH264eModParam;
     VENC_PARAM_MOD_H265E_S stH265eModParam;
     VENC_PARAM_MOD_JPEGE_S stJpegeModParam;  
  };
} VENC_PARAM_MOD_S;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VENC_H__ */
