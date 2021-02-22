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

#include "EigenDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "MemPtr.hpp"
#include "Complex.hpp"
#include "Math.hpp"
#include "Algorithms.hpp"
#include "SparseCCS.hpp"

#if HAVE_UMFPACK
extern "C" {
#include "umfpack.h"
}
#endif

#define MAX(a,b) ((a) > (b) ? (a) : (b))

template <typename T>
void Tsyev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
	   T *W, T *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len);

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	   float *W, float *WORK, int *LWORK, int *INFO, 
	   ftnlen jobz_len, ftnlen uplo_len) {
  return ssyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

template <>
void Tsyev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	   double *W, double *WORK, int *LWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len) {
  return dsyev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,INFO,jobz_len,uplo_len);
}

template <typename T>
static void Tgeevx(char * BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, T* A, int* LDA, T* W, T* VL, 
		   int *LDVL, T* VR, int *LDVR, int *ILO,
		   int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
		   T* RCONDV, T *WORK, int *LWORK, T *RWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, float* A, int* LDA, float* W, float* VL, 
	    int *LDVL, float* VR, int *LDVR, int *ILO,
	    int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	    float* RCONDV, float *WORK, int *LWORK, float *RWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return cgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, double* A, int* LDA, double* W, double* VL, 
	    int *LDVL, double* VR, int *LDVR, int *ILO,
	    int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	    double* RCONDV, double *WORK, int *LWORK, double *RWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return zgeevx_( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W,
		  VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
		  RCONDE, RCONDV, WORK, LWORK, RWORK, INFO, 
		  balanc_len, jobvl_len, jobvr_len, sense_len );
}

template <typename T>
static void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
		   int* N, T* A, int* LDA, T* WR, T* WI,
		   T* VL, int *LDVL, T* VR, int *LDVR, int *ILO,
		   int *IHI, T* SCALE, T* ABNRM, T* RCONDE,
		   T* RCONDV, T *WORK, int *LWORK, int *IWORK,
		   int *INFO, ftnlen balanc_len, 
		   ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, float* A, int* LDA, float* WR, float* WI,
	    float* VL, int *LDVL, float* VR, int *LDVR, int *ILO,
	    int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	    float* RCONDV, float *WORK, int *LWORK, int *IWORK,
	    int *INFO, ftnlen balanc_len, 
	    ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return sgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO, 
		 balanc_len, jobvl_len, jobvr_len, sense_len);
}

template <>
void Tgeevx(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	    int* N, double* A, int* LDA, double* WR, double* WI,
	    double* VL, int *LDVL, double* VR, int *LDVR, int *ILO,
	    int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	    double* RCONDV, double *WORK, int *LWORK, int *IWORK,
	    int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len) {
  return dgeevx_(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,WR,WI,
		 VL,LDVL,VR,LDVR,ILO,IHI,SCALE,ABNRM,RCONDE,
		 RCONDV,WORK,LWORK,IWORK,INFO, 
		 balanc_len, jobvl_len, jobvr_len, sense_len);
}

template <typename T>
void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	   T *B, int *LDB, T *ALPHAR, T *ALPHAI,
	   T *BETA, T *VL, int *LDVL, T *VR, 
	   int *LDVR, T *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 );

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	   float *B, int *LDB, float *ALPHAR, float *ALPHAI,
	   float *BETA, float *VL, int *LDVL, float *VR, 
	   int *LDVR, float *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return sggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO,l1,l2);
}

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	   double *B, int *LDB, double *ALPHAR, double *ALPHAI,
	   double *BETA, double *VL, int *LDVL, double *VR, 
	   int *LDVR, double *WORK, int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return dggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHAR,ALPHAI,
		BETA,VL,LDVL,VR,LDVR,WORK,LWORK,INFO,l1,l2);
}

template <typename T>
void Tggev(char *JOBVL, char *JOBVR, int *N, T *A, int *LDA, 
	   T *B, int *LDB, T *ALPHA, T *BETA, 
	   T *VL, int *LDVL, T *VR, int *LDVR, 
	   T *WORK, int *LWORK, T *RWORK, int *INFO, ftnlen l1, ftnlen l2 );

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	   float *B, int *LDB, float *ALPHA, float *BETA, 
	   float *VL, int *LDVL, float *VR, int *LDVR, 
	   float *WORK, int *LWORK, float *RWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return cggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Tggev(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	   double *B, int *LDB, double *ALPHA, double *BETA, 
	   double *VL, int *LDVL, double *VR, int *LDVR, 
	   double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return zggev_(JOBVL,JOBVR,N,A,LDA,B,LDB,ALPHA,BETA,VL,LDVL,VR,LDVR,
		WORK,LWORK,RWORK,INFO,l1,l2);  
}

template <typename T>
static void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
		  int *LWORK, int *INFO , ftnlen l1, ftnlen l2);

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	   int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return ssygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO,l1,l2);
}

template <>
void Tsygv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	   int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, int *INFO, ftnlen l1, ftnlen l2 ) {
  return dsygv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,INFO,l1,l2);
}

template <typename T>
static void Theev(char *JOBZ, char *UPLO, int *N, T *A, int *LDA, 
		  T *W, T *WORK, int *LWORK, T *RWORK, int *INFO, ftnlen l1, ftnlen l2);

template <>
void Theev(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
		  float *W, float *WORK, int *LWORK, float *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  return cheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Theev(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
		  double *W, double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen l1, ftnlen l2) {
  return zheev_(JOBZ,UPLO,N,A,LDA,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <typename T>
static void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, T *A, 
		  int *LDA, T *B, int *LDB, T *W, T *WORK,
	   int *LWORK, T *RWORK, int *INFO , ftnlen l1, ftnlen l2);

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
		  int *LDA, float *B, int *LDB, float *W, float *WORK,
	   int *LWORK, float *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return chegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <>
void Thegv(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
		  int *LDA, double *B, int *LDB, double *W, double *WORK,
	   int *LWORK, double *RWORK, int *INFO , ftnlen l1, ftnlen l2) {
  return zhegv_(ITYPE,JOBZ,UPLO,N,A,LDA,B,LDB,W,WORK,LWORK,RWORK,INFO,l1,l2);
}

template <typename T>
static void realEigenDecomposeSymmetric(int n, T *v, T *d, T *a,
					bool eigenvectors) {
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *Ain = a;
  int LDA = n;
  int INFO;
  T WORKSZE;
  int LWORK;
  LWORK = -1;
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORKSZE, &LWORK, &INFO, 1, 1);
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tsyev(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORK, &LWORK, &INFO, 1, 1);
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(T));
}

template <typename T>
static void realEigenDecompose(int n, T *v, T *d, T *a, 
			       bool eigenvectors, bool balance) {
  char BALANC;
  if (balance)
    BALANC = 'B';
  else
    BALANC = 'N';
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  char SENSE = 'N';
  int N = n;
  T *Ain = a;
  int LDA = n;
  MemBlock<T> WR(n);
  MemBlock<T> WI(n);
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  int ILO;
  int IHI;
  MemBlock<T> SCALE(n);
  T ABNRM;
  MemBlock<T> RCONDE(n);
  MemBlock<T> RCONDV(n);
  int LWORK;
  T WORKSZE;
  MemBlock<int> IWORK(2*n-2);
  int INFO;

  LWORK = -1;
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORKSZE, &LWORK, &IWORK, &INFO,1,1,1,1 );

  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, &WR, &WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &IWORK, &INFO,1,1,1,1 );
  for (int i=0;i<N;i++) {
    d[2*i] = WR[i];
    d[2*i+1] = WI[i];
  }
}

template <typename T>
static void realGenEigenDecompose(int n, T *v, T *d, T *a,
				  T *b, bool eigenvectors) {
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  MemBlock<T> ALPHAR(n);
  MemBlock<T> ALPHAI(n);
  MemBlock<T> BETA(n);
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  T WORKSZE;
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORKSZE, &LWORK, &INFO,1,1 );
  LWORK = (int) WORKSZE;
  MemBlock<T> WORK(LWORK);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHAR, &ALPHAI,
	 &BETA, VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &INFO,1,1 );
  int i;
  for (i=0;i<n;i++) {
    d[2*i] = ALPHAR[i]/BETA[i];
    d[2*i+1] = ALPHAI[i]/BETA[i];
  }
}

template <typename T>  
static bool realGenEigenDecomposeSymmetric(int n, T *v, T *d,
					   T *a, T *b, bool eigenvectors) {
  int ITYPE = 1;
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  T *W = d;
  T WORKSIZE;
  int LWORK = -1;
  int INFO;
  Tsygv( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORKSIZE, 
	 &LWORK, &INFO,1,1 );
  LWORK = (int) WORKSIZE;
  MemBlock<T> WORK(LWORK);
  Tsygv( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORK, 
	 &LWORK, &INFO,1,1 );
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,n*n*sizeof(T));
  return true;
}

template <typename T>
static void complexEigenDecomposeSymmetric(int n, T *v, T *d, T *a, 
					   bool eigenvectors) {
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  int LDA = n;
  MemBlock<T> RWORK(MAX(1,3*N-2));
  int LWORK;
  int INFO;
  T WORKSZE[2];
  LWORK = -1;
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO,1,1);
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Theev(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1,1);
  if (eigenvectors)
    memcpy(v,a,2*n*n*sizeof(T));
}

template <typename T>
static void complexEigenDecompose(int n, T *v, T *d, T *a,
				  bool eigenvectors, bool balance) {
  char BALANC;
  if (balance)
    BALANC = 'B';
  else
    BALANC = 'N';
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  char SENSE = 'N';
  int N = n;
  T *Ain = a;
  int LDA = n;
  T *W = d;
  T *VL = NULL;
  int LDVL = 1;
  T *VR = v;
  int LDVR = n;
  int ILO;
  int IHI;
  MemBlock<T> SCALE(n);
  T ABNRM;
  MemBlock<T> RCONDE(n);
  MemBlock<T> RCONDV(n);
  int LWORK;
  MemBlock<T> RWORK(2*n);
  int INFO;
  T WORKSZE[2];
  LWORK = -1;
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, WORKSZE, &LWORK, &RWORK, &INFO,1,1,1,1 );
  LWORK = (int) WORKSZE[0];
  MemBlock<T> WORK(2*LWORK);
  Tgeevx<T>( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, &SCALE, &ABNRM,
	     &RCONDE, &RCONDV, &WORK, &LWORK, &RWORK, &INFO,1,1,1,1 );
}

template <typename T>
static void complexGenEigenDecompose(int n, T *v, T *d, T *a, T *b, 
				     bool eigenvectors) {
  char JOBVL = 'N';
  char JOBVR;
  if (eigenvectors)
    JOBVR = 'V';
  else
    JOBVR = 'N';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = N;
  MemBlock<T> ALPHA(2*n);
  MemBlock<T> BETA(2*n);
  T *VL = NULL;
  int LDVL = n;
  T *VR = v;
  int LDVR = n;
  MemBlock<T> RWORK(8*n);
  T WORKSIZE[2];
  int LWORK = -1;
  int INFO;
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORKSIZE[0], &LWORK, &RWORK, &INFO,1,1 );
  LWORK = (int) WORKSIZE[0];
  MemBlock<T> WORK(LWORK*2);
  Tggev( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, &ALPHA, &BETA,
	 VL, &LDVL, VR, &LDVR, &WORK, &LWORK, &RWORK, &INFO,1,1 );
  int i;
  for (i=0;i<n;i++) 
    complex_divide<T>(ALPHA[2*i],ALPHA[2*i+1],
		      BETA[2*i],BETA[2*i+1],
		      d[2*i],d[2*i+1]);
}

template <typename T>
static bool complexGenEigenDecomposeSymmetric(int n, T *v, T *d, T *a, T *b, 
					      bool eigenvectors) {
  int ITYPE = 1;
  char JOBZ;
  if (eigenvectors)
    JOBZ = 'V';
  else
    JOBZ = 'N';
  char UPLO = 'U';
  int N = n;
  T *A = a;
  int LDA = n;
  T *B = b;
  int LDB = n;
  T *W = d;
  MemBlock<T> RWORK(MAX(1,3*N-2));
  int INFO;
  int LWORK;
  LWORK = MAX(1,2*N-1);
  MemBlock<T> WORK(2*LWORK);
  Thegv(&ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORK,
	&LWORK, &RWORK, &INFO,1,1 );    
  if (INFO>N) return false;
  if (eigenvectors)
    memcpy(v,a,2*n*n*sizeof(T));
  return true;

}

/**
 * Eigen decomposition, symmetric matrix, compact decomposition case
 */
template <typename T>
static void EigenDecomposeCompactSymmetric(Array &D, Array A) {
  index_t N = A.rows();
  NTuple Vdims(N,1);
  if (A.allReal()) {
    D = Array(GetDataClass<T>(T(0)),Vdims);
    realEigenDecomposeSymmetric(int(N), (T*) NULL, D.real<T>().data(), 
				A.real<T>().data(),false);
  } else {
    D = Array(GetDataClass<T>(T(0)),Vdims);
    complexEigenDecomposeSymmetric(int(N), (T*) NULL, D.real<T>().data(),
				   A.fortran<T>().data(),false);
  }
}

void EigenDecomposeCompactSymmetric(Array A, Array& D) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Create one square matrix to store the eigenvectors
  switch (A.dataClass()) {
  default: 
    throw Exception("Unhandled type for symmetric eigendecomposition");
  case Float:
    EigenDecomposeCompactSymmetric<float>(D,A);
    break;
  case Double:
    EigenDecomposeCompactSymmetric<double>(D,A);
    break;
  }
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */
template <typename T>
static inline void EigenDecomposeFullSymmetric(Array &V, Array &D, Array A) {
  index_t N = A.rows();
  NTuple Vdims(N,N);
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(N,1));
    V = Array(GetDataClass<T>(T(0)),Vdims);
    realEigenDecomposeSymmetric(int(N), V.real<T>().data(), 
				eigenvals.data(), 
				A.real<T>().data(),
				true);
    D = DiagonalArray(eigenvals);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    BasicArray<T> Vr(NTuple(2*N,N));
    complexEigenDecomposeSymmetric(int(N), Vr.data(),
				   eigenvals.data(),
				   A.fortran<T>().data(),
				   true);
    V = Array(SplitReal(Vr),SplitImag(Vr));
    D = DiagonalArray(eigenvals);
  }
}

void EigenDecomposeFullSymmetric(const Array &A, Array& V, Array& D) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    EigenDecomposeFullSymmetric<float>(V,D,A);
    break;
  case Double:
    EigenDecomposeFullSymmetric<double>(V,D,A);
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <typename T>
static void HandleEigenVectorsRealMatrix(BasicArray<T> &eigenvals,
					 BasicArray<T> &Vp, 
					 index_t N, Array &D, Array &V) {
  // Make a pass through the eigenvals, and look for complex eigenvalues.
  bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
  if (!complexEigenvalues) {
    D = DiagonalArray(SplitReal<T>(eigenvals));
    V = Array(Vp);
  } else {
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(GetDataClass<T>(T(0)),NTuple(N,N));
    index_t i = 1;
    while (i <= N) {
      if ((i < N) && (eigenvals[2*i] != 0)) {
	for (index_t j=1;j<=N;j++) {
	  V.set(NTuple(j,i),Array(Vp.get(NTuple(j,i)),Vp.get(NTuple(j,i+1))));
	  V.set(NTuple(j,i+1),Array(Vp.get(NTuple(j,i)),-Vp.get(NTuple(j,i+1))));
	}
	i += 2;
      } else {
	for (index_t j=1;j<=N;j++) 
	  V.set(NTuple(j,i),Array(Vp.get(NTuple(j,i))));
	i++;
      }
    }
  }
}

template <typename T>
static void EigenDecomposeFullGeneral(Array A, Array &V, Array &D,
				      bool balanceFlag) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    // A temporary vector to store the eigenvalues
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    BasicArray<T> Vp(NTuple(N,N));
    realEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
		       A.real<T>().data(), true, balanceFlag);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V);
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    complexEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			  A.fortran<T>().data(), true, balanceFlag);
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
}

void EigenDecomposeFullGeneral(const Array &A, Array& V, Array& D, bool balanceFlag) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float: 
    EigenDecomposeFullGeneral<float>(A,V,D,balanceFlag);
    return;
  case Double:
    EigenDecomposeFullGeneral<double>(A,V,D,balanceFlag);
    return;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T>
static void EigenDecomposeCompactGeneral(Array A, Array& D, 
					 bool balanceFlag) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    realEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
		       A.real<T>().data(),
		       false, balanceFlag);
    bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
    if (!complexEigenvalues) {
      D = Array(SplitReal<T>(eigenvals));
    } else {
      D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    }
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    complexEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
			  A.fortran<T>().data(),
			  false, balanceFlag);
    D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
  }
}

void EigenDecomposeCompactGeneral(Array A, Array& D, bool balanceFlag) {
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (A.rows() != A.columns())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    EigenDecomposeCompactGeneral<float>(A,D,balanceFlag);
    return;
  case Double:
    EigenDecomposeCompactGeneral<double>(A,D,balanceFlag);
    return;
  }
}

template <typename T>
static bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, 
						      Array& D) {
  index_t N = A.rows();
  if (A.allReal()) {
    BasicArray<T> eigenvals(NTuple(N,1));
    if (!realGenEigenDecomposeSymmetric(int(N), (T*) NULL, eigenvals.data(), 
					A.real<T>().data(),
					B.real<T>().data(),false)) {
      return false;
    }
    D = Array(eigenvals);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    if (!complexGenEigenDecomposeSymmetric(int(N), (T*) NULL, eigenvals.data(), 
					   A.fortran<T>().data(),
					   B.fortran<T>().data(),
					   false)) {
      return false;
    }
    D = Array(eigenvals);
  }
  return true;
}

bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, Array& D) {
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeCompactSymmetric<float>(A,B,D);
  case Double:
    return GeneralizedEigenDecomposeCompactSymmetric<double>(A,B,D);
  }
  return false;
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */

template <typename T>
static bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array &V, Array &D) {
  index_t N = A.rows();
  if (A.allReal()) {
      // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(N,1));
    BasicArray<T> Vp(NTuple(N,N));
    if (!realGenEigenDecomposeSymmetric(int(N), Vp.data(), eigenvals.data(), 
					A.real<T>().data(),
					B.real<T>().data(),true))
      return false;
    D = DiagonalArray(eigenvals);
    V = Array(Vp);
  } else {
    BasicArray<T> eigenvals(NTuple(N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    if (!complexGenEigenDecomposeSymmetric(int(N), Vp.data(), eigenvals.data(), 
					   A.fortran<T>().data(),
					   B.fortran<T>().data(),true))
      return false;
    D = DiagonalArray(eigenvals);
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
  return true;
}

bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array& V, Array& D) {
  switch (A.dataClass()) {
  default:
    throw Exception("Unsupported type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeFullSymmetric<float>(A,B,V,D);
  case Double:
    return GeneralizedEigenDecomposeFullSymmetric<double>(A,B,V,D);
  }
  return false;
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */

template <typename T>
static void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array &V, Array &D) {
  index_t N = A.rows();
  if (A.allReal()) {
    // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(2*N,1));
    // For a real matrix, the eigenvectors are stored in a packed
    // format - complex eigenvectors are stored as two successive 
    // columns, corresponding to the real and imaginary parts of
    // the vector.  Successive columns can be used because the 
    // eigenvalues occur in conjugate pairs.
    BasicArray<T> Vp(NTuple(N,N));
    realGenEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			  A.real<T>().data(),B.real<T>().data(),true);
    HandleEigenVectorsRealMatrix(eigenvals, Vp, N, D, V);
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    BasicArray<T> Vp(NTuple(2*N,N));
    complexGenEigenDecompose(int(N), Vp.data(), eigenvals.data(), 
			     A.fortran<T>().data(),
			     B.fortran<T>().data(),true);
    D = DiagonalArray(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    V = Array(SplitReal<T>(Vp),SplitImag<T>(Vp));
  }
}

void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array& V, Array& D) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case Float:
    return GeneralizedEigenDecomposeFullGeneral<float>(A,B,V,D);
  case Double:
    return GeneralizedEigenDecomposeFullGeneral<double>(A,B,V,D);
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
template <typename T>
static void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D) {
  index_t N = A.rows();
  if (A.allReal()) {
    // A temporary vector to store the eigenvalues
    BasicArray<T> eigenvals(NTuple(2*N,1));
    realGenEigenDecompose(int(N), (T*) NULL, eigenvals.data(),
			  A.real<T>().data(),
			  B.real<T>().data(),false);
    // Make a pass through the eigenvals, and look for complex eigenvalues.
    bool complexEigenvalues = MergedArrayHasComplexComponents(eigenvals);
    if (!complexEigenvalues) {
      D = Array(SplitReal<T>(eigenvals));
    } else {
      // Copy the eigenvalues into a complex vector
      D = Array(SplitReal<T>(eigenvals),SplitImag<T>(eigenvals));
    }
  } else {
    BasicArray<T> eigenvals(NTuple(2*N,1));
    complexGenEigenDecompose(int(N), (T*) NULL, eigenvals.data(), 
			     A.fortran<T>().data(), 
			     B.fortran<T>().data(),
			     false);
    D = Array(SplitReal(eigenvals),SplitImag(eigenvals));
  }
}

void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D) {
  // Select the eigenvector decomposition routine based on A's type
  switch (A.dataClass()) {
  default: 
    throw Exception("Unhandled type for eigendecomposition");
  case Float: 
    GeneralizedEigenDecomposeCompactGeneral<float>(A,B,D);
    break;
  case Double:
    GeneralizedEigenDecomposeCompactGeneral<double>(A,B,D);
    break;
  }
}


static void DNEUPARPACKError(int info) {
  if (info == 1) 
    throw Exception("ARPACK Error: The Schur form computed by LAPACK routine dlahqr could not be reordered by LAPACK routine dtrsen.  Please file a bug report with the matrix and arguments that caused this error.");
  if (info == -1)
    throw Exception("N must be positive.");
  if (info == -2)
    throw Exception("NEV must be positive.");
  if (info == -3)
    throw Exception("Too many eigenvalues/eigenvectors requested.");
  if (info == -5)
    throw Exception("WHICH must be one of 'LM', 'SM', 'LR', 'SR', 'LI', 'SI'");
  if (info == -6)
    throw Exception("BMAT must be one of 'I' or 'G'.");
  if (info == -7)
    throw Exception("Length of private work WORKL array is not sufficient.");
  if (info == -8)
    throw Exception("Error return from calculation of a real Schur form.  Informational error from LAPACK routine dlahqr.");
  if (info == -9)
    throw Exception("Error return from calculation of eigenvectors. Onformational error from LAPACK routine dtrevc.");
  if (info == -10)
    throw Exception("IPARAM(7) must be 1,2,3,4.");
  if (info == -11)
    throw Exception("IPARAM(7) = 1 and BMAT = 'G' are incompatible.");
  if (info == -12)
    throw Exception("HOWMNY = 'S' not yet implemented");
  if (info == -13)
    throw Exception("HOWMNY must be one of 'A' or 'P' if RVEC = .true.");
  if (info == -14)
    throw Exception("DNAUPD did not find any eigenvalues to sufficient accuracy.");
  throw Exception("Generic ARPACK error occured in call to dneupd.");
}

static void DNAUPARPACKError(int info) {
  if (info == 1)
    throw Exception("Maximum number of iterations taken.  All possible eigenvalues of OP has been found.");
  if (info == 3)
    throw Exception("No shifts could be applied during a cycle of the implicitly restarted Arnoldi iteration. One possibility is to increase the size of NCV relative to NEV.");
  if (info == -1)
    throw Exception("Problem size must be positive.");
  if (info == -2)
    throw Exception("Number of requested eigenvalues must be positive.");
  if (info == -3)
    throw Exception("Illegal value for number of spanning vectors (ncv) - ARPACK cannot solve for all of the eigenvalues of a matrix (use eig for that).");
  if (info == -4)
    throw Exception("The maximum number of Arnoldi update iteration must be greater than zero.");
  if (info == -5)
    throw Exception("WHICH must be one of 'LM', 'SM', 'LR', 'SR', 'LI', 'SI'");
  if (info == -6)
    throw Exception("BMAT must be one of 'I' or 'G'");
  if (info == -7)
    throw Exception("Length of private work array is not sufficient.");
  if (info == -8)
    throw Exception("Error return from LAPACK eigenvalue calculation;");
  if (info == -9)
    throw Exception("Starting vector is zero.");
  if (info == -10)
    throw Exception("Illegal mode selection for ARPACK dnaup");
  if (info == -11)
    throw Exception("IPARAM(7) = 1 and BMAT = 'G' are incompatible.");
  if (info == -12)
    throw Exception("IPARAM(1) must be equal to 0 or 1.");
  throw Exception("Could not build an Arnoldi factorization.");
}

extern "C" {
  int znaupd_(int *ido, char *bmat, int *n, const char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, double *rwork, 
	      int *info);
  int zneupd_(int *rvec, char *howmny, int *select, 
	      double *d, double *z, int *ldz, 
	      double *sigma, double *workev, char *bmat, 
	      int *n, const char *which, int *nev, double *tol, 
	      double *resid, int *ncv, double *v, int *ldv, int 
	      *iparam, int *ipntr, double *workd, double *workl, 
	      int *lworkl, double *rwork, int *info);
  int dnaupd_(int *ido, char *bmat, int *n, const char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, int *info,
	      int len1, int len2);
  int dneupd_(int *rvec, char *howmny, int *select, 
	      double *dr, double *di, double *z__, int *ldz, 
	      double *sigmar, double *sigmai, double *workev, char *
	      bmat, int *n, const char *which, int *nev, double *tol, 
	      double *resid, int *ncv, double *v, int *ldv, int 
	      *iparam, int *ipntr, double *workd, double *workl, 
	      int *lworkl, int *info);
  int dsaupd_(int *ido, char *bmat, int *n, const char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, int *info);
  int dseupd_(int *rvec, char *howmny, int *select, 
	      double *d, double *z__, int *ldz, 
	      double *sigma, char *bmat, int *n, const char *which, int *nev, double *tol, 
	      double *resid, int *ncv, double *v, int *ldv, int 
	      *iparam, int *ipntr, double *workd, double *workl, 
	      int *lworkl, int *info);
}


#define min(a,b) ((a) < (b) ? (a) : (b))
#define swap(a,b) {double tmp; tmp = a; a = b; b = tmp;}

static void LoadARPACKParams(int iparam[11]) {
  iparam[0] = 1;
  iparam[1] = 0;
  iparam[2] = 300;
  iparam[3] = 1;
  iparam[4] = 0;
  iparam[5] = 0;
  iparam[6] = 1;
  iparam[7] = 0;
  iparam[8] = 0;
  iparam[9] = 0;
  iparam[10] = 0;
}


template <typename T>
static void SparseDenseMatrixVectorMultiply(const SparseMatrix<T> &A, const T* B, T* C) {
  memset(C,0,int(sizeof(T)*A.rows()));
  ConstSparseIterator<T> A_iter(&A);
  while (A_iter.isValid()) {
    C[int(A_iter.row()-1)] += A_iter.value() * B[int(A_iter.col()-1)];
    A_iter.next();
  }
}

template <typename T>
static void SparseDenseMatrixVectorMultiply(const SparseMatrix<T> &A_real,
					    const SparseMatrix<T> &A_imag,
					    const T* B, T* C) {
  int rows = int(A_real.rows());
  memset(C,0,int(sizeof(T)*rows*2));
  ConstComplexSparseIterator<T> A_iter(&A_real,&A_imag);
  while (A_iter.isValid()) {
    T cr, ci;
    complex_multiply(A_iter.realValue(),A_iter.imagValue(),
		     B[2*(int(A_iter.col()-1))],
		     B[2*(int(A_iter.col()-1))+1],cr,ci);
    C[int(2*(A_iter.row()-1))] += cr;
    C[int(2*(A_iter.row()-1))+1] += ci;
    A_iter.next();
  }
}

static ArrayVector SparseDecodeComplexResults(int rvec, int nconv, double *d, double *z, int nev, 
					      int n, int nargout) {
  if (rvec == 0) {
    for (int i=0;i<(nconv)/2;i++) {
      swap(d[2*i],d[2*(nconv-1-i)]);
      swap(d[2*i+1],d[2*(nconv-1-i)+1]);
    }
  }
  BasicArray<double> eigvals_real(NTuple(nev,1));
  BasicArray<double> eigvals_imag(NTuple(nev,1));
  for (int i=0;i<min(nev,nconv);i++) {
    eigvals_real[i+1] = d[2*i];
    eigvals_imag[i+1] = d[2*i+1];
  }
  BasicArray<double> eigvecs_real;
  BasicArray<double> eigvecs_imag;
  if (nargout > 1) {
    eigvecs_real = BasicArray<double>(NTuple(n,nev));
    eigvecs_imag = BasicArray<double>(NTuple(n,nev));
    for (int i=0;i<min(nev,nconv);i++)
      for (int j=0;j<n;j++) {
	eigvecs_real[NTuple(j+1,i+1)] = z[i*2*n+2*j];
	eigvecs_imag[NTuple(j+1,i+1)] = z[i*2*n+2*j+1];
      }
  }
  ArrayVector retval;
  if (nargout <= 1)
    retval.push_back(Array(eigvals_real,eigvals_imag));
  else {
    retval.push_back(Array(eigvecs_real,eigvecs_imag));
    retval.push_back(DiagonalArray(eigvals_real,eigvals_imag));
  }
  free(z);
  return retval;
}

static ArrayVector SparseDecodeResults(int rvec, int nconv, double *dr, double *di, int nev, double *z,
				       int n, int nargout) {
  // Reverse the vectors dr and di
  if (rvec == 0) {
    for (int i=0;i<(nconv)/2;i++) {
      swap(dr[i],dr[nconv-1-i]);
      swap(di[i],di[nconv-1-i]);
    }
  }
  // Check for complex eigenvalues
  bool anycomplex = false;
  for (int i=0;(!anycomplex) && (i<nconv);i++,anycomplex = (di[i] != 0));
  if (anycomplex) {
    BasicArray<double> eigvals_real(NTuple(nev,1));
    BasicArray<double> eigvals_imag(NTuple(nev,1));
    for (int i=0;i<min(nev,nconv);i++) {
      eigvals_real[i+1] = dr[i];
      eigvals_imag[i+1] = di[i];
    }
    BasicArray<double> eigvecs_real;
    BasicArray<double> eigvecs_imag;
    if (nargout > 1) {
      eigvecs_real = BasicArray<double>(NTuple(n,nev));
      eigvecs_imag = BasicArray<double>(NTuple(n,nev));
      // if eigenvalue i is complex, then the corresponding eigenvector
      // should be constructed from columns i and i+1 of z if i is even
      // and columns i-1 and i of z if i is odd
      int vcol = 0;
      while (vcol < min(nconv,nev)) {
	if (di[vcol] != 0) {
	  for (int j=0;j<n;j++) {
	    eigvecs_real[NTuple(j+1,vcol+1)] = z[vcol*n+j];
	    eigvecs_imag[NTuple(j+1,vcol+1)] = z[(vcol+1)*n+j];
	    if ((vcol+1) < nev) {
	      eigvecs_real[NTuple(j+1,vcol+2)] = z[vcol*n+j];
	      eigvecs_imag[NTuple(j+1,vcol+2)] = -z[(vcol+1)*n+j];
	    }
	  }
	  vcol += 2;
	} else {
	  for (int j=0;j<n;j++) {
	    eigvecs_real[NTuple(j+1,vcol+1)] = z[vcol*n+j];
	  }
	  vcol++;
	}
      }
    }
    ArrayVector retval;
    if (nargout <= 1)
      retval.push_back(Array(eigvals_real,eigvals_imag));
    else {
      retval.push_back(Array(eigvecs_real,eigvecs_imag));
      retval.push_back(DiagonalArray(eigvals_real,eigvals_imag));
    }
    free(z);
    return retval;
  } else {
    ArrayVector retval;
    if (nargout <= 1) {
      retval.push_back(Array(BasicArrayFromNative(dr,NTuple(nev,1))));
    }
    else {
      // I know that technically this is a bad thing... dr and z are larger than
      // they need to be, but I don't think this will cause any problems.
      retval.push_back(Array(BasicArrayFromNative(z,NTuple(n,nev))));
      retval.push_back(DiagonalArray(BasicArrayFromNative(dr,NTuple(nev,1))));
    }
    return retval;
  }  
}

static ArrayVector SparseEigDecomposeNonsymmetricReal(const SparseMatrix<double> &a,
						      int nev, int nargout, QString which) {
  // Initialization call
  int ido = 0;
  char bmat = 'I';
  int n = int(a.rows());
  // How many eigenvalues to compute
  char cmach = 'E';
  char *which_cstr = strdup(qPrintable(which));
  double tol = dlamch_(&cmach,1);
  MacroBlockAlloc(double,n,residBlock,resid);
  int ncv = 2*nev+1;
  if (ncv > n) ncv = n;
  MacroBlockAlloc(double,2*n*ncv,vBlock,v);
  int ldv = n;
  int iparam[11];
  LoadARPACKParams(iparam);
  MacroBlockAlloc(double,3*n,workdBlock,workd);
  int lworkl = 3*ncv*ncv+6*ncv;
  MacroBlockAlloc(double,lworkl,worklBlock,workl);
  int info = 0;
  MacroBlockAlloc(int,14,ipntrBlock,ipntr);
  while (1) {
    dnaupd_(&ido, &bmat, &n , which_cstr, &nev, &tol, resid, 
	    &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	    &info,1,2);
    if ((ido == -1) || (ido == 1)) 
      SparseDenseMatrixVectorMultiply(a, workd+ipntr[0]-1, workd+ipntr[1]-1);
    else {
      break;
    }
  }
  if (info < 0)
    DNAUPARPACKError(info);
  // Compute vectors and values
  int rvec;
  if (nargout <= 1)
    rvec = 0;
  else
    rvec = 1;
  char howmny = 'A';
  MacroBlockAlloc(int,ncv,selectBlock,select);
  MacroBlockAlloc(double,nev+1,drb,dr);
  MacroBlockAlloc(double,nev+1,dib,di);
  double *z;
  if (nargout <= 1)
    z = NULL;
  else
    z = (double*) malloc(sizeof(double)*(n*(nev+1)));
  double sigmar;
  double sigmai;
  MacroBlockAlloc(double,3*ncv,workevBlock,workev);
  int ierr;
  dneupd_(&rvec, &howmny, select, dr, di, z, &ldv, 
	  &sigmar, &sigmai, workev, &bmat, &n, which_cstr, &nev, &tol, 
	  resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	  &lworkl, &ierr);
  int nconv = iparam[4];
  if (ierr != 0)
    DNEUPARPACKError(ierr);
  return SparseDecodeResults(rvec,nconv,dr,di,nev,z,n,nargout);
} 


static ArrayVector SparseEigDecomposeSymmetricReal(const SparseMatrix<double> &a,
						   int nev, int nargout, QString which) {
  // Initialization call
  char *which_cstr = strdup(qPrintable(which));
  int ido = 0;
  char bmat = 'I';
  int n = int(a.rows());
  // How many eigenvalues to compute
  char cmach = 'E';
  double tol = dlamch_(&cmach,1);
  MacroBlockAlloc(double,n,residBlock,resid);
  int ncv = 2*nev+1;
  if (ncv > n) ncv = n;
  MacroBlockAlloc(double,n*ncv,vBlock,v);
  int ldv = n;
  int iparam[11];
  LoadARPACKParams(iparam);
  MacroBlockAlloc(double,3*n,workdBlock,workd);
  int lworkl = ncv*ncv+8*ncv;
  MacroBlockAlloc(double,lworkl,worklBlock,workl);
  int info = 0;
  int ipntr[11];
  while (1) {
    dsaupd_(&ido, &bmat, &n , which_cstr, &nev, &tol, resid, 
	    &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	    &info);
    if ((ido == -1) || (ido == 1)) 
      SparseDenseMatrixVectorMultiply(a, workd+ipntr[0]-1, workd+ipntr[1]-1);
    else
      break;
  }
  if (info < 0)
    DNAUPARPACKError(info);
  // Compute vectors and values
  int rvec;
  if (nargout <= 1)
    rvec = 0;
  else
    rvec = 1;
  char howmny = 'A';
  MacroBlockAlloc(int,ncv,selectBlock,select);
  BasicArray<double> db(NTuple(nev,1));
  double *d = db.data();
  BasicArray<double> zb;
  double *z;
  if (nargout <= 1)
    z = NULL;
  else {
    zb = BasicArray<double>(NTuple(n,nev));
    z = zb.data();
  }
  double sigma;
  int ierr;
  dseupd_(&rvec, &howmny, select, d ,z, &ldv, 
	  &sigma,&bmat, &n, which_cstr, &nev, &tol, 
	  resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	  &lworkl, &ierr);
  int nconv = iparam[4];
  if (ierr != 0)
    DNEUPARPACKError(ierr);
  // Reverse the vectors dr and di
  for (int i=0;i<(nconv)/2;i++) 
    swap(d[i],d[nconv-1-i]);
  if (rvec == 1) {
    for (int i=0;i<(nconv)/2;i++)
      for (int j=0;j<n;j++)
	swap(z[i*n+j],z[(nconv-1-i)*n+j]);
  }
  ArrayVector retval;
  // I know that technically this is a bad thing... dr and z are larger than
  // they need to be, but I don't think this will cause any problems.
  if (nargout <= 1) {
    retval.push_back(Array(db));
  } else {
    retval.push_back(Array(zb));
    retval.push_back(DiagonalArray(db));
  }
  return retval;
} 

ArrayVector SparseEigDecomposeNonsymmetricComplex(const SparseMatrix<double> &a_real,
						  const SparseMatrix<double> &a_imag,
						  int nev, int nargout, QString which) {
  // Initialization call
  int ido = 0;
  char bmat = 'I';
  char *which_cstr = strdup(qPrintable(which));
  int n = int(a_real.rows());
  // How many eigenvalues to compute
  char cmach = 'E';
  double tol = dlamch_(&cmach,1);
  MacroBlockAlloc(double,2*n,residBlock,resid);
  int ncv = 2*nev+1;
  if (ncv > n) ncv = n;
  MacroBlockAlloc(double,2*n*ncv,vBlock,v);
  int ldv = n;
  int iparam[11];
  LoadARPACKParams(iparam);
  MacroBlockAlloc(double,2*3*n,workdBlock,workd);
  int lworkl = 3*ncv*ncv+5*ncv;
  MacroBlockAlloc(double,2*lworkl,worklBlock,workl);
  MacroBlockAlloc(double,ncv,rworkBlock,rwork);
  int info = 0;
  int ipntr[14];
  while (1) {
    znaupd_(&ido, &bmat, &n , which_cstr, &nev, &tol, resid, 
	    &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	    rwork, &info);
    if ((ido == -1) || (ido == 1)) 
      SparseDenseMatrixVectorMultiply(a_real, a_imag, workd+2*(ipntr[0]-1), workd+2*(ipntr[1]-1));
    else
      break;
  }
  if (info < 0)
    DNAUPARPACKError(info);
  // Compute vectors and values
  int rvec;
  if (nargout <= 1)
    rvec = 0;
  else
    rvec = 1;
  char howmny = 'A';
  MacroBlockAlloc(int,ncv,selectBlock,select);
  MacroBlockAlloc(double,2*(nev+1),dblock,d);
  double *z;
  if (nargout <= 1)
    z = NULL;
  else
    z = (double*) malloc(2*sizeof(double)*(n*(nev+1)));
  double sigma[2];
  sigma[0] = 0;
  sigma[1] = 0;
  MacroBlockAlloc(double,2*2*ncv,workevBlock,workev);
  int ierr;
  zneupd_(&rvec, &howmny, select, d, z, &ldv, 
	  sigma, workev, &bmat, &n, which_cstr, &nev, &tol, 
	  resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	  &lworkl, rwork, &ierr);
  int nconv = iparam[4];
  if (ierr != 0)
    DNEUPARPACKError(ierr);
  return SparseDecodeComplexResults(rvec,nconv,d,z,nev,n,nargout);
} 

static SparseMatrix<double> MakeSparseScaledIdentityReal(double shift, int size) {
  SparseMatrix<double> ret(NTuple(size,size));
  for (index_t i=1;i<=size;i++)
    ret.set(NTuple(i,i),shift);
  return ret;
}

// For shifted eigendecomposition problems, we have to change the behavior of the
// reverse communication interface.  This is done by changing the operation mode
// to 3, and by solving (A-shift*I)x = b.  Because this equation has to be solved
// multiple times, we calculate a C matrix as A-shift*I, decompose it using the
// UMFPack routines, and then use the result in repeated solutions.
ArrayVector SparseEigDecomposeNonsymmetricRealShifted(const SparseMatrix<double> &A,
						      int nev, int nargout, double shift) {
#if HAVE_UMFPACK
  // Set up the scaled identity matrix
  int rows = int(A.rows());
  Array scI(MakeSparseScaledIdentityReal(shift, rows));
  Array C(Subtract(Array(A),scI));
  QVector<uint32> rowindx, colstart;
  QVector<double> accsdata;
  SparseToCCS(C.constRealSparse<double>(),rowindx,colstart,accsdata);
  // Factor it...
  double *null = (double *) NULL;
  void *Symbolic, *Numeric;
  (void) umfpack_di_symbolic (int(A.cols()), int(A.cols()), (const int*) colstart.data(), 
			      (const int*) rowindx.data(), accsdata.data(), 
			      &Symbolic, null, null);
  (void) umfpack_di_numeric ( (const int*) colstart.data(),  (const int*) rowindx.data(), 
			      accsdata.data(), Symbolic, 
			      &Numeric, null, null) ;
  umfpack_di_free_symbolic (&Symbolic) ;
  // Initialization call
  int ido = 0;
  char bmat = 'I';
  int n = rows;
  const char *which = "LM";
  // How many eigenvalues to compute
  char cmach = 'E';
  double tol = dlamch_(&cmach);
  MacroBlockAlloc(double,n,residBlock,resid);
  int ncv = 2*nev+1;
  if (ncv > n) ncv = n;
  MacroBlockAlloc(double,2*n*ncv,vBlock,v);
  int ldv = n;
  int iparam[11];
  LoadARPACKParams(iparam);
  iparam[6] = 3;
  MacroBlockAlloc(double,3*n,workdBlock,workd);
  int lworkl = 3*ncv*ncv+6*ncv;
  MacroBlockAlloc(double,lworkl,worklBlock,workl);
  int info = 0;
  int ipntr[14];
  while (1) {
    dnaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	    &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	    &info, 1, 2);
    if ((ido == -1) || (ido == 1)) {
      int res = umfpack_di_solve(UMFPACK_A, (const int*) colstart.data(), 
				 (const int*) rowindx.data(), accsdata.data(), 
				 workd+ipntr[1]-1,workd+ipntr[0]-1,Numeric, null, null);
      if (res != 0)
	throw Exception("UMFPACK solution failed!");
    }
    else if (ido == 2)
      memcpy( workd+ipntr[1]-1, workd+ipntr[0]-1, sizeof(double)*rows);
    else
      break;
  }
  // Free the numeric component
  umfpack_di_free_numeric(&Numeric);
  if (info < 0)
    DNAUPARPACKError(info);
  // Compute vectors and values
  int rvec;
  if (nargout <= 1)
    rvec = 0;
  else
    rvec = 1;
  char howmny = 'A';
  MacroBlockAlloc(int,ncv,selectBlock,select);
  //lambda_a = 1/lambda_c + sigma
  MacroBlockAlloc(double,nev+1,drb,dr);
  MacroBlockAlloc(double,nev+1,dib,di);
  double *z;
  if (nargout <= 1)
    z = NULL;
  else
    z = (double*) malloc(sizeof(double)*(n*(nev+1)));
  double sigmar;
  double sigmai;
  sigmar = shift;
  sigmai = 0.0;
  MacroBlockAlloc(double,3*ncv,workevBlock,workev);
  int ierr;
  dneupd_(&rvec, &howmny, select, dr, di, z, &ldv, 
	  &sigmar, &sigmai, workev, &bmat, &n, which, &nev, &tol, 
	  resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	  &lworkl, &ierr);
  int nconv = iparam[4];
  if (ierr != 0)
    DNEUPARPACKError(ierr);
  return SparseDecodeResults(rvec,nconv,dr,di,nev,z,n,nargout);
#else
  throw Exception("Shifted eigendecomposition problems for sparse matrices requires UMFPACK and ARPACK support libraries, which were not available at compile time.  You must have UMFPACK and ARPACK installed at compile time for FreeMat to enable this functionality.");
#endif
} 

// For shifted eigendecomposition problems, we have to change the behavior of the
// reverse communication interface.  This is done by changing the operation mode
// to 3, and by solving (A-shift*I)x = b.  Because this equation has to be solved
// multiple times, we calculate a C matrix as A-shift*I, decompose it using the
// UMFPack routines, and then use the result in repeated solutions.
ArrayVector SparseEigDecomposeNonsymmetricComplexShifted(const SparseMatrix<double> &Areal,
							 const SparseMatrix<double> &Aimag,
							 int nev, int nargout, double *shift) {
#if HAVE_UMFPACK
  // Set up the scaled identity matrix
  int rows = int(Areal.rows());
  Array scI(MakeSparseScaledIdentityReal(shift[0],rows),
	    MakeSparseScaledIdentityReal(shift[1],rows));
  Array C(Subtract(Array(Areal,Aimag),scI));
  QVector<uint32> rowindx, colstart;
  QVector<double> accsreal, accsimag;
  SparseToCCS(C.constRealSparse<double>(),C.constImagSparse<double>(),
	      rowindx,colstart,accsreal,accsimag);
  // Factor it...
  double *null = (double *) NULL ;
  void *Symbolic, *Numeric ;
  int res;
  res = umfpack_zi_symbolic (int(Areal.cols()), int(Areal.cols()), (const int*) colstart.data(),
			     (const int*) rowindx.data(), accsreal.data(), accsimag.data(),
			     &Symbolic, null, null);
  res = umfpack_zi_numeric ((const int*) colstart.data(), (const int*) rowindx.data(),
			    accsreal.data(), accsimag.data(), Symbolic, &Numeric, null, null);
  umfpack_zi_free_symbolic (&Symbolic);    
  // Initialization call
  int ido = 0;
  char bmat = 'I';
  int n = rows;
  const char *which = "LM";
  // How many eigenvalues to compute
  char cmach = 'E';
  double tol = dlamch_(&cmach);
  MacroBlockAlloc(double,2*n,residBlock,resid);
  int ncv = 2*nev+1;
  if (ncv > n) ncv = n;
  MacroBlockAlloc(double,2*n*ncv,vBlock,v);
  int ldv = n;
  int iparam[11];
  LoadARPACKParams(iparam);
  iparam[6] = 3;
  MacroBlockAlloc(double,2*3*n,workdBlock,workd);
  int lworkl = 3*ncv*ncv+5*ncv;
  MacroBlockAlloc(double,2*lworkl,worklBlock,workl);
  MacroBlockAlloc(double,ncv,rworkBlock,rwork);
  MacroBlockAlloc(double,rows,xrBlock,xr);
  MacroBlockAlloc(double,rows,xiBlock,xi);
  MacroBlockAlloc(double,rows,yrBlock,yr);
  MacroBlockAlloc(double,rows,yiBlock,yi);
  int info = 0;
  int ipntr[14];
  while (1) {
    znaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	    &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	    rwork, &info);
    if ((ido == -1) || (ido == 1)) {
      for (int i=0;i<rows;i++) {
	yr[i] = workd[2*(ipntr[0]-1)+2*i];
	yi[i] = workd[2*(ipntr[0]-1)+2*i+1];
      }
      memset(xr,0,sizeof(double)*rows);
      memset(xi,0,sizeof(double)*rows);
      res = umfpack_zi_solve(UMFPACK_A, (const int*) colstart.data(),
			     (const int*) rowindx.data(), 
			     accsreal.data(), accsimag.data(), xr, xi, yr, yi,Numeric, null, null);
      for (int i=0;i<rows;i++) {
	workd[2*(ipntr[1]-1)+2*i] = xr[i];
	workd[2*(ipntr[1]-1)+2*i+1] = xi[i];
      }	
    }
    else if (ido == 2)
      memcpy( workd+2*(ipntr[1]-1), workd+2*(ipntr[0]-1), sizeof(double)*rows*2);
    else
      break;
  }
  // Free the numeric component
  umfpack_zi_free_numeric(&Numeric);
  if (info < 0)
    DNAUPARPACKError(info);
  // Compute vectors and values
  int rvec;
  if (nargout <= 1)
    rvec = 0;
  else
    rvec = 1;
  char howmny = 'A';
  MacroBlockAlloc(int,ncv,selectBlock,select);
  //lambda_a = 1/lambda_c + sigma
  MacroBlockAlloc(double,2*(nev+1),dblock,d);
  double *z;
  if (nargout <= 1)
    z = NULL;
  else
    z = (double*) malloc(2*sizeof(double)*(n*(nev+1)));
  double sigma[2];
  sigma[0] = shift[0];
  sigma[1] = shift[1];
  MacroBlockAlloc(double,2*2*ncv,workevBlock,workev);
  int ierr;
  zneupd_(&rvec, &howmny, select, d, z, &ldv, 
	  sigma, workev, &bmat, &n, which, &nev, &tol, 
	  resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	  &lworkl, rwork, &ierr);
  int nconv = iparam[4];
  if (ierr != 0)
    DNEUPARPACKError(ierr);
  return SparseDecodeComplexResults(rvec,nconv,d,z,nev,n,nargout);
#else
  throw Exception("Shifted eigendecomposition problems for sparse matrices requires UMFPACK and ARPACK support libraries, which were not available at compile time.  You must have UMFPACK and ARPACK installed at compile time for FreeMat to enable this functionality.");
#endif
} 

ArrayVector SparseEigDecompose(int nargout, Array A, int k, QString whichFlag) {
  if (A.isComplex()) {
    return SparseEigDecomposeNonsymmetricComplex(A.constRealSparse<double>(),
						 A.constImagSparse<double>(),
						 k, nargout, whichFlag);
  } else {
    bool symDetect = IsSymmetric(A.constRealSparse<double>());
    if (symDetect)
      return SparseEigDecomposeSymmetricReal(A.constRealSparse<double>(),
					     k, nargout, whichFlag);
    else
      return SparseEigDecomposeNonsymmetricReal(A.constRealSparse<double>(),
						k, nargout, whichFlag);
  }
}

ArrayVector SparseEigDecomposeShifted(int nargout, Array A, int k, double shift[2]) {
  if (shift[1] != 0)
    A.forceComplex();
  if (A.isComplex()) {
    return SparseEigDecomposeNonsymmetricComplexShifted(A.constRealSparse<double>(),
							A.constImagSparse<double>(),
							k, nargout, shift);
  } else {
    return SparseEigDecomposeNonsymmetricRealShifted(A.constRealSparse<double>(),
						     k, nargout, shift[0]);
  }
}
