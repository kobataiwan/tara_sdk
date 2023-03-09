
LIVE_DIR = ../
#EXEC = wis-streamer
EXEC = streaming_server

all:	$(EXEC)
#	install $(EXEC) $(IPNC_EXEC_DIR)

install:	$(EXEC)
	install $(EXEC) $(IPNC_EXEC_DIR)

#CC = $(BUILD_TOOL_PREFIX)gcc
#CPLUSPLUS = $(BUILD_TOOL_PREFIX)g++
CC = $(CROSS)gcc
CPLUSPLUS = $(CROSS)g++

INCLUDES = -I. \
	-I$(LIVE_DIR)/BasicUsageEnvironment/include \
	-I$(LIVE_DIR)/UsageEnvironment/include \
	-I$(LIVE_DIR)/groupsock/include \
	-I$(LIVE_DIR)/liveMedia/include \
	-I$(INIPARSER_SRC_DIR) \
	-I$(PUBLIC_INCLUDE_DIR) \
	-I$(MPP_BASE_DIR)/include -I$(IPNC_DIR)/../tara_api/include

CFLAGS =  -lpthread $(INCLUDES) -D_LINUX -D$(BUILD_CFG) -g -Wall -O3
LDFLAGS += -s 

LIBS =	-L$(LIVE_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
	-L$(LIVE_DIR)/UsageEnvironment -lUsageEnvironment \
	-L$(LIVE_DIR)/groupsock -lgroupsock \
	-L$(LIVE_DIR)/liveMedia -lliveMedia \
	-L$(INIPARSER_LIB_DIR) -liniparser \

OBJS = wis-streamer.o Err.o APPROInput.o \
	WISServerMediaSubsession.o \
	WISJPEGStreamSource.o \
	WISJPEGVideoServerMediaSubsession.o \
	WISH264VideoServerMediaSubsession.o \
	WISH265VideoServerMediaSubsession.o \
	WISPCMAudioServerMediaSubsession.o \
	$(APP_LIB_DIR)/av_interface.a 

$(EXEC): $(OBJS)
	$(CPLUSPLUS) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)


wis-streamer.cpp:			Err.hh

Err.cpp:				Err.hh

APPROInput.cpp:				APPROInput.hh Err.hh

WISServerMediaSubsession.cpp:		WISServerMediaSubsession.hh

WISServerMediaSubsession.hh:		APPROInput.hh

WISH264VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh
WISH264VideoServerMediaSubsession.cpp:	WISH264VideoServerMediaSubsession.hh

WISH265VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh
WISH265VideoServerMediaSubsession.cpp:	WISH265VideoServerMediaSubsession.hh

WISJPEGStreamSource.cpp:		WISJPEGStreamSource.hh
WISPCMAudioServerMediaSubsession.cpp:	WISPCMAudioServerMediaSubsession.hh


.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f $(EXEC)
