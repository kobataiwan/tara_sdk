#ifndef __AV_ENV_TYPE_H__
#define __AV_ENV_TYPE_H__

#include "Msg_Def.h"
#include "tara_preset.h"

//#define	MAX_AVSERVER_STREAM		5
#define	USER_RECT_MAX		2
#define MASK_RECT_MAX		4
#define OSD_TEXT_MAX		6
#define OSD_IMAGE_MAX		1
/*******************************************************
    structure define 
*******************************************************/
typedef struct _Rect_s
{
	int x;
	int y;
	int width;
	int height;
} Rect_s;

typedef struct _ThreadInfo_s
{
	int bRun;
	pthread_t hThread;
} ThreadInfo_s;

typedef struct _RectParam_s
{
	int id;
	int show;
	int type;		//draw or fill
	int color;
	int lineWidth;
	Rect_s stRect;
} RectParam_s;

typedef struct _osdRect_s
{
	ThreadInfo_s thread;
	int used;
	RectParam_s rect[USER_RECT_MAX];
} osdRect_s;

typedef struct mask_cfg_data_s
{
	ThreadInfo_s thread;
	__u8 maskOn;
	__u32 maskColor;
	RectParam_s rect[MASK_RECT_MAX];
	int update;
} mask_cfg_data;

typedef struct _TextParam_s
{
	int id;
	int show;
	int type;	
	int x;
	int y;
	int fontSize;
	int fontColor;
	char text[64];
	int update;
} TextParam_s;

typedef struct _osdText_cfg_data_s
{
	ThreadInfo_s thread;
	__u8 bOSD;
	__u8 bDbg;
	char Font[64];
	TextParam_s Name;
	TextParam_s Time;
	TextParam_s Text[OSD_TEXT_MAX];
} osdText_cfg_data;

typedef struct _noiseReduction_attr_s
{
	int mode;
	float level;
} noiseReduction_attr;

typedef struct _defogging_attr_s
{
	int mode;
	float level;
} defogging_attr;

typedef struct _gamma_attr_s
{
	int mode;
	int offset;
} gamma_attr;

typedef struct _imaging_picture_s
{
	int FlipMirror;
	int IrCutFilter;
	int AutoICRResponseTime;
	int AutoICRThreshold;
	int Constrast;
	int Saturation;
	int Hue;
	int Sharpness;
	defogging_attr Defogging;
	noiseReduction_attr _2DNR;
	noiseReduction_attr _3DNR;
	gamma_attr Gamma;
	int PictureEffect;
} imaging_picture;

typedef struct _whiteBalance_attr_s
{
	int mode;
	int crGain;
	int cbGain;	
} whiteBalance_attr;

typedef struct _wideDynamicRange_attr_s
{
	int mode;
	int ratio;
} wideDynamicRange_attr;

typedef struct _backlightCompensation_attr_s
{
	int mode;
	int level;
} backlightCompensation_attr;

typedef struct _exposureCompensation_attr_s
{
	int mode;
	float level;
} exposureCompensation_attr;

typedef struct _imaging_exposure_s
{
	int Brightness;
	backlightCompensation_attr  BacklightCompensation;
	exposureCompensation_attr ExposureCompensation;
	int FlickerControl;
	int ExposureMode;
	int ExposurePriority;
	float MaxExposureTime;
	float MinExposureTime;
	float ExposureTime;
	float MaxGain;
	float MinGain;
	float Gain;
	float MaxIris;
	float MinIris;
	float Iris;
	float AGain;
	float DGain;
	float ISPGain;
} imaging_exposure;

typedef struct _imaging_settings_s
{
	imaging_picture picture;
	whiteBalance_attr wb;
	imaging_exposure exp;
	wideDynamicRange_attr wdr;
} imaging_settings;

typedef struct _ptz_data_s {
	int mode;
	float	zoom;
	float digitalZoom;
	float opticalZoom;
} ptz_data;

typedef struct _videoSource_data_s {
	int framerate;
	int freezeframe;
} videoSource_data;

typedef struct _videoOutput_data_s {
	int on;
	int mode;
	int timing;
} videoOutput_data;

typedef struct _preset_attr_s{
	int type;
	char name[64];
} preset_attr_s;

typedef struct _preset_settings_s {
	int num;
	preset_attr_s preset;
} preset_settings_s;

typedef struct _preset_data_s {
	int powerOnPreset;
	int firstPresetNumber;
	int lastPresetNumber;
	int powerOnPresetNumber;
	int currentPresetNumber;
	int presetSwitchMode;
	int presetNumber;
	int nightPresetNumber;
	int nightPresetStartingTime;
	int nightPresetEndingTime;
//	preset_attr_s list;
} preset_data;

typedef struct _presetData_s{
	int num;
	int type;
	PRESET_S data;
} presetData_s;

typedef enum
{
	OSD_RECT_TYPE_DRAW,
	OSD_RECT_TYPE_FILL
} OSD_RECT_TYPE;

typedef enum
{
	OSD_TEXT_TYPE_DATE = 0,
	OSD_TEXT_TYPE_TIME,
	OSD_TEXT_TYPE_DATATIME,
	OSD_TEXT_TYPE_TEXT,
	OSD_TEXT_TYPE_MAX
} OSD_TEXT_TYPE;

typedef enum
{
	IMG_LIB_PACIDAL = 0,
	IMG_LIB_LUMENS
} IMG_LIB;

typedef enum{
	IMG_WB_MODE_AUTO = 0,
	IMG_WB_MODE_INDOOR,
	IMG_WB_MODE_OUTDOOR,
	IMG_WB_MODE_ONEPUSHWB,
	IMG_WB_MODE_ATW,
	IMG_WB_MODE_MANUAL,
	IMG_WB_MODE_WIDEAUTO,
	IMG_WB_MODE_MAX
} IMG_WB_MODE;

#if defined(VC_M210P_A)
#define VIDEO_SOURCE_DEFAULT_FPS		60
#else
#define VIDEO_SOURCE_DEFAULT_FPS		30
#endif

#endif /* End of #ifndef __AV_ENV_TYPE_H__ */
