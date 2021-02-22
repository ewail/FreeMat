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
#include "Operators.hpp"
#include "Math.hpp"
#include "Complex.hpp"

struct OpVecVar {
  template <typename T>
  static inline void func(const ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    throw Exception("Variance not implemented for sparse matrices");
  }
  template <typename T>
  static inline void func(const ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    throw Exception("Variance not implemented for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    if ((src.length() == 1) && (!IsFinite(src[1]))) {
      dest[1] = NaN();
      return;
    }
    T accum = 0;
    for (index_t i=1;i<=src.length();i++)
      accum += src[i];
    T mean = accum/src.length();
    accum = 0;
    if (src.length() > 1) {
      T normalizer = 1.0/(src.length()-1.0);
      for (index_t i=1;i<=src.length();i++) {
	T tmp = src[i] - mean;
	accum += tmp*tmp*normalizer;
      }
    }
    dest[1] = accum;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    if ((src_real.length() == 1) && 
	(!IsFinite(src_real[1]) || !IsFinite(src_imag[1]))) {
      dest_real[1] = NaN();
      dest_imag[1] = 0;
      return;
    }
    T accum_real = 0;
    T accum_imag = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      accum_real += src_real[i];
      accum_imag += src_imag[i];
    }
    T mean_real = accum_real/src_real.length();
    T mean_imag = accum_imag/src_imag.length();
    T accum = 0;
    if (src_real.length() > 1) {
      T normalizer = 1.0/(src_real.length()-1.0);
      for (index_t i=1;i<=src_real.length();i++) {
	T tmp_real = src_real[i] - mean_real;
	T tmp_imag = src_imag[i] - mean_imag;
	T tmp_val = complex_abs(tmp_real,tmp_imag);
	accum += tmp_val*tmp_val*normalizer;
      }
    }
    dest_real[1] = accum;
    dest_imag[1] = 0;
  }
};

//@@Signature
//function var VarFunction jitsafe
//inputs x dim
//outputs y
//DOCBLOCK elementary_var
ArrayVector VarFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("mean requires at least one argument");
  Array input(arg[0]);
  if (input.dimensions() == NTuple(0,0)) 
    return ArrayVector(Array(NaN()));
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  return ArrayVector(VectorOp<OpVecVar>(input,1,dim));
}
