include config.mak

SRCROOT=..

CFLAGS+=$(INC)

#LIBTARAVIDEOSOURCEOBJS=tara_lib.o tara_video_source.o tara_persistence.o tara_video_frame.o tara_video_encoder.o tara_info.o
LIBTARAVIDEOSOURCEOBJS=tara_lib.o tara_video_source.o tara_persistence.o tara_video_encoder.o tara_info.o
LIBTARAVIDEOSOURCEOBJS+=tara_feature.o
LIBTARAVIDEOSOURCEOBJS+=tara_network.o
LIBTARAVIDEOSOURCEOBJS+=tara_time.o
LIBTARAVIDEOSOURCEOBJS+=tara_sys.o
LIBTARAVIDEOSOURCEOBJS+=tara_accounts.o
LIBTARAVIDEOSOURCEOBJS+=tara_motion.o
LIBTARAVIDEOSOURCEOBJS+=tara_utc.o
LIBTARAVIDEOSOURCEOBJS+=tara_log.o
LIBTARAVIDEOSOURCEOBJS+=tara_lighting.o
LIBTARAVIDEOSOURCEOBJS+=tara_power.o

OBJS=$(LIBTARAVIDEOSOURCEOBJS)

all: $(OBJS) $(SLIBS) $(EXES)

tara_lib.o :
	$(CC) -g -c -fPIC tara_lib.c $(CFLAGS)

tara_video_source.o :
	$(CC) -g -c -fPIC tara_video_source.c $(CFLAGS)

tara_persistence.o :
	$(CC) -g -c -fPIC tara_persistence.c $(CFLAGS)

#tara_video_frame.o :
#	$(CC) -g -fPIC -Wall -c tara_video_frame.c $(CFLAGS)

tara_video_encoder.o :
	$(CC) -g -c -fPIC tara_video_encoder.c $(CFLAGS)

tara_info.o :
	$(CC) -g -c -fPIC tara_info.c $(CFLAGS)

tara_feature.o :
	$(CC) -g -c -fPIC tara_feature.c $(CFLAGS)

tara_network.o :
	$(CC) -g -c -fPIC tara_network.c $(CFLAGS)

tara_time.o :
	$(CC) -g -c -fPIC tara_time.c $(CFLAGS)

tara_sys.o :
	$(CC) -g -c -fPIC tara_sys.c $(CFLAGS)

tara_accounts.o : tara_accounts.c
	$(CC) -g -c -fPIC tara_accounts.c $(CFLAGS)

tara_motion.o :
	$(CC) -g -c -fPIC tara_motion.c $(CFLAGS)

tara_utc.o :
	$(CC) -g -c -fPIC tara_utc.c $(CFLAGS)

tara_log.o :
	$(CC) -g -c -fPIC tara_log.c $(CFLAGS)

tara_lighting.o :
	$(CC) -g -c -fPIC tara_lighting.c $(CFLAGS)

tara_power.o :
	$(CC) -g -c -fPIC tara_power.c $(CFLAGS)

clean :
	rm -rf $(OBJS) $(SLIBS) $(EXES)
