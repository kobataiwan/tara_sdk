#ifndef _TARA_LIGHTING_H_
#define _TARA_LIGHTING_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#ifdef CPLUSPLUS
extern "C" {
#endif

#define TARA_LIGHTING_LEVEL_MAX		100
#define TARA_LIGHTING_LEVEL_MIN		0
#define TARA_LIGHTING_FREQ_MAX		1000000
#define TARA_LIGHTING_FREQ_MIN		100

typedef struct {
	u32 freq;		// frequency setting, the valid value is 100Hz~1000000Hz.
	u32 level;	// brightness setting, the valid value is 0~100, 0 is to turn OFF the lighting. The default value is 50.
} TaraLightingConfig_t;

typedef struct {
	TaraLightingConfig_t cool;
	TaraLightingConfig_t warm;
} TaraLightingConfig;

typedef struct {
	u32 has_gpio_pwm;  // 1: can do pwm control by gpio
	u32 has_hw_pwm;    // 1: can do pwm control by hw
} TaraLightingDevice;

/*
 * set cool white level.
 * level: configuration to set, the valid value is 0~100, 0 is to turn OFF the cool white.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_setCoolWhiteLevel(u32 level);

/*
 * set cool white level and frequency.
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_setCoolWhite(TaraLightingConfig_t *ptr);

/*
 * set warm white level.
 * level: configuration to set, the valid value is 0~100, 0 is to turn OFF the cool white.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_setWarmWhiteLevel(u32 level);

/*
 * set warm white level and frequency.
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_setWarmWhite(TaraLightingConfig_t *ptr);

/*
 * get current TaraLightingConfig
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_getConfig(TaraLightingConfig *ptr);

/*
 * get PWM device
 * ptr: filled with current pwm device status
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_lighting_getPWMDevice(TaraLightingDevice *ptr);
#ifdef CPLUSPLUS
}
#endif
#endif
