/*
 * Copyright (c) 2009 Eugene Ingerman
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
//This is a generated file. Do not edit!
#include <QString>
#include <QLibrary> 
#include "blas_wrap.h"
#include "blas_dyn_link.h"
#include <iostream>
#include <vector>

BlasWrapper wrapper;

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */ 
typedef void (* p_caxpy )(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_ccopy )(int *n, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_cdotc )(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
typedef void (* p_cdotu )(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
typedef void (* p_cgbmv )(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
typedef void (* p_cgemm )(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_cgemv )(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
typedef void (* p_cgerc )(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
typedef void (* p_cgeru )(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
typedef void (* p_chbmv )(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_chemm )(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_chemv )(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_cher )(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len);
typedef void (* p_cher2 )(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len);
typedef void (* p_cher2k )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_cherk )(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_chpmv )(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
typedef void (* p_chpr )(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len);
typedef void (* p_chpr2 )(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len);
typedef void (* p_crotg )(complex *a, complex *b, float *c, complex *s);
typedef void (* p_cscal )(int *n, complex *alpha, complex *x, int *incx);
typedef void (* p_csscal )(int *n, float *alpha, complex *x, int *incx);
typedef void (* p_cswap )(int *n, complex *x, int *incx, complex *y, int *incy);
typedef void (* p_csymm )(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_csyr2k )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_csyrk )(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ctbmv )(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctbsv )(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctpmv )(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctpsv )(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ctrmv )(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ctrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ctrsv )(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef double (* p_dasum )(int *n, double *x, int *incx);
typedef void (* p_daxpy )(int *n, double *alpha, double *x, int *incx, double *y, int *incy);
typedef double (* p_dcabs1 )(doublecomplex *z);
typedef void (* p_dcopy )(int *n, double *x, int *incx, double *y, int *incy);
typedef double (* p_ddot )(int *n, double *x, int *incx, double *y, int *incy);
typedef void (* p_dgbmv )(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
typedef void (* p_dgemm )(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_dgemv )(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
typedef void (* p_dger )(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda);
typedef double (* p_dnrm2 )(int *n, double *x, int *incx);
typedef void (* p_drot )(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s);
typedef void (* p_drotg )(double *a, double *b, double *c, double *s);
typedef void (* p_dsbmv )(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dscal )(int *n, double *alpha, double *x, int *incx);
typedef void (* p_dspmv )(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dspr )(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len);
typedef void (* p_dspr2 )(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len);
typedef void (* p_dswap )(int *n, double *x, int *incx, double *y, int *incy);
typedef void (* p_dsymm )(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_dsymv )(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
typedef void (* p_dsyr )(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len);
typedef void (* p_dsyr2 )(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len);
typedef void (* p_dsyr2k )(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_dsyrk )(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_dtbmv )(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtbsv )(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtpmv )(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtpsv )(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_dtrmv )(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_dtrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_dtrsv )(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef double (* p_dzasum )(int *n, doublecomplex *x, int *incx);
typedef double (* p_dznrm2 )(int *n, doublecomplex *x, int *incx);
typedef int (* p_icamax )(int *n, complex *x, int *incx);
typedef int (* p_idamax )(int *n, double *x, int *incx);
typedef int (* p_isamax )(int *n, float *x, int *incx);
typedef int (* p_izamax )(int *n, doublecomplex *x, int *incx);
typedef double (* p_sasum )(int *n, float *x, int *incx);
typedef void (* p_saxpy )(int *n, float *alpha, float *x, int *incx, float *y, int *incy);
typedef double (* p_scasum )(int *n, complex *x, int *incx);
typedef double (* p_scnrm2 )(int *n, complex *x, int *incx);
typedef void (* p_scopy )(int *n, float *x, int *incx, float *y, int *incy);
typedef double (* p_sdot )(int *n, float *x, int *incx, float *y, int *incy);
typedef void (* p_sgbmv )(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
typedef void (* p_sgemm )(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_sgemv )(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
typedef void (* p_sger )(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda);
typedef double (* p_snrm2 )(int *n, float *x, int *incx);
typedef void (* p_srot )(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s);
typedef void (* p_srotg )(float *a, float *b, float *c, float *s);
typedef void (* p_ssbmv )(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_sscal )(int *n, float *alpha, float *x, int *incx);
typedef void (* p_sspmv )(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_sspr )(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len);
typedef void (* p_sspr2 )(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len);
typedef void (* p_sswap )(int *n, float *x, int *incx, float *y, int *incy);
typedef void (* p_ssymm )(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_ssymv )(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
typedef void (* p_ssyr )(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len);
typedef void (* p_ssyr2 )(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len);
typedef void (* p_ssyr2k )(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ssyrk )(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_stbmv )(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stbsv )(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stpmv )(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_stpsv )(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_strmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_strmv )(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_strsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_strsv )(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_zaxpy )(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zcopy )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zdotc )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
typedef void (* p_zdotu )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
typedef void (* p_zdscal )(int *n, double *alpha, doublecomplex *x, int *incx);
typedef void (* p_zgbmv )(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
typedef void (* p_zgemm )(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len);
typedef void (* p_zgemv )(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
typedef void (* p_zgerc )(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
typedef void (* p_zgeru )(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
typedef void (* p_zhbmv )(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zhemm )(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_zhemv )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zher )(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len);
typedef void (* p_zher2 )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len);
typedef void (* p_zher2k )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zherk )(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zhpmv )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
typedef void (* p_zhpr )(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len);
typedef void (* p_zhpr2 )(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len);
typedef void (* p_zrotg )(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s);
typedef void (* p_zscal )(int *n, doublecomplex *alpha, doublecomplex *x, int *incx);
typedef void (* p_zswap )(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
typedef void (* p_zsymm )(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
typedef void (* p_zsyr2k )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_zsyrk )(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
typedef void (* p_ztbmv )(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztbsv )(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztpmv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztpsv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztrmm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ztrmv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
typedef void (* p_ztrsm )(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
typedef void (* p_ztrsv )(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);


extern "C" {
void REF_caxpy_(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy);
void REF_ccopy_(int *n, complex *x, int *incx, complex *y, int *incy);
void REF_cdotc_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
void REF_cdotu_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
void REF_cgbmv_(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
void REF_cgemm_(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len);
void REF_cgemv_(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
void REF_cgerc_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
void REF_cgeru_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
void REF_chbmv_(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void REF_chemm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
void REF_chemv_(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void REF_cher_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len);
void REF_cher2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len);
void REF_cher2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void REF_cherk_(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void REF_chpmv_(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void REF_chpr_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len);
void REF_chpr2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len);
void REF_crotg_(complex *a, complex *b, float *c, complex *s);
void REF_cscal_(int *n, complex *alpha, complex *x, int *incx);
void REF_csscal_(int *n, float *alpha, complex *x, int *incx);
void REF_cswap_(int *n, complex *x, int *incx, complex *y, int *incy);
void REF_csymm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
void REF_csyr2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void REF_csyrk_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void REF_ctbmv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ctbsv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ctpmv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ctpsv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ctrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_ctrmv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ctrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_ctrsv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
double REF_dasum_(int *n, double *x, int *incx);
void REF_daxpy_(int *n, double *alpha, double *x, int *incx, double *y, int *incy);
double REF_dcabs1_(doublecomplex *z);
void REF_dcopy_(int *n, double *x, int *incx, double *y, int *incy);
double REF_ddot_(int *n, double *x, int *incx, double *y, int *incy);
void REF_dgbmv_(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
void REF_dgemm_(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len);
void REF_dgemv_(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
void REF_dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda);
double REF_dnrm2_(int *n, double *x, int *incx);
void REF_drot_(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s);
void REF_drotg_(double *a, double *b, double *c, double *s);
void REF_dsbmv_(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void REF_dscal_(int *n, double *alpha, double *x, int *incx);
void REF_dspmv_(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void REF_dspr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len);
void REF_dspr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len);
void REF_dswap_(int *n, double *x, int *incx, double *y, int *incy);
void REF_dsymm_(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len);
void REF_dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void REF_dsyr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len);
void REF_dsyr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len);
void REF_dsyr2k_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
void REF_dsyrk_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
void REF_dtbmv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_dtbsv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_dtpmv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_dtpsv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_dtrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_dtrmv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_dtrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_dtrsv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
double REF_dzasum_(int *n, doublecomplex *x, int *incx);
double REF_dznrm2_(int *n, doublecomplex *x, int *incx);
int REF_icamax_(int *n, complex *x, int *incx);
int REF_idamax_(int *n, double *x, int *incx);
int REF_isamax_(int *n, float *x, int *incx);
int REF_izamax_(int *n, doublecomplex *x, int *incx);
double REF_sasum_(int *n, float *x, int *incx);
void REF_saxpy_(int *n, float *alpha, float *x, int *incx, float *y, int *incy);
double REF_scasum_(int *n, complex *x, int *incx);
double REF_scnrm2_(int *n, complex *x, int *incx);
void REF_scopy_(int *n, float *x, int *incx, float *y, int *incy);
double REF_sdot_(int *n, float *x, int *incx, float *y, int *incy);
void REF_sgbmv_(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
void REF_sgemm_(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len);
void REF_sgemv_(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
void REF_sger_(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda);
double REF_snrm2_(int *n, float *x, int *incx);
void REF_srot_(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s);
void REF_srotg_(float *a, float *b, float *c, float *s);
void REF_ssbmv_(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void REF_sscal_(int *n, float *alpha, float *x, int *incx);
void REF_sspmv_(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void REF_sspr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len);
void REF_sspr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len);
void REF_sswap_(int *n, float *x, int *incx, float *y, int *incy);
void REF_ssymm_(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len);
void REF_ssymv_(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void REF_ssyr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len);
void REF_ssyr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len);
void REF_ssyr2k_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
void REF_ssyrk_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
void REF_stbmv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_stbsv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_stpmv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_stpsv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_strmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_strmv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_strsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_strsv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_zaxpy_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void REF_zcopy_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void REF_zdotc_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
void REF_zdotu_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
void REF_zdscal_(int *n, double *alpha, doublecomplex *x, int *incx);
void REF_zgbmv_(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
void REF_zgemm_(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len);
void REF_zgemv_(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
void REF_zgerc_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
void REF_zgeru_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
void REF_zhbmv_(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void REF_zhemm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
void REF_zhemv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void REF_zher_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len);
void REF_zher2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len);
void REF_zher2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void REF_zherk_(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void REF_zhpmv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void REF_zhpr_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len);
void REF_zhpr2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len);
void REF_zrotg_(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s);
void REF_zscal_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx);
void REF_zswap_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void REF_zsymm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
void REF_zsyr2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void REF_zsyrk_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void REF_ztbmv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ztbsv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ztpmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ztpsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ztrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_ztrmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void REF_ztrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void REF_ztrsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
}

void caxpy_(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[0] )
		wrapper.fn[0] = wrapper.Resolve("caxpy",(void (*)()) REF_caxpy_);
	(* reinterpret_cast<p_caxpy>(wrapper.fn[0]))( n, alpha, x, incx, y, incy );

}

void ccopy_(int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[1] )
		wrapper.fn[1] = wrapper.Resolve("ccopy",(void (*)()) REF_ccopy_);
	(* reinterpret_cast<p_ccopy>(wrapper.fn[1]))( n, x, incx, y, incy );

}

void cdotc_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val)
{
	REF_cdotc_( n, x, incx, y, incy, ret_val );
}

void cdotu_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val)
{
	REF_cdotu_( n, x, incx, y, incy, ret_val );
}

void cgbmv_(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[4] )
		wrapper.fn[4] = wrapper.Resolve("cgbmv",(void (*)()) REF_cgbmv_);
	(* reinterpret_cast<p_cgbmv>(wrapper.fn[4]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void cgemm_(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[5] )
		wrapper.fn[5] = wrapper.Resolve("cgemm",(void (*)()) REF_cgemm_);
	(* reinterpret_cast<p_cgemm>(wrapper.fn[5]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void cgemv_(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[6] )
		wrapper.fn[6] = wrapper.Resolve("cgemv",(void (*)()) REF_cgemv_);
	(* reinterpret_cast<p_cgemv>(wrapper.fn[6]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void cgerc_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda)
{
	if( !wrapper.fn[7] )
		wrapper.fn[7] = wrapper.Resolve("cgerc",(void (*)()) REF_cgerc_);
	(* reinterpret_cast<p_cgerc>(wrapper.fn[7]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void cgeru_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda)
{
	if( !wrapper.fn[8] )
		wrapper.fn[8] = wrapper.Resolve("cgeru",(void (*)()) REF_cgeru_);
	(* reinterpret_cast<p_cgeru>(wrapper.fn[8]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void chbmv_(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[9] )
		wrapper.fn[9] = wrapper.Resolve("chbmv",(void (*)()) REF_chbmv_);
	(* reinterpret_cast<p_chbmv>(wrapper.fn[9]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void chemm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[10] )
		wrapper.fn[10] = wrapper.Resolve("chemm",(void (*)()) REF_chemm_);
	(* reinterpret_cast<p_chemm>(wrapper.fn[10]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void chemv_(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[11] )
		wrapper.fn[11] = wrapper.Resolve("chemv",(void (*)()) REF_chemv_);
	(* reinterpret_cast<p_chemv>(wrapper.fn[11]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void cher_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[12] )
		wrapper.fn[12] = wrapper.Resolve("cher",(void (*)()) REF_cher_);
	(* reinterpret_cast<p_cher>(wrapper.fn[12]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void cher2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[13] )
		wrapper.fn[13] = wrapper.Resolve("cher2",(void (*)()) REF_cher2_);
	(* reinterpret_cast<p_cher2>(wrapper.fn[13]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void cher2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[14] )
		wrapper.fn[14] = wrapper.Resolve("cher2k",(void (*)()) REF_cher2k_);
	(* reinterpret_cast<p_cher2k>(wrapper.fn[14]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void cherk_(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[15] )
		wrapper.fn[15] = wrapper.Resolve("cherk",(void (*)()) REF_cherk_);
	(* reinterpret_cast<p_cherk>(wrapper.fn[15]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void chpmv_(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[16] )
		wrapper.fn[16] = wrapper.Resolve("chpmv",(void (*)()) REF_chpmv_);
	(* reinterpret_cast<p_chpmv>(wrapper.fn[16]))( uplo, n, alpha, a, x, incx, beta, y, incy, uplo_len );

}

void chpr_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len)
{
	if( !wrapper.fn[17] )
		wrapper.fn[17] = wrapper.Resolve("chpr",(void (*)()) REF_chpr_);
	(* reinterpret_cast<p_chpr>(wrapper.fn[17]))( uplo, n, alpha, x, incx, a, uplo_len );

}

void chpr2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len)
{
	if( !wrapper.fn[18] )
		wrapper.fn[18] = wrapper.Resolve("chpr2",(void (*)()) REF_chpr2_);
	(* reinterpret_cast<p_chpr2>(wrapper.fn[18]))( uplo, n, alpha, x, incx, y, incy, a, uplo_len );

}

void crotg_(complex *a, complex *b, float *c, complex *s)
{
	if( !wrapper.fn[19] )
		wrapper.fn[19] = wrapper.Resolve("crotg",(void (*)()) REF_crotg_);
	(* reinterpret_cast<p_crotg>(wrapper.fn[19]))( a, b, c, s );

}

void cscal_(int *n, complex *alpha, complex *x, int *incx)
{
	if( !wrapper.fn[20] )
		wrapper.fn[20] = wrapper.Resolve("cscal",(void (*)()) REF_cscal_);
	(* reinterpret_cast<p_cscal>(wrapper.fn[20]))( n, alpha, x, incx );

}

void csscal_(int *n, float *alpha, complex *x, int *incx)
{
	if( !wrapper.fn[21] )
		wrapper.fn[21] = wrapper.Resolve("csscal",(void (*)()) REF_csscal_);
	(* reinterpret_cast<p_csscal>(wrapper.fn[21]))( n, alpha, x, incx );

}

void cswap_(int *n, complex *x, int *incx, complex *y, int *incy)
{
	if( !wrapper.fn[22] )
		wrapper.fn[22] = wrapper.Resolve("cswap",(void (*)()) REF_cswap_);
	(* reinterpret_cast<p_cswap>(wrapper.fn[22]))( n, x, incx, y, incy );

}

void csymm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[23] )
		wrapper.fn[23] = wrapper.Resolve("csymm",(void (*)()) REF_csymm_);
	(* reinterpret_cast<p_csymm>(wrapper.fn[23]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void csyr2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[24] )
		wrapper.fn[24] = wrapper.Resolve("csyr2k",(void (*)()) REF_csyr2k_);
	(* reinterpret_cast<p_csyr2k>(wrapper.fn[24]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void csyrk_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[25] )
		wrapper.fn[25] = wrapper.Resolve("csyrk",(void (*)()) REF_csyrk_);
	(* reinterpret_cast<p_csyrk>(wrapper.fn[25]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void ctbmv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[26] )
		wrapper.fn[26] = wrapper.Resolve("ctbmv",(void (*)()) REF_ctbmv_);
	(* reinterpret_cast<p_ctbmv>(wrapper.fn[26]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctbsv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[27] )
		wrapper.fn[27] = wrapper.Resolve("ctbsv",(void (*)()) REF_ctbsv_);
	(* reinterpret_cast<p_ctbsv>(wrapper.fn[27]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctpmv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[28] )
		wrapper.fn[28] = wrapper.Resolve("ctpmv",(void (*)()) REF_ctpmv_);
	(* reinterpret_cast<p_ctpmv>(wrapper.fn[28]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ctpsv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[29] )
		wrapper.fn[29] = wrapper.Resolve("ctpsv",(void (*)()) REF_ctpsv_);
	(* reinterpret_cast<p_ctpsv>(wrapper.fn[29]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ctrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[30] )
		wrapper.fn[30] = wrapper.Resolve("ctrmm",(void (*)()) REF_ctrmm_);
	(* reinterpret_cast<p_ctrmm>(wrapper.fn[30]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ctrmv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[31] )
		wrapper.fn[31] = wrapper.Resolve("ctrmv",(void (*)()) REF_ctrmv_);
	(* reinterpret_cast<p_ctrmv>(wrapper.fn[31]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ctrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[32] )
		wrapper.fn[32] = wrapper.Resolve("ctrsm",(void (*)()) REF_ctrsm_);
	(* reinterpret_cast<p_ctrsm>(wrapper.fn[32]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ctrsv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[33] )
		wrapper.fn[33] = wrapper.Resolve("ctrsv",(void (*)()) REF_ctrsv_);
	(* reinterpret_cast<p_ctrsv>(wrapper.fn[33]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

double dasum_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[34] )
		wrapper.fn[34] = wrapper.Resolve("dasum",(void (*)()) REF_dasum_);
	return (* reinterpret_cast<p_dasum>(wrapper.fn[34]))( n, x, incx );

}

void daxpy_(int *n, double *alpha, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[35] )
		wrapper.fn[35] = wrapper.Resolve("daxpy",(void (*)()) REF_daxpy_);
	(* reinterpret_cast<p_daxpy>(wrapper.fn[35]))( n, alpha, x, incx, y, incy );

}

double dcabs1_(doublecomplex *z)
{
	if( !wrapper.fn[36] )
		wrapper.fn[36] = wrapper.Resolve("dcabs1",(void (*)()) REF_dcabs1_);
	return (* reinterpret_cast<p_dcabs1>(wrapper.fn[36]))( z );

}

void dcopy_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[37] )
		wrapper.fn[37] = wrapper.Resolve("dcopy",(void (*)()) REF_dcopy_);
	(* reinterpret_cast<p_dcopy>(wrapper.fn[37]))( n, x, incx, y, incy );

}

double ddot_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[38] )
		wrapper.fn[38] = wrapper.Resolve("ddot",(void (*)()) REF_ddot_);
	return (* reinterpret_cast<p_ddot>(wrapper.fn[38]))( n, x, incx, y, incy );

}

void dgbmv_(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len)
{
	if( !wrapper.fn[39] )
		wrapper.fn[39] = wrapper.Resolve("dgbmv",(void (*)()) REF_dgbmv_);
	(* reinterpret_cast<p_dgbmv>(wrapper.fn[39]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void dgemm_(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[40] )
		wrapper.fn[40] = wrapper.Resolve("dgemm",(void (*)()) REF_dgemm_);
	(* reinterpret_cast<p_dgemm>(wrapper.fn[40]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void dgemv_(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len)
{
	if( !wrapper.fn[41] )
		wrapper.fn[41] = wrapper.Resolve("dgemv",(void (*)()) REF_dgemv_);
	(* reinterpret_cast<p_dgemv>(wrapper.fn[41]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda)
{
	if( !wrapper.fn[42] )
		wrapper.fn[42] = wrapper.Resolve("dger",(void (*)()) REF_dger_);
	(* reinterpret_cast<p_dger>(wrapper.fn[42]))( m, n, alpha, x, incx, y, incy, a, lda );

}

double dnrm2_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[43] )
		wrapper.fn[43] = wrapper.Resolve("dnrm2",(void (*)()) REF_dnrm2_);
	return (* reinterpret_cast<p_dnrm2>(wrapper.fn[43]))( n, x, incx );

}

void drot_(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s)
{
	if( !wrapper.fn[44] )
		wrapper.fn[44] = wrapper.Resolve("drot",(void (*)()) REF_drot_);
	(* reinterpret_cast<p_drot>(wrapper.fn[44]))( n, x, incx, y, incy, c, s );

}

void drotg_(double *a, double *b, double *c, double *s)
{
	if( !wrapper.fn[45] )
		wrapper.fn[45] = wrapper.Resolve("drotg",(void (*)()) REF_drotg_);
	(* reinterpret_cast<p_drotg>(wrapper.fn[45]))( a, b, c, s );

}

void dsbmv_(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[46] )
		wrapper.fn[46] = wrapper.Resolve("dsbmv",(void (*)()) REF_dsbmv_);
	(* reinterpret_cast<p_dsbmv>(wrapper.fn[46]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void dscal_(int *n, double *alpha, double *x, int *incx)
{
	if( !wrapper.fn[47] )
		wrapper.fn[47] = wrapper.Resolve("dscal",(void (*)()) REF_dscal_);
	(* reinterpret_cast<p_dscal>(wrapper.fn[47]))( n, alpha, x, incx );

}

void dspmv_(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[48] )
		wrapper.fn[48] = wrapper.Resolve("dspmv",(void (*)()) REF_dspmv_);
	(* reinterpret_cast<p_dspmv>(wrapper.fn[48]))( uplo, n, alpha, ap, x, incx, beta, y, incy, uplo_len );

}

void dspr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len)
{
	if( !wrapper.fn[49] )
		wrapper.fn[49] = wrapper.Resolve("dspr",(void (*)()) REF_dspr_);
	(* reinterpret_cast<p_dspr>(wrapper.fn[49]))( uplo, n, alpha, x, incx, ap, uplo_len );

}

void dspr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len)
{
	if( !wrapper.fn[50] )
		wrapper.fn[50] = wrapper.Resolve("dspr2",(void (*)()) REF_dspr2_);
	(* reinterpret_cast<p_dspr2>(wrapper.fn[50]))( uplo, n, alpha, x, incx, y, incy, ap, uplo_len );

}

void dswap_(int *n, double *x, int *incx, double *y, int *incy)
{
	if( !wrapper.fn[51] )
		wrapper.fn[51] = wrapper.Resolve("dswap",(void (*)()) REF_dswap_);
	(* reinterpret_cast<p_dswap>(wrapper.fn[51]))( n, x, incx, y, incy );

}

void dsymm_(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[52] )
		wrapper.fn[52] = wrapper.Resolve("dsymm",(void (*)()) REF_dsymm_);
	(* reinterpret_cast<p_dsymm>(wrapper.fn[52]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[53] )
		wrapper.fn[53] = wrapper.Resolve("dsymv",(void (*)()) REF_dsymv_);
	(* reinterpret_cast<p_dsymv>(wrapper.fn[53]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void dsyr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[54] )
		wrapper.fn[54] = wrapper.Resolve("dsyr",(void (*)()) REF_dsyr_);
	(* reinterpret_cast<p_dsyr>(wrapper.fn[54]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void dsyr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[55] )
		wrapper.fn[55] = wrapper.Resolve("dsyr2",(void (*)()) REF_dsyr2_);
	(* reinterpret_cast<p_dsyr2>(wrapper.fn[55]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void dsyr2k_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[56] )
		wrapper.fn[56] = wrapper.Resolve("dsyr2k",(void (*)()) REF_dsyr2k_);
	(* reinterpret_cast<p_dsyr2k>(wrapper.fn[56]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void dsyrk_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[57] )
		wrapper.fn[57] = wrapper.Resolve("dsyrk",(void (*)()) REF_dsyrk_);
	(* reinterpret_cast<p_dsyrk>(wrapper.fn[57]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void dtbmv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[58] )
		wrapper.fn[58] = wrapper.Resolve("dtbmv",(void (*)()) REF_dtbmv_);
	(* reinterpret_cast<p_dtbmv>(wrapper.fn[58]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtbsv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[59] )
		wrapper.fn[59] = wrapper.Resolve("dtbsv",(void (*)()) REF_dtbsv_);
	(* reinterpret_cast<p_dtbsv>(wrapper.fn[59]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtpmv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[60] )
		wrapper.fn[60] = wrapper.Resolve("dtpmv",(void (*)()) REF_dtpmv_);
	(* reinterpret_cast<p_dtpmv>(wrapper.fn[60]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void dtpsv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[61] )
		wrapper.fn[61] = wrapper.Resolve("dtpsv",(void (*)()) REF_dtpsv_);
	(* reinterpret_cast<p_dtpsv>(wrapper.fn[61]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void dtrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[62] )
		wrapper.fn[62] = wrapper.Resolve("dtrmm",(void (*)()) REF_dtrmm_);
	(* reinterpret_cast<p_dtrmm>(wrapper.fn[62]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void dtrmv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[63] )
		wrapper.fn[63] = wrapper.Resolve("dtrmv",(void (*)()) REF_dtrmv_);
	(* reinterpret_cast<p_dtrmv>(wrapper.fn[63]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void dtrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[64] )
		wrapper.fn[64] = wrapper.Resolve("dtrsm",(void (*)()) REF_dtrsm_);
	(* reinterpret_cast<p_dtrsm>(wrapper.fn[64]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void dtrsv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[65] )
		wrapper.fn[65] = wrapper.Resolve("dtrsv",(void (*)()) REF_dtrsv_);
	(* reinterpret_cast<p_dtrsv>(wrapper.fn[65]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

double dzasum_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[66] )
		wrapper.fn[66] = wrapper.Resolve("dzasum",(void (*)()) REF_dzasum_);
	return (* reinterpret_cast<p_dzasum>(wrapper.fn[66]))( n, x, incx );

}

double dznrm2_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[67] )
		wrapper.fn[67] = wrapper.Resolve("dznrm2",(void (*)()) REF_dznrm2_);
	return (* reinterpret_cast<p_dznrm2>(wrapper.fn[67]))( n, x, incx );

}

int icamax_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[68] )
		wrapper.fn[68] = wrapper.Resolve("icamax",(void (*)()) REF_icamax_);
	return (* reinterpret_cast<p_icamax>(wrapper.fn[68]))( n, x, incx );

}

int idamax_(int *n, double *x, int *incx)
{
	if( !wrapper.fn[69] )
		wrapper.fn[69] = wrapper.Resolve("idamax",(void (*)()) REF_idamax_);
	return (* reinterpret_cast<p_idamax>(wrapper.fn[69]))( n, x, incx );

}

int isamax_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[70] )
		wrapper.fn[70] = wrapper.Resolve("isamax",(void (*)()) REF_isamax_);
	return (* reinterpret_cast<p_isamax>(wrapper.fn[70]))( n, x, incx );

}

int izamax_(int *n, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[71] )
		wrapper.fn[71] = wrapper.Resolve("izamax",(void (*)()) REF_izamax_);
	return (* reinterpret_cast<p_izamax>(wrapper.fn[71]))( n, x, incx );

}

double sasum_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[72] )
		wrapper.fn[72] = wrapper.Resolve("sasum",(void (*)()) REF_sasum_);
	return (* reinterpret_cast<p_sasum>(wrapper.fn[72]))( n, x, incx );

}

void saxpy_(int *n, float *alpha, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[73] )
		wrapper.fn[73] = wrapper.Resolve("saxpy",(void (*)()) REF_saxpy_);
	(* reinterpret_cast<p_saxpy>(wrapper.fn[73]))( n, alpha, x, incx, y, incy );

}

double scasum_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[74] )
		wrapper.fn[74] = wrapper.Resolve("scasum",(void (*)()) REF_scasum_);
	return (* reinterpret_cast<p_scasum>(wrapper.fn[74]))( n, x, incx );

}

double scnrm2_(int *n, complex *x, int *incx)
{
	if( !wrapper.fn[75] )
		wrapper.fn[75] = wrapper.Resolve("scnrm2",(void (*)()) REF_scnrm2_);
	return (* reinterpret_cast<p_scnrm2>(wrapper.fn[75]))( n, x, incx );

}

void scopy_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[76] )
		wrapper.fn[76] = wrapper.Resolve("scopy",(void (*)()) REF_scopy_);
	(* reinterpret_cast<p_scopy>(wrapper.fn[76]))( n, x, incx, y, incy );

}

double sdot_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[77] )
		wrapper.fn[77] = wrapper.Resolve("sdot",(void (*)()) REF_sdot_);
	return (* reinterpret_cast<p_sdot>(wrapper.fn[77]))( n, x, incx, y, incy );

}

void sgbmv_(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len)
{
	if( !wrapper.fn[78] )
		wrapper.fn[78] = wrapper.Resolve("sgbmv",(void (*)()) REF_sgbmv_);
	(* reinterpret_cast<p_sgbmv>(wrapper.fn[78]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void sgemm_(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[79] )
		wrapper.fn[79] = wrapper.Resolve("sgemm",(void (*)()) REF_sgemm_);
	(* reinterpret_cast<p_sgemm>(wrapper.fn[79]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void sgemv_(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len)
{
	if( !wrapper.fn[80] )
		wrapper.fn[80] = wrapper.Resolve("sgemv",(void (*)()) REF_sgemv_);
	(* reinterpret_cast<p_sgemv>(wrapper.fn[80]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void sger_(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda)
{
	if( !wrapper.fn[81] )
		wrapper.fn[81] = wrapper.Resolve("sger",(void (*)()) REF_sger_);
	(* reinterpret_cast<p_sger>(wrapper.fn[81]))( m, n, alpha, x, incx, y, incy, a, lda );

}

double snrm2_(int *n, float *x, int *incx)
{
	if( !wrapper.fn[82] )
		wrapper.fn[82] = wrapper.Resolve("snrm2",(void (*)()) REF_snrm2_);
	return (* reinterpret_cast<p_snrm2>(wrapper.fn[82]))( n, x, incx );

}

void srot_(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s)
{
	if( !wrapper.fn[83] )
		wrapper.fn[83] = wrapper.Resolve("srot",(void (*)()) REF_srot_);
	(* reinterpret_cast<p_srot>(wrapper.fn[83]))( n, x, incx, y, incy, c, s );

}

void srotg_(float *a, float *b, float *c, float *s)
{
	if( !wrapper.fn[84] )
		wrapper.fn[84] = wrapper.Resolve("srotg",(void (*)()) REF_srotg_);
	(* reinterpret_cast<p_srotg>(wrapper.fn[84]))( a, b, c, s );

}

void ssbmv_(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[85] )
		wrapper.fn[85] = wrapper.Resolve("ssbmv",(void (*)()) REF_ssbmv_);
	(* reinterpret_cast<p_ssbmv>(wrapper.fn[85]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void sscal_(int *n, float *alpha, float *x, int *incx)
{
	if( !wrapper.fn[86] )
		wrapper.fn[86] = wrapper.Resolve("sscal",(void (*)()) REF_sscal_);
	(* reinterpret_cast<p_sscal>(wrapper.fn[86]))( n, alpha, x, incx );

}

void sspmv_(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[87] )
		wrapper.fn[87] = wrapper.Resolve("sspmv",(void (*)()) REF_sspmv_);
	(* reinterpret_cast<p_sspmv>(wrapper.fn[87]))( uplo, n, alpha, ap, x, incx, beta, y, incy, uplo_len );

}

void sspr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len)
{
	if( !wrapper.fn[88] )
		wrapper.fn[88] = wrapper.Resolve("sspr",(void (*)()) REF_sspr_);
	(* reinterpret_cast<p_sspr>(wrapper.fn[88]))( uplo, n, alpha, x, incx, ap, uplo_len );

}

void sspr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len)
{
	if( !wrapper.fn[89] )
		wrapper.fn[89] = wrapper.Resolve("sspr2",(void (*)()) REF_sspr2_);
	(* reinterpret_cast<p_sspr2>(wrapper.fn[89]))( uplo, n, alpha, x, incx, y, incy, ap, uplo_len );

}

void sswap_(int *n, float *x, int *incx, float *y, int *incy)
{
	if( !wrapper.fn[90] )
		wrapper.fn[90] = wrapper.Resolve("sswap",(void (*)()) REF_sswap_);
	(* reinterpret_cast<p_sswap>(wrapper.fn[90]))( n, x, incx, y, incy );

}

void ssymm_(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[91] )
		wrapper.fn[91] = wrapper.Resolve("ssymm",(void (*)()) REF_ssymm_);
	(* reinterpret_cast<p_ssymm>(wrapper.fn[91]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void ssymv_(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[92] )
		wrapper.fn[92] = wrapper.Resolve("ssymv",(void (*)()) REF_ssymv_);
	(* reinterpret_cast<p_ssymv>(wrapper.fn[92]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void ssyr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[93] )
		wrapper.fn[93] = wrapper.Resolve("ssyr",(void (*)()) REF_ssyr_);
	(* reinterpret_cast<p_ssyr>(wrapper.fn[93]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void ssyr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[94] )
		wrapper.fn[94] = wrapper.Resolve("ssyr2",(void (*)()) REF_ssyr2_);
	(* reinterpret_cast<p_ssyr2>(wrapper.fn[94]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void ssyr2k_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[95] )
		wrapper.fn[95] = wrapper.Resolve("ssyr2k",(void (*)()) REF_ssyr2k_);
	(* reinterpret_cast<p_ssyr2k>(wrapper.fn[95]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void ssyrk_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[96] )
		wrapper.fn[96] = wrapper.Resolve("ssyrk",(void (*)()) REF_ssyrk_);
	(* reinterpret_cast<p_ssyrk>(wrapper.fn[96]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void stbmv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[97] )
		wrapper.fn[97] = wrapper.Resolve("stbmv",(void (*)()) REF_stbmv_);
	(* reinterpret_cast<p_stbmv>(wrapper.fn[97]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void stbsv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[98] )
		wrapper.fn[98] = wrapper.Resolve("stbsv",(void (*)()) REF_stbsv_);
	(* reinterpret_cast<p_stbsv>(wrapper.fn[98]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void stpmv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[99] )
		wrapper.fn[99] = wrapper.Resolve("stpmv",(void (*)()) REF_stpmv_);
	(* reinterpret_cast<p_stpmv>(wrapper.fn[99]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void stpsv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[100] )
		wrapper.fn[100] = wrapper.Resolve("stpsv",(void (*)()) REF_stpsv_);
	(* reinterpret_cast<p_stpsv>(wrapper.fn[100]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void strmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[101] )
		wrapper.fn[101] = wrapper.Resolve("strmm",(void (*)()) REF_strmm_);
	(* reinterpret_cast<p_strmm>(wrapper.fn[101]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void strmv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[102] )
		wrapper.fn[102] = wrapper.Resolve("strmv",(void (*)()) REF_strmv_);
	(* reinterpret_cast<p_strmv>(wrapper.fn[102]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void strsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[103] )
		wrapper.fn[103] = wrapper.Resolve("strsm",(void (*)()) REF_strsm_);
	(* reinterpret_cast<p_strsm>(wrapper.fn[103]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void strsv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[104] )
		wrapper.fn[104] = wrapper.Resolve("strsv",(void (*)()) REF_strsv_);
	(* reinterpret_cast<p_strsv>(wrapper.fn[104]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void zaxpy_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[105] )
		wrapper.fn[105] = wrapper.Resolve("zaxpy",(void (*)()) REF_zaxpy_);
	(* reinterpret_cast<p_zaxpy>(wrapper.fn[105]))( n, alpha, x, incx, y, incy );

}

void zcopy_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[106] )
		wrapper.fn[106] = wrapper.Resolve("zcopy",(void (*)()) REF_zcopy_);
	(* reinterpret_cast<p_zcopy>(wrapper.fn[106]))( n, x, incx, y, incy );

}

void zdotc_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val)
{
	REF_zdotc_( n, x, incx, y, incy, ret_val );
}

void zdotu_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val)
{
	REF_zdotu_( n, x, incx, y, incy, ret_val );
}

void zdscal_(int *n, double *alpha, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[109] )
		wrapper.fn[109] = wrapper.Resolve("zdscal",(void (*)()) REF_zdscal_);
	(* reinterpret_cast<p_zdscal>(wrapper.fn[109]))( n, alpha, x, incx );

}

void zgbmv_(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[110] )
		wrapper.fn[110] = wrapper.Resolve("zgbmv",(void (*)()) REF_zgbmv_);
	(* reinterpret_cast<p_zgbmv>(wrapper.fn[110]))( trans, m, n, kl, ku, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void zgemm_(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len)
{
	if( !wrapper.fn[111] )
		wrapper.fn[111] = wrapper.Resolve("zgemm",(void (*)()) REF_zgemm_);
	(* reinterpret_cast<p_zgemm>(wrapper.fn[111]))( transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc, transa_len, transb_len );

}

void zgemv_(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len)
{
	if( !wrapper.fn[112] )
		wrapper.fn[112] = wrapper.Resolve("zgemv",(void (*)()) REF_zgemv_);
	(* reinterpret_cast<p_zgemv>(wrapper.fn[112]))( trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len );

}

void zgerc_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda)
{
	if( !wrapper.fn[113] )
		wrapper.fn[113] = wrapper.Resolve("zgerc",(void (*)()) REF_zgerc_);
	(* reinterpret_cast<p_zgerc>(wrapper.fn[113]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zgeru_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda)
{
	if( !wrapper.fn[114] )
		wrapper.fn[114] = wrapper.Resolve("zgeru",(void (*)()) REF_zgeru_);
	(* reinterpret_cast<p_zgeru>(wrapper.fn[114]))( m, n, alpha, x, incx, y, incy, a, lda );

}

void zhbmv_(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[115] )
		wrapper.fn[115] = wrapper.Resolve("zhbmv",(void (*)()) REF_zhbmv_);
	(* reinterpret_cast<p_zhbmv>(wrapper.fn[115]))( uplo, n, k, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void zhemm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[116] )
		wrapper.fn[116] = wrapper.Resolve("zhemm",(void (*)()) REF_zhemm_);
	(* reinterpret_cast<p_zhemm>(wrapper.fn[116]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void zhemv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[117] )
		wrapper.fn[117] = wrapper.Resolve("zhemv",(void (*)()) REF_zhemv_);
	(* reinterpret_cast<p_zhemv>(wrapper.fn[117]))( uplo, n, alpha, a, lda, x, incx, beta, y, incy, uplo_len );

}

void zher_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[118] )
		wrapper.fn[118] = wrapper.Resolve("zher",(void (*)()) REF_zher_);
	(* reinterpret_cast<p_zher>(wrapper.fn[118]))( uplo, n, alpha, x, incx, a, lda, uplo_len );

}

void zher2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len)
{
	if( !wrapper.fn[119] )
		wrapper.fn[119] = wrapper.Resolve("zher2",(void (*)()) REF_zher2_);
	(* reinterpret_cast<p_zher2>(wrapper.fn[119]))( uplo, n, alpha, x, incx, y, incy, a, lda, uplo_len );

}

void zher2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[120] )
		wrapper.fn[120] = wrapper.Resolve("zher2k",(void (*)()) REF_zher2k_);
	(* reinterpret_cast<p_zher2k>(wrapper.fn[120]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void zherk_(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[121] )
		wrapper.fn[121] = wrapper.Resolve("zherk",(void (*)()) REF_zherk_);
	(* reinterpret_cast<p_zherk>(wrapper.fn[121]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void zhpmv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len)
{
	if( !wrapper.fn[122] )
		wrapper.fn[122] = wrapper.Resolve("zhpmv",(void (*)()) REF_zhpmv_);
	(* reinterpret_cast<p_zhpmv>(wrapper.fn[122]))( uplo, n, alpha, a, x, incx, beta, y, incy, uplo_len );

}

void zhpr_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len)
{
	if( !wrapper.fn[123] )
		wrapper.fn[123] = wrapper.Resolve("zhpr",(void (*)()) REF_zhpr_);
	(* reinterpret_cast<p_zhpr>(wrapper.fn[123]))( uplo, n, alpha, x, incx, a, uplo_len );

}

void zhpr2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len)
{
	if( !wrapper.fn[124] )
		wrapper.fn[124] = wrapper.Resolve("zhpr2",(void (*)()) REF_zhpr2_);
	(* reinterpret_cast<p_zhpr2>(wrapper.fn[124]))( uplo, n, alpha, x, incx, y, incy, a, uplo_len );

}

void zrotg_(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s)
{
	if( !wrapper.fn[125] )
		wrapper.fn[125] = wrapper.Resolve("zrotg",(void (*)()) REF_zrotg_);
	(* reinterpret_cast<p_zrotg>(wrapper.fn[125]))( a, b, c, s );

}

void zscal_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx)
{
	if( !wrapper.fn[126] )
		wrapper.fn[126] = wrapper.Resolve("zscal",(void (*)()) REF_zscal_);
	(* reinterpret_cast<p_zscal>(wrapper.fn[126]))( n, alpha, x, incx );

}

void zswap_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy)
{
	if( !wrapper.fn[127] )
		wrapper.fn[127] = wrapper.Resolve("zswap",(void (*)()) REF_zswap_);
	(* reinterpret_cast<p_zswap>(wrapper.fn[127]))( n, x, incx, y, incy );

}

void zsymm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len)
{
	if( !wrapper.fn[128] )
		wrapper.fn[128] = wrapper.Resolve("zsymm",(void (*)()) REF_zsymm_);
	(* reinterpret_cast<p_zsymm>(wrapper.fn[128]))( side, uplo, m, n, alpha, a, lda, b, ldb, beta, c, ldc, side_len, uplo_len );

}

void zsyr2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[129] )
		wrapper.fn[129] = wrapper.Resolve("zsyr2k",(void (*)()) REF_zsyr2k_);
	(* reinterpret_cast<p_zsyr2k>(wrapper.fn[129]))( uplo, trans, n, k, alpha, a, lda, b, ldb, beta, c, ldc, uplo_len, trans_len );

}

void zsyrk_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len)
{
	if( !wrapper.fn[130] )
		wrapper.fn[130] = wrapper.Resolve("zsyrk",(void (*)()) REF_zsyrk_);
	(* reinterpret_cast<p_zsyrk>(wrapper.fn[130]))( uplo, trans, n, k, alpha, a, lda, beta, c, ldc, uplo_len, trans_len );

}

void ztbmv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[131] )
		wrapper.fn[131] = wrapper.Resolve("ztbmv",(void (*)()) REF_ztbmv_);
	(* reinterpret_cast<p_ztbmv>(wrapper.fn[131]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztbsv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[132] )
		wrapper.fn[132] = wrapper.Resolve("ztbsv",(void (*)()) REF_ztbsv_);
	(* reinterpret_cast<p_ztbsv>(wrapper.fn[132]))( uplo, trans, diag, n, k, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztpmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[133] )
		wrapper.fn[133] = wrapper.Resolve("ztpmv",(void (*)()) REF_ztpmv_);
	(* reinterpret_cast<p_ztpmv>(wrapper.fn[133]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ztpsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[134] )
		wrapper.fn[134] = wrapper.Resolve("ztpsv",(void (*)()) REF_ztpsv_);
	(* reinterpret_cast<p_ztpsv>(wrapper.fn[134]))( uplo, trans, diag, n, ap, x, incx, uplo_len, trans_len, diag_len );

}

void ztrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[135] )
		wrapper.fn[135] = wrapper.Resolve("ztrmm",(void (*)()) REF_ztrmm_);
	(* reinterpret_cast<p_ztrmm>(wrapper.fn[135]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ztrmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[136] )
		wrapper.fn[136] = wrapper.Resolve("ztrmv",(void (*)()) REF_ztrmv_);
	(* reinterpret_cast<p_ztrmv>(wrapper.fn[136]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}

void ztrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len)
{
	if( !wrapper.fn[137] )
		wrapper.fn[137] = wrapper.Resolve("ztrsm",(void (*)()) REF_ztrsm_);
	(* reinterpret_cast<p_ztrsm>(wrapper.fn[137]))( side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb, side_len, uplo_len, transa_len, diag_len );

}

void ztrsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len)
{
	if( !wrapper.fn[138] )
		wrapper.fn[138] = wrapper.Resolve("ztrsv",(void (*)()) REF_ztrsv_);
	(* reinterpret_cast<p_ztrsv>(wrapper.fn[138]))( uplo, trans, diag, n, a, lda, x, incx, uplo_len, trans_len, diag_len );

}



#ifdef __cplusplus 
}
#endif /* __cplusplus */
