#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmkey.h"
#include "mutex.h"
#include "tara_sys.h"

s32 Tara_sys_reset(SYS_RESET_TYPE type)
{
	FILE * fp;
	
	if (type >= SYS_RESET_MAX) return ERROR_INVALARG;
	if (type == SYS_RESET_SOFT) fp = fopen ("/tmp/soft_reset", "w+");
	else if (type == SYS_RESET_HART) fp = fopen ("/tmp/hard_reset", "w+");
	else if (type == SYS_RESET_ACCOUNTS) fp = fopen ("/tmp/reset_accounts", "w+");
	else return ERROR_INVALARG;
	fclose(fp);
	
	return 0;
}
