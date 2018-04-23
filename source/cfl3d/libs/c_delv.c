#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <omp.h>

// TODO: Move this macro to a head file and share among all C files
#ifdef DBLE_PRECSN
	#ifdef C_CMPLX
		#pragma message("c_delv using double complex")
		#define FTYPE double complex
	#else
		#pragma message("c_delv using double real")
		#define FTYPE double
	#endif
#else
	#ifdef C_CMPLX
		#pragma message("c_delv using float complex")
		#define FTYPE float complex
	#else
		#pragma message("c_delv using float real")
		#define FTYPE float
	#endif 
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define q_ind(i, j, k, l)  ((l)*jdim*kdim*idim    + (k)*jdim*kdim   + (j)*jdim  + (i))
#define ux_ind(i, j, k, l) ((l)*jdim1*kdim1*idim1 + (k)*jdim1*kdim1 + (j)*jdim1 + (i))

#define sj_ind(i, j, k, l) ((l)*jdim*kdim*idim1 + (k)*jdim*kdim + (j)*jdim + (i))
#define sk_ind(i, j, k, l) ((l)*jdim*kdim*idim1 + (k)*jdim*kdim + (j)*jdim + (i))
#define si_ind(i, j, k, l) ((l)*jdim*kdim*idim  + (k)*jdim*kdim + (j)*jdim + (i))

#define vol_ind(i, j, k)   ((k)*jdim*kdim + (j)*jdim + (i))
#define blank_ind(i, j, k) ((k)*jdim*kdim + (j)*jdim + (i))
#define wt_ind(i, j, k)    ((k)*jdim*kdim + (j)*jdim + (i))

#define bcj_ind(i, j, k) ((k)*kdim*idim1 + (j)*kdim + (i))
#define bck_ind(i, j, k) ((k)*jdim*idim1 + (j)*jdim + (i))
#define bci_ind(i, j, k) ((k)*jdim*kdim  + (j)*jdim + (i))

#define qj0_ind(i, j, k, l) ((l)*kdim*idim1*5 + (k)*kdim*idim1 + (j)*kdim + (i))
#define qk0_ind(i, j, k, l) ((l)*jdim*idim1*5 + (k)*jdim*idim1 + (j)*jdim + (i))
#define qi0_ind(i, j, k, l) ((l)*jdim*kdim*5  + (k)*jdim*kdim  + (j)*jdim + (i))

#define volj0_ind(i, j, k) ((k)*kdim*idim1 + (j)*kdim + (i))
#define volk0_ind(i, j, k) ((k)*jdim*idim1 + (j)*jdim + (i))
#define voli0_ind(i, j, k) ((k)*jdim*kdim  + (j)*jdim + (i))

static int jdim, kdim, idim, iover, nbl, maxbl, i2d, jdim1, kdim1, idim1;
static int *ivmax, *jvmax, *kvmax;
static FTYPE *q, *sj, *sk, *si, *vol, *ux, *wt, *blank, *qj0, *qk0, *qi0;
static FTYPE *bcj, *bck, *bci, *volj0, *volk0, *voli0, *vormax;

// J-direction contribution
static void J_direction()
{
	for (int i = 0; i < idim1; i++)
	{
		for (int k = 0; k < kdim1; k++)
		{
			int wt_base1 = wt_ind(0, 0, 0);
			int wt_base2 = wt_ind(0, 0, 1);
			int wt_base3 = wt_ind(0, 0, 2);
			int wt_base4 = wt_ind(0, 0, 3);
			int wt_base5 = wt_ind(0, 0, 4);
			int wt_base6 = wt_ind(0, 0, 5);
			int wt_base7 = wt_ind(0, 0, 6);
			int wt_base8 = wt_ind(0, 0, 7);
			int wt_base9 = wt_ind(0, 0, 8);
			
			int q_base2  = q_ind(0, k, i, 1);
			int q_base3  = q_ind(0, k, i, 2);
			int q_base4  = q_ind(0, k, i, 3);
			
			int sj_base1 = sj_ind(0, k, i, 0);
			int sj_base2 = sj_ind(0, k, i, 1);
			int sj_base3 = sj_ind(0, k, i, 2);
			int sj_base4 = sj_ind(0, k, i, 3);
			
			int vol_base = vol_ind(0, k, i);
			
			// Cycle through interfaces
			int j;
			FTYPE term, factor;
			for (j = 1; j < jdim1; j++)
			{
				term = sj[sj_base4+j] / (vol[vol_base+j] + vol[vol_base+j-1]);
				wt[wt_base1+j] = term * ((q[q_base2+j] - q[q_base2+j-1]) * sj[sj_base1+j]);
				wt[wt_base2+j] = term * ((q[q_base2+j] - q[q_base2+j-1]) * sj[sj_base2+j]);
				wt[wt_base3+j] = term * ((q[q_base2+j] - q[q_base2+j-1]) * sj[sj_base3+j]);
				wt[wt_base4+j] = term * ((q[q_base3+j] - q[q_base3+j-1]) * sj[sj_base1+j]);
				wt[wt_base5+j] = term * ((q[q_base3+j] - q[q_base3+j-1]) * sj[sj_base2+j]);
				wt[wt_base6+j] = term * ((q[q_base3+j] - q[q_base3+j-1]) * sj[sj_base3+j]);
				wt[wt_base7+j] = term * ((q[q_base4+j] - q[q_base4+j-1]) * sj[sj_base1+j]);
				wt[wt_base8+j] = term * ((q[q_base4+j] - q[q_base4+j-1]) * sj[sj_base2+j]);
				wt[wt_base9+j] = term * ((q[q_base4+j] - q[q_base4+j-1]) * sj[sj_base3+j]);
			}
			
			j = 0;
			term   = sj[sj_base4+j] / (volj0[volj0_ind(k, i, 0)] + vol[vol_base+j]);
			factor = bcj[bcj_ind(k, i, 0)] + 1.0;
			term   = term * factor;
			int qj0_idx2 = qj0_ind(k, i, 1, 0);
			int qj0_idx3 = qj0_ind(k, i, 2, 0);
			int qj0_idx4 = qj0_ind(k, i, 3, 0);
			wt[wt_base1+j] = term * ((q[q_base2+j] - qj0[qj0_idx2]) * sj[sj_base1+j]);
			wt[wt_base2+j] = term * ((q[q_base2+j] - qj0[qj0_idx2]) * sj[sj_base2+j]);
			wt[wt_base3+j] = term * ((q[q_base2+j] - qj0[qj0_idx2]) * sj[sj_base3+j]);
			wt[wt_base4+j] = term * ((q[q_base3+j] - qj0[qj0_idx3]) * sj[sj_base1+j]);
			wt[wt_base5+j] = term * ((q[q_base3+j] - qj0[qj0_idx3]) * sj[sj_base2+j]);
			wt[wt_base6+j] = term * ((q[q_base3+j] - qj0[qj0_idx3]) * sj[sj_base3+j]);
			wt[wt_base7+j] = term * ((q[q_base4+j] - qj0[qj0_idx4]) * sj[sj_base1+j]);
			wt[wt_base8+j] = term * ((q[q_base4+j] - qj0[qj0_idx4]) * sj[sj_base2+j]);
			wt[wt_base9+j] = term * ((q[q_base4+j] - qj0[qj0_idx4]) * sj[sj_base3+j]);
			
			j = jdim - 1;
			term   = sj[sj_base4+j] / (volj0[volj0_ind(k, i, 2)] + vol[vol_base+j-1]);
			factor = bcj[bcj_ind(k, i, 1)] + 1.0;
			term   = term * factor;
			qj0_idx2 = qj0_ind(k, i, 1, 2);
			qj0_idx3 = qj0_ind(k, i, 2, 2);
			qj0_idx4 = qj0_ind(k, i, 3, 2);
			wt[wt_base1+j] = term * ((qj0[qj0_idx2] - q[q_base2+j-1]) * sj[sj_base1+j]);
			wt[wt_base2+j] = term * ((qj0[qj0_idx2] - q[q_base2+j-1]) * sj[sj_base2+j]);
			wt[wt_base3+j] = term * ((qj0[qj0_idx2] - q[q_base2+j-1]) * sj[sj_base3+j]);
			wt[wt_base4+j] = term * ((qj0[qj0_idx3] - q[q_base3+j-1]) * sj[sj_base1+j]);
			wt[wt_base5+j] = term * ((qj0[qj0_idx3] - q[q_base3+j-1]) * sj[sj_base2+j]);
			wt[wt_base6+j] = term * ((qj0[qj0_idx3] - q[q_base3+j-1]) * sj[sj_base3+j]);
			wt[wt_base7+j] = term * ((qj0[qj0_idx4] - q[q_base4+j-1]) * sj[sj_base1+j]);
			wt[wt_base8+j] = term * ((qj0[qj0_idx4] - q[q_base4+j-1]) * sj[sj_base2+j]);
			wt[wt_base9+j] = term * ((qj0[qj0_idx4] - q[q_base4+j-1]) * sj[sj_base3+j]);
			
			// Cycle through cell centers
			for (int l = 0; l < 9; l++)
			{
				int ux_base = ux_ind(0, k, i, l);
				int wt_base = wt_ind(0, 0, l);
				for (j = 0; j < jdim1; j++)
				{
					FTYPE addend = wt[wt_base+j] + wt[wt_base+j+1];
					ux[ux_base+j] += addend;
				}
			}		
		}  // for (int k = 0; k < kdim1; k++)
	}  // for (int i = 0; i < idim1; i++)
}

// K-direction contributions
static void K_direction()
{
	for (int i = 0; i < idim1; i++)
	{
		// Cycle through interfaces
		for (int k = 1; k < kdim1; k++)
		{
			int wt_base1 = wt_ind(0, k, 0);
			int wt_base2 = wt_ind(0, k, 1);
			int wt_base3 = wt_ind(0, k, 2);
			int wt_base4 = wt_ind(0, k, 3);
			int wt_base5 = wt_ind(0, k, 4);
			int wt_base6 = wt_ind(0, k, 5);
			int wt_base7 = wt_ind(0, k, 6);
			int wt_base8 = wt_ind(0, k, 7);
			int wt_base9 = wt_ind(0, k, 8);
			
			int sk_base1 = sk_ind(0, k, i, 0);
			int sk_base2 = sk_ind(0, k, i, 1);
			int sk_base3 = sk_ind(0, k, i, 2);
			int sk_base4 = sk_ind(0, k, i, 3);
			
			int q_base2 = q_ind(0, k, i, 1);
			int q_base3 = q_ind(0, k, i, 2);
			int q_base4 = q_ind(0, k, i, 3);
			
			int q1_base2 = q_ind(0, k-1, i, 1);
			int q1_base3 = q_ind(0, k-1, i, 2);
			int q1_base4 = q_ind(0, k-1, i, 3);
			
			int vol_base  = vol_ind(0, k, i);
			int vol1_base = vol_ind(0, k-1, i);
			
			for (int j = 0; j < jdim1; j++)
			{
				FTYPE term = sk[sk_base4+j] / (vol[vol_base+j] + vol[vol1_base+j]);
				wt[wt_base1+j] = term * ((q[q_base2+j] - q[q1_base2+j]) * sk[sk_base1+j]);
				wt[wt_base2+j] = term * ((q[q_base2+j] - q[q1_base2+j]) * sk[sk_base2+j]);
				wt[wt_base3+j] = term * ((q[q_base2+j] - q[q1_base2+j]) * sk[sk_base3+j]);
				wt[wt_base4+j] = term * ((q[q_base3+j] - q[q1_base3+j]) * sk[sk_base1+j]);
				wt[wt_base5+j] = term * ((q[q_base3+j] - q[q1_base3+j]) * sk[sk_base2+j]);
				wt[wt_base6+j] = term * ((q[q_base3+j] - q[q1_base3+j]) * sk[sk_base3+j]);
				wt[wt_base7+j] = term * ((q[q_base4+j] - q[q1_base4+j]) * sk[sk_base1+j]);
				wt[wt_base8+j] = term * ((q[q_base4+j] - q[q1_base4+j]) * sk[sk_base2+j]);
				wt[wt_base9+j] = term * ((q[q_base4+j] - q[q1_base4+j]) * sk[sk_base3+j]);
			}
		}  // for (int k = 1; k < kdim1; k++)
		
		int k = 0;
		
		int wt_base1 = wt_ind(0, k, 0);
		int wt_base2 = wt_ind(0, k, 1);
		int wt_base3 = wt_ind(0, k, 2);
		int wt_base4 = wt_ind(0, k, 3);
		int wt_base5 = wt_ind(0, k, 4);
		int wt_base6 = wt_ind(0, k, 5);
		int wt_base7 = wt_ind(0, k, 6);
		int wt_base8 = wt_ind(0, k, 7);
		int wt_base9 = wt_ind(0, k, 8);
		
		int sk_base1 = sk_ind(0, k, i, 0);
		int sk_base2 = sk_ind(0, k, i, 1);
		int sk_base3 = sk_ind(0, k, i, 2);
		int sk_base4 = sk_ind(0, k, i, 3);
		
		int q_base2 = q_ind(0, k, i, 1);
		int q_base3 = q_ind(0, k, i, 2);
		int q_base4 = q_ind(0, k, i, 3);
		
		int qk0_base2 = qk0_ind(0, i, 1, 0);
		int qk0_base3 = qk0_ind(0, i, 2, 0);
		int qk0_base4 = qk0_ind(0, i, 3, 0);
		
		int vol_base   = vol_ind(0, k, i);
		int volk0_base = volk0_ind(0, i, 0);
		int bck_base   = bck_ind(0, i, 0);
		
		for (int j = 0; j < jdim1; j++)
		{
			FTYPE term   = sk[sk_base4+j] / (volk0[volk0_base+j] + vol[vol_base+j]);
			FTYPE factor = bck[bck_base+j] + 1.0;
			term *= factor;
			wt[wt_base1+j] = term * ((q[q_base2+j] - qk0[qk0_base2+j]) * sk[sk_base1+j]);
			wt[wt_base2+j] = term * ((q[q_base2+j] - qk0[qk0_base2+j]) * sk[sk_base2+j]);
			wt[wt_base3+j] = term * ((q[q_base2+j] - qk0[qk0_base2+j]) * sk[sk_base3+j]);
			wt[wt_base4+j] = term * ((q[q_base3+j] - qk0[qk0_base3+j]) * sk[sk_base1+j]);
			wt[wt_base5+j] = term * ((q[q_base3+j] - qk0[qk0_base3+j]) * sk[sk_base2+j]);
			wt[wt_base6+j] = term * ((q[q_base3+j] - qk0[qk0_base3+j]) * sk[sk_base3+j]);
			wt[wt_base7+j] = term * ((q[q_base4+j] - qk0[qk0_base4+j]) * sk[sk_base1+j]);
			wt[wt_base8+j] = term * ((q[q_base4+j] - qk0[qk0_base4+j]) * sk[sk_base2+j]);
			wt[wt_base9+j] = term * ((q[q_base4+j] - qk0[qk0_base4+j]) * sk[sk_base3+j]);
		}
		
		
		k = kdim - 1;
		
		wt_base1 = wt_ind(0, k, 0);
		wt_base2 = wt_ind(0, k, 1);
		wt_base3 = wt_ind(0, k, 2);
		wt_base4 = wt_ind(0, k, 3);
		wt_base5 = wt_ind(0, k, 4);
		wt_base6 = wt_ind(0, k, 5);
		wt_base7 = wt_ind(0, k, 6);
		wt_base8 = wt_ind(0, k, 7);
		wt_base9 = wt_ind(0, k, 8);
		
		qk0_base2 = qk0_ind(0, i, 1, 2);
		qk0_base3 = qk0_ind(0, i, 2, 2);
		qk0_base4 = qk0_ind(0, i, 3, 2);
		
		q_base2 = q_ind(0, k-1, i, 1);
		q_base3 = q_ind(0, k-1, i, 2);
		q_base4 = q_ind(0, k-1, i, 3);
		
		sk_base1 = sk_ind(0, k, i, 0);
		sk_base2 = sk_ind(0, k, i, 1);
		sk_base3 = sk_ind(0, k, i, 2);
		sk_base4 = sk_ind(0, k, i, 3);
		
		vol_base   = vol_ind(0, k-1, i);
		volk0_base = volk0_ind(0, i, 2);
		bck_base   = bck_ind(0, i, 1);
		
		for (int j = 0; j < jdim1; j++)
		{
			FTYPE term   = sk[sk_base4+j] / (volk0[volk0_base+j] + vol[vol_base+j]);
			FTYPE factor = bck[bck_base+j] + 1.0;
			term *= factor;
			wt[wt_base1+j] = term * ((qk0[qk0_base2+j] - q[q_base2+j]) * sk[sk_base1+j]);
			wt[wt_base2+j] = term * ((qk0[qk0_base2+j] - q[q_base2+j]) * sk[sk_base2+j]);
			wt[wt_base3+j] = term * ((qk0[qk0_base2+j] - q[q_base2+j]) * sk[sk_base3+j]);
			wt[wt_base4+j] = term * ((qk0[qk0_base3+j] - q[q_base3+j]) * sk[sk_base1+j]);
			wt[wt_base5+j] = term * ((qk0[qk0_base3+j] - q[q_base3+j]) * sk[sk_base2+j]);
			wt[wt_base6+j] = term * ((qk0[qk0_base3+j] - q[q_base3+j]) * sk[sk_base3+j]);
			wt[wt_base7+j] = term * ((qk0[qk0_base4+j] - q[q_base4+j]) * sk[sk_base1+j]);
			wt[wt_base8+j] = term * ((qk0[qk0_base4+j] - q[q_base4+j]) * sk[sk_base2+j]);
			wt[wt_base9+j] = term * ((qk0[qk0_base4+j] - q[q_base4+j]) * sk[sk_base3+j]);
		}
		
		// Cycle through cell centers
		for (int l = 0; l < 9; l++)
		{
			for (int k = 0; k < kdim1; k++)
			{
				int ux_base  = ux_ind(0, k, i, l);
				int wt_base  = wt_ind(0, k, l);
				int wt_base1 = wt_ind(0, k+1, l);
				for (int j = 0; j < jdim1; j++)
				{
					FTYPE addend = wt[wt_base+j] + wt[wt_base1+j];
					ux[ux_base+j] += addend;
				}
			}
		} // for (int l = 0; l < 9; l++)
	}  // for (int i = 0; i < idim1; i++)
}

// I-direction contributions
static void I_direction()
{
	if (i2d != 0) return;
	
	// Be careful, don't run out of registers
	
	for (int i = 1; i < idim1; i++)
	{
		for (int k = 0; k < kdim1; k++)
		{
			int wt_base1 = wt_ind(0, k, 0);
			int wt_base2 = wt_ind(0, k, 1);
			int wt_base3 = wt_ind(0, k, 2);
			int wt_base4 = wt_ind(0, k, 3);
			int wt_base5 = wt_ind(0, k, 4);
			int wt_base6 = wt_ind(0, k, 5);
			int wt_base7 = wt_ind(0, k, 6);
			int wt_base8 = wt_ind(0, k, 7);
			int wt_base9 = wt_ind(0, k, 8);
			
			int q_base2 = q_ind(0, k, i, 1);
			int q_base3 = q_ind(0, k, i, 2);
			int q_base4 = q_ind(0, k, i, 3);
			int qb_delt = q_ind(0, 0, 1, 0);
			
			int si_base1 = si_ind(0, k, i, 0);
			int si_base2 = si_ind(0, k, i, 1);
			int si_base3 = si_ind(0, k, i, 2);
			int si_base4 = si_ind(0, k, i, 3);
			
			int ux_base1 = ux_ind(0, k, i, 0);
			int ux_base2 = ux_ind(0, k, i, 1);
			int ux_base3 = ux_ind(0, k, i, 2);
			int ux_base4 = ux_ind(0, k, i, 3);
			int ux_base5 = ux_ind(0, k, i, 4);
			int ux_base6 = ux_ind(0, k, i, 5);
			int ux_base7 = ux_ind(0, k, i, 6);
			int ux_base8 = ux_ind(0, k, i, 7);
			int ux_base9 = ux_ind(0, k, i, 8);
			int uxb_delt = ux_ind(0, 0, 1, 0);
			
			int vol_base  = vol_ind(0, k, i);
			int vol_base1 = vol_ind(0, k, i-1);
			
			for (int j = 0; j < jdim1; j++)
			{
				FTYPE term = si[si_base4+j] / (vol[vol_base+j] + vol[vol_base1+j]);
				wt[wt_base1+j] = term * ((q[q_base2+j] - q[q_base2-qb_delt+j]) * si[si_base1+j]);
				wt[wt_base2+j] = term * ((q[q_base2+j] - q[q_base2-qb_delt+j]) * si[si_base2+j]);
				wt[wt_base3+j] = term * ((q[q_base2+j] - q[q_base2-qb_delt+j]) * si[si_base3+j]);
				wt[wt_base4+j] = term * ((q[q_base3+j] - q[q_base3-qb_delt+j]) * si[si_base1+j]);
				wt[wt_base5+j] = term * ((q[q_base3+j] - q[q_base3-qb_delt+j]) * si[si_base2+j]);
				wt[wt_base6+j] = term * ((q[q_base3+j] - q[q_base3-qb_delt+j]) * si[si_base3+j]);
				wt[wt_base7+j] = term * ((q[q_base4+j] - q[q_base4-qb_delt+j]) * si[si_base1+j]);
				wt[wt_base8+j] = term * ((q[q_base4+j] - q[q_base4-qb_delt+j]) * si[si_base2+j]);
				wt[wt_base9+j] = term * ((q[q_base4+j] - q[q_base4-qb_delt+j]) * si[si_base3+j]);
				
				ux[ux_base1-uxb_delt+j] += wt[wt_base1+j];
				ux[ux_base2-uxb_delt+j] += wt[wt_base2+j];
				ux[ux_base3-uxb_delt+j] += wt[wt_base3+j];
				ux[ux_base4-uxb_delt+j] += wt[wt_base4+j];
				ux[ux_base5-uxb_delt+j] += wt[wt_base5+j];
				ux[ux_base6-uxb_delt+j] += wt[wt_base6+j];
				ux[ux_base7-uxb_delt+j] += wt[wt_base7+j];
				ux[ux_base8-uxb_delt+j] += wt[wt_base8+j];
				ux[ux_base9-uxb_delt+j] += wt[wt_base9+j];
				
				ux[ux_base1+j] += wt[wt_base1+j];
				ux[ux_base2+j] += wt[wt_base2+j];
				ux[ux_base3+j] += wt[wt_base3+j];
				ux[ux_base4+j] += wt[wt_base4+j];
				ux[ux_base5+j] += wt[wt_base5+j];
				ux[ux_base6+j] += wt[wt_base6+j];
				ux[ux_base7+j] += wt[wt_base7+j];
				ux[ux_base8+j] += wt[wt_base8+j];
				ux[ux_base9+j] += wt[wt_base9+j];
			}  // for (int j = 0; j < jdim1; j++)
		}  // for (int k = 0; k < kdim1; k++)
	}  // for (int i = 1; i < idim1; i++)
	
	int i = 0;
	for (int k = 0; k < kdim1; k++)
	{
		int wt_base1 = wt_ind(0, k, 0);
		int wt_base2 = wt_ind(0, k, 1);
		int wt_base3 = wt_ind(0, k, 2);
		int wt_base4 = wt_ind(0, k, 3);
		int wt_base5 = wt_ind(0, k, 4);
		int wt_base6 = wt_ind(0, k, 5);
		int wt_base7 = wt_ind(0, k, 6);
		int wt_base8 = wt_ind(0, k, 7);
		int wt_base9 = wt_ind(0, k, 8);
		
		int ux_base1 = ux_ind(0, k, 0, 0);
		int ux_base2 = ux_ind(0, k, 0, 1);
		int ux_base3 = ux_ind(0, k, 0, 2);
		int ux_base4 = ux_ind(0, k, 0, 3);
		int ux_base5 = ux_ind(0, k, 0, 4);
		int ux_base6 = ux_ind(0, k, 0, 5);
		int ux_base7 = ux_ind(0, k, 0, 6);
		int ux_base8 = ux_ind(0, k, 0, 7);
		int ux_base9 = ux_ind(0, k, 0, 8);
		
		int q_base2  = q_ind(0, k, 0, 1);
		int q_base3  = q_ind(0, k, 0, 2);
		int q_base4  = q_ind(0, k, 0, 3);
		
		int qi0_base2 = qi0_ind(0, k, 1, 0);
		int qi0_base3 = qi0_ind(0, k, 2, 0);
		int qi0_base4 = qi0_ind(0, k, 3, 0);
		
		int si_base1 = si_ind(0, k, 0, 0);
		int si_base2 = si_ind(0, k, 0, 1);
		int si_base3 = si_ind(0, k, 0, 2);
		int si_base4 = si_ind(0, k, 0, 3);
		
		int voli0_base = voli0_ind(0, k, 0);
		int vol_base   = vol_ind(0, k, 0);
		int bci_base   = bci_ind(0, k, 0);
		
		for (int j = 0; j < jdim1; j++)
		{
			FTYPE term   = si[si_base4+j] / (voli0[voli0_base+j] + vol[vol_base+j]);
			FTYPE factor = bci[bci_base+j] + 1.0;
			term *= factor;
			wt[wt_base1+j] = term * ((q[q_base2+j] - qi0[qi0_base2+j]) * si[si_base1+j]);
			wt[wt_base2+j] = term * ((q[q_base2+j] - qi0[qi0_base2+j]) * si[si_base2+j]);
			wt[wt_base3+j] = term * ((q[q_base2+j] - qi0[qi0_base2+j]) * si[si_base3+j]);
			wt[wt_base4+j] = term * ((q[q_base3+j] - qi0[qi0_base3+j]) * si[si_base1+j]);
			wt[wt_base5+j] = term * ((q[q_base3+j] - qi0[qi0_base3+j]) * si[si_base2+j]);
			wt[wt_base6+j] = term * ((q[q_base3+j] - qi0[qi0_base3+j]) * si[si_base3+j]);
			wt[wt_base7+j] = term * ((q[q_base4+j] - qi0[qi0_base4+j]) * si[si_base1+j]);
			wt[wt_base8+j] = term * ((q[q_base4+j] - qi0[qi0_base4+j]) * si[si_base2+j]);
			wt[wt_base9+j] = term * ((q[q_base4+j] - qi0[qi0_base4+j]) * si[si_base3+j]);
			ux[ux_base1+j] += wt[wt_base1+j];
			ux[ux_base2+j] += wt[wt_base2+j];
			ux[ux_base3+j] += wt[wt_base3+j];
			ux[ux_base4+j] += wt[wt_base4+j];
			ux[ux_base5+j] += wt[wt_base5+j];
			ux[ux_base6+j] += wt[wt_base6+j];
			ux[ux_base7+j] += wt[wt_base7+j];
			ux[ux_base8+j] += wt[wt_base8+j];
			ux[ux_base9+j] += wt[wt_base9+j];
		}  // for (int j = 0; j < jdim1; j++)
	}  // for (int k = 0; k < kdim1; k++)
	
	i = idim - 1;
	for (int k = 0; k < kdim1; k++)
	{
		int wt_base1 = wt_ind(0, k, 0);
		int wt_base2 = wt_ind(0, k, 1);
		int wt_base3 = wt_ind(0, k, 2);
		int wt_base4 = wt_ind(0, k, 3);
		int wt_base5 = wt_ind(0, k, 4);
		int wt_base6 = wt_ind(0, k, 5);
		int wt_base7 = wt_ind(0, k, 6);
		int wt_base8 = wt_ind(0, k, 7);
		int wt_base9 = wt_ind(0, k, 8);
		
		int ux_base1 = ux_ind(0, k, idim1-1, 0);
		int ux_base2 = ux_ind(0, k, idim1-1, 1);
		int ux_base3 = ux_ind(0, k, idim1-1, 2);
		int ux_base4 = ux_ind(0, k, idim1-1, 3);
		int ux_base5 = ux_ind(0, k, idim1-1, 4);
		int ux_base6 = ux_ind(0, k, idim1-1, 5);
		int ux_base7 = ux_ind(0, k, idim1-1, 6);
		int ux_base8 = ux_ind(0, k, idim1-1, 7);
		int ux_base9 = ux_ind(0, k, idim1-1, 8);
		
		int q_base2  = q_ind(0, k, idim1-1, 1);
		int q_base3  = q_ind(0, k, idim1-1, 2);
		int q_base4  = q_ind(0, k, idim1-1, 3);
		
		int qi0_base2 = qi0_ind(0, k, 1, 2);
		int qi0_base3 = qi0_ind(0, k, 2, 2);
		int qi0_base4 = qi0_ind(0, k, 3, 2);
		
		int si_base1 = si_ind(0, k, idim1, 0);
		int si_base2 = si_ind(0, k, idim1, 1);
		int si_base3 = si_ind(0, k, idim1, 2);
		int si_base4 = si_ind(0, k, idim1, 3);
		
		int voli0_base = voli0_ind(0, k, 2);
		int vol_base   = vol_ind(0, k, idim1-1);
		int bci_base   = bci_ind(0, k, 1);
		
		for (int j = 0; j < jdim1; j++)
		{
			FTYPE term   = si[si_base4+j] / (voli0[voli0_base+j] + vol[vol_base+j]);
			FTYPE factor = bci[bci_base+j] + 1.0;
			term *= factor;
			wt[wt_base1+j] = term * ((qi0[qi0_base2+j] - q[q_base2+j]) * si[si_base1+j]);
			wt[wt_base2+j] = term * ((qi0[qi0_base2+j] - q[q_base2+j]) * si[si_base2+j]);
			wt[wt_base3+j] = term * ((qi0[qi0_base2+j] - q[q_base2+j]) * si[si_base3+j]);
			wt[wt_base4+j] = term * ((qi0[qi0_base3+j] - q[q_base3+j]) * si[si_base1+j]);
			wt[wt_base5+j] = term * ((qi0[qi0_base3+j] - q[q_base3+j]) * si[si_base2+j]);
			wt[wt_base6+j] = term * ((qi0[qi0_base3+j] - q[q_base3+j]) * si[si_base3+j]);
			wt[wt_base7+j] = term * ((qi0[qi0_base4+j] - q[q_base4+j]) * si[si_base1+j]);
			wt[wt_base8+j] = term * ((qi0[qi0_base4+j] - q[q_base4+j]) * si[si_base2+j]);
			wt[wt_base9+j] = term * ((qi0[qi0_base4+j] - q[q_base4+j]) * si[si_base3+j]);
			ux[ux_base1+j] += wt[wt_base1+j];
			ux[ux_base2+j] += wt[wt_base2+j];
			ux[ux_base3+j] += wt[wt_base3+j];
			ux[ux_base4+j] += wt[wt_base4+j];
			ux[ux_base5+j] += wt[wt_base5+j];
			ux[ux_base6+j] += wt[wt_base6+j];
			ux[ux_base7+j] += wt[wt_base7+j];
			ux[ux_base8+j] += wt[wt_base8+j];
			ux[ux_base9+j] += wt[wt_base9+j];
		}  // for (int j = 0; j < jdim1; j++)
	}  // for (int k = 0; k < kdim1; k++)
}

// Set velocity derivatives for hole cells to one
static void hole_to_1()
{
	if (iover != 1) return;
	
	for (int nk = 0; nk < 9; nk++)
	{
		for (int i = 0; i < idim1; i++)
		{
			for (int k = 0; k < kdim1; k++)
			{
				int ux_base = ux_ind(0, k, i, nk);
				int blank_base = blank_ind(0, k, i);
				for (int j = 0; j < jdim1; j++)
				{
					if (creal(blank[blank_base + j]) == 0.0) 
						ux[ux_base + j] = 1.0;
				}
			}
		}
	}
}

static int c_delv_cnt = 0;

void c_delv_(
	int *_jdim,  int *_kdim,  int *_idim,  int *_iover, int *_nbl,
	int *_maxbl, int *_i2d,   int *_ivmax, int *_jvmax, int *_kvmax, 
	FTYPE *_q,     FTYPE *_sj,    FTYPE *_sk,    FTYPE *_si,  FTYPE *_vol,
	FTYPE *_ux,    FTYPE *_wt,    FTYPE *_blank, FTYPE *_qj0, FTYPE *_qk0,
	FTYPE *_qi0,   FTYPE *_bcj,   FTYPE *_bck,   FTYPE *_bci, FTYPE *_volj0,
	FTYPE *_volk0, FTYPE *_voli0, FTYPE *_vormax
)
{
	if (c_delv_cnt == 0) printf("Using c_delv\n"); fflush(stdout);
	
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
	
	jdim1 = jdim - 1;
	kdim1 = kdim - 1;
	idim1 = idim - 1;
	
	memset(ux, 0, sizeof(FTYPE) * idim1 * kdim1 * jdim1 * 9);

	// J-direction contribution
	J_direction();

	// K-direction contributions
	K_direction();
	
	// I-direction contributions
	I_direction();
	
	// Set velocity derivatives for hole cells to one
	hole_to_1();

	c_delv_cnt++;
}
