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
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"
#include <time.h>
#include "iniparser.h"
#include "soc_video_priv.h"
#include "soc_video.h"
#include "tara_video_source.h"
#include "shmkey.h"
#include "soc_video_priv.h"
#include "soc_video_api.h"
#include "hisi_comm.h"
#include "mpi_venc.h"

#include "hdal.h"
#include "hd_debug.h"


#define STREAM_PATH             "/tmp/stream"

#ifndef offsetof
# define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#endif
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef struct _VIDEO_REC_PRIV {
        int index;
        void *drv_priv;
        UINT32 enc_type;
        UINT32 enc_bitrate;
        // (1)
        HD_VIDEOCAP_SYSCAPS cap_syscaps;
        HD_PATH_ID cap_ctrl;
        HD_PATH_ID cap_path;

        HD_DIM  cap_dim;
        HD_DIM  proc_max_dim;

        // (2)
        HD_VIDEOPROC_SYSCAPS proc_syscaps;
        HD_PATH_ID proc_ctrl;
        HD_PATH_ID proc_path;

        HD_DIM  enc_max_dim;
        HD_DIM  enc_dim;

        // (3)
        HD_VIDEOENC_SYSCAPS enc_syscaps;
        HD_PATH_ID enc_path;

        // (4) user pull
        pthread_t  enc_thread_id;
        UINT32     enc_exit;
        UINT32     flow_start;

} VIDEO_REC_PRIV;

/* Private Data Structure */
struct nt9856x_priv {
        STR_CTRL_S StreamCtrl[MAX_VENC_CHN];
        void *rq[MAX_CAM_CHN];

        VIDEO_NORM_E gs_enNorm;         // = VIDEO_ENCODING_MODE_NTSC;
        //VpssGrpInfo vs_hw[1];		// how to translate?
        TaraVideoSource vs[MAX_CAM_CHN];
        ThreadInfo_s raw, viframe;
        int raw_stream;                 // = 0;
        int rawfileformat;
        int getstrflag;                 // = 0
        int getmjpflag;                 // = 0
        int getv2flag;                  // = 0;
        int getStrCnt;                  // = 0;
        int dbgflag;                    // =0;

        // depends on hd_type.h
        UINT32 shdr_mode;		// = 0;
        UINT32 thrdnr;			// = 1;
        UINT32 data_mode;		// = 0; option for vcap out to vprc
        UINT32 data2_mode;		// = 0; option for vprc out to venc
        UINT32 cap_out_fmt;		// = 0;
        UINT32 prc_out_fmt;		// = 0;

        pthread_t gs_VencPid, gs_MjpgPid, gs_Video2Pid; //gs_AencPid;
        AVSERVER_VENC_GETSTREAM_PARA_S gs_stParam, gs_mjParam, gs_v2Param;
        GetStreamThreadCtrl getStrThrCtrl[MAX_CAM_CHN];
        TaraVideoEncoder video1[MAX_CAM_CHN], video2[MAX_CAM_CHN];
        //TaraMjpegEncoder mjpeg1[MAX_CAM_CHN];		// ignore mjpeg first
        // 0: main stream
        // 1: sub stream
        VIDEO_REC_PRIV stream[MAX_STREAM_NUM];
        FILE* outFd[MAX_STREAM_NUM];
};

/* Exported Functions */
int nt9856x_video_init(struct nt9856x_priv *priv);
int nt9856x_video_restart(struct nt9856x_priv *priv);
//int hi3521d_video_set_blank_source(struct nt9856x_priv *priv, int chan, int on);
int nt9856x_video_set_encoder(struct nt9856x_priv *priv, int chan, TARA_VIDEO_STREAM_TYPE type,  TaraVideoEncoder *pvenc);
int nt9856x_video_set_mjpeg(struct nt9856x_priv *priv, int chan, TARA_VIDEO_STREAM_TYPE type,  TaraMjpegEncoder *pmjpg);
int nt9856x_video_encode_start(struct nt9856x_priv *priv, TARA_VIDEO_STREAM_TYPE type, VideoEncoder_t *pvideo);
int nt9856x_mjpeg_encode_start(struct nt9856x_priv *priv, TARA_VIDEO_STREAM_TYPE type,  MjpegEncoder_t *pmjpg);
int nt9856x_video_get_stream_start(struct nt9856x_priv *priv, int vi_num, int stream_num);
int nt9856x_video_get_stream_stop(struct nt9856x_priv *priv);
//int nt9856x_video_get_rawframe_start(struct nt9856x_priv *priv, int type);
//int nt9856x_video_get_rawframe_stop(struct nt9856x_priv *priv);
int nt9856x_video_stop(struct nt9856x_priv *priv);
//int hi3521d_video_destroy(struct hi3521d_priv *priv);


/* Global Variables */
struct soc_video_ops nt9856x_fops = {
//        .soc_init_codec_pts = hicomm_init_codec_pts,
//        .soc_sync_codec_pts = hicomm_sync_codec_pts,
        .soc_video_load_config = hicomm_load_config,		// parse_cfg in sample_video_record.c?
        .soc_video_init_and_bind = nt9856x_video_init,
//        .soc_video_restart = nt9856x_video_restart,		// TODO: impl
        .soc_video_stop = nt9856x_video_stop,
//        .soc_video_set_blank_source = hi3521d_video_set_blank_source,
        .soc_video_set_encoder = nt9856x_video_set_encoder,
//        .soc_video_set_mjpeg = nt9856x_video_set_mjpeg,
        .soc_video_encode_start = nt9856x_video_encode_start,
//        .soc_mjpeg_encode_start = nt9856x_mjpeg_encode_start,
        .soc_video_get_stream_start = nt9856x_video_get_stream_start,
        .soc_video_get_stream_stop = nt9856x_video_get_stream_stop,
//        .soc_video_get_rawframe_start = hi3521d_video_get_rawframe_start,
//        .soc_video_get_rawframe_stop = hi3521d_video_get_rawframe_stop,
//      .soc_video_destroy = hi3521d_video_destroy,
};

const TaraVideoSource vs_default[MAX_CAM_CHN] =
{
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25},
        {VS_TYPE_AHD, 1920, 1080, 25}
};

struct nt9856x_priv nt9856x_video_priv;

/* Definitions */
#define DBGINFO_BUFSIZE()       (0x200)

#define VDO_RAW_BUFSIZE(w, h, pxlfmt)   (ALIGN_CEIL_4((w) * HD_VIDEO_PXLFMT_BPP(pxlfmt) / 8) * (h))

//CA for AWB
#define VDO_CA_BUF_SIZE(win_num_w, win_num_h) ALIGN_CEIL_4((win_num_w * win_num_h << 3) << 1)
//LA for AE
#define VDO_LA_BUF_SIZE(win_num_w, win_num_h) ALIGN_CEIL_4((win_num_w * win_num_h << 1) << 1)
//YUV
#define VDO_YUV_BUFSIZE(w, h, pxlfmt)   (ALIGN_CEIL_4((w) * HD_VIDEO_PXLFMT_BPP(pxlfmt) / 8) * (h))

#define SEN_OUT_FMT     HD_VIDEO_PXLFMT_RAW12
#define CAP_OUT_FMT     HD_VIDEO_PXLFMT_RAW12
#define SHDR_CAP_OUT_FMT HD_VIDEO_PXLFMT_NRX12_SHDR2
#define CA_WIN_NUM_W    32
#define CA_WIN_NUM_H    32
#define LA_WIN_NUM_W    32
#define LA_WIN_NUM_H    32
#define VA_WIN_NUM_W    16
#define VA_WIN_NUM_H    16
#define YOUT_WIN_NUM_W  128
#define YOUT_WIN_NUM_H  128

// RESOLUTION_SET 0	0: 2M(IMX290), 1:5M(OS05A) 2: 2M (OS02K10) 3: 2M (AR0237IR) 4: 2M (OV2736IR)
#define VDO_SIZE_W      1920
#define VDO_SIZE_H      1080

#define VIDEOCAP_ALG_FUNC HD_VIDEOCAP_FUNC_AE | HD_VIDEOCAP_FUNC_AWB
#define VIDEOPROC_ALG_FUNC_0 HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_COLORNR | HD_VIDEOPROC_FUNC_WDR

#define VIDEOPROC_ALG_FUNC_1 HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_COLORNR

/* Static Functions */
static HD_RESULT mem_init(struct nt9856x_priv *priv)
{
        HD_RESULT              ret;
        HD_COMMON_MEM_INIT_CONFIG mem_cfg = {0};

        // config common pool (cap)
        mem_cfg.pool_info[0].type = HD_COMMON_MEM_COMMON_POOL;
        mem_cfg.pool_info[0].blk_size = DBGINFO_BUFSIZE()+VDO_RAW_BUFSIZE(VDO_SIZE_W, VDO_SIZE_H, CAP_OUT_FMT)

                                +VDO_CA_BUF_SIZE(CA_WIN_NUM_W, CA_WIN_NUM_H)

                                +VDO_LA_BUF_SIZE(LA_WIN_NUM_W, LA_WIN_NUM_H);
        if (priv->shdr_mode == 1) {
                mem_cfg.pool_info[0].blk_cnt = 4;
        } else {
                mem_cfg.pool_info[0].blk_cnt = 2;
        }

        mem_cfg.pool_info[0].ddr_id = DDR_ID0;
        // config common pool (main)
        mem_cfg.pool_info[1].type = HD_COMMON_MEM_COMMON_POOL;
        mem_cfg.pool_info[1].blk_size = DBGINFO_BUFSIZE()+VDO_YUV_BUFSIZE(VDO_SIZE_W, VDO_SIZE_H, HD_VIDEO_PXLFMT_YUV420);
        mem_cfg.pool_info[1].blk_cnt = 3;
        mem_cfg.pool_info[1].ddr_id = DDR_ID0;
	// config common pool (main)
        mem_cfg.pool_info[1].type = HD_COMMON_MEM_COMMON_POOL;
        mem_cfg.pool_info[1].blk_size = DBGINFO_BUFSIZE()+VDO_YUV_BUFSIZE(VDO_SIZE_W, VDO_SIZE_H, HD_VIDEO_PXLFMT_YUV420);
        mem_cfg.pool_info[1].blk_cnt = 3;
        mem_cfg.pool_info[1].ddr_id = DDR_ID0;
        if (priv->thrdnr) {
                mem_cfg.pool_info[1].blk_cnt += 1;
        }

	ret = hd_common_mem_init(&mem_cfg);
        return ret;
}

static HD_RESULT mem_exit(void)
{
        HD_RESULT ret = HD_OK;
        hd_common_mem_uninit();
        return ret;
}

static HD_RESULT init_module(void)
{
	HD_RESULT ret;
	if ((ret = hd_videocap_init()) != HD_OK)
		return ret;
	if ((ret = hd_videoproc_init()) != HD_OK)
		return ret;
	if ((ret = hd_videoenc_init()) != HD_OK)
		return ret;
        return HD_OK;
}

static HD_RESULT get_cap_caps(HD_PATH_ID video_cap_ctrl, HD_VIDEOCAP_SYSCAPS *p_video_cap_syscaps)
{
        HD_RESULT ret = HD_OK;
        hd_videocap_get(video_cap_ctrl, HD_VIDEOCAP_PARAM_SYSCAPS, p_video_cap_syscaps);
        return ret;
}

static HD_RESULT get_cap_sysinfo(HD_PATH_ID video_cap_ctrl)
{
        HD_RESULT ret = HD_OK;
        HD_VIDEOCAP_SYSINFO sys_info = {0};

        hd_videocap_get(video_cap_ctrl, HD_VIDEOCAP_PARAM_SYSINFO, &sys_info);
        printf("sys_info.devid =0x%X, cur_fps[0]=%d/%d, vd_count=%llu\r\n", sys_info.dev_id, GET_HI_UINT16(sys_info.cur_fps[0]), GET_LO_UINT16(sys_info.cur_fps[0]), sys_info.vd_count);
        return ret;
}

static HD_RESULT set_cap_param(struct nt9856x_priv *priv, HD_PATH_ID video_cap_path, HD_DIM *p_dim, UINT32 path)
{
        HD_RESULT ret = HD_OK;
	HD_VIDEOCAP_IN video_in_param = {0};
	HD_VIDEOCAP_CROP video_crop_param = {0};
	HD_VIDEOCAP_OUT video_out_param = {0};

	video_in_param.sen_mode = HD_VIDEOCAP_SEN_MODE_AUTO; //auto select sensor mode by the parameter of HD_VIDEOCAP_PARAM_OUT
	video_in_param.frc = HD_VIDEO_FRC_RATIO(30,1);	// FIXME: not fixed fps
	video_in_param.dim.w = p_dim->w;
	video_in_param.dim.h = p_dim->h;
	video_in_param.pxlfmt = SEN_OUT_FMT;

	// NOTE: only SHDR with path 1
	if ((path == 0) && (priv->shdr_mode == 1)) {
		video_in_param.out_frame_num = HD_VIDEOCAP_SEN_FRAME_NUM_2;
	} else {
		video_in_param.out_frame_num = HD_VIDEOCAP_SEN_FRAME_NUM_1;
	}

	ret = hd_videocap_set(video_cap_path, HD_VIDEOCAP_PARAM_IN, &video_in_param);
	if (ret != HD_OK) {
		return ret;
	}

	video_crop_param.mode = HD_CROP_OFF;	// no crop, full frame
	ret = hd_videocap_set(video_cap_path, HD_VIDEOCAP_PARAM_IN_CROP, &video_crop_param);
	if (ret != HD_OK) {
                return ret;
        }

	if (path == 0) {
		if ((priv->cap_out_fmt == 0) && (priv->shdr_mode == 0)) video_out_param.pxlfmt = HD_VIDEO_PXLFMT_RAW12;
		if ((priv->cap_out_fmt == 0) && (priv->shdr_mode == 1)) video_out_param.pxlfmt = HD_VIDEO_PXLFMT_RAW12_SHDR2;
		if ((priv->cap_out_fmt == 1) && (priv->shdr_mode == 0)) video_out_param.pxlfmt = HD_VIDEO_PXLFMT_NRX12;
		if ((priv->cap_out_fmt == 1) && (priv->shdr_mode == 1)) video_out_param.pxlfmt = HD_VIDEO_PXLFMT_NRX12_SHDR2;
	}
	else
		video_out_param.pxlfmt = CAP_OUT_FMT;
	video_out_param.dir = HD_VIDEO_DIR_NONE;
	ret = hd_videocap_set(video_cap_path, HD_VIDEOCAP_PARAM_OUT, &video_out_param);
	if (ret != HD_OK) {
                return ret;
        }

	if (priv->data_mode) {	//direct mode
                HD_VIDEOCAP_FUNC_CONFIG video_path_param = {0};

                video_path_param.out_func = HD_VIDEOCAP_OUTFUNC_DIRECT;
                ret = hd_videocap_set(video_cap_path, HD_VIDEOCAP_PARAM_FUNC_CONFIG, &video_path_param);
        }

	return ret;
}

static HD_RESULT set_cap_cfg(struct nt9856x_priv *priv, HD_PATH_ID *p_video_cap_ctrl)
{
        HD_RESULT ret = HD_OK;
        HD_VIDEOCAP_DRV_CONFIG cap_cfg = {0};
        HD_PATH_ID video_cap_ctrl = 0;
        HD_VIDEOCAP_CTRL iq_ctl = {0};
        char *chip_name = getenv("NVT_CHIP_ID");

	snprintf(cap_cfg.sen_cfg.sen_dev.driver_name, HD_VIDEOCAP_SEN_NAME_LEN-1, "nvt_sen_imx290");
	cap_cfg.sen_cfg.sen_dev.if_type = HD_COMMON_VIDEO_IN_MIPI_CSI;
	cap_cfg.sen_cfg.sen_dev.pin_cfg.pinmux.sensor_pinmux =  0x220; //PIN_SENSOR_CFG_MIPI
	printf("MIPI interface\n");

	if (priv->shdr_mode == 1) {
		//PIN_MIPI_LVDS_CFG_CLK2 | PIN_MIPI_LVDS_CFG_DAT0 | PIN_MIPI_LVDS_CFG_DAT1 | PIN_MIPI_LVDS_CFG_DAT2 | PIN_MIPI_LVDS_CFG_DAT3
		cap_cfg.sen_cfg.sen_dev.pin_cfg.pinmux.serial_if_pinmux = 0xf01;
		printf("Using g_shdr_mode\n");
	} else {
		printf("Using imx290\n");
		// PIN_MIPI_LVDS_CFG_CLK2 | PIN_MIPI_LVDS_CFG_DAT0 | PIN_MIPI_LVDS_CFG_DAT1 | PIN_MIPI_LVDS_CFG_DAT2 | PIN_MIPI_LVDS_CFG_DAT3
		cap_cfg.sen_cfg.sen_dev.pin_cfg.pinmux.serial_if_pinmux = 0xf01;
	}

	if (chip_name != NULL && strcmp(chip_name, "CHIP_NA51089") == 0) {
                cap_cfg.sen_cfg.sen_dev.pin_cfg.pinmux.cmd_if_pinmux = 0x01;//PIN_I2C_CFG_CH1
        } else {
                cap_cfg.sen_cfg.sen_dev.pin_cfg.pinmux.cmd_if_pinmux = 0x10;//PIN_I2C_CFG_CH2
        }
        cap_cfg.sen_cfg.sen_dev.pin_cfg.clk_lane_sel = HD_VIDEOCAP_SEN_CLANE_SEL_CSI0_USE_C0;
        cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[0] = 0;
        cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[1] = 1;

	if (priv->shdr_mode) {
		cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[2] = 2;
                cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[3] = 3;
	} else {
		cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[2] = 2;
		cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[3] = 3;
	}

	cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[4] = HD_VIDEOCAP_SEN_IGNORE;
        cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[5] = HD_VIDEOCAP_SEN_IGNORE;
        cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[6] = HD_VIDEOCAP_SEN_IGNORE;
        cap_cfg.sen_cfg.sen_dev.pin_cfg.sen_2_serial_pin_map[7] = HD_VIDEOCAP_SEN_IGNORE;
        ret = hd_videocap_open(0, HD_VIDEOCAP_0_CTRL, &video_cap_ctrl); //open this for device contro
	if (ret != HD_OK) {
                return ret;
        }

	if (priv->shdr_mode == 1) {
		cap_cfg.sen_cfg.shdr_map = HD_VIDEOCAP_SHDR_MAP(HD_VIDEOCAP_HDR_SENSOR1, (HD_VIDEOCAP_0|HD_VIDEOCAP_1));
        }

	ret |= hd_videocap_set(video_cap_ctrl, HD_VIDEOCAP_PARAM_DRV_CONFIG, &cap_cfg);
	iq_ctl.func = VIDEOCAP_ALG_FUNC;

	if (priv->shdr_mode == 1) {
		iq_ctl.func |= HD_VIDEOCAP_FUNC_SHDR;
	}
	ret |= hd_videocap_set(video_cap_ctrl, HD_VIDEOCAP_PARAM_CTRL, &iq_ctl);

	*p_video_cap_ctrl = video_cap_ctrl;
}

static HD_RESULT set_proc_cfg(struct nt9856x_priv *priv, HD_PATH_ID *p_video_proc_ctrl, HD_DIM* p_max_dim, HD_OUT_ID _out_id)
{
        HD_RESULT ret = HD_OK;
        HD_VIDEOPROC_DEV_CONFIG video_cfg_param = {0};
        HD_VIDEOPROC_CTRL video_ctrl_param = {0};
        HD_PATH_ID video_proc_ctrl = 0;

        ret = hd_videoproc_open(0, _out_id, &video_proc_ctrl); //open this for device control
        if (ret != HD_OK)
                return ret;

        if (p_max_dim != NULL ) {
		if ((HD_CTRL_ID)_out_id == HD_VIDEOPROC_0_CTRL) {
                        video_cfg_param.pipe = HD_VIDEOPROC_PIPE_RAWALL;
            		video_cfg_param.ctrl_max.func = VIDEOPROC_ALG_FUNC_0;
                } else {
                        video_cfg_param.pipe = HD_VIDEOPROC_PIPE_YUVALL;
            		video_cfg_param.ctrl_max.func = VIDEOPROC_ALG_FUNC_1;
                }
                if ((HD_CTRL_ID)_out_id == HD_VIDEOPROC_0_CTRL) {
                        video_cfg_param.isp_id = 0x00000000;
                } else {
                        video_cfg_param.isp_id = 0x00020000;
                }

		if (priv->thrdnr == 1) {
                        printf("[3DNR] Set 1\n");
                        video_cfg_param.ctrl_max.func |= HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_3DNR_STA;
                } else {
                        video_cfg_param.ctrl_max.func &= ~(HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_3DNR_STA);
                }
                if (priv->shdr_mode == 1) {
                        video_cfg_param.ctrl_max.func |= HD_VIDEOPROC_FUNC_SHDR;
                } else {
                        video_cfg_param.ctrl_max.func &= ~HD_VIDEOPROC_FUNC_SHDR;
                }

		video_cfg_param.in_max.func = 0;
                video_cfg_param.in_max.dim.w = p_max_dim->w;
                video_cfg_param.in_max.dim.h = p_max_dim->h;

                if ((HD_CTRL_ID)_out_id == HD_VIDEOPROC_0_CTRL) {
                        // NOTE: only SHDR with path 1
                        {
                                if ((priv->cap_out_fmt == 0) && (priv->shdr_mode == 0)) video_cfg_param.in_max.pxlfmt = HD_VIDEO_PXLFMT_RAW12;
                                if ((priv->cap_out_fmt == 0) && (priv->shdr_mode == 1)) video_cfg_param.in_max.pxlfmt = HD_VIDEO_PXLFMT_RAW12_SHDR2;
                                if ((priv->cap_out_fmt == 1) && (priv->shdr_mode == 0)) video_cfg_param.in_max.pxlfmt = HD_VIDEO_PXLFMT_NRX12;
                                if ((priv->cap_out_fmt == 1) && (priv->shdr_mode == 1)) video_cfg_param.in_max.pxlfmt = HD_VIDEO_PXLFMT_NRX12_SHDR2;
                        }
                }else {
                        video_cfg_param.in_max.pxlfmt = CAP_OUT_FMT;
                }

                video_cfg_param.in_max.frc = HD_VIDEO_FRC_RATIO(1,1);
                ret = hd_videoproc_set(video_proc_ctrl, HD_VIDEOPROC_PARAM_DEV_CONFIG, &video_cfg_param);
                if (ret != HD_OK) {
                        return HD_ERR_NG;
                }
        }
	{
		HD_VIDEOPROC_FUNC_CONFIG video_path_param = {0};

		video_path_param.in_func = 0;
		if (priv->data_mode)
                        video_path_param.in_func |= HD_VIDEOPROC_INFUNC_DIRECT; //direct NOTE: enable direct
                ret = hd_videoproc_set(video_proc_ctrl, HD_VIDEOPROC_PARAM_FUNC_CONFIG, &video_path_param);
	}

	if ((HD_CTRL_ID)_out_id == HD_VIDEOPROC_0_CTRL)
		video_ctrl_param.func = VIDEOPROC_ALG_FUNC_0;
	else
		video_ctrl_param.func = VIDEOPROC_ALG_FUNC_1;

        if (priv->thrdnr == 1) {
                printf("[3DNR] Set 2\n");
                video_ctrl_param.func |= HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_3DNR_STA;
		//use current path as reference path (if venc.dim == raw.dim)
		if ((HD_CTRL_ID)_out_id == HD_VIDEOPROC_0_CTRL) {
			video_ctrl_param.ref_path_3dnr = HD_VIDEOPROC_0_OUT_0;
                } else {
                        video_ctrl_param.ref_path_3dnr = HD_VIDEOPROC_1_OUT_0;
                }
        } else
		video_ctrl_param.func &= ~(HD_VIDEOPROC_FUNC_3DNR | HD_VIDEOPROC_FUNC_3DNR_STA);

        if (priv->shdr_mode == 1) {
                video_ctrl_param.func |= HD_VIDEOPROC_FUNC_SHDR;
        } else {
                video_ctrl_param.func &= ~HD_VIDEOPROC_FUNC_SHDR;
        }

	ret = hd_videoproc_set(video_proc_ctrl, HD_VIDEOPROC_PARAM_CTRL, &video_ctrl_param);

        *p_video_proc_ctrl = video_proc_ctrl;

        return ret;
}

static HD_RESULT set_proc_param(struct nt9856x_priv *priv, HD_PATH_ID video_proc_path, HD_DIM* p_dim, HD_VIDEO_PXLFMT pxlfmt, BOOL is_pull)
{
        HD_RESULT ret = HD_OK;
	HD_VIDEOPROC_FUNC_CONFIG video_path_param = {0};

        if (p_dim != NULL) { //if videoproc is already binding to dest module, not require to setting this!
                HD_VIDEOPROC_OUT video_out_param = {0};
                video_out_param.func = 0;
                video_out_param.dim.w = p_dim->w;
                video_out_param.dim.h = p_dim->h;
                video_out_param.pxlfmt = pxlfmt;
                video_out_param.dir = HD_VIDEO_DIR_NONE;
                video_out_param.frc = HD_VIDEO_FRC_RATIO(1,1);
                ret = hd_videoproc_set(video_proc_path, HD_VIDEOPROC_PARAM_OUT, &video_out_param);
        }
	else {
                HD_VIDEOPROC_OUT video_out_param = {0};
                video_out_param.func = 0;
                video_out_param.dim.w = 0;
                video_out_param.dim.h = 0;
                video_out_param.pxlfmt = pxlfmt;
                video_out_param.dir = HD_VIDEO_DIR_NONE;
                video_out_param.frc = HD_VIDEO_FRC_RATIO(1,1);
                video_out_param.depth = is_pull; //set 1 to allow pull
                ret = hd_videoproc_set(video_proc_path, HD_VIDEOPROC_PARAM_OUT, &video_out_param);
        }

	if (priv->data2_mode) {
		video_path_param.out_func |= HD_VIDEOPROC_OUTFUNC_LOWLATENCY; //enable low-latency
		ret = hd_videoproc_set(video_proc_path, HD_VIDEOPROC_PARAM_FUNC_CONFIG, &video_path_param);
        }

        return ret;
}

static HD_RESULT set_enc_cfg(struct nt9856x_priv *priv, HD_PATH_ID video_enc_path, HD_DIM *p_max_dim, UINT32 max_bitrate, UINT32 isp_id)
{
        HD_RESULT ret = HD_OK;
        HD_VIDEOENC_PATH_CONFIG video_path_config = {0};
        HD_VIDEOENC_FUNC_CONFIG video_func_config = {0};

        if (p_max_dim != NULL) {

                //--- HD_VIDEOENC_PARAM_PATH_CONFIG ---
                video_path_config.max_mem.codec_type = HD_CODEC_TYPE_H264;
                video_path_config.max_mem.max_dim.w  = p_max_dim->w;
                video_path_config.max_mem.max_dim.h  = p_max_dim->h;
                video_path_config.max_mem.bitrate    = max_bitrate;
                video_path_config.max_mem.enc_buf_ms = 3000;
                video_path_config.max_mem.svc_layer  = HD_SVC_4X;
                video_path_config.max_mem.ltr        = TRUE;
                video_path_config.max_mem.rotate     = FALSE;
                video_path_config.max_mem.source_output   = FALSE;
                video_path_config.isp_id             = isp_id;

                ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_PATH_CONFIG, &video_path_config);
                if (ret != HD_OK) {
                        printf("set_enc_path_config = %d\r\n", ret);
                        return HD_ERR_NG;
                }

                if (priv->data2_mode)
                        video_func_config.in_func |= HD_VIDEOENC_INFUNC_LOWLATENCY; //enable low-latency

                ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_FUNC_CONFIG, &video_func_config);
                if (ret != HD_OK) {
                        printf("set_enc_func_config = %d\r\n", ret);
                        return HD_ERR_NG;
                }
        }

        return ret;
}

static HD_RESULT set_enc_param(HD_PATH_ID video_enc_path, HD_DIM *p_dim, HD_VIDEO_PXLFMT pxlfmt, UINT32 enc_type, UINT32 bitrate)
{
        HD_RESULT ret = HD_OK;
        HD_VIDEOENC_IN  video_in_param = {0};
        HD_VIDEOENC_OUT video_out_param = {0};
        HD_H26XENC_RATE_CONTROL rc_param = {0};

        if (p_dim != NULL) {

                //--- HD_VIDEOENC_PARAM_IN ---
                video_in_param.dir           = HD_VIDEO_DIR_NONE;
                video_in_param.pxl_fmt = pxlfmt;
                video_in_param.dim.w   = p_dim->w;
                video_in_param.dim.h   = p_dim->h;
                video_in_param.frc     = HD_VIDEO_FRC_RATIO(1,1);
                ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_IN, &video_in_param);
                if (ret != HD_OK) {
                        printf("set_enc_param_in = %d\r\n", ret);
                        return ret;
                }

                printf("enc_type=%d\r\n", enc_type);

                if (enc_type == 0) {
                        //--- HD_VIDEOENC_PARAM_OUT_ENC_PARAM ---
                        video_out_param.codec_type         = HD_CODEC_TYPE_H265;
                        video_out_param.h26x.profile       = HD_H265E_MAIN_PROFILE;
                        video_out_param.h26x.level_idc     = HD_H265E_LEVEL_5;
                        video_out_param.h26x.gop_num       = 15;
                        video_out_param.h26x.ltr_interval  = 0;
                        video_out_param.h26x.ltr_pre_ref   = 0;
                        video_out_param.h26x.gray_en       = (pxlfmt == HD_VIDEO_PXLFMT_Y8) ? 1: 0;
                        video_out_param.h26x.source_output = 0;
                        video_out_param.h26x.svc_layer     = HD_SVC_DISABLE;
                        video_out_param.h26x.entropy_mode  = HD_H265E_CABAC_CODING;
                        ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_OUT_ENC_PARAM, &video_out_param);
                        if (ret != HD_OK) {
                                printf("set_enc_param_out = %d\r\n", ret);
                        }

                        //--- HD_VIDEOENC_PARAM_OUT_RATE_CONTROL ---
                        rc_param.rc_mode             = HD_RC_MODE_CBR;
                        rc_param.cbr.bitrate         = bitrate;
                        rc_param.cbr.frame_rate_base = 30;
                        rc_param.cbr.frame_rate_incr = 1;
                        rc_param.cbr.init_i_qp       = 26;
                        rc_param.cbr.min_i_qp        = 10;
                        rc_param.cbr.max_i_qp        = 45;
                        rc_param.cbr.init_p_qp       = 26;
                        rc_param.cbr.min_p_qp        = 10;
                        rc_param.cbr.max_p_qp        = 45;
                        rc_param.cbr.static_time     = 4;
                        rc_param.cbr.ip_weight       = 0;
                        ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_OUT_RATE_CONTROL, &rc_param);
                        if (ret != HD_OK) {
                                printf("set_enc_rate_control = %d\r\n", ret);
                                return ret;
                        }
                } else if (enc_type == 1) {

                        //--- HD_VIDEOENC_PARAM_OUT_ENC_PARAM ---
                        video_out_param.codec_type         = HD_CODEC_TYPE_H264;
                        video_out_param.h26x.profile       = HD_H264E_HIGH_PROFILE;
                        video_out_param.h26x.level_idc     = HD_H264E_LEVEL_5_1;
                        video_out_param.h26x.gop_num       = 15;
                        video_out_param.h26x.ltr_interval  = 0;
                        video_out_param.h26x.ltr_pre_ref   = 0;
                        video_out_param.h26x.gray_en       = (pxlfmt == HD_VIDEO_PXLFMT_Y8) ? 1: 0;
                        video_out_param.h26x.source_output = 0;
                        video_out_param.h26x.svc_layer     = HD_SVC_DISABLE;
                        video_out_param.h26x.entropy_mode  = HD_H264E_CABAC_CODING;
                        ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_OUT_ENC_PARAM, &video_out_param);
                        if (ret != HD_OK) {
                                printf("set_enc_param_out = %d\r\n", ret);
                                return ret;
                        }

                        //--- HD_VIDEOENC_PARAM_OUT_RATE_CONTROL ---
                        rc_param.rc_mode             = HD_RC_MODE_CBR;
                        rc_param.cbr.bitrate         = bitrate;
                        rc_param.cbr.frame_rate_base = 30;
                        rc_param.cbr.frame_rate_incr = 1;
                        rc_param.cbr.init_i_qp       = 26;
                        rc_param.cbr.min_i_qp        = 10;
                        rc_param.cbr.max_i_qp        = 45;
                        rc_param.cbr.init_p_qp       = 26;
                        rc_param.cbr.min_p_qp        = 10;
                        rc_param.cbr.max_p_qp        = 45;
                        rc_param.cbr.static_time     = 4;
                        rc_param.cbr.ip_weight       = 0;
                        ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_OUT_RATE_CONTROL, &rc_param);
                        if (ret != HD_OK) {
                                printf("set_enc_rate_control = %d\r\n", ret);
                                return ret;
                        }

                } else if (enc_type == 2) {
                        //--- HD_VIDEOENC_PARAM_OUT_ENC_PARAM ---
                        video_out_param.codec_type         = HD_CODEC_TYPE_JPEG;
                        video_out_param.jpeg.retstart_interval = 0;
                        video_out_param.jpeg.image_quality = 50;
                        ret = hd_videoenc_set(video_enc_path, HD_VIDEOENC_PARAM_OUT_ENC_PARAM, &video_out_param);
                        if (ret != HD_OK) {
                                printf("set_enc_param_out = %d\r\n", ret);
                                return ret;
                        }

                } else {

                        printf("not support enc_type\r\n");
                        return HD_ERR_NG;
                }
        }

        return ret;
}

static HD_RESULT open_module(VIDEO_REC_PRIV *p_stream, HD_DIM* p_proc_max_dim)
{
        HD_RESULT ret;

        // set videocap config
        ret = set_cap_cfg(p_stream->drv_priv, &p_stream->cap_ctrl);
        if (ret != HD_OK) {
                printf("set cap-cfg fail=%d\n", ret);
                return HD_ERR_NG;
        }
        // set videoproc config
	if (p_stream->index == 0)
        	ret = set_proc_cfg(p_stream->drv_priv, &p_stream->proc_ctrl, p_proc_max_dim, HD_VIDEOPROC_0_CTRL);
	else
                ret = set_proc_cfg(p_stream->drv_priv, &p_stream->proc_ctrl, p_proc_max_dim, HD_VIDEOPROC_1_CTRL);

        if (ret != HD_OK) {
                printf("set proc-cfg fail=%d\n", ret);
                return HD_ERR_NG;
        }
#if 0
        // only manage device 0 for capability?
	if (p_stream->index == 0) {
        	if ((ret = hd_videocap_open(HD_VIDEOCAP_0_IN_0, HD_VIDEOCAP_0_OUT_0, &p_stream->cap_path)) != HD_OK)
                	return ret;
	}
#endif
	if (p_stream->index == 0) {
                if ((ret = hd_videocap_open(HD_VIDEOCAP_0_IN_0, HD_VIDEOCAP_0_OUT_0, &p_stream->cap_path)) != HD_OK)
                        return ret;
        	if ((ret = hd_videoproc_open(HD_VIDEOPROC_0_IN_0, HD_VIDEOPROC_0_OUT_0, &p_stream->proc_path)) != HD_OK)
                	return ret;
        	if ((ret = hd_videoenc_open(HD_VIDEOENC_0_IN_0, HD_VIDEOENC_0_OUT_0, &p_stream->enc_path)) != HD_OK)
                	return ret;
	} else {
                if ((ret = hd_videocap_open(HD_VIDEOCAP_0_IN_0, HD_VIDEOCAP_0_OUT_0, &p_stream->cap_path)) != HD_OK)
                        return ret;
        	if ((ret = hd_videoproc_open(HD_VIDEOPROC_0_IN_0, HD_VIDEOPROC_0_OUT_1, &p_stream->proc_path)) != HD_OK)
                	return ret;
        	if ((ret = hd_videoenc_open(HD_VIDEOENC_0_IN_1, HD_VIDEOENC_0_OUT_1, &p_stream->enc_path)) != HD_OK)
                	return ret;
	}

        return HD_OK;
}

static HD_RESULT close_module(VIDEO_REC_PRIV *p_stream)
{
        HD_RESULT ret;

        if ((ret = hd_videocap_close(p_stream->cap_path)) != HD_OK)
                return ret;
        if ((ret = hd_videoproc_close(p_stream->proc_path)) != HD_OK)
                return ret;
        if ((ret = hd_videoenc_close(p_stream->enc_path)) != HD_OK)
                return ret;

        return HD_OK;
}

static HD_RESULT exit_module(void)
{
        HD_RESULT ret;

        if ((ret = hd_videocap_uninit()) != HD_OK)
                return ret;
        if ((ret = hd_videoproc_uninit()) != HD_OK)
                return ret;
        if ((ret = hd_videoenc_uninit()) != HD_OK)
                return ret;

        return HD_OK;
}

/* Exported Callback Functions */
int nt9856x_video_init(struct nt9856x_priv *priv)
{
        HD_RESULT ret;
	HD_DIM main_dim;
        HD_DIM sub_dim;
	UINT32 enc_type;
	UINT32 enc_bitrate = 4;

	// initialize the vs and 3 videos
	memcpy(&priv->vs, &vs_default, sizeof(priv->vs));
	memset(&priv->video1, 0, sizeof(priv->video1));
	memset(&priv->video2, 0, sizeof(priv->video2));
//	memset(&priv->mjpeg1, 0, sizeof(priv->mjpeg1));

	priv->thrdnr = 1;		// turn 3DNR on
	priv->prc_out_fmt = 0;
	priv->cap_out_fmt = 0;	// HD_VIDEO_PXLFMT_RAW12
	priv->data_mode = 0;	// direct mode
	priv->data2_mode = 0;	// 1: low latency
	//priv->drv_priv = (void *)priv;

        /******************************************
    	step 1: hdal/memory/module init.
        ******************************************/
	ret = hd_common_init(0);
	if (ret != HD_OK) {
		printf("common fail=%d\n", ret);
		goto exit;
	}

	// init memory
        ret = mem_init(priv);
        if (ret != HD_OK) {
    	   printf("mem fail=%d\n", ret);
                goto exit;
        }

	// open video_record modules (main)
	priv->stream[0].index = 0;
        priv->stream[0].proc_max_dim.w = VDO_SIZE_W; //assign by hicomm_load_config
        priv->stream[0].proc_max_dim.h = VDO_SIZE_H; //assign by hicomm_load_config,
	priv->stream[0].drv_priv = priv;

	ret = open_module(&priv->stream[0], &priv->stream[0].proc_max_dim);
        if (ret != HD_OK) {
    	   printf("open fail=%d\n", ret);
                goto exit;
        }

	// open video_record modules (sub) TODO: based on the config file
	priv->stream[1].index = 1;
        priv->stream[1].proc_max_dim.w = VDO_SIZE_W; //assign by hicomm_load_config
        priv->stream[1].proc_max_dim.h = VDO_SIZE_H; //assign by hicomm_load_config,
	priv->stream[1].drv_priv = priv;

        ret = open_module(&priv->stream[1], &priv->stream[1].proc_max_dim);
        if (ret != HD_OK) {
	       printf("open fail=%d\n", ret);
                goto exit;
        }

	// get videocap capability
        ret = get_cap_caps(priv->stream[0].cap_ctrl, &priv->stream[0].cap_syscaps);
        if (ret != HD_OK) {
                printf("get cap-caps fail=%d\n", ret);
                goto exit;
        }

        // set videocap parameter
        priv->stream[0].cap_dim.w = VDO_SIZE_W; //assign by user
        priv->stream[0].cap_dim.h = VDO_SIZE_H; //assign by user
        ret = set_cap_param(priv, priv->stream[0].cap_path, &priv->stream[0].cap_dim, 0);
        if (ret != HD_OK) {
                printf("set cap fail=%d\n", ret);
                goto exit;
        }

		// assign parameter by program options
        main_dim.w = VDO_SIZE_W;
        main_dim.h = VDO_SIZE_H;
        sub_dim.w = VDO_SIZE_W;
        sub_dim.h = VDO_SIZE_H;

        // set videoproc parameter (main)
	if (priv->prc_out_fmt == 1) {
	       ret = set_proc_param(priv, priv->stream[0].proc_path, NULL, HD_VIDEO_PXLFMT_YUV420_NVX2, FALSE);
                if (ret != HD_OK) {
            	   printf("set proc fail=%d\n", ret);
                        goto exit;
                }
        } else {
		ret = set_proc_param(priv, priv->stream[0].proc_path, NULL, HD_VIDEO_PXLFMT_YUV420, FALSE);
                if (ret != HD_OK) {
                        printf("set proc fail=%d\n", ret);
                        goto exit;
                }
        }

	// set videoproc parameter (sub)
        ret = set_proc_param(priv, priv->stream[1].proc_path, NULL, HD_VIDEO_PXLFMT_YUV420, FALSE);
        if (ret != HD_OK) {
        	printf("set proc fail=%d\n", ret);
            goto exit;
        }

	// bind capture and vpss
        hd_videocap_bind(HD_VIDEOCAP_0_OUT_0, HD_VIDEOPROC_0_IN_0);

	// main
	hd_videoproc_bind(HD_VIDEOPROC_0_OUT_0, HD_VIDEOENC_0_IN_0);

	// sub
	hd_videoproc_bind(HD_VIDEOPROC_0_OUT_1, HD_VIDEOENC_0_IN_1);


	if (priv->data_mode) {
                hd_videoproc_start(priv->stream[0].proc_path);
                hd_videocap_start(priv->stream[0].cap_path);
	} else {
		hd_videocap_start(priv->stream[0].cap_path);
		hd_videoproc_start(priv->stream[0].proc_path);
	}

exit:
	// close video_record modules (main)
	ret = close_module(&priv->stream[0]);
        if (ret != HD_OK) {
                printf("close fail=%d\n", ret);
        }

	ret = close_module(&priv->stream[1]);
        if (ret != HD_OK) {
                printf("close fail=%d\n", ret);
        }

        // uninit all modules
        ret = exit_module();
        if (ret != HD_OK) {
                printf("exit fail=%d\n", ret);
        }

       // uninit memory
        ret = mem_exit();
        if (ret != HD_OK) {
                printf("mem fail=%d\n", ret);
        }

        // uninit hdal
        ret = hd_common_uninit();
        if (ret != HD_OK) {
                printf("common fail=%d\n", ret);
        }

        return 0;
}

int nt9856x_video_encode_start(struct nt9856x_priv *priv, TARA_VIDEO_STREAM_TYPE stream_type, VideoEncoder_t *pvideo)
{
	int i, ret;
	UINT32 enc_type;
	UINT32 enc_bitrate = 4;

	if (!pvideo)
		return -1;

	if (stream_type != VIDEO_MAIN_STREAM && stream_type != VIDEO_SUB_STREAM)
		return -1;

	// assume we only have 1 chan
	if (stream_type == VIDEO_MAIN_STREAM) {
		memcpy(&priv->video1[0], &pvideo[0].venc, sizeof(TaraVideoEncoder));
		DBG("Video1 %s %s %dx%d %dfps %d bps qp=%d\n", priv->video1[0].video == ENCODER_ON ? "On" : "Off",
		priv->video1[0].type == VIDEO_CODEC_HEVC ? "H265" : "H264", priv->video1[0].width, priv->video1[0].height, priv->video1[0].framerate, riv->video1[0].bitrate, priv->video1[0].qp);
	} else if (stream_type == VIDEO_SUB_STREAM) {
		memcpy(&priv->video2[0], &pvideo[0].venc, sizeof(TaraVideoEncoder));
		DBG("Video2 %s %s %dx%d %dfps %d bps qp=%d\n", priv->video2[0].video == ENCODER_ON ? "On" : "Off",
                priv->video2[0].type == VIDEO_CODEC_HEVC ? "H265" : "H264", priv->video2[0].width, priv->video2[0].height, priv->video2[0].framerate, priv->video2[0].bitrate, priv->video2[0].qp);
	}

	if (stream_type == VIDEO_MAIN_STREAM) {
		// set videoenc config (main)
		priv->stream[0].enc_max_dim.w = VDO_SIZE_W;
		priv->stream[0].enc_max_dim.h = VDO_SIZE_H;
		priv->stream[0].enc_bitrate = enc_bitrate;	// 4M max
		ret = set_enc_cfg(priv, priv->stream[0].enc_path, &priv->stream[0].enc_max_dim, enc_bitrate * 1024 * 1024, 0);
		if (ret != HD_OK) {
			printf("set enc-cfg fail=%d\n", ret);
			return ret;;
		}

	       	// set videoenc parameter (main)
		priv->stream[0].enc_dim.w = VDO_SIZE_W;
		priv->stream[0].enc_dim.h = VDO_SIZE_H;
		priv->stream[0].enc_type = enc_type = 0;        // H.265 for main
		priv->stream[0].enc_bitrate = enc_bitrate;
		ret = set_enc_param(priv->stream[0].enc_path, &priv->stream[0].enc_dim, HD_VIDEO_PXLFMT_YUV420, enc_type, enc_bitrate * 1024 * 1024);
		if (ret != HD_OK) {
			printf("set enc fail=%d\n", ret);
			return ret;
		}

		hd_videoenc_start(priv->stream[0].enc_path);
		priv->stream[0].flow_start = 1;
	} else {
		// set videoenc config (sub)
		priv->stream[1].enc_max_dim.w = VDO_SIZE_W;
		priv->stream[1].enc_max_dim.h = VDO_SIZE_H;
		ret = set_enc_cfg(priv, priv->stream[1].enc_path, &priv->stream[1].enc_max_dim, 1 * 1024 * 1024, 0);
		if (ret != HD_OK) {
			printf("set enc-cfg fail=%d\n", ret);
			return ret;
		}

		// set videoenc parameter (sub)
		priv->stream[1].enc_dim.w = VDO_SIZE_W;
		priv->stream[1].enc_dim.h = VDO_SIZE_H;
		ret = set_enc_param(priv->stream[1].enc_path, &priv->stream[1].enc_dim, HD_VIDEO_PXLFMT_YUV420, enc_type, 1 * 1024 * 1024);
		if (ret != HD_OK) {
			printf("set enc fail=%d\n", ret);
			return ret;
		}

		hd_videoenc_start(priv->stream[1].enc_path);
		priv->stream[1].flow_start = 1;
	}

}

int nt9856x_video_set_encoder(struct nt9856x_priv *priv, int chan, TARA_VIDEO_STREAM_TYPE stream_type,  TaraVideoEncoder *pvenc)
{
	TaraVideoEncoder *pv;
	int rst = 0, ret = 0;
	UINT32 enc_type = 0;

        if (pvenc == NULL)
                return -1;

	// default #chan = 0
        if (stream_type == VIDEO_MAIN_STREAM)
                pv = &priv->video1[0];
        else pv = &priv->video2[0];

	rst |= pv->video ^ pvenc->video;
	rst |= pv->profile ^ pvenc->profile;
	rst |= pv->ratecontrol ^ pvenc->ratecontrol;
	rst |= pv->type ^ pvenc->type;
	rst |= pv->width ^ pvenc->width;
	rst |= pv->height ^ pvenc->height;

        memcpy(pv, pvenc, sizeof(TaraVideoEncoder));
        printf("chn%d:%s %dx%d, %d fps, %d bps, gop=%d, rcmode=%d\n", chan,
                        pv->video == 0 ? "Off" : "On",
                        pv->width, pv->height, pv->framerate, pv->bitrate, pv->gopsize, pv->ratecontrol);

	// why?
        if (rst == 0) {
		if (stream_type == VIDEO_MAIN_STREAM) {
			priv->stream[0].enc_max_dim.w = pv->width;
			priv->stream[0].enc_max_dim.h = pv->height;
			ret = set_enc_cfg(
                                        priv,
                                        priv->stream[0].enc_path, &priv->stream[0].enc_max_dim,
                                        1 * 1024 * 1024, 0);
			if (ret != HD_OK) {
				printf("set enc-cfg fail=%d\n", ret);
				return ret;
			}

			priv->stream[0].enc_dim.w = pv->width;
			priv->stream[0].enc_dim.h = pv->height;
			ret = set_enc_param(priv->stream[0].enc_path, &priv->stream[0].enc_dim, HD_VIDEO_PXLFMT_YUV420, enc_type, 1 * 1024 * 1024);
			if (ret != HD_OK) {
				printf("set enc fail=%d\n", ret);
				return ret;
			}
		}
		else if (stream_type == VIDEO_SUB_STREAM) {
			priv->stream[1].enc_max_dim.w = pv->width;
			priv->stream[1].enc_max_dim.h = pv->height;
			ret = set_enc_cfg(priv, priv->stream[1].enc_path, &priv->stream[1].enc_max_dim, 1 * 1024 * 1024, 0);
			if (ret != HD_OK) {
				printf("set enc-cfg fail=%d\n", ret);
				return ret;
			}

			priv->stream[1].enc_dim.w = pv->width;
			priv->stream[1].enc_dim.h = pv->height;
			ret = set_enc_param(priv->stream[1].enc_path, &priv->stream[1].enc_dim, HD_VIDEO_PXLFMT_YUV420, enc_type, 1 * 1024 * 1024);
			if (ret != HD_OK) {
				printf("set enc fail=%d\n", ret);
				return ret;
			}
		}
	}
        else
                return rst;	// video_platform_restart
        return SUCCESS;

}

FILE* nt9856x_getOutfd(VENC_CHN vChn)
{
	FILE* fdOut = NULL;
	char commFolder[] = "/tmp/streamDump/";
	char* fileName = NULL, *fullFilePath = NULL;
	char vChnStr = vChn + '0';

	fileName = strncat("dumpStream_", vChnStr, sizeof(vChnStr));
        fileName = strncat(fileName, ".dat", sizeof(char) * 4);
	fullFilePath = strncat(commFolder, fileName, sizeof(fileName));

	if ((fdOut = fopen(fullFilePath, "wb")) == NULL) {
		HD_VIDEOENC_ERR("open file (%s) fail...\r\n", fullFilePath);
	} else {
		printf("\r\nDump channel %d main stream to file (%s) ...", vChn, fullFilePath);
	}

	return fdOut;
}

/*****************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* getStreamProc(HI_VOID *p)
{
        struct nt9856x_priv *priv;
        HI_S32 i = 0, j = 0, ret = HD_OK;
        AVSERVER_VENC_GETSTREAM_PARA_S* pstPara;
        HI_S32 maxfd = 0;
        struct timeval TimeoutVal;
        fd_set read_fds;
        HI_S32 VencFd[MAX_CAM_CHN];
        HI_CHAR aszFileName[MAX_CAM_CHN][FILE_NAME_LEN];
        FILE *pCloseFile[MAX_CAM_CHN];
        char szFilePostfix[MAX_CAM_CHN][10];
        PAYLOAD_TYPE_E enPayLoadType[MAX_CAM_CHN];
        VENC_CHN_STAT_S stStat;
        VENC_STREAM_S stStream;
        HI_S32 s32Ret;
        VENC_CHN VencChn;
        HI_CHAR aszPathName[MAX_CAM_CHN][64];
        HI_U32 closeFlag[MAX_CAM_CHN]={0};
        HI_CHAR delFileName[64];
        HI_CHAR path[15] = STREAM_PATH;
        int delFile = 0;
        int stype;
        int qsize = 300;
        int framerate, gopsize;
        int onoff[MAX_CAM_CHN];
        UINT32 vir_addr_main;
        HD_VIDEOENC_BUFINFO phy_buf_main;
        HD_VIDEOENC_BS  data_pull;
        #define PHY2VIRT_MAIN(pa) (vir_addr_main + (pa - phy_buf_main.buf_info.phy_addr))


        pstPara = (AVSERVER_VENC_GETSTREAM_PARA_S*)p;
        stype = pstPara->strtype;

        if (stype == STREAM_TYPE_VIDEO1)
                priv = container_of(pstPara, struct nt9856x_priv, gs_stParam);
        else if (stype == STREAM_TYPE_VIDEO2)
                priv = container_of(pstPara, struct nt9856x_priv, gs_v2Param);
        else
                return NULL;

        for (i = 0; i < MAX_CAM_CHN; i++)
                onoff[i] = (stype == STREAM_TYPE_VIDEO1) ? priv->video1[i].video : priv->video2[i].video;

        /******************************************
        step 1:  check & prepare save-file
        ******************************************/
        memset(&VencFd, 0, sizeof(VencFd));
        mkdir(path, 0777);

        HD_VIDEOENC_MSG(
                "==== %s: %d %s\n", __func__,
                stype, stype == STREAM_TYPE_VIDEO1 ? "Video1" : "Video2");

        for (i = 0; i < MAX_CAM_CHN; i++) {
                VencChn = (i * MAX_STREAM_NUM) + stype;
                //VencFd[i] = 0;
                //pCloseFile[i] = NULL;
                priv->StreamCtrl[VencChn].channelNo = VencChn;
                priv->StreamCtrl[VencChn].pFile = NULL;
                priv->StreamCtrl[VencChn].serialNo = 0;
                priv->StreamCtrl[VencChn].frameCnt = 0;
                priv->StreamCtrl[VencChn].rb_rdidx = 0;
                priv->StreamCtrl[VencChn].rb_wtidx = 0;
                priv->StreamCtrl[VencChn].lastOffset = 0;
                priv->StreamCtrl[VencChn].lastSize = 0;

                if (stype == STREAM_TYPE_VIDEO1) {
                        if (onoff[i] == ENCODER_OFF)
                                continue;
                        enPayLoadType[i] = priv->video1[i].type == VIDEO_CODEC_HEVC ? PT_H265 : PT_H264;
                } else {
                        if (onoff[i] == ENCODER_OFF)
                                continue;
                        enPayLoadType[i] = priv->video2[i].type == VIDEO_CODEC_HEVC ? PT_H265 : PT_H264;
                }
        }


        HD_VIDEOENC_MSG("start to get stream%d %d\n", stype, pstPara->bThreadStart);
        /******************************************
        step 2:  Start to get streams of each channel.
        ******************************************/
        pthread_detach(pthread_self());
        if (stype == STREAM_TYPE_VIDEO1)
                priv->getstrflag = 1;
        else
                priv->getv2flag = 1;

        while (HI_TRUE == pstPara->bThreadStart) {
                for (i = 0; i < MAX_STREAM_NUM; i++) {
                        if (priv->stream[i].flow_start == 0) {
                                continue;
                        }

                        // query physical address of bs buffer ( this can ONLY query after hd_videoenc_start() is called !! )
                        hd_videoenc_get(
                                priv->stream[i].enc_path,
                                HD_VIDEOENC_PARAM_BUFINFO,
                                &phy_buf_main);

                        // mmap for bs buffer (just mmap one time only, calculate offset to virtual address later)
                        vir_addr_main = (UINT32) hd_common_mem_mmap(
                                                        HD_COMMON_MEM_MEM_TYPE_CACHE,
                                                        phy_buf_main.buf_info.phy_addr,
                                                        phy_buf_main.buf_info.buf_size);

                        priv->outFd[i] = nt9856x_getOutfd(i);
                        if (priv->outFd[i] == NULL) {
                                continue;
                        }

                        memset (&data_pull, 0x00, sizeof(HD_VIDEOENC_BS));

                        while (priv->stream[i].enc_exit == 0) {
                                //pull data
                                ret = hd_videoenc_pull_out_buf(
                                                priv->stream[i].enc_path,
                                                &data_pull, -1); // -1 = blocking mode

                                if (ret == HD_OK) {
                                        for (j = 0; j < data_pull.pack_num; j ++) {
                                                UINT8 *ptr = (UINT8 *)PHY2VIRT_MAIN(
                                                                        data_pull.video_pack[j].phy_addr);
                                                UINT32 len = data_pull.video_pack[j].size;
                                                if (priv->outFd[i])
                                                        fwrite(ptr, 1, len, priv->outFd[i]);
                                                if (priv->outFd)
                                                        fflush(priv->outFd[i]);
                                        }

                                        // release data
                                        ret = hd_videoenc_release_out_buf(
                                                        priv->stream[0].enc_path,
                                                        &data_pull);
                                        if (ret != HD_OK) {
                                                printf("enc_release error=%d !!\r\n", ret);
                                        }
                                }
                        }

                        // mummap for bs buffer
                        if (vir_addr_main)
                                hd_common_mem_munmap(
                                        (void *)vir_addr_main,
                                        phy_buf_main.buf_info.buf_size);

                        if (priv->outFd[i])
                                fclose(priv->outFd[i]);
                }
        }

        return;
}


int nt9856x_video_get_stream_start(struct nt9856x_priv *priv, int vi_num, int stream_num)
{
        priv->gs_stParam.bThreadStart = HI_TRUE;
        priv->gs_stParam.strtype = STREAM_TYPE_VIDEO1;
        priv->gs_v2Param.bThreadStart = HI_TRUE;
        priv->gs_v2Param.strtype = STREAM_TYPE_VIDEO2;
        //priv->gs_mjParam.bThreadStart = HI_TRUE;
        //priv->gs_mjParam.strtype = STREAM_TYPE_MJPEG1;

        pthread_create(&priv->gs_VencPid, 0, getStreamProc, (HI_VOID*)&priv->gs_stParam);
        pthread_create(&priv->gs_Video2Pid, 0, getStreamProc, (HI_VOID*)&priv->gs_v2Param);
//        pthread_create(&priv->gs_MjpgPid, 0, getMjpegProc, (HI_VOID*)&priv->gs_mjParam);

	return 0;
}

int nt9856x_video_get_stream_stop(struct nt9856x_priv *priv)
{
        DBG("%s\n", __func__);
        if (HI_TRUE == priv->gs_stParam.bThreadStart) {
                priv->gs_stParam.bThreadStart = HI_FALSE;
                pthread_join(priv->gs_VencPid, 0);
        }
        if (HI_TRUE == priv->gs_v2Param.bThreadStart) {
                priv->gs_v2Param.bThreadStart = HI_FALSE;
                pthread_join(priv->gs_Video2Pid, 0);
        }
        if (HI_TRUE == priv->gs_mjParam.bThreadStart) {
                priv->gs_mjParam.bThreadStart = HI_FALSE;
                pthread_join(priv->gs_MjpgPid, 0);
        }
        return SUCCESS;
}

int nt9856x_video_stop(struct nt9856x_priv *priv)
{

	if (priv->data_mode) {
                hd_videoproc_stop(priv->stream[0].proc_path);
                hd_videocap_stop(priv->stream[0].cap_path);
        } else {
                hd_videocap_stop(priv->stream[0].cap_path);
                hd_videoproc_stop(priv->stream[0].proc_path);
        }

	hd_videoenc_stop(priv->stream[0].enc_path);

	hd_videoproc_stop(priv->stream[1].proc_path);
	hd_videoenc_stop(priv->stream[1].enc_path);

	// unbind video_record modules (main)
        hd_videocap_unbind(HD_VIDEOCAP_0_OUT_0);
        hd_videoproc_unbind(HD_VIDEOPROC_0_OUT_0);

	// unbind video_record modules (sub)
        hd_videoproc_unbind(HD_VIDEOPROC_0_OUT_1);
}
