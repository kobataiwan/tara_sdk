#ifndef __SOC_OSD_H__
#define __SOC_OSD_H__

#include "av_env_type.h"

#if 0
#define RGN_HANDLE_NAME	 0
#define RGN_HANDLE_TIME	 1
extern const int TEXT_RGN_HANDLE_TABLE[OSD_TEXT_MAX];
extern const int USER_RGN_HANDLE_TABLE[USER_RECT_MAX];
extern const int MASK_RGN_HANDLE_TABLE[MASK_RECT_MAX];
extern RGN_ATTR_INFO_S gstOsdRgnAttrInfo;
extern RGN_ATTR_INFO_S gstMaskInfo;
extern osdRect_s userRect;
extern mask_cfg_data mask;
extern osdText_cfg_data osd;
#endif

void osd_start(void);
void osd_stop(void);
int osd_platform_cleanTime(void);
void* osd_MaskDynamicDisplay(void* p);
//void* osd_RectDynamicDisplay(void* p);
void* osd_TextdDynamicDisplay(void* p);
//int osd_addRect(int idx);
//HI_S32 osd_deleteRect(int handle);

#endif /* End of #ifndef __SOC_OSD_H__ */
