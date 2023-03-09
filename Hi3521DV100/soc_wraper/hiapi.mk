#include ./config.mak
#include ../mpp/Makefile.param

HIAPI_DIR=$(MPP_PATH)/sample/common
LIBS_DIR=lib
TARAAPI_DIR=$(SDK_PATH)/../tara_api

OBJS=loadbmp.o sample_comm_audio.o sample_comm_ivs.o sample_comm_sys.o sample_comm_vda.o 
OBJS+=sample_comm_vdec.o sample_comm_venc.o sample_comm_vi.o sample_comm_vo.o sample_comm_vpss.o
OBJS+=soc_video.o
OBJS+=soc_motion.o

APIBINS=libhiapi.a

INCS=-I./include -I$(REL_INC) -I$(HIAPI_DIR) -I$(MPP_PATH)/$(EXTDRV)/tlv320aic31 -I$(MPP_PATH)/$(EXTDRV)/nvp6134_ex
INCS+=-I$(HIAPI_DIR)/../audio/adp
INCS+=-I$(SDK_PATH)/../opensource/iniparser/src -I$(TARAAPI_DIR)/include -I$(SDK_PATH)/../ipnc_app/interface/inc

CFLAGS := -Wall -fPIC -D$(HIARCH) -DHICHIP=$(HICHIP) -D$(HIDBG) -D$(HI_FPGA) -lpthread -lm -ldl
CFLAGS += -DAI_DEV=0
CFLAGS += -DAO_DEV=0

all: $(OBJS) $(APIBINS)

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
	
soc_motion.o:
	$(CC) -g -c $(CFLAGS) soc_motion.c $(INCS)
	
libhiapi.a :
	mkdir -p lib
	$(AR) -r ./lib/$@ loadbmp.o sample_comm_audio.o sample_comm_ivs.o sample_comm_sys.o sample_comm_vda.o sample_comm_vdec.o sample_comm_venc.o sample_comm_vi.o sample_comm_vo.o sample_comm_vpss.o soc_video.o soc_motion.o

clean :
	rm -rf $(OBJS) $(APIBINS)
	rm -rf lib
