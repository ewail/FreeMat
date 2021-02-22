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
#include <QtCore>


template <typename T>
static void Tgesvd(char* JOBU, char *JOBV, int* M, int *N, T* A, int *LDA, T *S, 
		   T *U, int *LDU, T *VT, int *LDVT, T *WORK,
		   int *LWORK, int *INFO, ftnlen l1, ftnlen l2);

template <>
void Tgesvd(char* JOBU, char* JOBV, int* M, int *N, float* A, int *LDA, float *S, 
		   float *U, int *LDU, float *VT, int *LDVT, float *WORK,
		   int *LWORK, int *INFO, ftnlen l1, ftnlen l2) {
  sgesvd_(JOBU,JOBV,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,INFO,l1,l2);
}

template <>
void Tgesvd(char* JOBU, char* JOBV, int* M, int *N, double* A, int *LDA, double *S, 
		   double *U, int *LDU, double *VT, int *LDVT, double *WORK,
		   int *LWORK, int *INFO, ftnlen l1, ftnlen l2) {
  dgesvd_(JOBU,JOBV,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,INFO,l1,l2);
}

template <typename T>
static void Tgesvd(char* JOBU, char *JOBV, int* M, int *N, T* A, int *LDA, T *S, 
		   T *U, int *LDU, T *VT, int *LDVT, T *WORK,
		   int *LWORK, T *RWORK, int *INFO, ftnlen l1, ftnlen l2);

template <>
void Tgesvd(char* JOBU, char *JOBV, int* M, int *N, float* A, int *LDA, float *S, 
		   float *U, int *LDU, float *VT, int *LDVT, float *WORK,
		   int *LWORK, float *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  cgesvd_(JOBU,JOBV,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Tgesvd(char* JOBU, char *JOBV, int* M, int *N, double* A, int *LDA, double *S, 
		   double *U, int *LDU, double *VT, int *LDVT, double *WORK,
		   int *LWORK, double *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  zgesvd_(JOBU,JOBV,M,N,A,LDA,S,U,LDU,VT,LDVT,WORK,LWORK,RWORK,INFO,l1,l2);
}


template <typename T>
static void TSVD(int nrows, int ncols, BasicArray<T> &U, BasicArray<T> &VT, 
		 BasicArray<T> &S, BasicArray<T> &A, bool compact, bool vectors) {
  if (nrows*ncols == 0) return;
  // Here are the comments from the LAPACK routine SGESVD
  //      SUBROUTINE SGESVD( JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT,
  //     $                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     October 31, 1999
  //*
  //*     .. Scalar Arguments ..
  //      CHARACTER          JOBU, JOBVT
  //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), S( * ), U( LDU, * ),
  //     $                   VT( LDVT, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGESVD computes the singular value decomposition (SVD) of a real
  //*  M-by-N matrix A, optionally computing the left and/or right singular
  //*  vectors. The SVD is written
  //*
  //*       A = U * SIGMA * transpose(V)
  //*
  //*  where SIGMA is an M-by-N matrix which is zero except for its
  //*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
  //*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
  //*  are the singular values of A; they are real and non-negative, and
  //*  are returned in descending order.  The first min(m,n) columns of
  //*  U and V are the left and right singular vectors of A.
  //*
  //*  Note that the routine returns V**T, not V.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  JOBU    (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix U:
  //*          = 'A':  all M columns of U are returned in array U:
  //*          = 'S':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are returned in the array U;
  //*          = 'O':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no columns of U (no left singular vectors) are
  //*                  computed.
  //*
  char JOBU;
  if (!vectors)
    JOBU = 'N';
  else {
    if (!compact)
      JOBU = 'A';
    else
      JOBU = 'S';
  }
  //*  JOBVT   (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix
  //*          V**T:
  //*          = 'A':  all N rows of V**T are returned in the array VT;
  //*          = 'S':  the first min(m,n) rows of V**T (the right singular
  //*                  vectors) are returned in the array VT;
  //*          = 'O':  the first min(m,n) rows of V**T (the right singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no rows of V**T (no right singular vectors) are
  //*                  computed.
  //*
  //*          JOBVT and JOBU cannot both be 'O'.
  //*
  char JOBVT;
  if (!vectors)
    JOBVT = 'N';
  else
    JOBVT = 'A';
  //*  M       (input) INTEGER
  //*          The number of rows of the input matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the input matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit,
  //*          if JOBU = 'O',  A is overwritten with the first min(m,n)
  //*                          columns of U (the left singular vectors,
  //*                          stored columnwise);
  //*          if JOBVT = 'O', A is overwritten with the first min(m,n)
  //*                          rows of V**T (the right singular vectors,
  //*                          stored rowwise);
  //*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
  //*                          are destroyed.
  //*
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;
  //*  S       (output) REAL array, dimension (min(M,N))
  //*          The singular values of A, sorted so that S(i) >= S(i+1).
  //*
  //*  U       (output) REAL array, dimension (LDU,UCOL)
  //*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
  //*          If JOBU = 'A', U contains the M-by-M orthogonal matrix U;
  //*          if JOBU = 'S', U contains the first min(m,n) columns of U
  //*          (the left singular vectors, stored columnwise);
  //*          if JOBU = 'N' or 'O', U is not referenced.
  //*
  //*  LDU     (input) INTEGER
  //*          The leading dimension of the array U.  LDU >= 1; if
  //*          JOBU = 'S' or 'A', LDU >= M.
  //*
  int LDU = nrows;
  //*  VT      (output) REAL array, dimension (LDVT,N)
  //*          If JOBVT = 'A', VT contains the N-by-N orthogonal matrix
  //*          V**T;
  //*          if JOBVT = 'S', VT contains the first min(m,n) rows of
  //*          V**T (the right singular vectors, stored rowwise);
  //*          if JOBVT = 'N' or 'O', VT is not referenced.
  //*
  //*  LDVT    (input) INTEGER
  //*          The leading dimension of the array VT.  LDVT >= 1; if
  //*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).
  //*
  int LDVT = ncols;
  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
  //*          if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged
  //*          superdiagonal elements of an upper bidiagonal matrix B
  //*          whose diagonal is in S (not necessarily sorted). B
  //*          satisfies A = U * B * VT, so it has the same singular values
  //*          as A, and singular vectors related by U and VT.
  T WORKSIZE;
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 1.
  //*          LWORK >= MAX(3*MIN(M,N)+MAX(M,N),5*MIN(M,N)).
  //*          For good performance, LWORK should generally be larger.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit.
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
  //*          > 0:  if SBDSQR did not converge, INFO specifies how many
  //*                superdiagonals of an intermediate bidiagonal form B
  //*                did not converge to zero. See the description of WORK
  //*                above for details.

  int INFO;
  LWORK = -1;
  Tgesvd(&JOBU,&JOBVT,&M,&N,A.data(),&LDA,S.data(),U.data(),&LDU,VT.data(),&LDVT,&WORKSIZE,&LWORK,&INFO,1,1);
  if (INFO < 0)
    WarningMessage(QString("svd (real) had illegal value for parameter (workspace) %1").arg(-INFO));
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tgesvd(&JOBU,&JOBVT,&M,&N,A.data(),&LDA,S.data(),U.data(),&LDU,VT.data(),&LDVT,&WORK,&LWORK,&INFO,1,1);
  if (INFO > 0)
    WarningMessage(QString("svd did not converge"));
  if (INFO < 0)
    WarningMessage(QString("svd (real) had illegal value for parameter %1").arg(-INFO));
}

template <typename T>
static void TSVD(int nrows, int ncols, BasicArray<T> &U, BasicArray<T> &VT, 
		 BasicArray<T> &S, BasicArray<T> &a_real, BasicArray<T> &a_imag, 
		 bool compact, bool vectors) {
  if (nrows*ncols == 0) return;
  // Here are the comments from the LAPACK routine CGESVD
  //
  //      SUBROUTINE CGESVD( JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     October 31, 1999
  //*
  //*     .. Scalar Arguments ..
  //      CHARACTER          JOBU, JOBVT
  //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
  //*     ..
  //*     .. Array Arguments ..
  //      REAL               RWORK( * ), S( * )
  //      COMPLEX            A( LDA, * ), U( LDU, * ), VT( LDVT, * ),
  //     $                   WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CGESVD computes the singular value decomposition (SVD) of a complex
  //*  M-by-N matrix A, optionally computing the left and/or right singular
  //*  vectors. The SVD is written
  //*
  //*       A = U * SIGMA * conjugate-transpose(V)
  //*
  //*  where SIGMA is an M-by-N matrix which is zero except for its
  //*  min(m,n) diagonal elements, U is an M-by-M unitary matrix, and
  //*  V is an N-by-N unitary matrix.  The diagonal elements of SIGMA
  //*  are the singular values of A; they are real and non-negative, and
  //*  are returned in descending order.  The first min(m,n) columns of
  //*  U and V are the left and right singular vectors of A.
  //*
  //*  Note that the routine returns V**H, not V.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  JOBU    (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix U:
  //*          = 'A':  all M columns of U are returned in array U:
  //*          = 'S':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are returned in the array U;
  //*          = 'O':  the first min(m,n) columns of U (the left singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no columns of U (no left singular vectors) are
  //*                  computed.
  //*
  char JOBU;
  if (!vectors)
    JOBU = 'N';
  else {
    if (!compact)
      JOBU = 'A';
    else
      JOBU = 'S';
  }
  //*  JOBVT   (input) CHARACTER*1
  //*          Specifies options for computing all or part of the matrix
  //*          V**H:
  //*          = 'A':  all N rows of V**H are returned in the array VT;
  //*          = 'S':  the first min(m,n) rows of V**H (the right singular
  //*                  vectors) are returned in the array VT;
  //*          = 'O':  the first min(m,n) rows of V**H (the right singular
  //*                  vectors) are overwritten on the array A;
  //*          = 'N':  no rows of V**H (no right singular vectors) are
  //*                  computed.
  //*
  //*          JOBVT and JOBU cannot both be 'O'.
  //*
  char JOBVT;
  if (!vectors)
    JOBVT = 'N';
  else
    JOBVT = 'A';
  //*  M       (input) INTEGER
  //*          The number of rows of the input matrix A.  M >= 0.
  //*
  int M = nrows;
  //*  N       (input) INTEGER
  //*          The number of columns of the input matrix A.  N >= 0.
  //*
  int N = ncols;
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit,
  //*          if JOBU = 'O',  A is overwritten with the first min(m,n)
  //*                          columns of U (the left singular vectors,
  //*                          stored columnwise);
  //*          if JOBVT = 'O', A is overwritten with the first min(m,n)
  //*                          rows of V**H (the right singular vectors,
  //*                          stored rowwise);
  //*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
  //*                          are destroyed.
  //*
  BasicArray<T> A(MergeComplex(a_real,a_imag));
  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).
  //*
  int LDA = nrows;    
  //*  S       (output) REAL array, dimension (min(M,N))
  //*          The singular values of A, sorted so that S(i) >= S(i+1).
  //*
  //*  U       (output) COMPLEX array, dimension (LDU,UCOL)
  //*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
  //*          If JOBU = 'A', U contains the M-by-M unitary matrix U;
  //*          if JOBU = 'S', U contains the first min(m,n) columns of U
  //*          (the left singular vectors, stored columnwise);
  //*          if JOBU = 'N' or 'O', U is not referenced.
  //*
  //*  LDU     (input) INTEGER
  //*          The leading dimension of the array U.  LDU >= 1; if
  //*          JOBU = 'S' or 'A', LDU >= M.
  //*
  int LDU = nrows;    
  //*  VT      (output) COMPLEX array, dimension (LDVT,N)
  //*          If JOBVT = 'A', VT contains the N-by-N unitary matrix
  //*          V**H;
  //*          if JOBVT = 'S', VT contains the first min(m,n) rows of
  //*          V**H (the right singular vectors, stored rowwise);
  //*          if JOBVT = 'N' or 'O', VT is not referenced.
  //*
  //*  LDVT    (input) INTEGER
  //*          The leading dimension of the array VT.  LDVT >= 1; if
  //*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).
  //*
  int LDVT = ncols;
  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
  //*
  T WORKSIZE[2];
  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK. LWORK >= 1.
  //*          LWORK >=  2*MIN(M,N)+MAX(M,N).
  //*          For good performance, LWORK should generally be larger.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.
  //*
  int LWORK;
  //*  RWORK   (workspace) REAL array, dimension (5*min(M,N))
  //*          On exit, if INFO > 0, RWORK(1:MIN(M,N)-1) contains the
  //*          unconverged superdiagonal elements of an upper bidiagonal
  //*          matrix B whose diagonal is in S (not necessarily sorted).
  //*          B satisfies A = U * B * VT, so it has the same singular
  //*          values as A, and singular vectors related by U and VT.
  //*
  int minMN;
  minMN = (M < N) ? M : N;
  MemBlock<T> RWORK(5*minMN);
  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit.
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
  //*          > 0:  if CBDSQR did not converge, INFO specifies how many
  //*                superdiagonals of an intermediate bidiagonal form B
  //*                did not converge to zero. See the description of RWORK
  //*                above for details.
  //*
  int INFO;
  LWORK = -1;
  Tgesvd( &JOBU, &JOBVT, &M, &N, A.data(), &LDA, S.data(), U.data(), &LDU, VT.data(), &LDVT, 
	  WORKSIZE, &LWORK, &RWORK, &INFO, 1, 1);
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK*2);
  Tgesvd( &JOBU, &JOBVT, &M, &N, A.data(), &LDA, S.data(), U.data(), &LDU, VT.data(), &LDVT, 
	  &WORK, &LWORK, &RWORK, &INFO, 1, 1);
  if (INFO > 0)
    WarningMessage(QString("svd did not converge"));
  if (INFO < 0)
    WarningMessage(QString("svd had illegal value for parameter %1").arg(-INFO));
}


template <typename T>
static ArrayVector SVDFunction(BasicArray<T> &A, bool computevectors, bool compactform) {
  int nrows = int(A.rows());
  int ncols = int(A.cols());
  int mindim = (nrows < ncols) ? nrows : ncols;
  // A vector to store the singular values
  BasicArray<T> svals(NTuple(mindim,1));
  // A matrix to store the left singular vectors
  BasicArray<T> umat;
  // A matrix to store the right singular vectors
  BasicArray<T> vtmat;
  if (computevectors) 
    {
      if (!compactform) {
	umat = BasicArray<T>(NTuple(nrows,nrows));
	vtmat = BasicArray<T>(NTuple(ncols,ncols));
      } else {
	umat = BasicArray<T>(NTuple(nrows,mindim));
	vtmat = BasicArray<T>(NTuple(ncols,ncols));
      }
    }
  TSVD<T>(nrows,ncols,umat,vtmat,svals,A,compactform,computevectors);
  ArrayVector retval;
  if (!computevectors) {
    retval.push_back(Array(svals));
  } else {
    retval.push_back(Array(umat));
    BasicArray<T> smat;
    if (!compactform) 
      smat = BasicArray<T>(NTuple(A.rows(),A.cols()));
    else
      smat = BasicArray<T>(NTuple(mindim,A.cols()));
    for (index_t i=1;i<=mindim;i++)
      smat[NTuple(i,i)] = svals[i];
    retval.push_back(Array(smat));
    retval.push_back(Transpose(Array(vtmat)));
  }      
  return retval;
}

template <typename T>
static ArrayVector SVDFunction(BasicArray<T> &A_real, 
			       BasicArray<T> &A_imag,
			       bool computevectors, bool compactform) {
  int nrows = int(A_real.rows());
  int ncols = int(A_real.cols());
  int mindim = (nrows < ncols) ? nrows : ncols;
  // A vector to store the singular values
  BasicArray<T> svals(NTuple(mindim,1));
  // A matrix to store the left singular vectors
  BasicArray<T> umat;
  // A matrix to store the right singular vectors
  BasicArray<T> vtmat;
  if (computevectors) 
    {
      if (!compactform) {
	umat = BasicArray<T>(NTuple(2*nrows,nrows));
	vtmat = BasicArray<T>(NTuple(2*ncols,ncols));
      } else {
	umat = BasicArray<T>(NTuple(2*nrows,mindim));
	vtmat = BasicArray<T>(NTuple(2*ncols,ncols));
      }
    }
  TSVD<T>(nrows,ncols,umat,vtmat,svals,A_real,A_imag,compactform,computevectors);
  ArrayVector retval;
  if (!computevectors) {
    retval.push_back(Array(svals));
  } else {
    retval.push_back(Array(SplitReal(umat),SplitImag(umat)));
    BasicArray<T> smat_real;
    if (!compactform) {
      smat_real = BasicArray<T>(NTuple(A_real.rows(),A_real.cols()));
    } else {
      smat_real = BasicArray<T>(NTuple(mindim,A_real.cols()));
    }
    for (index_t i=1;i<=mindim;i++) {
      smat_real[NTuple(i,i)] = svals[i];
    }
    retval.push_back(Array(smat_real));
    retval.push_back(Hermitian(Array(SplitReal(vtmat),SplitImag(vtmat))));
  }
  return retval;
}

//@@Signature
//function svd SVDFunction jitsafe
//inputs A flag
//outputs U S V
//DOCBLOCK transforms_svd

ArrayVector SVDFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 2)
    throw Exception("svd function takes at most two arguments");
  if (arg.size() < 1)
    throw Exception("svd function requries at least one argument - the matrix to decompose");
  Array A(arg[0].asDenseArray());
  bool compactform = false;
  if (arg.size() > 1) {
    Array flag(arg[1]);
    if (flag.asInteger() == 0)
      compactform = true;
  }
  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply svd to reference types.");
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  if (A.isSparse())
    throw Exception("SVD does not work with sparse matrices.");
  if (AnyNotFinite(A))
    throw Exception("SVD only defined for matrices with finite entries.");
  bool computevectors = (nargout>1);
  switch (A.dataClass()) {
  default: throw Exception("illegal argument type to svd");
  case Float:
    if (A.allReal())
      return SVDFunction(A.real<float>(),computevectors,compactform);
    else
      return SVDFunction(A.real<float>(),A.imag<float>(),computevectors,compactform);
    /*
    // Hack to address apparent bug in single precision SVD routines??
    Array Ad = A.toClass(Double);
    ArrayVector retvec;
    if (Ad.allReal())
    retvec = SVDFunction(Ad.real<double>(),computevectors,compactform);
    else
    retvec = SVDFunction(Ad.real<double>(),A.imag<double>(),computevectors,compactform);
    // Convert the results 
    for (int i=0;i<retvec.size();i++)
    retvec[i] = retvec[i].toClass(Float);
    return retvec;
    }*/
  case Double:
    if (A.allReal())
      return SVDFunction(A.real<double>(),computevectors,compactform);
    else
      return SVDFunction(A.real<double>(),A.imag<double>(),computevectors,compactform);
  }
}
