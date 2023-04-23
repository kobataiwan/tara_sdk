#include ./config.mak
#include ../mpp/Makefile.param

export MPP_PATH=/home/u/pacidal/0527_tara/tara_sdk/Hi3521DV100/mpp

HIAPI_DIR=$(MPP_PATH)/sample/common
LIBS_DIR=lib
TARAAPI_DIR=$(SDK_PATH)/../tara_api

OBJS=sample_video_record.o
#OBJS=loadbmp.o sample_comm_audio.o sample_comm_ivs.o sample_comm_sys.o sample_comm_vda.o
#OBJS+=sample_comm_vdec.o sample_comm_venc.o sample_comm_vi.o sample_comm_vo.o sample_comm_vpss.o
##OBJS+=soc_video.o
OBJS+=soc_video_comm.o
OBJS+=nt9856x_video.o hisi_comm.o soc_video_api.o
#OBJS+=soc_motion.o

APIBINS=libhiapi.a

INCS=-I./include -I$(REL_INC) -I$(HIAPI_DIR) -I$(MPP_PATH)/$(EXTDRV)/tlv320aic31 -I$(MPP_PATH)/$(EXTDRV)/nvp6134_ex
INCS+=-I./include/hdal/include -I./include/hdal/vendor/isp/include -I./include/vos
INCS+=-I$(HIAPI_DIR)/../audio/adp
INCS+=-I$(SDK_PATH)/../opensource/iniparser/src -I$(TARAAPI_DIR)/include -I$(SDK_PATH)/../ipnc_app/interface/inc

CFLAGS := -Wall -fPIC -D$(HIARCH) -DHICHIP=$(HICHIP) -D$(HIDBG) -D$(HI_FPGA) -lpthread -lm -ldl -mfloat-abi=hard
CFLAGS += -DAI_DEV=0
CFLAGS += -DAO_DEV=0

all: $(OBJS) $(APIBINS)

sample_video_record.o:
	$(CC) -g -c $(CFLAGS) sample_video_record.c $(INCS)

loadbmp.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/loadbmp.c $(INCS)
	
sample_comm_audio.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_audio.c $(INCS)

sample_comm_ivs.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_ivs.c $(INCS)
	
sample_comm_sys.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_sys.c $(INCS)
	
sample_comm_vda.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_vda.c $(INCS)
	
sample_comm_vdec.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_vdec.c $(INCS)
	
sample_comm_venc.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_venc.c $(INCS)
	
sample_comm_vi.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_vi.c $(INCS)
	
sample_comm_vo.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_vo.c $(INCS)

sample_comm_vpss.o:
	$(CC) -g -c $(CFLAGS) $(HIAPI_DIR)/sample_comm_vpss.c $(INCS)

soc_video.o:
	$(CC) -g -c $(CFLAGS) soc_video.c $(INCS)

soc_video_comm.o:
	$(CC) -g -c $(CFLAGS) soc_video_comm.c $(INCS)

nt9856x_video.o:
	$(CC) -g -c $(CFLAGS) nt9856x_video.c $(INCS)
soc_video_api.o:
	$(CC) -g -c $(CFLAGS) soc_video_api.c $(INCS)
hisi_comm.o:
	$(CC) -g -c $(CFLAGS) hisi_comm.c $(INCS)
	
soc_motion.o:
	$(CC) -g -c $(CFLAGS) soc_motion.c $(INCS)
	
libhiapi.a :
	mkdir -p lib
	$(AR) -r ./lib/$@ nt9856x_video.o soc_video_api.o hisi_comm.o sample_video_record.o soc_video_comm.o

clean :
	rm -rf $(OBJS) $(APIBINS)
	rm -rf lib
