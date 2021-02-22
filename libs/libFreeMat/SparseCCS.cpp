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
#include "SparseCCS.hpp"
#include "Math.hpp"
#include "Algorithms.hpp"

// Normally, the i,j,v representation for a sparse matrix has
// a great degree of redundancy in one of the components.  If
// j is the column coordinate then a vector such as:
//   1 3
//   3 3
//   5 3

// In: 1 1 2 2 3 3 3 3 4 5 5
// out: 

QVector<uint32> CompressCCSCols(const QVector<uint32> &cols, index_t colcount) {
  QVector<uint32> y(int(colcount+1));
  // Scan through the columns
  int mp=0;
  for (unsigned col=1;col<=unsigned(colcount+1);col++) {
    while ((mp < cols.size()) && (cols[mp] < col)) mp++;
    y[col-1] = mp;
  }
  return y;
}

QVector<uint32> DecompressCCSCols(const QVector<uint32> &colstart, index_t colcount) {
  QVector<uint32> x;
  for (int p=1;p<colstart.size();p++) {
    for (unsigned n=0;n<unsigned(colstart[p] - colstart[p-1]);n++)
      x << p;
  }
  return x;
}

template <typename T>
static void TSparseMatToIJV(const SparseMatrix<T>&A, QVector<index_t> &rows,
			    QVector<index_t> &cols, QVector<T> &Adata) {
  ConstSparseIterator<T> iter(&A);
  while (iter.isValid()) {
    cols << iter.col();
    rows << iter.row();
    Adata << iter.value();
    iter.next();
  }
}

template <typename T>
static void TSparseMatToIJV(const SparseMatrix<T> &Areal, const SparseMatrix<T> &Aimag,
			    QVector<index_t> &rows,	QVector<index_t> &cols,
			    QVector<T> &Areal_part,	QVector<T> &Aimag_part) {
  ConstComplexSparseIterator<T> iter(&Areal,&Aimag);
  while (iter.isValid()) {
    cols << iter.col();
    rows << iter.row();
    Areal_part << iter.realValue();
    Aimag_part << iter.imagValue();
    iter.next();
  }
}

template <typename T>
static Array TSparseToIJV(const Array &A, Array &rows, Array &cols) {
  QVector<index_t> adata_rows;
  QVector<index_t> adata_cols;
  QVector<T> adata_real;
  QVector<T> adata_imag;
  if (A.allReal()) {
    TSparseMatToIJV(A.constRealSparse<T>(),adata_rows,adata_cols,adata_real);
    rows = Array(ToBasicArray(adata_rows));
    cols = Array(ToBasicArray(adata_cols));
    return Array(ToBasicArray(adata_real));
  } else {
    TSparseMatToIJV(A.constRealSparse<T>(),A.constImagSparse<T>(),
		    adata_rows,adata_cols,adata_real,adata_imag);
    rows = Array(ToBasicArray(adata_rows));
    cols = Array(ToBasicArray(adata_cols));
    return Array(ToBasicArray(adata_real),ToBasicArray(adata_imag));
  }
}

#define MacroSparseToIJV(ctype,cls)			\
  case cls: return TSparseToIJV<ctype>(a,rows,cols);

Array SparseToIJV(const Array &a, Array &rows, Array &cols) {
  switch (a.dataClass()) {
  default: throw Exception("unhandled case for sparse-to-ijv");
    MacroExpandCases(MacroSparseToIJV);
  }
}

#undef MacroSparseToIJV


// Array IJVForm::indices() {
//   return Add(Multiply(Subtract(m_cols,Array(double(1))),
// 		      Array(double(m_size_rows))),m_rows); 
// }


template <typename T>
static SparseMatrix<T> TIJVToSparse(const BasicArray<index_t> &ip,
				    const BasicArray<index_t> &jp,
				    const BasicArray<T> &dp,
				    index_t rows, index_t cols) {
  index_t ii = (ip.isScalar() == 1) ? 0 : 1;
  index_t jj = (jp.isScalar() == 1) ? 0 : 1;
  index_t dd = (dp.isScalar() == 1) ? 0 : 1;
  SparseMatrix<T> retval(NTuple(rows,cols));
  index_t len = qMax(ip.length(),qMax(jp.length(),dp.length()));
  for (index_t i=1;i<=len;i++)
    retval[NTuple(ip[(i-1)*ii+1],jp[(i-1)*jj+1])] = dp[(i-1)*dd+1];
  return retval;
}

template <typename T>
static Array TIJVToSparse(const BasicArray<index_t> &ip,
			  const BasicArray<index_t> &jp,
			  const Array &dat, index_t rows, 
			  index_t cols) {
  if (dat.allReal())
    return Array(TIJVToSparse(ip,jp,dat.constReal<T>(),rows,cols));
  else
    return Array(TIJVToSparse(ip,jp,dat.constReal<T>(),rows,cols),
		 TIJVToSparse(ip,jp,dat.constImag<T>(),rows,cols));
}

#define MacroIJVToSparse(ctype,cls)				\
  case cls: return TIJVToSparse<ctype>(ip,jp,dat,rows,cols);

Array IJVToSparse(const BasicArray<index_t> &ip,
		  const BasicArray<index_t> &jp,
		  const Array &dat,
		  index_t rows, index_t cols) {
  switch (dat.dataClass()) {
  default: throw Exception("unhandled type for ijv-to-sparse");
    MacroExpandCases(MacroIJVToSparse);
  }
}

Array IJVToSparse(const BasicArray<index_t> &ip,
		  const BasicArray<index_t> &jp,
		  const Array &dat) {
  return IJVToSparse(ip,jp,dat.asDenseArray(),MaxValue(ip),MaxValue(jp));
}
