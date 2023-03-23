include config.mak

SRCROOT=..

CFLAGS+=$(INC)

OBJS=myshare.o mutex.o cfg_util.o

QUEUEBINS=vqueue aqueue
QUEUEBINS=

APIBINS=libtaraapi.a

all: $(OBJS) $(APIBINS)

myshare.o :
	$(CC) -g -c -fPIC myshare.c $(CFLAGS)

mutex.o :# mutex.c mutex.h
	$(CC) -g -c -fPIC mutex.c $(CFLAGS)

cfg_util.o:
	$(CC) -g -c -fPIC cfg_util.c $(CFLAGS)

vqueue:
	make -f vqueue.mk

aqueue:
	make -f aqueue.mk

libtaraapi.a :
	mkdir -p $(LIBS_DIR)
	$(AR) -rv ./$(LIBS_DIR)/$@ myshare.o mutex.o cfg_util.o 
	ranlib ./$(LIBS_DIR)/$@

clean :
	rm -rf $(OBJS) $(APIBINS)
	rm -rf $(LIBS_DIR)
