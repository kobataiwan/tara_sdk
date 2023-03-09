/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Msg_Def.h
* @brief Definition of message command, message key, and message type.
*/
#ifndef __MSG_H__
#define __MSG_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys_env_type.h>
#include <serial_env_type.h>
/* Codec server */
/**
* @defgroup UTILITY Utility API

* This contains kinds of API  for communicating.
* @{
*/

/** @} */
enum {
	MSG_CMD_START = 0,

	MSG_CMD_GET_MEM,
	MSG_CMD_QUIT,
	MSG_CMD_SET_MEM,
	MSG_CMD_PRINT_MEM,
	MSG_CMD_RESET_FRAMEBUF,
	MSG_CMD_GET_CUR_FRAME,
	MSG_CMD_GET_CUR_FRAME_INFO,
	MSG_CMD_GET_LAST_MJPEG_FRAME,
	MSG_CMD_GET_MEDIA_INFO,
	MSG_CMD_GET_VOL,
	MSG_CMD_GET_NEW_FRAME,
	MSG_CMD_GET_AND_LOCK_IFRAME,
	MSG_CMD_SET_DAY_NIGHT,
	MSG_CMD_SET_BRIGHTNESS,
	MSG_CMD_SET_CONTRAST,
	MSG_CMD_SET_SATURATION,
	MSG_CMD_SET_HUE,
	MSG_CMD_SET_SHARPNESS,
	MSG_CMD_SET_IRCUTFILTER,
	MSG_CMD_SET_IRCUTFILTER_RESPONSETIME,
	MSG_CMD_SET_IRCUTFILTER_THRESHOLD,
	MSG_CMD_SET_WHITE_BALANCE_MODE,
	MSG_CMD_SET_WB_ONEPUSH_TRIGGER,
	MSG_CMD_SET_WHITE_BALANCE_CRGAIN,
	MSG_CMD_SET_WHITE_BALANCE_CBGAIN,
	MSG_CMD_SET_WIDEDYNRANGE,
	MSG_CMD_SET_WIDEDYNRANGE_LEVEL,
	MSG_CMD_SET_2DNOISEREDUCTION_MODE,
	MSG_CMD_SET_2DNOISEREDUCTION_LEVEL,
	MSG_CMD_SET_3DNOISEREDUCTION_MODE,
	MSG_CMD_SET_3DNOISEREDUCTION_LEVEL,
	MSG_CMD_SET_BLC_MODE,
	MSG_CMD_SET_BLC_LEVEL,
	MSG_CMD_SET_FLICKER_CONTROL,
	MSG_CMD_SET_EXP_COMPENSATION_MODE,
	MSG_CMD_SET_EXP_COMPENSATION_LEVEL,
	MSG_CMD_SET_EXP_MODE,
	MSG_CMD_SET_EXP_PRIORITY,
	MSG_CMD_SET_MAX_EXPTIME,
	MSG_CMD_SET_MIN_EXPTIME,
	MSG_CMD_SET_MAX_EXPGAIN,
	MSG_CMD_SET_MIN_EXPGAIN,
	MSG_CMD_SET_EXPTIME,
	MSG_CMD_SET_EXPGAIN,
	MSG_CMD_SET_AGAIN,
	MSG_CMD_SET_DGAIN,
	MSG_CMD_SET_ISPGAIN,
	MSG_CMD_SET_MAX_IRIS,
	MSG_CMD_SET_MIN_IRIS,
	MSG_CMD_SET_IRIS,
	MSG_CMD_SET_FOCUS,
	MSG_CMD_SET_FOCUS_TRIGGER,
	MSG_CMD_SET_FOCUS_POSITION,
	MSG_CMD_GET_FOCUS_POSITION,
	MSG_CMD_SET_PICTURE_EFFECT,
	MSG_CMD_SET_DEFOGGING_MODE,
	MSG_CMD_SET_DEFOGGING_LEVEL,
	MSG_CMD_SET_GAMMA_MODE,
	MSG_CMD_SET_GAMMA_OFFSET,
	MSG_CMD_SET_FLIPMIRROR,

	MSG_CMD_SET_DI_ACTIV_EMODE,
	MSG_CMD_GET_DI_VOLTAGE_LEVEL,
	MSG_CMD_SET_DO_ACTIV_EMODE,
	MSG_CMD_SET_DO_LOGICAL_STATE,
	MSG_CMD_GET_DO_VOLTAGE_LEVEL,
	MSG_CMD_GET_RECOVERY_VOLTAGE_LEVEL,
	
	MSG_CMD_SET_MOTION_SETTINGS,
	MSG_CMD_GET_MOTION_STATUS,
	
	MSG_CMD_SET_PTZ_SETTINGS,
	
	MSG_CMD_POLLING,

	MSG_CMD_SET_IMAGINGSETTINGS,
	MSG_CMD_SET_VENC_CONFIG,
//	MSG_CMD_SET_VIDEOOUTPUT,
	MSG_CMD_SET_AUDIOIN_CONFIG,
	MSG_CMD_SET_RESTART_VENC,
	MSG_CMD_SET_RESTART_AUDIO,
	MSG_CMD_SET_AUDIO_SOURCE,
	MSG_CMD_SET_AUDIO_OUTPUT_GAIN,
	MSG_CMD_SET_AUDIO_OUTPUT_FILE,
	MSG_CMD_GET_SDCARD_DETECT,
	MSG_CMD_SET_CODEC_PTS,
	MSG_CMD_RESTART,
	MSG_CMD_SET_OSD_NAME,
	MSG_CMD_SET_OSD_TIME,
//	MSG_CMD_SET_MASK,
//	MSG_CMD_SET_ALL_MASK_RECT,
	
	MSG_CMD_DRAW_MASK_RECT,
	MSG_CMD_GET_MASK_ATTR,
	MSG_CMD_SET_MASKS_VISIBILITY,
	MSG_CMD_GET_MASKS_VISIBILITY,
	MSG_CMD_SET_MASKS_COLOR,
	MSG_CMD_GET_MASKS_COLOR,

	MSG_CMD_ADD_RECT_ID,
	MSG_CMD_DEL_RECT_ID,
	MSG_CMD_DRAW_RECT,
	MSG_CMD_GET_RECT_ATTR,

	MSG_CMD_DRAW_NAME,
	MSG_CMD_GET_NAME_ATTR,
	MSG_CMD_SET_NAME_VISIBILITY,
	MSG_CMD_GET_NAME_VISIBILITY,
	
	MSG_CMD_DRAW_TIME,
	MSG_CMD_GET_TIME_ATTR,
	MSG_CMD_SET_TIME_VISIBILITY,
	MSG_CMD_GET_TIME_VISIBILITY,

	MSG_CMD_DRAW_TEXT,
	MSG_CMD_GET_TEXT_ATTR,
	MSG_CMD_SET_TEXT_VISIBILITY,
	MSG_CMD_SET_IMG_WDR,
	MSG_CMD_GET_IMG_WDR,
	MSG_CMD_GET_IMG_GAIN,
	MSG_CMD_GET_IMG_AGAIN,
	MSG_CMD_GET_IMG_DGAIN,
	MSG_CMD_GET_IMG_ISPGAIN,
	MSG_CMD_GET_IMG_EXPTIME,
	MSG_CMD_SET_PTZF_ZOOM_MODE,
	MSG_CMD_GET_PTZF_ZOOM_MODE,
	MSG_CMD_SET_PTZF_ZOOM,
	MSG_CMD_GET_PTZF_ZOOM,
	MSG_CMD_SET_PTZF_DIGITAL_ZOOM,
	MSG_CMD_GET_PTZF_DIGITAL_ZOOM,
	MSG_CMD_SET_PTZF_OPTICAL_ZOOM,
	MSG_CMD_GET_PTZF_OPTICAL_ZOOM,
	MSG_CMD_GET_PTZF_OPTICAL_ZOOM_STATE,
	MSG_CMD_SET_VS_FRAMERATE,
	MSG_CMD_GET_VS_FRAMERATE,
	MSG_CMD_SET_VIDEO_FREEZE_FRAME,
	MSG_CMD_GET_VIDEO_FREEZE_FRAME,
	MSG_CMD_SET_VO_ATTR,
	MSG_CMD_GET_VO_ATTR,
	MSG_CMD_SET_VIDEO_ONOFF,
	MSG_CMD_SET_VIDEO_ATTR,
	MSG_CMD_GET_VIDEO_ONOFF,
	MSG_CMD_GET_VIDE_ATTR,
	MSG_CMD_SET_PRESET_NAME,
	MSG_CMD_LOAD_PRESET,
	MSG_CMD_SAVE_PRESET,
	MSG_CMD_CLEAR_PRESET,
	MSG_CMD_GET_PWRON_PRESET,
	MSG_CMD_SET_PWRON_PRESET,
	MSG_CMD_GET_PRESET_FIRST_NUMBER,
	MSG_CMD_GET_PRESET_LAST_NUMBER,
	MSG_CMD_GET_PRESET_PWRON_NUMBER,
	MSG_CMD_GET_PRESET_CURRENT_NUMBER,
	MSG_CMD_GET_PRESET_NUMBER,
	MSG_CMD_GET_PRESET_TYPE,
	MSG_CMD_SET_PRESET_TYPE,
	MSG_CMD_SAVE_LTI,
	MSG_CMD_LOAD_LTI,
	MSG_CMD_SET_UTC_ENTER,
	MSG_CMD_SET_UTC_UP,
	MSG_CMD_SET_UTC_DOWN,
	MSG_CMD_SET_UTC_RIGHT,
	MSG_CMD_SET_UTC_LEFT,
	MSG_CMD_END
};


/* system control */
/**
* @brief Commands used to control system server.
*/
enum {
	SYS_MSG_GET_DHCP_EN = 1, ///< Get DHCP status.
	SYS_MSG_GET_HTTP_PORT, ///< Get http port.
	SYS_MSG_SET_NETMASK, ///< Set new netmask of IP netcam.
	SYS_MSG_GET_DNS, ///< Get DNS address.
	SYS_MSG_GET_SNTP_FQDN, ///< Get SNTP FQDN.
	SYS_MSG_SET_TIMEFORMAT, ///< Set display time format.
	SYS_MSG_GET_DEVICE_TYPE, ///< Get device type.
	SYS_MSG_GET_TITLE, ///< Get IP netcam title.
	SYS_MSG_GET_REGUSR, ///< Get IP netcam title.
	SYS_MSG_GET_OSDTEXT, ///< Get IP netcam title.
	SYS_MSG_GET_MAC, ///< Get ethernet MAC address.
	SYS_MSG_GET_IP, ///< Get IP netcam IP address.
	SYS_MSG_GET_GATEWAY, ///< Get IP netcam gateway.
	SYS_MSG_GET_NETMASK, ///< Get IP netcam netmask.
	SYS_MSG_GET_DHCP_CONFIG, ///< Get DHCP status.
	SYS_MSG_GET_USER_AUTHORITY, ///< Get user authority by name.
	SYS_MSG_GET_DI_VOLTAGE_LEVEL,
	SYS_MSG_GET_DO_VOLTAGE_LEVEL,
	SYS_MSG_GET_MOTION_STATUS,
	SYS_MSG_GET_FOCUS_POSITION,
	SYS_MSG_GET_MAX,
	
	//SYS_MSG_GET_UPNP_PORT,
	SYS_MSG_SET_IP, ///< Set static IP address.
	SYS_MSG_SET_REGUSR, ///< Set IP netcam title.
//	SYS_MSG_SET_OSDTEXT, ///< Set IP netcam title.
	SYS_MSG_SET_GATEWAY, ///< Set gateway in static IP mode.
	SYS_MSG_SET_DHCPC_ENABLE, ///< Set DHCP enable.
	
	SYS_MSG_SET_DNS, ///< Set DNS address.
	SYS_MSG_SET_HOSTNAME,    ///<set hostname.	
	SYS_MSG_SET_SNTP_SERVER_IP, ///< Set SNTP FQDN.
//    SYS_MSG_SET_LOSTALARM, ///< Enable ethernet lost alarm or not.
//SYS_MSG_SET_SDAENABLE, ///< Enable save file to SD card when alarm happen or not.
	SYS_MSG_SET_IMAGE_SOURCE, ///< Select NTSC/PAL mode.
	/* Live Video Page */
	SYS_MSG_SET_CLICK_SNAP_FILENAME,
	SYS_MSG_SET_CLICK_SNAP_STORAGE,
	/* Video & Image */
	SYS_MSG_SET_TITLE, ///< Set IP netcam title.
	SYS_MSG_SET_LOCAL_DISPLAY,
	SYS_MSG_SET_FLIPMIRROR,
	SYS_MSG_SET_ENCRYPTION,
	/* Video Analytics */
	SYS_MSG_SET_MOTION,
	SYS_MSG_SET_PTZ,
//	SYS_MSG_SET_MOTIONENABLE, ///< Enable motion detection or not.
//	SYS_MSG_SET_MOTIONCENABLE, ///< Use customer define or normal mode.
//	SYS_MSG_SET_MOTIONLEVEL, ///< Set motion level in normal mode.
//	SYS_MSG_SET_MOTIONCVALUE, ///< Set customer value at customer mode.
//	SYS_MSG_SET_MOTIONBLOCK, ///< Define motion blocks.
	/* Image settings */
	SYS_MSG_SET_BRIGHTNESS, ///< Set brightness.
	SYS_MSG_SET_CONTRAST, ///< Set contrast.
	SYS_MSG_SET_SATURATION, ///< Set saturation.
	SYS_MSG_SET_HUE,
	SYS_MSG_SET_SHARPNESS, ///< Set sharpness.
	SYS_MSG_SET_CLEARSYSLOG, ///< Clear System Log
	SYS_MSG_SET_CLEARACCESSLOG, ///< Clear Access Log
	SYS_MSG_SET_IRCUTFILTER,
	SYS_MSG_SET_IRCUTFILTER_RESPONSETIME,
	SYS_MSG_SET_IRCUTFILTER_THRESHOLD,
	SYS_MSG_SET_WHITE_BALANCE_MODE, ///< Set whitebalance moe of IP netcam.
	SYS_MSG_SET_WB_ONEPUSH_TRIGGER,
	SYS_MSG_SET_WHITE_BALANCE_CRGAIN,
	SYS_MSG_SET_WHITE_BALANCE_CBGAIN,
	SYS_MSG_SET_WIDEDYNRANGE_MODE,
	SYS_MSG_SET_WIDEDYNRANGE_LEVEL,
	SYS_MSG_SET_2DNOISEREDUCTION_MODE,
	SYS_MSG_SET_2DNOISEREDUCTION_LEVEL,
	SYS_MSG_SET_3DNOISEREDUCTION_MODE,
	SYS_MSG_SET_3DNOISEREDUCTION_LEVEL,
	SYS_MSG_SET_BLC_MODE, ///< Set backlight mode of IP netcam.
	SYS_MSG_SET_BLC_LEVEL,
	SYS_MSG_SET_FLICKER_CONTROL,
	SYS_MSG_SET_EXPMODE,
	SYS_MSG_SET_EXPPRIORITY,
	SYS_MSG_SET_MAXEXPTIME,
	SYS_MSG_SET_MINEXPTIME,
	SYS_MSG_SET_EXPTIME,
	SYS_MSG_SET_MAXEXPGAIN,
	SYS_MSG_SET_MINEXPGAIN,
	SYS_MSG_SET_EXPGAIN,
	SYS_MSG_SET_AGAIN,
	SYS_MSG_SET_DGAIN,
	SYS_MSG_SET_ISPGAIN,
	SYS_MSG_SET_FOCUS,
	SYS_MSG_SET_FOCUS_TRIGGER,
	SYS_MSG_SET_FOCUS_POSITION,
	SYS_MSG_SET_PICTURE_EFFECT,
	SYS_MSG_SET_DEFOGGING_MODE,
	SYS_MSG_SET_DEFOGGING_LEVEL,
	SYS_MSG_SET_GAMMA_MODE,
	SYS_MSG_SET_GAMMA_OFFSET,
	SYS_MSG_SET_LDC,
    /* Audio Page */
	SYS_MSG_SET_AUDIOENABLE, ///< Enable audio send to web site or not.
	SYS_MSG_SET_AUDIOMODE,
	SYS_MSG_SET_AUDIOINVOLUME, ///< Set alarm audio volume
	SYS_MSG_SET_AUDIO_ENCODE,
	SYS_MSG_SET_AUDIO_SAMPLERATE,
	SYS_MSG_SET_AUDIO_BITRATE,
	SYS_MSG_SET_AUDIO_ALARMLEVEL,
	SYS_MSG_SET_AUDIOOUTVOLUME,
	SYS_MSG_SET_AUDIORECEIVER_ENABLE,
	SYS_MSG_SET_AUDIOSERVER_IP,
    /* Date Time Page */
	SYS_MSG_SET_DAYLIGHT, ///< Set system time to use daylight or not.
	SYS_MSG_SET_TIMEZONE, ///< Set system time zone.
	SYS_MSG_SET_DATETIME_TYPE,
    /* Network & Port Page */
	SYS_MSG_SET_MULTICAST,///< Set Multicast fuction enable / disable.
	SYS_MSG_SET_HTTPPORT, ///< set web site port. This operation may take serveral time.
	SYS_MSG_SET_HTTPSPORT,
	SYS_MSG_SET_GIOINTYPE, ///< Set GIO input alarm triger type(high triger or low triger).
	SYS_MSG_SET_GIOOUTTYPE, ///< Set GIO output high or low.
	SYS_MSG_SET_RS485,
	SYS_MSG_SET_DI_ACTIVEMODE,
	
	SYS_MSG_SET_DO_ACTIVEMODE,
	SYS_MSG_SET_DO_LOGICAL_STATE,
	

	/* Maintainence */
	SYS_MSG_SET_DEFAULT,
	/**********************/
//	SYS_MSG_SET_IMAGE_DEFAULT, ///< Set all image setting to default.
	SYS_MSG_DO_LOGIN, ///< Do user login.
	SYS_MSG_ADD_USER, ///< Add/change user acount.
	SYS_MSG_DEL_USER, ///< Delete user acount.
	SYS_MSG_SET_LANGUAGE,
	SYS_MSG_SET_GIOINENABLE, ///< Enable GIO input alarm or not.
	SYS_MSG_SET_GIOOUTENABLE, ///< Enable GIO ouput at alarm or not.
	SYS_MSG_QUIT, ///< Make system server quit processing.
	SYS_MSG_DO_BOOT_PROC,///< Tell system server to do something about boot control.
        SYS_MSG_ADD_PROFILE,  ///create onvif profile.....
        SYS_MSG_ADD_VIDEOSOURCE_CONF,        //add video source confiuration .
	SYS_MSG_SET_AUDIOSOURCE_CONF,
        SYS_MSG_SET_VIDEOSOURCE_CONF,
        SYS_MSG_SET_IMAGING_CONF,
        SYS_MSG_ADD_VIDEOENCODER_CONF,
        SYS_MSG_SET_VIDEOENCODER_CONF,
        SYS_MSG_DEL_VIDEOENCODER_CONF,
	SYS_MSG_ADD_AUDIO_SOURCE_CONF,
	SYS_MSG_ADD_AUDIO_ENCODER_CONF,
	SYS_MSG_SET_AUDIO_SOURCE_CONF,
	SYS_MSG_SET_AUDIO_ENCODER_CONF,
	SYS_MSG_DEL_AUDIO_SOURCE_CONF,
	SYS_MSG_DEL_AUDIO_ENCODER_CONF,
        SYS_MSG_DEL_PROFILE,
        SYS_MSG_SET_DIS_MODE,            //set discovery mode
        SYS_MSG_ADD_SCOPE,                //add scopes
        SYS_MSG_DEL_SCOPE,                //remove scopes
#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
	SYS_MSG_SET_SCOPE,                //Set Scopes
#endif
        SYS_MSG_SET_ANALYTICS,             //set video analytics
        SYS_MSG_SET_OUTPUT,             //set video output
	SYS_MSG_SET_RELAY,		//set relays
	SYS_MSG_SET_RELAY_LOGICALSTATE,
	SYS_MSG_SET_METADATA,
	SYS_MSG_ADD_METADATA,
#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
	SYS_MSG_DEL_METADATA,
#endif
	SYS_MSG_SET_STREAMCODEC,
//	SYS_MSG_SET_VIDEOOUTPUT,
	SYS_MSG_SET_AUDIO,
	SYS_MSG_SET_AUDIO_SOURCE,
	SYS_MSG_SET_AUDIO_OUTPUT_GAIN,
	SYS_MSG_SET_AUDIO_OUTPUT_FILE,
	SYS_MSG_SET_SD_LOOPRECORDING,
	SYS_MSG_SET_RECORDING,
	SYS_MSG_SET_DO_FWUPDATE,
	SYS_MSG_SET_RECOVER,
	SYS_MSG_SET_OSDNAME,
	SYS_MSG_SET_OSDTIME,
	SYS_MSG_SET_EXPOSURE_COMPENSATION_MODE,
	SYS_MSG_SET_EXPOSURE_COMPENSATION_LEVEL,
	SYS_MSG_SET_MAX_IRIS,
	SYS_MSG_SET_MIN_IRIS,
	SYS_MSG_SET_IRIS,
//	SYS_MSG_SET_MASK,
//	SYS_MSG_SET_MASK_RECT,
	SYS_MSG_MAX
};

enum{
	REC_MSG_SET_RECORDING_CONFIG = 1,
	REC_MSG_SET_LOOP_RECORDING,
	REC_MSG_MAX
};

enum {
	MSG_CMD_SET_SERIAL_ATTRS = 1,
	MSG_CMD_GET_SERIAL_ATTRS,
	MSG_CMD_SET_SERIAL_TXD_TYPE,
	MSG_CMD_GET_SERIAL_TXD_TYPE,
	MSG_CMD_GET_SERIAL_RXD,
	MSG_CMD_SEND_SERIAL_TXD,
	MSG_CMD_SERIAL_MAX
};
/**
* @brief Commands used to communicate with file manager.

*/
typedef enum {
	FILE_MSG_QUIT, ///< Let file manager shutdown.
	FILE_MSG_READ, ///< Read system settings from file manager.
	FILE_MSG_WRITE_I, ///< Tell file manager to write system config immediately.
	FILE_MSG_WRITE, ///< Tell file manager to write system config.
	FILE_MSG_WRITE_VENC,
	FILE_MSG_WRITE_AENC,
	FILE_MSG_WRITE_RECORDING,
	FILE_MSG_WRITE_MD,
	FILE_MSG_WRITE_PTZ,
	FILE_MSG_WRITE_OSD,
	FILE_MSG_WRITE_MASK,
	FILE_MSG_WRITE_NET,
	FILE_MSG_WRITE_USERLIST,
	FILE_MSG_WRITE_DATETIME,
	FILE_MSG_WRITE_VO,
	FILE_MSG_WRITE_FOCUS,
	FILE_MSG_WRITE_PRESET,
	FILE_MSG_WRITE_SERIAL,
	FILE_MSG_FORMAT, ///< Ask file manager to format NAND flash.
	FILE_MSG_RESET, ///< Reset to default.
	FILE_MSG_WRITE_LNAME,
	FILE_GET_MID, ///< Get file manager's share memory ID.
	FILE_MSG_CLEAR_SYSLOG,
	FILE_MSG_CLEAR_ACCESSLOG,
	FILE_MSG_MAX ///< File manager command number.
} FileCmd_t;

typedef enum {
	FMSG_NETFIELD_SET_DHCP,
	FMSG_NETFIELD_SET_IP,
	FMSG_NETFIELD_SET_MASK,
	FMSG_NETFIELD_SET_GATEWAY,
	FMSG_NETFIELD_SET_DNS,
	FMSG_NETFIELD_SET_HTTP_PORT
} FileMsgNetField_t;

typedef enum {
	FMSG_DATETIME_SET_TYPE,
	FMSG_DATETIME_SET_NTP,
	FMSG_DATETIME_SET_TZ
} FileMsgDateTimeField_t;

typedef enum{
	FMSG_AUDIO_AUDIO1,
	FMSG_AUDIO_SOURCE1,
	FMSG_AUDIO_OUTPUT
} FileMsgAudioField_t;

typedef enum{
	FMSG_SD_LOOPRECORDING,
	FMSG_RECORDING_VIDEO1,
	FMSG_RECORDING_VIDEO2,
	FMSG_RECORDING_VIDEO3
} FileMsgRecordingField_t;

typedef enum{
//	FMSG_MD_SETTINGS,
	FMSG_MD_VS1=0,
	FMSG_MD_VS2=1,
	FMSG_MD_VS3=2,
	FMSG_MD_VS4=3,
	FMSG_MD_VS5=4,
	FMSG_MD_VS6=5,
	FMSG_MD_VS7=6,
	FMSG_MD_VS8=7,
	FMSG_MD_ALL=8
} FileMsgMotionField_t;

typedef enum{
	FMSG_PTZ_SETTINGS
} FileMsgPTZField_t;

typedef enum{
	FMSG_FOCUS_ATTRS,
	FMSG_FOCUS_MANUAL_POS
} FileMsgFocusField_t;

typedef enum{
	FMSG_OSD_NAME,
	FMSG_OSD_TIME,
	FMSG_OSD_TEXT1,
	FMSG_OSD_TEXT2,
	FMSG_OSD_TEXT3,
	FMSG_OSD_TEXT4,
	FMSG_OSD_TEXT5,
	FMSG_OSD_TEXT6,
	FMSG_OSD_TEXT7,
	FMSG_OSD_TEXT8,
	FMSG_OSD_TEXT9,
	FMSG_OSD_TEXT10,
	FMSG_OSD_TEXT11,
} FileMsgOSDField_t;

typedef enum{
	FMSG_MASK_MASK,
	FMSG_MASK_RECT1,
	FMSG_MASK_RECT2,
	FMSG_MASK_RECT3,
	FMSG_MASK_RECT4,
	FMSG_MASK_RECT5,
	FMSG_MASK_RECT6,
	FMSG_MASK_RECT7,
	FMSG_MASK_RECT8,
	FMSG_MASK_RECT9,
	FMSG_MASK_RECT
} FileMsgMaskField_t;

typedef enum{
	FMSG_VIDEO_SOURCE,
	FMSG_VIDEO_OUTPUT
} FileMsgVideoField_t;

typedef enum{
	FMSG_PRESET_NUM,
	FMSG_PRESET_NAME,
	FMSG_PRESET_PWRON
} FileMsgPresetField_t;

typedef enum{
	FMSG_SERIAL_ATTRS,
	FMSG_SERIAL_TXD
} FileMsgSerialField_t;
/**
* @ingroup MSG_UTIL
* @brief Message type

*/
enum {
	MSG_TYPE_START = 0,

	MSG_TYPE_MSG1, ///< Message type 1. Always reserved for server.
	MSG_TYPE_MSG2, ///< Message type 2.
	MSG_TYPE_MSG3, ///< Message type 3.
	MSG_TYPE_MSG4, ///< Message type 4.
	MSG_TYPE_MSG5, ///< Message type 5.
	MSG_TYPE_MSG6, ///< Message type 6.
	MSG_TYPE_MSG7, ///< Message type 7.
	MSG_TYPE_MSG8, ///< Message type 8.
	MSG_TYPE_MSG9, ///< Message type 9.
	MSG_TYPE_MSG10, ///< Message type 10.
	MSG_TYPE_MSG11, ///< Message type 11.
	MSG_TYPE_MSG12, ///< Message type 12.
	MSG_TYPE_MSG13, ///< Message type 13.
	MSG_TYPE_MSG14, ///< Message type 14.

	MSG_TYPE_END
};

typedef enum {
	FMT_H265 = 0,
	FMT_H265_SD,
	FMT_H265_CIF,
	FMT_H264,
	FMT_H264_SD,
	FMT_H264_CIF,
	FMT_MJPEG,
	FMT_MJPEG2,
	FMT_AUDIO,
	FMT_MAX_NUM
} FrameFormat_t;

typedef struct _PackInfo{
	int offset;
	int size;
}PackInfo_t;

typedef struct _FrameInfo{
	int fileName;
	int frameCnt;
	int packCnt;
	PackInfo_t packInfo[4];
	int width;
	int height;
	int format;
	int vencChn;
//	int frameType;
	unsigned long long timestamp;
	pid_t pid;
	int numStr;
	int framerate;
	int bitrate;
}FrameInfo_t;

typedef struct _MSG_MEM{
#if 1
	unsigned long data[128];
#else
	unsigned long addr;
	unsigned long size;
	unsigned long rev1[8];
	unsigned long rev2[8];
	unsigned long rev3[8];
	unsigned long rev4[8];
	unsigned long rev5[8];
//	unsigned long rev6[9];
#endif
}MSG_MEM;

typedef struct _MSG_BUF {
	long 		Des; ///< message type, must be > 0 .
	int 		cmd; ///< data .
	int			Src;
	int			ret;
	MSG_MEM		mem_info;
	FrameInfo_t	frame_info;
} MSG_BUF;

typedef struct _SERIAL_MSG_BUF {
	long 		Des; ///< message type, must be > 0 .
	int 		cmd; ///< data .
	int			Src;
	int			ret;
	MSG_MEM		mem_info;
//	serialAttr		uartInfo;
} SERIAL_MSG_BUF;

typedef struct TARA_MSG_BUF_s {
	long 		Des; ///< message type, must be > 0 .
	int 		cmd; ///< data .
	int			Src;
	int			ret;
	int			idx;
	MSG_MEM		mem_info;
	FrameInfo_t	frame_info;
} TARA_MSG_BUF;

/**
* @brief System message driver buffer type
*/
typedef struct SYS_MSG_BUF{
	long 		Des; ///< Where this message go. See /ref Sys_Msg_Def.h
	int			cmd; ///< command to control system server.
	int			src; ///< Where this message from. See /ref Sys_Msg_Def.h
	int			shmid; ///< Share memory which was used to communicate with system server.
	int			length; ///< Data length.
	int			offset; ///< Offset to begin of share memory.
} SYS_MSG_BUF;

/**
* @brief File message buffer type

* This structure is used to communicate with file manager.
*/
typedef struct _FILE_MSG_BUF{
	long 		Des; ///< Where this message go.
	FileCmd_t	cmd; ///< Message command ID.
	int			src; ///< Where this message from.
	int			shmid; ///< Share memory ID which is used to communicate.
	int			shmidx; ///< Share memory index (0 or 1). 0 is used for system settings; and 1 is used for system log.
	int			length; ///< Data length in share memory.
	int			offset; ///< Offset to begin of share memory.
	int			nPageNum; ///< System log page number.
	int			nItemNum; ///< System log item number.
} FILE_MSG_BUF;

#if 0

/**
* @ingroup ALARM_MSG_DRV
* @brief Commands used to communicate with alarm manager.

*/
typedef enum {
	ALARM_EVENT_MOTION_DETECT, ///< Motion detection alarm.
	ALARM_EVENT_AUDIO_TRIG,
	ALARM_EVENT_MOTION_AUDIO, ///< Motion detection audio alarm.
	ALARM_EVENT_ALARMRESET_TRIG, ///< Reset alarm.
	ALARM_EVENT_ALARMIN_TRIG, ///< External trigger alarm.
	ALARM_EVENT_ALARMETH_TRIG, ///< Ethernet lost alarm.
	ALARM_EVENT_SCHEDULE, ///< Schedule alarm.
	ALARM_EVENT_SCHEDULE_END, ///< Schedule end alarm.
	ALARM_EVENT_QUIT, ///<  Let alarm manager shutdown.
	ALARM_DMVAEVENT_TRIP_ZONE, ///<  DMVA Trip Zone alarm.
	ALARM_EVENT_MAX ///<  Alarm manager command number.
} Event_t;
/**
* @ingroup ALARM_MSG_DRV
* @brief Alarm message buffer type

* This structure is used to communicate with alarm manager.
*/
typedef struct REC_MSG_BUF{
	long 		Des; ///< Where this message go.
	int			src; ///< Where this message from.
//	Event_t		event; ///< Message command ID.
	int			index;
	unsigned char Recording;
	unsigned char RecordingMode;
	unsigned char StorgeLocation;
	char StorgeRelativePath[16];
//	int			serial;///< Motion detect JPEG serial.
//	int			alarmaudio_duration;///<Play alarm audio base on duration.
//	int			alarmaudio_enable;///<Restart the alarm audio.
} REC_MSG_BUF;
#endif

#define MSG_KEY			0x12345
#define SYS_MSG_KEY		0x36f9 ///< Message key used by system server.
#define FILE_MSG_KEY	0xc54be5 ///< File message key.
#define RECORDER_KEY	0x542c7ae6 ///< recorder message key.
#define SERIAL_KEY		0x82
//#define ALARM_KEY		0x542c7ae6 ///< Alarm message key.
#define AUD_MSG_KEY     0x31       ///< Audio alarm message key
#if 0 //stfu@20140714 FIX RDK_3.8 ONVIF
#define STREAMER_PATH "/opt/ipnc/wis-streamer" //path of wis-streamer Used for creating msg queue for wis-streamer
#define Message_Wis_Streamer 2   //Priority of Wis-streamer msg queue
#endif

/*
Pre define message usage by function
ex:
FuncName_Msg_Def.h
*/
#include <Stream_Msg_Def.h>
#include <Sys_Msg_Def.h>
#include <File_Msg_Def.h>

#if 0
#include <Alarm_Msg_Def.h>
#endif
/**
* @ingroup UTILITY API
* @defgroup MSG_QUEUE Message queue.

* This is the place to store message data for communication.
* @{
*/
int Msg_Init( int msgKey );
int Msg_Kill( int qid );
int Msg_Send( int qid, void *pdata , int size );
int Msg_Rsv( int qid, int msg_type, void *pdata , int size );
int Msg_Send_Rsv( int qid, int msg_type, void *pdata , int size );



#if 0 //stfu@20140714 FIX RDK_3.8 ONVIF
int msg_queue_wis_streamer(void);
#endif
/** @} */


#define FFLAG_TNF		(0)
#define FFLAG_SNF		(1)
#define FFLAG_SNF_TNF	(2)
#if 0

#define ALL_ADV_FTR		(0)
#define NO_VS_LDC_FTR	(1)
#define NO_ADV_FTR		(2)

#define MAX_GUI_ROI	(3)

typedef enum{
	FACE_NO_RECOG = 0,
	FACE_RECOGIZE,
	FACE_REGUSER,
	FACE_DELUSER
}FACE_RECOG_TYPE;

typedef enum{
	DEMO_NONE=0,
	VS_DEMO,
	VNF_DEMO,
	LDC_DEMO,
	FD_DEMO,
	ROI_FD_DEMO,
	ROI_CENTER_DEMO,
	MAX_DEMO_MODE
}ADV_DEMO_ENUM;

typedef enum{
	ROI_NONE = 0,
	ROI_FD_CFG,
	ROI_CENTER_CFG
}ROI_TYPE;

typedef enum{
	SALDRE_SYS_NONE = 0,
	SALDRE_SYS_GLOBAL,
	SALDRE_SYS_LOCAL,
	SALDRE_SYS_ADAPTIVE
} SALDRE_MODE_TYPE;


typedef struct _FaceDetectParam
{
  int	   fdetect; 					    ///< facedetect (ON/OFF)
  int      startX;
  int      startY;
  int      width;
  int      height;
  int      fdconflevel;
  int      fddirection;
  int 	   frecog;
  int	   frconflevel;
  int	   frdatabase;
  int	   pmask;
  int      maskoption;
}FaceDetectParam;

typedef struct _ApproMotionPrm{
	int bMotionEnable;
	int bMotionCEnale;
	int MotionLevel;
	int MotionCValue;
	int MotionBlock;
}ApproMotionPrm;

typedef struct _OSDPrm{
	int detailedInfo;
	int	dateEnable;
	int	timeEnable;
	int	logoEnable;
	int logoPos;
	int	textEnable;
	int textPos;
	char text[24];
}OSDPrm;

typedef struct _DateTimePrm{
	int dateFormat;
	int datePos;
	int timeFormat;
	int timePos;
}DateTimePrm;

typedef struct _CodecAdvPrm{
	int ipRatio;
	int fIframe;
	int qpInit;
	int qpMin;
	int qpMax;
	int meConfig;
	int packetSize;
}CodecAdvPrm;

typedef struct _ROI_Prm{
	int startx;
	int starty;
	int width;
	int height;
}ROI_Prm;

typedef struct _CodecROIPrm{
	char 	numROI;
	ROI_Prm	roi[3];
}CodecROIPrm;
#endif
typedef struct _DynRangePrm{
	int enable;
	int mode;
	int level;
}DynRangePrm;
#if 0
typedef struct _VnfParam{
	int enable;
	int mode;
	int strength;
}VnfParam;

typedef struct _OsdTextPrm{
	int nLegnth;
	char strText[16];
}OsdTextPrm;

typedef struct _VencPrm{
	int num;
	StreamParam VencParam;
}VencPrm;
#endif

#if defined (__cplusplus)
}
#endif

#endif
/**
* @page MSG_QUEUE_HOW How to use message queue API?
* 1. Call the API Msg_Init( int msgKey ) to initialize message queue.\n
* 2. Now you can use any message queue API functions as you want.\n
* 3. Call the API Msg_Kill( int qid )  to cleanup message queue.
* @section MSG_QUEUE_HOW_EX Example
* @code
#include <Msg_Def.h>
int main()
{
	int qid, key;
	if((qid=Msg_Init(key)) < 0){
		return -1;
	}
	// More message queue API
	Msg_Kill(qid );
	return 0;
}
* @endcode
*/
