/* ../../../dependencies/lapack/src/zgghrd.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static doublecomplex c_b1 = {1.,0.};
static doublecomplex c_b2 = {0.,0.};
static integer c__1 = 1;

/* Subroutine */ int zgghrd_(char *compq, char *compz, integer *n, integer *
	ilo, integer *ihi, doublecomplex *a, integer *lda, doublecomplex *b, 
	integer *ldb, doublecomplex *q, integer *ldq, doublecomplex *z__, 
	integer *ldz, integer *info, ftnlen compq_len, ftnlen compz_len)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3;
    doublecomplex z__1;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    static doublereal c__;
    static doublecomplex s;
    static logical ilq, ilz;
    static integer jcol, jrow;
    extern /* Subroutine */ int zrot_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *);
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    static doublecomplex ctemp;
    extern /* Subroutine */ int xerbla_(char *, integer *, ftnlen);
    static integer icompq, icompz;
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    ftnlen), zlartg_(doublecomplex *, doublecomplex *, doublereal *, 
	    doublecomplex *, doublecomplex *);


/*  -- LAPACK routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     September 30, 1994 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGGHRD reduces a pair of complex matrices (A,B) to generalized upper */
/*  Hessenberg form using unitary transformations, where A is a */
/*  general matrix and B is upper triangular:  Q' * A * Z = H and */
/*  Q' * B * Z = T, where H is upper Hessenberg, T is upper triangular, */
/*  and Q and Z are unitary, and ' means conjugate transpose. */

/*  The unitary matrices Q and Z are determined as products of Givens */
/*  rotations.  They may either be formed explicitly, or they may be */
/*  postmultiplied into input matrices Q1 and Z1, so that */

/*       Q1 * A * Z1' = (Q1*Q) * H * (Z1*Z)' */
/*       Q1 * B * Z1' = (Q1*Q) * T * (Z1*Z)' */

/*  Arguments */
/*  ========= */

/*  COMPQ   (input) CHARACTER*1 */
/*          = 'N': do not compute Q; */
/*          = 'I': Q is initialized to the unit matrix, and the */
/*                 unitary matrix Q is returned; */
/*          = 'V': Q must contain a unitary matrix Q1 on entry, */
/*                 and the product Q1*Q is returned. */

/*  COMPZ   (input) CHARACTER*1 */
/*          = 'N': do not compute Q; */
/*          = 'I': Q is initialized to the unit matrix, and the */
/*                 unitary matrix Q is returned; */
/*          = 'V': Q must contain a unitary matrix Q1 on entry, */
/*                 and the product Q1*Q is returned. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          It is assumed that A is already upper triangular in rows and */
/*          columns 1:ILO-1 and IHI+1:N.  ILO and IHI are normally set */
/*          by a previous call to ZGGBAL; otherwise they should be set */
/*          to 1 and N respectively. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  A       (input/output) COMPLEX*16 array, dimension (LDA, N) */
/*          On entry, the N-by-N general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          rest is set to zero. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) COMPLEX*16 array, dimension (LDB, N) */
/*          On entry, the N-by-N upper triangular matrix B. */
/*          On exit, the upper triangular matrix T = Q' B Z.  The */
/*          elements below the diagonal are set to zero. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  Q       (input/output) COMPLEX*16 array, dimension (LDQ, N) */
/*          If COMPQ='N':  Q is not referenced. */
/*          If COMPQ='I':  on entry, Q need not be set, and on exit it */
/*                         contains the unitary matrix Q, where Q' */
/*                         is the product of the Givens transformations */
/*                         which are applied to A and B on the left. */
/*          If COMPQ='V':  on entry, Q must contain a unitary matrix */
/*                         Q1, and on exit this is overwritten by Q1*Q. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= N if COMPQ='V' or 'I'; LDQ >= 1 otherwise. */

/*  Z       (input/output) COMPLEX*16 array, dimension (LDZ, N) */
/*          If COMPZ='N':  Z is not referenced. */
/*          If COMPZ='I':  on entry, Z need not be set, and on exit it */
/*                         contains the unitary matrix Z, which is */
/*                         the product of the Givens transformations */
/*                         which are applied to A and B on the right. */
/*          If COMPZ='V':  on entry, Z must contain a unitary matrix */
/*                         Z1, and on exit this is overwritten by Z1*Z. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. */
/*          LDZ >= N if COMPZ='V' or 'I'; LDZ >= 1 otherwise. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  This routine reduces A to Hessenberg and B to triangular form by */
/*  an unblocked reduction, as described in _Matrix_Computations_, */
/*  by Golub and van Loan (Johns Hopkins Press). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Decode COMPQ */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;

    /* Function Body */
    if (lsame_(compq, "N", (ftnlen)1, (ftnlen)1)) {
	ilq = FALSE_;
	icompq = 1;
    } else if (lsame_(compq, "V", (ftnlen)1, (ftnlen)1)) {
	ilq = TRUE_;
	icompq = 2;
    } else if (lsame_(compq, "I", (ftnlen)1, (ftnlen)1)) {
	ilq = TRUE_;
	icompq = 3;
    } else {
	icompq = 0;
    }

/*     Decode COMPZ */

    if (lsame_(compz, "N", (ftnlen)1, (ftnlen)1)) {
	ilz = FALSE_;
	icompz = 1;
    } else if (lsame_(compz, "V", (ftnlen)1, (ftnlen)1)) {
	ilz = TRUE_;
	icompz = 2;
    } else if (lsame_(compz, "I", (ftnlen)1, (ftnlen)1)) {
	ilz = TRUE_;
	icompz = 3;
    } else {
	icompz = 0;
    }

/*     Test the input parameters. */

    *info = 0;
    if (icompq <= 0) {
	*info = -1;
    } else if (icompz <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1) {
	*info = -4;
    } else if (*ihi > *n || *ihi < *ilo - 1) {
	*info = -5;
    } else if (*lda < max(1,*n)) {
	*info = -7;
    } else if (*ldb < max(1,*n)) {
	*info = -9;
    } else if (ilq && *ldq < *n || *ldq < 1) {
	*info = -11;
    } else if (ilz && *ldz < *n || *ldz < 1) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("ZGGHRD", &i__1, (ftnlen)6);
	return 0;
    }

/*     Initialize Q and Z if desired. */

    if (icompq == 3) {
	zlaset_("Full", n, n, &c_b2, &c_b1, &q[q_offset], ldq, (ftnlen)4);
    }
    if (icompz == 3) {
	zlaset_("Full", n, n, &c_b2, &c_b1, &z__[z_offset], ldz, (ftnlen)4);
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

/*     Zero out lower triangle of B */

    i__1 = *n - 1;
    for (jcol = 1; jcol <= i__1; ++jcol) {
	i__2 = *n;
	for (jrow = jcol + 1; jrow <= i__2; ++jrow) {
	    i__3 = jrow + jcol * b_dim1;
	    b[i__3].r = 0., b[i__3].i = 0.;
/* L10: */
	}
/* L20: */
    }

/*     Reduce A and B */

    i__1 = *ihi - 2;
    for (jcol = *ilo; jcol <= i__1; ++jcol) {

	i__2 = jcol + 2;
	for (jrow = *ihi; jrow >= i__2; --jrow) {

/*           Step 1: rotate rows JROW-1, JROW to kill A(JROW,JCOL) */

	    i__3 = jrow - 1 + jcol * a_dim1;
	    ctemp.r = a[i__3].r, ctemp.i = a[i__3].i;
	    zlartg_(&ctemp, &a[jrow + jcol * a_dim1], &c__, &s, &a[jrow - 1 + 
		    jcol * a_dim1]);
	    i__3 = jrow + jcol * a_dim1;
	    a[i__3].r = 0., a[i__3].i = 0.;
	    i__3 = *n - jcol;
	    zrot_(&i__3, &a[jrow - 1 + (jcol + 1) * a_dim1], lda, &a[jrow + (
		    jcol + 1) * a_dim1], lda, &c__, &s);
	    i__3 = *n + 2 - jrow;
	    zrot_(&i__3, &b[jrow - 1 + (jrow - 1) * b_dim1], ldb, &b[jrow + (
		    jrow - 1) * b_dim1], ldb, &c__, &s);
	    if (ilq) {
		d_cnjg(&z__1, &s);
		zrot_(n, &q[(jrow - 1) * q_dim1 + 1], &c__1, &q[jrow * q_dim1 
			+ 1], &c__1, &c__, &z__1);
	    }

/*           Step 2: rotate columns JROW, JROW-1 to kill B(JROW,JROW-1) */

	    i__3 = jrow + jrow * b_dim1;
	    ctemp.r = b[i__3].r, ctemp.i = b[i__3].i;
	    zlartg_(&ctemp, &b[jrow + (jrow - 1) * b_dim1], &c__, &s, &b[jrow 
		    + jrow * b_dim1]);
	    i__3 = jrow + (jrow - 1) * b_dim1;
	    b[i__3].r = 0., b[i__3].i = 0.;
	    zrot_(ihi, &a[jrow * a_dim1 + 1], &c__1, &a[(jrow - 1) * a_dim1 + 
		    1], &c__1, &c__, &s);
	    i__3 = jrow - 1;
	    zrot_(&i__3, &b[jrow * b_dim1 + 1], &c__1, &b[(jrow - 1) * b_dim1 
		    + 1], &c__1, &c__, &s);
	    if (ilz) {
		zrot_(n, &z__[jrow * z_dim1 + 1], &c__1, &z__[(jrow - 1) * 
			z_dim1 + 1], &c__1, &c__, &s);
	    }
/* L30: */
	}
/* L40: */
    }

    return 0;

/*     End of ZGGHRD */

} /* zgghrd_ */

