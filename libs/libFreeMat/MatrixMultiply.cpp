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

#include "MatrixMultiply.hpp"

extern "C" {
  void sgemm_ (char * ta, char* tb, int* m, int* n, int* k, float *alp,
	       const float*A, int* LDA, const float* B, 
	       int* LDB, float* BETA, float *C, int*LDC, int transa_len, int transb_len);
  void cgemm_ (char * ta, char* tb, int* m, int* n, int* k, float *alp,
	       const float*A, int* LDA, const float* B, 
	       int* LDB, float* BETA, float *C, int*LDC, int transa_len, int transb_len);
  void dgemm_ (char * ta, char* tb, int* m, int* n, int* k, double *alp,
	       const double*A, int* LDA, const double* B, 
	       int* LDB, double* BETA, double *C, int*LDC, int transa_len, int transb_len);
  void zgemm_ (char * ta, char* tb, int* m, int* n, int* k, double *alp,
	       const double*A, int* LDA, const double* B, 
	       int* LDB, double* BETA, double *C, int*LDC, int transa_len, int transb_len);
}

template <typename T>
static void Trealgemm(char * ta, char* tb, int* m, int* n, int* k, T *alp,
	       const T*A, int* LDA, const T* B, int* LDB, 
	       T* BETA, T *C, int*LDC);


template <>
void Trealgemm(char * ta, char* tb, int* m, int* n, int* k, float *alp,
	       const float*A, int* LDA, const float* B, int* LDB, 
	       float* BETA, float *C, int*LDC) {
  return sgemm_(ta,tb,m,n,k,alp,A,LDA,B,LDB,BETA,C,LDC, 1, 1);
}

template <>
void Trealgemm(char * ta, char* tb, int* m, int* n, int* k, double *alp,
	       const double*A, int* LDA, const double* B, int* LDB, 
	       double* BETA, double *C, int*LDC) {
  return dgemm_(ta,tb,m,n,k,alp,A,LDA,B,LDB,BETA,C,LDC, 1, 1);
}


template <typename T>
static void Tcomplexgemm(char * ta, char* tb, int* m, int* n, int* k, T *alp,
		  const T*A, int* LDA, const T* B, int* LDB, 
		  T* BETA, T *C, int*LDC);

template <>
void Tcomplexgemm(char * ta, char* tb, int* m, int* n, int* k, float *alp,
		  const float*A, int* LDA, const float* B, int* LDB, 
		  float* BETA, float *C, int*LDC) {
  return cgemm_(ta,tb,m,n,k,alp,A,LDA,B,LDB,BETA,C,LDC, 1, 1);
}

template <>
void Tcomplexgemm(char * ta, char* tb, int* m, int* n, int* k, double *alp,
		  const double*A, int* LDA, const double* B, int* LDB, 
		  double* BETA, double *C, int*LDC) {
  return zgemm_(ta,tb,m,n,k,alp,A,LDA,B,LDB,BETA,C,LDC, 1, 1);
}

/***************************************************************************
 * Matrix-matrix multiply for real arguments
 ***************************************************************************/

template <typename T>
static void realMatrixMatrixMultiply(int m, int n, int k,
			       T* c, const T* a, const T *b) {
  if ((m == 0) || (n == 0) || (k == 0)) return;
  // Use gemm, which computes
  // C = alpha*A*B + beta*C
  //      SUBROUTINE SGEMM ( TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB,
  //     $                   BETA, C, LDC )
  //*     .. Scalar Arguments ..
  //      CHARACTER*1        TRANSA, TRANSB
  //      INTEGER            M, N, K, LDA, LDB, LDC
  //      REAL               ALPHA, BETA
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), B( LDB, * ), C( LDC, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGEMM  performs one of the matrix-matrix operations
  //*
  //*     C := alpha*op( A )*op( B ) + beta*C,
  //*
  //*  where  op( X ) is one of
  //*
  //*     op( X ) = X   or   op( X ) = X',
  //*
  //*  alpha and beta are scalars, and A, B and C are matrices, with op( A )
  //*  an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.
  //*
  //*  Parameters
  //*  ==========
  //*
  //*  TRANSA - CHARACTER*1.
  //*           On entry, TRANSA specifies the form of op( A ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSA = 'N' or 'n',  op( A ) = A.
  //*
  //*              TRANSA = 'T' or 't',  op( A ) = A'.
  //*
  //*              TRANSA = 'C' or 'c',  op( A ) = A'.
  //*
  //*           Unchanged on exit.
  
  char TRANSA = 'N';
  
  //*
  //*  TRANSB - CHARACTER*1.
  //*           On entry, TRANSB specifies the form of op( B ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSB = 'N' or 'n',  op( B ) = B.
  //*
  //*              TRANSB = 'T' or 't',  op( B ) = B'.
  //*
  //*              TRANSB = 'C' or 'c',  op( B ) = B'.
  //*
  //*           Unchanged on exit.
  
  char TRANSB = 'N';
  
  //*
  //*  M      - INTEGER.
  //*           On entry,  M  specifies  the number  of rows  of the  matrix
  //*           op( A )  and of the  matrix  C.  M  must  be at least  zero.
  //*           Unchanged on exit.
  //*
  
  int M = m;

  //*  N      - INTEGER.
  //*           On entry,  N  specifies the number  of columns of the matrix
  //*           op( B ) and the number of columns of the matrix C. N must be
  //*           at least zero.
  //*           Unchanged on exit.
  //*

  int N = n;

  //*  K      - INTEGER.
  //*           On entry,  K  specifies  the number of columns of the matrix
  //*           op( A ) and the number of rows of the matrix op( B ). K must
  //*           be at least  zero.
  //*           Unchanged on exit.
  //*

  int K = k;

  //*  ALPHA  - REAL            .
  //*           On entry, ALPHA specifies the scalar alpha.
  //*           Unchanged on exit.
  //*
  
  T ALPHA = 1;
  
  //*  A      - REAL             array of DIMENSION ( LDA, ka ), where ka is
  //*           k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.
  //*           Before entry with  TRANSA = 'N' or 'n',  the leading  m by k
  //*           part of the array  A  must contain the matrix  A,  otherwise
  //*           the leading  k by m  part of the array  A  must contain  the
  //*           matrix A.
  //*           Unchanged on exit.
  //*

  //*  LDA    - INTEGER.
  //*           On entry, LDA specifies the first dimension of A as declared
  //*           in the calling (sub) program. When  TRANSA = 'N' or 'n' then
  //*           LDA must be at least  max( 1, m ), otherwise  LDA must be at
  //*           least  max( 1, k ).
  //*           Unchanged on exit.
  //*

  int LDA = m;

  //*  B      - REAL             array of DIMENSION ( LDB, kb ), where kb is
  //*           n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.
  //*           Before entry with  TRANSB = 'N' or 'n',  the leading  k by n
  //*           part of the array  B  must contain the matrix  B,  otherwise
  //*           the leading  n by k  part of the array  B  must contain  the
  //*           matrix B.
  //*           Unchanged on exit.
  //*
  
  //*  LDB    - INTEGER.
  //*           On entry, LDB specifies the first dimension of B as declared
  //*           in the calling (sub) program. When  TRANSB = 'N' or 'n' then
  //*           LDB must be at least  max( 1, k ), otherwise  LDB must be at
  //*           least  max( 1, n ).
  //*           Unchanged on exit.
  //*

  int LDB = k;

  //*  BETA   - REAL            .
  //*           On entry,  BETA  specifies the scalar  beta.  When  BETA  is
  //*           supplied as zero then C need not be set on input.
  //*           Unchanged on exit.
  //*
  
  T BETA = 0.0f;
  
  //*  C      - REAL             array of DIMENSION ( LDC, n ).
  //*           Before entry, the leading  m by n  part of the array  C must
  //*           contain the matrix  C,  except when  beta  is zero, in which
  //*           case C need not be set on entry.
  //*           On exit, the array  C  is overwritten by the  m by n  matrix
  //*           ( alpha*op( A )*op( B ) + beta*C ).
  //*

  T *C = c;

  //*  LDC    - INTEGER.
  //*           On entry, LDC specifies the first dimension of C as declared
  //*           in  the  calling  (sub)  program.   LDC  must  be  at  least
  //*           max( 1, m ).
  //*           Unchanged on exit.
  //*
  
  int LDC = m;

  Trealgemm( &TRANSA, &TRANSB, &M, &N, &K, &ALPHA, a, &LDA, b, &LDB,
	  &BETA, C, &LDC );
}


/***************************************************************************
 * Matrix-matrix multiply for complex arguments
 ***************************************************************************/

template <typename T>
static void complexMatrixMatrixMultiply(int m, int n, int k,
				 T* c, const T* a, const T*b) {
  if ((m == 0) || (n == 0) || (k == 0)) return;
  // Use gemm, which computes
  // C = alpha*A*B + beta*C

  //      SUBROUTINE CGEMM ( TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB,
  //     $                   BETA, C, LDC )
  //*     .. Scalar Arguments ..
  //      CHARACTER*1        TRANSA, TRANSB
  //      INTEGER            M, N, K, LDA, LDB, LDC
  //      COMPLEX               ALPHA, BETA
  //*     .. Array Arguments ..
  //      COMPLEX               A( LDA, * ), B( LDB, * ), C( LDC, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGEMM  performs one of the matrix-matrix operations
  //*
  //*     C := alpha*op( A )*op( B ) + beta*C,
  //*
  //*  where  op( X ) is one of
  //*
  //*     op( X ) = X   or   op( X ) = X',
  //*
  //*  alpha and beta are scalars, and A, B and C are matrices, with op( A )
  //*  an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.
  //*
  //*  Parameters
  //*  ==========
  //*
  //*  TRANSA - CHARACTER*1.
  //*           On entry, TRANSA specifies the form of op( A ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSA = 'N' or 'n',  op( A ) = A.
  //*
  //*              TRANSA = 'T' or 't',  op( A ) = A'.
  //*
  //*              TRANSA = 'C' or 'c',  op( A ) = A'.
  //*
  //*           Unchanged on exit.
  
  char TRANSA = 'N';
  
  //*
  //*  TRANSB - CHARACTER*1.
  //*           On entry, TRANSB specifies the form of op( B ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSB = 'N' or 'n',  op( B ) = B.
  //*
  //*              TRANSB = 'T' or 't',  op( B ) = B'.
  //*
  //*              TRANSB = 'C' or 'c',  op( B ) = B'.
  //*
  //*           Unchanged on exit.
  
  char TRANSB = 'N';
  
  //*
  //*  M      - INTEGER.
  //*           On entry,  M  specifies  the number  of rows  of the  matrix
  //*           op( A )  and of the  matrix  C.  M  must  be at least  zero.
  //*           Unchanged on exit.
  //*
  
  int M = m;

  //*  N      - INTEGER.
  //*           On entry,  N  specifies the number  of columns of the matrix
  //*           op( B ) and the number of columns of the matrix C. N must be
  //*           at least zero.
  //*           Unchanged on exit.
  //*

  int N = n;

  //*  K      - INTEGER.
  //*           On entry,  K  specifies  the number of columns of the matrix
  //*           op( A ) and the number of rows of the matrix op( B ). K must
  //*           be at least  zero.
  //*           Unchanged on exit.
  //*

  int K = k;

  //*  ALPHA  - COMPLEX            .
  //*           On entry, ALPHA specifies the scalar alpha.
  //*           Unchanged on exit.
  //*
  
  T ALPHA[2];
  ALPHA[0] = 1.0;
  ALPHA[1] = 0.0;
  
  //*  A      - COMPLEX             array of DIMENSION ( LDA, ka ), where ka is
  //*           k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.
  //*           Before entry with  TRANSA = 'N' or 'n',  the leading  m by k
  //*           part of the array  A  must contain the matrix  A,  otherwise
  //*           the leading  k by m  part of the array  A  must contain  the
  //*           matrix A.
  //*           Unchanged on exit.
  //*

  
  //*  LDA    - INTEGER.
  //*           On entry, LDA specifies the first dimension of A as declared
  //*           in the calling (sub) program. When  TRANSA = 'N' or 'n' then
  //*           LDA must be at least  max( 1, m ), otherwise  LDA must be at
  //*           least  max( 1, k ).
  //*           Unchanged on exit.
  //*

  int LDA = m;

  //*  B      - COMPLEX             array of DIMENSION ( LDB, kb ), where kb is
  //*           n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.
  //*           Before entry with  TRANSB = 'N' or 'n',  the leading  k by n
  //*           part of the array  B  must contain the matrix  B,  otherwise
  //*           the leading  n by k  part of the array  B  must contain  the
  //*           matrix B.
  //*           Unchanged on exit.
  //*
  
  
  //*  LDB    - INTEGER.
  //*           On entry, LDB specifies the first dimension of B as declared
  //*           in the calling (sub) program. When  TRANSB = 'N' or 'n' then
  //*           LDB must be at least  max( 1, k ), otherwise  LDB must be at
  //*           least  max( 1, n ).
  //*           Unchanged on exit.
  //*

  int LDB = k;

  //*  BETA   - COMPLEX            .
  //*           On entry,  BETA  specifies the scalar  beta.  When  BETA  is
  //*           supplied as zero then C need not be set on input.
  //*           Unchanged on exit.
  //*
  
  T BETA[2];  
  BETA[0] = 0.0;
  BETA[1] = 0.0;
  
  //*  C      - COMPLEX             array of DIMENSION ( LDC, n ).
  //*           Before entry, the leading  m by n  part of the array  C must
  //*           contain the matrix  C,  except when  beta  is zero, in which
  //*           case C need not be set on entry.
  //*           On exit, the array  C  is overwritten by the  m by n  matrix
  //*           ( alpha*op( A )*op( B ) + beta*C ).
  //*

  T *C = c;

  //*  LDC    - INTEGER.
  //*           On entry, LDC specifies the first dimension of C as declared
  //*           in  the  calling  (sub)  program.   LDC  must  be  at  least
  //*           max( 1, m ).
  //*           Unchanged on exit.
  //*
  
  int LDC = m;

  Tcomplexgemm( &TRANSA, &TRANSB, &M, &N, &K, ALPHA, a, &LDA, b, &LDB,
	  BETA, C, &LDC);
}


template <typename T>
static Array realMatrixMultiply(const BasicArray<T> &A, const BasicArray<T> &B) {
  BasicArray<T> C(NTuple(A.rows(),B.cols()));
  realMatrixMatrixMultiply<T>(int(A.rows()),int(B.cols()),int(A.cols()),C.data(),A.constData(),B.constData());
  return Array(C);
}

template <typename T>
static Array complexMatrixMultiply(const BasicArray<T> &A, const BasicArray<T> &B) {
  BasicArray<T> C(NTuple(A.rows(),B.cols()));
  complexMatrixMatrixMultiply<T>(int(A.rows()/2),int(B.cols()),int(A.cols()),C.data(),A.constData(),B.constData());
  return Array(SplitReal(C),SplitImag(C));
}
  
Array MatrixMultiply(const Array &A, const Array &B) {
  switch (A.dataClass()) {
  default: throw Exception("Unhandled case for matrix matrix multiply");
  case Float:
    if (A.allReal() && B.allReal()) 
      return realMatrixMultiply(A.constReal<float>(),B.constReal<float>());
    else 
      return complexMatrixMultiply(A.fortran<float>(),B.fortran<float>());
  case Double:
    if (A.allReal() && B.allReal())
      return realMatrixMultiply(A.constReal<double>(),B.constReal<double>());
    else 
      return complexMatrixMultiply(A.fortran<double>(),B.fortran<double>());
  }
}
