#CROSS_COMPILE=$(BUILD_TOOL_PREFIX)
INCLUDES += -I./ -I$(PUBLIC_INCLUDE_DIR) -I$(MPP_BASE_DIR)/include -I$(MPP_BASE_DIR)/../av_server -I$(IPNC_DIR)/../tara_api/include

#C_FLAGS += -Wall -fPIC -O3 -lpthread -D$(SENSOR) -D$(BUILD_CFG) -DONVIF_H265
#C_FLAGS += -Wall -O3 -lpthread -D$(SENSOR) -D$(BUILD_CFG) -DONVIF_H265
C_FLAGS += -Wall -O3 -lpthread -mfloat-abi=hard
AR_FLAGS += -r

CC = $(CROSS_COMPILE)gcc $(INCLUDES) $(C_FLAGS) -c
AR = $(CROSS_COMPILE)ar

#REL_EXE1 = reboot_ipnc

#REL_LIB1 = file_msg_drv.a

#REL_LIB2 = recorder_msg_drv.a

REL_LIB3 = share_mem.a

#REL_LIB4 = sys_msg_drv.a

REL_LIB5 = msg_util.a

REL_LIB7 = av_interface.a

#REL_LIB8 = libtara.a

REL_LIB9 = sem_util.a

#REL_LIB10 = serial_msg_drv.a

#REL_LIB11 = netUtil.a

REL_OBJ1 = sem_util.o msg_util.o share_mem.o file_msg_drv.o

REL_OBJ2 = sem_util.o msg_util.o share_mem.o recorder_msg_drv.o

REL_OBJ3 = share_mem.o

REL_OBJ4 = sem_util.o msg_util.o share_mem.o sys_msg_drv.o

REL_OBJ5 = msg_util.o

REL_OBJ7 = sem_util.o msg_util.o avDrvMsg.o av_interface.o 

REL_OBJ8 = sem_util.o msg_util.o avDrvMsg.o serial_msg_drv.o tara_api.o 

REL_OBJ9 = sem_util.o

REL_OBJ10 = sem_util.o msg_util.o serial_msg_drv.o

REL_OBJ11 = netUtil.o

#INTERFACE_LIBS = $(REL_LIB11)
#INTERFACE_LIBS = $(REL_LIB1) $(REL_LIB2) $(REL_LIB3) $(REL_LIB4) $(REL_LIB5) $(REL_LIB7)  $(REL_LIB9) $(REL_LIB10) $(REL_LIB11) $(REL_LIB8)
INTERFACE_LIBS = $(REL_LIB1) $(REL_LIB2) $(REL_LIB3) $(REL_LIB4) $(REL_LIB5) $(REL_LIB7)  $(REL_LIB9) $(REL_LIB10) $(REL_LIB11)

INSTALL_LIBS := $(foreach libs,$(INTERFACE_LIBS),$(APP_LIB_DIR)/$(libs))

#INTERFACE_EXES = $(REL_EXE1)

all: $(INTERFACE_LIBS)
	if [ ! -d $(APP_LIB_DIR) ]; then \
		if [ -e $(APP_LIB_DIR) ]; then \
			$(RM) -f $(APP_LIB_DIR); \
		fi; \
		mkdir $(APP_LIB_DIR); \
	fi
	$(MAKE) install

$(REL_LIB1): $(REL_OBJ1)
	$(AR) $(AR_FLAGS) $(REL_LIB1) $(REL_OBJ1)

$(REL_LIB2): $(REL_OBJ2)
	$(AR) $(AR_FLAGS) $(REL_LIB2) $(REL_OBJ2)

$(REL_LIB3): $(REL_OBJ3)
	$(AR) $(AR_FLAGS) $(REL_LIB3) $(REL_OBJ3)

$(REL_LIB4): $(REL_OBJ4)
	$(AR) $(AR_FLAGS) $(REL_LIB4) $(REL_OBJ4)

$(REL_LIB5): $(REL_OBJ5)
	$(AR) $(AR_FLAGS) $(REL_LIB5) $(REL_OBJ5)
	
$(REL_LIB7): $(REL_OBJ7) 
	$(AR) $(AR_FLAGS) $(REL_LIB7) $(REL_OBJ7)
	
$(REL_LIB8): $(REL_OBJ8) 
	$(AR) $(AR_FLAGS) $(REL_LIB8) $(REL_OBJ8)
#	cp -f $(REL_LIB8) $(IPNC_DIR)/../sample_app/lib
#	cp ../inc/tara_api.h $(IPNC_DIR)/../sample_app/include
##	cp ../inc/tara_imaging.h $(IPNC_DIR)/../sample/
##	cp ../inc/tara_preset.h $(IPNC_DIR)/../sample/
#	cp ../inc/tara_osd.h $(IPNC_DIR)/../sample_app/include
##	cp ../inc/tara_video.h $(IPNC_DIR)/../sample/
##	cp ../inc/tara_ptzf.h $(IPNC_DIR)/../sample/

$(REL_LIB9): $(REL_OBJ9) 
	$(AR) $(AR_FLAGS) $(REL_LIB9) $(REL_OBJ9)

$(REL_LIB10): $(REL_OBJ10) 
	$(AR) $(AR_FLAGS) $(REL_LIB10) $(REL_OBJ10)

$(REL_LIB11): $(REL_OBJ11) 
	$(AR) $(AR_FLAGS) $(REL_LIB11) $(REL_OBJ11)

file_msg_drv.o : file_msg_drv.c $(PUBLIC_INCLUDE_DIR)/file_msg_drv.h $(PUBLIC_INCLUDE_DIR)/Msg_Def.h\
		$(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<

share_mem.o: share_mem.c $(PUBLIC_INCLUDE_DIR)/share_mem.h
	$(CC) $(C_FLAGS) -o $@ $<

sys_msg_drv.o : sys_msg_drv.c $(PUBLIC_INCLUDE_DIR)/sys_msg_drv.h $(PUBLIC_INCLUDE_DIR)/Msg_Def.h\
		$(PUBLIC_INCLUDE_DIR)/sys_env_type.h $(PUBLIC_INCLUDE_DIR)/share_mem.h
	$(CC) $(C_FLAGS) -o $@ $<

recorder_msg_drv.o : recorder_msg_drv.c $(PUBLIC_INCLUDE_DIR)/recorder_msg_drv.h $(PUBLIC_INCLUDE_DIR)/Msg_Def.h\
		$(PUBLIC_INCLUDE_DIR)/sys_env_type.h $(PUBLIC_INCLUDE_DIR)/share_mem.h
	$(CC) $(C_FLAGS) -o $@ $<

msg_util.o: msg_util.c $(PUBLIC_INCLUDE_DIR)/Msg_Def.h
	$(CC) $(C_FLAGS) -o $@ $<

avDrvMsg.o: avDrvMsg.c $(PUBLIC_INCLUDE_DIR)/avDrvMsg.h $(PUBLIC_INCLUDE_DIR)/av_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<

av_interface.o: av_interface.c $(PUBLIC_INCLUDE_DIR)/av_interface.h
	$(CC) $(C_FLAGS) -o $@ $<

#iniparser.o: iniparser.c $(PUBLIC_INCLUDE_DIR)/iniparser.h $(PUBLIC_INCLUDE_DIR)/tara_osd.h
#	$(CC) $(C_FLAGS) -o $@ $<

#dictionary.o: dictionary.c $(PUBLIC_INCLUDE_DIR)/dictionary.h $(PUBLIC_INCLUDE_DIR)/tara_osd.h
#	$(CC) $(C_FLAGS) -o $@ $<

#tara_api.o: tara_api.c $(PUBLIC_INCLUDE_DIR)/tara_api.h $(PUBLIC_INCLUDE_DIR)/tara_osd.h $(PUBLIC_INCLUDE_DIR)/tarai_video.h $(PUBLIC_INCLUDE_DIR)/tara_imaging.h $(PUBLIC_INCLUDE_DIR)/tara_ptzf.h $(PUBLIC_INCLUDE_DIR)/tara_preset.h $(PUBLIC_INCLUDE_DIR)/tara_serial.h
tara_api.o: tara_api.c $(PUBLIC_INCLUDE_DIR)/tara_api.h
	$(CC) $(C_FLAGS) -o $@ $<

sem_util.o: sem_util.c $(PUBLIC_INCLUDE_DIR)/sem_util.h
	$(CC) $(C_FLAGS) -o $@ $<

serial_msg_drv.o: serial_msg_drv.c $(PUBLIC_INCLUDE_DIR)/serial_msg_drv.h $(PUBLIC_INCLUDE_DIR)/serial_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<

netUtil.o: netUtil.c $(PUBLIC_INCLUDE_DIR)/netUtil.h
	$(CC) $(C_FLAGS) -o $@ $<

#reboot_ipnc: 
#	$(BUILD_TOOL_PREFIX)gcc -g -Wall -static  -c -o reboot_ipnc.o reboot_ipnc.c
#	$(BUILD_TOOL_PREFIX)gcc -o reboot_ipnc reboot_ipnc.o
	
clean:
	-$(RM) -f *.o
	-$(RM) -f *.a
ifneq ($(APP_LIB_DIR),)
	-$(RM) -Rf $(APP_LIB_DIR)
endif
	rm -rf $(IPNC_DIR)/../sample_app/include/*
	rm -rf $(IPNC_DIR)/../sample_app/lib/*

$(APP_LIB_DIR)/%.a : %.a
	install $< $(APP_LIB_DIR)
	
install: $(INSTALL_LIBS)
#	install $(REL_EXE1) $(EXEC_DIR)
