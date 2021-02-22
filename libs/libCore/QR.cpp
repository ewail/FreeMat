/*
 * Copyright (c) 2009 Samit Basu
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

#include "Array.hpp"
#include "MemPtr.hpp"
#include "LAPACK.hpp"
#include "Algorithms.hpp"

// Need to wrap:
//sgeqrf, sorgqr, cungqr, sgeqp3_, 
template <typename T>
static void Rgeqrf(int *M, int *N, T *A, int *LDA, T *TAU, T *WORK, int *LWORK, int *INFO);

template <>
void Rgeqrf(int *M, int *N, float *A, int *LDA, float *TAU, float *WORK, int *LWORK, int *INFO) {
  sgeqrf_(M,N,A,LDA,TAU,WORK,LWORK,INFO);
}

template <>
void Rgeqrf(int *M, int *N, double *A, int *LDA, double *TAU, double *WORK, int *LWORK, int *INFO) {
  dgeqrf_(M,N,A,LDA,TAU,WORK,LWORK,INFO);
}

template <typename T>
static void Rorgqr(int *M, int *N, int *K, T *A, int *LDA, T *TAU, T *WORK, int *LWORK, int *INFO);

template <>
void Rorgqr(int *M, int *N, int *K, float *A, int *LDA, float *TAU, float *WORK, int *LWORK, int *INFO) {
  sorgqr_(M,N,K,A,LDA,TAU,WORK,LWORK,INFO);
}

template <>
void Rorgqr(int *M, int *N, int *K, double *A, int *LDA, double *TAU, double *WORK, int *LWORK, int *INFO) {
  dorgqr_(M,N,K,A,LDA,TAU,WORK,LWORK,INFO);
}

template <typename T>
static void Cgeqrf(int *M, int *N, T *A, int *LDA, T *TAU, T *WORK, int *LWORK, int *INFO);

template <>
void Cgeqrf(int *M, int *N, float *A, int *LDA, float *TAU, float *WORK, int *LWORK, int *INFO) {
  cgeqrf_(M,N,A,LDA,TAU,WORK,LWORK,INFO);
}

template <>
void Cgeqrf(int *M, int *N, double *A, int *LDA, double *TAU, double *WORK, int *LWORK, int *INFO) {
  zgeqrf_(M,N,A,LDA,TAU,WORK,LWORK,INFO);
}

template <typename T>
static void Cungqr(int *M, int *N, int *K, T *A, int *LDA, T *TAU, T *WORK, int *LWORK, int *INFO);

template <>
void Cungqr(int *M, int *N, int *K, float *A, int *LDA, float *TAU, float *WORK, int *LWORK, int *INFO) {
  cungqr_(M,N,K,A,LDA,TAU,WORK,LWORK,INFO);
}

template <>
void Cungqr(int *M, int *N, int *K, double *A, int *LDA, double *TAU, double *WORK, int *LWORK, int *INFO) {
  zungqr_(M,N,K,A,LDA,TAU,WORK,LWORK,INFO);
}

template <typename T>
static void Rgeqp3(int *M, int *N, T *A, int *LDA, int *JPVT, T *TAU, T *WORK, int *LWORK, int *INFO);

template <>
void Rgeqp3(int *M, int *N, float *A, int *LDA, int *JPVT, float *TAU, float *WORK, int *LWORK, int *INFO) {
  sgeqp3_(M,N,A,LDA,JPVT,TAU,WORK,LWORK,INFO);
}

template <>
void Rgeqp3(int *M, int *N, double *A, int *LDA, int *JPVT, double *TAU, double *WORK, int *LWORK, int *INFO) {
  dgeqp3_(M,N,A,LDA,JPVT,TAU,WORK,LWORK,INFO);
}

template <typename T>
static void Cgeqp3(int *M, int *N, T *A, int *LDA, int *JPVT, T *TAU, T *WORK, int *LWORK, T *RWORK, int *INFO);

template <>
void Cgeqp3(int *M, int *N, float *A, int *LDA, int *JPVT, float *TAU, float *WORK, int *LWORK, float *RWORK, int *INFO) {
  cgeqp3_(M,N,A,LDA,JPVT,TAU,WORK,LWORK,RWORK,INFO);
}

template <>
void Cgeqp3(int *M, int *N, double *A, int *LDA, int *JPVT, double *TAU, double *WORK, int *LWORK, double *RWORK, int *INFO) {
  zgeqp3_(M,N,A,LDA,JPVT,TAU,WORK,LWORK,RWORK,INFO);
}

template <typename T>
static void RealQRD(BasicArray<T> &Q, BasicArray<T> &R, BasicArray<T> &A) {
  // Note - to get a complete QR decomposition, we need 2 LAPACK routines.
  // The first is sgeqrf, which computes the QR decomposition, but stores
  // the matrix Q as a sequence of Householder transformations.  To get
  // a usable representation of Q within FreeMat, the routine sorgqr must
  // be called to expand the transformations into a Q matrix.
  // Generally speaking, a QR decomposition should be Q=MxN and R=NxN
  // if M>N.  If M<=N, then Q=MxM, R = MxN.  In either case, if L=min(M,N),
  // then Q = MxL and R=LxN
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
  int M = int(A.rows());
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = int(A.cols());
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the elements on and above the diagonal of the array
  //*          contain the min(M,N)-by-N upper trapezoidal matrix R (R is
  //*          upper triangular if m >= n); the elements below the diagonal,
  //*          with the array TAU, represent the orthogonal matrix Q as a
  //*          product of min(m,n) elementary reflectors (see Further
  //*          Details).
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = M;
  //*  TAU     (output) REAL array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors (see Further
  //*          Details).
  //*
  MemBlock<T> TAUBlock(qMin(M,N)); 
  T *TAU = &TAUBlock;
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
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
  T WORKSZE;
  int INFO;
  T* a = A.data();
  Rgeqrf(&M, &N, a, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORKBlock(LWORK);
  T* WORK = &WORKBlock;
  Rgeqrf(&M, &N, a, &LDA, TAU, WORK, &LWORK, &INFO);
  int minmn = qMin(M,N);
  // Need to copy out the upper triangle of A into the upper triangle of R
  T* r = R.data();
  T* q = Q.data();
  memset(r,0,minmn*N*sizeof(T));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = qMin(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(T));
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
  //*
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix Q. M >= N >= 0.
  //*
  N = minmn;
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
  Rorgqr(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORKBlock2(LWORK); 
  WORK = &WORKBlock2;
  Rorgqr(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
}

template <typename T>
static void ComplexQRD(BasicArray<T> &Q, BasicArray<T> &R, BasicArray<T> A) {
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
  int M = int(A.rows()/2);
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = int(A.cols());
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the elements on and above the diagonal of the array
  //*          contain the min(M,N)-by-N upper trapezoidal matrix R (R is
  //*          upper triangular if m >= n); the elements below the diagonal,
  //*          with the array TAU, represent the unitary matrix Q as a
  //*          product of min(m,n) elementary reflectors (see Further
  //*          Details).
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = M;
  //*  TAU     (output) COMPLEX array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors (see Further
  //*          Details).
  //*
  MemBlock<T> TAUBlock(qMin(M,N)*2);
  T *TAU = &TAUBlock;
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
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
  T WORKSZE[2];
  int INFO;
  Cgeqrf(&M, &N, A.data(), &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORKBlock(LWORK*2);
  T *WORK = &WORKBlock;
  Cgeqrf(&M, &N, A.data(), &LDA, TAU, WORK, &LWORK, &INFO);
  int minmn = qMin(M,N);
  T* r = R.data();
  T* a = A.data();
  T* q = Q.data();
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(T)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = qMin(minmn-1,i);
    for (j=0;j<=k;j++) {
      r[2*(j+i*minmn)] = a[2*(j+i*M)];
      r[2*(j+i*minmn)+1] = a[2*(j+i*M)+1];
    }
  }
  memset(q,0,M*minmn*sizeof(T)*2);
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
  Cungqr(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORKBlock2(LWORK*2);
  WORK = &WORKBlock2;
  Cungqr(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
}

template <typename T>
static void RealQRDP(BasicArray<T> &Q, BasicArray<T> &R, BasicArray<T> &P, BasicArray<T> &A) {
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
  int M = int(A.rows());
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = int(A.cols());
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the upper triangle of the array contains the
  //*          min(M,N)-by-N upper trapezoidal matrix R; the elements below
  //*          the diagonal, together with the array TAU, represent the
  //*          orthogonal matrix Q as a product of min(M,N) elementary
  //*          reflectors.
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A. LDA >= max(1,M).
  //*
  int LDA = int(A.rows());
  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(J).ne.0, the J-th column of A is permuted
  //*          to the front of A*P (a leading column); if JPVT(J)=0,
  //*          the J-th column of A is a free column.
  //*          On exit, if JPVT(J)=K, then the J-th column of A*P was the
  //*          the K-th column of A.
  //*
  int minmn = qMin(M,N);
  MemBlock<int> JPVTBlock(N);
  int *JPVT = &JPVTBlock;
  //*  TAU     (output) REAL array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors.
  //*
  MemBlock<T> TAUBlock(minmn); 
  T* TAU = &TAUBlock;
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO=0, WORK(1) returns the optimal LWORK.
  //*
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
  T WORKSZE;
  int INFO;
  Rgeqp3(&M, &N, A.data(), &LDA, JPVT, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORKBlock(LWORK); 
  T* WORK = &WORKBlock;
  Rgeqp3(&M, &N, A.data(), &LDA, JPVT, TAU, WORK, &LWORK, &INFO);
  // Need to copy out the upper triangle of A into the upper triangle of R
  T *r = R.data();
  T *q = Q.data();
  T *a = A.data();
  memset(r,0,minmn*N*sizeof(T));
  int i, j, k;
  for (i=0;i<N;i++) {
    k = qMin(minmn-1,i);
    for (j=0;j<=k;j++)
      r[j+i*minmn] = a[j+i*M];
  }
  memset(q,0,M*minmn*sizeof(T));
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
  M = int(A.rows());
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
  Rorgqr(&M, &N, &K, q, &LDA, TAU, &WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORKBlock2(LWORK);
  WORK = &WORKBlock2;
  Rorgqr(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  for (int i=0;i<A.cols();i++)
    P.set(i+1,JPVT[i]);
}

template <typename T>
static void ComplexQRDP(BasicArray<T> &Q, BasicArray<T> &R, BasicArray<T> &P, BasicArray<T> A) {
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
  int M = int(A.rows()/2);
  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.
  //*
  int N = int(A.cols());
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, the upper triangle of the array contains the
  //*          min(M,N)-by-N upper trapezoidal matrix R; the elements below
  //*          the diagonal, together with the array TAU, represent the
  //*          unitary matrix Q as a product of min(M,N) elementary
  //*          reflectors.
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A. LDA >= max(1,M).
  //*
  int LDA = int(A.rows()/2);
  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(J).ne.0, the J-th column of A is permuted
  //*          to the front of A*P (a leading column); if JPVT(J)=0,
  //*          the J-th column of A is a free column.
  //*          On exit, if JPVT(J)=K, then the J-th column of A*P was the
  //*          the K-th column of A.
  //*
  MemBlock<int> JPVTBlock(N);
  int *JPVT = &JPVTBlock;
  //*  TAU     (output) COMPLEX array, dimension (min(M,N))
  //*          The scalar factors of the elementary reflectors.
  //*
  MemBlock<T> TAUBlock(qMin(M,N)*2);
  T *TAU = &TAUBlock;
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO=0, WORK(1) returns the optimal LWORK.
  //*
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
  MemBlock<T> RWORKBlock(2*N);
  T *RWORK = &RWORKBlock;
  //*  INFO    (output) INTEGER
  //*          = 0: successful exit.
  //*          < 0: if INFO = -i, the i-th argument had an illegal value.
  LWORK = -1;
  T WORKSZE[2];
  int INFO;
  Cgeqp3(&M, &N, A.data(), &LDA, JPVT, TAU, WORKSZE, &LWORK, RWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORKBlock(LWORK*2);
  T *WORK = &WORKBlock;
  Cgeqp3(&M, &N, A.data(), &LDA, JPVT, TAU, WORK, &LWORK, RWORK, &INFO);
  int minmn = qMin(M,N);
  T *r = R.data();
  T *a = A.data();
  T *q = Q.data();
  // Need to copy out the upper triangle of A into the upper triangle of R
  memset(r,0,minmn*N*sizeof(float)*2);
  int i, j, k;
  for (i=0;i<N;i++) {
    k = qMin(minmn-1,i);
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
  M = int(A.rows()/2);    
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
  Cungqr(&M, &N, &K, q, &LDA, TAU, WORKSZE, &LWORK, &INFO);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORKBlock2(LWORK*2);
  WORK = &WORKBlock2;
  Cungqr(&M, &N, &K, q, &LDA, TAU, WORK, &LWORK, &INFO);
  for (int i=0;i<A.cols();i++)
    P.set(i+1,JPVT[i]);
}

template <typename T>
static BasicArray<T> Trim(BasicArray<T> in, const NTuple &dim) {
  BasicArray<T> ret(dim);
  for (index_t i=1;i<=dim.count();i++)
    ret.set(i,in[i]);
  return ret;
}

template <typename T>
static ArrayVector QRDNoPivotFunction(bool compactDec, NTuple odims, Array A) {
  ArrayVector retvec;
  index_t minmn = qMin(A.rows(),A.cols());
  if (A.allReal()) {
    BasicArray<T> q(NTuple(A.rows(),minmn));
    BasicArray<T> r(NTuple(minmn,A.cols()));
    if (minmn != 0) RealQRD(q,r,A.real<T>());
    retvec << Array(q);
    if (compactDec)
      retvec << Array(r);
    else
      retvec << Array(Trim(r,odims));
  } else {
    BasicArray<T> q(NTuple(2*A.rows(),minmn));
    BasicArray<T> r(NTuple(2*minmn,A.cols()));
    if (minmn != 0)  ComplexQRD(q,r,A.fortran<T>());
    retvec << Array(SplitReal(q),SplitImag(q));
    if (compactDec)
      retvec << Array(SplitReal(r),SplitImag(r));
    else
      retvec << Array(Trim(SplitReal(r),odims),Trim(SplitImag(r),odims));
  }
  return retvec;
}

template <typename T>
static ArrayVector QRDPivotFunction(bool compactDec, bool compactSav, 
				    NTuple odims, Array A, 
				    index_t permSize) {
  ArrayVector retvec;
  index_t minmn = qMin(A.rows(),A.cols());
  BasicArray<T> p(NTuple(1,A.cols()));
  if (A.allReal()) {
    BasicArray<T> q(NTuple(A.rows(),minmn));
    BasicArray<T> r(NTuple(minmn,A.cols()));
    if (minmn != 0)  RealQRDP(q,r,p,A.real<T>());
    retvec << Array(q);
    if (compactDec)
      retvec << Array(r);
    else
      retvec << Array(Trim(r,odims));
  } else {
    BasicArray<T> q(NTuple(2*A.rows(),minmn));
    BasicArray<T> r(NTuple(2*minmn,A.cols()));
    if (minmn != 0)  ComplexQRDP(q,r,p,A.fortran<T>());
    retvec << Array(SplitReal(q),SplitImag(q));
    if (compactDec)
      retvec << Array(SplitReal(r),SplitImag(r));
    else
      retvec << Array(Trim(SplitReal(r),odims),Trim(SplitImag(r),odims));
  }
  if (compactSav)
    retvec << Array(p);
  else {
    BasicArray<T> p2(NTuple(permSize,permSize));
    for (index_t i=1;i<=permSize;i++)
      if (p.get(i) <= permSize)
	p2.set(NTuple(p.get(i),i),1);
    retvec << Array(p2);
  }
  return retvec;  
}

static ArrayVector QRDNoPivotFunction(bool compactDec, Array A) {
  NTuple odims(A.dimensions());
  if ((!compactDec) && (A.rows() > A.cols())) 
    A.resize(NTuple(A.rows(),A.rows()));
  else 
    compactDec = true;
  ArrayVector retvec;
  switch (A.dataClass()) {
  default: throw Exception("illegal argument type to qr");
  case Float:
    return QRDNoPivotFunction<float>(compactDec,odims,A);
  case Double:
    return QRDNoPivotFunction<double>(compactDec,odims,A);
  }
}

static ArrayVector QRDPivotFunction(bool compactDec, Array A) {
  NTuple odims(A.dimensions());
  bool compactSav = compactDec;
  index_t Acols = A.cols();
  if ((!compactDec) && (A.rows() > A.cols())) 
    A.resize(NTuple(A.rows(),A.rows()));
  else 
    compactDec = true;
  switch (A.dataClass()) {
  default: throw Exception("illegal argument type to qr");
  case Float:
    return QRDPivotFunction<float>(compactDec,compactSav,odims,A,Acols);
  case Double:
    return QRDPivotFunction<double>(compactDec,compactSav,odims,A,Acols);
  }
}

//@@Signature
//function qr QRDFunction jitsafe
//inputs x n
//outputs varargout
//DOCBLOCK transforms_qr
ArrayVector QRDFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("qr function requires at least one argument - the matrix to decompose.");
  Array A(arg[0].asDenseArray());
  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply qr decomposition to reference types.");
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  //  if (AnyNotFinite(A))
  //    throw Exception("QR Decomposition only defined for matrices with finite entries.");
  bool compactDecomposition = false;
  if ((arg.size() == 2) && (arg[1].asInteger() == 0))
    compactDecomposition = true;
  if (nargout == 3)
    return QRDPivotFunction(compactDecomposition, A);
  else
    return QRDNoPivotFunction(compactDecomposition, A);
}
