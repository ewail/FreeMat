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
#include "LAPACK.hpp"
#include "LUDecompose.hpp"
#include "SparseCCS.hpp"
#include "MemPtr.hpp"
#include "Math.hpp"
#include "Algorithms.hpp"
#if HAVE_UMFPACK
extern "C" {
#include "umfpack.h"
}
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

int* FixupPivotVector(int nrows, int *piv, int p) {
  // Adjust the pivot vector
  int *fullpivot = (int*) malloc(sizeof(int)*nrows);
  int i;
  for (i=0;i<nrows;i++)
    fullpivot[i] = i;
  for (i=0;i<p;i++) 
    if (piv[i] != (i+1)) {
      int tmp = fullpivot[i];
      fullpivot[i] = fullpivot[piv[i]-1];
      fullpivot[piv[i]-1] = tmp;
    }
  free(piv);
  return fullpivot;
}

// In all cases, l is m x n  and   u is n x n if m > n
//               l is m x m  and   u is m x n if m < n
//           or  l is m x p  and   u is p x n

template <class T>
void RealLUP(int nrows, int ncols, T *l, T *u,
	     T *pmat, T *a, 
	     void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  //  sgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[i+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[i+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
}

template <class T>
void RealLU(int nrows, int ncols, T *l, T *u, T*a, 
	    void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  //  dgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
  free(fullpivot);
}

template <class T>
void ComplexLUP(int nrows, int ncols, T *l, T *u,
		T *pmat, T *a, 
		void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
}

template <class T>
void ComplexLU(int nrows, int ncols, T *l, T *u, T*a, 
	       void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
  free(fullpivot);
}


ArrayVector LUDecompose(int nargout, Array A) {
  if (nargout > 3)
    throw Exception("illegal usage of lu function - for dense matrices, maximum number of outputs is 3");
  int nrows = int(A.rows());
  int ncols = int(A.columns());
  int p = min(nrows,ncols);
  ArrayVector retval;
  if (A.isSparse())
    throw Exception("lu function does not work on sparse inputs");
  A = A.asDenseArray();
  switch (A.dataClass()) {
  default:
    throw Exception("unhandled type for lu function");
  case Float:
    {
      if (A.allReal()) {
	BasicArray<float> l(NTuple(nrows,p));
	BasicArray<float> u(NTuple(p,ncols));
	if (nargout <= 2) {
	  if (p != 0)
	    RealLU<float>(nrows,ncols,l.data(),u.data(),A.real<float>().data(),sgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	} else if (nargout == 3) {
	  BasicArray<float> piv(NTuple(nrows,nrows));
	  if (p != 0)
	    RealLUP<float>(nrows,ncols,l.data(),u.data(),piv.data(),A.real<float>().data(),sgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	  retval.push_back(Array(piv));
	}
	return retval;
      } else {
	BasicArray<float> l(NTuple(2*nrows,p));
	BasicArray<float> u(NTuple(2*p,ncols));
	if (nargout <= 2) {
	  if (p != 0)
	    ComplexLU<float>(nrows,ncols,l.data(),u.data(),
			     A.fortran<float>().data(),cgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	} else if (nargout == 3) {
	  BasicArray<float> piv(NTuple(nrows,nrows));
	  if (p != 0)
	    ComplexLUP<float>(nrows,ncols,l.data(),u.data(),piv.data(),
			      A.fortran<float>().data(),cgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	  retval.push_back(Array(piv));
	}
      }
    }
  case Double: 
    {
      if (A.allReal()) {
	BasicArray<double> l(NTuple(nrows,p));
	BasicArray<double> u(NTuple(p,ncols));
	if (nargout <= 2) {
	  if (p != 0)
	    RealLU<double>(nrows,ncols,l.data(),u.data(),A.real<double>().data(),dgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	} else if (nargout == 3) {
	  BasicArray<double> piv(NTuple(nrows,nrows));
	  if (p != 0)
	    RealLUP<double>(nrows,ncols,l.data(),u.data(),piv.data(),
			    A.real<double>().data(),dgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	  retval.push_back(Array(piv));
	}
	return retval;
      } else {
	BasicArray<double> l(NTuple(2*nrows,p));
	BasicArray<double> u(NTuple(2*p,ncols));
	if (nargout <= 2) {
	  if (p != 0)
	    ComplexLU<double>(nrows,ncols,l.data(),u.data(),
			      A.fortran<double>().data(),zgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	} else if (nargout == 3) {
	  BasicArray<double> piv(NTuple(nrows,nrows));
	  if (p != 0)
	    ComplexLUP<double>(nrows,ncols,l.data(),u.data(),piv.data(),
			       A.fortran<double>().data(),zgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	  retval.push_back(Array(piv));
	}
      }
    }
  }
  return retval;
}

template <typename T>
static Array InvertMatrixReal(const BasicArray<T> &A,
			      void (*getrf)(int*,int*,T*,int*,int*,int*),
			      void (*getri)(int*,T*,int*,int*,T*,int*,int*)) {
  int M = int(A.rows());
  int N = int(A.columns());
  BasicArray<T> Acopy(A);
  int LDA = M;
  BasicArray<int> IPIV(NTuple(qMin(M,N),1));
  int INFO;
  // Do the decomposition
  getrf(&M,&N,Acopy.data(),&LDA,IPIV.data(),&INFO);
  // Compute the inverse
  T WORKSIZE;
  int IWORKSIZE = -1;
  getri(&N,Acopy.data(),&LDA,IPIV.data(),&WORKSIZE,&IWORKSIZE,&INFO);
  IWORKSIZE = int(WORKSIZE);
  BasicArray<T> WORK(NTuple(IWORKSIZE,1));
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORK.data(),&IWORKSIZE,&INFO);
  if (INFO > 0) Acopy.fill(Inf());
  return Array(Acopy);
}

template <typename T>
static Array InvertMatrixComplex(const BasicArray<T> &A,
				 void (*getrf)(int*,int*,T*,int*,int*,int*),
				 void (*getri)(int*,T*,int*,int*,T*,int*,int*)) {
  int M = int(A.rows()/2);
  int N = int(A.columns());
  BasicArray<T> Acopy(A);
  int LDA = M;
  BasicArray<int> IPIV(NTuple(qMin(M,N),1));
  int INFO;
  // Do the decomposition
  getrf(&M,&N,Acopy.data(),&LDA,IPIV.data(),&INFO);
  // Compute the inverse
  T WORKSIZE[2];
  int IWORKSIZE = -1;
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORKSIZE,&IWORKSIZE,&INFO);
  IWORKSIZE = int(WORKSIZE[0]);
  BasicArray<T> WORK(NTuple(IWORKSIZE*2,1));
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORK.data(),&IWORKSIZE,&INFO);
  BasicArray<T> Acopy_real(SplitReal<T>(Acopy));
  BasicArray<T> Acopy_imag(SplitImag<T>(Acopy));
  if (INFO > 0) {
    Acopy_real.fill(Inf());
    Acopy_imag.fill(0);
  }
  return Array(Acopy_real,Acopy_imag);
}

Array Invert(const Array &A) {
  if (A.isSparse()) 
    throw Exception("Sparse matrix inverse not currently supported");
  if (A.isEmpty()) return A;
  switch (A.dataClass()) {
  default: throw Exception("Invert does not support this data class");
  case Float:
    if (A.allReal())
      return InvertMatrixReal(A.constReal<float>(),sgetrf_,sgetri_);
    else
      return InvertMatrixComplex(A.fortran<float>(),cgetrf_,cgetri_);
  case Double:
    if (A.allReal())
      return InvertMatrixReal(A.constReal<double>(),dgetrf_,dgetri_);
    else
      return InvertMatrixComplex(A.fortran<double>(),zgetrf_,zgetri_);
  }
}

static ArrayVector SparseLUDecomposeReal(const SparseMatrix<double> &A) {
#if HAVE_UMFPACK
  // Convert A into CCS form
  int32 Arows = int32(A.rows());
  int32 Acols = int32(A.cols());
  QVector<uint32> rowindx, colstart;
  QVector<double> accsdata;
  SparseToCCS(A,rowindx,colstart,accsdata);
  double *null = (double *) NULL ;
  void *Symbolic, *Numeric ;
  (void) umfpack_di_symbolic(Acols, Acols, (const int*) colstart.data(),
			     (const int*) rowindx.data(), accsdata.data(),
			     &Symbolic, null, null);
  (void) umfpack_di_numeric((const int*) colstart.data(), (const int*) rowindx.data(),
			    accsdata.data(), Symbolic, 
			    &Numeric, null, null);

  // Set up the output arrays for the LU Decomposition.
  // The first matrix is L, which is stored in comprssed row form.
  int lnz;
  int unz;
  int n_row;
  int n_col;
  int nz_udiag;

  (void) umfpack_di_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

  QVector<uint32> Lp(Arows+1);
  QVector<uint32> Lj(lnz);
  QVector<double> Lx(lnz);
  QVector<uint32> Up(Acols+1);
  QVector<uint32> Uj(unz);
  QVector<double> Ux(unz);
  BasicArray<int32> P(NTuple(Arows,1));
  BasicArray<int32> Q(NTuple(Acols,1));
  MacroBlockAlloc(double,Arows,Rsb,Rs);
    
  int do_recip;
  umfpack_di_get_numeric((int*)Lp.data(), (int*)Lj.data(), Lx.data(), 
			 (int*)Up.data(), (int*)Uj.data(), Ux.data(), 
			 P.data(), Q.data(), NULL, &do_recip, Rs, Numeric);
  SparseMatrix<double> RMat(NTuple(Arows,Arows));
  for (index_t i=1;i<=Arows;i++) {
    if (do_recip)
      RMat.set(NTuple(i,i),Rs[int(i-1)]);
    else
      RMat.set(NTuple(i,i),1.0/Rs[int(i-1)]);
  }
  Array Pc(Add(Array(P).toClass(Double),Array(double(1))));
  Array Qc(Add(Array(Q).toClass(Double),Array(double(1))));

  ArrayVector retval;
  // Push L, U, P, Q, R
  int Amid;
  Amid = (Arows < Acols) ? Arows : Acols;
  SparseMatrix<double> LMat(NTuple(Arows,Amid));
  CCSToSparse(LMat,Lj,Lp,Lx);
  SparseMatrix<double> UMat(NTuple(Amid,Acols));
  CCSToSparse(UMat,Uj,Up,Ux);
  retval << Transpose(Array(LMat));
  retval << Array(UMat);
  retval << Pc;
  retval << Qc;
  retval << Array(RMat);
  umfpack_di_free_symbolic(&Symbolic);
  umfpack_di_free_numeric(&Numeric);
  return retval;
#else
  throw Exception("LU Decompositions of sparse matrices requires UMFPACK support, which was not available at compile time.  You must have UMFPACK installed at compile time for FreeMat to enable this functionality.");
#endif
}

static ArrayVector SparseLUDecomposeComplex(const SparseMatrix<double> &Ar, const SparseMatrix<double> &Ai) {
#if HAVE_UMFPACK
  // Convert A into CCS form
  int32 Arows = int32(Ar.rows());
  int32 Acols = int32(Ar.cols());
  QVector<uint32> rowindx, colstart;
  QVector<double> accsreal, accsimag;
  SparseToCCS(Ar,Ai,rowindx,colstart,accsreal,accsimag);
  double *null = (double *) NULL ;
  void *Symbolic, *Numeric ;
  (void) umfpack_zi_symbolic (Acols, Acols, (const int*) colstart.data(),
			      (const int*) rowindx.data(), accsreal.data(),
			      accsimag.data(), &Symbolic, null, null);
  (void) umfpack_zi_numeric ((const int*) colstart.data(), (const int*) rowindx.data(),
			     accsreal.data(), accsimag.data(), Symbolic, &Numeric, null, null);
  // Set up the output arrays for the LU Decomposition.
  // The first matrix is L, which is stored in comprssed row form.
  int lnz;
  int unz;
  int n_row;
  int n_col;
  int nz_udiag;

  (void) umfpack_zi_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

  QVector<uint32> Lp(Arows+1);
  QVector<uint32> Lj(lnz);
  QVector<double> Lx(lnz);
  QVector<double> Ly(lnz);
  QVector<uint32> Up(Acols+1);
  QVector<uint32> Uj(unz);
  QVector<double> Ux(unz);
  QVector<double> Uy(unz);
  BasicArray<int32> P(NTuple(Arows,1));
  BasicArray<int32> Q(NTuple(Acols,1));
  MacroBlockAlloc(double,Arows,Rsb,Rs);
  
  int do_recip;
  umfpack_zi_get_numeric((int*)Lp.data(), (int*)Lj.data(), Lx.data(), Ly.data(), 
			 (int*)Up.data(), (int*)Uj.data(), Ux.data(), Uy.data(), 
			 P.data(), Q.data(), NULL, NULL, &do_recip, Rs, Numeric);
  SparseMatrix<double> RMat(NTuple(Arows,Arows));
  for (index_t i=1;i<=Arows;i++) {
    if (do_recip)
      RMat.set(NTuple(i,i),Rs[int(i-1)]);
    else
      RMat.set(NTuple(i,i),1.0/Rs[int(i-1)]);
  }
  Array Pc(Add(Array(P).toClass(Double),Array(double(1))));
  Array Qc(Add(Array(Q).toClass(Double),Array(double(1))));

  ArrayVector retval;
  // Push L, U, P, Q, R
  int Amid;
  Amid = (Arows < Acols) ? Arows : Acols;
  SparseMatrix<double> LMat_real(NTuple(Arows,Amid));
  SparseMatrix<double> LMat_imag(NTuple(Arows,Amid));
  CCSToSparse(LMat_real,LMat_imag,Lj,Lp,Lx,Ly);
  SparseMatrix<double> UMat_real(NTuple(Amid,Acols));
  SparseMatrix<double> UMat_imag(NTuple(Amid,Acols));
  CCSToSparse(UMat_real,UMat_imag,Uj,Up,Ux,Uy);
  retval << Transpose(Array(LMat_real,LMat_imag));
  retval << Array(UMat_real,UMat_imag);
  retval << Pc;
  retval << Qc;
  retval << Array(RMat);
  umfpack_di_free_symbolic(&Symbolic);
  umfpack_di_free_numeric(&Numeric);
  return retval;
#else
  throw Exception("LU Decompositions of sparse matrices requires UMFPACK support, which was not available at compile time.  You must have UMFPACK installed at compile time for FreeMat to enable this functionality.");
#endif
}

  
ArrayVector SparseLUDecompose(int nargout, Array A) {
  if (A.dataClass() != Double)
    throw Exception("FreeMat currently only supports the LU decomposition for double and dcomplex matrices");
  if (!A.isSquare())
    throw Exception("FreeMat currently only supports LU decompositions for square matrices");
  if (A.allReal())
    return SparseLUDecomposeReal(A.constRealSparse<double>());
  else
    return SparseLUDecomposeComplex(A.constRealSparse<double>(), A.constImagSparse<double>());
}

