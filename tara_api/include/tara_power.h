#ifndef _TARA_POWER_H_
#define _TARA_POWER_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#ifdef CPLUSPLUS
extern "C" {
#endif

#define POWER_CMD_SOCKET              "/tmp/.pwr_cmd"

enum {
	PWR_CMD_GET_START=0,
	PWR_CMD_GET_CONFIG=PWR_CMD_GET_START,
	PWR_CMD_GET_POWER_MONITOR,
	PWR_CMD_GET_WOL,
	PWR_CMD_GET_WOL_DETECTED,
	PWR_CMD_GET_MAX,
	PWR_CMD_SET_START=80,
	PWR_CMD_SET_CONFIG=PWR_CMD_SET_START,
	PWR_CMD_SET_WOL,
	PWR_CMD_SET_SYSTEM_POWER,
	PWR_CMD_SET_MAX
};

typedef enum {
	POWER_OFF=0,
	POWER_ON
} POWER_MODE;

typedef enum {
	WOL_EVENT_DETECTED_NO = 0,
	WOL_EVENT_DETECTED_YES
} WOL_EVENT_DETECTED;

typedef struct {
	float voltage;	// voltage, measured in V
	u32 current;		// current, measured in mA
	u32 power;			// power, measured in mW
} TaraPowerMonitor_t;

typedef struct {
	TaraPowerMonitor_t gand5008a;
	TaraPowerMonitor_t lighting;
	TaraPowerMonitor_t ahd;
	TaraPowerMonitor_t _5v;
	TaraPowerMonitor_t tof;
} TaraPowerMonitor;

typedef struct {
	POWER_MODE ahd;							// AHD cameras power control.
	POWER_MODE _5v;							// 5V power control.
	POWER_MODE tof;							// ToF sensors power control.
	POWER_MODE videoPipeline;		// nvp6158C power control.
} TaraPowerConfig;

/*
 * set power settings
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_setConfig(TaraPowerConfig *ptr);

/*
 * get power settings
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_getConfig(TaraPowerConfig *ptr);

/*
 * get current TaraPowerMonitor
 * ptr: filled with current power monitor value
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_getPowerMonitor(TaraPowerMonitor *ptr);

/*
 * set Wake-on-LAN setting
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_setWoL(u32 *ptr);

/*
 * get Wake-on-LAN setting
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_getWoL(u32 *ptr);

/*
 * Power control for the core system
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_setSystemPower(u32 *ptr);

/*
 * WoL event detected.
 * ptr: WOL_EVENT_DETECTED_NO:  net EPHY_PME# is 3.3V, 
 *      WOL_EVENT_DETECTED_YES: net EPHY_PME# is 0V
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_power_getWoLDetected(u32 *ptr);
#ifdef CPLUSPLUS
}
#endif
#endif
