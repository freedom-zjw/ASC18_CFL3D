#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <omp.h>

// TODO: Move this macro to a head file and share among all C files
// TODO: Use different math functions (abs, pow, ...) for different types
#define FTYPE float
#define SQRT  sqrtf
#define FABS  fabsf
#define POW   powf

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int jdim, kdim, idim, iover, nbl, maxbl, i2d;
int *ivmax, *jvmax, *kvmax;
FTYPE *q, *sj, *sk, *si, *vol, *ux, *wt, *blank, *qj0, *qk0, *qi0;
FTYPE *bcj, *bck, *bci, *volj0, *volk0, *voli0, *vormax;

int iter = 0;

void c_delv_(
	int *_jdim,  int *_kdim,  int *_idim,  int *_iover, int *_nbl,
	int *_maxbl, int *_i2d,   int *_ivmax, int *_jvmax, int *_kvmax, 
	FTYPE *_q,     FTYPE *_sj,    FTYPE *_sk,    FTYPE *_si,  FTYPE *_vol,
	FTYPE *_ux,    FTYPE *_wt,    FTYPE *_blank, FTYPE *_qj0, FTYPE *_qk0,
	FTYPE *_qi0,   FTYPE *_bcj,   FTYPE *_bck,   FTYPE *_bci, FTYPE *_volj0,
	FTYPE *_volk0, FTYPE *_voli0, FTYPE *_vormax
)
{
	// Single variables
	jdim  = *_jdim;
	kdim  = *_kdim;
	idim  = *_idim;
	iover = *_iover;
	nbl   = *_nbl;
	maxbl = *_maxbl;
	i2d   = *_i2d;
	
	// Pointers
	ivmax  = _ivmax;
	jvmax  = _jvmax;
	kvmax  = _kvmax;
	q      = _q;
	sj     = _sj;
	sk     = _sk;
	si     = _si;
	vol    = _vol;
	ux     = _ux;
	wt     = _wt;
	blank  = _blank;
	qj0    = _qj0;
	qk0    = _qk0;
	qi0    = _qi0;
	bcj    = _bcj;
	bck    = _bck;
	bci    = _bci;
	volj0  = _volj0;
	volk0  = _volk0;
	voli0  = _voli0;
	vormax = _vormax;
	
	int jdim1 = jdim - 1;
	int kdim1 = kdim - 1;
	int idim1 = idim - 1;
	
	if (iter == 0)
	{
		printf("%d %d %d %d %d %d %d\n", jdim, kdim, idim, iover, nbl, maxbl, i2d);
	}
	iter++;
	assert(iter < 1);
}