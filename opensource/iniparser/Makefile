TOOL_NAME :=iniparser-master
TOOL_TAR_BALL := iniparser-master.tar.bz2
TOOL_DIR := $(shell pwd)

#ifeq ($(CHIP), 3516a)
#CROSS = arm-hisiv300-linux
#TOOLCHAINI_VERSION = hisiv300
#else ifeq ($(CHIP), 3516cv300)
#CROSS = arm-hisiv500-linux
#TOOLCHAINI_VERSION = hisiv500
#else ifeq ($(CHIP), 3519v101)
#CROSS = arm-hisiv500-linux
#TOOLCHAINI_VERSION = hisiv500
#else ifeq ($(CHIP), 3516dv300)
#CROSS = arm-himix200-linux
#TOOLCHAINI_VERSION = himix200
#else ifeq ($(CHIP), 3521dv100)
#CROSS = arm-linux-gnueabi
#TOOLCHAINI_VERSION = linaro
#endif

ifneq ($(wildcard $(CROSS_GCC)),)
  CROSS=arm-linux-gnueabi
endif

LIB := $(TOOLCHAINI_VERSION)_lib
SRCDIR = src
STRIP = $(CROSS)-strip

all:
	rm $(TOOL_NAME) -rf;
	tar jxvf $(TOOL_TAR_BALL);
	mkdir -p $(LIB)/;
	mkdir -p $(SRCDIR)/;
	make -C $(TOOL_NAME)	
	cp ./$(TOOL_NAME)/libiniparser.so $(LIB)/
	cp ./$(TOOL_NAME)/src/*.h $(SRCDIR)/
	rm $(TOOL_NAME) -rf;

clean:
	rm $(TOOL_NAME) -rf;
	rm $(LIB) -rf;
	rm $(SRCDIR) -rf;

distclean: clean
