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
#include "Math.hpp"
#include "Complex.hpp"

//@@Signature
//function dot DotFunction jitsafe
//inputs x1 x2 dim
//outputs y
//DOCBLOCK elementary_dot


template <typename T>
static ArrayVector DotFunction(const BasicArray<T> &xreal,
			       const BasicArray<T> &ximag,
			       const BasicArray<T> &yreal,
			       const BasicArray<T> &yimag,
			       int DotDim) {
  BasicArray<T> rvec_real(xreal.dimensions().forceOne(DotDim));
  BasicArray<T> rvec_imag(ximag.dimensions().forceOne(DotDim));
  ConstBasicIterator<T> xsource_real(&xreal,DotDim);
  ConstBasicIterator<T> xsource_imag(&ximag,DotDim);
  ConstBasicIterator<T> ysource_real(&yreal,DotDim);
  ConstBasicIterator<T> ysource_imag(&yimag,DotDim);
  BasicIterator<T> dest_real(&rvec_real,DotDim);
  BasicIterator<T> dest_imag(&rvec_imag,DotDim);
  index_t dimLen = xreal.dimensions()[DotDim];
  while (xsource_real.isValid() && 
	 ysource_real.isValid() && 
	 xsource_imag.isValid() &&
	 ysource_imag.isValid() &&
	 dest_real.isValid() &&
	 dest_imag.isValid()) {
    double accum_real = 0;
    double accum_imag = 0;
    for (index_t i=1;i<=dimLen;i++) {
      T dot_real, dot_imag;
      complex_multiply<T>(xsource_real.get(),-xsource_imag.get(),
			  ysource_real.get(),ysource_imag.get(),
			  dot_real, dot_imag);
      accum_real += dot_real;
      accum_imag += dot_imag;
      xsource_real.next(); ysource_real.next();
      xsource_imag.next(); ysource_imag.next();
    }
    dest_real.set(accum_real);
    dest_imag.set(accum_imag);
    dest_real.next(); dest_imag.next();
    xsource_real.nextSlice(); ysource_real.nextSlice(); dest_real.nextSlice();
    xsource_imag.nextSlice(); ysource_imag.nextSlice(); dest_imag.nextSlice();
  }
  return Array(rvec_real,rvec_imag);
}

template <typename T>
static ArrayVector DotFunction(const BasicArray<T> &xreal,
			       const BasicArray<T> &yreal,
			       int DotDim) {
  if (xreal.isEmpty() && yreal.isEmpty() &&
      xreal.is2D() && yreal.is2D()) {
    if (DotDim == 0)
      return ArrayVector(Array(T(0)));
    else
      return ArrayVector(Array(GetDataClass(T(0)),NTuple(0,1)));
  }
  BasicArray<T> rvec(xreal.dimensions().forceOne(DotDim));
  ConstBasicIterator<T> xsource(&xreal,DotDim);
  ConstBasicIterator<T> ysource(&yreal,DotDim);
  BasicIterator<T> dest(&rvec,DotDim);
  index_t dimLen = xreal.dimensions()[DotDim];
  while (xsource.isValid() && ysource.isValid() && dest.isValid()) {
    double accum = 0;
    for (index_t i=1;i<=dimLen;i++) {
      accum += xsource.get()*ysource.get();
      xsource.next(); ysource.next();
    }
    dest.set(accum);
    dest.next();
    xsource.nextSlice(); ysource.nextSlice(); dest.nextSlice();
  }
  return Array(rvec);
}

ArrayVector SumFunction(int, const ArrayVector&);

template <typename T>
static ArrayVector DotFunction(const Array &x, const Array &y, int DotDim) {
  if (x.isSparse()) {
    Array z(DotMultiply(x,y));
    ArrayVector v;
    v.push_back(z);
    v.push_back(Array(DotDim+1));
    return SumFunction(1,v);
  } else {
    if (x.allReal()) 
      return DotFunction(x.constReal<T>(),y.constReal<T>(),DotDim);
    else 
      return DotFunction(x.constReal<T>(),x.constImag<T>(),
			 y.constReal<T>(),y.constImag<T>(),DotDim);
  }
}

ArrayVector DotFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("dot requires at least two arguments");
  Array x(arg[0]);
  Array y(arg[1]);
  if (x.isVector() && y.isVector() && arg.size() == 2) {
    x.reshape(NTuple(x.length(),1));
    y.reshape(NTuple(y.length(),1));
  }
  int DotDim;
  if (arg.size() > 2) {
    DotDim = arg[2].asInteger() - 1;
    if (DotDim < 0) throw Exception("dimension argument to dot should be positive");
  } else
    DotDim = x.dimensions().firstNonsingular();
  if (x.dimensions() != y.dimensions()) 
    throw Exception("arguments to dot should be the same size");
  if (x.isSparse() ^ y.isSparse()) {
    x = x.asDenseArray();
    y = y.asDenseArray();
  }
  if ((x.dataClass() == Float) || (y.dataClass() == Float)) {
    x = x.toClass(Float);
    y = y.toClass(Float);
  } else {
    x = x.toClass(Double);
    y = y.toClass(Double);
  }
  if (!x.allReal() || !y.allReal()) {
    x.forceComplex();
    y.forceComplex();
  }
  if (x.dataClass() == Float)
    return DotFunction<float>(x,y,DotDim);
  else
    return DotFunction<double>(x,y,DotDim);
}
