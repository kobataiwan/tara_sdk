include Rules.make
SDKREL_PATH=$(PWD)

SUBDIRS = tara_api $(CHIP_NAME)/av_server
DEBUGSUBDIRS = $(addsuffix .debug, $(SUBDIRS))
RELEASESUBDIRS = $(addsuffix .release, $(SUBDIRS))
CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)

#OSDRV_CROSS = arm-hisiv500-linux
OSDRV_CROSS = arm-linux-gnueabi
all: 
	make clean
	make apps

apps:
	make -C $(IPNC_INSTALL_DIR)/ipnc_app/interface
	make -C $(IPNC_INSTALL_DIR)/ipnc_app/network
	make -C $(IPNC_INSTALL_DIR)/tara_api
	make -C $(IPNC_INSTALL_DIR)/$(CHIP_NAME)/av_server


install: $(INSTALLSUBDIRS)

$(DEBUGSUBDIRS):
	@echo
	@echo Executing make debug in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) debug

debug:	$(DEBUGSUBDIRS)

$(RELEASESUBDIRS):
	@echo
	@echo Executing make release in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) release

release:	$(RELEASESUBDIRS)

$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) install

clean:	$(CLEANSUBDIRS)
	make -f $(IPNC_INSTALL_DIR)/$(CHIP_NAME)/soc_wraper/hiapi.mk clean
	make -C $(IPNC_INSTALL_DIR)/ipnc_app/network clean
	make -C $(IPNC_INSTALL_DIR)/ipnc_app/interface clean
	
depend:
	
	
$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
