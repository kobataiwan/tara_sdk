#include ./config.mak
#include ../mpp/Makefile.param

VDAPI_DIR=$(MPP_PATH)/ahd_app_sample

OBJS=raptor3_api.o video_output.o clock.o video_fmt_info.o raptor3_api_drv.o video_auto_detect.o
OBJS+=coax.o coax_firmup.o video_auto_distance_detect.o video_auto_eq.o motion.o audio.o
OBJS+=main_sample_thread.o

APIBINS=libvdapi.a

INCS=-I$(MPP_PATH)/ahd_app_sample/ahd_app
INCS+=-I$(MPP_PATH)/ahd_app_sample/video_decoder
INCS+=-I$(MPP_PATH)/ahd_app_sample/video_decoder/include
INCS+=-I$(MPP_PATH)/ahd_app_sample/video_decoder/api

CFLAGS := -Wall -fPIC -Wno-unused-but-set-variable -lpthread -lm -ldl
CFLAGS += -DTARAAPI_VER_V20

all: $(OBJS) $(APIBINS)

raptor3_api.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/raptor3_api.c $(INCS)
	
video_output.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/video_output.c $(INCS)

clock.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/clock.c $(INCS)
	
video_fmt_info.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/video_fmt_info.c $(INCS)
	
raptor3_api_drv.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/raptor3_api_drv.c $(INCS)
	
video_auto_detect.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/video_auto_detect.c $(INCS)
	
coax.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/coax.c $(INCS)
	
coax_firmup.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/coax_firmup.c $(INCS)
	
video_auto_distance_detect.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/video_auto_distance_detect.c $(INCS)

video_auto_eq.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/video_auto_eq.c $(INCS)

motion.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/motion.c $(INCS)
	
audio.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/video_decoder/api/audio.c $(INCS)
	
main_sample_thread.o:
	$(CC) -g -c $(CFLAGS) $(VDAPI_DIR)/ahd_app/main_sample_thread.c $(INCS)
	
libvdapi.a :
	mkdir -p lib
	$(AR) -r ./lib/$@ raptor3_api.o video_output.o clock.o video_fmt_info.o raptor3_api_drv.o video_auto_detect.o coax.o coax_firmup.o video_auto_distance_detect.o video_auto_eq.o motion.o audio.o main_sample_thread.o

clean :
	rm -rf $(OBJS) $(APIBINS)
	rm -rf lib
