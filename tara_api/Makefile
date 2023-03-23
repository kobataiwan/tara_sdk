include config.mak 

ifeq ($(ARCH),)
  # cross compilation, output directory is "lib"
  LIBDIR=$(TARA_API_DIR)/./lib
else
  # native compilation, otuput directory is "libnative"
  LIBDIR=$(TARA_API_DIR)/./libnative
endif

SRCROOT=..

#TARASDKOBJS=tara_video_source.o tara_persistence.o tara_video_frame.o tara_video_encoder.o tara_info.o
TARASDKOBJS=tara_video_source.o tara_persistence.o tara_video_encoder.o tara_info.o
TARASDKOBJS+=tara_feature.o
TARASDKOBJS+=tara_network.o
TARASDKOBJS+=tara_time.o
TARASDKOBJS+=tara_sys.o
TARASDKOBJS+=tara_accounts.o
TARASDKOBJS+=tara_motion.o
TARASDKOBJS+=tara_utc.o
TARASDKOBJS+=tara_log.o
TARASDKOBJS+=tara_lighting.o
TARASDKOBJS+=tara_power.o
LIBTARAVIDEOOBJS=tara_lib.o $(TARASDKOBJS)

CFLAGS+=$(INC) -mfloat-abi=hard

LIBS=taraapi tarasdk
SHAREDLIBS=libtara.so
STATICLIBS=libtara.a
SHARED_SUFFIX=.shared
STATIC_SUFFIX=.static
EXES=sample_video_source$(SHARED_SUFFIX) \
	sample_persistence$(SHARED_SUFFIX)
EXES=


all: $(LIBS) $(SHAREDLIBS) $(STATICLIBS) $(EXES)

libtara.so :
	$(CC) -shared $(CFLAGS) $(LIBTARAVIDEOOBJS) -L$(LIBDIR) -ltaraapi -o $@
	$(STRIP) $@
	cp $(SHAREDLIBS) $(LIBDIR)

libtara.a :
	cp $(LIBDIR)/libtaraapi.a $(LIBDIR)/$@
	$(AR) -r $(LIBDIR)/$@ $(LIBTARAVIDEOOBJS)

sample_video_source$(SHARED_SUFFIX): sample_video_source.c
	$(CC) -o $@ $(CFLAGS) sample_video_source.c -L$(LIBDIR) -ltara
	$(STRIP) $@

sample_persistence$(SHARED_SUFFIX): sample_persistence.c
	$(CC) -o $@ $(CFLAGS) sample_persistence.c -L$(LIBDIR) -ltara
	$(STRIP) $@

tarasdk :
	make -f tarasdk.mk

taraapi :
	make -f taraapi.mk

clean :
	rm -rf *.o *.lo *.so $(EXES) $(LIBDIR)/$(SHAREDLIBS) $(LIBDIR)/$(STATICLIBS)
	make clean -f tarasdk.mk
	make clean -f taraapi.mk
