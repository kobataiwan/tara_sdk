# ============================================================================
# Use of this software is controlled by the terms and conditions found in the
# license agreement under which this software has been supplied or provided.
# ============================================================================
BASE_INSTALL_DIR  	:= $(shell pwd)/..

BUILD_CFG := dbg
SDK_VER := V2.3.0-6
TARA_VER = `date +%Y``date +%m``date +%d`
SUBVER := 
TARAAPI_VER=v2.0

ifeq ($(MODEL), GAND5208X)
	SENSOR := NVP6158
	CHIP := 3521dv100
	BOARD := GAND5208X_V1
	MFR := Pacidal
	HTML := 16MB
	MODEL_NAME := GAND5208X
endif
ifeq ($(MODEL), GAND5008A)
	SENSOR := NVP6158
	CHIP := 3521dv100
	BOARD := GAND5008A_v0
	MFR := Pacidal
	HTML := 32MB
	MODEL_NAME := GAND5008A
endif



ifeq ($(CHIP), 3521dv100)
	CHIP_TYPE_ID 			:= CHIP_TYPE_3521dv100
	CHIP_NAME 				:= Hi3521DV100
#	BUILD_TOOL_PREFIX := arm-linux-gnueabi-
	TOOLCHAINI_VERSION:= linaro
	KERNEL_CFG				:= hi3521d_full_defconfig
	KERNELDIR 				:= $(BASE_INSTALL_DIR)/osdrv/opensource/kernel/linux-3.18.y
  TOOLCHAIN_INSTALL_DIR=/opt/arm-ca9-linux-gnueabihf-8.4
  CROSS_GCC=$(TOOLCHAIN_INSTALL_DIR)/bin/arm-ca9-linux-gnueabihf-
  ifneq ($(wildcard $(CROSS_GCC)),)
    # assign CROSS to full path of GCC executable, but replace "-gcc" by "-"
    export CROSS=$(CROSS_GCC:-gcc=-)
    export CROSS_COMPILE=$(CROSS_GCC:-gcc=-)
  else
    # default environment
    #CROSS=/opt/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-
	CROSS=arm-ca9-linux-gnueabihf-
  endif
	export LIBC=glibc
#	export CROSS=arm-linux-gnueabi-
#	export CROSS_COMPILE=arm-linux-gnueabi-
endif

export CROSS=arm-ca9-linux-gnueabihf-
export CROSS_GCC=arm-ca9-linux-gnueabihf-gcc
export CROSS_COMPILE=arm-ca9-linux-gnueabihf-

# Defining the install base directory for IPNC RDK
IPNC_INSTALL_DIR   	:= $(shell pwd)

# The directory that points to the Linux Support Package
UBOOTDIR 		:= $(BASE_INSTALL_DIR)/osdrv/opensource/uboot/u-boot-2010.06
			
# The directory that points to where filesystem is mounted
TARGET_DIR		= $(IPNC_INSTALL_DIR)/target/$(CHIP_NAME)
PCTOOLDIR 		= $(TARGET_DIR)/bin/pc

MPP_BASE_DIR	:= $(IPNC_INSTALL_DIR)/$(CHIP_NAME)/mpp
TARGET_FS 		:= $(TARGET_DIR)/rootfs_glibc5.5
#TARGET_FS 		:= $(TARGET_DIR)/rootfs_uclibc
PACIDAL_FS		:= $(TARGET_DIR)/rootfs_tara

THIRD_PARTY_BASE_DIR	:= $(IPNC_INSTALL_DIR)/opensource

# The directory that points to the tftp directory
# This will have generated binaries likeu-boot, uImage, ubifs etc
# And also other binary files can be kept here
# This will all files needed that are needed for burning to NAND
TFTP_HOME 		:= $(TARGET_DIR)/image

# Where to copy the resulting executables and data to (when executing 'make
# install') in a proper file structure. This IPNC_EXEC_DIR should either be visible
# from the target, or you will have to copy this (whole) directory onto the
# target filesystem.
IPNC_EXEC_DIR		:= $(TARGET_FS)/srv/bin

# The directory that points to the IPNC Application package
IPNC_DIR				:= $(IPNC_INSTALL_DIR)/ipnc_app
TARA_API_DIR		:= $(IPNC_INSTALL_DIR)/tara_api
TARA_API_LIB		:= $(TARA_API_DIR)/lib
TARA_API_INC		:= $(TARA_API_DIR)/include

# The directory to IPNC application interface modules to AVCAPTURE
IPNC_INTERFACE_DIR	:= $(IPNC_DIR)/interface
PUBLIC_INCLUDE_DIR	:= $(IPNC_INTERFACE_DIR)/inc
APP_LIB_DIR		:= $(IPNC_INTERFACE_DIR)/lib
INIPARSER_DIR	:= $(THIRD_PARTY_BASE_DIR)/iniparser
INIPARSER_LIB_DIR := $(INIPARSER_DIR)/$(TOOLCHAINI_VERSION)_lib
INIPARSER_SRC_DIR := $(INIPARSER_DIR)/src
UPNP_DIR					:= $(THIRD_PARTY_BASE_DIR)/libupnp
UPNP_LIB_DIR			:= $(UPNP_DIR)/$(TOOLCHAINI_VERSION)_lib
UPNP_INC_DIR			:= $(UPNP_DIR)/include
ZLIB_DIR					:= $(THIRD_PARTY_BASE_DIR)/zlib
ZLIB_BUILD_DIR		:= $(ZLIB_DIR)/local-arm-$(TOOLCHAINI_VERSION)
FCGI_DIR					:= $(THIRD_PARTY_BASE_DIR)/libfcgi
FCGI_INC_DIR			:= $(FCGI_DIR)/include
FCGI_LIB_DIR			:= $(FCGI_DIR)/$(TOOLCHAINI_VERSION)_lib
FCGI_BIN_DIR			:= $(FCGI_DIR)/$(TOOLCHAINI_VERSION)_bin
SPAWN_FCGI_DIR		:= $(THIRD_PARTY_BASE_DIR)/spawn-fcgi
SPAWN_FCGI_BIN_DIR:= $(SPAWN_FCGI_DIR)/$(TOOLCHAINI_VERSION)_bin
OPENSSL_DIR 			:= $(IPNC_INSTALL_DIR)/opensource/openssl/local-arm-$(TOOLCHAINI_VERSION)
NGINX_DIR					:= $(THIRD_PARTY_BASE_DIR)/nginx
NGINX_BIN_DIR			:= $(NGINX_DIR)/_build_$(TOOLCHAINI_VERSION)/http/sbin
DRM_DIR						:= $(THIRD_PARTY_BASE_DIR)/DRM
FFMPEG_DIR				:= $(THIRD_PARTY_BASE_DIR)/ffmpeg-2.8.13
FFMPEG_LIB_DIR		:= $(FFMPEG_DIR)/lib
FFMPEG_INC_DIR		:= $(FFMPEG_DIR)/include
SDL_DIR						:= $(THIRD_PARTY_BASE_DIR)/SDL1
SDL_LIB_DIR				:= $(SDL_DIR)/$(TOOLCHAINI_VERSION)_build/lib
SDL_INC_DIR				:= $(SDL_DIR)/$(TOOLCHAINI_VERSION)_build/include
SDL_TTF_DIR				:= $(THIRD_PARTY_BASE_DIR)/SDL_ttf
SDL_TTF_LIB_DIR		:= $(SDL_TTF_DIR)/$(TOOLCHAINI_VERSION)_build/lib
SDL_TTF_INC_DIR		:= $(SDL_TTF_DIR)/$(TOOLCHAINI_VERSION)_build/include
FREETYPE_DIR			:= $(THIRD_PARTY_BASE_DIR)/freetype
FREETYPE_LIB_DIR	:= $(FREETYPE_DIR)/$(TOOLCHAINI_VERSION)_build/lib
FREETYPE_INC_DIR	:= $(FREETYPE_DIR)/$(TOOLCHAINI_VERSION)_build/include
LUMENS_DIR				:= $(THIRD_PARTY_BASE_DIR)/Lumens
LUMENS_API_DIR		:= $(LUMENS_DIR)/lti/api
LUMENS_BIN_DIR		:= $(LUMENS_DIR)/rootfs/lti/bin
LUMENS_ROOTFS_DIR		:= $(LUMENS_DIR)/rootfs
DROPBEAR_DIR				:= $(THIRD_PARTY_BASE_DIR)/dropbear
DROPBEAR_BUILD_DIR	:= $(DROPBEAR_DIR)/local-arm-$(TOOLCHAINI_VERSION)
CHRONY_DIR				:= $(THIRD_PARTY_BASE_DIR)/chrony
CHRONY_CFG				:= $(CHRONY_DIR)
CHRONY_BIN				:= $(CHRONY_DIR)/$(TOOLCHAINI_VERSION)/usr/local/sbin
SNMP_DIR					:= $(THIRD_PARTY_BASE_DIR)/snmp
ETHTOOL_DIR				:= $(THIRD_PARTY_BASE_DIR)/ethtool
ETHTOOL_BIN				:= $(ETHTOOL_DIR)/$(TOOLCHAINI_VERSION)/sbin
################################################################################
# All EXPORT symbols are defined here
################################################################################
export IPNC_DIR
export TARA_API_DIR
export TARA_API_LIB
export TARA_API_INC
export IPNC_EXEC_DIR
export TOOLCHAIN_INSTALL_DIR
export CROSS_GCC
export PUBLIC_INCLUDE_DIR
export APP_LIB_DIR
export TARGET_DIR
export TARGET_FS
export PACIDAL_FS
export KERNELDIR
export UBOOTDIR
export MPP_BASE_DIR
export PCTOOLDIR
export OPENSSL_DIR
export ITE_HW_VER
export SENSOR_ID
export KERNEL_CFG
export BUILD_CFG
export SUBVER
export TARA_VER
export SDK_VER
export BOARD
export MFR
export MFR_ID
export HTML
export SENSOR
export CHIP
export CHIP_TYPE_ID
export CHIP_NAME
export INIPARSER_LIB_DIR
export INIPARSER_SRC_DIR
export UPNP_LIB_DIR
export UPNP_INC_DIR
export ZLIB_BUILD_DIR
export FCGI_INC_DIR
export FCGI_LIB_DIR
export FCGI_BIN_DIR
export SPAWN_FCGI_BIN_DIR 
export NGINX_BIN_DIR
export DRM_DIR
export MODEL
export FFMPEG_LIB_DIR
export FFMPEG_INC_DIR
export SDL_LIB_DIR
export SDL_INC_DIR
export SDL_TTF_LIB_DIR
export SDL_TTF_INC_DIR
export FREETYPE_LIB_DIR
export LUMENS_API_DIR
export LUMENS_BIN_DIR
export LUMENS_ROOTFS_DIR
export FUNC
export MODEL_NAME
export SNS
export TOOLCHAINI_VERSION
export DROPBEAR_BUILD_DIR
export CHRONY_CFG
export CHRONY_BIN
export SNMP_DIR
export TARAAPI_VER
export ETHTOOL_BIN
