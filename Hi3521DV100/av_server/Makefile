#include config.mak
SHELL := /bin/bash
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../mpp/Makefile.param
     include $(PARAM_FILE)
endif

CFLAGS += -DTARAAPI_VER_V20
TARAAPI_DIR=$(SDK_PATH)/../tara_api

SRCROOT=$(PWD)
EXES=av_server
HILIBS=hiapi vdapi
LIBS_DIR=lib

INC_FLAGS :=-I../soc_wraper/include
INC_FLAGS +=-I$(TARAAPI_DIR)/include
INC_FLAGS +=-I$(SDK_PATH)/../ipnc_app/interface/inc
INC_FLAGS +=-I$(SDK_PATH)/../opensource/iniparser/src
INC_FLAGS +=-I$(MPP_PATH)/ahd_app_sample/ahd_app
INC_FLAGS +=-I$(MPP_PATH)/ahd_app_sample/video_decoder
INC_FLAGS +=-I$(MPP_PATH)/ahd_app_sample/video_decoder/api
#INC_FLAGS +=-I$(SDK_PATH)/../opensource/SDL1/$(TOOLCHAINI_VERSION)_build/include/SDL
#INC_FLAGS +=-I$(SDK_PATH)/../opensource/SDL_ttf/$(TOOLCHAINI_VERSION)_build/include/SDL


.PHONY : clean all
# target source

CFLAGS += $(INC_FLAGS)
CFLAGS += -D$(SENSOR) -D$(BOARD) -D_nOSD_USED -D_nAUDIO_ON_ -D_nIVS_ON_
ifeq ($(BUILD_CFG), dev)
CFLAGS += -D_DBG_MSG_
CFLAGS += -mfloat-abi=hard
endif

#LDFLAGS +=-L$(SDK_PATH)/../opensource/SDL1/$(TOOLCHAINI_VERSION)_build/lib -lSDL
#LDFLAGS +=-L$(SDK_PATH)/../opensource/SDL_ttf/$(TOOLCHAINI_VERSION)_build/lib -lSDL_ttf
#LDFLAGS +=-L$(SDK_PATH)/../opensource/freetype/$(TOOLCHAINI_VERSION)_build/lib -lfreetype
#LDFLAGS +=-L$(SDK_PATH)/../opensource/zlib/local-arm-$(TOOLCHAINI_VERSION)/lib -lz
LDFLAGS +=-L$(TARAAPI_DIR)/lib -ltara -L$(SDK_PATH)/../opensource/iniparser/$(TOOLCHAINI_VERSION)_lib -liniparser

LIBS = $(SDK_PATH)/../ipnc_app/interface/lib/msg_util.a \
       $(SDK_PATH)/../ipnc_app/interface/lib/sem_util.a $(SDK_PATH)/soc_wraper/lib/libhiapi.a ./lib/libvdapi.a $(SDK_PATH)/soc_wraper/include/hdal/lib/libhdal.a

#MPI_LIBS := $(REL_LIB)/libmpi.a
#MPI_LIBS += $(REL_LIB)/libhdmi.a $(REL_LIB)/libjpeg.a

all: $(HILIBS) $(EXES)
	$(CROSS)strip av_server
#	cp av_server $(SDK_PATH)/../target/$(CHIP_NAME)/rootfs_glibc5.5/srv/bin

av_server:
	$(CC) -lpthread -lm -ldl -o $@ $(CFLAGS) av_server.c -Wl,--start-group $(MPI_LIBS) $(AUDIO_LIBA) $(LIBS) $(LDFLAGS) -Wl,--end-group

hiapi:
	pushd $(SDK_PATH)/soc_wraper;make -f hiapi.mk;popd;
	
vdapi:
	make -f vdapi.mk
	
clean:
	pushd $(SDK_PATH)/soc_wraper;make -f hiapi.mk clean;popd;
	make -f vdapi.mk clean
	rm -rf *.o $(EXES)

