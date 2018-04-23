#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <omp.h>

// TODO: Move this macro to a head file and share among all C files
// TODO: Use different math functions (abs, pow, ...) for different types
#ifdef DBLE_PRECSN
	#pragma message("C_fhat using double type")
	#define FTYPE double
	#define SQRT  sqrt
	#define FABS  fabs
	#define POW   pow
#else
	#pragma message("C_fhat using float type")
	#define FTYPE float
	#define SQRT  sqrtf
	#define FABS  fabsf
	#define POW   powf
#endif

#define ind(i, j) ((i) + (j) * nvtq)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int n, iexp, nvtq;
static FTYPE gamma, gm1, gp1, gm1g, gp1g, ggm1, cprec;
static FTYPE uref, avn, epsa_l, epsa_r, x1, c1, zero;
static FTYPE *ax = NULL, *ay = NULL, *az = NULL, *area = NULL;
static FTYPE *at = NULL, *qr = NULL, *ql = NULL, *f    = NULL;

static void c_fhat_original()
{
	// NOTICE: i is shifted to start from 0, we can directly use them;
	// but for qr(), ql() and f(), the index of 2nd dimension need to -1.
	#pragma ivdep
	for (int i = 0; i < n; i++)
	{
		// huangh223 added local variables: qr[1-5], ql[1-5], a[x-z,t]i
		// These can avoid repeat index computation and make it convenient for coding.
		// NOTICE: if the corresponding values in the array is modified,
		// always keep these local variables up-to-date.
		FTYPE qr1 = qr[ind(i, 0)];
		FTYPE qr2 = qr[ind(i, 1)];
		FTYPE qr3 = qr[ind(i, 2)];
		FTYPE qr4 = qr[ind(i, 3)];
		FTYPE qr5 = qr[ind(i, 4)];
		
		FTYPE ql1 = ql[ind(i, 0)];
		FTYPE ql2 = ql[ind(i, 1)];
		FTYPE ql3 = ql[ind(i, 2)];
		FTYPE ql4 = ql[ind(i, 3)];
		FTYPE ql5 = ql[ind(i, 4)];
		
		FTYPE axi = ax[i];
		FTYPE ayi = ay[i];
		FTYPE azi = az[i];
		FTYPE ati = at[i];
		
		// ------ Original code translation start ------
		// ----- Comments in original code is kept -----
		
		FTYPE t1 = qr1 - ql1;
		FTYPE t2 = qr2 - ql2;
		FTYPE t3 = qr3 - ql3;
		FTYPE t4 = qr4 - ql4;
		
		// Pressure and enthalpy
		FTYPE t16 = 1.0 / qr1;
		FTYPE t5  = qr5;
		qr5 = x1 * qr5 * t16 + 0.5 * (qr2 * qr2 + qr3 * qr3 + qr4 * qr4);
		qr[ind(i, 4)] = qr5;  // Always keep qr* updated
		
		FTYPE t15 = 1.0 / ql1;
		FTYPE t19 = ql5;
		ql5 = x1 * ql5 * t15 + 0.5 * (ql2 * ql2 + ql3 * ql3 + ql4 * ql4);
		ql[ind(i, 4)] = ql5;  // Always keep ql* updated
		
		// Unsplit contributions f(r) + f(l)
		FTYPE t18 = axi * qr2 + ayi * qr3 + azi * qr4 + ati;
		FTYPE t17 = axi * ql2 + ayi * ql3 + azi * ql4 + ati;
		FTYPE t6  = t18 * qr1;
		FTYPE t7  = t17 * ql1;
		FTYPE f1  = t6 + t7;
		FTYPE f2  = t6 * qr2 + t7 * ql2;
		FTYPE f3  = t6 * qr3 + t7 * ql3;
		FTYPE f4  = t6 * qr4 + t7 * ql4;
		FTYPE f5  = t6 * qr5 + t7 * ql5;
		FTYPE t8  = t5 + t19;
		f2 += axi * t8;
		f3 += ayi * t8;
		f4 += azi * t8;
		f5 -= ati * t8;
		
		// Roe averaged variables
		t6 = qr1 * t15;
		t7 = SQRT(t6);
		t6 = 1.0 / (1.0 + t7);
		t8 = t7 * t6;
		
		// Average density
		qr1 = ql1 * t7;
		qr[ind(i, 0)] = qr1;  // Always keep qr* updated
		
		// u, v, w, h average
		FTYPE t9  = ql2 * t6 + qr2 * t8;
		FTYPE t10 = ql3 * t6 + qr3 * t8;
		FTYPE t11 = ql4 * t6 + qr4 * t8;
		FTYPE t12 = ql5 * t6 + qr5 * t8;
		
		// Extract sound speed
		t6  = (t9 * t9 + t10 * t10 + t11 * t11) * 0.5;
		t7  = gm1 * (t12 - t6);
		t8  = SQRT(t7);
		FTYPE t13 = t9 * axi + t10 * ayi + t11 * azi;
		
		// The variables in t are as follows:
		// 1-4        delta q1-q4
		// 6          q2a
		// 7          c2a
		// 8          ca
		// 9,10,11,12 ua, va, wa, ha
		// 13         ubara
		// 14,15,16   alpha(1),  alpha(4),  alpha(5)
		// 18,19,17   lambda(1), lambda(4), lambda(5)
		// rhoa*delta(ubar), delta(p)/c2a
		
		ql1 = qr1 * (t18 - t17);
		ql2 = (t5 - t19) / t7;
		ql[ind(i, 0)] = ql1;  // Always keep ql* updated
		ql[ind(i, 1)] = ql2;  // Always keep ql* updated
		
		// Update t18 and t18 = abs(t18)
		t18 = t13 + ati;
		#ifdef C_CMPLX
		if (creal(t18) < 0) t18 = -t18;
		#else
		t18 = FABS(t18);
		#endif
		
		// Update t19 and t19 = abs(t19)
		t19 = t13 + ati + t8;
		#ifdef C_CMPLX
		if (creal(t19) < 0) t19 = -t19;
		#else
		t19 = FABS(t19);
		#endif
		
		// Update t17 and t17 = abs(t17)
		t17 = t13 + ati - t8;
		#ifdef C_CMPLX
		if (creal(t17) < 0) t17 = -t17;
		#else
		t17 = FABS(t17);
		#endif
		
		// Limit eigenvalues a la Harten and Gnoffo (NASA TP-2953)
		if (creal(epsa_r) > 0) 
		{
			FTYPE cc = t8;
			FTYPE uu = t9;
			FTYPE vv = t10;
			FTYPE ww = t11;
			
			#ifdef C_CMPLX
			if (creal(uu) < 0) uu = -uu;
			if (creal(vv) < 0) vv = -vv;
			if (creal(ww) < 0) ww = -ww;
			#else
			uu = FABS(uu);
			vv = FABS(vv);
			ww = FABS(ww);
			#endif
			
			FTYPE epsaa = epsa_r * (cc + uu + vv + ww);
			
			FTYPE temp;
			#ifdef C_CMPLX
			if (creal(epsaa) > creal(zero)) temp = epsaa; else temp = zero;
			#else
			temp = MAX(epsaa, zero);
			#endif
			
			FTYPE epsbb = 0.25 / temp;
			FTYPE epscc = 2.0  * epsaa;
			
			if (creal(t18) < creal(epscc)) t18 = t18 * t18 * epsbb + epsaa;
			if (creal(t17) < creal(epscc)) t17 = t17 * t17 * epsbb + epsaa;
			if (creal(t19) < creal(epscc)) t19 = t19 * t19 * epsbb + epsaa;
		}
		
		FTYPE t14 = t18 * (t1 - ql2);
		t15 = 0.5 * (ql2 + ql1 / t8);
		t16 = (ql2 - t15) * t17;
		t15 = t15 * t19;
		
		qr2 = t18 * (t2 * qr1 - axi * ql1);
		qr3 = t18 * (t3 * qr1 - ayi * ql1);
		qr4 = t18 * (t4 * qr1 - azi * ql1);
		qr5 = t9 * qr2 + t10 * qr3 + t11 * qr4;
		qr[ind(i, 1)] = qr2;  // Always keep qr* updated
		qr[ind(i, 2)] = qr3;  // Always keep qr* updated
		qr[ind(i, 3)] = qr4;  // Always keep qr* updated
		qr[ind(i, 4)] = qr5;  // Always keep qr* updated
		
		ql1 = t14 + t15 + t16;
		ql2 = t8 * (t15 - t16);
		ql[ind(i, 0)] = ql1;  // Always keep ql* updated
		ql[ind(i, 1)] = ql2;  // Always keep ql* updated
		
		f1 = f1 - ql1;
		f2 = f2 - ql1 * t9  - axi * ql2 - qr2;
		f3 = f3 - ql1 * t10 - ayi * ql2 - qr3;
		f4 = f4 - ql1 * t11 - azi * ql2 - qr4;
		f5 = f5 - ql1 * t12 - t13 * ql2 - qr5 + t7 * c1 * t14;
		
		// Introduce factor one-half area
		t7 = 0.5 * area[i];
		f[ind(i, 0)] = t7 * f1;
		f[ind(i, 1)] = t7 * f2;
		f[ind(i, 2)] = t7 * f3;
		f[ind(i, 3)] = t7 * f4;
		f[ind(i, 4)] = t7 * f5;
	}
}

static void c_fhat_modified()
{
	// NOTICE: i is shifted to start from 0, we can directly use them;
	// but for qr(), ql() and f(), the index of 2nd dimension need to -1.
	#pragma ivdep
	for (int i = 0; i < n; i++)
	{
		// huangh223 added local variables: qr[1-5], ql[1-5], a[x-z,t]i
		// These can avoid repeat index computation and make it convenient for coding.
		// NOTICE: if the corresponding values in the array is modified,
		// always keep these local variables up-to-date.
		FTYPE qr1 = qr[ind(i, 0)];
		FTYPE qr2 = qr[ind(i, 1)];
		FTYPE qr3 = qr[ind(i, 2)];
		FTYPE qr4 = qr[ind(i, 3)];
		FTYPE qr5 = qr[ind(i, 4)];
		
		FTYPE ql1 = ql[ind(i, 0)];
		FTYPE ql2 = ql[ind(i, 1)];
		FTYPE ql3 = ql[ind(i, 2)];
		FTYPE ql4 = ql[ind(i, 3)];
		FTYPE ql5 = ql[ind(i, 4)];
		
		FTYPE axi = ax[i];
		FTYPE ayi = ay[i];
		FTYPE azi = az[i];
		FTYPE ati = at[i];
		
		// ------ Original code translation start ------
		// ----- Comments in original code is kept -----
		
		FTYPE t1 = qr1 - ql1;
		FTYPE t2 = qr2 - ql2;
		FTYPE t3 = qr3 - ql3;
		FTYPE t4 = qr4 - ql4;
		
		// Pressure and enthalpy
		FTYPE t16 = 1.0 / qr1;
		FTYPE t5  = qr5;
		qr5 = x1 * qr5 * t16 + 0.5 * (qr2 * qr2 + qr3 * qr3 + qr4 * qr4);
		qr[ind(i, 4)] = qr5;  // Always keep qr* updated
		
		FTYPE t15 = 1.0 / ql1;
		FTYPE t19 = ql5;
		ql5 = x1 * ql5 * t15 + 0.5 * (ql2 * ql2 + ql3 * ql3 + ql4 * ql4);
		ql[ind(i, 4)] = ql5;  // Always keep ql* updated
		
		// Unsplit contributions f(r) + f(l)
		FTYPE t18 = axi * qr2 + ayi * qr3 + azi * qr4 + ati;
		FTYPE t17 = axi * ql2 + ayi * ql3 + azi * ql4 + ati;
		FTYPE t6  = t18 * qr1;
		FTYPE t7  = t17 * ql1;
		FTYPE f1  = t6 + t7;
		FTYPE f2  = t6 * qr2 + t7 * ql2;
		FTYPE f3  = t6 * qr3 + t7 * ql3;
		FTYPE f4  = t6 * qr4 + t7 * ql4;
		FTYPE f5  = t6 * qr5 + t7 * ql5;
		FTYPE t8  = t5 + t19;
		f2 += axi * t8;
		f3 += ayi * t8;
		f4 += azi * t8;
		f5 -= ati * t8;
		FTYPE t8t  = t8;
		FTYPE delp = t5 - t19;
		
		// Roe averaged variables
		t6 = qr1 * t15;
		t7 = SQRT(t6);
		t6 = 1.0 / (1.0 + t7);
		t8 = t7 * t6;
		
		// Average density
		qr1 = ql1 * t7;
		qr[ind(i, 0)] = qr1;  // Always keep qr* updated
		
		// u, v, w, h average
		FTYPE t9  = ql2 * t6 + qr2 * t8;
		FTYPE t10 = ql3 * t6 + qr3 * t8;
		FTYPE t11 = ql4 * t6 + qr4 * t8;
		FTYPE t12 = ql5 * t6 + qr5 * t8;
		
		// Extract sound speed
		t6  = (t9 * t9 + t10 * t10 + t11 * t11) * 0.5;
		t7  = gm1 * (t12 - t6);
		t8  = SQRT(t7);
		FTYPE t13 = t9 * axi + t10 * ayi + t11 * azi;
		
		// Compute preconditioning parameters
		FTYPE temp, vmag1, vel2, vel;
		#ifdef C_CMPLX
		temp = delp;
		if (creal(temp) < 0) temp = -temp;
		#else
		temp = FABS(delp);
		#endif
		
		// vmag1 = 2.0 * t6 + 2.0 * t7 * abs(delp) / t8t
		// vel2 = max(vmag1, avn*uref**2)
		vmag1 = 2.0 * t6 + 2.0 * t7 * temp / t8t;
		#ifdef C_CMPLX
		temp = avn * uref * uref;
		if (creal(vmag1) > creal(temp)) vel2 = vmag1; else vel2 = temp;
		#else
		vel2 = MAX(vmag1, avn * uref * uref);
		#endif
		
		// vel = sqrt(min(t7,vel2))
		#ifdef C_CMPLX
		if (creal(t7) < creal(vel2)) vel = SQRT(t7); else vel = SQRT(vel2);
		#else
		vel = SQRT(MIN(t7, vel2));
		#endif
		
		vel = cprec * vel + (1 - cprec) * t8;
		FTYPE xm2   = (vel / t8) * (vel / t8);
		FTYPE xmave = t13 / t8;
		FTYPE tt1   = 0.5 * (1 + xm2);
		FTYPE tmp1  = xmave * xmave;
		FTYPE tmp2  = (1 - xm2) * (1 - xm2);
		FTYPE tt2   = 0.5 * SQRT(tmp1 * tmp2 + 4 * xm2);
		
		// The variables in t are as follows:
		// 1-4        delta q1-q4
		// 6          q2a
		// 7          c2a
		// 8          ca
		// 9,10,11,12 ua, va, wa, ha
		// 13         ubara
		// 14,15,16   alpha(1),  alpha(4),  alpha(5)
		// 18,19,17   lambda(1), lambda(4), lambda(5)
		// rhoa*delta(ubar), delta(p)/c2a
		
		ql1 = qr1 * (t18 - t17);
		ql2 = (t5 - t19) / t7;
		ql[ind(i, 0)] = ql1;  // Always keep ql* updated
		ql[ind(i, 1)] = ql2;  // Always keep ql* updated
		
		t18 = t13 + ati;
		t19 = tt1 * t13 + tt2 * t8 + ati; 
		t17 = tt1 * t13 - tt2 * t8 + ati;
		FTYPE fplus =  (t19 - t18) / (xm2 * t8);
		FTYPE fmins = -(t17 - t18) / (xm2 * t8);
		FTYPE fsum  = 2.0 / (fplus + fmins) / xm2;
		
		#ifdef C_CMPLX
		if (creal(t18) < 0) t18 = -t18;
		if (creal(t19) < 0) t19 = -t19;
		if (creal(t17) < 0) t17 = -t17;
		#else
		t18 = FABS(t18);
		t19 = FABS(t19);
		t17 = FABS(t17);
		#endif
		
		// Limit eigenvalues a la Harten and Gnoffo (NASA TP-2953)
		if (creal(epsa_r) > 0) 
		{
			FTYPE cc = t8;
			FTYPE uu = t9;
			FTYPE vv = t10;
			FTYPE ww = t11;
			
			#ifdef C_CMPLX
			if (creal(uu) < 0) uu = -uu;
			if (creal(vv) < 0) vv = -vv;
			if (creal(ww) < 0) ww = -ww;
			#else
			uu = FABS(uu);
			vv = FABS(vv);
			ww = FABS(ww);
			#endif
			
			FTYPE epsaa = epsa_r * (cc + uu + vv + ww);
			
			FTYPE temp;
			#ifdef C_CMPLX
			if (creal(epsaa) > creal(zero)) temp = epsaa; else temp = zero;
			#else
			temp = MAX(epsaa, zero);
			#endif
			
			FTYPE epsbb = 0.25 / temp;
			FTYPE epscc = 2.0  * epsaa;
			
			if (creal(t18) < creal(epscc)) t18 = t18 * t18 * epsbb + epsaa;
			if (creal(t17) < creal(epscc)) t17 = t17 * t17 * epsbb + epsaa;
			if (creal(t19) < creal(epscc)) t19 = t19 * t19 * epsbb + epsaa;
		}
		
		FTYPE t14 = t18 * (t1 - ql2);
		t15 = 0.5 * t19 * (fplus * ql2 + ql1 / t8);
		t16 = 0.5 * t17 * (fmins * ql2 - ql1 / t8);
		
		qr2 = t18 * (t2 * qr1 - axi * ql1);
		qr3 = t18 * (t3 * qr1 - ayi * ql1);
		qr4 = t18 * (t4 * qr1 - azi * ql1);
		qr5 = t9 * qr2 + t10 * qr3 + t11 * qr4;
		qr[ind(i, 1)] = qr2;  // Always keep qr* updated
		qr[ind(i, 2)] = qr3;  // Always keep qr* updated
		qr[ind(i, 3)] = qr4;  // Always keep qr* updated
		qr[ind(i, 4)] = qr5;  // Always keep qr* updated
		
		ql1 = t14 + fsum * (t15 + t16);
		ql2 = fsum * t8 * xm2 * (fmins * t15 - fplus * t16);
		ql[ind(i, 0)] = ql1;  // Always keep ql* updated
		ql[ind(i, 1)] = ql2;  // Always keep ql* updated
		
		f1 = f1 - ql1;
		f2 = f2 - ql1 * t9  - axi * ql2 - qr2;
		f3 = f3 - ql1 * t10 - ayi * ql2 - qr3;
		f4 = f4 - ql1 * t11 - azi * ql2 - qr4;
		f5 = f5 - ql1 * t12 - t13 * ql2 - qr5 + t7 * c1 * t14;
		
		// Introduce factor one-half area
		t7 = 0.5 * area[i];
		f[ind(i, 0)] = t7 * f1;
		f[ind(i, 1)] = t7 * f2;
		f[ind(i, 2)] = t7 * f3;
		f[ind(i, 3)] = t7 * f4;
		f[ind(i, 4)] = t7 * f5;
	}
}

static int c_fhat_cnt = 0;

void c_fhat_(
	int *_n, int *_iexp, int *_nvtq, 
	FTYPE *_gamma, FTYPE *_gm1, FTYPE *_gp1, FTYPE *_gm1g, FTYPE *_gp1g, 
	FTYPE *_ggm1, FTYPE *_cprec, FTYPE *_uref, FTYPE *_avn, FTYPE *_epsa_r, 
	FTYPE *_ax, FTYPE *_ay, FTYPE *_az, FTYPE *_area, 
	FTYPE *_at, FTYPE *_qr, FTYPE *_ql, FTYPE *_f
)
{
	if (c_fhat_cnt == 0) printf("Using c_fhat\n"); fflush(stdout);
	
	// Single variables
	n      = *_n;
	iexp   = *_iexp;
	nvtq   = *_nvtq;
	gamma  = *_gamma;
	gm1    = *_gm1;
	gp1    = *_gp1;
	gm1g   = *_gm1g;
	gp1g   = *_gp1g;
	ggm1   = *_ggm1;
	cprec  = *_cprec;
	uref   = *_uref;
	avn    = *_avn;
	epsa_r = *_epsa_r;
	zero   = POW(10.0, (FTYPE) -iexp);
	epsa_l = 2.0 * epsa_r;
	x1     = gamma / gm1;
	c1     = 1.0   / gm1;
	
	// Pointers
	ax   = _ax;
	ay   = _ay;
	az   = _az;
	area = _area;
	at   = _at;
	qr   = _qr;
	ql   = _ql;
	f    = _f;
	
	if (creal(cprec) == 0.0) c_fhat_original();
	else c_fhat_modified();
	
	c_fhat_cnt++;
}
