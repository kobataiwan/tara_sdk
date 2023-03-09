#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "tara_video_source.h"
#include "tara_lib.h"
#include "tara_log.h"

int main(int argc, char *argv[])
{
	s32 chan=0;
	TaraVideoSource vsrc;
	
	Tara_log_set_level(TARA_LOG_INFO);
	Tara_log_set_syslog(1, NULL, 0);
	Tara_init();
	for (chan=0; chan<num_channel; chan++) {
		memset(&vsrc, 0, sizeof(vsrc));
		Tara_video_source_getStatus(chan, &vsrc);
		Tara_log(TARA_LOG_INFO, "#%d type:%d %dx%d frate:%d\n", chan, vsrc.type, vsrc.width, vsrc.height, vsrc.framerate);
	}

	return 0;
}
