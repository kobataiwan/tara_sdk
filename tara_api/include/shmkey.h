#ifndef _SHM_KEY_H_
#define _SHM_KEY_H_

#define VIDEO_ENCODER_STREAM1_KEY(cam) ((key_t)0x10000 + (cam))
#define VIDEO_ENCODER_STREAM2_KEY(cam) ((key_t)0x11000 + (cam))
#define VIDEO_ENCODER_STREAM3_KEY(cam) ((key_t)0x12000 + (cam))  // reserved
#define VIDEO_JPEG_STREAM1_KEY(cam)    ((key_t)0x20000 + (cam))
#define VIDEO_JPEG_STREAM2_KEY(cam)    ((key_t)0x21000 + (cam))  // reserved
#define VIDEO_RAW_STREAM1_KEY(cam)     ((key_t)0x30000 + (cam))
#define VIDEO_RAW_STREAM2_KEY(cam)     ((key_t)0x31000 + (cam))  // reserved

#define VIDEO_MAIN_STREAM_KEY(cam)     VIDEO_ENCODER_STREAM1_KEY(cam)
#define VIDEO_SUB_STREAM_KEY(cam)      VIDEO_ENCODER_STREAM2_KEY(cam)
#define VIDEO_MJPG_STREAM_KEY(cam)     VIDEO_JPEG_STREAM1_KEY(cam)
#define VIDEO_MAINRAW_STREAM_KEY(cam)  VIDEO_RAW_STREAM1_KEY(cam)
#define VIDEO_SUBRAW_STREAM_KEY(cam)   VIDEO_RAW_STREAM2_KEY(cam)
#define VIDEO_MJPGRAW_STREAM_KEY(cam)  ((key_t)0x23000 + (cam))  // !!! //
#define AUDIO_STREAM_KEY(cam)          ((key_t)0x1A000 + (cam))  // !!! //


#define VIDEO_SOURCE_KEY               0x40000
#define PERSISTENCE_KEY                0x50000
#define VIDEO1_ENCODER_KEY             0x60000
#define VIDEO2_ENCODER_KEY             0x60001
#define MJPEG1_ENCODER_KEY             0x60002
#define INFO_KEY                       0x70000
#define FEATURE_KEY                    0x80000
#define NETWORK_KEY                    0x90000
#define TIME_KEY                       0xA0000
#define MOTION_KEY                     0xB0000
#define LIGHTING_KEY                   0xC0000
#define POWER_KEY                      0xD0000

#endif
