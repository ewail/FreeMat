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
#include "Algorithms.hpp"
#include "IEEEFP.hpp"
  
template <class T>
static T NonIEEEMult(const T & A, const T & B) {
  if (A == 0) return 0;
  return A*B;
}


template <class T>
static void Conv2MainReal(T* C, const T* A, const T*B,
			  int Am, int An, int Bm, int Bn, 
			  int Cm, int Cn, int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      double accum = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++)
	  accum += NonIEEEMult(A[i+j*Am],B[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm]);
      C[m+n*Cm] = accum;
    }
}

template <class T>
static void Conv2MainComplex(T* Cr, T* Ci,
			     const T* Ar, const T* Ai,
			     const T* Br, const T* Bi,
			     int Am, int An, 
			     int Bm, int Bn, 
			     int Cm, int Cn, 
			     int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      double accum_r = 0;
      double accum_i = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++) {
	  T p_r, p_i;
	  complex_multiply(Ar[i+j*Am],
			   Ai[i+j*Am],
			   Br[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm],
			   Bi[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm],
			   p_r,
			   p_i);
	  accum_r += p_r;
	  accum_i += p_i;
	}
      Cr[m+n*Cm] = accum_r;
      Ci[m+n*Cm] = accum_i;
    }
}

template <typename T>
static Array Conv2FunctionDispatch(Array X, Array Y, int Cm, int Cn, 
				   int Cm_offset, int Cn_offset, DataClass dclass) {
  if (X.allReal() && Y.allReal()) {
    Array C(dclass,NTuple(Cm,Cn));
    Conv2MainReal<T>(C.real<T>().data(),X.constReal<T>().constData(),Y.constReal<T>().constData(),
		     int(X.rows()),int(X.cols()),int(Y.rows()),int(Y.cols()),Cm,Cn,Cm_offset,Cn_offset);
    return C;
  } else {
    Array C(dclass,NTuple(Cm,Cn));
    X.forceComplex();
    Y.forceComplex();
    Conv2MainComplex<T>(C.real<T>().data(),C.imag<T>().data(),
			X.constReal<T>().constData(),X.constImag<T>().constData(),
			Y.constReal<T>().constData(),Y.constImag<T>().constData(),
			int(X.rows()),int(X.cols()),int(Y.rows()),int(Y.cols()),Cm,Cn,Cm_offset,Cn_offset);
    return C;
  }
}

#define MacroConv(ctype,cls) \
  case cls: return Conv2FunctionDispatch<ctype>(X,Y,Cm,Cn,Cm_offset,Cn_offset,cls);

Array Conv2FunctionDispatch(Array X, Array Y, int Cm, int Cn,
			    int Cm_offset, int Cn_offset) {
  switch (X.dataClass()) {
  default: throw Exception("illegal argument type to conv2");
    MacroConv(float,Float);
    MacroConv(double,Double);
  }
}

#undef MacroConv

static Array Conv2FunctionFullXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows() + Y.rows() - 1);
  Cn = int(X.cols() + Y.cols() - 1);
  Cm_offset = 0;
  Cn_offset = 0;
  return Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset);
}

static Array Conv2FunctionSameXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows());
  Cn = int(X.cols());
  Cm_offset = (int) round((double)((Y.rows()-1)/2));
  Cn_offset = (int) round((double)((Y.cols()-1)/2));
  return Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset);
}

static Array Conv2FunctionValidXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows()-Y.rows()+1);
  Cn = int(X.cols()-Y.cols()+1);
  if ((Cm < 0) || (Cn < 0))
    return EmptyConstructor();
  if ((Cm == 0) || (Cn == 0))
    return Array(X.dataClass(),NTuple(Cm,Cn));
  Cm_offset = int(Y.rows()-1);
  Cn_offset = int(Y.cols()-1);
  return Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset);    
}

static Array Conv2FunctionXY(Array X, Array Y, QString type) {
  // Check the arguments
  if (X.isReferenceType() || Y.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!X.is2D() || !Y.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  if (type == "FULL")
    return Conv2FunctionFullXY(X,Y);
  if (type == "SAME")
    return Conv2FunctionSameXY(X,Y);
  if (type == "VALID")
    return Conv2FunctionValidXY(X,Y);
  throw Exception("could not recognize the arguments to conv2");
}

static Array Conv2FunctionRCX(Array hcol, Array hrow, Array X, QString type) {
  if (hcol.isReferenceType() || hrow.isReferenceType() ||
      X.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!hcol.is2D() || !hrow.is2D() || !X.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  hcol.reshape(NTuple(hcol.length(),1));
  hrow.reshape(NTuple(1,hrow.length()));
  Array rvec;
  rvec = Conv2FunctionXY(X,hcol,type);
  rvec = Conv2FunctionXY(rvec,hrow,type);
  return rvec;
}

//@@Signature
//function conv2 Conv2Function jitsafe
//inputs hcol hrow X shape
//outputs Z
//DOCBLOCK signal_conv2
ArrayVector Conv2Function(int nargout, const ArrayVector& arg) {
  // Call the right function based on the arguments
  if (arg.size() < 2) 
    throw Exception("conv2 requires at least 2 arguments");
  Array X(arg[0]);
  Array Y(arg[1]);
  if (X.isEmpty() || Y.isEmpty())
    return ArrayVector(EmptyConstructor());
  DataClass via, out;
  ComputeTypes(X,Y,via,out);
  X = X.asDenseArray().toClass(via);
  Y = Y.asDenseArray().toClass(via);
  Array Z;
  if (arg.size() == 2)
    Z = Conv2FunctionXY(X,Y,"FULL").toClass(out);
  else if ((arg.size() == 3) && (arg[2].isString()))
    Z = Conv2FunctionXY(X,Y,arg[2].asString().toUpper()).toClass(out);
  else if (arg.size() == 3)
    Z = Conv2FunctionRCX(X,Y,arg[2],"FULL").toClass(out);
  else if ((arg.size() == 4) && (arg[3].isString()))
    Z = Conv2FunctionRCX(X,Y,arg[2],
			    arg[3].asString().toUpper()).toClass(out);
  else
    throw Exception("could not recognize which form of conv2 was requested - check help conv2 for details");
  return ArrayVector(Z);
}
