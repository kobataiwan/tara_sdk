/* ===========================================================================
* @file AVDrvMsg.h
*
* @path $(IPNCPATH)/include/
*
* @desc Message driver for av_interface
* .
* Copyright (c) Pacidal Inc.
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#ifndef _AVDRVMSG_
#define _AVDRVMSG_

#if defined (__cplusplus)
extern "C" {
#endif

/**
  \file avDrvMsg.h

  \brief Message driver for av_interface
*/

#include <Msg_Def.h>
//#include <sys_env_type.h>
//#include "av_env_type.h"


/* Only one AVDrvInit is allowed in each process */
int AVDrvInit(int proc_id);
int AVDrvExit();
/* API */
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt, int chn);
FrameInfo_t GetCurrentFrameInfo(int num);
FrameInfo_t GetLastMjpegFrame(int num);
FrameInfo_t ResetFrameBuffer(int chn);
#if 0
void SendQuitCmd();
int SetBrightness(unsigned char nValue);
int SetContrast(unsigned char nValue);
int SetSaturation(unsigned char nValue);
int SetHue(unsigned char nValue);
int SetSharpness(unsigned char nValue);
int Set2DNoiseReductionMode(unsigned char nValue);
int Set2DNoiseReductionLevel(float nValue);
int Set3DNoiseReductionMode(unsigned char nValue);
int Set3DNoiseReductionLevel(float nValue);
int SetIRCutFilter(unsigned char nValue);
int SetIRCutFilterResponseTime(float nValue);
int SetIRCutFilterThreshold(float nValue);
int SetWhiteBalanceMode(unsigned char nValue);
int SetWBOnePushTriggerMsg(unsigned char nValue);
int SetWhiteBalanceCrGain(unsigned char nValue);
int SetWhiteBalanceCbGain(unsigned char nValue);
int SetBacklightCompensationMode(unsigned char nValue);
int SetBacklightCompensationLevel(unsigned char nValue);
int SetWideDynRangeMode(unsigned char nValue);
int SetWideDynRangeLevel(unsigned char nValue);
int SetFlickerControl(unsigned char nValue);
int SetExposureCompensationMode(unsigned char nValue);
int SetExposureCompensationLevel(float nValue);
int SetExposureMode(unsigned char nValue);
int SetExposurePriority(unsigned char nValue);
int SetExposureTime(unsigned int nValue);
int SetMaxExposureTime(unsigned int nValue);
int SetMinExposureTime(unsigned int nValue);
int SetExposureGain(unsigned int nValue);
int SetAGain(float nValue);
int SetDGain(float nValue);
int SetISPGain(float nValue);
int SetMaxIris(float nValue);
int SetMinIris(float nValue);
int SetIris(float nValue);
int SetFocusMsg(FocusSettings* pPrm);
int SetFocusTrigger(int nValue);
int SetFocusPosition(int nValue);
int GetFocusPosition(void);
int SetPictureEffect(int nValue);
int SetDefoggingMode(int nValue);
int SetDefoggingLevel(float nValue);
int SetGammaMode(int nValue);
int SetMaxExposureGain(unsigned int nValue);
int SetMinExposureGain(unsigned int nValue);

int SetFlipMirror(int nValue);
int SetDIActiveMode(int nValue);
unsigned char GetDIVoltageLevel(void);
unsigned char GetRecoveryVoltageLevel(void);
unsigned char GetMotionStatus(void);
int SetDOActiveMode(int nValue);
int SetDOLogicalState(int nValue);

//int SetAACBitrate(int nValue);

//int SendFaceDetectMsg(FaceDetectParam* faceParam);
//int SetClipSnapName(char* strText, int nLength);
//int SetClipSnapLocation(unsigned char nValue);
//int SetDynRangeParam(void);
int SetWideDynRangePrmMsg(DynRangePrm* dynRangePrm);
int SetVencConfMsg(CodecParam *codecPrm);
int SetAndRestartVencMsg(CodecParam *codecPrm);
int SetAndRestartAudioMsg(int idx, AudioEncParam *codecPrm);
int SetAudioSourceMsg(int idx, AudioSourceParam *codecPrm);
int SetAudioOutputGainMsg(int gain);
int SetAudioOutputFileMsg(AudioOutputParam *outPrm);
int SetMotionDetectMsg(MotionSettings_tmp *outPrm);
int SetPTZSettingsMsg(ptzSettings *outPrm);
int SetCodecPTSMsg(void);
int SetAVRestartMsg(void);
int SetOSDNameMsg(OSD_PARAMS *osdparm);
int SetOSDTimeMsg(OSD_PARAMS *osdparm);
//int SetVideoOutputMsg(VideoOutputSettings *parm);
//int SetMaskMsg(maskSettings *parm);
//int SetMaskRectMsg(maskSettings *parm);

int av_drawMaskRectMsg(RectParam_s *parm);
RectParam_s av_getMaskAttrMsg(int id);
int av_setMasksVisibilityMsg(int on);
int av_getMasksVisibilityMsg(void);
int av_setMasksColorMsg(int color);
int av_getMasksColorMsg(void);

int av_createRectMsg(int osdid);
int av_deleteRectMsg(int osdid);
int av_drawRectMsg(RectParam_s *parm);
RectParam_s av_getRectAttrMsg(int id);

int av_drawNameMsg(TextParam_s *parm);
TextParam_s av_getNameMsg(void);
int av_setNameVisibilityMsg(int on);

int av_drawTimeMsg(TextParam_s *parm);
TextParam_s av_getTimeMsg(void);
int av_drawTextMsg(TextParam_s *parm);
TextParam_s av_getTextAttrMsg(int id);
int av_setTextVisibilityMsg(TextParam_s *parm);

wideDynamicRange_attr av_getImagingWDRMsg(void);
int av_setImagingWDRMsg(wideDynamicRange_attr *parm);
float av_getImagingGainMsg(void);
float av_getImagingAGainMsg(void);
float av_getImagingDGainMsg(void);
float av_getImagingISPGainMsg(void);
unsigned long av_getImagingExposureTimeMsg(void);

int av_setPtzfDigitalZoomModeMsg(int prm);
int av_getPtzfDigitalZoomModeMsg(void);
int av_setPtzfZoomMsg(float prm);
float av_getPtzfZoomMsg(void);
int av_setPtzfDigitalZoomMsg(float prm);
float av_getPtzfDigitalZoomMsg(void);
int av_setPtzfOpticalZoomMsg(float prm);
float av_getPtzfOpticalZoomMsg(void);
float av_getPtzfOpticalZoomStateMsg(void);
int av_setVideoSourceFrameRateMsg(int prm);
int av_getVideoSourceFrameRateMsg(void);
int av_setVideoFreezeFrameMsg(int prm);
int av_getVideoFreezeFrameMsg(void);
videoOutput_data av_getVideoOutputAttrMsg(void);
int av_setVideoOutputAttrMsg(videoOutput_data *parm);
//int av_setVideoOnOffMsg(VencPrm *parm);


int av_loadPresetMsg(int parm);
int av_savePresetMsg(preset_settings_s *parm);
int av_clearPresetMsg(int parm);
int av_setPowerOnPreset(int on);
int av_getPowerOnPreset(void);
int av_setPresetNameMsg(preset_settings_s *parm);
int av_getPresetFirstNumMsg(void);
int av_getPresetLastNumMsg(void);
int av_getPresetPWROnNumMsg(void);
int av_getPresetCurrentNumMsg(void);
int av_getPresetNumMsg(void);
PRESET_S av_presetLTILoadMsg(int num);
int av_presetLTISaveMsg(presetData_s *parm);
int av_setUtcEnterMsg(int prm);
int av_setUtcUpMsg(int prm);
int av_setUtcDownMsg(int prm);
int av_setUtcRightMsg(int prm);
int av_setUtcLeftMsg(int prm);
#endif

#if defined (__cplusplus)
}
#endif

#endif

