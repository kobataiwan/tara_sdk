#ifndef __TARA_LOG_H__
#define __TARA_LOG_H__

#ifdef CPLUSPLUS
extern "C" {
#endif

#include "myglobal.h"
#include "myerr.h"

/*
 * format and output log message
 * level: message level, output message if level is greater than log level
 * format: format of the format string
 */
#define Tara_log(level, format, args...) if (dbg_level>=level && tosyslog == 1) syslog(level, format, ##args); tolog(level, format, ##args)

/*
 * set log level
 * level: log level
 */
extern void Tara_log_set_level(s32 level);

/*
 * set log file, size
 * logpath: can be tty device or local file
 *          ex. /dev/console ; /dev/pts/0 ; /tmp/app.log
 * size: if logpath is local file, set the maximum size of the file
 *       if file size reaches the maximum size, move logpath to logpath.1 and start new file
 */
extern void Tara_log_set_logfile(s8 *logpath, s32 size);

/*
 * set log to syslog
 * to_syslog: 1 => output to syslog; 0 => donot output to syslog
 * server: if to_syslog is 1, forward log to remote syslogd server
 * port: if set, forward log to remote server port
 */
extern void Tara_log_set_syslog(s32 to_syslog, s8 *server, u16 port);

#ifdef CPLUSPLUS
}
#endif
#endif
