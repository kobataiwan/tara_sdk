/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file sys_env_type.h
* @brief System evironment structure and Global definition.
*/
#ifndef __SYS_ENV_TYPE_H__
#define __SYS_ENV_TYPE_H__

#include <asm/types.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>

#define MAGIC_NUM			0x9A131904

#define SUCCESS 0
#define FAIL	-1

#define GUI_RELOAD_TIME_IN_SECS 	(14)
#define BOOT_PROC_TIMEOUT_CNT		(2000)

#define SYS_ENV_SIZE		sizeof(SysInfo)
#define MAX_LOG_PAGE_NUM	20
#define NUM_LOG_PER_PAGE	20
#define LOG_ENTRY_SIZE		sizeof(LogEntry_t)

#define MAX_DOMAIN_NAME			40 		///< Maximum length of domain name. Ex: www.xxx.com
#define MAX_LANCAM_TITLE_LEN	11 		///< Maximum length of LANCAM title.
#define OSDTEXTLEN				24
#define MAX_FQDN_LENGTH			256 	///< Maximum length of FQDN.
#define MAX_NAME_LENGTH			256 	///< Maximum length of normal string.
#define MAX_OSD_STREAMS			3 		///< Maximum length of normal string.
#define MAX_CODEC_STREAMS		3 		///< Maximum length of normal string.
#define MAX_STREAM_NAME			75  //25///< Maximum length of normal string.
#define MAC_LENGTH				6 		///< Length of MAC address.
#define SCHDULE_NUM				8 		///< How many schedules will be stored in system.
#define MAX_FILE_NAME			128		///< Maximum length of file name.

#define ACOUNT_NUM				16 		///< How many acounts which are stored in system.
#define USER_LEN				32 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN			28//16 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define PASSWORD_ZERO           0               ///< minimum length of password>
#define IP_STR_LEN				20		///< IP string length
#define MAX_LENGTH              20
#define MOTION_BLK_LEN			(4) 	///< Motion block size
#define LANGUAGE_LEN				(8)

#define OSD_MSG_FALSE			(0)
#define OSD_MSG_TRUE			(1)
#define OSD_PARAMS_UPDATED		(4)

/*
 The following flags is defined for alarmstatus.
 Modify these sould also check where the alarmstatus is used.
 Be sure that you also check your web UI coding.
*/
#define FLG_UI_EXT 				(1 << 0)
#define FLG_UI_MOTION 			(1 << 1)
#define FLG_UI_ETH 				(1 << 2)
#define FLG_UI_AUDIO			(1 << 3)
#define FLG_UI_RECORD 			(1 << 4)
#define FLG_UI_AVI 				(1 << 5)
#define FLG_UI_JPG 				(1 << 6)

#define LOGIN_TIMEOUT		1800 	/* Set for 30 min */

#define INVALID_IP			0
#define ETH_NAME			"eth0"
#define USB_PATH			"/proc/usb_appro"
#define USB_CONNECT_PATH	"/proc/usb_connect"
#define USB_CFG_FILE		"/mnt/ramdisk/NET.TXT"

#define MAX_PROF        5
#define MAX_PROF_TOKEN  20
#define MAX_CONF_TOKEN  20
#define MAX_IP_ADDR     30
#define MAX_RELAYS      5
#define MAX_IMAGING     5

#define MAX_VS_NUM		8

// ??? ONVIF: JPEG = 0, MPEG4 = 1, H264 = 2 by Justin
// ONVIF: JPEG = 0, MJPEG = 1, H264 = 2
typedef enum{
	OPROFILE_VENCODER_JPEG = 0,
	OPROFILE_VENCODER_MPEG4,
	OPROFILE_VENCODER_H264,
	OPROFILE_VENCODER_NOTSUPPORT
}OPROFILE_AESC_VENCODER;

typedef enum{
	AUTO = 0,
	CUSTOM,
	SVC
}ENCPRESET_MODE;

typedef enum{
	IrCutFilter_Mode_Auto = 0,
	IrCutFilter_Mode_On,
	IrCutFilter_Mode_Off
}IRCUTEFILTER_MODE;

typedef enum{
	WhiteBalance_Mode_Auto = 0,
	WhiteBalance_Mode_Indoor,
	WhiteBalance_Mode_Outdoor,
	WhiteBalance_Mode_OnePushWB,
	WhiteBalance_Mode_ATW,
	WhiteBalance_Mode_Manual,
	WhiteBalance_Mode_WideAuto
}WHITE_BALANCE_MODE;

typedef enum{
	WideDynamicRange_Mode_Off = 0,
	WideDynamicRange_Mode_Auto,
	WideDynamicRange_Mode_On,
	WideDynamicRange_Mode_End
}WIDEDYNARANGE_MODE;

typedef enum{
	BacklightCompensation_Mode_Off = 0,
	BacklightCompensation_Mode_On
}BLC_MODE;

typedef enum{
	FlickerControl_Mode_Off = 0,
	FlickerControl_Mode_50Hz,
	FlickerControl_Mode_60Hz
}FLICKERCTRL_MODE;

typedef enum{
	NoiseReduction_Mode_Off = 0,
	NoiseReduction_Mode_Auto,
	NoiseReduction_Mode_On
}NR_MODE;

typedef enum{
	Exposure_Priority_LowNoise = 0,
	Exposure_Priority_FreamRate,
	Exposure_Priority_Traffic
}EXP_PRIORITY;

typedef enum{
	Exposure_Mode_Auto = 0,
	Exposure_Mode_ShutterPriority,
	Exposure_Mode_IrisPriority,
	Exposure_Mode_Manual
}EXP_MODE;

typedef enum{
	FlipMirror_Mode_Off = 0,
	FlipMirror_Mode_Flip,
	FlipMirror_Mode_Mirror,
	FlipMirror_Mode_Both
}FLIPMIRROR_MODE;

typedef enum{
	FactoryDefault_Soft = 0,
	FactoryDefault_Hard,
	FactoryDefault_Accounts,
	FactoryDefault_NotSupport
}FACTORY_DEFAULT;

typedef enum {
	UserLevel__Administrator = 0, 
	UserLevel__Operator = 1, 
	UserLevel__User = 2, 
	UserLevel__Anonymous = 3, 
	UserLevel__Extended = 4
}USER_LEVEL;

typedef enum {
	DateTimeType_Now = 0,
	DateTimeType_NTP,
	DateTimeType_Manual,
	DateTimeType_Compare
}DATETIME_TYPE;

typedef enum{
	SD_LoopRecording_Off = 0,
	SD_LoopRecording_On
}SD_LOOPRECORDING;

typedef enum{
	Recording_Off = 0,
	Recording_On
}RECORDING;

typedef enum{
	Recording_Mode_Continuous = 0,
	Recording_Mode_Event,
	Recording_Mode_Scheduled
}RECORDING_MODE;

typedef enum{
	UnencryptedUpdate_Off = 0,
	UnencryptedUpdate_On
}UnencryptedUpdate_MODE;

typedef enum{
	Ptz_DigitalZoom_Off = 0,
	Ptz_DigitalZoom_Combined,
	Ptz_DigitalZoom_Separate
}PtzDigitalZoom_MODE;

typedef enum{
	OSD_Type_Date = 0,
	OSD_Type_Time,
	OSD_Type_DateTime,
	OSD_Type_Text,
	OSD_Type_Picture
}OSDType_MODE;

typedef enum{
	PictureEffect_Off = 0,
	PictureEffect_ColorNegative,
	PictureEffect_BlackNWhite
}PictureEffect_MODE;

typedef enum{
	Defogging_Mode_Off = 0,
	Defogging_Mode_On
}Defogging_MODE;

typedef enum{
	Gamma_Mode_037 = 0,
	Gamma_Mode_Normal,
	Gamma_Mode_055,
	Gamma_Mode_Straight
}Gamma_MODE;

typedef enum{
	VideoOutput_Mode_BT1120 = 0,
	VideoOutput_Mode_BT656,
	VideoOutput_Mode_CVBS
}VideoOutput_MODE;

typedef enum{
	VideoOutput_Timing_1080P60 = 0,
	VideoOutput_Timing_1080P50,
	VideoOutput_Timing_1080P30,
	VideoOutput_Timing_1080P25,
	VideoOutput_Timing_1080I60,
	VideoOutput_Timing_1080I50,
	VideoOutput_Timing_720P60,
	VideoOutput_Timing_720P50,
	VideoOutput_Timing_NTSC,
	VideoOutput_Timing_PAL
}VideoOutput_Timing;

typedef enum {
	Focus_Mode_Auto = 0,
	Focus_Mode_Manual
}Focus_Mode;

typedef enum {
	AF_Mode_NormalAF = 0,
	AF_Mode_IntervalAF,
	AF_Mode_ZoomTrigger
}Auto_Focus_Mode;

typedef enum {
	AF_Sensitivity_Normal = 0,
	AF_Sensitivity_Low
} AF_Sensitivity;

typedef enum {
	ExposureCompensation_Mode_Off = 0,
	ExposureCompensation_Mode_On
} ExposureCompensation_Mode;

/* Onvif Structures */
/* Onvif - Video source configuration structure */

typedef struct Add_VideoSource_configuration{
	char                                   Vname[MAX_PROF_TOKEN];               //video source configuration name
	int                                    Vcount;         // count for number of users
	char                                   Vtoken[MAX_CONF_TOKEN];             //video source configuration token
	char                                   Vsourcetoken[MAX_PROF_TOKEN];
	int                                    windowx;             //x cordinate of window
	int                                    windowy;             //y cordinate of window
	int                                    windowwidth;          //width
	int                                    windowheight;        //height
}Add_VideoSource_configuration;

/* Onvif - Video multicast configuration structure */
typedef struct _Video_Multicast_configuration{
	int                                    nIPType;               //ip address IPv4/IPv6
	char                                   IPv4Addr[MAX_IP_ADDR];               //ipv4 address
	char                                   IPv6Addr[MAX_IP_ADDR];               //ipv6 address
	int                                    port;                   //port number
	int                                    ttl;                     //TTL
	int                                    autostart;                //autostart

}Video_Multicast_configuration;

/* Onvif - Video encoder configuration structure */
typedef struct _Add_VideoEncoder_configuration{
	char                                   VEname[MAX_PROF_TOKEN] ;                 //video encoder name
	char                                   VEtoken[MAX_CONF_TOKEN];                 //video encoder token
	int                                    VEusercount;            //video encoder user count
	float                                  VEquality;              //video encoder quality
	int                                    Vencoder;             //video encoder for jpeg, mpeg, h.264
	int                                    Rwidth;               //resolution width
	int                                    Rheight;                //resolution height
	int                                    frameratelimit;        //frameratelimit
	int                                    encodinginterval;       //encodinginterval
	int                                    bitratelimit;            //bitratelimit
	int                                    Mpeggovlength;              //mpeg GoVLength
	int                                    Mpegprofile;                 //mpegprofile SP/ASP
	int                                    H264govlength;               //H264 govlength
	int                                    H264profile;                 //H264 profile  baseline/main/extended/high
	long                                   sessiontimeout;
	Video_Multicast_configuration          Multicast_Conf;

}Add_VideoEncoder_configuration;

/* Onvif - Video analyticengine configuration structure */
typedef struct _Video_AnalyticEngine{
	int                                   VAMsize;                  //video analytic module size
	int                                   VAsizeitem;               //size of simple items
	char                                  VASname[MAX_LENGTH];                   //simple item name
	char                                  VASvalue[MAX_LENGTH];                 //simple item value
	int                                   VAEsize;                 //Element size
	char                                  VAEname[MAX_LENGTH];                   //element name
	char                                  Aconfigname[MAX_LENGTH];
	char                                  Aconfigtype[MAX_LENGTH];

}Video_AnalyticEngine;

/* Onvif - Video ruleengine configuration structure */
typedef struct _Video_RuleEngine{
	int                                   VARsize;                 //video analytic rule size
	int                                   VARSsizeitem;               //size of simple items for ruleengine
	char                                  VARSname[MAX_LENGTH];                   //simple item name for rule engine
	char                                  VARSvalue[MAX_LENGTH];                 //simple item value for rule engine
	int                                   VAREsize;                 //Element size for rule engine
	char                                  VAREname[MAX_LENGTH];                   //element name for rule engine
	char                                  Rconfigname[MAX_LENGTH];
	char                                  Rconfigtype[MAX_LENGTH];

}Video_RuleEngine;

/* Onvif - VideoAnalytic configuration structure */
typedef struct _Add_VideoAnalytics_Configuration{
	char                                  VAname[MAX_PROF_TOKEN];                    //video analytic name
	char                                  VAtoken[MAX_CONF_TOKEN];                  //video analytic token
	int                                   VAusercount;              //video analytic user count
	Video_AnalyticEngine VAE;
	Video_RuleEngine VRE;
} Add_VideoAnalytics_Configuration;

typedef struct _Add_VideoOutput_Configuration{
	char name[MAX_CONF_TOKEN];
	int usecount;
	char token[MAX_CONF_TOKEN];
} Add_VideoOutput_Configuration;

/* Onvif - Audio configuration structure */
typedef struct _Add_AudioSource_Configuration{
	char                                   Aname[MAX_LENGTH];                 //audio name
	int                                    Ausecount;                //audio user count
	char                                   Atoken[MAX_LENGTH];                //audio token
	char                                   Asourcetoken[MAX_LENGTH];          //audio source token
}Add_AudioSource_Configuration;

typedef struct _Add_AudioEncoder_Configuration{
	char                                   AEname[MAX_LENGTH];                     //audio encoder name
	char                                   AEtoken[MAX_LENGTH];                   //audio encoder token
	int                                    AEusercount;               //audio encoder user count
	int                                    bitrate;                    //Bitrate
	int                                    samplerate;                //Samplerate
	int 				       AEencoding;

#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
	Video_Multicast_configuration          Multicast_Conf;
#endif
}Add_AudioEncoder_Configuration;

#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
typedef struct _Add_AudioOutput_Configuration{
    char                                   AOname[MAX_LENGTH];                 //audio name
    int                                    AOusecount;                //audio user count
    char                                   AOtoken[MAX_LENGTH];                //audio token
    int                                   AOvolume[MAX_LENGTH];          //audio source token
}Add_AudioOutput_Configuration;
#endif


typedef struct _Metadata_configuration{
	char 	                                MDname[MAX_LENGTH];
	int					MDusecount;
	char					MDtoken[MAX_LENGTH];
	int 					MDanalytics;
	long 					sessiontimeout;
	Video_Multicast_configuration           VMC;
}Metadata_configuration;
/* Onvif - Profile Structure*/
typedef struct
{
	char                                  profilename[MAX_PROF_TOKEN];       //name of profile
	char                                  profiletoken[MAX_CONF_TOKEN];      //token of profile
	int                                    fixed;             //profile fixed or not
	Add_VideoSource_configuration AVSC;

	Add_AudioSource_Configuration AASC;

	Add_VideoEncoder_configuration AESC;

	Add_AudioEncoder_Configuration AAEC;

	Add_VideoAnalytics_Configuration  AVAC;
	Add_VideoOutput_Configuration AVOC;

#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
	Add_AudioOutput_Configuration AAOC;
#endif

	/* PTZ configuration */
	char                                   PTZname[MAX_PROF_TOKEN];                  //ptz name
	char                                   PTZtoken[MAX_CONF_TOKEN];                 //ptz token
	int                                    PTZusercount;             //ptz user count
	char                                   PTZnodetoken[MAX_CONF_TOKEN];             //ptz nade token
	char                                   DefaultAbsolutePantTiltPositionSpace[MAX_LENGTH];         //default absolute pant tilt position space
	char                                   DefaultAbsoluteZoomPositionSpace[MAX_LENGTH];             //default absolute zoom position space
	char                                   DefaultRelativePanTiltTranslationSpace[MAX_LENGTH];       //default relative pan tilt translation space
	char                                   DefaultRelativeZoomTranslationSpace[MAX_LENGTH];          //default relative zoom translation space
	char                                   DefaultContinuousPanTiltVelocitySpace[MAX_LENGTH];         //Default Continuous PanTilt Velocity Space
	char                                   DefaultContinuousZoomVelocitySpace[MAX_LENGTH];            //Default Continuous Zoom Velocity Space
	float                                  PTZspeedx;                                    //pan tilt speed
	float                                  PTZspeedy;                                    // pan tilt speed
	char                                   PTZspeedspace[MAX_LENGTH];                                // pan tilt speed
	float                                  PTZzoom;                                      //ptz zoom
	char                                   PTZzoomspce[MAX_LENGTH];                                  //ptz zoom space
	long*                                  DefaultPTZTimeout;                            //default time out for ptz
	char                                   PANURI[MAX_LENGTH];                                        //pan tilt limit uri
	float                                  PTZrangeminx;                                     //ptz min x  range
	float                                  PTZrangemaxx;                                     //ptz max x range
	float                                  PTZrangeminy;                                     //ptz min y range
	float                                  PTZrangemaxy;                                      //ptz max y range
	char                                   PTZzoomURI[MAX_IP_ADDR];                                        //zoom uri
	char                                   PTZMname[MAX_LENGTH];                                           //meta data name
	char                                   PTZMtoken[MAX_CONF_TOKEN];                                         //meta data token
	int                                    PTZMusercount;                                      //meta data user count
	int                                    PTZfilterstatus;                                    //ptz filter status
	int                                    PTZfilterposition;                                  //ptz filter position
	int                                    PTZManalytics;                                      //analytics
	/* Extension */
	char                                   AExname[MAX_PROF_TOKEN];                           //extension audio output configuration name
	int                                    AExusecount;                       //extension audio output configuration user count
	char                                   AExtoken[MAX_CONF_TOKEN];                          //extension audio output configuration token
	char                                   AExoutputtoken[MAX_CONF_TOKEN];                    //extension audio output configuration otput token
	char                                   AExsendprimacy[MAX_LENGTH];                    //extension audio output configuration primacy
	int                                    AExoutputlevel;                      //extension audio output configuration level
	char                                   AExDname[MAX_LENGTH];                            //audio decoder name
	int                                    AExDusecount;                       //audio decoder user count
	char                                   AExDtoken[MAX_LENGTH];                         //audio decoder token
	/* metadata configuration */
	Metadata_configuration                 MDC;

}Onvif_profile;

/* Onvif Scopes structures*/
typedef struct _Onvif_Scopes{
	char type[3][100];
	char name[50];
	char location[3][100];
	char hardware[50];
#if 1 //stfu@20140616 Fix for ONVIF Profile S
	char profile[50];
	char improper_input[3][100];
#endif
} Onvif_Scopes;


typedef struct _Onvif_Relay_in{
	char token[MAX_CONF_TOKEN];
	int relaymode;
	long delaytime;
	int idlestate;
	//Relay_conf set_relay_conf;
} Onvif_Relay_in;

typedef struct _Onvif_Relay{
	int	relaylogicalstate;
	Onvif_Relay_in onvif_relay_in;
}Onvif_Relay;

/* End-Onvif Structures */

/**

* @brief Infomation of network status and user settings.
*/
typedef struct _Network_Config_Data{
	struct in_addr	ip; ///< IP address in static IP mode
	struct in_addr	netmask; ///< netmask in static IP mode
	struct in_addr	gateway; ///< gateway in static IP mode
	struct in_addr	dns; ///< DNS IP in static IP mode
	__u16		http_port; ///< HTTP port in web site.
	__u16		https_port; ///< HTTP port in web site.
	int			dhcp_enable; ///< current DHCP status.
	int			dhcp_invokeTimes;
	int			dhcp_retries;
	int			dhcp_timeout;
	__u8        discovery_mode;         ///<set 0 for discoverable and 1 for non-discoverable>
	char		ntp_server[MAX_DOMAIN_NAME+1]; ///< NTP server FQDN
	__u8		ntp_timezone; ///< current time zone
	__u8		ntp_frequency; ///< NTP server query frequence
	__u8		time_format; ///< time display format
	__u8		daylight_time; ///< daylight saving time mode
	__u8		MAC[MAC_LENGTH]; ///< hardware MAC address
	__u8		mpeg4resolution; ///< MPEG4_1 resolution
	__u8		mpeg42resolution; ///< MPEG4_2 resolution
	__u8		liveresolution; ///< live resolution
	__u8		mpeg4quality; ///< MPEG4 quality
	__u8		supportmpeg4; ///< support MPEG4
	__u8		imageformat; ///< image format
	__u8		imagesource; ///< image source (NTSC/PAL)
	__u8		defaultstorage; ///< default storage
	char		defaultcardgethtm[MAX_NAME_LENGTH]; ///< default card get page
	char		brandurl[MAX_NAME_LENGTH]; ///< brand URL
	__u8		supporttstamp; ///< support time stamp
	__u8		supportmotion; ///< support motion detection
	__u8		supportwireless; ///< support wireless
	__u8		serviceftpclient; ///< ftp client support
	__u8		servicesmtpclient; ///< smtp client support
	__u8		servicepppoe; ///< PPPoE support
	__u8		servicesntpclient; ///< sntp client support
	__u8		serviceddnsclient; ///< DNS client support
	__u8		supportmaskarea; ///< mask area support
	__u8		maxchannel; ///< max channel
	__u8		supportrs485; ///< RS485 support
	__u8		supportrs232; ///< RS232 support
	__u8		layoutnum; ///< layout No.
	__u8		supportmui; ///< support MUI
	__u8		mui; ///< MUI
	__u8		supportsequence; ///< support sequence
	__u8		quadmodeselect; ///< quadmode select
	__u8		serviceipfilter; ///< service IP filter
	__u8		oemflag0; ///< OEM flag
	__u8		supportdncontrol; ///< Daynight control support
	__u8		supportavc; ///< avc support
	__u8		supportaudio; ///< support audio
	__u8		supportptzpage; ///< PTZ support
	__u8		multicast_enable; ///< multicast enable
} Network_Config_Data;

/**
* @brief Infomation about ftp configuration.
*/
typedef struct _Ftp_Config_Data{
	char		servier_ip[MAX_DOMAIN_NAME+1]; ///< FTP server address
	char		username[USER_LEN+1]; ///< FTP login username
	char		password[PASSWORD_LEN+1]; ///< FTP login password
	char		foldername[MAX_FILE_NAME]; ///< FTP upload folder
	int			image_acount; ///< Image count
	int			pid; ///< PID
	__u16		port; ///< FTP port
 	__u8		rftpenable; ///< RFTP enable
	__u8        ftpfileformat; ///< file format
} Ftp_Config_Data;

/**
 * Imaging structure
 */
typedef struct _Imaging_Conf{
	char                    Isourcetoken[MAX_LENGTH];
	int			BacklightCompensationMode;
	float			BackLightLevel;
	float			Brightness;
	float			Saturation;
	float			Hue;
	float			Contrast;
	float			ExposureMode;
	float			ExposurePriority;
	float			bottom;
	float			top;
	float			right;
	float			left;
	float			MinExposureTime;
	float			MaxExposureTime;
	float			MinGain;
	float			MaxGain;
	float			MinIris;
	float			MaxIris;
	float			ExposureTime;
	float			Gain;
	float			Iris;
	int			AutoFocusMode ;
	float			DefaultSpeed;
	float 			NearLimit;
	float 			FarLimit;
	int 			IrCutFilterMode;
	float			AutoICRResponseTime;
	float 		AutoICRThreshold;
	float			Sharpness;
	int			WideDynamicMode;
	float			WideDMLevel;
	int			WhiteBalanceMode;
	float 			CrGain;
	float 			CbGain;
	int			_2DNoiseReductionMode;
	float			_2DNoiseReductionLevel;
	int			_3DNoiseReductionMode;
	float			_3DNoiseReductionLevel;
//	float			NR;
	int			Flicker;
	float			AGain;
	float 		DGain;
	float			ISPGain;
	int				PictureEffect;
	int				DefoggingMode;
	float			DefoggingLevel;
	int				GammaMode;
	int				GammaOffset;
	int				ExposureCompensationMode;
	float			ExposureCompensationLevel;
} Imaging_Conf;

typedef struct _Imaging_Conf_in{
	char			Isourcetoken_t[MAX_LENGTH];
	int				BacklightCompensationMode_t;
	float			BackLightLevel_t;
	float			Brightness_t;
	float			Saturation_t;
	float			Contrast_t;
	float			ExposureMode_t;
	float			ExposurePriority_t;
	float			bottom_t;
	float			top_t;
	float			right_t;
	float			left_t;
	float			MinExposureTime_t;
	float			MaxExposureTime_t;
	float			MinGain_t;
	float			MaxGain_t;
	float			MinIris_t;
	float			MaxIris_t;
	float			ExposureTime_t;
	float			Gain_t;
	float			Iris_t;
	int				AutoFocusMode_t ;
	float			DefaultSpeed_t;
	float 			NearLimit_t;
	float 			FarLimit_t;
	int 			IrCutFilterMode_t;
	float			Sharpness_t;
	int				WideDynamicMode_t;
	float			WideDMLevel_t;
	int				WhiteBalanceMode_t;
	float 			CrGain_t;
	float 			CbGain_t;

} Imaging_Conf_in;

typedef struct _Imaging_Conf_tmp{
	int      position;
	Imaging_Conf_in imaging_conf_in;
}Imaging_Conf_tmp;
/**
* @brief Infomation about SMTP configuration.
*/
typedef struct _Smtp_Config_Data{
	char		servier_ip[MAX_DOMAIN_NAME+1]; ///< SMTP server address
	__u16		server_port; ///< SMTP server port
	char		username[USER_LEN+1]; ///< SMTP username
	char		password[PASSWORD_LEN+1]; ///< SMTP password
	__u8		authentication; ///< SMTP authentication
	char		sender_email[MAX_NAME_LENGTH]; ///< sender E-mail address
	char		receiver_email[MAX_NAME_LENGTH]; ///< receiver E-mail address
	char		CC[MAX_NAME_LENGTH]; ///< CC E-mail address
	char		subject[MAX_NAME_LENGTH]; ///< mail subject
	char		text[MAX_NAME_LENGTH]; ///< mail text
	__u8		attachments; ///< mail attachment
	__u8		view; ///< smtp view
	__u8		asmtpattach; ///< attatched file numbers
	__u8        attfileformat; ///< attachment file format
} Smtp_Config_Data;

/**
* @brief custom data structure for time.
*/
typedef struct _Time_t{
	__u8	nHour;	///< Hour from 0 to 23.
	__u8	nMin;	///< Minute from 0 to 59.
	__u8	nSec;	///< Second from 0 to 59.
} Time_t;

/**
* @brief custom data structure for schedule entry.
*/
typedef struct _Schedule_t{
	__u8	bStatus;	///< schedule status ( 0:disable 1:enable }
	__u8	nDay;		///< schedule day of week (1:Mon 2:Tue 3:Wed 4:Thr 5:Fri 6:Sat 7:Sun 8:Everyday)
	Time_t	tStart;		///< schedule start time
	Time_t	tDuration;	///< schedule duration
} Schedule_t;

/**
* @brief IPCAM configuration data.
*/
typedef struct _Lancam_Config_Data{
//	char			title[MAX_LANCAM_TITLE_LEN+1];	///< camera title
	char		location[MAX_NAME_LENGTH];
	char			hostname[MAX_FILE_NAME];            ///<host name
	__u8		dateTimeType;
	char 		timezone[MAX_LANCAM_TITLE_LEN];
	char 		ntpServer[MAX_DOMAIN_NAME+1];
	__u8			gioinenable;					///< GIO input enable
	__u8			giointype;						///< GIO input type
	__u8			gioinstate;
	__u8			giooutenable;					///< GIO output enable
	__u8			gioouttype;						///< GIO output type
	__u8			giooutstate;
	__u8			recovery;					///< one key recovery
	__u8			enRecovery;
	__u8			FlipMirror; 						///< mirroring
	__u8            encryptEnable;					///< AES Encryption
//	__u8			democfg;						///< demo config
	__u8			regusr[MAX_LANCAM_TITLE_LEN+1];	///< register user name
	__u8			osdtext[OSDTEXTLEN];			///< register user name
	__u8            dateformat;
	__u8            tstampformat;
	__u8            dateposition;
	__u8            timeposition;
	__u8			rs485config;
	__u8			nClickSnapFilename[OSDTEXTLEN];	///< click snap filename
	__u8			nClickSnapStorage;				///< click snap storage setting

	__u8		    nLocalDisplay; 			        ///< local display video enable/disable
	__u8			expPriority;
	__u8			modelname[OSDTEXTLEN];
	__u8			manufacturer[OSDTEXTLEN];
	__u8			brandname[OSDTEXTLEN];
	__u8			productname[OSDTEXTLEN];
	__u8			sensorname[OSDTEXTLEN];
	__u8			chipname[OSDTEXTLEN];
	Network_Config_Data	net;						///< network status and user settings
} Lancam_Config_Data;


typedef struct _Advanced_Mode{
	__u8			vsEnable;
	__u8			ldcEnable;
	__u8			vnfEnable;
	__u8			vnfMode;
	__u8			vnfStrength;
	__u8			dynRange;						///< Dynamic Range switch ON/OFF
	__u8			dynRangeStrength;				///< Dynamic Range Strength
} Advanced_Mode;


typedef struct _hostname_tmp{
  char       hostname_t[MAX_FILE_NAME];
}hostname_tmp;

/**
* @brief IPCAM user account data.
*/
typedef struct _Acount_t{
	char	user[USER_LEN+1];			///< username
	char	password[PASSWORD_LEN+1];	///< password
	__u8	authority;				///< user authority
//	char language[LANGUAGE_LEN];
}Acount_t;

/**
* @brief motion detection configuration data.
*/
typedef struct _Motion_Config_Data{
  __u8      motionenable;					///< motion detection enable
  __u8      motioncenable;					///< customized sensitivity enable
  __u8      motionlevel;					///< predefined sensitivity level
  __u8      motioncvalue;					///< customized sensitivity value
  __u8	 	motionblock[MOTION_BLK_LEN];	///< motion detection block data
}Motion_Config_Data;

typedef struct _Audio_Config_Data{
	__u8			enabled;
  __u8      audioenable;
  __u8      audiomode;				///< motion detection enable
  __u8      audioinvolume;				///< customized sensitivity enable
  __u8		codectype;
  __u8      samplerate;				///< predefined sensitivity level
  __u8      bitrate;				///< customized sensitivity value
  __u32     bitrateValue;			///< customized sensitivity value
  __u8 		alarmlevel;
  __u8 		alarmON;
  __u8	 	audiooutvolume;	        ///< motion detection block data
  __u8      audiorecvenable;    	///< audio receiver enable
  __u8      audioServerIp[MAX_NAME_LENGTH];        ///< audio server ip address
}Audio_Config_Data;

typedef struct _OSD_config{
	__u8			dstampenable;					///< stream1 date stamp enable (ON/OFF)
	__u8			tstampenable;   				///< stream1 time stamp enable (ON/OFF)
	__u8			nLogoEnable;					///< stream1 logo enable (ON/OFF)
	__u8			nLogoPosition;   				///< stream1 logo position
	__u8			nTextEnable;					///< stream1 text enable (ON/OFF)
	__u8			nTextPosition;   				///< stream1 text position
	__u8			overlaytext[OSDTEXTLEN];		///< stream1 text
    __u8            nDetailInfo;
}OSD_config;

typedef struct _CodecAdvParam{
	__u32 ipRatio;
	__u8 fIframe;
	__u8 qpInit;
	__u8 qpMin;
	__u8 qpMax;
	__u8 meConfig;
	__u8 packetSize;
	__u8 codec;
//	__u8 enSize;
	__u8 rcType;
	__u32 bitrate;
	__u32 framerate;
}CodecAdvParam;

typedef struct _ROI_Param{
	__u32 startx;
	__u32 starty;
	__u32 width;
	__u32 height;
}ROI_Param;

typedef struct _CodecROIParam{
	__u32 		numROI;
  	__u32     	prevwidth;
  	__u32     	prevheight;
	ROI_Param	roi[3];
}CodecROIParam;

/**
* @brief motion detection configuration data.
*/
typedef struct _Face_Config_Data{
  __u8	    fdetect; 					    ///< facedetect (ON/OFF)
  __u32     startX;
  __u32     startY;
  __u32     width;
  __u32     height;
  __u32     prevwidth;
  __u32     prevheight;
  __u8      fdconflevel;
  __u8      fddirection;
  __u8      frecog;
  __u8	    frconflevel;
  __u8	    frdatabase;
  __u8      pmask;
  __u8      maskoption;
}Face_Config_Data;

typedef struct _CodecParam{
	__u8 pos; 
	__u8 enable;
	__u16 codec;
//	__u8 enSize;
	__u8 rcType;
	__u32 bitrate;
	__u32 framerate;
	__u32 gop;
	__u32 minQp;
	__u32 maxQp;
	__u32 IQp;
	__u32 PQp;
	__u32 Qfactor;
	__u32 profile;	
	 __u16     width;
	 __u16     height;
}CodecParam;
#if 0
typedef struct _StreamParam{
	__u8 enable;
	__u16 codec;
//	__u8 enSize;
	__u8 rcType;
	__u32 bitrate;
	__u32 framerate;
	__u32 gop;
	__u32 minQp;
	__u32 maxQp;
	__u32 IQp;
	__u32 PQp;
	__u32 Qfactor;
	__u32 profile;
	__u16 width;
	__u16 height;
	__u16 portnum;
	__u8 	portname[MAX_STREAM_NAME];
}StreamParam;

typedef struct _VencPrm{
	int num;
	StreamParam VencParam;
}VencPrm;
#endif
typedef struct _VideoOutputSettings{
	__u8 VideoOutput;
	__s32 VideoOutputMode;
	__s32 VideoOutputTiming;
}VideoOutputSettings;

typedef struct _VideoSourceSettings{
	__s32 framerate;
	__s32 freezeframe;
}VideoSourceSettings;

typedef struct _AudioSourceParm{
	__u8 Type;
	__u8 InputGain;
}AudioSourceParam;

typedef struct _AudioEncParam{
	__u8 Audio;
	__u8 Encoding;
	__u32 Samplerate;
	__u32 Bitrate;
}AudioEncParam;

typedef struct _AudioOutputParam{
	int AudioOutputGain;
	char FileName[32];
}AudioOutputParam;

typedef struct _OSD_PARAMS
{
	__u8 bOSD;
//	OSDType_MODE OSDType;
	__u8 OSDType;
	__u32 OSDX;
	__u32 OSDY;
	__u32 OSDFontSize;
	__u32 OSDFontColor;
	char OSDText[64];
} OSD_PARAMS;

typedef struct _OSD_SETTINGS_S
{
	__u8 bOSD;
	__u8 bDbg;
	char Font[64];
//	char CameraName[64];
	OSD_PARAMS Name;
	OSD_PARAMS Time;
	OSD_PARAMS Text[11];
	OSD_PARAMS Logo;
} OSD_SETTINGS_S;

/**
* @brief SD card configuration data.
*/
typedef struct _SdcardSettings{
	__u8 CardStatus;
	__u8 LoopRecording;
}SdcardSettings;

typedef struct _RecordingSettings{
	__u8 Recording;
	__u8 RecordingMode;
	__u8 StorgeLocation;
	char StorgeRelativePath[16];
}RecordingSettings;

typedef struct _VideoRecordingSettings{
	__u8 num;
	RecordingSettings video;
}VideoRecordingSettings;
/**
* @brief structure for PTZ control data.
*/
typedef struct _Ptz_Data{
  char      ptzdata[300];
}Ptz_Data;

/**
* @brief PTZ configuration data.
*/
typedef struct _Ptz_Config_Data{
  char      ptzzoomin[6];	///< zoom-in
  char      ptzzoomout[7];	///< zoom-out
  char      ptzpanup[2];	///< pan-up
  char      ptzpandown[4];	///< pan-down
  char	ptzpanleft[4];		///< pan-left
  char	ptzpanright[5];		///< pan-right
}Ptz_Config_Data;

/**
* @brief event log data structure.
*/
typedef struct LogEntry_t{
	char event[50];		///< event description
	struct tm time;		///< event log time
}LogEntry_t;

/**
* @brief user settings structure.
*/
typedef struct _login_data_t{
	char id[USER_LEN+1];
	char password[PASSWORD_LEN+1];
	char remote_ip[IP_STR_LEN];
}login_data_t;

typedef struct _get_user_authority_t{
	char user_id[USER_LEN+1];
	int	authority;
}get_user_authority_t;

typedef struct _add_user_t{
	char	user_id[USER_LEN+1];
	char	password[PASSWORD_LEN+1];
	unsigned char	authority;
}add_user_t;

/* Onvif Structures */
typedef struct _media_profile{
	char profile_name[20];
	char profile_token[20];
}media_profile;


typedef struct _video_source_conf_in{
	char                                   Vname_t[MAX_PROF_TOKEN];
	int                                    Vcount_t;         // count for number of users
	char                                   Vtoken_t[MAX_CONF_TOKEN];             //video source configuration token
	char                                   Vsourcetoken_t[MAX_PROF_TOKEN];        //video source profile token
	int                                    windowx_t;             //x cordinate of window
	int                                    windowy_t;             //y cordinate of window
	int                                    windowwidth_t;          //width
	int                                    windowheight_t;        //height
}video_source_conf_in;

typedef struct _video_source_conf{
	int                                    position;            //position of profile
	video_source_conf_in  video_source_conf_in_t;

}video_source_conf;

typedef struct _Video_Multicast_configuration_in{
	int                                    nIPType_t;               //ip address IPv4/IPv6
	char                                   IPv4Addr_t[MAX_IP_ADDR];               //ipv4 address
	char                                   IPv6Addr_t[MAX_IP_ADDR];               //ipv6 address
	int                                    port_t;                   //port number
	int                                    ttl_t;                     //TTL
	int                                    autostart_t;                //autostart

}Video_Multicast_configuration_in;

typedef struct _video_encoder_conf_in{
	char                                   VEname_t[MAX_PROF_TOKEN] ;                 //video encoder name
	char                                   VEtoken_t[MAX_CONF_TOKEN];                 //video encoder token
	int                                    VEusercount_t;            //video encoder user count
	float                                  VEquality_t;              //video encoder quality
	int                                    Vencoder_t;             //video encoder for jpeg, mpeg, h.264
	int                                    Rwidth_t;               //resolution width
	int                                    Rheight_t;                //resolution height
	int                                    frameratelimit_t;        //frameratelimit
	int                                    encodinginterval_t;       //encodinginterval
	int                                    bitratelimit_t;            //bitratelimit
	int                                    Mpeggovlength_t;              //mpeg GoVLength
	int                                    Mpegprofile_t;                 //mpegprofile SP/ASP
	int                                    H264govlength_t;               //H264 govlength
	int                                    H264profile_t;                 //H264 profile  baseline/main/extended/high
	long                                   sessiontimeout_t;
	Video_Multicast_configuration_in       Multicast_Conf_t;

}video_encoder_conf_in;

typedef struct _video_encoder_conf{
	int                                    position;
	video_encoder_conf_in                  video_encoder_conf_in_t;
}video_encoder_conf;

typedef struct _video_analytics_conf_in{
	char                                  VAname_t[MAX_PROF_TOKEN];                    //video analytic name
	char                                  VAtoken_t[MAX_CONF_TOKEN];                  //video analytic token
	int                                   VAusercount_t;              //video analytic user count
	int                                   VARsize_t;                 //video analytic rule size
	int                                   VARSsizeitem_t;               //size of simple items for ruleengine
	char                                  VARSname_t[MAX_LENGTH];                   //simple item name for rule engine
	char                                  VARSvalue_t[MAX_LENGTH];                 //simple item value for rule engine
	int                                   VAREsize_t;                 //Element size for rule engine
	char                                  VAREname_t[MAX_LENGTH];                   //element name for rule engine
	char                                  Rconfigname_t[MAX_LENGTH];
	char                                  Rconfigtype_t[MAX_LENGTH];
	int                                   VAMsize_t;                  //video analytic module size
	int                                   VAsizeitem_t;               //size of simple items
	char                                  VASname_t[MAX_LENGTH];                   //simple item name
	char                                  VASvalue_t[MAX_LENGTH];                 //simple item value
	int                                   VAEsize_t;                 //Element size
	char                                  VAEname_t[MAX_LENGTH];                   //element name
	char                                  Aconfigname_t[MAX_LENGTH];
	char                                  Aconfigtype_t[MAX_LENGTH];
}video_analytics_conf_in;

typedef struct _video_analytics_conf{
	int                                   position;
	video_analytics_conf_in               video_analytics_conf_t;
}video_analytics_conf;

typedef struct _video_output_conf_in{
	char name[MAX_CONF_TOKEN];
	int usecount;
	char token[MAX_CONF_TOKEN];
}video_output_conf_in;

typedef struct _video_output_conf{
	int position;
	video_output_conf_in 		      video_output_conf_t;
}video_output_conf;

typedef struct _Onvif_Scopes_t{
	char type_t[3][100];
	char name_t[50];
	char location_t[3][100];
	char hardware_t[50];
#if 1 //stfu@20140616 Fix for ONVIF Profile S
	char profile[50];
	char improper_input_t[3][100];
#endif
} Onvif_Scopes_t;

typedef struct _relay_conf_t{
	char token_t[MAX_LENGTH];
	int relaymode_t;
	long delaytime_t;
	int idlestate_t;
} relay_conf_t;

typedef struct _relay_conf{
	int position;
	int logicalstate_t;
	relay_conf_t relay_conf_in_t;
}relay_conf;

typedef struct _Add_AudioSource_Configuration_t{
	char                                   Aname_t[MAX_LENGTH];                 //audio name
	int                                    Ausecount_t;                //audio user count
	char                                   Atoken_t[MAX_LENGTH];                //audio token
	char                                   Asourcetoken_t[MAX_LENGTH];          //audio source token
}Add_AudioSource_Configuration_t;

typedef struct _Add_AudioSource_Configuration_tmp{
	int position;
	Add_AudioSource_Configuration_t Add_AudioSource_in;
}Add_AudioSource_Configuration_tmp;

typedef struct _Add_AudioEncoder_Configuration_t{
	char                                   AEname_t[MAX_LENGTH];                     //audio encoder name
	char                                   AEtoken_t[MAX_LENGTH];                   //audio encoder token
	int                                    AEusercount_t;               //audio encoder user count
	int                                    bitrate_t;                    //Bitrate
	int                                    samplerate_t;                //Samplerate
	int 				       AEencoding_t;

}Add_AudioEncoder_Configuration_t;

typedef struct _Add_AudioEncoder_Configuration_tmp{

	int position;
	Video_Multicast_configuration_in multicast_in;
	Add_AudioEncoder_Configuration_t Add_AudioEncoder_in;
}Add_AudioEncoder_Configuration_tmp;

typedef struct _Metadata_configuration_in{
	char                                    MDname_t[MAX_LENGTH];
	int                                     MDusecount_t;
	char                                    MDtoken_t[MAX_LENGTH];
	int                                     MDanalytics_t;
	long                                    sessiontimeout_t;
	Video_Multicast_configuration_in       VMC_t;
}Metadata_configuration_in;

typedef struct _Metadata_configuration_tmp{
	int 					position;
	Metadata_configuration_in		MDC_in;
}Metadata_configuration_tmp;

#if 1 //stfu@20140703 FIX RDK_3.8 ONVIF
typedef struct _rtspServerPort{
	long mtype;
	unsigned short rtspServerPort_H264_MPEG4_MJPEG[5];
	int size_Port;
	int RTSP_Enable;
	int change;
} rtspServerPort;
#endif
/* End-Onvif Structures */
/*
typedef enum{
	MOTION_OSD_AREA = 0,
	MOTION_OSD_GRID,
	MOTION_OSD_OFF
} MOTION_OSD;
*/
typedef enum{
	MOTION_OSD_ON_VIDEO1 = 0,
	MOTION_OSD_ON_VIDEO2,
	MOTION_OSD_ON_VIDEO3,
	MOTION_OSD_ON_MJPEG1,
	MOTION_OSD_ON_MJPEG2,
	MOTION_OSD_ON_ALL
} MOTION_OSD_ON;

typedef struct _motionSettings {
	unsigned char MotionDetection;
	int Sensitivity;
	unsigned char MotionOSD;
	unsigned char MotionState;
	unsigned char MotionOSDOn;
} MotionSettings;

typedef struct _motionSettings_tmp {
	int position;
	MotionSettings mdsettings;
} MotionSettings_tmp;

typedef struct _ptzSettings {
	unsigned char enable;
	int digitalZoomMode;
	float	Zoom;
	float digitalZoom;
	float opticalZoom;
} ptzSettings;

typedef struct _focusSettings {
	int FocusMode;
	int AFMode;
	int AFSensitivity;
	int AFActiveTime;
	int AFIntervalTime;
//	int AutoFocusSensitivity;
	int FocusPosition;
} FocusSettings;

typedef struct _MASK_RECT_S {
	__s32 s32X;
	__s32 s32Y;
	__u32 u32Width;
	__u32 u32Height;
} MASK_RECT_S;

typedef struct _maskSettings {
	__u8 mask;
	__u32 maskColor;
	MASK_RECT_S stRegion[8];	
} maskSettings;

typedef struct _presetSettings {
	int powerOnPreset;
	int powerOnPresetNumber;
//	int presetSwitchMode;
	int presetNumber;
//	int nightPresetNumber;
//	int nightPresetStartingTime;
//	int nightPresetEndingTime;
} presetSettings;

/**
* @brief system info main data structure.
*/
typedef struct SysInfo{
//	unsigned int uBootEnvOffset;
//	unsigned char UnencryptedUpdate;
//	unsigned long relUniqueId;			///< IPCAM device type
	unsigned int nprofile;              //number of media profiles
	unsigned int nrelays;				//number of relay configs
	Acount_t	acounts[ACOUNT_NUM];	///< user account data
	char language[LANGUAGE_LEN+1];
	unsigned short DeviceType;			///< IPCAM device type
	__u8 reloadFlag;
	Audio_Config_Data audio_config;
//	StreamParam stream_config[MAX_CODEC_STREAMS];
	VideoOutputSettings vo;
	VideoSourceSettings videoSource;
	AudioSourceParam AudioSource[1];
	AudioEncParam Audio[1];
	AudioOutputParam AudioOutput;
	SdcardSettings sd;
	RecordingSettings VideoRecording[3];
	MotionSettings md[MAX_VS_NUM];
	ptzSettings ptz;
//	OSD_SETTINGS_S osd;
	FocusSettings focus;
//	maskSettings mask;
	presetSettings preset;
	Lancam_Config_Data lan_config;		///< IPCAM configuration data
	LogEntry_t	tCurLog;				///< event log
  Onvif_profile   Oprofile[MAX_PROF];                             //Onvif Media profile
  Onvif_Scopes    onvif_scopes;                         //Onvif scopes
	Onvif_Relay     onvif_relay[MAX_RELAYS];				//Onvif_relays
	Imaging_Conf    imaging_conf[MAX_IMAGING];
	__u32 mmcqdPid;                     ///< PID of mmcqd process
	rtspServerPort RtspServerPort;	//stfu@20140703 FIX RDK_3.8 ONVIF
//	int SystemConfig[5];
//	int StreamCount;
//	int MJPEGCount;
}SysInfo;


#endif /* __SYS_ENV_TYPE_H__ */
