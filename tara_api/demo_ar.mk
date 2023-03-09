#INC=-I $(SRCROOT)/libs/pool
CC=arm-linux-gnueabi-gcc
STRIP=arm-linux-gnueabi-strip
INC=-I./include
CFLAGS+=$(INC)

EXES=sample_video

all: $(EXES)

sample_get_jpeg_by_time : sample_get_jpeg_by_time.c
	$(CC) -o $@ $(CFLAGS) sample_get_jpeg_by_time.c -L./lib -ltara
	$(STRIP) $@

sample_get_jpeg_by_cmd : sample_get_jpeg_by_cmd.c
	$(CC) -o $@ $(CFLAGS) sample_get_jpeg_by_cmd.c -L./lib -ltara
	$(STRIP) $@

sample_get_frame_by_cmd : sample_get_frame_by_cmd.c
	$(CC) -o $@ $(CFLAGS) sample_get_frame_by_cmd.c -L./lib -ltara
	$(STRIP) $@

clean :
	rm -rf $(EXES)
