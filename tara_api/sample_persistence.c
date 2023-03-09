#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tara_persistence.h"
#include "tara_lib.h"

int main(int argc, char *argv[])
{
	s32 ii, jj;	
	s8 data256[256];
	s8 data2048[2048];
	s8 *p8=NULL;
	
	if (argc < 2) {
		printf("%s set|get|clean [idx]\n", argv[0]);
		printf("  ex get persistence Data1 \n");
		printf("  %s get 1\n", argv[0]);
		printf("  ex get persistence Data2048 \n");
		printf("  %s get 2048\n", argv[0]);
		printf("  ex set persistence Data1 \n");
		printf("  %s set 1\n", argv[0]);
		printf("  ex set persistence Data2048 \n");
		printf("  %s set 2048\n", argv[0]);
		printf("  ex clean all persistence data \n");
		printf("  %s clean\n", argv[0]);

		return 0;
	}
	
	Tara_init();
	s32 idx = (argc == 3) ? atoi(argv[2]) : 1;
	if (idx < 1 || (idx > 32 && idx != 2048)) {
		printf("the valid index value is from 1 to 32, or 2048\n");
		return 0;
	}
	
	if (strcmp(argv[1], "set") == 0) {
		if (idx == 2048) {
			memset(&data2048, 0, sizeof(data2048));
			snprintf(data2048, sizeof(data2048), "hello persistence Data2048");
			Tara_persistence_setData2048(data2048);
		} else {
			memset(&data256, 0, sizeof(data256));
			snprintf(data256, sizeof(data256), "hello persistence Data256 #%02d", idx);
			Tara_persistence_setData256(idx-1, data256);
		}

	} else if (strcmp(argv[1], "get") == 0) {
		TaraPersistenceData *pdata=Tara_persistence_getData();		
		p8 = (idx == 2048) ? pdata->data2048 : pdata->data256[idx-1];
		printf("Data%d %s\n", idx, p8);
		for (jj=0; jj<256; jj++) {
			printf("%02X%s", *p8++, (jj+1)%16!=0?" ":"\n");
		}
	} else if (strcmp(argv[1], "clean") == 0) {
		memset(&data256, 0, sizeof(data256));
		memset(&data2048, 0, sizeof(data2048));
		
		for (ii=0; ii<PERSISTENCE_256_NUM; ii++)
			Tara_persistence_setData256(ii, data256);			
		Tara_persistence_setData2048(data2048);		
	} else
		printf("not support !\n");
		
	return 0;
}
