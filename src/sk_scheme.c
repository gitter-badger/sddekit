/* Apache 2.0 INS-AMU 2015 */

#include <math.h>

#include <stdlib.h>

#include "sk_scheme.h"
#include "sk_util.h"

int sk_sch_em_init(sk_sch_em_data *d, int nx)
{
	SK_MALLOCHECK(d->f=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->g=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->z=malloc(sizeof(double)*nx));
	return 0;
}

void sk_sch_em_free(sk_sch_em_data *d)
{
	free(d->f);
	free(d->g);
	free(d->z);
}

SK_DEFSCH(sk_sch_em)
{
	int i;
	double sqrt_dt;
	sk_sch_em_data *d = data;
	(*sys)(sysd, t, nx, x, d->f, d->g, NULL, NULL, nc, c);
	sk_util_fill_gauss(rng, nx, d->z);
	sqrt_dt = sqrt(dt);
	for (i=0; i<nx; i++)
		x[i] += dt * d->f[i] + sqrt_dt * d->g[i] * d->z[i];
	return 0;
}

int sk_sch_emcolor_init(sk_sch_emcolor_data *d, int nx, double lam)
{
	SK_MALLOCHECK(d->f=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->g=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->z=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->eps=malloc(sizeof(double)*nx));
	d->first_call = 1;
	d->lam = lam;
	return 0;
}

void sk_sch_emcolor_free(sk_sch_emcolor_data *d)
{
	free(d->f);
	free(d->g);
	free(d->z);
	free(d->eps);
}

SK_DEFSCH(sk_sch_emcolor)
{
	int i;
	double E; /* not stored so can be chaned while running */
	sk_sch_emcolor_data *d = data;
	if (d->first_call) {
		sk_util_fill_gauss(rng, nx, d->z);
		(*sys)(sysd, t-dt, nx, x, d->f, d->g, NULL, NULL, nc, c);
		for (i=0; i<nx; i++)
			d->eps[i] = sqrt(d->g[i] * d->lam) * d->z[i];
		d->first_call = 0;
	}
	E = exp(-d->lam * dt);
	sk_util_fill_gauss(rng, nx, d->z);
	(*sys)(sysd, t, nx, x, d->f, d->g, NULL, NULL, nc, c);
	for (i=0; i<nx; i++) {
		x[i] += dt * (d->f[i] + d->eps[i]);
		d->eps[i] *= E;
		d->eps[i] += sqrt(d->g[i] * d->lam * (1 - E*E)) * d->z[i];
	}
	return 0;
}


int sk_sch_heun_init(sk_sch_heun_data *d, int nx)
{
	SK_MALLOCHECK(d->fl=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->fr=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->gl=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->gr=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->z=malloc(sizeof(double)*nx));
	SK_MALLOCHECK(d->xr=malloc(sizeof(double)*nx));
	return 0;
}

void sk_sch_heun_free(sk_sch_heun_data *d)
{
	free(d->fl);
	free(d->fr);
	free(d->gl);
	free(d->gr);
	free(d->z);
	free(d->xr);
}

SK_DEFSCH(sk_sch_heun)
{
	int i;
	double sqrt_dt;
	sk_sch_heun_data *d = data;
	(*sys)(sysd, t, nx, x, d->fl, d->gl, NULL, NULL, nc, c);
	for (i=0; i<nx; i++)
		d->xr[i] = x[i] + dt * d->fl[i];
	(*sys)(sysd, t, nx, d->xr, d->fr, d->gr, NULL, NULL, nc, c);
	sk_util_fill_gauss(rng, nx, d->z);
	sqrt_dt = sqrt(dt);
	for (i=0; i<nx; i++)
		x[i] += 0.5 * (dt*(d->fl[i] + d->fr[i]) 
				+ sqrt_dt*(d->gl[i] + d->gr[i])*d->z[i]);
	return 0;
}