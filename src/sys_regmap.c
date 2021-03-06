/* copyright 2016 Apache 2 sddekit authors */

#include "sddekit.h"

typedef struct regmapd {
	sd_sys sys;
	uint32_t i, *n;  /* n=1 for sum instead of averaging */
} regmapd;

static sd_stat apply(sd_sys *sys, sd_sys_in *in, sd_sys_out *out)
{
	uint32_t l;
	regmapd *d = sys->ptr;
	out->f[0] = 0.0;
	out->g[0] = 0.0;
	in->x[0] = 0.0;
	for (l=0; l<in->nc; l++)
		in->x[0] += in->i[l];
	out->o[0] = in->x[0] / d->n[d->i];
	return SD_OK;
}

static sd_sys defaults = {
	.apply = &apply
};

sd_sys *
sd_regmap_new()
{
	regmapd z={0}, *d = sd_malloc(sizeof(regmapd));
	if (d == NULL)
	{
		sd_err("alloc regmapd failed.");
		return NULL;
	}
	*d = z;
	d->sys = defaults;
	d->sys.ptr = d;
	return &(d->sys);
}
