//#ifndef __SOC_VIDEO_API_H__
//#define __SOC_VIDEO_API_H__
#include <stdbool.h>
#include "tara_video_source.h"
#include "tara_video_encoder_priv.h"
#include "tara_media_stream.h"

/* Video APIs */
#define MAX_NAME_LEN	16
extern struct soc_video_priv	*video_inst;

//extern struct soc_video_ops hi3521d_fops;
//extern struct hi3521d_priv hi3521d_video_priv;

// for nt9856x
extern struct soc_video_ops nt9856x_fops;
extern struct nt9856x_priv nt9856x_video_priv;

struct soc_video_ops {
	void (*soc_init_codec_pts) (void);
	void (*soc_sync_codec_pts) (void *priv);
        int (*soc_video_load_config) (void *priv);
        int (*soc_video_init_and_bind) (void *priv);
        int (*soc_video_restart) (void *priv);
        int (*soc_video_stop) (void *priv);
	int (*soc_video_set_blank_source) (void *priv, int chan, int on);
        int (*soc_video_set_encoder) (void *priv, int chan, TARA_VIDEO_STREAM_TYPE type, TaraVideoEncoder *pvenc);
        int (*soc_video_set_mjpeg) (void *priv, int chan, TARA_VIDEO_STREAM_TYPE type, TaraMjpegEncoder *pmjpg);
        int (*soc_video_encode_start) (void *priv, TARA_VIDEO_STREAM_TYPE type, VideoEncoder_t *pvideo);
        int (*soc_mjpeg_encode_start) (void *priv, TARA_VIDEO_STREAM_TYPE type, MjpegEncoder_t *pmjpg);
	/* Begin thread start/stop */
	int (*soc_video_get_stream_start) (void *priv, int vi_num, int stream_num);
	int (*soc_video_get_stream_stop) (void *priv);
	/* RAW frame are optional */
	int (*soc_video_get_rawframe_start) (void *priv, int type);
	int (*soc_video_get_rawframe_stop) (void *priv);
	/* End thread start/stop */
        int (*soc_video_destroy) (void *priv);
};

struct soc_video_priv {
	char customer[MAX_NAME_LEN];		/* ex. AR */
	char chip_vendor[MAX_NAME_LEN];		/* ex. Hisilicon */
	char chip_model[MAX_NAME_LEN];		/* ex. Hi3521D */
	bool valid;

	void *video_priv;

	struct soc_video_ops *vops;
};

int register_soc_video_intf(char *customer, char *chip_vendor, char *chip_model, struct soc_video_ops *vops, void *userContext);

struct soc_video_priv *get_soc_video_inst(char *customer, char *chip_vendor, char *chip_model);

// test
struct soc_video_priv *get_hi3521d_inst(char *customer, char *chip_vendor, char *chip_model);
struct soc_video_priv *get_nt9856x_inst(char *customer, char *chip_vendor, char *chip_model);

void put_soc_video_inst(struct soc_video_priv *priv);
//#endif /* End of #ifndef __SOC_VIDEO_API_H__ */
