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

struct OpVecMean {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    T accum = 0;
    index_t col = src.col();
    while (src.col() == col) {
      accum += src.value();
      src.next();
    }
    dest[1] = accum/src.rows();
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    T accum_real = 0;
    T accum_imag = 0;
    index_t col = src.col();
    while (src.col() == col) {
      accum_real += src.realValue();
      accum_imag += src.imagValue();
      src.next();
    }
    dest_real[1] = accum_real/src.rows();
    dest_imag[1] = accum_imag/src.rows();
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    T accum = 0;
    for (index_t i=1;i<=src.length();i++)
      accum += src[i];
    dest[1] = accum/src.rows();
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    T accum_real = 0;
    T accum_imag = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      accum_real += src_real[i];
      accum_imag += src_imag[i];
    }
    dest_real[1] = accum_real/src_real.length();
    dest_imag[1] = accum_imag/src_imag.length();
  }
};

//@@Signature
//function mean MeanFunction jitsafe
//inputs x dim
//outputs y
//DOCBLOCK elementary_mean
ArrayVector MeanFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("mean requires at least one argument");
  Array input(arg[0]);
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  if (input.dimensions() == NTuple(0,0))
    return ArrayVector(Array(NaN()));
  if (input.isEmpty()) {
    NTuple dims(input.dimensions());
    if (dims == NTuple(0,0)) return ArrayVector(Array(NaN()));
    if (dims[dim] != 0) 
      dims[dim] = 1;
    return ArrayVector(Array(input.dataClass(),dims));
  }
  return ArrayVector(VectorOp<OpVecMean>(input,1,dim));
}
