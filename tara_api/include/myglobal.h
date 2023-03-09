#ifndef __MY_GLOBAL_DEF_H__
#define __MY_GLOBAL_DEF_H__

#include <syslog.h>

#ifdef CPLUSPLUS
extern "C" {
#endif

#define MAX_CHANNEL_NUM      8

enum {
	TARA_LOG_PANIC=0,
	TARA_LOG_ALERT=1,
	TARA_LOG_CRIT=2,
	TARA_LOG_ERROR=3,
	TARA_LOG_WARNING=4,
	TARA_LOG_NOTICE=5,
	TARA_LOG_INFO=6,
	TARA_LOG_DEBUG=7,
	TARA_LOG_DEBUG_ALL=10
};

typedef char                s8;
typedef short              s16;
typedef int                s32;
typedef long long          s64;

typedef unsigned char       u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

extern u32 num_channel;
extern u32 dbg_level;
extern s32 tosyslog;
extern s8 *logfile;
extern s32 logsize;
extern void tolog(int level, const char *fmt, ...);

#ifdef CPLUSPLUS
}
#endif
#endif
