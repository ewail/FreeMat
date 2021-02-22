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

struct OpVecProd {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    T result = 1;
    index_t count = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      result = result * src.value();
      src.next();
    }
    if (count < src.rows())
      result = 0;
    dest[1] = result;
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    T result_real = 1;
    T result_imag = 0;
    index_t count = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      complex_multiply(result_real,result_imag,src.realValue(),src.imagValue(),
		       result_real,result_imag);
      src.next();
    }
    if (count < src.rows()) {
      result_real = 0;
      result_imag = 0;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    T result = 1;
    for (index_t i=1;i<=src.length();i++)
      result = result * src[i];
    dest[1] = result;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    T result_real = 1;
    T result_imag = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      T temp_real, temp_imag;
      complex_multiply(result_real,result_imag,
		       src_real[i],src_imag[i],
		       temp_real,temp_imag);
      result_real = temp_real;
      result_imag = temp_imag;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
  }
};

//@@Signature
//function prod ProdFunction jitsafe
//inputs x dimension
//outputs y
//DOCBLOCK elementary_prod
ArrayVector ProdFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("prod requires at least one argument");
  Array input(arg[0]);
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  if (input.isEmpty() && input.is2D()) return ArrayVector(Array(1.0));
  return ArrayVector(VectorOp<OpVecProd>(input,1,dim));
}
