#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include "myglobal.h"

u32 dbg_level=TARA_LOG_ERROR;
s32 tosyslog=0;
s8 *logfile=NULL;
s32 logsize=100000;

void tolog(int level, const char *fmt, ...)
{
	if (dbg_level >= level) {
		FILE *logfp=stderr;
		va_list ap;
		va_start(ap, fmt);

		if (logfile) {
			struct stat sbuf;
			if (stat(logfile, &sbuf)==0 && sbuf.st_size>logsize) {
				s8 nfname[128];
				s32 fd=-1;
				if (((fd = open(logfile, O_RDONLY)) != -1) &&
				    (flock(fd, LOCK_EX) == 0)) {
					sprintf(nfname, "%s.1", logfile);
					rename(logfile, nfname);
				}

				if (fd != -1) close(fd);
			}

			logfp = fopen(logfile, "a");
		}

		vfprintf(logfp, fmt, ap);
		if (logfile) fclose(logfp);
		va_end(ap);
	}
}
