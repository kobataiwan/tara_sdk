#ifndef _TARA_TIME_H_
#define _TARA_TIME_H_

#include <errno.h>
#include <time.h>
#include "myglobal.h"
#include "myerr.h"

enum {
	DATETIME_TYPE_NOW = 0,	//Synchronized with the current viewing device
	DATETIME_TYPE_MANUALY,	//Set manually 
	DATETIME_TYPE_NTP,			//Synchronized with the NTP server 
	DATETIME_TYPE_MAX
};
#define TIME_ZONE_SIZE		9
#define NTP_SERVER_SIZE		32

typedef struct {
	s32 type;				// time sync mode
	struct tm tm_set;		// if type is DATETIME_TYPE_NOW or DATETIME_TYPE_MANUALY, set device time with tm_set
	s8 ntpserver[NTP_SERVER_SIZE];
	s8 time_zone[TIME_ZONE_SIZE];
} TaraDateTimeConfig;

#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * get current DateTimeConfiguation
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_time_getConfig(TaraDateTimeConfig *ptr);

/*
 * set DateTimeConfiguration
 * ptr: configuration to set
 * return: 0 for success, otherwise error number returned
 */
extern s32 Tara_time_setConfig(TaraDateTimeConfig *ptr);

#ifdef CPLUSPLUS
}
#endif
#endif

