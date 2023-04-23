#include "sample_comm.h"
#include "iniparser.h"
#include "soc_video_api.h"

#define MAX_NUM_SOC_VIDEO_INTFS		16
#define BIT(nr)                 (1 << (nr))
// TODO: use linked list instead?
struct soc_video_priv registered_soc_video[MAX_NUM_SOC_VIDEO_INTFS] = {0};

int register_soc_video_intf(char *customer, char *chip_vendor, char *chip_model, struct soc_video_ops *vops, void *userContext)
{
	int i = 0;
	struct soc_video_priv *tmp;

	for (i = 0; i < MAX_NUM_SOC_VIDEO_INTFS; i++)
	{
		tmp = &registered_soc_video[i];

		if (tmp->valid == false) {
			tmp->valid = true;
			strncpy(tmp->customer, customer, strlen(customer));
			strncpy(tmp->chip_vendor, chip_vendor, strlen(chip_vendor));
			strncpy(tmp->chip_model, chip_model, strlen(chip_model));
			tmp->video_priv = userContext;
			tmp->vops = vops;
		}
	}

	return -1;
}

struct soc_video_priv *get_soc_video_inst(char *customer, char *chip_vendor, char *chip_model)
{
	int i = 0;
	int match_bmp = 0;	/* chip_model BIT(2), chip_vendor BIT(1) */
	struct soc_video_priv *tmp;

	for (i = 0; i < MAX_NUM_SOC_VIDEO_INTFS; i++)
	{
		tmp = &registered_soc_video[i];

		if (tmp->valid == false)
			continue;

		if (!strncmp(customer, tmp->customer, strlen(customer)))
			match_bmp |= BIT(0);

		if (!strncmp(chip_vendor, tmp->chip_vendor, strlen(chip_vendor)))
			match_bmp |= BIT(1);

		if (!strncmp(chip_model, tmp->chip_model, strlen(chip_model)))
			match_bmp |= BIT(2);

		if (match_bmp > 4)	// at least match model name
			return tmp;
	}

	return NULL;
}

#if 0
struct soc_video_priv *get_hi3521d_inst(char *customer, char *chip_vendor, char *chip_model)
{
	struct soc_video_priv *hi3521d_soc = malloc(sizeof(struct soc_video_priv));
	hi3521d_soc->video_priv = (void *)&hi3521d_video_priv;
	hi3521d_soc->vops = &hi3521d_fops;

	return hi3521d_soc;
}
#endif

struct soc_video_priv *get_nt9856x_inst(char *customer, char *chip_vendor, char *chip_model)
{
	struct soc_video_priv *nt9856x_soc = malloc(sizeof(struct soc_video_priv));
	nt9856x_soc->video_priv = (void *)&nt9856x_video_priv;
	nt9856x_soc->vops = &nt9856x_fops;

	return nt9856x_soc;
}

void put_soc_video_inst(struct soc_video_priv *priv)
{
	int i = 0;

	for (i = 0; i < MAX_NUM_SOC_VIDEO_INTFS; i++)
	{
		if (priv == &registered_soc_video[i]) {
			registered_soc_video[i].valid = false;
			if (registered_soc_video[i].video_priv)
				free(registered_soc_video[i].video_priv);
		}
	}

	return;
}
