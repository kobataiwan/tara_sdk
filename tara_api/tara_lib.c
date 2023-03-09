#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "myglobal.h"
#include "myerr.h"
#include "tara_lib.h"
#include "tara_log.h"

u32 num_channel=0;

s32 Tara_init()
{
	num_channel = MAX_CHANNEL_NUM;

	Tara_log(TARA_LOG_INFO, "%s:%d num_channel is initialized to %d\n", __FUNCTION__, __LINE__, num_channel);
	return ERROR_NONE;
}
