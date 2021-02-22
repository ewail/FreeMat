/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "QRDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
// Note - to get a complete QR decomposition, we need 2 LAPACK routines.
// The first is sgeqrf, which computes the QR decomposition, but stores
// the matrix Q as a sequence of Householder transformations.  To get
// a usable representation of Q within FreeMat, the routine sorgqr must
// be called to expand the transformations into a Q matrix.
// Generally speaking, a QR decomposition should be Q=MxN and R=NxN
// if M>N.  If M<=N, then Q=MxM, R = MxN.  In either case, if L=min(M,N),
// then Q = MxL and R=LxN
void floatQRD(int nrows, int ncols, float *q, float *r, float *a) {
  //      SUBROUTINE SGEQRF( M, N, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGEQRF computes a QR factorization of a real M-by-N matrix A:
  //*  A = Q * R.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the elements on and above the diagonal of the array
  //*          contain the min(M,N)-by-N upper trapezoidal matrix R (R is
  //*          upper triangular if m >= n); the elements below the diagonal,
  //*          with the array TAU, represent the orthogonal matrix Q as a
  //*          product of min(m,n) elementary reflectors (see Further
  //*          Details).
  //*
  float *A;
  A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  TAU     (output) REAL array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors (see Further
  //*          Details).
  //*
  float *TAU;
  TAU = (float*) Malloc(MIN(M,N)*sizeof(float));
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  float *WORK;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.  LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is 
  //*          the optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //
  int LWORK;
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  LWORK = -1;
  float WORKSZE;
  int INFO;
  sgeqrf_(&M, &N, A, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (float*) Malloc(LWORK*sizeof(float));
  sgeqrf_(&M, &N, A, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(float));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(float));
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++)
      q[i+j*M] = a[i+j*M];
  //      SUBROUTINE SORGQR( M, N, K, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, K, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SORGQR generates an M-by-N real matrix Q with orthonormal columns,
  //*  which is defined as the first N columns of a product of K elementary
  //*  reflectors of order M
  //*
  //*        Q  =  H(1) H(2) . . . H(k)
  //*
  //*  as returned by SGEQRF.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix Q. M >= 0.
  M = nrows;
  //*
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix Q. M >= N >= 0.
  N = minmn;
  //*
  //*  K       (input) INTEGER
  //*          The number of elementary reflectors whose product defines the
  //*          matrix Q. N >= K >= 0.
  int K;
  K = minmn;
  //*
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the i-th column must contain the vector which
  //*          defines the elementary reflector H(i), for i = 1,2,...,k, as
  //*          returned by SGEQRF in the first k columns of its array
  //*          argument A.
  //*          On exit, the M-by-N matrix Q.
  //*
  //*  LDA     (input) INTEGER
  //*          The first dimension of the array A. LDA >= max(1,M).
  LDA = M;
  //*
  //*  TAU     (input) REAL array, dimension (K)
  //*          TAU(i) must contain the scalar factor of the elementary
  //*          reflector H(i), as returned by SGEQRF.
  //*
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is the
  //*          optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument has an illegal value
  //*
  LWORK = -1;
  sorgqr_(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (float*) Malloc(LWORK*sizeof(float));
  sorgqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void doubleQRD(int nrows, int ncols, double *q, double *r, double *a) {
  int M = nrows;
  int N = ncols;
  double *A;
  A = a;
  int LDA = nrows;
  double *TAU;
  TAU = (double*) Malloc(MIN(M,N)*sizeof(double));
  double *WORK;
  int LWORK;
  LWORK = -1;
  double WORKSZE;
  int INFO;
  dgeqrf_(&M, &N, A, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (double*) Malloc(LWORK*sizeof(double));
  dgeqrf_(&M, &N, A, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(double));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(double));
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++)
      q[i+j*M] = a[i+j*M];
  M = nrows;
  N = minmn;
  int K;
  K = minmn;
  LDA = M;
  LWORK = -1;
  dorgqr_(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (double*) Malloc(LWORK*sizeof(double));
  dorgqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void complexQRD(int nrows, int ncols, float *q, float *r, float *a) {
  //      SUBROUTINE CGEQRF( M, N, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      COMPLEX            A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CGEQRF computes a QR factorization of a complex M-by-N matrix A:
  //*  A = Q * R.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the elements on and above the diagonal of the array
  //*          contain the min(M,N)-by-N upper trapezoidal matrix R (R is
  //*          upper triangular if m >= n); the elements below the diagonal,
  //*          with the array TAU, represent the unitary matrix Q as a
  //*          product of min(m,n) elementary reflectors (see Further
  //*          Details).
  //*
  float *A;
  A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  TAU     (output) COMPLEX array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors (see Further
  //*          Details).
  //*
  float *TAU;
  TAU = (float*) Malloc(MIN(M,N)*sizeof(float)*2);
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  float *WORK;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.  LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is
  //*          the optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //
  LWORK = -1;
  float WORKSZE[2];
  int INFO;
  cgeqrf_(&M, &N, A, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (float*) Malloc(LWORK*sizeof(float)*2);
  cgeqrf_(&M, &N, A, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(float)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++) {
      r[2*(j+i*minmn)] = a[2*(j+i*M)];
      r[2*(j+i*minmn)+1] = a[2*(j+i*M)+1];
    }
  }
  memset(q,0,M*minmn*sizeof(float)*2);
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++) {
      q[2*(i+j*M)] = a[2*(i+j*M)];
      q[2*(i+j*M)+1] = a[2*(i+j*M)+1];
    }
  //      SUBROUTINE CUNGQR( M, N, K, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, K, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      COMPLEX            A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CUNGQR generates an M-by-N complex matrix Q with orthonormal columns,
  //*  which is defined as the first N columns of a product of K elementary
  //*  reflectors of order M
  //*
  //*        Q  =  H(1) H(2) . . . H(k)
  //*
  //*  as returned by CGEQRF.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix Q. M >= 0.
  M = nrows;    
  //*
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix Q. M >= N >= 0.
  N = minmn;
  //*
  //*  K       (input) INTEGER
  //*          The number of elementary reflectors whose product defines the
  //*          matrix Q. N >= K >= 0.
  int K;
  K = minmn;
  //*
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the i-th column must contain the vector which
  //*          defines the elementary reflector H(i), for i = 1,2,...,k, as
  //*          returned by CGEQRF in the first k columns of its array
  //*          argument A.
  //*          On exit, the M-by-N matrix Q.
  //*
  //*  LDA     (input) INTEGER
  //*          The first dimension of the array A. LDA >= max(1,M).
  LDA = M;
  //*
  //*  TAU     (input) COMPLEX array, dimension (K)
  //*          TAU(i) must contain the scalar factor of the elementary
  //*          reflector H(i), as returned by CGEQRF.
  //*
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is the
  //*          optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument has an illegal value
  //*
  LWORK = -1;
  cungqr_(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (float*) Malloc(LWORK*sizeof(float)*2);
  cungqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void dcomplexQRD(int nrows, int ncols, double *q, double *r, double *a) {
  int M = nrows;
  int N = ncols;
  double *A;
  A = a;
  int LDA = nrows;
  double *TAU;
  TAU = (double*) Malloc(MIN(M,N)*sizeof(double)*2);
  double *WORK;
  int LWORK;
  LWORK = -1;
  double WORKSZE[2];
  int INFO;
  zgeqrf_(&M, &N, A, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (double*) Malloc(LWORK*sizeof(double)*2);
  zgeqrf_(&M, &N, A, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(double)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++) {
      r[2*(j+i*minmn)] = a[2*(j+i*M)];
      r[2*(j+i*minmn)+1] = a[2*(j+i*M)+1];
    }
  }
  memset(q,0,M*minmn*sizeof(double)*2);
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++) {
      q[2*(i+j*M)] = a[2*(i+j*M)];
      q[2*(i+j*M)+1] = a[2*(i+j*M)+1];
    }
  M = nrows;    
  N = minmn;
  int K;
  K = minmn;
  LDA = M;
  LWORK = -1;
  zungqr_(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (double*) Malloc(LWORK*sizeof(double)*2);
  zungqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void floatQRDP(int nrows, int ncols, float *q, float *r, int *p, float *a) {
  //      SUBROUTINE SGEQP3( M, N, A, LDA, JPVT, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            JPVT( * )
  //      REAL               A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGEQP3 computes a QR factorization with column pivoting of a
  //*  matrix A:  A*P = Q*R  using Level 3 BLAS.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A. M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the upper triangle of the array contains the
  //*          min(M,N)-by-N upper trapezoidal matrix R; the elements below
  //*          the diagonal, together with the array TAU, represent the
  //*          orthogonal matrix Q as a product of min(M,N) elementary
  //*          reflectors.
  //*
  float *A;
  A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A. LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(J).ne.0, the J-th column of A is permuted
  //*          to the front of A*P (a leading column); if JPVT(J)=0,
  //*          the J-th column of A is a free column.
  //*          On exit, if JPVT(J)=K, then the J-th column of A*P was the
  //*          the K-th column of A.
  //*
  int *JPVT;
  JPVT = p;
  //*  TAU     (output) REAL array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors.
  //*
  float *TAU;
  TAU = (float*) Malloc(MIN(M,N)*sizeof(float));
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO=0, WORK(1) returns the optimal LWORK.
  //*
  float *WORK;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 3*N+1.
  //*          For optimal performance LWORK >= 2*N+( N+1 )*NB, where NB
  //*          is the optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  INFO    (output) INTEGER
  //*          = 0: successful exit.
  //*          < 0: if INFO = -i, the i-th argument had an illegal value.
  LWORK = -1;
  float WORKSZE;
  int INFO;
  sgeqp3_(&M, &N, A, &LDA, JPVT, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (float*) Malloc(LWORK*sizeof(float));
  sgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(float));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(float));
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++)
      q[i+j*M] = a[i+j*M];
  //      SUBROUTINE SORGQR( M, N, K, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, K, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SORGQR generates an M-by-N real matrix Q with orthonormal columns,
  //*  which is defined as the first N columns of a product of K elementary
  //*  reflectors of order M
  //*
  //*        Q  =  H(1) H(2) . . . H(k)
  //*
  //*  as returned by SGEQRF.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix Q. M >= 0.
  M = nrows;
  //*
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix Q. M >= N >= 0.
  N = minmn;
  //*
  //*  K       (input) INTEGER
  //*          The number of elementary reflectors whose product defines the
  //*          matrix Q. N >= K >= 0.
  int K;
  K = minmn;
  //*
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the i-th column must contain the vector which
  //*          defines the elementary reflector H(i), for i = 1,2,...,k, as
  //*          returned by SGEQRF in the first k columns of its array
  //*          argument A.
  //*          On exit, the M-by-N matrix Q.
  //*
  //*  LDA     (input) INTEGER
  //*          The first dimension of the array A. LDA >= max(1,M).
  LDA = M;
  //*
  //*  TAU     (input) REAL array, dimension (K)
  //*          TAU(i) must contain the scalar factor of the elementary
  //*          reflector H(i), as returned by SGEQRF.
  //*
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is the
  //*          optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument has an illegal value
  //*
  LWORK = -1;
  sorgqr_(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (float*) Malloc(LWORK*sizeof(float));
  sorgqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void doubleQRDP(int nrows, int ncols, double *q, double *r, int *p, double *a) {
  int M = nrows;
  int N = ncols;
  double *A;
  A = a;
  int LDA = nrows;
  int *JPVT;
  JPVT = p;
  double *TAU;
  TAU = (double*) Malloc(MIN(M,N)*sizeof(double));
  double *WORK;
  int LWORK;
  LWORK = -1;
  double WORKSZE;
  int INFO;
  dgeqp3_(&M, &N, A, &LDA, JPVT, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (double*) Malloc(LWORK*sizeof(double));
  dgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(double));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(double));
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++)
      q[i+j*M] = a[i+j*M];
  M = nrows;
  N = minmn;
  int K;
  K = minmn;
  LDA = M;
  LWORK = -1;
  dorgqr_(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  WORK = (double*) Malloc(LWORK*sizeof(double));
  dorgqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void complexQRDP(int nrows, int ncols, float *q, float *r, int *p, float *a) {
  //      SUBROUTINE CGEQP3( M, N, A, LDA, JPVT, TAU, WORK, LWORK, RWORK,
  //     $                   INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            JPVT( * )
  //      REAL               RWORK( * )
  //      COMPLEX            A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CGEQP3 computes a QR factorization with column pivoting of a
  //*  matrix A:  A*P = Q*R  using Level 3 BLAS.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A. M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the upper triangle of the array contains the
  //*          min(M,N)-by-N upper trapezoidal matrix R; the elements below
  //*          the diagonal, together with the array TAU, represent the
  //*          unitary matrix Q as a product of min(M,N) elementary
  //*          reflectors.
  //*
  float *A;
  A = a;
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A. LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(J).ne.0, the J-th column of A is permuted
  //*          to the front of A*P (a leading column); if JPVT(J)=0,
  //*          the J-th column of A is a free column.
  //*          On exit, if JPVT(J)=K, then the J-th column of A*P was the
  //*          the K-th column of A.
  //*
  int *JPVT = p;
  //*  TAU     (output) COMPLEX array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors.
  //*
  float *TAU;
  TAU = (float*) Malloc(MIN(M,N)*sizeof(float)*2);
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO=0, WORK(1) returns the optimal LWORK.
  //*
  float *WORK;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= N+1.
  //*          For optimal performance LWORK >= ( N+1 )*NB, where NB
  //*          is the optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //* 
  int LWORK;
  //*  RWORK   (workspace) REAL array, dimension (2*N)
  //*
  float *RWORK = (float*) Malloc(2*N*sizeof(float));
  //*  INFO    (output) INTEGER
  //*          = 0: successful exit.
  //*          < 0: if INFO = -i, the i-th argument had an illegal value.
  LWORK = -1;
  float WORKSZE[2];
  int INFO;
  cgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORKSZE, &LWORK, RWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (float*) Malloc(LWORK*sizeof(float)*2);
  cgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORK, &LWORK, RWORK, &INFO);
  Free(WORK);
  Free(RWORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(float)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++) {
      r[2*(j+i*minmn)] = a[2*(j+i*M)];
      r[2*(j+i*minmn)+1] = a[2*(j+i*M)+1];
    }
  }
  memset(q,0,M*minmn*sizeof(float)*2);
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++) {
      q[2*(i+j*M)] = a[2*(i+j*M)];
      q[2*(i+j*M)+1] = a[2*(i+j*M)+1];
    }
  //      SUBROUTINE CUNGQR( M, N, K, A, LDA, TAU, WORK, LWORK, INFO )
  //*
  //*  -- LAPACK routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //      INTEGER            INFO, K, LDA, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      COMPLEX            A( LDA, * ), TAU( * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CUNGQR generates an M-by-N complex matrix Q with orthonormal columns,
  //*  which is defined as the first N columns of a product of K elementary
  //*  reflectors of order M
  //*
  //*        Q  =  H(1) H(2) . . . H(k)
  //*
  //*  as returned by CGEQRF.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix Q. M >= 0.
  M = nrows;    
  //*
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix Q. M >= N >= 0.
  N = minmn;
  //*
  //*  K       (input) INTEGER
  //*          The number of elementary reflectors whose product defines the
  //*          matrix Q. N >= K >= 0.
  int K;
  K = minmn;
  //*
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the i-th column must contain the vector which
  //*          defines the elementary reflector H(i), for i = 1,2,...,k, as
  //*          returned by CGEQRF in the first k columns of its array
  //*          argument A.
  //*          On exit, the M-by-N matrix Q.
  //*
  //*  LDA     (input) INTEGER
  //*          The first dimension of the array A. LDA >= max(1,M).
  LDA = M;
  //*
  //*  TAU     (input) COMPLEX array, dimension (K)
  //*          TAU(i) must contain the scalar factor of the elementary
  //*          reflector H(i), as returned by CGEQRF.
  //*
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= max(1,N).
  //*          For optimum performance LWORK >= N*NB, where NB is the
  //*          optimal blocksize.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument has an illegal value
  //*
  LWORK = -1;
  cungqr_(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (float*) Malloc(LWORK*sizeof(float)*2);
  cungqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}

void dcomplexQRDP(int nrows, int ncols, double *q, double *r, int *p, double *a) {
  int M = nrows;
  int N = ncols;
  double *A;
  A = a;
  int LDA = nrows;
  int *JPVT = p;
  double *TAU;
  TAU = (double*) Malloc(MIN(M,N)*sizeof(double)*2);
  double *WORK;
  int LWORK;
  double *RWORK = (double*) Malloc(2*N*sizeof(double));
  LWORK = -1;
  double WORKSZE[2];
  int INFO;
  zgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORKSZE, &LWORK, RWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (double*) Malloc(LWORK*sizeof(double)*2);
  zgeqp3_(&M, &N, A, &LDA, JPVT, TAU, WORK, &LWORK, RWORK, &INFO);
  Free(WORK);
  int minmn;
  minmn = MIN(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(double)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = MIN(minmn-1,i);
    for (j=0;j<=k;j++) {
      r[2*(j+i*minmn)] = a[2*(j+i*M)];
      r[2*(j+i*minmn)+1] = a[2*(j+i*M)+1];
    }
  }
  memset(q,0,M*minmn*sizeof(double)*2);
  for (i=0;i<M;i++)
    for (j=0;j<minmn;j++) {
      q[2*(i+j*M)] = a[2*(i+j*M)];
      q[2*(i+j*M)+1] = a[2*(i+j*M)+1];
    }
  M = nrows;    
  N = minmn;
  int K;
  K = minmn;
  LDA = M;
  LWORK = -1;
  zungqr_(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  WORK = (double*) Malloc(LWORK*sizeof(double)*2);
  zungqr_(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  Free(WORK);
  Free(TAU);
}
