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
#include "Operators.hpp"
#include "Math.hpp"
#include "Complex.hpp"

template <class T>
struct XNEntryReal {
  index_t n;
  T x;
};

template <class T>
static bool RealLess(const XNEntryReal<T>& a, const XNEntryReal<T>& b) {
  return a.x < b.x;
}

template <>
bool RealLess(const XNEntryReal<Array>& a, const XNEntryReal<Array>& b) {
  return a.x.asString() < b.x.asString();
}

template <class T>
static bool RealGreater(const XNEntryReal<T>& a, const XNEntryReal<T>& b) {
  return a.x > b.x;
}

template <>
bool RealGreater(const XNEntryReal<Array>& a, const XNEntryReal<Array>& b) {
  return a.x.asString() > b.x.asString();
}

template <class T>
struct XNEntryComplex {
  index_t n;
  T x;
  T y;
};

template <class T>
static bool ComplexLess(const XNEntryComplex<T>& a, const XNEntryComplex<T>& b) {
  return complex_lt(a.x,a.y,b.x,b.y);
}

template <class T>
static bool ComplexGreater(const XNEntryComplex<T>& a, const XNEntryComplex<T>& b) {
  return complex_gt(a.x,a.y,b.x,b.y);
}


struct OpVecSortAscend {
  template <typename T>
  static inline void func(ConstSparseIterator<T> &,
			  SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src,
			  BasicArray<T> &dest,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryReal<T> > tmp(int(src.length()));
    for (index_t i=1;i<=src.length();i++) {
      tmp[int(i-1)].n = i; tmp[int(i-1)].x = src[i];
    }
    qStableSort(tmp.begin(),tmp.end(),RealLess<T>);
    for (int i=0;i<tmp.size();i++) {
      dest[i+1] = tmp[i].x;
      dest_index[i+1] = tmp[i].n;
    }
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T> & dest_real,
			  BasicArray<T> & dest_imag,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryComplex<T> > tmp(int(src_real.length()));
    for (index_t i=1;i<=src_real.length();i++) {
      tmp[int(i-1)].n = i; 
      tmp[int(i-1)].x = src_real[i];
      tmp[int(i-1)].y = src_imag[i];
    }
    qStableSort(tmp.begin(),tmp.end(),ComplexLess<T>);
    for (int i=0;i<tmp.size();i++) {
      dest_real[index_t(i+1)] = tmp[i].x;
      dest_imag[index_t(i+1)] = tmp[i].y;
      dest_index[index_t(i+1)] = tmp[i].n;
    }
  }
};

struct OpVecSortDescend {
  template <typename T>
  static inline void func(ConstSparseIterator<T> &,
			  SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&,
			  SparseSlice<index_t>&) {
    throw Exception("sort does not work for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src,
			  BasicArray<T> &dest,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryReal<T> > tmp(int(src.length()));
    for (index_t i=1;i<=src.length();i++) {
      tmp[int(i-1)].n = i; tmp[int(i-1)].x = src[i];
    }
    qStableSort(tmp.begin(),tmp.end(),RealGreater<T>);
    for (int i=0;i<tmp.size();i++) {
      dest[i+1] = tmp[i].x;
      dest_index[i+1] = tmp[i].n;
    }
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T> & dest_real,
			  BasicArray<T> & dest_imag,
			  BasicArray<index_t>& dest_index) {
    QVector<XNEntryComplex<T> > tmp(int(src_real.length()));
    for (index_t i=1;i<=src_real.length();i++) {
      tmp[int(i-1)].n = i; 
      tmp[int(i-1)].x = src_real[i];
      tmp[int(i-1)].y = src_imag[i];
    }
    qStableSort(tmp.begin(),tmp.end(),ComplexGreater<T>);
    for (int i=0;i<tmp.size();i++) {
      dest_real[index_t(i+1)] = tmp[i].x;
      dest_imag[index_t(i+1)] = tmp[i].y;
      dest_index[index_t(i+1)] = tmp[i].n;
    }
  }
};

//@@Signature
//function sort SortFunction
//inputs A dim mode
//outputs B Index
//DOCBLOCK array_sort
ArrayVector SortFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("sort requires at least one argument");
  Array input(arg[0]);
  if (input.isScalar()) {
    ArrayVector ret;
    ret.push_back(input);
    ret.push_back(Array(double(1)));
    return ret;
  }
  int workDim = 0;
  if ((arg.size() >= 2) && (!arg[1].isEmpty()))
    workDim = arg[1].asInteger()-1;
  else
    workDim = input.dimensions().firstNonsingular();
  bool ascendSort = true;
  if (arg.size() >= 3) {
    QString tdir(arg[2].asString().toLower());
    if (tdir[0] == 'a') 
      ascendSort = true;
    else if (tdir[0] == 'd')
      ascendSort = false;
    else
      throw Exception("String argument must be either 'ascend' or 'descend'");
  }
  if (IsCellStringArray(input)) {
    Array indx;
    Array F;
    if (ascendSort)
      F = BiVectorOp<Array,OpVecSortAscend>(input.constReal<Array>(),
					    int(input.dimensions()[workDim]),
					    workDim,indx);
    else
      F = BiVectorOp<Array,OpVecSortDescend>(input.constReal<Array>(),
					     int(input.dimensions()[workDim]),
					     workDim,indx);
    ArrayVector ret;
    ret.push_back(F);
    ret.push_back(indx);
    return ret;
  } else {
    ArrayVector ret;
    if (ascendSort)
      ret = BiVectorOp<OpVecSortAscend>(input,
					int(input.dimensions()[workDim]),
					workDim);
    else
      ret =  BiVectorOp<OpVecSortDescend>(input,
					  int(input.dimensions()[workDim]),
					  workDim);
    if (input.dataClass() == StringArray) {
      ret[0] = ret[0].toClass(StringArray);
    }
    return ret;
  }
}
