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
#include "IEEEFP.hpp"
#include "Math.hpp"
#include "Complex.hpp"

// The LessThan function is called by Min
struct OpLessThan {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (IsNaN(v1)) return v2;
    if (IsNaN(v2)) return v1;
    return (v1 < v2) ? v1 : v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    if (IsNaN(ar) || IsNaN(ai)) {
      cr = br; ci = bi;
      return;
    }
    if (IsNaN(br) || IsNaN(bi)) {
      cr = ar; ci = ai;
      return;
    }
    if (complex_lt(ar,ai,br,bi)) {
      cr = ar; ci = ai;
    } else {
      cr = br; ci = bi;
    }
  }
};

struct OpVecMin {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest,
			  SparseSlice<index_t>& dest_index) {
    bool init = false;
    T result = 0;
    index_t count = 0;
    index_t zero_index = 0;
    index_t index = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      if (src.row() != count) zero_index = src.row();
      if (!IsNaN(src.value())) {
	if (!init) {
	  init = true;
	  result = src.value();
	  index = src.row();
	} else {
	  if (src.value() < result) {
	    index = src.row();
	    result = src.value();
	  }
	}
      }
      src.next();
    }
    if (count < src.rows()) {
      if (!init) {
	result = 0;
	index = zero_index;
      } else {
	if (result > 0) {
	  result = 0;
	  index = zero_index;
	}
      }
    }
    dest[1] = result;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag,
			  SparseSlice<index_t>& dest_index) {
    bool init = false;
    T result_real = 0;
    T result_imag = 0;
    index_t count = 0;
    index_t zero_index = 0;
    index_t index = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      if (src.row() != count) zero_index = src.row();
      if (!IsNaN(src.realValue()) && !IsNaN(src.imagValue())) {
	if (!init) {
	  init = true;
	  result_real = src.realValue();
	  result_imag = src.imagValue();
	  index = src.row();
	} else 
	  if (complex_lt(src.realValue(),src.imagValue(),
			 result_real,result_imag)) {
	    result_real = src.realValue();
	    result_imag = src.imagValue();
	    index = src.row();
	  }
      }
      src.next();
    }
    if (count < src.rows()) {
      result_real = 0;
      result_imag = 0;
      index = zero_index;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest,
			  BasicArray<index_t>& dest_index) {
    bool init = false;
    T result = 0;
    index_t index = 0;
    for (index_t i=1;i<=src.length();i++) {
      if (!IsNaN(src[i])) {
	if (!init) {
	  init = true;
	  result = src[i];
	  index = i;
	} else {
	  if (src[i] < result) {
	    index = i;
	    result = src[i];
	  }
	}
      }
    }
    if (!init) {
      result = NaN();
      index = 1;
    }
    dest[1] = result;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag,
			  BasicArray<index_t>& dest_index) {
    bool init = false;
    T result_real = 0;
    T result_imag = 0;
    index_t index = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      if (!IsNaN(src_real[i]) && !IsNaN(src_imag[i])) {
	if (!init) {
	  init = true;
	  result_real = src_real[i];
	  result_imag = src_imag[i];
	  index = i;
	} else {
	  if (complex_lt(src_real[i],src_imag[i],
			 result_real,result_imag)) {
	    result_real = src_real[i];
	    result_imag = src_imag[i];
	    index = i;
	  }
	}
      }
    }
    if (!init) {
      result_real = NaN();
      result_imag = NaN();
      index = 1;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
    dest_index[1] = index;
  }
};

//@@Signature
//function min MinFunction jitsafe
//input x z dim
//output y n
//DOCBLOCK elementary_min
ArrayVector MinFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1 || arg.size() > 3)
    throw Exception("min requires at least one argument, and at most three arguments");
  // Determine if this is a call to the Min function or the LessThan function
  // (the internal version of the two array min function)
  if (arg.size() == 2) {
    Array ret(DotOp<OpLessThan>(arg[0],arg[1]));
    if ((arg[0].dataClass() == Bool) && (arg[1].dataClass() == Bool))
      return ArrayVector(ret.toClass(Bool));
    return ArrayVector(ret);
  } 
  int dim;
  if (arg.size() > 2)
    dim = arg[2].asInteger()-1;
  else
    dim = arg[0].dimensions().firstNonsingular();
  if (arg[0].isEmpty()) {
    NTuple dims(arg[0].dimensions());
    if (dims == NTuple(0,0)) return ArrayVector(arg[0]);
    if (dims[dim] != 0)
      dims[dim] = 1;
    Array ret(arg[0].dataClass(),dims);
    ArrayVector p;
    p.push_back(ret); p.push_back(ret);
    return p;
  }
  return BiVectorOp<OpVecMin>(arg[0],1,dim);
}

JitScalarFunc2(min,OpLessThan::func);
