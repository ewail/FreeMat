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
#include "SparseCCS.hpp"
#include "Algorithms.hpp"
#include "Math.hpp"

static inline index_t CountNonZeros(const BasicArray<bool> &dp) {
  index_t nonZero = 0;
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) nonZero++;
  return nonZero;
}

static ArrayVector SingleFindModeFull(const Array &arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D()))
    return ArrayVector(Array(Double));
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> sp(retdims);
  index_t j = 1;
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) 
      sp[j++] = i;
  return ArrayVector(Array(sp));
}
  
static ArrayVector RCFindModeFull(const Array &arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  return retval;
}

template <class T>
static ArrayVector RCVFindModeFullReal(Array arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  const BasicArray<T> &vp(arg.constReal<T>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    Array wp(GetDataClass(T(0)));
    if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
	&& (wp.dataClass() != Double))
      wp = wp.toClass(Double);
    retval.push_back(wp);
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  BasicArray<T> qp(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      qp[j] = vp[i];
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  Array zp(qp);
  if ((zp.dataClass() != Bool) && (zp.dataClass() != Float)
      && (zp.dataClass() != Double))
    zp = zp.toClass(Double);
  retval.push_back(zp);
  return retval;
}  

template <class T>
static ArrayVector RCVFindModeFullComplex(Array arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  const BasicArray<T> &vp(arg.constReal<T>());
  const BasicArray<T> &zp(arg.constImag<T>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    Array wp(GetDataClass(T(0)));
    if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
	&& (wp.dataClass() != Double))
      wp = wp.toClass(Double);
    retval.push_back(wp);
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  BasicArray<T> qp(retdims);
  BasicArray<T> ip(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      qp[j] = vp[i];
      ip[j] = zp[i];
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  Array wp(qp,ip);
  if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
      && (wp.dataClass() != Double))
    wp = wp.toClass(Double);
  retval.push_back(wp);
  return retval;
}

template <class T>
static ArrayVector RCVFindModeFull(Array x) {
  if (x.allReal())
    return RCVFindModeFullReal<T>(x);
  else
    return RCVFindModeFullComplex<T>(x);
}

#define MacroFindFull(ctype,cls) \
  case cls: return RCVFindModeFull<ctype>(x);
  
static ArrayVector RCVFindModeFull(Array x) {
  switch (x.dataClass()) {
  default: throw Exception("find not defined for reference types");
    MacroExpandCasesAll(MacroFindFull);
  }
}

static ArrayVector FindModeSparse(Array x, int nargout) {
  Array xrows, xcols, xdata;
  xdata = SparseToIJV(x,xrows,xcols);
  NTuple retDim;
  index_t nnz = xdata.length();
  if (x.isRowVector())
    retDim = NTuple(1,nnz);
  else
    retDim = NTuple(nnz,1);
  ArrayVector retval;
  if (nargout == 3) {
    retval.push_back(xrows);
    retval.push_back(xcols);
    retval.push_back(xdata);
  } else if (nargout == 2) {
    retval.push_back(xrows);
    retval.push_back(xcols);
  } else {
    retval.push_back(Add(Multiply(Subtract(xcols,Array(double(1))),
				  Array(double(x.rows()))),xrows));
  }
  return retval;
}
  
//@@Signature
//function find FindFunction
//inputs x keep flag
//outputs row col vals
//DOCBLOCK array_find

static ArrayVector FindTrim(ArrayVector a, int cnt, bool first_flag) {
  if (cnt < 0) return a;
  if (a.size() == 0) return a;
  int N = int(a[0].length());
  if (N == 0) return a;
  if (cnt > N) return a;
  ArrayVector ret;
  Array ndx;
  bool vertflag = !(a[0].isRowVector());
  if (first_flag)
    ndx = RangeConstructor(1,1,cnt,vertflag);
  else
    ndx = RangeConstructor((N-cnt)+1,1,N,vertflag);
  for (int i=0;i<a.size();i++) 
    ret.push_back(a[i].get(ndx));
  return ret;
}

ArrayVector FindFunction(int nargout, const ArrayVector& arg) {
  // Detect the Find mode...
  if (arg.size() < 1)
    throw Exception("find function takes at least one argument");
  Array tmp(arg[0]);
  if (tmp.isScalar()) tmp = tmp.asDenseArray();
  int k = -1;
  bool first_flag = true;
  if (arg.size() > 1)
    k  = arg[1].asInteger();
  if (arg.size() == 3) {
    QString flag = arg[2].asString().toLower();
    if (flag=="first")
      first_flag = true;
    else if (flag=="last")
      first_flag = false;
    else
      throw Exception("third option to find must be either 'first' or 'last'");
  }
  if ((nargout <= 1) && !tmp.isSparse())
    return FindTrim(SingleFindModeFull(tmp),k,first_flag);
  if ((nargout == 2) && !tmp.isSparse())
    return FindTrim(RCFindModeFull(tmp),k,first_flag);
  if ((nargout == 3) && !tmp.isSparse())
    return FindTrim(RCVFindModeFull(tmp),k,first_flag);
  if (nargout > 3)
    throw Exception("Do not understand syntax of find call (too many output arguments).");
  return FindTrim(FindModeSparse(tmp,nargout),k,first_flag);
}
