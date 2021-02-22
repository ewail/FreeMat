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

#ifndef __FMLAPACK_hpp__
#define __FMLAPACK_hpp__

#include <string.h>

#define ftnlen short

// Lapack routines used are declared here
extern "C" {
  double dlamch_(char * CMACH, ftnlen cmach_len);
  
  float slamch_(char * CMACH, ftnlen cmach_len);

  void dgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       double *A, int * LDA, double * AF, int * LDAF, 
	       int * IPIV, char * EQUED, double * R, double * C, 
	       double * B, int * LDB, double * X, int * LDX, 
	       double * RCOND, double * FERR, double * BERR,
	       double * WORK, int * IWORK, int * INFO, 
	       ftnlen fact_len, ftnlen trans_len, ftnlen equed_len);

  void zgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       double *A, int * LDA, double * AF, int * LDAF, 
	       int * IPIV, char * EQUED, double * R, double * C, 
	       double * B, int * LDB, double * X, int * LDX, 
	       double * RCOND, double * FERR, double * BERR,
	       double * WORK, double * RWORK, int * INFO, 
	       ftnlen fact_len, ftnlen trans_len, ftnlen equed_len);

  void sgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       float *A, int * LDA, float * AF, int * LDAF, 
	       int * IPIV, char * EQUED, float * R, float * C, 
	       float * B, int * LDB, float * X, int * LDX, 
	       float * RCOND, float * FERR, float * BERR,
	       float * WORK, int * IWORK, int * INFO, 
	       ftnlen fact_len, ftnlen trans_len, ftnlen equed_len);

  void cgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       float *A, int * LDA, float * AF, int * LDAF, 
	       int * IPIV, char * EQUED, float * R, float * C, 
	       float * B, int * LDB, float * X, int * LDX, 
	       float * RCOND, float * FERR, float * BERR,
	       float * WORK, float * RWORK, int * INFO, 
	       ftnlen fact_len, ftnlen trans_len, ftnlen equed_len);

  void dgelsy_(int* M, int *N, int *NRHS, double* A, int *LDA,
	       double *B, int *LDB, int *JPVT, double* RCOND,
	       int *RANK, double *WORK, int* LWORK, int* INFO);

  void zgelsy_(int* M, int *N, int *NRHS, double* A, int *LDA,
	       double *B, int *LDB, int *JPVT, double* RCOND,
	       int *RANK, double *WORK, int* LWORK, double* RWORK,
	       int* INFO);

  void sgelsy_(int* M, int *N, int *NRHS, float* A, int *LDA,
	       float *B, int *LDB, int *JPVT, float* RCOND,
	       int *RANK, float *WORK, int* LWORK, int* INFO);

  void cgelsy_(int* M, int *N, int *NRHS, float* A, int *LDA,
	       float *B, int *LDB, int *JPVT, float* RCOND,
	       int *RANK, float *WORK, int* LWORK, float* RWORK,
	       int* INFO);

  void sgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, float* A, int* LDA, float* WR, float* WI,
	       float* VL, int *LDVL, float* VR, int *LDVR, int *ILO,
	       int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	       float* RCONDV, float *WORK, int *LWORK, int *IWORK,
	       int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

  void dgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, double* A, int* LDA, double* WR, double* WI,
	       double* VL, int *LDVL, double* VR, int *LDVR, int *ILO,
	       int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	       double* RCONDV, double *WORK, int *LWORK, int *IWORK,
	       int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

  void cgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, float* A, int* LDA, float* W, float* VL, 
	       int *LDVL, float* VR, int *LDVR, int *ILO,
	       int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	       float* RCONDV, float *WORK, int *LWORK, float *RWORK,
	       int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

  void zgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, double* A, int* LDA, double* W, double* VL, 
	       int *LDVL, double* VR, int *LDVR, int *ILO,
	       int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	       double* RCONDV, double *WORK, int *LWORK, double *RWORK,
	       int *INFO, ftnlen balanc_len, 
	       ftnlen jobvl_len, ftnlen jobvr_len, ftnlen sense_len);

  void sgesvd_(char* JOBS, char* JOBVT, int* M, int *N, float* A, 
	       int *LDA, float *S, float *U, int *LDU, float *VT, 
	       int *LDVT, float *WORK, int *LWORK, int *INFO, ftnlen 
	       jobu_len, ftnlen jobvt_len);

  void dgesvd_(char* JOBS, char* JOBVT, int* M, int *N, double* A, 
	       int *LDA, double *S, double *U, int *LDU, double *VT, 
	       int *LDVT, double *WORK, int *LWORK, int *INFO, ftnlen 
	       jobu_len, ftnlen jobvt_len);

  void cgesvd_(char* JOBS, char* JOBVT, int* M, int *N, float* A, 
	       int *LDA, float *S, float *U, int *LDU, float *VT, 
	       int *LDVT, float *WORK, int *LWORK, float *RWORK, int *INFO, ftnlen 
	       jobu_len, ftnlen jobvt_len);

  void zgesvd_(char* JOBS, char* JOBVT, int* M, int *N, double* A, 
	       int *LDA, double *S, double *U, int *LDU, double *VT, 
	       int *LDVT, double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen 
	       jobu_len, ftnlen jobvt_len);

  void sgeqrf_(int *M, int *N, float *A, int *LDA, float *TAU, float *WORK,
	       int *LWORK, int *INFO);

  void sgeqp3_(int *M, int *N, float *A, int *LDA, int *JPVT, 
	       float *TAU, float *WORK, int *LWORK, int *INFO);

  void sorgqr_(int *M, int *N, int *K, float *A, int *LDA, 
	       float *TAU, float *WORK, int *LWORK, int *INFO);

  void dgeqrf_(int *M, int *N, double *A, int *LDA, double *TAU, double *WORK,
	       int *LWORK, int *INFO);

  void dgeqp3_(int *M, int *N, double *A, int *LDA, int *JPVT,
	       double *TAU, double *WORK, int *LWORK, int *INFO);

  void dorgqr_(int *M, int *N, int *K, double *A, int *LDA, 
	       double *TAU, double *WORK, int *LWORK, int *INFO);

  void cgeqrf_(int *M, int *N, float *A, int *LDA, float *TAU, float *WORK,
	       int *LWORK, int *INFO);

  void cgeqp3_(int *M, int *N, float *A, int *LDA, int *JPVT,
	       float *TAU, float *WORK, int *LWORK, float *RWORK, int *INFO);

  void cungqr_(int *M, int *N, int *K, float *A, int *LDA, 
	       float *TAU, float *WORK, int *LWORK, int *INFO);

  void zgeqrf_(int *M, int *N, double *A, int *LDA, double *TAU, double *WORK,
	       int *LWORK, int *INFO);

  void zgeqp3_(int *M, int *N, double *A, int *LDA, int *JPVT,
	       double *TAU, double *WORK, int *LWORK, double *RWORK, int *INFO);

  void zungqr_(int *M, int *N, int *K, double *A, int *LDA, 
	       double *TAU, double *WORK, int *LWORK, int *INFO);

  void ssyev_(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	      float *W, float *WORK, int *LWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len);

  void dsyev_(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	      double *W, double *WORK, int *LWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len);

  void cheev_(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	      float *W, float *WORK, int *LWORK, float *RWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len);

  void zheev_(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	      double *W, double *WORK, int *LWORK, double *RWORK, int *INFO, 
	      ftnlen jobz_len, ftnlen uplo_len);

  void sggev_(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	      float *B, int *LDB, float *ALPHAR, float *ALPHAI,
	      float *BETA, float *VL, int *LDVL, float *VR, 
	      int *LDVR, float *WORK, int *LWORK, int *INFO, 
	      ftnlen jobvl_len, ftnlen jobvr_len );

  void ssygv_(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	      int *LDA, float *B, int *LDB, float *W, float *WORK,
	      int *LWORK, int *INFO, ftnlen jobz_len, ftnlen uplo_len );

  void dggev_(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	      double *B, int *LDB, double *ALPHAR, double *ALPHAI,
	      double *BETA, double *VL, int *LDVL, double *VR, 
	      int *LDVR, double *WORK, int *LWORK, int *INFO, 
	      ftnlen jobvl_len, ftnlen jobvr_len  );

  void dsygv_(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	      int *LDA, double *B, int *LDB, double *W, double *WORK,
	      int *LWORK, int *INFO, ftnlen jobz_len, ftnlen uplo_len );

  void cggev_(char *JOBVL, char *JOBVR, int *N, float *A, int *LDA, 
	      float *B, int *LDB, float *ALPHA, float *BETA, 
	      float *VL, int *LDVL, float *VR, int *LDVR, 
	      float *WORK, int *LWORK, float *RWORK, int *INFO, 
	      ftnlen jobvl_len, ftnlen jobvr_len  );

  void chegv_(int *ITYPE, char *JOBZ, char *UPLO, int *N, float *A, 
	      int *LDA, float *B, int *LDB, float *W, float *WORK,
	      int *LWORK, float *RWORK, int *INFO, ftnlen 
	      jobz_len, ftnlen uplo_len );

  void zggev_(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA, 
	      double *B, int *LDB, double *ALPHA, double *BETA, 
	      double *VL, int *LDVL, double *VR, int *LDVR, 
	      double *WORK, int *LWORK, double *RWORK, int *INFO, ftnlen 
	      jobz_len, ftnlen uplo_len );

  void zhegv_(int *ITYPE, char *JOBZ, char *UPLO, int *N, double *A, 
	      int *LDA, double *B, int *LDB, double *W, double *WORK,
	      int *LWORK, double *RWORK, int *INFO, ftnlen jobz_len, ftnlen uplo_len );

  void sgetrf_(int *M, int *N, float *A, int *LDA, int *IPIV, int *INFO);
  
  void cgetrf_(int *M, int *N, float *A, int *LDA, int *IPIV, int *INFO);

  void dgetrf_(int *M, int *N, double *A, int *LDA, int *IPIV, int *INFO);
  
  void zgetrf_(int *M, int *N, double *A, int *LDA, int *IPIV, int *INFO);

  void sgetri_(int *N, float *A, int *LDA, int *IPIV, float *WORK, int *LWORK, int *INFO);

  void cgetri_(int *N, float *A, int *LDA, int *IPIV, float *WORK, int *LWORK, int *INFO);

  void dgetri_(int *N, double *A, int *LDA, int *IPIV, double *WORK, int *LWORK, int *INFO);
  
  void zgetri_(int *N, double *A, int *LDA, int *IPIV, double *WORK, int *LWORK, int *INFO);

  int xerbla_(char *srname, int *info);

  double snrm2_(int *N, float *X, int *INCX);
  
  double dnrm2_(int *N, double *X, int *INCX);

  double scnrm2_(int *N, float *X, int *INCX);
  
  double dznrm2_(int *N, double *X, int *INCX);

  void dgecon_(char *norm, int *N, double *A, int *LDA, double *Anorm,
	       double *rcond, double *work, int *iwork, int *info, ftnlen norm_len);

  void sgecon_(char *norm, int *N, float *A, int *LDA, float *Anorm,
	       float *rcond, float *work, int *iwork, int *info, ftnlen norm_len);

  void cgecon_(char *norm, int *N, float *A, int *LDA, float *Anorm,
	       float *rcond, float *work, float *rwork, int *info, ftnlen norm_len);

  void zgecon_(char *norm, int *N, double *A, int *LDA, double *Anorm,
	       double *rcond, double *work, double *rwork, int *info, ftnlen norm_len);

  double dlange_(char *norm, int *M, int *N, double *A, int *LDA,
		 double *work, ftnlen norm_len);

  float slange_(char *norm, int *M, int *N, float *A, int *LDA,
		float *work, ftnlen norm_len);

  float clange_(char *norm, int *M, int *N, float *A, int *LDA,
		float *work, ftnlen norm_len);

  double zlange_(char *norm, int *M, int *N, double *A, int *LDA,
		 double *work, ftnlen norm_len);

}

/**
 * Call the LAPACK slamch function to get the $$\epsilon$$ value for
 * a floating point number.
 */
double getEPS();
/**
 * Call the LAPACK dlamch function to get the $$\epsilon$$ value for
 * a double-precision floating point number.
 */
float getFloatEPS();

template <typename T>
T lamch();

/**
 * This function effectively pads each column in the source matrix by a
 * number of zeros.  It actually allows us to change the stride of the
 * matrix (amount that must be added to an index to move from one column
 * to the next).  
 */
template <typename T>
void changeStride(T*dst, int dstStride, T*src, int srcStride, 
		  int rowCount, int colCount) {
  for (int i=0;i<colCount;i++)
    memcpy(dst + i*dstStride, src + i*srcStride, rowCount*sizeof(T));
}

#endif
