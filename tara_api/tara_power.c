#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/file.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h> 
#include <sys/socket.h>

#include "shmkey.h"
#include "mutex.h"
#include "tara_power.h"
#include "tara_log.h"

s32 power_control(s32 cmd, void *data)
{
	int ret=0;
	int fd=-1;
	char *sockname = POWER_CMD_SOCKET;
	struct stat sb;
	struct sockaddr_un addr;
	u32 buf[128];
	fd_set rfds;
	struct timeval tv;
	int cnt, size_to_read=0, size_to_write=0;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	u32 *pu32 = buf;
	
	if (cmd >= PWR_CMD_SET_MAX) return ERROR_INVALARG;
	if (cmd >= PWR_CMD_GET_MAX && cmd < PWR_CMD_SET_START) return ERROR_INVALARG;
	if (stat(sockname, &sb) != 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d sockname:%s not exist\n", __FUNCTION__, __LINE__, sockname);
		ret = -1;
		goto CLEANUP;
	}

	// connect to server
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) goto CLEANUP;

	
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockname);
	if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		ret = -1;
		goto CLEANUP;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	
	memset(buf, 0, sizeof(buf));
	pu32[0] = cmd;

	if (cmd == PWR_CMD_SET_CONFIG) {
		size_to_write = sizeof(TaraPowerConfig);
		memcpy(pu32+1, (TaraPowerConfig *)data, size_to_write);
		size_to_write += sizeof(u32);
	} else if (cmd == PWR_CMD_SET_WOL || cmd == PWR_CMD_SET_SYSTEM_POWER) {
		size_to_write = sizeof(u32);
		memcpy(pu32+1, (u32 *)data, size_to_write);
		size_to_write += sizeof(u32);
	} else size_to_write = sizeof(u32);

	// send command
	if (write(fd, buf, size_to_write) == -1) {
		Tara_log(TARA_LOG_ERROR, "%s:%d write error: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		goto CLEANUP;
	}
		
	ret = 0;
	// wait ack ( optional )
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	cnt = 0;
	if (cmd == PWR_CMD_GET_CONFIG) {
		size_to_read = sizeof(TaraPowerConfig);
		pu32 = (u32 *)data;
	} else if (cmd == PWR_CMD_GET_POWER_MONITOR) {
		size_to_read = sizeof(TaraPowerMonitor);
		pu32 = (u32 *)data;
	} else if (cmd == PWR_CMD_GET_WOL || cmd == PWR_CMD_GET_WOL_DETECTED) {
		size_to_read = sizeof(u32);
		pu32 = (u32 *)data;
	} 

	while(cnt++ < 3) {
		ret = select(fd+1, &rfds, NULL, NULL, &tv);
		if (ret > 0) {
			int retry = 3;
			while(size_to_read > 0) {
				ret = read(fd, pu32, size_to_read);
				if (ret > 0) {
					size_to_read -= ret;
					pu32 += ret;
				} else if (ret < 0) break;
				else {
					if (retry-- == 0) break;
				}
			}
			break;
		}
	}
	if (size_to_read == 0) ret = 0;
	else ret = -1;

CLEANUP:
	if (fd != -1) close(fd);
	return ret;
}

/*
 * set power settings
 * ptr: configuration to set.
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_power_setConfig(TaraPowerConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_SET_CONFIG, ptr);	
	return 0;	
}

/*
 * get power settings
 * ptr: filled with current configuration
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_power_getConfig(TaraPowerConfig *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_GET_CONFIG, ptr);
	return 0;
}

/*
 * get current TaraPowerMonitor
 * ptr: filled with current power monitor value
 * return: 0 for success, otherwise error number returned
 */
s32 Tara_power_getPowerMonitor(TaraPowerMonitor *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_GET_POWER_MONITOR, ptr);
	return 0;
}

s32 Tara_power_setWoL(u32 *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_SET_WOL, ptr);
	return 0;
}

s32 Tara_power_getWoL(u32 *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_GET_WOL, ptr);
	return 0;
}

s32 Tara_power_getWoLDetected(u32 *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_GET_WOL_DETECTED, ptr);
	return 0;
}

s32 Tara_power_setSystemPower(u32 *ptr)
{
	if (!ptr) return ERROR_INVALARG;
	power_control(PWR_CMD_SET_SYSTEM_POWER, ptr);
	return 0;
}
