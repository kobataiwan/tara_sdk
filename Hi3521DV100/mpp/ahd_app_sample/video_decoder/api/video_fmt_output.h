﻿/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: The decoder's video format module
 *  Description	: Video Format Information
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/

#include "video_fmt_info.h"

NC_VO_PORT_FMT_S VD_VO_PortFormatDefs[ NC_VIVO_CH_FORMATDEF_MAX ] =
{
	[ AHD20_SD_SH720_NT ] = {
		    .name = "AHD20_SD_SH720_NT",
		    .width = 720,
		    .height = 480,
		    .vo_clk = 0xa0,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_SH720_PAL ] = {
		    .name = "AHD20_SD_SH720_PAL",
		    .width = 720,
		    .height = 576,
		    .vo_clk = 0xa0,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_2EX_Btype_NT ] = {
		    .name = "AHD20_SD_H960_2EX_Btype_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_2EX_Btype_PAL ] = {
		    .name = "AHD20_SD_H960_2EX_Btype_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_NT ] = {
		    .name = "AHD20_SD_H960_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0xa0,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_PAL ] = {
		    .name = "AHD20_SD_H960_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0xa0,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H1280_NT ] = {
		    .name = "AHD20_SD_H1280_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H1280_PAL ] = {
		    .name = "AHD20_SD_H1280_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H1440_NT ] = {
		    .name = "AHD20_SD_H1440_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H1440_PAL ] = {
		    .name = "AHD20_SD_H1440_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_EX_NT ] = {
		    .name = "AHD20_SD_H960_EX_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_EX_PAL ] = {
		    .name = "AHD20_SD_H960_EX_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_2EX_NT ] = {
		    .name = "AHD20_SD_H960_2EX_NT",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_SD_H960_2EX_PAL ] = {
		    .name = "AHD20_SD_H960_2EX_PAL",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_1080P_30P ] = {
		    .name = "AHD20_1080P_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_1080P_25P ] = {
		    .name = "AHD20_1080P_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_60P ] = {
		    .name = "AHD20_720P_60P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_50P ] = {
		    .name = "AHD20_720P_50P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_30P ] = {
		    .name = "AHD20_720P_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_25P ] = {
		    .name = "AHD20_720P_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_30P_EX ] = {
		    .name = "AHD20_720P_30P_EX",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_25P_EX ] = {
		    .name = "AHD20_720P_25P_EX",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_30P_EX_Btype ] = {
		    .name = "AHD20_720P_30P_EX_Btype",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD20_720P_25P_EX_Btype ] = {
		    .name = "AHD20_720P_25P_EX_Btype",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_3M_30P ] = {
		    .name = "AHD30_3M_30P",
		    .width = 1920,
		    .height = 1080,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_3M_25P ] = {
		    .name = "AHD30_3M_25P",
		    .width = 1920,
		    .height = 1080,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_3M_18P ] = {
		    .name = "AHD30_3M_18P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_4M_30P ] = {
		    .name = "AHD30_4M_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_4M_25P ] = {
		    .name = "AHD30_4M_25P",
		    .width = 1920,
		    .height = 1080,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_4M_15P ] = {
		    .name = "AHD30_4M_15P",
		    .width = 2560,
		    .height = 1440,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_5M_20P ] = {
		    .name = "AHD30_5M_20P",
		    .width = 1920,
		    .height = 1080,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_5M_12_5P ] = {
		    .name = "AHD30_5M_12_5P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},

		[ AHD30_5_3M_20P ] = {
		    .name = "AHD30_5_3M_20P",
		    .width = 3072,
		    .height = 1728,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},

		[ AHD30_6M_18P ] = {
		    .name = "AHD30_6M_18P",
		    .width = 3280,
		    .height = 1844,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_6M_20P ] = {
		    .name = "AHD30_6M_20P",
		    .width = 3136,
		    .height = 1764,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_8M_X_30P ] = {
		    .name = "AHD30_8M_X_30P",
		    .width = 1920,
		    .height = 2160,
		    .vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_8M_X_25P ] = {
		    .name = "AHD30_8M_X_25P",
		    .width = 1920,
		    .height = 2160,
		    .vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_8M_7_5P ] = {
		    .name = "AHD30_8M_7_5P",
		    .width = 3840,
		    .height = 2160,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_8M_12_5P ] = {
		    .name = "AHD30_8M_12_5P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
				    .seq_ch01 = 0xc,
				    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ AHD30_8M_15P ] = {
		    .name = "AHD30_8M_15P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0xc,
		    .seq_ch23 =  0x04,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_FHD_30P ] = {
		    .name = "TVI_FHD_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_FHD_25P ] = {
		    .name = "TVI_FHD_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_60P ] = {
		    .name = "TVI_HD_60P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_50P ] = {
		    .name = "TVI_HD_50P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_30P ] = {
		    .name = "TVI_HD_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_25P ] = {
		    .name = "TVI_HD_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_30P_EX ] = {
		    .name = "TVI_HD_30P_EX",
		    .width = 2560,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_25P_EX ] = {
		    .name = "TVI_HD_25P_EX",
		    .width = 2560,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_B_30P ] = {
		    .name = "TVI_HD_B_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_B_25P ] = {
		    .name = "TVI_HD_B_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_B_30P_EX ] = {
		    .name = "TVI_HD_B_30P_EX",
		    .width = 2560,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_HD_B_25P_EX ] = {
		    .name = "TVI_HD_B_25P_EX",
		    .width = 2560,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_3M_18P ] = {
		    .name = "TVI_3M_18P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},

		[ TVI_5M_20P ] = {
			.name = "TVI_5M_20P",
			.width = 2560,
			.height = 1944,
			.vo_clk = 0x58,
			.merge = 0x01,
			.mux_mode = 0x00,
			.seq_ch01 = 0x00,
			.seq_ch23 =  0x00,
			.chid_vin01 = 0x00,
			.chid_vin23 = 0x00,
		},
		[ TVI_5M_12_5P ] = {
		    .name = "TVI_5M_12_5P",
		    .width = 1920,
		    .height = 1080,
			.vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_4M_30P ] = {
		    .name = "TVI_4M_30P",
		    .width = 2560,
		    .height = 1440,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_4M_25P ] = {
		    .name = "TVI_4M_25P",
		    .width = 2560,
		    .height = 1440,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_4M_15P ] = {
		    .name = "TVI_4M_15P",
			 .width = 2560,
			.height = 1440,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_FHD_30P ] = {
		    .name = "CVI_FHD_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_FHD_25P ] = {
		    .name = "CVI_FHD_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_60P ] = {
		    .name = "CVI_HD_60P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_50P ] = {
		    .name = "CVI_HD_50P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_30P ] = {
		    .name = "CVI_HD_30P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_25P ] = {
		    .name = "CVI_HD_25P",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x00,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_30P_EX ] = {
		    .name = "CVI_HD_30P_EX",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_HD_25P_EX ] = {
		    .name = "CVI_HD_25P_EX",
		    .width = 1920,
		    .height = 1080,
		    .vo_clk = 0x58,
		    .merge = 0x00,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_4M_30P ] = {
		    .name = "CVI_4M_30P",
		    .width = 2560,
		    .height = 1440,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_4M_25P ] = {
		    .name = "CVI_4M_25P",
		    .width = 2560,
		    .height = 1440,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_8M_15P ] = {
		    .name = "CVI_8M_15P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ CVI_8M_12_5P ] = {
		    .name = "CVI_8M_12_5P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x58,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_8M_15P ] = {
		    .name = "TVI_8M_15P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x47,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},
		[ TVI_8M_12_5P ] = {
		    .name = "TVI_8M_12_5P",
		    .width = 3840,
		    .height = 2160,
			.vo_clk = 0x47,
		    .merge = 0x01,
		    .mux_mode = 0x00,
		    .seq_ch01 = 0x00,
		    .seq_ch23 =  0x00,
		    .chid_vin01 = 0x00,
		    .chid_vin23 = 0x00,
		},

};
