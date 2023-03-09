#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h> 
#include <unistd.h>
#include "tara_utc.h"


s32 Tara_utc_control(s32 chan, s32 cmd, void *data)
{
	int ret;
	int fd=-1;
	char *sockname=UTC_CMD_SOCKET;
	struct stat sb;
	struct sockaddr_un addr;
	s32 buf[2];
	
	if (chan > num_channel) return ERROR_INVALARG;
	if (cmd >= UTC_CMD_MAX) return ERROR_INVALARG;
	if (stat(sockname, &sb) != 0) {
		tolog(TARA_LOG_INFO, "%s:%d sockname:%s not exist\n", __FUNCTION__, __LINE__, sockname);
		ret = -1;
		goto CLEANUP;
	}

	// connect to server
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) goto CLEANUP;

	fcntl(fd, F_SETFL, O_NONBLOCK);
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockname);
	if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		ret = -1;
		goto CLEANUP;
	}
	buf[0] = chan;
	buf[1] = cmd;
	// send command
	if (write(fd, buf, sizeof(buf)) == -1) {
		tolog(TARA_LOG_INFO, "%s:%d write error: %s (ignored)\n", __FUNCTION__, __LINE__, strerror(errno));
	}
	// wait ack ( optional )

	ret = 0;

CLEANUP:
	if (fd != -1) close(fd);
	return ret;
}
