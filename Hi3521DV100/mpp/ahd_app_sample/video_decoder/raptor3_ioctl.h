﻿#ifndef __RAPTOR3_IOCTL_H__
#define __RAPTOR3_IOCTL_H__

/*-------------------------- Clock -------------------------*/
#define IOC_VDEC_ADC_CLOCK_SET						0x10
#define IOC_VDEC_PRE_CLOCK_SET						0x11
#define IOC_VDEC_VIDEO_OUTPUT_CLOCK_SET				0x12
#define IOC_VDEC_EXTERNAL_FSC_ENABLE				0x13
#define IOC_VDEC_OUTPUT_AUTO_VCLK_SET				0x14
#define IOC_VDEC_CLOCK_SET							0x15

/*----------------------- Video Input ----------------------*/
#define IOC_VDEC_INPUT_H_TIMING_SET					0x22
#define IOC_VDEC_INPUT_AFE_INIT						0x28
#define IOC_VDEC_INPUT_COLOR_SET					0x2F

/*----------------------- Video Output ---------------------*/

#define IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_SET		0x32
#define IOC_VDEC_OUTPUT_PORT_MUX_MODE_SET			0x33
#define IOC_VDEC_OUTPUT_PORT_ENABLE_SET				0x34
#define IOC_VDEC_OUTPUT_PORT_FORMAT_SET				0x35
#define IOC_VDEC_OUTPUT_PORT_MUX_CHID_SET			0x36
#define IOC_VDEC_OUTPUT_CHANNEL_SEQUENCE_GET		0x37

/*----------------- Video Auto Format Detect --------------*/

#define IOC_VDEC_AUTO_DETECT_MODE_SET				0x40
#define IOC_VDEC_AUTO_VFC_GET						0x41
#define IOC_VDEC_AUTO_CABLE_DIST_GET				0x42
#define IOC_VDEC_AUTO_SAM_VAL_GET					0x43
#define IOC_VDEC_AUTO_HSYNC_ACCUM_GET				0x44
#define IOC_VDEC_AUTO_AGC_VAL_GET					0x45
#define IOC_VDEC_AUTO_NOVID_SET						0x46
#define IOC_VDEC_AUTO_NOVID_GET						0x47
#define IOC_VDEC_AUTO_ACC_GAIN_GET					0x48
#define IOC_VDEC_AUTO_DATA_OUT_MODE_SET				0x49
#define IOC_VDEC_AUTO_ONVIDEO_SET					0x4A
#define IOC_VDEC_AUTO_ONVIDEO_CHECK					0x4B
#define IOC_VDEC_AHD_TVI_DISTINGUISH 				0x4C
#define IOC_VDEC_CVI_TVI_DISTINGUISH 				0x4D
#define IOC_VDEC_AHD_CVI_DISTINGUISH 				0x4E

/*--------------------- Equalizer  ------------------------------*/
#define IOC_VDEC_MANUAL_CABLE_DIST_GET				0x50
#define IOC_VDEC_MANUAL_EQ_DIST_SET					0x51

/*----------------------- Coaxial protocol  ---------------------*/
// Coax UP Stream - 8bit
#define IOC_VDEC_COAX_TX_INIT			  0xA0
#define IOC_VDEC_COAX_TX_CMD_SEND	  0xA1

// Coax UP Stream - 16bit only ACP 720P Support
#define IOC_VDEC_COAX_TX_16BIT_INIT		  0xB4
#define IOC_VDEC_COAX_TX_16BIT_CMD_SEND	  0xB5
#define IOC_VDEC_COAX_TX_CVI_NEW_CMD_SEND 0xB6

// Coax Down Stream
#define IOC_VDEC_COAX_RX_INIT      0xA2
#define IOC_VDEC_COAX_RX_DATA_READ 0xA3
#define IOC_VDEC_COAX_RX_BUF_CLEAR 0xA4
#define IOC_VDEC_COAX_RX_DEINIT    0xA5

// Coax Test
#define IOC_VDEC_COAX_TEST_TX_INIT_DATA_READ  0xA6
#define IOC_VDEC_COAX_TEST_DATA_SET           0xA7
#define IOC_VDEC_COAX_TEST_DATA_READ          0xA8

// Coax FW Update
#define IOC_VDEC_COAX_FW_ACP_HEADER_GET     0xA9
#define IOC_VDEC_COAX_FW_READY_CMD_SET  0xAA
#define IOC_VDEC_COAX_FW_READY_ACK_GET  0xAB
#define IOC_VDEC_COAX_FW_START_CMD_SET  0xAC
#define IOC_VDEC_COAX_FW_START_ACK_GET  0xAD
#define IOC_VDEC_COAX_FW_SEND_DATA_SET  0xAE
#define IOC_VDEC_COAX_FW_SEND_ACK_GET   0xAF
#define IOC_VDEC_COAX_FW_END_CMD_SET    0xB0
#define IOC_VDEC_COAX_FW_END_ACK_GET    0xB1

// Bank Dump Test
#define IOC_VDEC_COAX_BANK_DUMP_GET    0xB2

// ACP Option
#define IOC_VDEC_COAX_RT_NRT_MODE_CHANGE_SET 0xB3

/*----------------------- MOTION -----------------*/
#define IOC_VDEC_MOTION_SET			0x70
#define IOC_VDEC_MOTION_PIXEL_SET     0x71
#define IOC_VDEC_MOTION_PIXEL_GET     0x72
#define IOC_VDEC_MOTION_TSEN_SET      0x73
#define IOC_VDEC_MOTION_PSEN_SET      0x74
#define IOC_VDEC_MOTION_ALL_PIXEL_SET 0x75
#define IOC_VDEC_MOTION_DETECTION_GET 0x76

/*----------------------- AUDIO -----------------*/

#define IOC_VDEC_AUDIO_DEFAULT_SET	0x80


#define IOC_VDEC_GET_CHIP_ID		0x90
#define IOC_VDEC_HAFC_GAIN12_CTRL	0x91
#define IOC_VDEC_AFE_CONTROL_PD		0x92
#define IOC_VDEC_AFE_CONTROL_PU		0x93
#define IOC_VDEC_MANUAL_AGC_STABLE_ENABLE	0x94
#define IOC_VDEC_MANUAL_AGC_STABLE_DISABLE	0x95


#endif
