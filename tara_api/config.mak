ifeq ($(ARCH),)
  # cross compilation to ARM
  AR_INSTALL_DIR=/opt/gcc-linaro/gcc-linaro-5.5.0-2017.10
  AR_GCC=$(AR_INSTALL_DIR)/bin/arm-linux-gnueabi-gcc
  ifneq ($(wildcard $(AR_GCC)),)
    # detect AR config for for native builds
    # assign CROSS to full path of GCC executable, but remove "-gcc"
    CROSS=$(AR_GCC:-gcc=-)
  else
    # default environment, use PATH variable to find executables
    CROSS=arm-ca9-linux-gnueabihf-
  endif
else
  # native build
  CROSS=
endif
LIBS_DIR=lib
CC=$(CROSS)gcc
STRIP=$(CROSS)strip
AR=$(CROSS)ar

CFLAGS+=-Wall -Werror -I$(TARA_API_DIR)/./include
