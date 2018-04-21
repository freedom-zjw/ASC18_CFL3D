#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// TODO: Move this macro to a head file and share among all C files
#define FTYPE float

FTYPE *ax = NULL, *ay = NULL, *az = NULL, *area = NULL;
FTYPE *at = NULL, *qr = NULL, *ql = NULL, *f    = NULL;

void c_fhat_hookptr_(
	FTYPE *_ax, FTYPE *_ay, FTYPE *_az, FTYPE *_area, 
	FTYPE *_at, FTYPE *_qr, FTYPE *_ql, FTYPE *_f
)
{
	ax   = _ax;
	ay   = _ay;
	az   = _az;
	area = _area;
	at   = _at;
	qr   = _qr;
	ql   = _ql;
	f    = _f;
}

void c_fhat_(
	int *_n, int *_iexp, FTYPE *_gamma, FTYPE *_gm1, FTYPE *_gp1, FTYPE *_gm1g, 
	FTYPE *_gp1g, FTYPE *_ggm1, FTYPE *_cprec, FTYPE *_uref, FTYPE *_avn, 
	FTYPE *_epsa_l, FTYPE *_epsa_r, FTYPE *_x1, FTYPE *_c1, FTYPE *_zero
)
{
	int n    = *_n;
	int iexp = *_iexp;
	FTYPE gamma  = *_gamma;
	FTYPE gm1    = *_gm1;
	FTYPE gp1    = *_gp1;
	FTYPE gm1g   = *_gm1g;
	FTYPE gp1g   = *_gp1g;
	FTYPE ggm1   = *_ggm1;
	FTYPE cprec  = *_cprec;
	FTYPE uref   = *_uref;
	FTYPE avn    = *_avn;
	FTYPE epsa_l = *_epsa_l;
	FTYPE epsa_r = *_epsa_r;
	FTYPE x1     = *_x1;
	FTYPE c1     = *_c1;
	FTYPE zero   = *_zero;
}
