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
#ifndef __SYSTEM_DEFAULT_H__
#define __SYSTEM_DEFAULT_H__

#define VIDEO_CFG	 "/var/cfg/video.cfg"
#define VIDEO_PLATFORM_CFG	 "/var/cfg/video_platform.cfg"
#define AUDIO_CFG	 "/var/cfg/audio.cfg"
#define IMAGING_CFG "/var/cfg/imaging.cfg"
#define INFO_CFG "/var/cfg/info.cfg"
#define FEATURE_CFG "/var/cfg/feature.cfg"
#define USER_LIST "/var/cfg/accounts.cfg"
#define DATETIME_CFG "/var/cfg/time.cfg"
#define RECORDING_CFG "/var/cfg/recording.cfg"
#define MOTION_CFG "/var/cfg/motion.cfg"
#define MOTION_PLATFORM_CFG "/var/cfg/motion_platform.cfg"
#define	PTZF_CFG	"/var/cfg/ptzf.cfg"
#define	OSD_CFG	"/var/cfg/osd.cfg"
#define	STREAMING_CFG	"/var/cfg/streaming.cfg"
#define	VO_CFG	"/var/cfg/videooutput.cfg"
#define	IO_CFG	"/var/cfg/io.cfg"
#define	MASK_CFG	"/var/cfg/mask.cfg"
#define	PRESET_CFG	"/var/cfg/preset.cfg"
#define	SERIAL_CFG	"/var/cfg/serial.cfg"
#define NET_INTERNAL_CFG	"/var/cfg/net_internal.cfg"

#define VIDEO_DEFAULT_CFG	 "/var/cfg/video_default.cfg"
#define AUDIO_DEFAULT_CFG	 "/var/cfg/audio_default.cfg"
#define IMAGING_DEFAULT_CFG "/var/cfg/imaging_default.cfg"
#define INFO_DEFAULT_CFG "/var/cfg/info_default.cfg"
#define FEATURE_DEFAULT_CFG "/var/cfg/feature_default.cfg"
#define USER_DEFAULT_LIST "/var/cfg/accounts_default.cfg"
#define DATETIME_DEFAULT_CFG "/var/cfg/time_default.cfg"
#define RECORDING_DEFAULT_CFG "/var/cfg/recording_default.cfg"
#define MOTION_DEFAULT_CFG "/var/cfg/motion_default.cfg"
#define	PTZF_DEFAULT_CFG	"/var/cfg/ptzf_default.cfg"
#define	OSD_DEFAULT_CFG	"/var/cfg/osd_default.cfg"
#define	STREAMING_DEFAULT_CFG	"/var/cfg/streaming_default.cfg"
#define	VO_DEFAULT_CFG	"/var/cfg/videooutput_default.cfg"
#define	IO_DEFAULT_CFG	"/var/cfg/io_default.cfg"
#define	MASK_DEFAULT_CFG	"/var/cfg/mask_default.cfg"
#define	PRESET_DEFAULT_CFG	"/var/cfg/preset_default.cfg"
#define	SERIAL_DEFAULT_CFG	"/var/cfg/serial_default.cfg"

/* Macro */
#define IPV4(a,b,c,d)		((d<<24)|(c<<16)|(b<<8)|(a))

#define STREAM_PATH		"/tmp/stream"
#define SYS_FILE			"/tmp/sysenv.cfg"
#define LOG_FILE			"/tmp/logfile.log"
#define SYS_LOG_FILE 		"/tmp/systemlog.log"
#define ACCESS_LOG_FILE 	"/tmp/accesslog.log"

#define TITLE_DEFAULT				"NMD265V4"
#define HOST_DEFAULT        "NMD265V4"
#define MODELNAME_DEFAULT		"NMHV8274"
#define	SENSOR_DEFAULT			"IMX274"
#define CHIP_DEFAULT				"Hi3519V101"
#define	LOCATION_DEFAULT		"camera_default"

#if defined(ST240) || defined(ST230) || defined(NMHA5326S) || defined(NMHD4689G) || defined(NMHC2291S) || defined(ST210) || defined(ST220) || defined(NMHD2422P)
#define	UBOOTENVOFFSER_DEFAULT		327680
#elif defined(NMHC2323D)
#define	UBOOTENVOFFSER_DEFAULT		393216
#else
#define	UBOOTENVOFFSER_DEFAULT		262144
#endif

#define TIMEZONE_DEFAULT		"UTC+08:00"
#if defined(VCM210P)
#define IP_DEFAULT					IPV4(192,168,100,100)
#define GATEWAY_DEAFULT				IPV4(192,168,100,254)
#else
#define IP_DEFAULT					IPV4(192,168,  1,168)
#define GATEWAY_DEAFULT				IPV4(192,168,  1,  1)
#endif
#define NETMASK_DEFAULT				IPV4(255,255,255,  0)

#define DNS_DEFAULT					IPV4(  8,  8,  8,  8)
#define HTTP_PORT_DEFAULT			8080
#define HTTPS_PORT_DEFAULT			81
#define DHCP_ENABLE_DEFAULT			0 //stfu@20131008 default to disable
#define NTP_SERVER_DEFAULT			"tw.pool.ntp.org" //stfu@20140703 Modified //"192.168.1.168"
#define NTP_TIMEZONE_DEFAULT		20
#define NTP_FREQUENCY_DEFAULT		0
#define TIME_FORMAT_DEFAULT			0
#define DAYLIGHT_DEFAULT			0
#define DEVICE_TYPE_DEFAULT			0x1687
#define MAC_DEFAULT					{0x00, 0x00, 0x00, 0x00, 0x00, 0x00} //stfu@20131008 Pacidal default, was{0x12,0x34,0x56,0x78,0x9a,0xbc}
#define FTP_SERVER_IP_DEFAULT		"192.168.1.1"
#define FTP_USERNAME_DEFAULT		"ftpuser"
#define FTP_PASSWORD_DEFAULT		"9999"
#define FTP_FOLDERNAME_DEFAULT		"default_folder"
#define FTP_IMAGEAMOUNT_DEFAULT 	5
#define FTP_PID_DEFAULT				0
#define SMTP_SERVER_IP_DEFAULT		"192.168.1.1"
#define SMTP_SERVER_PORT_DEFAULT	25
#define SMTP_AUTHENTICATION_DEFAULT	0
#define SMTP_USERNAME_DEFAULT		"smtpuser"
#define SMTP_PASSWORD_DEFAULT		"9999"
#define SMTP_SNEDER_EMAIL_DEFAULT	"user@domain.com"
#define SMTP_RECEIVER_EMAIL_DEFAULT	"user@domain.com"
#define SMTP_CC_DEFAULT				""
#define SMTP_SUBJECT_DEFAULT		TITLE_DEFAULT //miachen@20131213 "NBL306" //stfu@20131131 //"NBX305"
#define SMTP_TEXT_DEFAULT			"pacidal mail."
#define SMTP_ATTACHMENTS_DEFAULT	2
#define SMTP_VIEW_DEFAULT			0
#define MPEG4_RESOLUTION_DEF		0
#define MPEG4_RESOLUTION2_DEF		0
#define LIVE_RESOLUTION_DEF			0
#define MPEG4QUALITY_DEF			0
//#define JPEGQUALITY_DEF				80
#define MIRROR_DEF					0
#define ENCRYPT_DEF					0
#define M41SFEAT_DEF 				0
#define M42SFEAT_DEF				0
#define JPGSFEAT_DEF				0
#define FDETECT_DEF 				0
#define REGUSR_DEF					"User1"
//#define DEMOCFG_DEF					0
//#define	OSDSTREAM_DEF				0
//#define	OSDWINNUM_DEF				0
//#define	OSDWIN_DEF					0

//#define	MAXGAIN_DEF					3		//stfu@20131003 per mia change /*++ miachen@20130718, 0 for AGC ceiling setting ++*/
//#define	VS_DEFAULT					0
//#define	LDC_DEFAULT					0
//#define	VNF_DEFAULT					0
//#define	VNF_MODE_DEFAULT			2
//#define	VNF_STRENGTH_DEFAULT		0
//#define DYNRANGE_DEF				0
//#define DYNRANGESTR_DEF				1
#define	OSDTEXT_DEF					TITLE_DEFAULT   //miachen@20131213 "NBL306-RDK" //stfu@20131131 //"NBX305-RDK"
//#define	DUMMY_DEF					0
//#define FTP_PORT_DEFAULT			21

//#define AEW_TYPE_DEFAULT			3
#define SUPPORTMPEG4_DEFAULT		1		//???
#define IMAGEFORMAT_DEFAULT			1		//???
#define IMAGESOURCE_DEFAULT			0   //??? miachen@20140129, 60hz for ipcam
//#define MAX_FRAMERATE_DEFAULT		30000
#define DEFAULTSTORAGE_DEFAULT		1		//???
#define DEFAULTCARDGETHTM_DEFAULT	"sdget.htm"
#define BRANDURL_DEFAULT			"www.pacidal.com"
#define BRANDNAME_DEFAULT			"Pacidal"

#define SUPPORTTSTAMP_DEFAULT		1			//???
//#define	MPEG41XSIZE_DEFAULT			1280
//#define	MPEG41YSIZE_DEFAULT			720
//#define	MPEG42XSIZE_DEFAULT			352
//#define	MPEG42YSIZE_DEFAULT			192
//#define JPEGXSIZE_DEFAULT			640
//#define JPEGYSIZE_DEFAULT			352
#define SUPPORTMOTION_DEFAULT		2
#define SUPPORTWIRELESS_DEFAULT		0
#define SERVICEFTPCLIENT_DEFAULT	1
#define SERVICESMTPCLIENT_DEFAULT	1
#define SERVICEPPPOE_DEFAULT		0
#define SERVICESNTPCLIENT_DEFAULT	1
#define SERVICEDDNSCLIENT_DEFAULT	0
#define SUPPORTMASKAREA_DEFAULT		0
#define MAXCHANNEL_DEFAULT			1
#define SUPPORTRS485_DEFAULT		0
#define SUPPORTRS232_DEFAULT		1
#define LAYOUTNUM_DEFAULT			0
#define SUPPORTMUI_DEFAULT			0
#define MUI_DEFAULT					0
#define SUPPORTSEQUENCE_DEFAULT		0
#define QUADMODESELECT_DEFAULT		-1
#define SERVICEIPFILTER_DEFAULT		0
#define OEMFLAG0_DEFAULT			1
#define	SUPPORTDNCONTROL_DEFAULT	0
#define SUPPORTAVC_DEFAULT			0
#define SUPPORTAUDIO_DEFAULT		1
#define SUPPORTPTZPAGE_DEFAULT		1

//#define DAY_NIGHT_DEFAULT			2   // miachen@20131213 0	/*== miachen@20130508 Default: AUTO */
#define LOCALDISPLAY_DEFAULT		0
//#define BINNING_DEFAULT				0

#define CONTRAST_DEFAULT			119
#define CONTRAST_MAX					255
#define CONTRAST_MIN					0

#define SATURATION_DEFAULT		119
#define SATURATION_MAX				255
#define SATURATION_MIN				0

#define	HUE_DEFAULT						119
#define	HUE_MAX								255
#define	HUE_MIN								0

#define SHARPNESS_DEFAULT			119
#define SHARPNESS_MAX					255
#define SHARPNESS_MIN					0

#define _2DNRLEVEL_DEFAULT		0.27
#define _3DNRLEVEL_DEFAULT		0.80
#define NRLEVEL_MAX						1.0
#define NRLEVEL_MIN						0.0

//#define	ICRMODE_DEFAULT				0
#define	ICR_RESPONSETIME_DEFAULT	3
#define	ICR_RESPONSETIME_MAX			10
#define	ICR_RESPONSETIME_MIN			0

#define AUTOICR_THR_DEFAULT				50
#define AUTOICR_THR_MAX						255
#define AUTOICR_THR_MIN						0

#define BRIGHTNESS_DEFAULT				119
#define BRIGHTNESS_MAX						255
#define BRIGHTNESS_MIN						0

#define BACKLIGHT_VALUE_DEFAULT		50
#define BACKLIGHT_VALUE_MAX				255
#define BACKLIGHT_VALUE_MIN				0

#define MAXEXP_TIME_DEFAULT				33333
#define MAXEXP_TIME_MAX						500000
#define MAXEXP_TIME_MIN						30

#define MINEXP_TIME_DEFAULT				100
#define	MINEXP_TIME_MAX						500000
#define	MINEXP_TIME_MIN						30

#define EXP_TIME_DEFAULT					16667
#define EXP_TIME_MAX							500000
#define EXP_TIME_MIN							30

#define MAXEXP_GAIN_DEFAULT				30
#define MAXEXP_GAIN_MAX						80
#define MAXEXP_GAIN_MIN						0

#define MINEXP_GAIN_FEFAULT				0
#define MINEXP_GAIN_MAX						80
#define MINEXP_GAIN_MIN						0

#define EXP_GAIN_DEFAULT					0
#define	EXP_GAIN_MAX							80
#define	EXP_GAIN_MIN							0

#define WB_CRGAIN_DEFAULT					29
#define WB_CRGAIN_MAX							255
#define WB_CRGAIN_MIN							0

#define WB_CBGAIN_DEFAULT					32
#define WB_CBGAIN_MAX							255
#define WB_CBGAIN_MIN							0

#define	JPEG_QUALITY_DEFAULT			80
#define	JPEG_QUALITY_MAX					95
#define	JPEG_QUALITY_MIN					1

#define	UI_FRAME_RATE_DEFAULT				30
#define	UI_FRAME_RATE_MAX						30
#define	UI_FRAME_RATE_MIN						2

#define	GOVLENGTH_DEFAULT					30
#define	GOVLENGTH_MAX							300
#define	GOVLENGTH_MIN							2

#define DHCP_CONFIG_DEFAULT			1 //stfu@20131124 dhcpenable to be configurable
#define DISCOVERY_MODE              0
#define NO_SCHEDULE					{0,1,{0,0,0},{0,0,0}}
#define SCHEDULE_DEFAULT			{NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE}
#define LOSTALARM_DEFAULT			0
#define SDAENABLE_DEFAULT			0
#define ALARMDURATION_DEFAULT		0		//miachen@20140417, 1
#define AFTPENABLE_DEFAULT			0
#define ASMTPENABLE_DEFAULT			0
#define AVIDURATION_DEFAULT			0
#define AVIFORMAT_DEFAULT			0
#define FTPFILEFORMAT_DEFAULT		0
#define SDFILEFORMAT_DEFAULT		0
#define ATTFILEFORMAT_DEFAULT		0

#define ASMTPATTACH_DEFAULT			0
#define RFTPENABLE_DEFAULT			0
#define SD_INSERT_DEFAULT			0
#define SDRENABLE_DEFAULT			0
#define AUDIOENABLE_DEFAULT			0
#define MOTIONENABLE_DEFAULT		0
#define MOTIONCENABLE_DEFAULT		0
#define MOTIONLEVEL_DEFAULT			1
#define MOTIONCVALUE_DEFAULT		50
#define MOTIONBLOCK_DEFAULT			"000"

#define PTZZOOMIN_DEFAULT			"0"	//"zoomin"
#define PTZZOOMOUT_DEFAULT			"0"	//"zoomout"
#define PTZPANUP_DEFAULT			"0"	//"up"
#define PTZPANDOWN_DEFAULT			"0"	//"down"
#define PTZPANLEFT_DEFAULT			"0"	//"left"
#define PTZPANRIGHT_DEFAULT			"0"	//"right"

//GIO
#define	GIOINENABLE_DEFAULT		0
#define	GIOINTYPE_DEFAULT		1
#define	GIOINSTATE_DEFAULT	1
#define	GIOOUTENABLE_DEFAULT	0
#define	GIOOUTTYPE_DEFAULT		0
#define	GIOOUTSTATE_DEFAULT		0

#define DATEFORMAT_DEFAULT 			2
#define TSTAMPENABLE_DEFAULT 		0
#define TSTAMPFORMAT_DEFAULT 		1
#define AUDIOINVOLUME_DEFAULT 		2
#define ALARMSTATUS_DEFAULT			0
#define MULTICAST_ENABLE_DEFAULT	0

#define LANGUAGE_DEFAULT		"AUTO"
#define NONE_ACOUT				{"", "", 9}

#if defined(VCM210P)
#define ADMIN_ACOUT			{"admin", "9999", UserLevel__Administrator}
#else
#define ADMIN_ACOUT			{"admin", "camera", UserLevel__Administrator}
#endif

#define INTEGRATOR				{"integrator", "integrator", UserLevel__Administrator}
#define ROOT								{"root", "worldpeace", UserLevel__Administrator}
#define OPERATOR					{"operator", "operator", UserLevel__Operator}
#define USER								{"user", "user", UserLevel__User}

#define ACOUNT_DEFAULT			{ROOT, INTEGRATOR, ADMIN_ACOUT, OPERATOR, USER, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, \
							 	NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT}
#define ACOUNT_TIME_DEFAULT		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define CUR_LOG_DEFAULT			{"", {0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define CUR_DMVA_LOG_DEFAULT	{"", "", "",0,0,{0, 0, 0, 0, 0, 0, 0, 0, 0}}

/* stream_codec_config */
#define STREAM_CODEC_CFG1 {\
	1,			/*enable*/	\
	VIDEO_CODEC_H264,	/*codec*/\
	VIDEO_RC_MODE_CBR,	/*rate cotrol mode*/\
	2000,		/*bitrate*/ \
	30, 		/*framerate*/ \
	30,		/*gop*/\
	26,		/*minQp*/\
	40,		/*maxQp*/\
	26,		/*IQp*/\
	40,		/*PQp*/\
	31,		/*Qfactor*/\
	0,		/*profile*/\
	1920,	/*width*/\
	1080,	/*height*/\
	553,		/*portnum*/\
	"stream"	/*portname*/\
}
#define STREAM_CODEC_CFG2 {\
	1,			/*enable*/	\
	VIDEO_CODEC_H264,	/*codec*/\
	VIDEO_RC_MODE_CBR, /*rate cotrol mode*/\
	2000, 	/*bitrate*/ \
	30,		/*framerate*/ \
	30,		/*gop*/\
	26,		/*minQp*/\
	40,		/*maxQp*/\
	26,		/*IQp*/\
	40,		/*PQp*/\
	31,		/*Qfactor*/\
	0,		/*profile*/\
	960,	/*width*/\
	540,	/*height*/\
	554,		/*portnum*/\
	"stream"	/*portname*/\
}
#define STREAM_CODEC_CFG3 {\
	1,			/*enable*/	\
	VIDEO_CODEC_H264,	/*codec*/\
	VIDEO_RC_MODE_CBR, /*rate cotrol mode*/\
	2000, 	/*bitrate*/ \
	30,		/*framerate*/ \
	30,		/*gop*/\
	26,		/*minQp*/\
	40,		/*maxQp*/\
	26,		/*IQp*/\
	40,		/*PQp*/\
	31,		/*Qfactor*/\
	0,		/*profile*/\
	720,	/*width*/\
	480,	/*height*/\
	555,	/*portnum*/\
	"stream"	/*portname*/\
}
#define STREAM_CODEC_CFG4 {\
	1,			/*enable*/	\
	VIDEO_CODEC_MJPG,	/*codec*/\
	VIDEO_RC_MODE_CBR, /*rate cotrol mode*/\
	2000, 	/*bitrate*/ \
	30,		/*framerate*/ \
	30,		/*gop*/\
	26,		/*minQp*/\
	40,		/*maxQp*/\
	26,		/*IQp*/\
	40,		/*PQp*/\
	90,		/*Qfactor*/\
	0,		/*profile*/\
	960,	/*width*/\
	540,	/*height*/\
	551,	/*portnum*/\
	"mjpeg"	/*portname*/\
}
#define STREAM_CODEC_CFG5 {\
	0,			/*enable*/	\
	VIDEO_CODEC_MJPG,	/*codec*/\
	VIDEO_RC_MODE_CBR, /*rate cotrol mode*/\
	2000, 	/*bitrate*/ \
	30,		/*framerate*/ \
	30,		/*gop*/\
	26,		/*minQp*/\
	40,		/*maxQp*/\
	26,		/*IQp*/\
	40,		/*PQp*/\
	90,		/*Qfactor*/\
	0,		/*profile*/\
	960,	/*width*/\
	540,	/*height*/\
	552,	/*portnum*/\
	"mjpeg"	/*portname*/\
}

#define VIDEOOUTPUT_DEFAULT {\
	0, \
	0, \
	0 \
}

#define AUDIOSOURCE1 {\
	0, /*microphone*/\
	0 /*input gain*/\
}

#define AUDIO1 {\
	0,\
	2,\
	8000,\
	64000 \
}

#define AUDIOOUTPUT {\
	0,\
	"alarm_1_8K.wav" \
}

#define AUDIO_CONFIG {\
	1,\
	0, \
	0, \
	75, /*vol*/\
	2, /*AudioEncod_G711U*/\
	8000, \
	64000, \
	0, \
	50, \
	0, \
	0, \
	0, \
	"192.168.1.1" \
}
#define SD_DEFAULT {\
	0,\
	0\
}
#define VIDEO1RECORDING_DEFAULT { \
	0,\
	0,\
	0,\
	"\video1" \
}
#define VIDEO2RECORDING_DEFAULT { \
	0,\
	0,\
	0,\
	"\video2" \
}
#define VIDEO3RECORDING_DEFAULT { \
	0,\
	0,\
	0,\
	"\video3" \
}

#define MASK_SETTINGS_DEFAULT { \
	0, \
	0xffffff, \
	{{160, 6, 320, 240}, \
	{800, 112, 320, 240}, \
	{1140, 218, 320, 240}, \
	{160, 324, 320, 240}, \
	{800, 430, 320, 240}, \
	{1140, 536, 320, 240}, \
	{160, 642, 320, 240}, \
	{160, 642, 320, 240}} \
}

#define VIDEO_SOURCE_FRAMERATE_DEFAULT		30

/* Onvif */
#define COM_PROF					"", "", 0
#define VSC_PROF					"", 0, "", "", 0, 0, 0, 0
#define ASC_PROF					"", 0, "", ""
#define VEC_PROF					"", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 720000
#define MULC_PROF					0, "", "", 0, 0, 0
#define AEC_PROF					"", "", 0, 0, 0, 0
#define VAC_PROF					"", "", 0,
#define VAENGINE					0, 0, "", "", 0, "", "", ""
#define VRENGINE					0, 0, "", "", 0, "", "", ""
#define VOC_PROF					"",0,""

#define AOC_PROF					"",0,""
#define PTZ_PROF					"", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROF					"", 0, "", "", "", 0, "", 0, ""
#define MDC_PROF					"" , 0, "", 0, 720000

#define NON_PROF					{COM_PROF,{VSC_PROF},{ASC_PROF},{VEC_PROF,{MULC_PROF}},{AEC_PROF, {MULC_PROF}},{VAC_PROF{VAENGINE},{VRENGINE}},{VOC_PROF},{AOC_PROF},PTZ_PROF,EXT_PROF,{MDC_PROF,{MULC_PROF}}}
#define COM_PROFD					"Default_Profile", "Default_Token", 1
#define VSC_PROFD					"DVS_Name", 1, "DVS_Token", "DVS_SourceToken", 0, 0, 1920, 1080
#define ASC_PROFD					"G711", 1, "G711", "G711"
#define VEC_PROFD					"DE_Name", "DE_Token", 1, 100, 2, 1280, 720, 30, 1, 5000, 30, 0, 30, 3, 720000
#define MULC_PROFD				0, "239.0.0.0", "", 1234, 0, 0
#define AEC_PROFD					"DAE_Name", "G711", 0, 0, 0, 0
#define VAC_PROFD					"video_analytics", "video_analytics", 1
#define VAENGINED					0, 0, "", "", 0, "", "", ""
#define VRENGINED					0, 0, "", "", 0, "", "", ""
#define VOC_PROFD					"video_output", 1, "video_output"
#define AOC_PROFD					"audio_output", 1, "audio_output"
#define PTZ_PROFD					"", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROFD					"", 0, "", "", "", 0, "", 0, ""
#define MDC_PROFD					"MD_name" , 1, "MD_token", 0, 720000

#define DEFAULT_PROF			{COM_PROFD,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD,{MULC_PROFD}},{AEC_PROFD, {MULC_PROFD}},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},{AOC_PROFD},PTZ_PROFD,EXT_PROFD,{MDC_PROFD,{MULC_PROFD}}}
#define ONVIF_PROFILE			{DEFAULT_PROF, NON_PROF, NON_PROF, NON_PROF}

#define ONVIF_SCOPES_TYPE {NULL, NULL, NULL}
#define ONVIF_SCOPES_NAME	"onvif://www.onvif.org/name/camera"
#define ONVIF_SCOPES_LOCATION	{"onvif://www.onvif.org/camera_location", NULL,NULL}
#define ONVIF_SCOPES_IMPROPER_INPUT	{NULL, NULL, NULL}
#define ONVIF_SCOPES_PROFILE	"onvif://www.onvif.org/Profile/Streaming"
#define ONVIF_SCOPES_HW		"onvif://www.onvif.org/hardware/NMHS5178S"

#define ONVIF_SCOPES	{ONVIF_SCOPES_TYPE, ONVIF_SCOPES_NAME, ONVIF_SCOPES_LOCATION, ONVIF_SCOPES_HW, ONVIF_SCOPES_PROFILE, ONVIF_SCOPES_PROFILE}

#define RELAYS_PROFD			0 ,{"DR_Token", 0, 0, 0}
#define RELAY_NON_PROF			0 ,{"", 0, 0, 0}
#define RELAYS_CONFIG			{RELAYS_PROFD, RELAY_NON_PROF, RELAY_NON_PROF, RELAY_NON_PROF}
#define IMAGING_CONFD			{"DVS_SourceToken" ,0, 1, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 43, 33333, 1024, 269313, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 128, 128, 0, 1, 0, 128, 128, 1, 0.5, 2, 0.5, FlickerControl_Mode_60Hz, 16, 1, 1, 0, 0, 0.5, 0, 0, 0, 0}
#define IMAGING_CONFG			{"" ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0}
#define IMAGING_CONF			{IMAGING_CONFD,IMAGING_CONFG,IMAGING_CONFG,IMAGING_CONFG}
#define RTSPSERVERPORT 			{0,553,554,8553,8554,8551,5,1,0}
/* End-Onvif */

#define	TX_OUTPUT_GAIN	0	//miachen@20140812

#endif   /* __SYSTEM_DEFAULT_H__ */
