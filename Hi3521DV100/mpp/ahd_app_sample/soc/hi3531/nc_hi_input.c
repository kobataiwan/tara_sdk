#include <stdio.h>
#include <string.h>

#include <sys/poll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <linux/limits.h>				// PATH_MAX
#include <fcntl.h>                      // O_RDWR , O_NOCTTY �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕
#include <time.h>
#include <unistd.h>
#include <pthread.h>


#include <linux/kdev_t.h>				// MKDEV
#include <sys/types.h>					// dev_t
#include <sys/stat.h>

#include <_debug_macro.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _IX_HEADER_
#define _IX_HEADER_


#define	FILE_NAME_INPUT_INFO_BASE		"/proc/bus/input/devices"					// �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕移�
//#define	IX_TMP_PATH_DEFAULT				"/tmp"										// mknod�뜝�룞�삕 �뜝�떦源띿삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙂�룞�삕 �뜝�룞�삕�뜝�룞�삕
#define	IX_TMP_PATH_DEFAULT				"/dev"

#define FLAG_SET						1
#define FLAG_RELEASE					0

//#define FILE_TOUCH_CAL_INFO				"/etc/ixlib/ts_calibration_info.db"			// �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 移쇔뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떛�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#define FILE_TOUCH_CAL_INFO				"./ts_calibration_info.db"			// �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 移쇔뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떛�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕

#define IX_KEY_PRESS_MAX		54			// KEY PRESS �뜝�룞�삕 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙇��媛�
// �뜝�떍�룞�삕�궎 �뜝�룞�삕�뜝�떛釉앹삕 �몴
#define	 ASKII_RESERVED 		0x0			//
#define	 ASKII_TAP				0x09
#define	 ASKII_RETURN			0x0d
#define  ASKII_DELETE			0x10
#define  ASKII_ESCAPE			0x1b
#define  ASKII_SPACE			0x20
#define  ASKII_EXCLAM			0x21 			// !
#define  ASKII_QUOTES			0x22			// "
#define  ASKII_HASH				0x23			// #
#define  ASKII_DOLOR			0x24			// $
#define  ASKII_PERCENT			0x25			// %
#define  ASKII_AMPERSAND		0x26			// &
#define  ASKII_SINGLEQUOTE 		0x27			// '
#define  ASKII_OPENPARAN		0x28			// (
#define  ASKII_CLOSEPARAN		0x29			// )
#define  ASKII_STAR				0x2A			// *
#define  ASKII_PLUS				0x2B			// +
#define  ASKII_COMMA			0x2C			// ,
#define  ASKII_MINUS			0x2D			// -
#define  ASKII_DOT				0x2E			// .
#define  ASKII_FORDWARDSLASH	0x2F			// /
#define  ASKII_ZERO				0x30
#define  ASKII_1				0x31
#define  ASKII_2				0x32
#define  ASKII_3				0x33
#define  ASKII_4				0x34
#define  ASKII_5				0x35
#define  ASKII_6				0x36
#define  ASKII_7				0x37
#define  ASKII_8				0x38
#define  ASKII_9				0x39
#define  ASKII_COLON			0x3a			// :
#define  ASKII_SEMICOLON		0x3b			// ;
#define  ASKII_OPENANGLE		0x3c			// <
#define  ASKII_EQUAL			0x3d			// =
#define  ASKII_CLOSEANGLE		0x3e			// >
#define  ASKII_QUESTION			0x3f			// ?
#define  ASKII_AT				0x40  			// @
#define  ASKII_A				0x41
#define  ASKII_B				0x42
#define  ASKII_C				0x43
#define  ASKII_D				0x44
#define  ASKII_E				0x45
#define  ASKII_F				0x46
#define  ASKII_G				0x47
#define  ASKII_H				0x48
#define  ASKII_I				0x49
#define  ASKII_J				0x4a
#define  ASKII_K				0x4b
#define  ASKII_L				0x4c
#define  ASKII_M				0x4d
#define  ASKII_N				0x4e
#define  ASKII_O				0x4f
#define  ASKII_P				0x50
#define  ASKII_Q				0x51
#define  ASKII_R				0x52
#define  ASKII_S				0x53
#define  ASKII_T				0x54
#define  ASKII_U				0x55
#define  ASKII_V				0x56
#define  ASKII_W				0x57
#define  ASKII_X				0x58
#define  ASKII_Y				0x59
#define  ASKII_Z				0x5a
#define  ASKII_OPENSQURE		0x5b			// [
#define  ASKII_BACKSLASH		0x5c			// "\"
#define  ASKII_CLOSESQUARE		0x5d			// ]
#define  ASKII_CARAT			0x5e			// ^
#define  ASKII_UNDERBAR			0x5f			// _
#define  ASKII_GRAVE			0x60			// `
#define  ASKII_a				0x61
#define  ASKII_b				0x62
#define  ASKII_c				0x63
#define  ASKII_d				0x64
#define  ASKII_e				0x65
#define  ASKII_f				0x66
#define  ASKII_g				0x67
#define  ASKII_h				0x68
#define  ASKII_i				0x69
#define  ASKII_j				0x6a
#define  ASKII_k				0x6b
#define  ASKII_l				0x6c
#define  ASKII_m				0x6d
#define  ASKII_n				0x6e
#define  ASKII_o				0x6f
#define  ASKII_p				0x70
#define  ASKII_q				0x71
#define  ASKII_r				0x72
#define  ASKII_s				0x73
#define  ASKII_t				0x74
#define  ASKII_u				0x75
#define  ASKII_v				0x76
#define  ASKII_w				0x77
#define  ASKII_x				0x78
#define  ASKII_y				0x79
#define  ASKII_z				0x7a
#define  ASKII_CURLYOPEN		0x7b			// {
#define  ASKII_PIPE				0x7c			// |
#define  ASKII_CURLYCLOSE		0x7d			// }
#define  ASKII_TILDE			0x7e			// ~




#define IX_ALPABAT_MAX					26					// �뜝�룞�삕�뜝�떇釉앹삕 �뜝�뙇�뙋�삕 �뜝�룞�삕�뜝�룞�삕 A~Z
#define IX_NUMBER_MAX					10					// �뜝�룞�삕�뜝�룞�삕  �뜝�뙇�뙋�삕 �뜝�룞�삕�뜝�룞�삕  0~9

#define IX_INPUT_DEVICE_MAX				16

#define IXERR_NONE						0                   //IXERR_NONE
#define IXERR_READ						1                   //IX READ ERROR
#define IXERR_FD						2                   //IX FD ERROR
#define IXERR_DEV_INFO_OPEN				3  	                //IXERR_DEV_INFO_OPEN
#define IXERR_AUTOSCAN_SCANDEVICE   	4					//ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 ix_scan_device �뜝�룞�삕�뜝�룞�삕
#define IXERR_AUTOSCAN_MKNOD			5					//ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 mknod �뜝�룞�삕�뜝�룞�삕",
#define IXERR_AUTOSCAN_DEVICE_OPEN  	6                   //ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕"
#define IXERR_AUTOSCAN_EX_ADD        	7                   //ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕",
#define IXERR_WRITE_EVENT 		       	8                   //ix_write_event_low �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",
#define IXERR_WRITE_MOUSE_BUTTON_EVENT 	9                   //ix_write_mouse_button_action �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",
#define IXERR_WRITE_KEYBOARD_EVENT		10					//ix_write_keyboard_key_action �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",


#define	IX_KEY_VAULE_UP_LOW				0					// �궎�뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕  LOW �뜝�룞�삕
#define	IX_KEY_VAULE_DOWN_LOW			1					// �궎�뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
#define	IX_KEY_VAULE_PRESS_LOW			2					// �궎�뜝�룞�삕�듃 DOWN->UP �뜝�룞�삕 �뜝�룞�삕, �뜝�룞�삕�궎�뜝�룞�삕 �뜝�뙓�뱶媛믣뜝�룞�삕�뜝�룞�삕 CODE�뜝�룞�삕 �뜝�룞�삕�솚�뜝�떦�뙋�삕.

#define	IX_MOUSE_VAULE_UP_LOW			0					//  �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
#define	IX_MOUSE_VAULE_DOWN_LOW			1					//  �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

//---------------------------------------------------------------------------------
// �뜝�룞�삕�듃 �뜝�룞�삕�뜝�룞�삕�겕
//---------------------------------------------------------------------------------
#define IX_MOUSE_BTN_LEFT				0x00000001			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉
#define IX_MOUSE_BTN_RIGHT				0x00000002			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉
#define IX_MOUSE_BTN_MIDDLE				0x00000004			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�듉

#define IX_MOUSE_FLAG_BTN_DOWN			0x00000001			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
#define IX_MOUSE_FLAG_BTN_UP			0x00000002			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#define IX_MOUSE_FLAG_WHEEL				0x00000004			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
#define IX_MOUSE_FLAG_MOVE				0x00000008			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#define IX_MOUSE_FLAG_DOUBLE_CLICK		0x00000010			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕


#define	IX_KEY_VAULE_UP					0x00000001			// �궎�뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕   , state�뜝�룞�삕 make�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕.
#define	IX_KEY_VAULE_DOWN				0x00000002			// �궎�뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
#define	IX_KEY_VAULE_PRESS				0x00000004			// �궎�뜝�룞�삕�듃 DOWN->UP �뜝�룞�삕 �뜝�룞�삕, �뜝�룞�삕�궎�뜝�룞�삕 �뜝�뙓�뱶媛믣뜝�룞�삕�뜝�룞�삕 CODE�뜝�룞�삕 �뜝�룞�삕�솚�뜝�떦�뙋�삕.
//#define	IX_KEY_VAULE_REPEAT			0x00000008					// �궎�뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕  (�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�)

#define IX_KEYBOARD_BTN_LEFT_SHIFT		0x80000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 SHIFT
#define IX_KEYBOARD_BTN_RIGHT_SHIFT		0x40000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 SHIFT

#define IX_KEYBOARD_BTN_LEFT_ALT		0x20000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 ALT
#define IX_KEYBOARD_BTN_RIGHT_ALT		0x10000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 ALT

#define IX_KEYBOARD_BTN_LEFT_CTRL		0x08000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 CTRL
#define IX_KEYBOARD_BTN_RIGHT_CTRL		0x04000000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL

#define IX_KEYBOARD_CAPSLOCK			0x02000000			// �궎�뜝�룞�삕�뜝�룞�삕 Caps Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕     // imesu 090215
#define IX_KEYBOARD_NUMLOCK				0x01000000			// �궎�뜝�룞�삕�뜝�룞�삕 Num Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕     // imesu 090215

#define IX_KEYBOARD_UP					0x00800000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕  	�뜝�룞�삕�뜝�룞�삕�궎
#define IX_KEYBOARD_DOWN				0x00400000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�떍琉꾩삕�뜝�룞�삕 	�뜝�룞�삕�뜝�룞�삕�궎
#define IX_KEYBOARD_LEFT				0x00200000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 		�뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕     // imesu 090215
#define IX_KEYBOARD_RIGHT				0x00100000			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 	�뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕     // imesu 090215



#define IX_SCREEN_X_DEFAULT				640					// �뜝�룞�삕�겕�뜝�룞�삕 X �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#define IX_SCREEN_Y_DEFAULT				480					// �뜝�룞�삕�겕�뜝�룞�삕 Y �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕

#define IX_CAL_TOUCH_INFO_MAX			4					// �뜝�룞�삕�몴 �뜝�뙃琉꾩삕 �뜝�뙇�뙋�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

#if 0//wj@@test-touch
#define IX_CAL_TOUCH_INFO_OFSSET		1				// TOUCH SCREEN �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#else
#define IX_CAL_TOUCH_INFO_OFSSET		0x00010000          //(1<< 16)// TOUCH SCREEN �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
#endif



#define IX_MOUSE_DOUBLE_CLICK_TIME_INTAVAL	   500			//500msec = 0.5 �뜝�룞�삕
//---------------------------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�뙣�룞�삕�뜝�룞�삕
//---------------------------------------------------------------------------------
#define IE_MOUSE_DOWN					1
#define IE_MOUSE_UP 					2
#define IE_MOUSE_WHEEL 					3
#define IE_MOUSE_MOVE   				4
#define IE_KEY_PRESS 					5
#define IE_KEY_DOWN 					6
#define IE_KEY_UP 						7

typedef struct input_event ix_event_t;					// kernel �뜝�룞�삕 �뜝�뙇�뙋�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕泥� /include/linux/input.h

typedef struct
{
	int     fd;					// �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕
	char 	name[128];			// �뜝�룞�삕移� �뜝�떛紐뚯삕
	int 	input_number;		// �뜝�뙃琉꾩삕�뜝�룞�삕移� �뜝�룞�삕�샇
	int 	event_number;		// event �뜝�룞�삕移� �뜝�룞�삕�샇

	int 	dev_major;			// �뜝�룞�삕移� �뜝�뙇諭꾩삕�샇
	int 	dev_minor;			// �뜝�룞�삕移� �뜝�떥諭꾩삕�샇

	int		is_touch;			// �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�떛紐뚯삕 1 0 �뜝�떛紐뚯삕 �뜝�떍�뙋�삕
	int		is_alive;			// is pluged-in

} ix_mng_t;

typedef  unsigned long ix_btn_state_t;	// �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙓琉꾩삕�뜝�룞�삕

typedef struct {
	int  tx;					// touch screen x�뜝�룞�삕�몴�뜝�룞�삕
	int  ty;					// touch screen y�뜝�룞�삕�몴�뜝�룞�삕

	int  sx;					// lcd screen x�뜝�룞�삕�몴�뜝�룞�삕
	int  sy;					// lcd screen y�뜝�룞�삕�몴�뜝�룞�삕
}ix_cal_touch_info_t;

typedef struct {
	int  xa;					// �뜝�룞�삕�뜝�룞�삕
	int  xb;					//

	int  ya;					// �뜝�룞�삕�뜝�룞�삕
	int  yb;					//
}ix_adjust_touch_info_t;


typedef struct {
	int				x;			//x �뜝�룞�삕�몴
	int				y;			//y �뜝�룞�삕�몴
	int				wheel;		//WHEEL �뜝�룞�삕�솕
	ix_btn_state_t  state;		//�뜝�룞�삕�뜝�룞�삕

}ie_mouse_t;

typedef struct {
	int				key;		//�뜝�뙃琉꾩삕 �몴�뜝�룞�삕
	ix_btn_state_t  state;		//�뜝�룞�삕�뜝�룞�삕
}ie_key_t;

typedef struct {
	int		type;				// �뜝�뙃�젰怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕
	union{
		ie_mouse_t		mouse;	// �뜝�룞�삕�뜝�럩�뒪 �뜝�뙃琉꾩삕
		ie_key_t		key;	// keyboard �뜝�뙃琉꾩삕
	}data;
}ie_event_t;


typedef      int (*ix_event_func_t) (ie_event_t *event_data);

#ifdef __cplusplus
extern "C"
{
#endif


	void 		ix_print_error				( char *msg );								// �뜝�룞�삕�뜝�룞�삕 �뜝�뙣�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�
	void		ix_debug_disp_event_data	( ix_event_t *event_data );					// �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �몴�뜝�룞�삕�뜝�떬�뙋�삕.
	void		ix_debug_disp_ix_device_info( void );									// ix_scan_device �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �솕�뜝�띂�뿉 �몴�뜝�룞�삕�뜝�떬�뙋�삕.


	int 		ix_init						( void );
	int			ix_auto_scan_open			( char *path );								// 1)�뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�뙓�벝�삕 �뜝�떙�궪�삕
																						// 2) mknod
																						// 3) �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕..
																						// 4) ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占�

	int 		ix_get_touch_fd				( char *path );								// �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�룞�삕�뜝�떬�뙋�삕.

	int 		ix_add_event				( ix_event_func_t func );					//�뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕髥��뜝占�.
	int 		ix_scan_device				( void );									// �뜝�뙃琉꾩삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떙�궪�삕�뜝�떬�뙋�삕.

	int 		ix_read_event_data_low		( int fd,  ix_event_t *event_data );		//�뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�떩�뼲�삕 �뜝�룞�삕�뜝�듅怨ㅼ삕
	int 		ix_read_keyboard_low		( int fd,  int *key, int *key_state );		//�궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
	int 		ix_read_key_press_low		( int fd,  int *key );						//�궎�뜝�룞�삕�뜝�뜴媛� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕
	int 		ix_read_touch_low			( int fd,  int *x, int *y, int *press );	// touch �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
	int 		ix_read_mouse_low			( int fd,  int *x, int *y, int *btn);		//�뜝�룞�삕�뜝�럩�뒪 �뜝�뙃�젰怨ㅼ삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�듅�뙋�삕.
	int 		ix_set_screen_size			( int x ,  int y);

	int			ix_write_event_low			( int fd, ix_event_t *event_data );			//�뜝�떛釉앹삕�듃 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕孃��뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뼱�꽌 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
	int			ix_write_mouse_button_action( int mouse_button, int action );			//�뜝�룞�삕�뜝�럩�뒪 �듅�뜝�룞�삕�뜝�룞�삕�듉�뜝�룞�삕 Up down �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
	int			ix_write_mouse_rel_move		( int rel_x, int rel_y );					//�뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
	int			ix_write_mouse_abs_move		( int abs_x, int abs_y );					//�뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
	int			ix_write_keyboard_key_action( int keyboard_key, int action);			//�궎�뜝�룞�삕�뜝�뜴�뿉 �궎 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
	int			ix_write_key_press		( int askii );								//�궎�뜝�룞�삕�뜝�뜴�뿉 key press �뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뒗�뙋�삕.

	int			ix_get_mouse_position		( int *x,  int *y );						// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕夷덂뜝占�.
	int			ix_set_mouse_double_click_intaval( int msec );							// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �겢�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	int			ix_get_key_value			( int *key );								// keyboard �뜝�뙃�젰怨ㅼ삕�뜝�룞�삕 �뜝�룞�삕夷덂뜝占�.


	int			ix_adjust_touch				( void );									// touch screen�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
	//int 		ix_get_touch_position		( int *x, int *y );							// �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�듅�뙋�삕.
    int         ix_get_touch                ( int x_tch, int y_tch, int *x_scr_ptr, int *y_scr_ptr );
    int         ix_get_touch_info           ( ix_adjust_touch_info_t *info );
    int         ix_set_touch                ( ix_adjust_touch_info_t *info );
    int         ix_print_touch              ( void );
    int         ix_adjust_touch_init        ( void );

	int			ix_loading_ts_cailbration_info( void );									//�뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 移쇔뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떛�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떥�벝�삕�뜝�떬�뙋�삕.

	int			ix_write_ts_cailbration_info( char *section, char *keyname ,int value );	// �뜝�룞�삕移섇뜝�룞�삕�겕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕.
	int			system_device_input_autoDetect( void );
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//------------------------------------------------------------------------------






#ifndef _IX_VAR_

#endif

#endif  // _IX_HEADER_


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EX_POLL_LIST_MAX            256
#define EX_WAIT_TIME_DEFAULT        100              // 20 mSec

#define  EXERR_NONE					0                                               //EXERR_NONE
#define  EXERR_WAIT_TIME			1                                               //EXERR_WAIT_TIME
#define  EXERR_POLL_FUNC			2                                               //EXERR_POLL_FUNC
#define  EXERR_POLL_ADD_FD			3                                               //EXERR_POLL_ADD_FD
#define  EXERR_POLL_ADD_MEM			4                                               //EXERR_POLL_ADD_MEM

typedef struct
{
	int 	fd;
	void 	*priv;

	int		(*on_read	)(void * self);
	int		(*on_write	)(void * self);
} ex_mng_t;

typedef struct pollfd pollfd_t;

#ifdef __cplusplus
extern "C"
{
#endif



	int			ex_init					( int wait_time	);
	void		ex_print_error			( char *msg		);

	ex_mng_t *	ex_add_fd				( int fd		);
	void		ex_del_fd					( int fd		);
	int			ex_adjust_poll			( void			);

	int			ex_loop					( void 			);

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
//
// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//
//------------------------------------------------------------------------------

extern int 	(*ex_on_poll_timeout)(void			);
extern int 	(*ex_on_hooking_loop)(int poll_state);


#define _IX_VAR_ 1




//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------

#define	TRACE 		printf("< %s :%d >\n", __FUNCTION__ , __LINE__)
#define ABS( x )   (((x) > 0 )? (x):-(x))		// �뜝�룞�삕�뜝�럥媛�

static	int				ix_isInit					=0;
static 	int				ix_error_code			= IXERR_NONE;				// �뜝�룞�삕�뜝�룞�삕�뜝�뙓�벝�삕
static	ix_mng_t		ix_mng[IX_INPUT_DEVICE_MAX];
static	int				ix_mng_count			= 0;
static	ex_mng_t 		*input_obj[IX_INPUT_DEVICE_MAX];					// �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕黎뷴뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�듃


static int				ix_sreen_x_min = 0;
static int				ix_sreen_y_min = 0;

static int				ix_sreen_x_max = IX_SCREEN_X_DEFAULT;
static int				ix_sreen_y_max = IX_SCREEN_Y_DEFAULT;

static int				ix_mouse_x = IX_SCREEN_X_DEFAULT/2;
static int				ix_mouse_y = IX_SCREEN_Y_DEFAULT/2;
static int				ix_mouse_wheel	= 0;
static int				ix_key			= 0;											// KEY  �뜝�뙃�젰怨ㅼ삕

static ix_btn_state_t	ix_button_state = 0;											// �뜝�룞�삕�듉 �뜝�룞�삕�뜝�듅怨ㅼ삕

static unsigned long	ix_mouse_event_flag	 =0;									// �뜝�룞�삕�뜝�럩�뒪 �뜝�떛釉앹삕�듃 �뜝�뙥�궪�삕 ( up ,down, move ..etc)
static unsigned long	ix_key_event_flag	 =0;									// key    �뜝�떛釉앹삕�듃 �뜝�뙥�궪�삕 ( up ,down ,press )
//static unsigned long	ix_mouse_double_click_flag=0;

static ix_event_func_t  ix_event_func = NULL;			// �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 										//

static ix_cal_touch_info_t  	ix_cal_touch_info[IX_CAL_TOUCH_INFO_MAX];
static ix_adjust_touch_info_t  	ix_adjust_touch_info;

static char 			ix_capslock_flag 	=0;										// �궎�뜝�룞�삕�뜝�룞�삕 caps lock flag
static char 			ix_numlock_flag 	=0;										// �궎�뜝�룞�삕�뜝�룞�삕 caps lock flag

	   int  			ix_mouse_click_interval = IX_MOUSE_DOUBLE_CLICK_TIME_INTAVAL;		// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �겢�뜝�룞�삕 ���뜝�룞�삕 �뜝�룞�삕�뜝�떢諭꾩삕

static struct timeval	ix_pre_time;

static char ix_adjust_flag =0;
static char ix_adjust_touch_flag =0;

static char ix_askii[128] = {
	KEY_RESERVED,

	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,
	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,

	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,
	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,

	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,
	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,


	KEY_RESERVED ,KEY_SPACE ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,
	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED,


	KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_RESERVED ,KEY_MINUS,
	KEY_DOT      ,KEY_SLASH    ,KEY_0		 ,KEY_1		   ,KEY_2	,
	KEY_3,	KEY_4,	KEY_5,	KEY_6, KEY_7,

	KEY_8,	KEY_9,	KEY_RESERVED ,KEY_SEMICOLON , KEY_RESERVED,
	KEY_EQUAL, KEY_RESERVED, KEY_RESERVED,	KEY_RESERVED,

	KEY_A ,KEY_B, KEY_C, KEY_D ,KEY_E,
	KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
	KEY_K, KEY_L, KEY_M, KEY_N ,KEY_O,
	KEY_P, KEY_Q, KEY_R, KEY_S ,KEY_T,
	KEY_U, KEY_V, KEY_W, KEY_X ,KEY_Y,
	KEY_Z,

	KEY_LEFTBRACE, KEY_RESERVED, KEY_RIGHTBRACE ,KEY_RESERVED ,KEY_RESERVED, KEY_RESERVED ,
	KEY_A, KEY_B, KEY_C, KEY_D ,KEY_E,
	KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
	KEY_K, KEY_L, KEY_M, KEY_N ,KEY_O,
	KEY_P, KEY_Q, KEY_R, KEY_S ,KEY_T,
	KEY_U, KEY_V, KEY_W, KEY_X ,KEY_Y,
	KEY_Z,
	KEY_RESERVED, KEY_RESERVED, KEY_RESERVED ,KEY_RESERVED
};
static char ix_key_to_askii[IX_KEY_PRESS_MAX]= {
	ASKII_RESERVED , ASKII_RESERVED,

	ASKII_1 ,ASKII_2 ,ASKII_3 ,ASKII_4,
	ASKII_5 ,ASKII_6 ,ASKII_7 ,ASKII_8 ,ASKII_9,ASKII_ZERO,		 // 1,2,3,4,5,6,7,8,9,0

	ASKII_MINUS, ASKII_EQUAL,									 // - =
	ASKII_RESERVED, ASKII_RESERVED,

	ASKII_Q,ASKII_W,ASKII_E,ASKII_R,ASKII_T,
	ASKII_Y,ASKII_U,ASKII_I,ASKII_O,ASKII_P,			// Q,W,E,R,T,Y,U,I,O,P

	ASKII_OPENSQURE, ASKII_CLOSESQUARE,					// [ ]
	ASKII_RESERVED, ASKII_RESERVED,

	ASKII_A,ASKII_S,ASKII_D,ASKII_F,ASKII_G,
	ASKII_H,ASKII_J,ASKII_K,ASKII_L,					// A,S,D,F,G,H,J,K,L

	ASKII_SEMICOLON,ASKII_SINGLEQUOTE,					// ; '
	ASKII_GRAVE, ASKII_RESERVED,ASKII_BACKSLASH,		// `  ASKII_RESERVED

	ASKII_Z ,ASKII_X ,ASKII_C ,ASKII_V ,ASKII_B,		// Z,X,C,V,B,N,M
	ASKII_N ,ASKII_M,

	ASKII_COMMA,ASKII_DOT,ASKII_FORDWARDSLASH			// , . /
};


static char ix_key_to_askii_with_shift[IX_KEY_PRESS_MAX]= {
	ASKII_RESERVED , ASKII_RESERVED,

	ASKII_AT, ASKII_HASH ,ASKII_DOLOR ,ASKII_PERCENT,
	ASKII_CARAT , ASKII_AMPERSAND, ASKII_STAR,ASKII_OPENPARAN,ASKII_CLOSEPARAN,ASKII_EXCLAM ,

	ASKII_UNDERBAR, ASKII_PLUS,
	ASKII_RESERVED, ASKII_RESERVED,

	ASKII_q,ASKII_w,ASKII_e,ASKII_r,ASKII_t,			// q, w, e, r, t
	ASKII_y,ASKII_u,ASKII_i,ASKII_o,ASKII_p,			// y, u, i, o ,p

	ASKII_CURLYOPEN, ASKII_CURLYCLOSE,					// { }
	ASKII_RESERVED, ASKII_RESERVED,

	ASKII_a,ASKII_s,ASKII_d,ASKII_f,ASKII_g,			// a, s, d, f, g
	ASKII_h,ASKII_j,ASKII_k,ASKII_l,					// h, j, k,l

	ASKII_COLON , ASKII_QUOTES,							// : "
	ASKII_TILDE , ASKII_RESERVED , ASKII_PIPE,			// ~ ASKII_RESERVED |

	ASKII_z ,ASKII_x ,ASKII_c ,ASKII_v ,ASKII_b,		// z, x, c, v, b
	ASKII_n ,ASKII_m,									// n, m,

	ASKII_OPENANGLE, ASKII_CLOSEANGLE, ASKII_QUESTION   // < > ?
};

//--------------------------------------------------------------
// �뜝�뙃�눦�삕
//--------------------------------------------------------------
static int ix_get_key_press_value(int *m_key, ix_btn_state_t button_state);


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 :
// �뜝�떊怨ㅼ삕 :
// �뜝�룞�삕�솚 : �뜝�룞�삕�뜝�룞�삕0, �뜝�룞�삕�뜝�룞�삕-1
//--------------------------------------------------------------
int ix_init( void )
{
	if( ix_isInit ) return 0;

	// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떗源띿삕�솕 ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕�뜝�룞�삕.
	ix_event_func = NULL;

	memset(ix_cal_touch_info, 0, sizeof(ix_cal_touch_info_t)*IX_CAL_TOUCH_INFO_MAX);
	memset(&ix_adjust_touch_info, 0, sizeof(ix_adjust_touch_info_t));

	gettimeofday( &ix_pre_time, NULL );
	ix_isInit = 1;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 :
//--------------------------------------------------------------
static char  *ix_error_string( void )
{

	char *error_string[] ={ "�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",                                    			//IXERR_NONE
							"ix_read_event_data_low �뜝�뙃�눦�삕 read �뜝�룞�삕�뜝�룞�삕 ",               	//IXERR_READ
							"ix_read_event_data_low �뜝�뙃�눦�삕 fd �뜝�룞�삕�뜝�룞�삕 ",						//IXERR_FD
							"ix_scan_device �뜝�뙃�눦�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",				//IXERR_DEV_INFO_OPEN
							"ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 ix_scan_device �뜝�룞�삕�뜝�룞�삕",			//IXERR_AUTOSCAN_SCANDEVICE
							"ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 mknod �뜝�룞�삕�뜝�룞�삕",					//IXERR_AUTOSCAN_MKNOD
							"ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",			//IXERR_AUTOSCAN_DEVICE_OPEN
							"ix_auto_scan_open �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕",			//IXERR_AUTOSCAN_EX_ADD
							"ix_write_event_low �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",			//IXERR_WRITE_EVENT
							"ix_write_mouse_button_action �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",	//IXERR_WRITE_MOUSE_BUTTON_EVENT
							"ix_write_keyboard_key_action �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥紐뚯삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",	//IXERR_WRITE_KEYBOARD_EVENT

                        };
	return( error_string[ix_error_code]);

	//printf( "\r[gx error:%d]%s %s\n", ex_error_code, ex_error_string(), msg);
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ixlib�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�뙓�벝�삕, �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�뙓�슱�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�
// �뜝�떊怨ㅼ삕 : �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣�눦�삕�뜝�룞�삕
//--------------------------------------------------------------
void ix_print_error( char *msg )
{
	printf( "\r[ix error:%d]%s %s\n", ix_error_code, ix_error_string(), msg);
	//return gx_error_code;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떙�궪�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕 mknod �뜝�룞�삕�뜝�룞�삕 ,�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 Open�뜝�떬�뙋�삕.
// �뜝�룞�삕�뜝�룞�삕 : http://forum.falinux.com/zbxe/?mid=Kernel_API&document_srl=405682&listStyle=&cpage=
// �뜝�룞�삕�뜝�룞�삕 : Thread �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�벝�뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕
// �뜝�룞�삕�솚 : �뜝�룞�삕�뜝�룞�삕 		: �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 , 0�뜝�룞�삕�뜝�룞�삕 �겕�뜝�떊�냲�삕 �뜝�룞�삕�뜝�룞�삕.
//		  �뜝�룞�삕�뜝�룞�삕 		: 0 �뜝�룞�삕�뜝�룞�삕 �뜝�뙗�뙋�삕.
// �뜝�떊怨ㅼ삕 : path 		: �뜝�뜦蹂� �뜝�룞�삕移�
// 		  major		: �뜝�룞�삕移섇뜝�룞�삕 major �뜝�룞�삕�샇
// 		  minor 	: �뜝�룞�삕移섇뜝�룞�삕 minor �뜝�룞�삕�샇
// 		  open_flag	: open �뜝���씛�삕 flag �뜝�떖�눦�삕
//--------------------------------------------------------------
int ux_mknod_device_open(char *path, char major, char minor , int open_flag)
{
	char 	file_name[PATH_MAX];			// 4096
	int		dev_fd =0;

	static 	int mknod_count =0;

	sprintf(file_name, "%s-%d-%d", path , getpid(), ++mknod_count ); // �뜝�뙥釉앹삕 �뜝�룞�삕�뜝�룞�삕

	remove(file_name);												// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뒗怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕. remove�뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 open�뜝�떦怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

	// mknod �뜝�떬�뙋�삕.
	mknod(file_name, (S_IRWXU|S_IRWXG|S_IFCHR), MKDEV(major , minor));

	dev_fd = open(file_name, open_flag);							// mknod �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 open �뜝�룞�삕 �뜝�룞�삕 fd�뜝�룞�삕 �뜝�룞�삕�뜝�듅�뙋�삕.
	if(dev_fd < 0)		return dev_fd;

	unlink(file_name);										   		// �뜝�룞�삕�뜝�떥�눦�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�뙓�벝�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 .. �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떎�뒗怨ㅼ삕 �뜝�떍�땲�뙋�삕.^^;
	return dev_fd;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�떩�뼲�삕 �뜝�룞�삕�뜝�듅怨ㅼ삕
// �뜝�룞�삕�뜝�룞�삕 : fd: �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 , event_data: �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
// �뜝�룞�삕�솚 : �뜝�룞�삕�뜝�룞�삕 :�뜝�떩�뼲�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕  , �뜝�룞�삕�뜝�룞�삕 : -1
//--------------------------------------------------------------
int ix_read_event_data_low(int fd, ix_event_t *event_data )
{
	int	read_size = 0;

	if(fd <0)
	{
		ix_error_code = IXERR_FD; 		// �뜝�룞�삕�뜝�룞�삕
		return -1;
	}

	read_size = read( fd, event_data, sizeof(ix_event_t));

	if(read_size != sizeof(ix_event_t))
	{
		ix_error_code = IXERR_READ; 	// �뜝�떩�뼲�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�떢怨ㅼ삕 �뜝�떆諛붾챿�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
		return -1;
	}

	return read_size;

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�떩�뼲�삕 �뜝�룞�삕�뜝�듅怨ㅼ삕
//--------------------------------------------------------------
void ix_debug_disp_event_data( ix_event_t *event_data )
{

	char *ix_code_rel_str[] = {
								"REL_X        ",
								"REL_Y        ",
								"REL_Z        ",
								"REL_RX       ",
								"REL_RY       ",
								"REL_RZ       ",
								"REL_HWHEEL   ",
								"REL_DIAL     ",
								"REL_WHEEL    ",
								"REL_MISC     ",
							};

	char *ix_code_abs_str[] = {
								"ABS_X         ",
								"ABS_Y         ",
								"ABS_Z         ",
								"ABS_RX        ",
								"ABS_RY        ",
								"ABS_RZ        ",
								"ABS_THROTTLE  ",
								"ABS_RUDDER    ",
								"ABS_WHEEL     ",
								"ABS_GAS       ",
								"ABS_BRAKE     ",
								"ABS_HAT0X     ",
								"ABS_HAT0Y     ",
								"ABS_HAT1X     ",
								"ABS_HAT1Y     ",
								"ABS_HAT2X     ",
								"ABS_HAT2Y     ",
								"ABS_HAT3X     ",
								"ABS_HAT3Y     ",
								"ABS_PRESSURE  ",
								"ABS_DISTANCE  ",
								"ABS_TILT_X    ",
								"ABS_TILT_Y    ",
								"ABS_TOOL_WIDTH",
							};

	//printf("[Sec]%ld, [uSec]%ld ",event_data->time.tv_sec, event_data->time.tv_usec );

	switch(event_data->type)
	{
    case    EV_SYN :  	printf("EV_SYN  ");		break;
	case    EV_KEY :  	printf("EV_KEY  ");		break;
	case    EV_ABS :    printf("EV_ABS  ");		break;      // �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 �뜝�뙃琉꾩삕
	case    EV_REL :    printf("EV_ABS  ");		break;

    default : 	printf("[type]:%02x! " ,event_data->type); 			break;
	}

	switch(event_data->type)
	{
 	case    EV_ABS :    // �뜝�룞�삕�뜝�럥媛�
 		printf("[code:%d]%s , [value]:%d  \n",
 					event_data->code ,
 					ix_code_abs_str[event_data->code],
 					event_data->value
 			   );
 		break;

	case    EV_REL :   	// �뜝�룞�삕獄쏉옙
 		printf("[code:%d]%s , [value]:%d  \n",
 					event_data->code ,
 					ix_code_rel_str[event_data->code],
 					event_data->value
 			   );
		break;

    default : 	printf("[code]:%d ,[value]:%d \n" ,event_data->code ,event_data->value );
   		break;
	}

}
//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ix_scan_device �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �솕�뜝�띂�뿉 �몴�뜝�룞�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
void ix_debug_disp_ix_device_info( void )
{
	int lp=0;
	printf("=========================================\n");
	printf("==     input device infomation         ==\n");
	printf("=========================================\n");

	for(lp = 0 ; lp < ix_mng_count ; lp++)
	{
		printf(" [device name ]:%s  \n" ,ix_mng[lp].name );
		printf(" [input number]:%d  \n" ,ix_mng[lp].input_number );
		printf(" [event number]:%d  \n" ,ix_mng[lp].event_number );
		printf(" [event major ]:%d  \n" ,ix_mng[lp].dev_major );
		printf(" [event minor ]:%d  \n" ,ix_mng[lp].dev_minor );
		printf("-----------------------------------------------\n");
	}

	if(!ix_mng_count )
		printf("no input device scan data!!!\n");

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
// down : l
//--------------------------------------------------------------
int ix_read_keyboard_low(int fd , int *key, int *key_state )
{
	ix_event_t	 event_data;

	*key 		= KEY_RESERVED ;				// �뜝�떗源띿삕�솕
	*key_state 	= 0;							// �뜝�떗源띿삕�솕

	while(1)
	{
		if(ix_read_event_data_low(fd , &event_data ))	return -1;

		//ix_debug_disp_event_data(&event_data );

		if(event_data.type == EV_KEY)
		{
			*key 		= event_data.code;
			*key_state	= event_data.value;
		}
		if(event_data.type == EV_SYN)			break;			// syn�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
	}

	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
//        �궎�뜝�룞�삕�뜝�뜴瑜� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
//        �궎�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢怨ㅼ삕 �뜝�떥諭꾩삕 �뜝�듅�뙋�삕.
//[Sec]665, [uSec]902491 EV_KEY  [code]:2 ,[value]:1       //value�뜝�룞�삕 1�뜝�떛紐뚯삕 down
//[Sec]665, [uSec]902604 EV_SYN  [code]:0 ,[value]:0
// �뜝�룞�삕�뜝�룞�삕  key �뜝�룞�삕 2�뜝�룞�삕 syn �뜝�떖�씛�삕 �뜝�룞�삕�뜝�뙎源띿삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
int ix_read_key_press_low(int fd , int *key )
{
	int 	key_state =0;					// syn�뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	ix_event_t	 event_data;

	*key 	= KEY_RESERVED ;				// �뜝�떗源띿삕�솕


	while(1)
	{
		if(ix_read_event_data_low(fd , &event_data ))	return -1;

		if(event_data.type == EV_KEY)
		{
			*key 		= event_data.code;
			key_state	= event_data.value;
		}

		if(	 (event_data.type	== EV_SYN			)
		   &&(key_state			== IX_KEY_VAULE_DOWN_LOW))
		{
			break;			// �궎�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕 syn�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
		}
	}
	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : mouse �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
// �뜝�룞�삕�뜝�룞�삕 :
//		*x	: REL_X �뜝�룞�삕
//		*y	: REL_Y �뜝�룞�삕
//		*btn: code �뜝�룞�삕�뜝�룞�삕 �뜝�떛�슱�삕�뜝�떦�슱�삕 �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�듉 �뜝�룞�삕移섇뜝�룞�삕 �뜝�떬怨ㅼ삕�뜝�뙏�뙋�삕.
//			  �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�떆�슱�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듃 �뜝�룞�삕�뜝�룞�삕�겕 �뜝�떦�슱�삕 �뜝�룞�삕�뜝�떆�슱�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�듅紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//
//  [�뜝�룞�삕�뜝�룞�삕1. �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�떗諭꾩삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕, �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�]
//	type = 	EV_KEY, code = BTN_TOUCH,	value = 1    <- �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//	type = 	EV_ABS, code = ABS_X,		value = -14  <- X �뜝�룞�삕�몴�뜝�룞�삕�뜝�룞�삕 -14�뜝�떛�뙋�삕.
//	type = 	EV_ABS, code = ABS_Y,		value = 99   <- Y �뜝�룞�삕�몴�뜝�룞�삕�뜝�룞�삕 +99�뜝�떛�뙋�삕.
//	type = 	EV_SYN, code = 0,			value = 1    <- �뜝�룞�삕�뜝�뙠源띿삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢怨ㅼ삕 �뜝�떦�냲�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�떛�뙋�삕.
//
//	�뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�듅�슱�삕�뜝�룞�삕 , �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떛�뙋�삕 �뜝�룞�삕�뜝�룞�삕 �솗�뜝�룞�삕 �뜝�떦琉꾩삕�뜝�룞�삕,
//--------------------------------------------------------------
int ix_read_touch_low(int fd , int *x, int *y, int *press )
{

	ix_event_t	 event_data;

	while(1)
	{
		if(ix_read_event_data_low(fd , &event_data ))	return -1;

		//ix_debug_disp_event_data(&event_data );

		if( (event_data.type	== EV_KEY)
		   &&(event_data.code	== BTN_TOUCH))
		{
			*press = event_data.value;
		}

		if( event_data.type	== EV_ABS)
		{
			if(event_data.code == ABS_X)	*x = event_data.value;

			if(event_data.code == ABS_Y)	*y = event_data.value;
		}

		if(event_data.type == EV_SYN)			break;			// syn�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
	}
	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : mouse �뜝�룞�삕�뜝�룞�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
// �뜝�룞�삕�뜝�룞�삕 :
//		*x	: REL_X �뜝�룞�삕
//		*y	: REL_Y �뜝�룞�삕
//		*btn: code �뜝�룞�삕�뜝�룞�삕 �뜝�떛�슱�삕�뜝�떦�슱�삕 �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�듉 �뜝�룞�삕移섇뜝�룞�삕 �뜝�떬怨ㅼ삕�뜝�뙏�뙋�삕.
//			  �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�떆�슱�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듃 �뜝�룞�삕�뜝�룞�삕�겕 �뜝�떦�슱�삕 �뜝�룞�삕�뜝�떆�슱�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�듅紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//
//  [�뜝�룞�삕�뜝�룞�삕1. �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�떗諭꾩삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕, �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�]
//	type = 	EV_KEY, code = BTN_LEFT,	value = 1    <- �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//	type = 	EV_REL, code = REL_X,		value = -14  <- X �뜝�룞�삕�몴�뜝�룞�삕�뜝�룞�삕 -14�뜝�떛�뙋�삕.
//	type = 	EV_REL, code = REL_Y,		value = 99   <- Y �뜝�룞�삕�몴�뜝�룞�삕�뜝�룞�삕 +99�뜝�떛�뙋�삕.
//	type = 	EV_SYN, code = 0,			value = 1    <- �뜝�룞�삕�뜝�뙠源띿삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢怨ㅼ삕 �뜝�떦�냲�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�떛�뙋�삕.
//
//	�뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�듅�슱�삕�뜝�룞�삕 , �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떛�뙋�삕 �뜝�룞�삕�뜝�룞�삕 �솗�뜝�룞�삕 �뜝�떦琉꾩삕�뜝�룞�삕,
//--------------------------------------------------------------
int ix_read_mouse_low(int fd , int *x, int *y, int *btn)
{
	ix_event_t	 event_data;

	while(1)
	{
		if(ix_read_event_data_low(fd , &event_data ))	return -1;

		ix_debug_disp_event_data(&event_data );

		if( event_data.type	== EV_KEY)
		{
			switch(event_data.code)
			{
			case BTN_LEFT	:
					if(event_data.value == 1)	*btn |= IX_MOUSE_BTN_LEFT;		// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
					else						*btn &= ~IX_MOUSE_BTN_LEFT;		// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
				 break;

			case BTN_RIGHT	:
					if(event_data.value == 1)	*btn |= IX_MOUSE_BTN_RIGHT;		// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
					else						*btn &= ~IX_MOUSE_BTN_RIGHT;	// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
				 break;

			case BTN_MIDDLE :
					if(event_data.value == 1)	*btn |= IX_MOUSE_BTN_MIDDLE;	// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룜�뜲 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
					else						*btn &= ~IX_MOUSE_BTN_MIDDLE;	// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룜�뜲 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
				 break;
			}
		}

		if( event_data.type	== EV_REL)
		{
			if(event_data.code == REL_X)	*x = event_data.value;				// x �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

			if(event_data.code == REL_Y)	*y = event_data.value;				// y �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		}

		if(event_data.type == EV_SYN)			break;			// syn�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
	}
	return 0;
}



//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕黎뷴뜝�룞�삕�뜝占� 李얍뜝�떍�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
// �뜝�룞�삕�뜝�룞�삕 : imesu 2009�뜝�룞�삕 2�뜝�룞�삕 13�뜝�룞�삕
//         cat /proc/bus/input/devices �뜝�룞�삕�뜝�룞�삕 input�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룜�룄
//         event�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뙋�삕 �뜝�룞�삕筌∽옙 �뜝�뙇�뙋�삕.
//         �뜝�떛怨ㅼ삕�뜝占� S: Sysfs=/class/input/input �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕永밧뜝占� �뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�듅�벝�삕
//         �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 /proc/bus/input/devices �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕 �뜝�뙇源띿삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�슱�삕�뜝�룞�삕.
//         input �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�뙐紐뚯삕  -1�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�슱�삕�뜝�룞�삕.
//--------------------------------------------------------------
int ix_scan_device(void)
{
	FILE *fp;
	char read_buf[1024];
	char filename_buf[PATH_MAX];			// 4096

	char *name_ptr = NULL;
	char *tmp_ptr = NULL;

	int tmp_num1=0;
	int tmp_num2=0;

	int lp=0;

	char *find_str;

	fp = fopen(FILE_NAME_INPUT_INFO_BASE, "r");
	if(fp == NULL)
	{
		ix_error_code = IXERR_DEV_INFO_OPEN;
		return -1;
	}

	ix_mng_count = 0;		// �뜝�떗源띿삕�솕

	memset(ix_mng , 0, sizeof(ix_mng_t)*IX_INPUT_DEVICE_MAX);  // �뜝�떗源띿삕�솕


	while(!feof(fp))
	{
		if(ix_mng_count >= IX_INPUT_DEVICE_MAX) 	break;

		memset(read_buf, 0, sizeof(read_buf));

		fgets(read_buf, sizeof(read_buf)-1, fp );			// �뜝�룞�삕 �뜝�뙐�뼲�삕 �뜝�떩�뼲�삕夷덂뜝占�.
		//printf("[line data]%s" ,read_buf );
		if(read_buf[0] == 'S')			//	�뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�샇�뜝�룞�삕 �뜝�룞�삕罹붷뜝�떬�뙋�삕.
		{
			sscanf(read_buf, "S: Sysfs=/class/input/input%d" ,&tmp_num1);
			ix_mng[ix_mng_count].input_number = tmp_num1;
		}

		if(read_buf[0] == 'H')			//	�뜝�떛釉앹삕�듃  �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�샇�뜝�룞�삕 �뜝�룞�삕罹붷뜝�떬�뙋�삕.
		{
			find_str = strstr(read_buf, "event");			// �뜝�룞�삕�뜝�뙓�슱�삕�뜝�룞�삕 "event" �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떙�궪�삕�뜝�떦怨ㅼ삕 , �뜝�뙇�떎紐뚯삕 �뜝�떥釉앹삕 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�떬怨ㅼ삕�뜝�뙏�뙋�삕.
			if(find_str !=NULL)
			{
				sscanf(find_str, "event%d" ,&tmp_num1 );
				ix_mng[ix_mng_count].event_number = tmp_num1;
			}
			else //imesu 2�뜝�룞�삕 13�뜝�룞�삕 �뜝�뙥怨ㅼ삕
			{
				ix_mng[ix_mng_count].event_number = -1;
			}
		}

		if(read_buf[0] == 'N')							// imesu 2�뜝�룞�삕 13�뜝�룞�삕 �뜝�뙥怨ㅼ삕 , �뜝�떍琉꾩삕 �뜝�뙇�눦�삕泥섇뜝�룞�삕�뜝�룞�삕 �뜝�떥釉앹삕�뜝�룞�삕 �뜝�룞�삕泥닷뜝�떦�뙋�삕 �뜝�떥釉앹삕
		{
			name_ptr = strchr(read_buf,'=');
			name_ptr++;  								// = �뜝�룞�삕 �뜝�떗�슱�삕 �뜝�룞�삕�뜝�룜�꽌 �뜝�룞�삕�뜝�룞�삕
			tmp_ptr = strchr(read_buf,'\n');
			//printf(" %s \n",name_ptr);

			strncpy(ix_mng[ix_mng_count].name , name_ptr , tmp_ptr - name_ptr);		//
		}

		if(	read_buf[0] ==0x0a)	ix_mng_count++;	          // 0x0a �뜝�룞�삕 LF �뜝�룞�삕 '\r'
	}
	fclose(fp);

	// �뜝�뙃琉꾩삕 �뜝�룞�삕移� major , minor �뜝�룞�삕�샇�뜝�룞�삕 �뜝�룞�삕夷덂뜝占�.
	for(lp = 0 ; lp < ix_mng_count ; lp++)
	{
		sprintf(filename_buf,"/sys/class/input/event%d/dev" ,ix_mng[lp].event_number );

		fp = fopen(filename_buf, "r");
		if(fp !=NULL)
		{
			fscanf(fp , "%d:%d" ,&tmp_num1 ,&tmp_num2 );

			ix_mng[lp].dev_major = tmp_num1;				// major �뜝�룞�삕�샇 �뜝�룞�삕�뜝�룞�삕
			ix_mng[lp].dev_minor = tmp_num2;				// minor �뜝�룞�삕�샇 �뜝�룞�삕�뜝�룞�삕
			fclose(fp);
		}

		// �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�떥怨ㅼ삕�뜝�룞�삕 �솗�뜝�룞�삕�뜝�떬�뙋�삕.
		sprintf(filename_buf,"/sys/class/input/input%d/capabilities/abs" ,ix_mng[lp].input_number );

		fp = fopen(filename_buf, "r");
		if(fp !=NULL)
		{
			fscanf(fp , "%d" ,&tmp_num1  );

//			printf( "FILE %s [%d]\n", filename_buf, tmp_num1 );

			if( tmp_num1 >= 3 ) ix_mng[lp].is_touch = 1;			// �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�떛�뙋�삕.
			else                ix_mng[lp].is_touch = 0;			// �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�떍�땲�뙋�삕.

			fclose(fp);
		}

	}

	// �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣�룞�삕�뜝�룞�삕
#if 0
	for(lp = 0 ; lp < ix_mng_count ; lp++)
	{
		printf(" --------------------------------------\n");
		printf(" [device name ]:%s  \n" ,ix_mng[lp].name );
		printf(" [input number]:%d  \n" ,ix_mng[lp].input_number );
		printf(" [event number]:%d  \n" ,ix_mng[lp].event_number );
		printf(" [event major ]:%d  \n" ,ix_mng[lp].dev_major );
		printf(" [event minor ]:%d  \n" ,ix_mng[lp].dev_minor );
		printf(" --------------------------------------\n");
	}
	printf("input device scan complete!!!!\n");
#endif

	return 0;
}



//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
static int ix_copy_to_user()
{
	ie_event_t 		ie_data={0,};							// input event data;
	int 			mx, my;
	int 			mkey;								// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�뙃�젰怨ㅼ삕

	ix_get_mouse_position(&mx , &my);					// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�몴�뜝�룞�삕�뜝�룞�삕 �뜝�떬怨ㅼ삕�뜝�뙏�뙋�삕.

//add by smshin for NVS3260 Mouse
{
	int x = mx;
	int y = my;

#if 1
	mx = x; // make even number using shift.
	my = y;
#else
	if( ReadSystemVideoOutPath() )
	{
		//CVBS Out
		mx = ((x>>1)<<1)>>1; // make even number using shift.
		my = y>>1;
	}
	else
	{
		//VGA Out
		mx = ((((720*x)/1024)>>1)<<1)>>1; // make even number using shift.
		my = ((((ReadSystemVideoFormat())?576:480)*y)/768)>>1;
	}
#endif
	SetMousePoint( x, y );
}
	ix_get_key_value(&mkey);							// keyboard �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떬怨ㅼ삕�뜝�뙏�뙋�삕.

	if( ix_mouse_event_flag & IX_MOUSE_FLAG_BTN_DOWN )
	{
		//printf("mouse down\n");
		ix_mouse_event_flag 	&= ~IX_MOUSE_FLAG_BTN_DOWN;
		ie_data.data.mouse.x 	= mx;							// x �뜝�룞�삕�몴�뜝�룞�삕
		ie_data.data.mouse.y 	= my;							// y �뜝�룞�삕�몴�뜝�룞�삕

//		printf("mx:%d, my:%d ",mx, my);	;
		ie_data.data.mouse.state= ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		ie_data.type 		 	= IE_MOUSE_DOWN;
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_mouse_event_flag & IX_MOUSE_FLAG_MOVE )
	{
		//printf("mouse move\n");
		ix_mouse_event_flag 	&= ~IX_MOUSE_FLAG_MOVE;
		ie_data.data.mouse.x	 = mx;
		ie_data.data.mouse.y	 = my;
		ie_data.data.mouse.state = ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		ie_data.type 			 = IE_MOUSE_MOVE;
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_mouse_event_flag & IX_MOUSE_FLAG_BTN_UP )
	{
		//printf("mouse up\n");
		ix_mouse_event_flag 	&= ~IX_MOUSE_FLAG_BTN_UP;
		ie_data.data.mouse.x	 = mx;
		ie_data.data.mouse.y	 = my;
		ie_data.data.mouse.state = ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		ie_data.type 			 = IE_MOUSE_UP;
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_mouse_event_flag & IX_MOUSE_FLAG_WHEEL )
	{
//		printf("mouse wheel\n");
		ix_mouse_event_flag 	&= ~IX_MOUSE_FLAG_WHEEL;
		ie_data.data.mouse.x 	= mx;							// x �뜝�룞�삕�몴�뜝�룞�삕
		ie_data.data.mouse.y 	= my;							// y �뜝�룞�삕�몴�뜝�룞�삕

//		printf("mx:%d, my:%d ",mx, my);
		ie_data.data.mouse.state = ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		ie_data.data.mouse.wheel = ix_mouse_wheel;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		ie_data.type 		 	 = IE_MOUSE_WHEEL;
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_key_event_flag & IX_KEY_VAULE_DOWN )					// key �뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕.
	{
		//printf("IX_KEY_VAULE_DOWN  \n");
		ix_key_event_flag		&= 	~IX_KEY_VAULE_DOWN;
		ie_data.data.key.key	 = 	mkey;
		ie_data.type 			 = 	IE_KEY_DOWN;
		ie_data.data.key.state	 =	ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_key_event_flag & IX_KEY_VAULE_UP )						// key �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떎�뼲�삕�뜝�룞�삕.
	{
		//printf("IX_KEY_VAULE_UP  \n");
		ix_key_event_flag    	&= 	~IX_KEY_VAULE_UP;
		ie_data.data.key.key	= 	mkey;
		ie_data.type 			= 	IE_KEY_UP;
		ie_data.data.key.state  = 	ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	if( ix_key_event_flag & IX_KEY_VAULE_PRESS )					// key �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕孃��뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떙�눦�삕 �뜝�뙇�뙋�삕 �뜝�떍�룞�삕�궎 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚
	{
		ix_get_key_press_value(&mkey, ix_button_state);

		//printf("IX_KEY_VAULE_PRESS %d \n", mkey);
		ix_key_event_flag   	&= 	~IX_KEY_VAULE_PRESS;
		ie_data.data.key.key	= 	mkey;
		ie_data.type 			= 	IE_KEY_PRESS;
		ie_data.data.key.state  =	ix_button_state;				// �뜝�룞�삕�뜝�듅怨ㅼ삕
		if(ix_event_func != NULL)
			ix_event_func(&ie_data);
	}

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �겢�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
// �뜝�룞�삕�뜝�룞�삕 : msec : �뜝�룞�삕�뜝�럩�뒪 �뜝�뙃琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떆怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕, �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕  msec�뜝�떛�뙋�삕.
//--------------------------------------------------------------
int  ix_set_mouse_double_click_intaval(int msec)
{
	ix_mouse_click_interval = msec;

	return ix_mouse_click_interval;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �겢�뜝�룞�삕�뜝�룞�삕 �솗�뜝�룞�삕�뜝�떬�뙋�삕.
// �뜝�룞�삕�솚 : 1  :  �뜝�룞�삕�뜝�룞�삕�겢�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 ,
//        0 :  �뜝�룞�삕�뜝�룞�삕�겢�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
static int  ix_check_mouse_double_click( struct timeval cur_time)
{
/*
	int intaval=0;

	// �뜝�떆怨ㅼ삕 �솗�뜝�룞�삕
	intaval = (cur_time.tv_usec - ix_pre_time.tv_usec);

	printf("cur :%ld, old :%ld, intaval :%d \n", cur_time.tv_usec,ix_pre_time.tv_usec, intaval );
	if(intaval < ix_mouse_click_interval)
	{	// �뜝�떗源띿삕�솕
		ix_mouse_double_click_flag++;
		if(ix_mouse_double_click_flag ==2 )
		{
			printf(">>>>>>> Double \n");
			ix_mouse_double_click_flag =0;
			ix_pre_time  = cur_time;
			return 1;
		}
	}
	else
	{
		//ix_mouse_double_click_flag=1;
		//ix_pre_time  = cur_time;
		return 0;
	}
*/
	return 0;

}



//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : input event tye key �뜝�뙃琉꾩삕 泥섇뜝�룞�삕
//
//--------------------------------------------------------------
static int ix_input_event_type_key(ix_event_t event_data)
{
	//printf("[code] 0x%x\n",event_data.code );
	switch(event_data.code)
	{
	case BTN_TOUCH  :					// touh �뜝�뙃琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 left�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕�뜝�떛�뙋�삕.
	case BTN_LEFT	:
			if(event_data.value)	ix_button_state |= IX_MOUSE_BTN_LEFT;			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_MOUSE_BTN_LEFT;			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case BTN_RIGHT	:
			if(event_data.value)	ix_button_state |= IX_MOUSE_BTN_RIGHT;			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_MOUSE_BTN_RIGHT;			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case BTN_MIDDLE :
			if(event_data.value)	ix_button_state |= IX_MOUSE_BTN_MIDDLE;			// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룜�뜲 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_MOUSE_BTN_MIDDLE;		// �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕�뜝�룜�뜲 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_LEFTSHIFT :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_LEFT_SHIFT;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 shitf �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_LEFT_SHIFT;	// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 shitf �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case KEY_RIGHTSHIFT   :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_RIGHT_SHIFT;	// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 shitf �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_RIGHT_SHIFT;	// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 shitf �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case KEY_LEFTALT    :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_LEFT_ALT;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 ALT �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_LEFT_ALT;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 ALT �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case KEY_RIGHTALT    :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_RIGHT_ALT;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 ALT �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_RIGHT_ALT;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 ALT �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		 	break;

	case KEY_LEFTCTRL     :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_LEFT_CTRL;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_LEFT_CTRL;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_RIGHTCTRL     :
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_BTN_RIGHT_CTRL;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_BTN_RIGHT_CTRL;	// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_CAPSLOCK     :	// imesu 090215
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_CAPSLOCK;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_CAPSLOCK;		// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 CTRL �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;


	case KEY_NUMLOCK     :	// imesu 090215
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_NUMLOCK;			// �궎�뜝�룞�삕�뜝�룞�삕 Num Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_NUMLOCK;		// �궎�뜝�룞�삕�뜝�룞�삕 Num Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_UP     :		// imesu 090215
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_UP;				// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_UP;				// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_DOWN   :		// imesu 090215
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_DOWN;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�떍琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_DOWN;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�떍琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_LEFT    :		// imesu 090215  �뜝�룞�삕�뜝�룞�삕�궎
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_LEFT;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_LEFT;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_RIGHT    :		// imesu 090215
			if(event_data.value)	ix_button_state |= IX_KEYBOARD_RIGHT;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕
			else					ix_button_state &= ~IX_KEYBOARD_RIGHT;			// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�궎 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	default : // �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �궎�뜝�룞�삕�뜝�룞�삕�뜝占� 泥섇뜝�룞�삕�뜝�뙥�뙋�삕. �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떥釉앹삕
		  //if(event_data.value == 1)	ix_button_state= event_data.value;				// key�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뒗�뙋�삕.
		  	break;
	}

	// �뜝�룞�삕�뜝�룞�삕 �겢�뜝�룞�삕 �솗�뜝�룞�삕
	switch(event_data.code)
	{
	case BTN_TOUCH  :					// touh �뜝�뙃琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 left�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕�뜝�떛�뙋�삕.
	case BTN_LEFT	:
			if(ix_check_mouse_double_click(event_data.time))
			{
				if(event_data.value)	ix_mouse_event_flag |=IX_MOUSE_FLAG_DOUBLE_CLICK;	//�뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕.
				else					ix_mouse_event_flag &=~IX_MOUSE_FLAG_DOUBLE_CLICK;		// �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			}
			break;

	default :

			break;
	}

	// FLAG�뜝�룞�삕�뜝�룞�삕
	switch(event_data.code)
	{
	case BTN_TOUCH  :					// touh �뜝�뙃琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 left�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕�뜝�떛�뙋�삕.
	case BTN_LEFT	:
	case BTN_RIGHT	:
	case BTN_MIDDLE :
			if(event_data.value)	ix_mouse_event_flag |=IX_MOUSE_FLAG_BTN_DOWN;	//�뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕.
			else					ix_mouse_event_flag |=IX_MOUSE_FLAG_BTN_UP;		// �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
			break;

	case KEY_CAPSLOCK     :				// imesu 090215 �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 DOWN -> UP�뜝�룞�삕 �뜝�떕�뙋�삕. �뜝�룞�삕 2�뜝�룞�삕ix_input_event_type_key()�뜝�룞�삕 �샇�뜝�룞�삕�솼�뜝占�. �뜝�뙎琉꾩삕�뜝�룞�삕 DOWN �뜝�떖�씛�삕 FLAG �뜝�룞�삕�뜝�룞�삕�뜝�떦怨ㅼ삕 �뜝�떬�뙋�삕.
			if(event_data.value)
			{
				ix_key_event_flag |= IX_KEY_VAULE_DOWN;								// �궎�뜝�룞�삕�뜝�룞�삕 Num Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
				ix_key_event_flag |= IX_KEY_VAULE_PRESS;							// key�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕. low code �뜝�룞�삕�뜝�룞�삕 ascii�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚 ex) KEY_1 -> 0x30

				if(!ix_capslock_flag)		ix_capslock_flag = FLAG_SET;			// CapsLock �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떎�뼲�삕�뜝�룞�삕.
				else 						ix_capslock_flag = FLAG_RELEASE;		// CapsLock �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떎�뼲�삕�뜝�룞�삕.
			}
			else
			{
				ix_key_event_flag |= IX_KEY_VAULE_UP;
			}

			break;


	case KEY_NUMLOCK     :	// imesu 090215
			if(event_data.value)
			{
				ix_key_event_flag 		|= IX_KEY_VAULE_DOWN;						// �궎�뜝�룞�삕�뜝�룞�삕 Num Lock �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
				ix_key_event_flag 		|= IX_KEY_VAULE_PRESS;						// key�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕. low code �뜝�룞�삕�뜝�룞�삕 ascii�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚 ex) KEY_1 -> 0x30

				if(!ix_numlock_flag) 	ix_numlock_flag = FLAG_SET;					// CapsLock �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떎�뼲�삕�뜝�룞�삕.
				else 					ix_numlock_flag = FLAG_RELEASE;				// CapsLock �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떎�뼲�삕�뜝�룞�삕.
			}
			else
			{
				ix_key_event_flag |= IX_KEY_VAULE_UP;
			}
			break;


	default : // �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �궎�뜝�룞�삕�뜝�룞�삕�뜝占� 泥섇뜝�룞�삕�뜝�뙥�뙋�삕. �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떥釉앹삕
			ix_key = event_data.code;								// key �뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뒗�뙋�삕.

		  	if(event_data.value )
		  	{
		  		ix_key_event_flag |= IX_KEY_VAULE_DOWN;				// key�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕. low code �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
		  		ix_key_event_flag |= IX_KEY_VAULE_PRESS;			// key�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕. low code �뜝�룞�삕�뜝�룞�삕 ascii�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚 ex) KEY_1 -> 0x30
		  	}
		  	else
		  	{
		  		ix_key_event_flag |= IX_KEY_VAULE_UP;				// key�뜝�룞�삕 �뜝�룞�삕�뜝�떕�뙋�삕.
			}
			break;
	}
	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 :REL �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕 泥섇뜝�룞�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
static int ix_input_event_type_rel(ix_event_t event_data)
{
	if(event_data.code == REL_X)
	{
		ix_mouse_x += event_data.value;							// x �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

		if(ix_mouse_x < ix_sreen_x_min )	ix_mouse_x = ix_sreen_x_min;
		if(ix_mouse_x >= ix_sreen_x_max )	ix_mouse_x = ix_sreen_x_max-1;
		ix_mouse_event_flag |=IX_MOUSE_FLAG_MOVE;
	}
	if(event_data.code == REL_Y)
	{
		ix_mouse_y += event_data.value;							// y �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

		if(ix_mouse_y < ix_sreen_y_min )	ix_mouse_y = ix_sreen_y_min;
		if(ix_mouse_y >= ix_sreen_y_max )	ix_mouse_y = ix_sreen_y_max-1;
		ix_mouse_event_flag |=IX_MOUSE_FLAG_MOVE;
	}
	if(event_data.code == REL_WHEEL )
	{
		ix_mouse_wheel = event_data.value;
		ix_mouse_event_flag |= IX_MOUSE_FLAG_WHEEL;
	}
	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ABS �뜝�룞�삕 泥섇뜝�룞�삕
//--------------------------------------------------------------
static int ix_input_event_type_abs(ix_event_t event_data)
{

	int x, y;
	if(event_data.code == ABS_X)
	{
		ix_mouse_x = event_data.value;							// x �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

		if(ix_adjust_touch_flag)
		{
			x = (ix_adjust_touch_info.xa * ix_mouse_x + ix_adjust_touch_info.xb) / IX_CAL_TOUCH_INFO_OFSSET;

			if(x <= 0)
			{
				x =0;
			}
			else if(x >= ix_sreen_x_max)
			{
				x = ix_sreen_x_max;
			}

			ix_mouse_x = x;
		}

	}
	if(event_data.code == ABS_Y)
	{
		ix_mouse_y = event_data.value;							// y �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

		if(ix_adjust_touch_flag)
		{
			y = (ix_adjust_touch_info.ya * ix_mouse_y + ix_adjust_touch_info.yb) / IX_CAL_TOUCH_INFO_OFSSET;

			if(y <= 0)
			{
				y =0;
			}
			else if(y >= ix_sreen_y_max)
			{
				y = ix_sreen_y_max;
			}

			ix_mouse_y = y;
		}
	}



	ix_mouse_event_flag |= IX_MOUSE_FLAG_MOVE;

	//printf("mx:%d, my:%d ",ix_mouse_y, ix_mouse_y);	TRACE;

	return 0;

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�뙃琉꾩삕�뜝�룞�삕 泥섇뜝�룞�삕�뜝�떬�뙋�삕.
//		*on_self �뜝�뙃�젰�벝�삕�뜝�룞�삕 �뜝�뙃琉꾩삕�뜝�룞�삕 泥섇뜝�룞�삕�뜝�떬�뙋�삕.
//       �뜝�룞�삕移섇뜝�뙃琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�럩�뒪�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙃�젰�뙋�삕.
//--------------------------------------------------------------
static int ix_event_read(void *on_self )
{
	ex_mng_t		*mng      = NULL;

	ix_event_t 		event_data;

	mng = (ex_mng_t *)on_self;

	ix_read_event_data_low(mng->fd , &event_data);

	//ix_debug_disp_event_data(&event_data);						// �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕 �솗�뜝�룞�삕�뜝�떬�뙋�삕.

	switch(event_data.type)
	{
	case EV_KEY :	ix_input_event_type_key(event_data);	break;

	case EV_REL :	ix_input_event_type_rel(event_data);	break;

	case EV_ABS :	ix_input_event_type_abs(event_data);	break;

	case EV_SYN :	ix_copy_to_user();		break;				// �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.

	default : break;
	}

	return 0;

}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떙�궪�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕 mknod �뜝�떬�뙋�삕.
// 			1)�뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�뙓�벝�삕 �뜝�떙�궪�삕
// 			2) mknod
// 			3) �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕..
// 			4) ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占�
// �뜝�떊怨ㅼ삕 : path : �뜝�뜦蹂� �뜝�룞�삕�뜝�뜲由� �뜝�룞�삕移�
// �뜝�룞�삕�솚 :
// �뜝�룞�삕�뜝�룞�삕 :
//--------------------------------------------------------------
int	ix_auto_scan_open( char *path )
{
	char 	device_path[PATH_MAX];							// 4096
	int		lp=0;


	// close all previous device
	for( ;lp<ix_mng_count;lp++)
	{
		if( ix_mng[lp].fd > 0 )	close( ix_mng[lp].fd );
	}

	// 1)�뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�뙓�벝�삕 �뜝�떙�궪�삕
	if(ix_scan_device() <0)
	{
		ix_error_code = IXERR_AUTOSCAN_SCANDEVICE;
		return -1;
	}

	// 2) mknod
	// 3) �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕..
	for( lp= 0 ; lp < ix_mng_count;lp++)
	{
		sprintf(device_path, "%s/event%d", path, ix_mng[lp].event_number);
		ix_mng[lp].fd = ux_mknod_device_open(device_path ,ix_mng[lp].dev_major ,ix_mng[lp].dev_minor, O_RDWR );

		if(ix_mng[lp].fd < 0)	continue;

		//DBG_INFO("%s",device_path);
		// 4) ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占�
		input_obj[lp] 			= ex_add_fd( ix_mng[lp].fd );

		if(input_obj[lp] == NULL) continue;

		input_obj[lp]->on_read  = ix_event_read;			// read
	}

	ex_adjust_poll();

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�떢紐뚯삕 �뜝�룞�삕夷덂뜝占�.
//        �뜝�뙓�벝�삕�뜝�룞�삕�뜝�룞�삕 mknod �뜝�떬�뙋�삕.
// 			1) �뜝�뙃琉꾩삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕移� �뜝�떙�궪�삕
// 			2) mknod
// 			3) �뜝�룞�삕�뜝�룞�삕黎뷴뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕..
// 			4) ex �뜝�뙃�눦�삕 �뜝�룞�삕�뜝占�
// �뜝�떊怨ㅼ삕 : path : �뜝�뜦蹂� �뜝�룞�삕�뜝�뜲由� �뜝�룞�삕移�
// �뜝�룞�삕�솚 :
// �뜝�룞�삕�뜝�룞�삕 :
//--------------------------------------------------------------
int ix_get_touch_fd( char *path )
{
	char 	device_path[PATH_MAX];							// 4096
	int lp;

	for( lp= 0 ; lp < ix_mng_count;lp++)
	{
		if( !ix_mng[lp].is_touch ) continue;

		printf( "INDEX %d\n" , lp );

		sprintf(device_path, "%s/event%d", path, ix_mng[lp].event_number);
		ix_mng[lp].fd = ux_mknod_device_open(device_path ,ix_mng[lp].dev_major ,ix_mng[lp].dev_minor, O_RDWR );

		if(ix_mng[lp].fd >= 0 )	return ix_mng[lp].fd;
	}

	return -1;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룜瑜� �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//        �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
int ix_set_screen_size(int x, int y)
{
	ix_sreen_x_max = x;
	ix_sreen_y_max = y;

	ix_mouse_x	= (ix_sreen_x_max - ix_sreen_x_min )/2 + ix_sreen_x_min ;
	ix_mouse_y	= (ix_sreen_y_max - ix_sreen_y_min )/2 + ix_sreen_y_min ;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�듅�뙋�삕.
//--------------------------------------------------------------
int ix_get_mouse_position(int *x, int *y)
{
	*x = ix_mouse_x;
	*y = ix_mouse_y;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�떢紐뚯삕 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕髥��뜝占�.
//--------------------------------------------------------------
int ix_add_event(ix_event_func_t func)
{
	ix_event_func = (ix_event_func_t) func;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : key �뜝�뙃�젰怨ㅼ삕�뜝�룞�삕 �뜝�룞�삕�뜝�듅�뙋�삕.
//--------------------------------------------------------------
int ix_get_key_value(int *key)
{
	*key = ix_key;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : touch screen �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뒗�뙋�삕.
// �뜝�룞�삕�솚 : -1 �뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙥�궪�삕 , 0�뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
int ix_set_cal_touch_info(int index ,ix_cal_touch_info_t one_point)
{

	if(index < 0)						return -1;
	//if(index >=IX_CAL_TOUCH_INFO_MAX) 	return -1;

	ix_cal_touch_info[index] = one_point;

	printf("\r x[%d]:%d , y[%d]:%d ",index ,ix_cal_touch_info[index].sx ,index ,ix_cal_touch_info[index].sy); TRACE;
	printf("\r x[%d]:%d , y[%d]:%d ",index ,ix_cal_touch_info[index].tx ,index ,ix_cal_touch_info[index].ty);TRACE;
	printf("\n");

	ix_adjust_flag =0;
	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : touch screen �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
int ix_adjust_touch( void )
{
	int tx1 =0; //touch min
	int tx2 =0;	//touch max

	int sx1 =0; //screen min
	int sx2 =0;	//screen max

	int ty1 =0; //touch min
	int ty2 =0; //touch max

	int sy1 =0; //screen min
	int sy2 =0;	//screen max

	tx1 =(ix_cal_touch_info[0].tx + ix_cal_touch_info[2].tx )/2 ;
	tx2 =(ix_cal_touch_info[1].tx + ix_cal_touch_info[3].tx )/2 ;
	sx1 =(ix_cal_touch_info[0].sx + ix_cal_touch_info[2].sx )/2 ;
	sx2 =(ix_cal_touch_info[1].sx + ix_cal_touch_info[3].sx )/2 ;

	ty1 =(ix_cal_touch_info[0].ty + ix_cal_touch_info[1].ty )/2 ;
	ty2 =(ix_cal_touch_info[2].ty + ix_cal_touch_info[3].ty )/2 ;
	sy1 =(ix_cal_touch_info[0].sy + ix_cal_touch_info[1].sy )/2 ;
	sy2 =(ix_cal_touch_info[2].sy + ix_cal_touch_info[3].sy )/2 ;


#if 0//wj@@test-touch
	ix_adjust_touch_info.xa = ((tx2 -tx1)*IX_CAL_TOUCH_INFO_OFSSET) /(sx2 - sx1);
	ix_adjust_touch_info.xb = tx1 -(( ix_adjust_touch_info.xa * sx1)/IX_CAL_TOUCH_INFO_OFSSET) ;

	ix_adjust_touch_info.ya = ((ty2 -ty1)*IX_CAL_TOUCH_INFO_OFSSET )/(sy2 - sy1);
	ix_adjust_touch_info.yb = ty1 -(( ix_adjust_touch_info.ya * sy1)/IX_CAL_TOUCH_INFO_OFSSET) ;
#else
    ix_adjust_touch_info.xa = ((sx1 - sx2) * IX_CAL_TOUCH_INFO_OFSSET) /(tx1 - tx2);
    ix_adjust_touch_info.xb = (sx1 * IX_CAL_TOUCH_INFO_OFSSET) - (ix_adjust_touch_info.xa * tx1) ;

    ix_adjust_touch_info.ya = ((sy1 - sy2) * IX_CAL_TOUCH_INFO_OFSSET )/(ty1 - ty2);
    ix_adjust_touch_info.yb = (sy1 * IX_CAL_TOUCH_INFO_OFSSET) - (ix_adjust_touch_info.ya * ty1) ;


    ix_adjust_touch_flag = FLAG_SET;

#endif

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕孃��뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뼱�꽌 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룜�꽌 �뜝�룞�삕�뜝占� �뜝�룞�삕移� �뜝�떛釉앹삕�듃 �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕.
//      : �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�떛遺�釉앹삕�뜝�룞�삕 ix_write_event_low()�뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�뙏�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝占�
// imesu 2008-02-20
//--------------------------------------------------------------
static int ix_write_event_bulk_low(ix_event_t *event_data)
{
	int flag_write_event = -1;									// ix_write_event_low �뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 泥댄겕�뜝�룞�삕
	int lp =0;

	if(event_data == NULL)	return -1;

	for(lp = 0; lp < ix_mng_count; lp++)
	{
		if(!(ix_write_event_low(ix_mng[lp].fd, event_data )))
		{
			flag_write_event = 0;	//�뜝�떬諭꾩삕�뜝�떛�씛�삕 event write�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�뙥�궪�삕!
		}
	}
	if(flag_write_event <0)
	{
		return -1;	//ix_write_event_low�뜝�뙃�눦�삕 �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕
	}
	return 0;

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�떛釉앹삕�듃 �뜝�룞�삕移섇뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕孃��뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙇�뼱�꽌 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
//--------------------------------------------------------------
int ix_write_event_low(int fd, ix_event_t *event_data )
{
	if( fd < 0 || event_data == NULL )
	{
		ix_error_code = IXERR_WRITE_EVENT; 		// �뜝�룞�삕�뜝�룞�삕
		return -1;
	}

	write( fd, event_data, sizeof(ix_event_t));
	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�럩�뒪 �듅�뜝�룞�삕�뜝�룞�삕�듉�뜝�룞�삕 Up down �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
//--------------------------------------------------------------
int ix_write_mouse_button_action(int mouse_button, int action)
{
	ix_event_t event_data;

	//step1: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
    gettimeofday( &event_data.time, NULL );
	event_data.type = EV_KEY;

	//step2: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	switch(mouse_button)
	{
	case IX_MOUSE_BTN_LEFT:		event_data.code = BTN_LEFT;							break;
	case IX_MOUSE_BTN_RIGHT:	event_data.code = BTN_RIGHT;						break;
	case IX_MOUSE_BTN_MIDDLE:	event_data.code = BTN_MIDDLE;						break;
	default:					ix_error_code   = IXERR_WRITE_MOUSE_BUTTON_EVENT; 	return -1;	// �뜝�룞�삕�뜝�룞�삕
	}

	//step3: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�듉 �뜝�룞�삕�뜝�듅怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕
	switch(action)
	{
	case IE_MOUSE_DOWN:	event_data.value = IX_MOUSE_VAULE_DOWN_LOW;					break;
	case IE_MOUSE_UP:	event_data.value = IX_MOUSE_VAULE_UP_LOW;					break;
	default:			ix_error_code    = IXERR_WRITE_MOUSE_BUTTON_EVENT; 		return -1;	// �뜝�룞�삕�뜝�룞�삕
	}

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	//step4: SYC �뜝�뙥�궪�삕
    //gettimeofday( &event_data.time, NULL );

	event_data.type  = EV_SYN;
	event_data.code  = SYN_REPORT;
	event_data.value = 0;


	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	return 0;

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�럩�뒪 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
// �뜝�룞�삕�솚 : 0 :�뜝�룞�삕�뜝�룞�삕, -1; �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
int ix_write_mouse_rel_move(int rel_x, int rel_y)
{
	ix_event_t event_data;

	//step1: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
    gettimeofday( &event_data.time, NULL );
	event_data.type = EV_REL;

	//step2: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 X �뜝�룞�삕�뜝�뜽媛� �뜝�룞�삕�뜝�룞�삕
	event_data.code = REL_X;
	event_data.value = rel_x;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;
	//step3: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 Y �뜝�룞�삕�뜝�뜽媛� �뜝�룞�삕�뜝�룞�삕
	event_data.code = REL_Y;
	event_data.value = rel_y;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	//step4: SYC �뜝�뙥�궪�삕

    //gettimeofday( &event_data.time, NULL );
	event_data.type  = EV_SYN;
	event_data.code  = SYN_REPORT;
	event_data.value = 0;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �궎�뜝�룞�삕�뜝�뜴�뿉 �궎 �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궓�뜝�룞�삕.
// �뜝�떊怨ㅼ삕	:
//			keyboard_key: keyboard�뜝�룞�삕 �궎 code�뜝�룞�삕 �뜝�떎諭꾩삕�뜝�룞�삕. �뜝�룞�삕�뜝�룞�삕: Linux 而ㅵ뜝�떥�냼�눦�삕/include/linux/input.h�뜝�룞�삕 'Keys and buttons' �뜝�뙎紐뚯삕
//
//			action: �뜝�뙏�뙋�삕 �궎�뜝�룞�삕 �뜝�뙥�궪�삕�뜝�룞�삕�궎�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//
//			IE_KEY_DOWN
//			IE_KEY_UP
//--------------------------------------------------------------
int ix_write_keyboard_key_action(int keyboard_key, int action)
{
	ix_event_t event_data;

	//step1: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
    gettimeofday( &event_data.time, NULL );
	event_data.type = EV_KEY;

	//step2: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 KEY �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	event_data.code = keyboard_key;

	//step3: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�뙎�눦�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	switch(action)
	{
	case IE_KEY_DOWN:	event_data.value = 1;							break;
	case IE_KEY_UP:		event_data.value = 0;							break;
	default:			ix_error_code    = IXERR_WRITE_KEYBOARD_EVENT; 	return -1;	// �뜝�룞�삕�뜝�룞�삕
	}

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	//step4: SYC �뜝�뙥�궪�삕
    //gettimeofday( &event_data.time, NULL );
	event_data.type  = EV_SYN;
	event_data.code  = SYN_REPORT;
	event_data.value = 0;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕孃��뜝占� �뜝�뙐琉꾩삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�뙇�뙋�삕 �뜝�떍�룞�삕�궎 �뜝�뙓�벝�삕�뜝占� �뜝�룞�삕�솚�뜝�떦�슱�삕 �뜝�룞�삕�솚�뜝�떬�뙋�삕.
//        �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� KEY_1�뜝�룞�삕 0x30�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚�뜝�떦�슱�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
//        Shift , ALT, CTRL�궎�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕髥��뜝占�.
// http://lxr.falinux.com/source/include/linux/input.h �뜝�룞�삕 �뜝�뙇�뙋�삕 �궎 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �궎�뜝�룞�삕�뜝�룞�삕 �뜝�띁�뿴�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�떎�뼲�삕 �뜝�룞�삕�뜷?
//--------------------------------------------------------------
static int ix_get_key_press_value(int *m_key, ix_btn_state_t button_state)
{
	int shift_flag	= 0;
	int askii		= 0;

	//printf("[*m_key]:0x%x ", *m_key);  TRACE;
	if(*m_key > IX_KEY_PRESS_MAX) return -1;

	if( (button_state & IX_KEYBOARD_BTN_LEFT_SHIFT)||(button_state & IX_KEYBOARD_BTN_RIGHT_SHIFT) )
	{
		shift_flag = FLAG_SET;											// shift �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 flag�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
	}

	if(KEY_1 <= *m_key && *m_key <=KEY_SLASH )
	{
		askii = ix_key_to_askii[*m_key]; 								// KEY_XXX �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떍�룞�삕�궎 �뜝�뙓�벝�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚

		if(ASKII_A <= askii && askii <= ASKII_Z)
		{
			if( shift_flag == FLAG_SET || ix_capslock_flag == FLAG_SET )
			{
				askii =ix_key_to_askii_with_shift[*m_key];
			}
		}
		else
		{
			if( shift_flag == FLAG_SET)
			{
				askii =ix_key_to_askii_with_shift[*m_key];
			}
		}
	}
	*m_key = askii;
	return 0;
}
/*
//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�듅�뙋�삕.
// imesu 2009-02-20
//--------------------------------------------------------------
int ix_touch_calcurator(int *x, int *y)
{
	int tx =0;
	int ty =0;

	int sx = 0;
	int sy = 0;

	ix_get_mouse_position(&tx,&ty);							// �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕移섇뜝�룞�삕�겕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�듅�뙋�삕.

	printf("ix_adjust_touch_info.xa:%d ",ix_adjust_touch_info.xa);	TRACE;
	printf("ix_adjust_touch_info.xb:%d ",ix_adjust_touch_info.xb);	TRACE;
	printf("ix_adjust_touch_info.ya:%d ",ix_adjust_touch_info.ya);	TRACE;
	printf("ix_adjust_touch_info.yb:%d ",ix_adjust_touch_info.yb);	TRACE;

	if(ix_adjust_touch_info.xa ==0)	return -1;				// �뜝�떩紐뚯삕 0�뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.

	sx = (tx-ix_adjust_touch_info.xb)/ix_adjust_touch_info.xa;
	if(sx <= 0)
	{
		sx =0;
	}
	else if(sx >= ix_sreen_x_max)
	{
		sx = ix_sreen_x_max;
	}

	if(ix_adjust_touch_info.ya ==0)	return -1;				// �뜝�떩紐뚯삕 0�뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.

	sy = (ty-ix_adjust_touch_info.yb)/ix_adjust_touch_info.ya;
	if(sy <= 0)
	{
		sy =0;
	}
	else if(sy >= ix_sreen_x_max)
	{
		sy = ix_sreen_x_max;
	}
	*x = sx;
	*y = sy;

	return 0;
}
*/

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�룞�삕 �궎�뜝�룞�삕�뜝�뜴�뿉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕�뜝�룞�삕 , �뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚 ( �뜝�떍�룞�삕�궎 -> KEY_XX)
// imesu 2009-02-20
// �뜝�룞�삕�뜝�룞�삕 :
//         key :  �뜝�떍�룞�삕�궎 �뜝�뙓�뱶媛믣뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�뙣�뒗�뙋�삕.
// �뜝�룞�삕�뜝�룞�삕 : ix_write_key_press('A');  ix_write_key_press(3)
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�뙓琉꾩삕 �뜝�룞�삕�뜝�룞�삕 1�뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.
// TO DO : �뜝��諭꾩삕�뜝�룞�삕 泥섇뜝�룞�삕
//--------------------------------------------------------------
int ix_write_key_press(int ascii)
{
	int key_xxx =0;
	ix_event_t event_data;

	if(128 < ascii )	return -1;

	// �뜝��諭꾩삕�뜝�뙓怨ㅼ삕 �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕 �뜝��諭꾩삕�뜝�룞�삕 泥섇뜝�룞�삕
	if(ASKII_a <= ascii && ascii<=ASKII_z)
	{
		ix_button_state |= IX_KEYBOARD_BTN_LEFT_SHIFT;
	}
	else
	{
		ix_button_state &= ~IX_KEYBOARD_BTN_LEFT_SHIFT;	// �궎�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 shitf �뜝�룞�삕�듉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕?
	}

	key_xxx = ix_askii[ascii];

	//printf("key_xxx :%d , ascii:0x%x " ,key_xxx, ascii);	TRACE;

	//step1: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
    gettimeofday( &event_data.time, NULL );
	event_data.type = EV_KEY;

	//step2: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 KEY �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	event_data.code = key_xxx;

	//step3: event �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�뙎�눦�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
	event_data.value = 1;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	//step4: SYC �뜝�뙥�궪�삕
    //gettimeofday( &event_data.time, NULL );
	event_data.type  = EV_SYN;
	event_data.code  = SYN_REPORT;
	event_data.value = 0;

	// �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝占� �뜝�뙓�뱾�윭�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. ... �뜝�룞�삕�뜝�뙥�슱�삕 �뜝�룞�삕移� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떎紐뚯삕, �뜝�뙏�뙋�삕 �뜝�뙓�뱾�윭�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕.!!
	if(ix_write_event_bulk_low(&event_data) <0)		return -1;

	if(ix_write_keyboard_key_action(key_xxx, IE_KEY_UP) <0 ) return -1;

	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�룞�삕 �궎�뜝�룞�삕�뜝�뜴�뿉�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�뙃�젰�슱�삕�뜝�룞�삕 , �뜝�룞�삕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�솚 ( �뜝�떍�룞�삕�궎 -> KEY_XX)
// imesu 2009-02-20
// �뜝�룞�삕�뜝�룞�삕 :
//         str :  �뜝�룞�삕�뜝�뙓�슱�삕�뜝�룞�삕 �뜝�뙃琉꾩삕 �뜝�뙣�뒗�뙋�삕.
//         size : �뜝�룞�삕�뜝�뙓�슱�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕
// �뜝�룞�삕�뜝�룞�삕 : ix_write_key_press('ABC', 3);
//--------------------------------------------------------------
int ix_write_key_string_press(char *str, int size)
{
	return 0;

}



//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 :
//--------------------------------------------------------------
int ix_get_touch(int x_tch, int y_tch, int *x_scr_ptr, int *y_scr_ptr)
{
    int   x, y;

#if 1//reserved
    if ( !ix_adjust_touch_flag )
    {
        *x_scr_ptr = x_tch;
        *y_scr_ptr = y_tch;

        return 1;
    }
#endif

    x = (ix_adjust_touch_info.xa * x_tch + ix_adjust_touch_info.xb) / IX_CAL_TOUCH_INFO_OFSSET;
    y = (ix_adjust_touch_info.ya * y_tch + ix_adjust_touch_info.yb) / IX_CAL_TOUCH_INFO_OFSSET;

    if( x < 0 )                 x = 0;
    if( y < 0 )                 y = 0;

    if(x >= ix_sreen_x_max)     x = ix_sreen_x_max;
    if(y >= ix_sreen_y_max)     y = ix_sreen_y_max;

    *x_scr_ptr = (int)x;
    *y_scr_ptr = (int)y;
    return 1;
}

#if 1//�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ix_adjust_touch_info �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떬�뙋�삕
//--------------------------------------------------------------
int ix_set_touch( ix_adjust_touch_info_t *info )
{

    ix_adjust_touch_info.xa = info->xa;
    ix_adjust_touch_info.xb = info->xb;
    ix_adjust_touch_info.ya = info->ya;
    ix_adjust_touch_info.yb = info->yb;

    ix_adjust_touch_flag = FLAG_SET;

	return 1;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ix_adjust_touch_info �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�듅�뙋�삕
//--------------------------------------------------------------
int ix_get_touch_info( ix_adjust_touch_info_t *info )
{
    *info = ix_adjust_touch_info;
	return 1;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ix_adjust_touch_info �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�듃 �뜝�떬�뙋�삕
//--------------------------------------------------------------
int ix_print_touch( void )
{

    printf( "ix_adjust_touch_info = %d \n", ix_adjust_touch_info.xa );
    printf( "ix_adjust_touch_info = %d \n", ix_adjust_touch_info.xb );
    printf( "ix_adjust_touch_info = %d \n", ix_adjust_touch_info.ya );
    printf( "ix_adjust_touch_info = %d \n", ix_adjust_touch_info.yb );

	return 1;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ix_adjust_touch_info �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떗源띿삕�솕
//--------------------------------------------------------------
int ix_adjust_touch_init( void )
{
    ix_adjust_touch_flag = 0;

	return 0;
}
#endif



//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 移쇔뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떛�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떥�벝�삕�뜝�떬�뙋�삕.
//--------------------------------------------------------------
int ix_loading_ts_cailbration_info( void )
{
	FILE 	*fd;
	char 	linebuf[128]={0,};
	char	cmd[128]	={0,};
	int 	lp			=0;
	int		tag_index	=0;

	char	*tag[] ={	"[screen-x]",	"[screen-y]",
						"[touch-x]",	"[touch-y]"
					};

	if(access(FILE_TOUCH_CAL_INFO ,F_OK))
	{
		printf("%s not found!!\n",FILE_TOUCH_CAL_INFO);
		return -1;
	}

	fd = fopen(FILE_TOUCH_CAL_INFO ,"r");
	if(fd <0)
	{
		printf("file open error!!\n");
	}

	while(!feof(fd))
	{
		fgets(linebuf, sizeof(linebuf)-1 , fd);

		if(linebuf[0] == '#')  continue;			// �뜝�뙇�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.

		sscanf(linebuf ,"%s", cmd);

		for( tag_index=0 ; tag_index < 4 ; tag_index++ )
		{
			if(!strcmp(cmd , tag[tag_index]))
			{
				for(lp=0 ; lp <4 ; lp++)
				{
					memset(linebuf, 0, sizeof(linebuf));
					memset(cmd    , 0, sizeof(cmd));

					fgets(linebuf, sizeof(linebuf)-1 , fd);		// �뜝�룞�삕�뜝�뙐�뼲�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕.
					if(linebuf[0] == '#')  continue;			// �뜝�뙇�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.

					if(atoi(&linebuf[0]) == (lp+1))
					{
						sprintf(cmd,"%d=",lp+1);

						if(!memcmp(linebuf , cmd, 2 ))
						{
							switch(tag_index)
							{
							case 0	: ix_cal_touch_info[lp].sx = atoi(&linebuf[2]); break;
							case 1	: ix_cal_touch_info[lp].sy = atoi(&linebuf[2]); break;
							case 2	: ix_cal_touch_info[lp].tx = atoi(&linebuf[2]); break;
							case 3	: ix_cal_touch_info[lp].ty = atoi(&linebuf[2]); break;
							default : break;
							}
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}

#if 1
	for(lp=0 ; lp <4 ; lp++)
	{
		printf("======================================\n");
		printf("\r screen -x[%d]:%d , y[%d]:%d ",lp,ix_cal_touch_info[lp].sx ,lp ,ix_cal_touch_info[lp].sy);TRACE;
		printf("\r touch  -x[%d]:%d , y[%d]:%d ",lp ,ix_cal_touch_info[lp].tx ,lp ,ix_cal_touch_info[lp].ty);TRACE;
		printf("\n");
	}
#endif

	fclose(fd);
	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕移� �뜝�룞�삕�겕�뜝�룞�삕 移쇔뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떛�눦�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
// 1) section�뜝�룞�삕 李얍뜝�듅�뙋�삕.
// keyname  �뜝�룞�삕 '1='
//--------------------------------------------------------------
int ix_write_ts_cailbration_info( char *section, char *keyname ,int value )
{

	FILE 	*fd;
	char 	linebuf[128]={0,};
	char	cmd[128]	={0,};
	int 	lp			=0;
	int size=0;

	if(access(FILE_TOUCH_CAL_INFO ,F_OK))
	{
		printf("%s not found!!\n",FILE_TOUCH_CAL_INFO);
		return -1;
	}

	fd = fopen(FILE_TOUCH_CAL_INFO ,"r+");
	if(fd <0)
	{
		printf("file open error!!\n");
	}

	while(!feof(fd))
	{
		fgets(linebuf, sizeof(linebuf)-1 , fd);

		if(linebuf[0] == '#')  continue;			// �뜝�뙇�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.

		sscanf(linebuf ,"%s", cmd);

		if(!strcmp(cmd , section))					// �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떬�뙋�삕.
		{
			printf("[cmd]:%s [section]:%s \n",cmd ,section);

			for(lp=0 ; lp <4 ; lp++)
			{
				memset(linebuf, 0, sizeof(linebuf));
				memset(cmd    , 0, sizeof(cmd));

				fgets(linebuf, sizeof(linebuf)-1 , fd);		// �뜝�룞�삕�뜝�뙐�뼲�삕 �뜝�떩�뼲�삕 �뜝�듅�뙋�삕. �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 \n �뜝�룞�삕 �뜝�룞�삕�뜝�뙃�벝�삕
				size = sizeof(linebuf);
				printf("[size]:%d \n",size);
				fseek(fd, -size, SEEK_CUR	);

				if(linebuf[0] == '#')  continue;			// �뜝�뙇�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.

				if(atoi(&linebuf[0]) == (lp+1))				// key�뜝�룞�삕 李얍뜝�듅�뙋�삕.
				{
					// keyname �뜝�룞�삕 �솗�뜝�룞�삕�뜝�떬�뙋�삕.
					if(!strncmp(linebuf , keyname, 2 ))
					{

						printf("linebuf:%d , keyname:%s   \n",atoi(&linebuf[0]) ,keyname );
						printf("lp�뜝�룞�삕 key %s �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕. \n", keyname);
						fprintf(fd, "\r%s%d",keyname,value );	// �뜝�룞�삕�뜝�룞�삕 , ?? �뜝�룞�삕 �뜝�룞�삕移� �뜝�떍琉꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕?																// fgets �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�떛�벝�삕�뜝�떬�뙋�삕.
					}
				}
			}
		}
		else
		{
			continue;
		}

	}

#if 0
	for(lp=0 ; lp <4 ; lp++)
	{
		printf("======================================\n");
		printf("\r screen -x[%d]:%d , y[%d]:%d ",lp,ix_cal_touch_info[lp].sx ,lp ,ix_cal_touch_info[lp].sy);TRACE;
		printf("\r touch  -x[%d]:%d , y[%d]:%d ",lp ,ix_cal_touch_info[lp].tx ,lp ,ix_cal_touch_info[lp].ty);TRACE;
		printf("\n");
	}
#endif

	fclose(fd);
	return 0;

}

void nc_scan_device( void )
{
	if( ex_init( EX_WAIT_TIME_DEFAULT ) )							// ex lib�뜝�룞�삕 �뜝�떗源띿삕�솕 �뜝�떬�뙋�삕.
	{
		ex_print_error("\n");
		return;
	}

	if( ix_init() )													// ix lib�뜝�룞�삕 �뜝�떗源띿삕�솕�뜝�떬�뙋�삕.
	{
		ix_print_error("\n");
		return;
	}

	if(ix_auto_scan_open(IX_TMP_PATH_DEFAULT))						// �뜝�뙃琉꾩삕 �뜝�룞�삕�뜝�룞�삕鈺닷뜝�룞�삕�뜝占� �뜝�뙓�벝�삕�뜝�떙�궪�삕 �뜝�룞�삕 open泥섇뜝�룞�삕
	{
		ix_print_error("\n");
		return;
	}

}
void* th_autoDetect( void *arg )
{
	while(1)
	{
		sleep(5);
		nc_scan_device();
	}

}


void set_max_screen_size( void )
{
	int max_x, max_y;
#if 0
	if( ReadSystemVideoOutPath() )
	{
		max_x = 720;
		max_y = (ReadSystemVideoFormat()) ? 576 : 480;
	}
	else
#endif
	{
		max_x = NC_HI_FB_Get_Width();
		max_y = NC_HI_FB_Get_Height();
	}

	ix_set_screen_size( max_x, max_y );
}

int system_device_input_autoDetect( void )
{
	void *arg;
	pthread_t mouse_check = 0;

	nc_scan_device();
	set_max_screen_size();

	pthread_create(&mouse_check, NULL, th_autoDetect, arg);
	//sleep(1);
	return 1;
}


unsigned int s_beforeCursorPos[2] = {0, };

int SetMousePoint( int x, int y )
{
	int ret;
	unsigned int temp[4] = {0,};

	temp[0] = (unsigned int)x;	// Mouse pos X
	temp[1] = (unsigned int)y;	// Mouse pos Y


	//ret = ioctl( g_fd_drv_mux, IOC_LCD_POS_CURSOR, temp );
	if(s_beforeCursorPos[0] != temp[0] || s_beforeCursorPos[1] != temp[1])
	{
		NC_HI_FB_Move_Cursor( x,  y );
	}
	/*if( ret < 0 )
  	{
		printf( "Error in ioctl IOC_LCD_POS_CURSOR (Err:0x%2x)\n", ret );
   		return -1;
   	}
	*/
	s_beforeCursorPos[0] = temp[0];
	s_beforeCursorPos[1] = temp[1];

	return 0;
}


//#include "input.h"
//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
//--------------------------------------------------------------
static 	ex_mng_t		ex_mng_list[EX_POLL_LIST_MAX];				// �뜝�룞�삕�뜝�룞�삕 �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝�룞�삕�듃
static 	int 			ex_mng_count	= 0;						// �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

static 	pollfd_t		ex_poll_list[EX_POLL_LIST_MAX];				// �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝占� 泥섇뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥�
static  int				ex_poll_index[EX_POLL_LIST_MAX];            // �뜝�떛釉앹삕�듃�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�뙏�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
static 	int 			ex_poll_count	= 0;						// �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕
static 	int 			ex_adjust_poll_req	= 0;					// ex_adjust_poll_implement()�뜝�룞�삕 �샇�뜝�뜦�뿬�뜝�룞�삕 �솗�뜝�룞�삕 flag

//--------------------------------------------------------------
// �뜝�뙃�눦�삕
//--------------------------------------------------------------
static 	int 			ex_wait_time			= EX_WAIT_TIME_DEFAULT;		// �뜝�떍諭꾩삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�뙃�눦�삕�뜝�룞�삕 �깉�뜝�룞�삕�뜝�룞�삕 ���뜝�룞�삕 �뜝�떍�슱�삕
static 	int				ex_error_code			= EXERR_NONE;				// �뜝�룞�삕�뜝�룞�삕�뜝�뙓�벝�삕

		int 			(*ex_on_poll_timeout)	(void)				= NULL;
		int				(*ex_on_hooking_loop)	(int poll_state)	= NULL;

static	int				ex_adjust_poll_implement(void);						// poll 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�벝�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�솕 �뜝�떬�뙋�삕.
//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : event �뜝�룞�삕�뜝�떛釉뚮윭�뜝�룞�삕�뜝�룞�삕 �뜝�떗源띿삕�솕�뜝�떬�뙋�삕.
// �뜝�떊怨ㅼ삕 : wait_time poll �뜝�룞�삕�뜝占� �뜝�떆怨ㅼ삕 �뜝�룞�삕�뜝�룞�삕, �뜝�룞�삕�뜝�룞�삕[ms], 0�뜝�떛�궪�삕�뜝�룞�삕 �뜝�룞�삕
// �뜝�룞�삕�솚 : �뜝�룞�삕�뜝�룞�삕0, �뜝�룞�삕�뜝�룞�삕-1
//--------------------------------------------------------------
int ex_init( int wait_time )
{
	int lp;
	if(wait_time<0)
	{
		ex_error_code = EXERR_WAIT_TIME;
		return -1;
	}

	ex_wait_time = wait_time;

	memset(ex_mng_list,0,sizeof(ex_mng_list));
	for( lp = 0; lp < EX_POLL_LIST_MAX; lp++ )
	{
		ex_mng_list[lp].fd = -1;										// �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝占� 泥섇뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥�
	}
	ex_mng_count = 0;										// �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

	memset(ex_poll_list,0,sizeof(ex_poll_list));
	for( lp = 0; lp < EX_POLL_LIST_MAX; lp++ )
	{
		ex_poll_list[lp].fd = -1;										// �뜝�떛釉앹삕�듃 �뜝�룞�삕�뜝占� 泥섇뜝�룞�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥�
	}
	ex_poll_count = 0;										// �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕泥� �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�겕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕

	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : ex_error_code�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�뙓�슱�삕�뜝�룞�삕 �뜝�룞�삕�솚
//--------------------------------------------------------------
static char  *ex_error_string( void)
{
	char *error_string[] ={ "�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",                                    		//EXERR_NONE
							"ex_init�뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�떆怨ㅼ삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕",               		//EXERR_WAIT_TIME
							"poll �뜝�뙃�눦�삕 �뜝�룞�삕�뜝�룞�삕"	     								//EXERR_POLL_FUNC
							"ex_add_fd�뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� fd �뜝�룞�삕�뜝�룞�삕 ",						//EXERR_POLL_ADD_FD
							"ex_add_fd�뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣紐뚯삕 �뜝�룞�삕�뜝�룞�삕 "					//EXERR_POLL_ADD_MEM
                        };
   return( error_string[ex_error_code]);
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : exlib�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�뙓�벝�삕, �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝�뙓�슱�삕 �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣�눦�삕�뜝�룞�삕 �뜝�룞�삕�뜝占�
// �뜝�떊怨ㅼ삕 : �뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�뙣�눦�삕�뜝�룞�삕
//--------------------------------------------------------------
void ex_print_error( char *msg )
{
	printf( "\r[gx error:%d]%s %s\n", ex_error_code, ex_error_string(), msg);
	//return gx_error_code;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 :
//--------------------------------------------------------------
int ex_loop( void )
{
	int 			poll_state;
	int				mng_index;
	int 			lp;
	struct pollfd	*ex_poll_item;
	ex_mng_t		*ex_mng_item;

__RETRY__ :

	if( ex_adjust_poll_req )
	{
		ex_adjust_poll_implement();			// poll 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�벝�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�솕 �뜝�떬�뙋�삕.
		ex_adjust_poll_req = 0;				// flag �뜝�떗源띿삕�솕
	}

	// poll()�뜝�룞�삕 �샇�뜝�룞�삕�뜝�떦�슱�삕 event �뜝�뙥�궪�삕 �뜝�룞�삕�뜝�룞�삕 �솗�뜝�룞�삕
	poll_state = poll( ex_poll_list,    	// event �뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕
                        ex_poll_count,    	// 泥댄겕�뜝�룞�삕 pollfd �뜝�룞�삕�뜝�룞�삕
                        ex_wait_time     	// time out �뜝�떆怨ㅼ삕
                       );

#if 0
	if( poll_state < 0 )
	{
		ex_error_code = EXERR_POLL_FUNC;
		return -1;
	}
#endif
	if( ex_on_hooking_loop != NULL )
	{
		if( ex_on_hooking_loop(poll_state) )
		{
			//fprintf(stdout, "\n");
		}
	}

	if( poll_state == 0 )
	{
		if( ex_on_poll_timeout != NULL)
			ex_on_poll_timeout();

		goto __RETRY__;
	}

	for( lp = 0; lp < ex_poll_count; lp++ )
	{
		ex_poll_item	= &ex_poll_list[lp];
		mng_index		= ex_poll_index[lp];
		ex_mng_item		= &ex_mng_list[mng_index];

		if( ex_poll_item->revents & POLLIN 	&& ex_mng_item->on_read  != NULL )
			ex_mng_item->on_read ( ex_mng_item );
		if( ex_poll_item->revents & POLLOUT && ex_mng_item->on_write != NULL )
			ex_mng_item->on_write( ex_mng_item );
/*
		if( ex_poll_item.revents & POLLERR 	&& ex_event_t->on_poll_err != NULL )
 			on_poll_err( );
		if( ex_poll_item.revents & POLLHUP 	&& ex_event_t->on_poll_hup != NULL )
			on_poll_hup( );
*/
	}
	return 0;
}


//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : fd�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�뙃�룞�삕�궎�뜝�룞�삕, fd�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�뙋�삕 ex_mng_t �뜝�룞�삕�뜝�룞�삕泥닷뜝�룞�삕 �뜝�룞�삕�솚�뜝�떬�뙋�삕.
// �뜝�룞�삕�솚 : NULL�뜝�떛紐뚯삕 �뜝�룞�삕�뜝�룞�삕, �뜝�룞�삕 �뜝�룞�삕 ex_mng_t �뜝�룞�삕�뜝�룞�삕泥� �뜝�뙇�눦�삕
// �뜝�룞�삕�뜝�룞�삕 : �뜝�룞�삕�뜝�룞�삕 fd�뜝�룞�삕 �뜝�룞�삕�왎�윴�뜝占�
// �뜝�룞�삕�뜝�룞�삕 : �뜝�뙃�눦�삕�샇�뜝�룞�삕 �뜝�룞�삕 ex_adjust_poll�뜝�뙃�눦�삕 �샇�뜝�룞�삕�뜝�룞�삕 �뜝�떗�슱�삕.
//--------------------------------------------------------------
ex_mng_t * ex_add_fd( int fd )
{
	int lp;

	if(fd <0)
	{
		ex_error_code = EXERR_POLL_ADD_FD;
		return NULL;
	}

	for(lp=0;lp<ex_mng_count;lp++)
	{
		if(ex_mng_list[lp].fd == fd)
		{
			return &ex_mng_list[lp];
		}
	}

	if(ex_mng_count >=EX_POLL_LIST_MAX)
	{
		ex_error_code = EXERR_POLL_ADD_MEM;
		return NULL;
	}

	ex_mng_list[ex_mng_count].fd = fd;
	return &ex_mng_list[ex_mng_count++];
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : fd�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕�궎�뜝�룞�삕
// �뜝�룞�삕�뜝�룞�삕 : ex_del_fd()�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕 ex_adjust_poll()�뜝�룞�삕 �뜝�뙠�벝�삕�뜝占� �뜝�룞�삕�뜝�뙇�뼲�삕�뜝占�.
//--------------------------------------------------------------
void ex_del_fd(int fd)
{
	int find_lp, del_lp;

	if(ex_mng_count <1)
		return;

	for( find_lp=0; find_lp<ex_mng_count; find_lp++ )
	{
		if(ex_mng_list[find_lp].fd ==fd)
		{
			// �뜝�룞�삕�뜝�룞�삕�뜝�떬�뙋�삕.
			for(del_lp =find_lp ; del_lp < ex_mng_count-1 ;del_lp++ )
			{
				ex_mng_list[del_lp]= ex_mng_list[del_lp+1];
			}
			ex_mng_count--;

			// �뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕 clear�뜝�떬�뙋�삕.
			ex_mng_list[del_lp].fd 			=-1;
			ex_mng_list[del_lp].priv 		= NULL;
			ex_mng_list[del_lp].on_read		= NULL;
			ex_mng_list[del_lp].on_write	= NULL;
		}
	}

}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : poll 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�벝�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�솕 �뜝�떦怨ㅼ삕 �뜝�떆琉꾩삕�뜝�뙎紐뚯삕 �뜝�룞�삕�뜝�룞�삕�뜝占�.
//       :ex_loop() POLL �뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떗�슱�삕�뜝�떦�뙋�삕.
//--------------------------------------------------------------
int ex_adjust_poll( void)
{
	ex_adjust_poll_req = 1;
	return 0;
}

//--------------------------------------------------------------
// �뜝�룞�삕�뜝�룞�삕 : poll 泥섇뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�뜝�떦�벝�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕�솕 �뜝�떬�뙋�삕.
//       :ex_loop() POLL �뜝�떦源띿삕 �뜝�룞�삕�뜝�룞�삕 �뜝�떗�슱�삕�뜝�떦�뙋�삕.
//--------------------------------------------------------------
static int ex_adjust_poll_implement( void)
{
	int lp;
	short events;

	ex_poll_count = 0;								// �뜝�떗源띿삕�솕

	for(lp=0;lp<ex_mng_count;lp++)
	{
		if(ex_mng_list[lp].fd < 0) continue;

		events = 0;

		if(ex_mng_list[lp].on_read  !=NULL) events |= POLLIN;			//  read �뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕繹앭뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 POLLIN�뜝�룞�삕 �뜝�룞�삕�뜝占�
		if(ex_mng_list[lp].on_write !=NULL) events |= POLLOUT;			//  on_write �뜝�뙃�눦�삕�뜝�룞�삕 �뜝�룞�삕繹앭뜝占� �뜝�룞�삕�뜝�룞�삕�뜝�룞�삕 POLLIN�뜝�룞�삕 �뜝�룞�삕�뜝占�

		if(events == 0) continue;

		ex_poll_list[ex_poll_count].fd      = ex_mng_list[lp].fd ;
		ex_poll_list[ex_poll_count].events  = events;
		ex_poll_list[ex_poll_count].revents = 0;

		ex_poll_index[ex_poll_count] 		= lp;

		ex_poll_count++;
	}

	return 0;
}


//extern void UIApp_TouchEvent( int x, int y, int _1down0up );

typedef struct tagMOUSE_INFO
{
	unsigned char	btn_info;	//Mid Btn(BIT2), Right Btn(BIT1), Left Btn(BIT0)
	unsigned short	x;
	unsigned short	y;
	unsigned short	z;
} NC_MOUSE_INFO, *PNC_MOUSE_INFO;

static NC_MOUSE_INFO s_mouse_data;
static NC_MOUSE_INFO s_mouse_data_old;



#define BIT0	0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10	0x0400
#define BIT11	0x0800
#define BIT12	0x1000
#define BIT13	0x2000
#define BIT14	0x4000
#define BIT15	0x8000

void SetMouseData( NC_MOUSE_INFO m )
{
	s_mouse_data_old = s_mouse_data;

	s_mouse_data.x = m.x;
	s_mouse_data.y = m.y;
	s_mouse_data.z = m.z;
}

static char s_wheel_value = 0;
void SetMouseWheelValue( char nWheelValue )
{
	s_wheel_value = nWheelValue;
}

char GetMouseWheelValue( void )
{
	return s_wheel_value;
}

void SetReleaseMouseRightButton( void )
{
	s_mouse_data.btn_info &= ~BIT1;
}

void SetReleaseMouseLeftButton( void )
{
	s_mouse_data.btn_info &= ~BIT0;
}

void SetPressMouseLeftButton( void )
{
	s_mouse_data.btn_info |= BIT0;
}

void SetPressMouseRightButton( void )
{
	s_mouse_data.btn_info |= BIT1;
}

////////////////////////////////////////////////////////////////////////////////////////

static struct timeval s_old_mouse_time;
static struct timeval s_now_mouse_time;

typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef int BOOL;

#define FALSE 0
#define TRUE 1

static BYTE s_dc_count = 0;
static BYTE s_dc = 0;
static BYTE s_dc_change_mode = 0;		//	when double click, current screen view mode
static BYTE s_dc_change_mode_group = 0;	//	when double click, group number of current screen view mode


#define MOUSE_DOUBLE_CLICK_INTERVAL		500000

void InitMouseDoubleClickInverval( void )
{
	gettimeofday( &s_now_mouse_time, NULL );
	memcpy( &s_old_mouse_time, &s_now_mouse_time, sizeof( s_now_mouse_time ) );
}

BOOL IsMouseDoubleClick( void )
{
	BOOL bDoubleClick = s_dc;

	if( s_dc )
		s_dc = 0;

	return bDoubleClick;
}

void ClearDoubleCheckFlags( void )
{
	s_dc = FALSE;
	s_dc_count = 0;
}

static DWORD get_mouse_click_inverval( void )
{
	memcpy( &s_old_mouse_time, &s_now_mouse_time, sizeof(s_now_mouse_time) );
	gettimeofday( &s_now_mouse_time, NULL );

	if( s_now_mouse_time.tv_usec < s_old_mouse_time.tv_usec )
	{
		s_now_mouse_time.tv_usec += 1000000;
		s_now_mouse_time.tv_sec --;
	}

	return (s_now_mouse_time.tv_sec  - s_old_mouse_time.tv_sec)*1000000 +
		   (s_now_mouse_time.tv_usec - s_old_mouse_time.tv_usec);
}

void CheckDoubleClick( void )
{
	BYTE 	nPress;
	DWORD 	nInterval;

	//	Double Click�뜝�룞�삕 �뜝�떙�궪�삕�뜝�룞�삕�뜝�룞�삕 �뜝�룞�삕�뜝�룞�삕...
	//if( IsShowMessageBox() || IsShowPopupMenu() )
	//	return;

	nPress = s_mouse_data.btn_info;

	nInterval = get_mouse_click_inverval();
	if( nInterval > MOUSE_DOUBLE_CLICK_INTERVAL )
	{
        ClearDoubleCheckFlags();

        if( nPress == 0 )
        	return;
	}

    if( nPress == 1 && s_dc_count == 0 )		//	first press
    {
        s_dc_count = 1;
		s_dc = FALSE;
    }
    else if( nPress == 0 && s_dc_count == 1 )	//	first release
    {
        s_dc_count = 2;
		s_dc = FALSE;
    }
#if 1
    else if( nPress == 1 && s_dc_count == 2 )	//	second press
	{
		s_dc_count = 0;
		s_dc = TRUE;
	}
    else
    {
    	s_dc_count = 0;
    	s_dc = FALSE;
    }
#else
    else if( nPress == 1 && s_dc_count == 2 )	//	second press
    {
        s_dc_count = 3;
		s_dc = FALSE;
    }
    else if( nPress == 0 && s_dc_count == 3 )	//	second release
    {
        s_dc_count = 0;
        s_dc = TRUE;
    }
	else if( nPress == 0 )
	{
        s_dc_count = 0;
		s_dc = FALSE;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////

int input_event_proc( ie_event_t* event_data )
{
	NC_MOUSE_INFO 	mouse;
	int				state = (int)event_data->data.mouse.state;

	mouse.x = event_data->data.mouse.x;
	mouse.y = event_data->data.mouse.y;
	mouse.z = event_data->data.mouse.wheel;
//	mouse.btn_info = (BYTE)btn; //Mid Btn(BIT2), Right Btn(BIT1), Left Btn(BIT0)

	if( mouse.z )
		SetMouseWheelValue( mouse.z );
	SetMouseData( mouse );

	int l_down=0;
	int l_up=0;

	int lr_move=0;

	int r_down=0;
	int r_up=0;

	int wheel_down = 0;
	int wheel_up = 0;


#if 1
	switch(event_data->type)
	{
		case IE_MOUSE_DOWN	:
//			printf("IE_MOUSE_DOWN   x:%d , y:%d, wheel:%d \n" ,event_data->data.mouse.x ,event_data->data.mouse.y,  event_data->data.mouse.wheel);
			if( state & IX_MOUSE_BTN_RIGHT)
			{
				SetPressMouseRightButton();

				r_down = 1;
				//UIApp_TouchEvent( mouse.x, mouse.y, 2 );
			}

			if( state & IX_MOUSE_BTN_LEFT )
			{
				SetPressMouseLeftButton();

				l_down = 1;
				//UIApp_TouchEvent( mouse.x, mouse.y, 1 );
			}
			break;

		case IE_MOUSE_UP 	:
			if( !(state & IX_MOUSE_BTN_RIGHT))
			{
				if( s_mouse_data.btn_info & BIT1 )
				{
					//UIApp_TouchEvent( mouse.x, mouse.y, 3 );
					r_up = 1;
				}
				SetReleaseMouseRightButton();
			}

			if( !(state & IX_MOUSE_BTN_LEFT) )
			{
				if( s_mouse_data.btn_info & BIT0 )
				{
					//UIApp_TouchEvent( mouse.x, mouse.y, 0 );
					l_up = 1;
				}
				SetReleaseMouseLeftButton();
			}

//			printf("IE_MOUSE_UP   x:%d , y:%d, wheel:%d \n" ,event_data->data.mouse.x ,event_data->data.mouse.y,  event_data->data.mouse.wheel);
			break;

		case IE_MOUSE_MOVE :

#if 0
{
			BYTE str_xy[10] = {0,};
//			printf("IE_MOUSE_MOVE   x:%d , y:%d, wheel:%d \n" ,event_data->data.mouse.x ,event_data->data.mouse.y,  event_data->data.mouse.wheel);

			SPRINTF( str_xy, "X:%4d, Y:%4d", event_data->data.mouse.x, event_data->data.mouse.y);
			PrintSingleBox( LAYER_HD, SBOX_1F, 0, 20, 60, 5, UD_COL1, MIXTBL0 );
			PrintEngString( LAYER_HD,  0, 22, IMG_DISP, IMGBUF0, str_xy, NOBLINK, STRLEN(str_xy), 0 );
}
#endif
			if(s_mouse_data.btn_info==0)
			{
				// Hover
				//UIApp_TouchEvent( mouse.x, mouse.y, 0 );
				lr_move = 1;
			}
			else if(s_mouse_data.btn_info==1)
			{
				// Left Drag
				//UIApp_TouchEvent( mouse.x, mouse.y, 1 );
				l_down = 1;
			}
			else if(s_mouse_data.btn_info==2)
			{
				// Right Drag
				//UIApp_TouchEvent( mouse.x, mouse.y, 2 );
				r_down = 1;
			}
			break;

		case IE_MOUSE_WHEEL :
//			printf("IE_MOUSE_WHEEL   x:%d , y:%d, wheel:%d \n" ,event_data->data.mouse.x ,event_data->data.mouse.y,  event_data->data.mouse.wheel);
			if(  event_data->data.mouse.wheel < 0 )
			{
				wheel_down = 1;
			}
			else if( event_data->data.mouse.wheel > 0)
			{
				wheel_up = 1;
			}
			break;

		case IE_KEY_PRESS :
//			printf("IE_KEY_PRESS     state:%#x, key:%c \n",event_data->data.key.state,event_data->data.key.key);
			break;

	}
#endif

	CheckDoubleClick();
	if(IsMouseDoubleClick())
	{
		UIApp_TouchEvent( mouse.x, mouse.y, 4 );
		//printf("DblClick..(%d,%d)\n",mouse.x,mouse.y);
	}
	else
	{
		     if(l_down) UIApp_TouchEvent( mouse.x, mouse.y, 1 );
		else if(l_up)   UIApp_TouchEvent( mouse.x, mouse.y, 0 );
		else if(r_down) UIApp_TouchEvent( mouse.x, mouse.y, 2 );
		else if(r_up)   UIApp_TouchEvent( mouse.x, mouse.y, 3 );
		else if(lr_move) UIApp_TouchEvent( mouse.x, mouse.y, 0 );
		else if(wheel_up)   UIApp_MouseWheelEvent( mouse.x, mouse.y, 1 );
		else if(wheel_down) UIApp_MouseWheelEvent( mouse.x, mouse.y, 0 );
	}

#if 0
	//wake up main thread
	MUTEX_LOCK( &s_mouse_cond_mutex );
	if( s_mouse_wait )
	{
//		fprintf(stdout, ">>[%d]%s\n", __LINE__, __FUNCTION__);
		COND_BROADCAST( &s_mouse_cond_cond );
		s_mouse_wait = FALSE;
	}
	MUTEX_UNLOCK( &s_mouse_cond_mutex );
#endif
	return 0;
}

void *tMouseProc( void *arg )
{
	while( 1 )
	{
		if( ex_loop() )
		{
			 DBG_ERR("\n");
		}
	}
}


int run_mouse_proc( void )
{
	int ret = -1;
	pthread_t mouse_check = 0;

	ret = pthread_create( &mouse_check, NULL, tMouseProc, NULL);

	return 1;
}

void InitMouseEvent( void )
{
	ix_add_event( input_event_proc );
	//ex_on_hooking_loop = check_other_event;

	// run mouse process
	run_mouse_proc();
}

int NC_HI_Input_Init(void)
{
	system_device_input_autoDetect();
	DBG_INFO("mouse\n");
	InitMouseEvent();
	DBG_INFO("ok\n");
}




