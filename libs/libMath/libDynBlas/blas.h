//This is a generated file. Do not edit!
#ifndef __BLAS_H
#define __BLAS_H

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

void caxpy_(int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy);
void ccopy_(int *n, complex *x, int *incx, complex *y, int *incy);
void cdotc_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
void cdotu_(int *n, complex *x, int *incx, complex *y, int *incy, complex *ret_val);
void cgbmv_(char *trans, int *m, int *n, int *kl, int *ku, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
void cgemm_(char *transa, char *transb, int *m, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int transa_len, int transb_len);
void cgemv_(char *trans, int *m, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int trans_len);
void cgerc_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
void cgeru_(int *m, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda);
void chbmv_(char *uplo, int *n, int *k, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void chemm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
void chemv_(char *uplo, int *n, complex *alpha, complex *a, int *lda, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void cher_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int *lda, int uplo_len);
void cher2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int *lda, int uplo_len);
void cher2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void cherk_(char *uplo, char *trans, int *n, int *k, float *alpha, complex *a, int *lda, float *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void chpmv_(char *uplo, int *n, complex *alpha, complex *a, complex *x, int *incx, complex *beta, complex *y, int *incy, int uplo_len);
void chpr_(char *uplo, int *n, float *alpha, complex *x, int *incx, complex *a, int uplo_len);
void chpr2_(char *uplo, int *n, complex *alpha, complex *x, int *incx, complex *y, int *incy, complex *a, int uplo_len);
void crotg_(complex *a, complex *b, float *c, complex *s);
void cscal_(int *n, complex *alpha, complex *x, int *incx);
void csscal_(int *n, float *alpha, complex *x, int *incx);
void cswap_(int *n, complex *x, int *incx, complex *y, int *incy);
void csymm_(char *side, char *uplo, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int side_len, int uplo_len);
void csyr2k_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *b, int *ldb, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void csyrk_(char *uplo, char *trans, int *n, int *k, complex *alpha, complex *a, int *lda, complex *beta, complex *c, int *ldc, int uplo_len, int trans_len);
void ctbmv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ctbsv_(char *uplo, char *trans, char *diag, int *n, int *k, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ctpmv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ctpsv_(char *uplo, char *trans, char *diag, int *n, complex *ap, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ctrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void ctrmv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ctrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, complex *alpha, complex *a, int *lda, complex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void ctrsv_(char *uplo, char *trans, char *diag, int *n, complex *a, int *lda, complex *x, int *incx, int uplo_len, int trans_len, int diag_len);
double dasum_(int *n, double *x, int *incx);
void daxpy_(int *n, double *alpha, double *x, int *incx, double *y, int *incy);
double dcabs1_(doublecomplex *z);
void dcopy_(int *n, double *x, int *incx, double *y, int *incy);
double ddot_(int *n, double *x, int *incx, double *y, int *incy);
void dgbmv_(char *trans, int *m, int *n, int *kl, int *ku, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
void dgemm_(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int transa_len, int transb_len);
void dgemv_(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int trans_len);
void dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda);
double dnrm2_(int *n, double *x, int *incx);
void drot_(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s);
void drotg_(double *a, double *b, double *c, double *s);
void dsbmv_(char *uplo, int *n, int *k, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void dscal_(int *n, double *alpha, double *x, int *incx);
void dspmv_(char *uplo, int *n, double *alpha, double *ap, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void dspr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *ap, int uplo_len);
void dspr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *ap, int uplo_len);
void dswap_(int *n, double *x, int *incx, double *y, int *incy);
void dsymm_(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int side_len, int uplo_len);
void dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int uplo_len);
void dsyr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int uplo_len);
void dsyr2_(char *uplo, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda, int uplo_len);
void dsyr2k_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
void dsyrk_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *c, int *ldc, int uplo_len, int trans_len);
void dtbmv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void dtbsv_(char *uplo, char *trans, char *diag, int *n, int *k, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void dtpmv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void dtpsv_(char *uplo, char *trans, char *diag, int *n, double *ap, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void dtrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void dtrmv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
void dtrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void dtrsv_(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx, int uplo_len, int trans_len, int diag_len);
double dzasum_(int *n, doublecomplex *x, int *incx);
double dznrm2_(int *n, doublecomplex *x, int *incx);
int icamax_(int *n, complex *x, int *incx);
int idamax_(int *n, double *x, int *incx);
int isamax_(int *n, float *x, int *incx);
int izamax_(int *n, doublecomplex *x, int *incx);
double sasum_(int *n, float *x, int *incx);
void saxpy_(int *n, float *alpha, float *x, int *incx, float *y, int *incy);
double scasum_(int *n, complex *x, int *incx);
double scnrm2_(int *n, complex *x, int *incx);
void scopy_(int *n, float *x, int *incx, float *y, int *incy);
double sdot_(int *n, float *x, int *incx, float *y, int *incy);
void sgbmv_(char *trans, int *m, int *n, int *kl, int *ku, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
void sgemm_(char *transa, char *transb, int *m, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int transa_len, int transb_len);
void sgemv_(char *trans, int *m, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int trans_len);
void sger_(int *m, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda);
double snrm2_(int *n, float *x, int *incx);
void srot_(int *n, float *x, int *incx, float *y, int *incy, float *c, float *s);
void srotg_(float *a, float *b, float *c, float *s);
void ssbmv_(char *uplo, int *n, int *k, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void sscal_(int *n, float *alpha, float *x, int *incx);
void sspmv_(char *uplo, int *n, float *alpha, float *ap, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void sspr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *ap, int uplo_len);
void sspr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *ap, int uplo_len);
void sswap_(int *n, float *x, int *incx, float *y, int *incy);
void ssymm_(char *side, char *uplo, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int side_len, int uplo_len);
void ssymv_(char *uplo, int *n, float *alpha, float *a, int *lda, float *x, int *incx, float *beta, float *y, int *incy, int uplo_len);
void ssyr_(char *uplo, int *n, float *alpha, float *x, int *incx, float *a, int *lda, int uplo_len);
void ssyr2_(char *uplo, int *n, float *alpha, float *x, int *incx, float *y, int *incy, float *a, int *lda, int uplo_len);
void ssyr2k_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
void ssyrk_(char *uplo, char *trans, int *n, int *k, float *alpha, float *a, int *lda, float *beta, float *c, int *ldc, int uplo_len, int trans_len);
void stbmv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void stbsv_(char *uplo, char *trans, char *diag, int *n, int *k, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void stpmv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void stpsv_(char *uplo, char *trans, char *diag, int *n, float *ap, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void strmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void strmv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void strsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, float *alpha, float *a, int *lda, float *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void strsv_(char *uplo, char *trans, char *diag, int *n, float *a, int *lda, float *x, int *incx, int uplo_len, int trans_len, int diag_len);
void zaxpy_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void zcopy_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void zdotc_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
void zdotu_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *ret_val);
void zdscal_(int *n, double *alpha, doublecomplex *x, int *incx);
void zgbmv_(char *trans, int *m, int *n, int *kl, int *ku, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
void zgemm_(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int transa_len, int transb_len);
void zgemv_(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int trans_len);
void zgerc_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
void zgeru_(int *m, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda);
void zhbmv_(char *uplo, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void zhemm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
void zhemv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void zher_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int *lda, int uplo_len);
void zher2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int *lda, int uplo_len);
void zher2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void zherk_(char *uplo, char *trans, int *n, int *k, double *alpha, doublecomplex *a, int *lda, double *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void zhpmv_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *a, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy, int uplo_len);
void zhpr_(char *uplo, int *n, double *alpha, doublecomplex *x, int *incx, doublecomplex *a, int uplo_len);
void zhpr2_(char *uplo, int *n, doublecomplex *alpha, doublecomplex *x, int *incx, doublecomplex *y, int *incy, doublecomplex *a, int uplo_len);
void zrotg_(doublecomplex *a, doublecomplex *b, double *c, doublecomplex *s);
void zscal_(int *n, doublecomplex *alpha, doublecomplex *x, int *incx);
void zswap_(int *n, doublecomplex *x, int *incx, doublecomplex *y, int *incy);
void zsymm_(char *side, char *uplo, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int side_len, int uplo_len);
void zsyr2k_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void zsyrk_(char *uplo, char *trans, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *beta, doublecomplex *c, int *ldc, int uplo_len, int trans_len);
void ztbmv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ztbsv_(char *uplo, char *trans, char *diag, int *n, int *k, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ztpmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ztpsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *ap, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ztrmm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void ztrmv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);
void ztrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, int side_len, int uplo_len, int transa_len, int diag_len);
void ztrsv_(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx, int uplo_len, int trans_len, int diag_len);


#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /*_BLAS_H*/
