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

#include "LeastSquaresSolver.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "MemPtr.hpp"
#include "Algorithms.hpp"

#define MSGBUFLEN 2048

template <typename T>
static void Tgelsy(int* M, int *N, int *NRHS, T* A, int *LDA,
	    T *B, int *LDB, int *JPVT, T* RCOND,
	    int *RANK, T *WORK, int* LWORK, int* INFO);

template <>
void Tgelsy(int* M, int *N, int *NRHS, double* A, int *LDA,
		   double *B, int *LDB, int *JPVT, double* RCOND,
		   int *RANK, double *WORK, int* LWORK, int* INFO) {
  return dgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);
}

template <>
void Tgelsy(int* M, int *N, int *NRHS, float* A, int *LDA,
		   float *B, int *LDB, int *JPVT, float* RCOND,
		   int *RANK, float *WORK, int* LWORK, int* INFO) {
  return sgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,INFO);  
}

template <typename T>
static void Tgelsy(int* M, int *N, int *NRHS, T* A, int *LDA,
		   T *B, int *LDB, int *JPVT, T* RCOND,
		   int *RANK, T *WORK, int* LWORK, T* RWORK,
		   int* INFO);

template <>
void Tgelsy(int* M, int *N, int *NRHS, float* A, int *LDA,
		   float *B, int *LDB, int *JPVT, float* RCOND,
		   int *RANK, float *WORK, int* LWORK, float* RWORK,
		   int* INFO) {
  return cgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,RWORK,INFO);
}

template <>
void Tgelsy(int* M, int *N, int *NRHS, double* A, int *LDA,
		   double *B, int *LDB, int *JPVT, double* RCOND,
		   int *RANK, double *WORK, int* LWORK, double* RWORK,
		   int* INFO) {
  return zgelsy_(M,N,NRHS,A,LDA,B,LDB,JPVT,RCOND,
		 RANK,WORK,LWORK,RWORK,INFO);
}

/***************************************************************************
 * Least-squares solver for double matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
template <typename T>
static void realSolveLeastSq(int m, int n, int k, T *c, T *a, T *b) {
  if ((m == 0) || (n == 0)) return;
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE DGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  DGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  T *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<T> B(Bsize*NRHS);
  changeStride(&B,Bsize,b,m,m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.
  MemBlock<int> JPVT(N);

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  T RCOND = lamch<T>();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  T WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*             LWORK >= MAX( MN+3*N+1, 2*MN+NRHS ),
  //*          where MN = min( M, N ).
  //*          The block algorithm requires that:
  //*             LWORK >= MAX( MN+2*N+NB*(N+1), 2*MN+NB*NRHS ),
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines DGEQP3, DTZRZF, STZRQF, DORMQR,
  //*          and DORMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORKSIZE, &LWORK, &INFO);
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORK, &LWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N) {
      WarningMessage(QString("Matrix is rank deficient to machine precision.  RANK = %1\n").arg(RANK));
    }
  } else
    // Problem should be underdetermined, rank should be M
    if (RANK < M) {
      WarningMessage(QString("Matrix is rank deficient to machine precision.  RANK = %1\n").arg(RANK));
    }
  changeStride(c,n,&B,Bsize,n,k);
}

/***************************************************************************
 * Least-squares solver for complex matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
template <typename T>
static void complexSolveLeastSq(int m, int n, int k, T *c, T *a, T*b) {
  if ((m == 0) || (n == 0)) return;
  //	SUBROUTINE ZGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   RWORK( * )
  //	COMPLEX*16         A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) COMPLEX*16 array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  T *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  MemBlock<T> B(Bsize*NRHS*2);
  changeStride(&B,2*Bsize,b,2*m,2*m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  MemBlock<int> JPVT(N);

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  T RCOND = lamch<T>();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) COMPLEX*16 array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  T WORKSIZE[2];

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*            LWORK >= MN + MAX( 2*MN, N+1, MN+NRHS )
  //*          where MN = min(M,N).
  //*          The block algorithm requires that:
  //*            LWORK >= MN + MAX( 2*MN, NB*(N+1), MN+MN*NB, MN+NB*NRHS )
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines ZGEQP3, ZTZRZF, CTZRQF, ZUNMQR,
  //*          and ZUNMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  RWORK   (workspace) DOUBLE PRECISION array, dimension (2*N)

  MemBlock<T> RWORK(2*N);

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, WORKSIZE, &LWORK, &RWORK, &INFO);
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK);
  Tgelsy(&M, &N, &NRHS, A, &LDA, &B, &LDB, &JPVT, &RCOND,
	 &RANK, &WORK, &LWORK, &RWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N)
      WarningMessage(QString("Matrix is rank deficient to machine precision.  RANK = %1\n").arg(RANK));
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) 
      WarningMessage(QString("Matrix is rank deficient to machine precision.  RANK = %1\n").arg(RANK));
  changeStride(c,2*n,&B,2*Bsize,2*n,k);
}


template <typename T>
static Array SolveLeastSquaresReal(BasicArray<T> A, BasicArray<T> B) {
  BasicArray<T> C(NTuple(A.cols(),B.cols()));
  realSolveLeastSq(int(A.rows()),int(A.cols()),int(B.cols()),C.data(),A.data(),B.data());
  return Array(C);
}

template <typename T>
static Array SolveLeastSquaresComplex(BasicArray<T> A, BasicArray<T> B) {
  BasicArray<T> C(NTuple(2*A.cols(),B.cols()));
  complexSolveLeastSq(int(A.rows()/2),int(A.cols()),int(B.cols()),C.data(),A.data(),B.data());
  return Array(SplitReal<T>(C),SplitImag<T>(C));
}

// MxN * N x K = M x K
Array SolveLeastSquares(const Array & Ain, const Array &Bin) {
  if (Ain.rows() != Bin.rows()) 
    throw Exception("Solving Ax=B in a least squares sense requires the number of rows in A and B to be the same.");
  DataClass via_type;
  DataClass out_type;
  ComputeTypes(Ain,Bin,via_type,out_type);
  Array A = Ain.toClass(via_type);
  Array B = Bin.toClass(via_type);
  if (AnyNotFinite(A) || AnyNotFinite(B))
    throw Exception("Solving Ax=b in a least squares sense does not currently support non-finite entries in A or B");
  switch (A.dataClass()) {
  default:
    throw Exception("Unsupported data type for linear equation solver");
  case Float:
    if (A.allReal() && B.allReal())
      return SolveLeastSquaresReal<float>(A.constReal<float>(),B.constReal<float>()).toClass(out_type);
    else
      return SolveLeastSquaresComplex<float>(A.fortran<float>(),B.fortran<float>()).toClass(out_type);
  case Double:
    if (A.allReal() && B.allReal())
      return SolveLeastSquaresReal<double>(A.constReal<double>(),B.constReal<double>()).toClass(out_type);
    else
      return SolveLeastSquaresComplex<double>(A.fortran<double>(),B.fortran<double>()).toClass(out_type);
  }
  return Array();
}
