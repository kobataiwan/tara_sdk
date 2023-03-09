#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_log.h"

void Tara_log_set_level(s32 level)
{
	dbg_level = level;
}

void Tara_log_set_logfile(s8 *logpath, s32 size)
{
	if (logfile) free(logfile);	
	if (logpath && strlen(logpath)>0) logfile = strdup(logpath);
	if (size <= 0) logsize = 100000;
	else logsize = size;
}

void Tara_log_set_syslog(s32 to_syslog, s8 *server, u16 port)
{
	tosyslog = to_syslog;
	s8 cmd[64];
	int rc;
	if (tosyslog == 0) {
		snprintf(cmd, sizeof(cmd), "/etc/init.d/S85syslogd stop");
	} else if (server && strlen(server) > 0) {
		if (port > 0 && port < 65536) {
			snprintf(cmd, sizeof(cmd), "/etc/init.d/S85syslogd start %s:%d", server, port);
		} else {
			snprintf(cmd, sizeof(cmd), "/etc/init.d/S85syslogd start %s", server);
		}
	} else {
		snprintf(cmd, sizeof(cmd), "/etc/init.d/S85syslogd start");
	}

	rc = system(cmd);
	if (WIFSIGNALED(rc)) {
		printf("%s:%d system call interrupted by signal %d (%s)", __FILE__, __LINE__, WTERMSIG(rc), strsignal(WTERMSIG(rc)));
	}
}
