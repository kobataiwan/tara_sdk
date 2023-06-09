#ifndef _ISP_MTR_H_
#define _ISP_MTR_H_

//=============================================================================
// constant define
//=============================================================================
#define MOVE_DIR_INIT               0
#define MOVE_DIR_FWD                1
#define MOVE_DIR_BWD                (-1)

#define MOTOR_STATUS_FREE           0
#define MOTOR_STATUS_BUSY           1

#define ZOOM_SECTION_WIDE           0
#define ZOOM_SECTION_TELE           10
#define ZOOM_SECTION_NUM            (ZOOM_SECTION_TELE + 1)

#define FOCUS_DISTANCE_TABLE_NUM    13

//=============================================================================
// struct & definition
//=============================================================================
typedef enum _MOTOR_ID {
	MTR_ID_0,
	///MTR_ID_1,
	MTR_ID_MAX,
	ENUM_DUMMY4WORD(MOTOR_ID)
} MOTOR_ID;

typedef enum _MTR_SPEED {
	MTR_SPEED_OFF  = 0,
	MTR_SPEED_FULL = 100,
	MTR_SPEED_MAX  = 100,
	ENUM_DUMMY4WORD(MTR_SPEED)
} MTR_SPEED;

typedef enum _MTR_GET_FOCUS_CMD {
	MTR_GET_FOCUS_SPEED = 1,
	MTR_GET_FOCUS_FOCAL_LENGTH,
	MTR_GET_FOCUS_RANGE,
	MTR_GET_FOCUS_POSITION,
	MTR_GET_FOCUS_BUSY_STATUS,
	MTR_GET_MOTION_QUEUE_CMD_STATUS,
	ENUM_DUMMY4WORD(MTR_GET_FOCUS_CMD)
} MTR_GET_FOCUS_CMD;

typedef enum _MTR_SET_FOCUS_CMD {
	MTR_SET_FOCUS_INIT = 1,
	MTR_SET_FOCUS_SPEED,
	MTR_SET_FOCUS_ASSIGN_POSITION,
	MTR_SET_FOCUS_FOCAL_LENGTH,
	MTR_SET_FOCUS_POSITION,
	MTR_SET_FOCUS_POSITION_IN_QUEUE,
	MTR_SET_FOCUS_PRESS,
	MTR_SET_FOCUS_RELEASE,
	ENUM_DUMMY4WORD(MTR_SET_FOCUS_CMD)
} MTR_SET_FOCUS_CMD;

typedef enum _MTR_GET_ZOOM_CMD {
	MTR_GET_ZOOM_SPEED = 1,
	MTR_GET_ZOOM_SECTION_POS,
	MTR_GET_ZOOM_MAX_SECTION_POS,
	MTR_GET_ZOOM_POSITION,
	MTR_GET_ZOOM_BUSY_STATUS,
	ENUM_DUMMY4WORD(MTR_GET_ZOOM_CMD)
} MTR_GET_ZOOM_CMD;

typedef enum _MTR_SET_ZOOM_CMD {
	MTR_SET_ZOOM_INIT = 1,
	MTR_SET_ZOOM_SPEED,
	MTR_SET_ZOOM_ASSIGN_POSITION,
	MTR_SET_ZOOM_SECTION_POS,
	MTR_SET_ZOOM_POSITION,
	MTR_SET_ZOOM_PRESS,
	MTR_SET_ZOOM_RELEASE,
	MTR_SET_ZOOM_POSITION_CALI,
	ENUM_DUMMY4WORD(MTR_SET_ZOOM_CMD)
} MTR_SET_ZOOM_CMD;

typedef enum _MTR_GET_IRCUT_CMD {
	MTR_GET_IRCUT_POSITION = 1,
	ENUM_DUMMY4WORD(MTR_GET_IRCUT_CMD)
} MTR_GET_IRCUT_CMD;

typedef enum _MTR_SET_IRCUT_CMD {
	MTR_SET_IRCUT_INIT = 1,
	MTR_SET_IRCUT_POSITION,
	ENUM_DUMMY4WORD(MTR_SET_IRCUT_CMD)
} MTR_SET_IRCUT_CMD;

typedef enum _MTR_GET_APERTURE_CMD {
	MTR_GET_APERTURE_INFO = 1,
	MTR_GET_APERTURE_FNO,
	MTR_GET_APERTURE_SECTION_POS,
	MTR_GET_APERTURE_POSITION,
	MTR_GET_APERTURE_FLUX_RATIO,
	MTR_GET_APERTURE_BUSY_STATUS,
	ENUM_DUMMY4WORD(MTR_GET_APERTURE_CMD)
} MTR_GET_APERTURE_CMD;

typedef enum _MTR_SET_APERTURE_CMD {
	MTR_SET_APERTURE_INIT = 1,
	MTR_SET_APERTURE_ASSIGN_POSITION,
	MTR_SET_APERTURE_SECTION_POS,
	MTR_SET_APERTURE_POSITION,
	MTR_SET_APERTURE_FLUX_RATIO,
	ENUM_DUMMY4WORD(MTR_SET_APERTURE_CMD)
} MTR_SET_APERTURE_CMD;

typedef enum _MTR_GET_SHUTTER_CMD {
	MTR_GET_SHUTTER_STATE = 1,
	ENUM_DUMMY4WORD(MTR_GET_SHUTTER_CMD)
} MTR_GET_SHUTTER_CMD;

typedef enum _MTR_SET_SHUTTER_CMD {
	MTR_SET_SHUTTER_INIT = 1,
	MTR_SET_SHUTTER_STATE,
	ENUM_DUMMY4WORD(MTR_SET_SHUTTER_CMD)
} MTR_SET_SHUTTER_CMD;

typedef enum {
	MTR_GET_MISC_NAME_STR = 1,
	MTR_GET_MISC_CAPABILITY,
	MTR_GET_MISC_ZF_RANGE_TABLE,
	MTR_GET_MISC_ZF_CURVE_TABLE,
	MTR_GET_MISC_LED_STATE,
	MTR_GET_MISC_FZ_BACKLASH,
	ENUM_DUMMY4WORD(MTR_GET_MISC_CMD)
} MTR_GET_MISC_CMD;

typedef enum _MTR_SET_MISC_CMD {
	MTR_SET_MISC_ZF_RANGE_TABLE = 1,
	MTR_SET_MISC_ZF_CURVE_TABLE,
	MTR_SET_MISC_LED_STATE,
	MTR_SET_MISC_FZ_BACKLASH,
	ENUM_DUMMY4WORD(MTR_SET_MISC_CMD)
} MTR_SET_MISC_CMD;

typedef enum _MOTOR_SPEED_CATEGORY {
	MOTOR_SPEED_VERY_LOW  = 20,
	MOTOR_SPEED_LOW       = 40,
	MOTOR_SPEED_MEDIUM    = 60,
	MOTOR_SPEED_HIGH      = 80,
	MOTOR_SPEED_VERY_HIGH = 100,
	ENUM_DUMMY4WORD(MOTOR_SPEED_CATEGORY)
} MOTOR_SPEED_CATEGORY;

typedef enum _MOTOR_ROTATE_DIR {
	MOTOR_ROTATE_FWD,
	MOTOR_ROTATE_BWD,
	ENUM_DUMMY4WORD(MOTOR_ROTATE_DIR)
} MOTOR_ROTATE_DIR;

typedef enum _IRCUT_ACT {
	IRCUT_OPEN,
	IRCUT_CLOSE,
	ENUM_DUMMY4WORD(IRCUT_ACT)
} IRCUT_ACT;

typedef enum _APERTURE_ACT_MODE {
	APERTURE_ACT_SECTION,
	APERTURE_ACT_STEPLESS,
	ENUM_DUMMY4WORD(APERTURE_ACT_MODE)
} APERTURE_ACT_MODE;

typedef enum _APERTURE_SEC_POS {
	APERTURE_POS_BIG  = 0,
	APERTURE_POS_F1_6 = 0,
	APERTURE_POS_F2_0,
	APERTURE_POS_F2_4,
	APERTURE_POS_F2_8,
	APERTURE_POS_F3_4,
	APERTURE_POS_F4_0,
	APERTURE_POS_F4_8,
	APERTURE_POS_F5_6,
	APERTURE_POS_F6_8,
	APERTURE_POS_F9_6,
	APERTURE_POS_F11_0,
	APERTURE_POS_F14_0,
	APERTURE_POS_F16_0,
	APERTURE_POS_F19_0,
	APERTURE_POS_F22_0,
	APERTURE_POS_SMALL = APERTURE_POS_F22_0,
	APERTURE_POS_CLOSE,
	APERTURE_POS_MAX,
	ENUM_DUMMY4WORD(APERTURE_SEC_POS)
} APERTURE_SEC_POS;

typedef enum _SHUTTER_ACT {
	SHUTTER_OPEN,
	SHUTTER_CLOSE,
	ENUM_DUMMY4WORD(SHUTTER_ACT)
} SHUTTER_ACT;

typedef enum _MOTOR_SIGNAL {
	MOTOR_SIGNAL_FPI,
	MOTOR_SIGNAL_ZPI,
	MOTOR_SIGNAL_LED,
	ENUM_DUMMY4WORD(MOTOR_SIGNAL)
} MOTOR_SIGNAL;

typedef enum _HW_GPIO_PIN_DEF {
	HW_GPIO_RESET = 0,
	HW_GPIO_POWER_DN,
	HW_GPIO_FZ,
	HW_GPIO_IS,
	HW_GPIO_IRCUT0,
	HW_GPIO_IRCUT1,
	HW_GPIO_LED0,
	HW_GPIO_LED1,
	HW_GPIO_FSTS,
	HW_GPIO_ZSTS,
	HW_GPIO_EXT0,
	HW_GPIO_EXT1,
	HW_GPIO_EXT2,
	HW_GPIO_EXT3,
	HW_GPIO_MAX,
	ENUM_DUMMY4WORD(HW_GPIO_PIN_DEF)
} HW_GPIO_PIN_DEF;

typedef enum _LENS_SUPPORT_PROPERTY {
	LENS_SUPPORT_NONE     = 0x00000000,
	LENS_SUPPORT_FOCUS    = 0x00000001,
	LENS_SUPPORT_ZOOM     = 0x00000002,
	LENS_SUPPORT_IRCUT    = 0x00000004,
	LENS_SUPPORT_APERTURE = 0x00000008,
	LENS_SUPPORT_SHUTTER  = 0x00000010,
	ENUM_DUMMY4WORD(LENS_SUPPORT_PROPERTY)
} LENS_SUPPORT_PROPERTY;

typedef struct _MTR_CTL_CMD {
	UINT32 argu[2];
	UINT32 data[2];
} MTR_CTL_CMD;

typedef struct _MTR_CTL_LCMD {
	UINT32 argu[16];
	UINT32 data[16];
} MTR_CTL_LCMD;

typedef struct _LENS_ZOOM_FOCUS_TAB {
	INT32 zoom_position;
	INT32 focus_min;
	INT32 focus_max;
} LENS_ZOOM_FOCUS_TAB;

typedef struct _LENS_ZOOM_FOCUS_ITEM {
	UINT32 tab_idx;
	LENS_ZOOM_FOCUS_TAB zoom_focus_tab;
} LENS_ZOOM_FOCUS_ITEM;

typedef struct _LENS_FOCUS_DISTANCE_TAB {
	INT32 lens_position[FOCUS_DISTANCE_TABLE_NUM];
} LENS_FOCUS_DISTANCE_TAB;

#if defined(__KERNEL__) || defined(__FREERTOS)
typedef struct _CTL_MTR_DRV_TAB {
	ER (*open)         (UINT32 id);
	ER (*close)        (UINT32 id);
	ER (*get_focus)    (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*set_focus)    (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*get_zoom)     (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*set_zoom)     (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*get_ircut)    (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*set_ircut)    (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*get_aperture) (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*set_aperture) (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*get_shutter)  (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*set_shutter)  (UINT32 id, UINT32 cmd_type, MTR_CTL_CMD *pcmd);
	ER (*get_misc)     (UINT32 id, UINT32 cmd_type, MTR_CTL_LCMD *pcmd);
	ER (*set_misc)     (UINT32 id, UINT32 cmd_type, MTR_CTL_LCMD *pcmd);
} CTL_MTR_DRV_TAB;
#endif

#endif

