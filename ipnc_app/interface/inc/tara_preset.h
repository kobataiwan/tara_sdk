#ifndef _TARA_PRESET_H_
#define _TARA_PRESET_H_

typedef enum {
	PRESET_TYPE_IMAGING = 0,
	PRESET_TYPE_POSITION,
	PRESET_TYPE_VISCA,
	PRESET_TYPE_CUSTOM,
	PRESET_TYPE_END
} PRESET_TYPE;

typedef enum {
	PRESET_SWITCH_MODE_MANUAL = 0,
	PRESET_SWITCH_MODE_AUTO,
	PRESET_SWITCH_MODE_LIGHT_SENSOR,
	PRESET_SWITCH_MODE_TIME_BASED,
	PRESET_SWITCH_MODE_END
} PRESET_SWITCH_MODE;

typedef struct {
	int digitalZoomMode;
	float	zoom;
	float digitalZoom;
	float opticalZoom;
} ZOOM_CFG_S;

typedef struct _FOCUS_CFG_S{
	int FocusMode;
	int AFMode;
	int AFSensitivity;
	int AFActiveTime;
	int AFIntervalTime;
	int FocusPosition;
} FOCUS_CFG_S;

typedef struct _EXP_CFG_S{
	int FlickerControl;
	int HighDynamicRangeMode;
	int HighDynamicRangeLevel;
	int ExposureCompensationMode;
	float ExposureCompensationLevel;
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
} EXP_CFG_S;

typedef struct _WB_CFG_S{
	int WhiteBalanceMode;
	int WhiteBalanceCrGain;
	int WhiteBalanceCbGain;
} WB_CFG_S;

typedef struct _IMAGING_CFG_S{
	int IrCutFilterMode;
	int AutoICRResponseTime;
	int AutoICRThreshold;
	int FlipMirror;
	int Brightness;
	int Contrast;
	int ColorSaturation;
	int Hue;
	int Sharpness;
	int DefoggingMode;
	float DefoggingLevel;
	int _2DNoiseReductionMode;
	float _2DNoiseReductionLevel;
	int _3DNoiseReductionMode;
	float _3DNoiseReductionLevel;
	int GammaMode;
	int PictureEffect;
} IMAGING_CFG_S;

typedef struct {
/* Zoom */
	ZOOM_CFG_S zoom;

/* Focus */
	FOCUS_CFG_S focus;

/* Exposure */
	EXP_CFG_S	exp;

/* White Balance */
	WB_CFG_S wb;

/* Picture */
	IMAGING_CFG_S imaging;
} PRESET_S;

int TARA_preset_LTI_load(int num, PRESET_S *pstPreset);
int TARA_preset_LTI_save(int num, int type, PRESET_S stPreset);
int TARA_preset_load(int num);
int TARA_preset_save(int num, int type);
int TARA_preset_clear(int num);

int TARA_preset_setPowerOnPreset(int on);
int TARA_preset_getPowerOnPreset(int *on);

int TARA_preset_getFirstPresetNum(int *num);
int TARA_preset_getLastPresetNum(int *num);
int TARA_preset_getPowerOnPresetNum(int *num);
int TARA_preset_setPowerOnPresetNum(int num);
int TARA_preset_getCurrentPresetNum(int *num);

int TARA_preset_getPresetSwitchMode(int *mode);
int TARA_preset_setPresetSwitchMode(int mode);
int TARA_preset_getPresetNum(int *num);
int TARA_preset_setPresetNum(int num);
int TARA_preset_getNightPresetNum(int *num);
int TARA_preset_setNightPresetNum(int num);
int TARA_preset_getNightPresetTime(int *startTime, int *endTime);
int TARA_preset_setNightPresetTime(int startTime, int endTime);

int TARA_preset_getPresetNames(char **name);
int TARA_preset_getPresetName(int num, char *name);
int TARA_preset_setPresetName(int num, char *name);

#endif /*_TARA_PRESET_H_*/
