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
#ifndef __Operators_hpp__
#define __Operators_hpp__

#include "Array.hpp"
#include "SparseMatrix.hpp"
#include "Algorithms.hpp"

template <class Op>
static inline bool IsZeroPreservingUnary()
{
  double pr, pi;
  Op::func(0.0,0.0,pr,pi);
  return ((Op::func(0.0) == 0) && (pr == 0) && (pi == 0));
}

template <class Op>
static inline bool IsZeroPreservingCmp()
{
  return ((Op::func(0.0,0.0) == false) && (Op::func(0,0,0,0) == false));
}

template <class Op>
static inline bool IsZeroPreserving()
{
  double pr, pi;
  Op::func(0.0,0.0,0.0,0.0,pr,pi);
  return ((Op::func(0.0,0.0) == 0) && (pr == 0) && (pi == 0));
}

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A, 
				    const T & B) {
  ConstSparseIterator<T> aspin(&A);
  SparseMatrix<S> retval(A.dimensions());
  // While more columns...
  while (aspin.isValid()) {
    retval.set(aspin.pos(),Op::func(aspin.value(),B));
    aspin.next();
  }
  return retval;
}

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const T& A, 
				    const SparseMatrix<T>& B) {
  ConstSparseIterator<T> bspin(&B);
  SparseMatrix<S> retval(B.dimensions());
  // While more columns...
  while (bspin.isValid()) {
    retval.set(bspin.pos(),Op::func(A,bspin.value()));
    bspin.next();
  }
  return retval;
}

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A, 
				    const SparseMatrix<T>& B) {
  ConstSparseIterator<T> aspin(&A);
  ConstSparseIterator<T> bspin(&B);
  SparseMatrix<S> retval(A.dimensions());
  // While more columns...
  while (aspin.isValid() || bspin.isValid()) {
    if (aspin.pos() == bspin.pos()) {
      retval.set(aspin.pos(),Op::func(aspin.value(),bspin.value()));
      aspin.next();
      bspin.next();
    } else if (A.dimensions().map(aspin.pos()) <
	       B.dimensions().map(bspin.pos())) {
      retval.set(aspin.pos(),Op::func(aspin.value(),T(0)));
      aspin.next();
    } else {
      retval.set(bspin.pos(),Op::func(T(0),bspin.value()));
      bspin.next();
    }
  }
  return retval;
}
	
// Complex, Complex --> Complex
template <typename S, typename T, class Op>
static inline void DotOp(const SparseMatrix<T>& A_real, 
			 const SparseMatrix<T>& A_imag,
			 const SparseMatrix<T>& B_real,
			 const SparseMatrix<T>& B_imag,
			 SparseMatrix<S>& C_real,
			 SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions());
  C_imag = SparseMatrix<S>(A_imag.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  ConstComplexSparseIterator<T> b_spin(&B_real, &B_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid() || b_spin.isValid()) {
    S value_c_real, value_c_imag;
    if (a_spin.pos() == b_spin.pos()) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),
	       b_spin.realValue(),b_spin.imagValue(),
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next(); b_spin.next();
    } else if (dim.map(a_spin.pos()) < dim.map(b_spin.pos())) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),T(0),T(0),
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next();
    } else {
      Op::func(T(0),T(0),b_spin.realValue(),b_spin.imagValue(),
	       value_c_real,value_c_imag);
      C_real.set(b_spin.pos(),value_c_real);
      C_imag.set(b_spin.pos(),value_c_imag);
      b_spin.next();
    }
  }
}

// Complex, Complex --> Complex
template <typename S, typename T, class Op>
static inline void DotOp(const SparseMatrix<T>& A_real, 
			 const SparseMatrix<T>& A_imag,
			 const T& B_real,
			 const T& B_imag,
			 SparseMatrix<S>& C_real,
			 SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions());
  C_imag = SparseMatrix<S>(A_imag.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid()) {
    S value_c_real, value_c_imag;
    Op::func(a_spin.realValue(),a_spin.imagValue(),
	     B_real,B_imag,value_c_real,value_c_imag);
    C_real.set(a_spin.pos(),value_c_real);
    C_imag.set(a_spin.pos(),value_c_imag);
    a_spin.next();
  } 
}

// Complex, Complex --> Complex
template <typename S, typename T, class Op>
static inline void DotOp(const T& A_real, 
			 const T& A_imag,
			 const SparseMatrix<T>& B_real,
			 const SparseMatrix<T>& B_imag,
			 SparseMatrix<S>& C_real,
			 SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(B_real.dimensions());
  C_imag = SparseMatrix<S>(B_imag.dimensions());
  ConstComplexSparseIterator<T> b_spin(&B_real, &B_imag);
  NTuple dim(B_real.dimensions());
  while (b_spin.isValid()) {
    S value_c_real, value_c_imag;
    Op::func(A_real,A_imag,
	     b_spin.realValue(),b_spin.imagValue(),
	     value_c_real,value_c_imag);
    C_real.set(b_spin.pos(),value_c_real);
    C_imag.set(b_spin.pos(),value_c_imag);
    b_spin.next();
  } 
}
	
// Complex, Complex --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A_real, 
				    const SparseMatrix<T>& A_imag,
				    const SparseMatrix<T>& B_real,
				    const SparseMatrix<T>& B_imag) {
  SparseMatrix<S> C(A_real.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  ConstComplexSparseIterator<T> b_spin(&B_real, &B_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid() || b_spin.isValid()) {
    if (a_spin.pos() == b_spin.pos()) {
      C.set(a_spin.pos(),
	    Op::func(a_spin.realValue(),a_spin.imagValue(),
		     b_spin.realValue(),b_spin.imagValue()));
      a_spin.next(); b_spin.next();
    } else if (dim.map(a_spin.pos()) < dim.map(b_spin.pos())) {
      C.set(a_spin.pos(),Op::func(a_spin.realValue(),a_spin.imagValue(),T(0),T(0)));
      a_spin.next();
    } else {
      C.set(b_spin.pos(),Op::func(T(0),T(0),b_spin.realValue(),b_spin.imagValue()));
      b_spin.next();
    }
  }
  return C;
}

// Perform the operation via a typed intermediary
template <typename T, class Op>
Array DotOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Tclass,NTuple(0,0));
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<T,T,Op>(Acast.constRealSparse<T>(),
			Bcast.constRealSparse<T>());
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      SparseMatrix<T> Freal(Acast.dimensions());
      SparseMatrix<T> Fimag(Acast.dimensions());;
      DotOp<T,T,Op>(Acast.constRealSparse<T>(),
		    Acast.constImagSparse<T>(),
		    Bcast.constRealSparse<T>(),
		    Bcast.constImagSparse<T>(),
		    Freal, Fimag);
      F = Array(Freal,Fimag);
    }
    return F;
  }
  if (Acast.isSparse() && Bcast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) 
      return DotOp<T,T,Op>(Acast.constRealSparse<T>(),Bcast.constRealScalar<T>());
    else {
      Acast.forceComplex(); Bcast.forceComplex();
      SparseMatrix<T> Freal(Acast.dimensions());
      SparseMatrix<T> Fimag(Acast.dimensions());
      DotOp<T,T,Op>(Acast.constRealSparse<T>(),
		    Acast.constImagSparse<T>(),
		    Bcast.constRealScalar<T>(),
		    Bcast.constImagScalar<T>(), Freal, Fimag);
      return Array(Freal,Fimag);
    }
  }
  if (Acast.isScalar() && Bcast.isSparse()) {
    if (Acast.allReal() && Bcast.allReal()) 
      return DotOp<T,T,Op>(Acast.constRealScalar<T>(),Bcast.constRealSparse<T>());
    else {
      Acast.forceComplex(); Bcast.forceComplex();
      SparseMatrix<T> Freal(Bcast.dimensions());
      SparseMatrix<T> Fimag(Bcast.dimensions());
      DotOp<T,T,Op>(Acast.constRealScalar<T>(),
		    Acast.constImagScalar<T>(),
		    Bcast.constRealSparse<T>(),
		    Bcast.constImagSparse<T>(), Freal, Fimag);
      return Array(Freal,Fimag);
    }
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (!Bcast.isScalar()) Bcast = Bcast.asDenseArray();
  if (Acast.isScalar() && Bcast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::create(Op::func(Acast.constRealScalar<T>(),
				Bcast.constRealScalar<T>()));
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(T(0),T(0));
      Op::func(Acast.constRealScalar<T>(),
	       Acast.constImagScalar<T>(),
	       Bcast.constRealScalar<T>(),
	       Bcast.constImagScalar<T>(),
	       F.realScalar<T>(),F.imagScalar<T>());
    }
  } else if (Acast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::create(Tclass,Bcast.dimensions());
      T* ret = F.real<T>().data();
      const T& Ap = Acast.constRealScalar<T>();
      const T* Bp = Bcast.constReal<T>().constData();
      uint64 q = uint64(Bcast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap,Bp[i]);
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Tclass,Bcast.dimensions());
      T* Cr = F.real<T>().data();
      T* Ci = F.imag<T>().data();
      const T& Ar = Acast.constRealScalar<T>();
      const T& Ai = Acast.constImagScalar<T>();
      const T* Br = Bcast.constReal<T>().constData();
      const T* Bi = Bcast.constImag<T>().constData();
      uint64 q = uint64(Bcast.length());
      for (uint64 i=0;i<q;i++) Op::func(Ar,Ai,Br[i],Bi[i],Cr[i],Ci[i]);
    }
  } else if (Bcast.isScalar()) {
    if (Bcast.allReal() && Acast.allReal()) {
      F = Array::create(Tclass,Acast.dimensions());
      T* ret = F.real<T>().data();
      const T* Ap = Acast.constReal<T>().constData();
      const T& Bp = Bcast.constRealScalar<T>();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp);
    } else { 
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Tclass,Acast.dimensions());
      T* Cr = F.real<T>().data();
      T* Ci = F.imag<T>().data();
      const T* Ar = Acast.constReal<T>().constData();
      const T* Ai = Acast.constImag<T>().constData();
      const T& Br = Bcast.constRealScalar<T>();
      const T& Bi = Bcast.constImagScalar<T>();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) Op::func(Ar[i],Ai[i],Br,Bi,Cr[i],Ci[i]);
    }
  } else {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = Array::create(Tclass,Acast.dimensions());
      T* ret = F.real<T>().data();
      const T* Ap = Acast.constReal<T>().constData();
      const T* Bp = Bcast.constReal<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp[i]);
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Tclass,Acast.dimensions());
      T* Cr = F.real<T>().data();
      T* Ci = F.imag<T>().data();
      const T* Ar = Acast.constReal<T>().constData();
      const T* Ai = Acast.constImag<T>().constData();
      const T* Br = Bcast.constReal<T>().constData();
      const T* Bi = Bcast.constImag<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) Op::func(Ar[i],Ai[i],Br[i],Bi[i],Cr[i],Ci[i]);
    }
  }
  return F;
}


// Assumes that the operation cannot create complex values from real ones
//
// So the table looks like this (order is unimportant)
// a  b  c v
// s  s  s s
// s  d  s s
// s  c  s s
// s  l  s s
// s  i  e 
// d  d  d d
// d  c  d d
// d  l  d d
// d  i  i d
// i1 i2 e 
// c  i  e
// c  l  d d
// c  c  d d
// l  l  d d
//
// The resulting rule:
//   default: c is double, v is double
//   if (a or b is single), c is single, v is single
//   if (a or b is an integer)
//     if (same type, or one is a double), 
//       c is integer
//     else illegal.
//
// if (a or b single) --> v single, c single
// else {
//   v double
//   if (a and b not integer) --> c double, else c integer
// }
//
// Arithmetic Operations on Floating-Point Numbers
// This section describes which classes you can use in arithmetic operations with floating-point numbers.
// Double-Precision.   You can perform basic arithmetic operations with double and any of the following other classes. When one or more operands is an integer (scalar or array), the double operand must be a scalar. The result is of type double, except where noted otherwise:
//       single  The result is of type single
//       double
//       int* or uint*  The result has the same data type as the integer operand
//       char
//       logical
// This example performs arithmetic on data of types char and double. The result is of type double:
// c = 'uppercase' - 32;
// class(c)
// ans =
//    double
// char(c)
// ans =
//    UPPERCASE
// Single-Precision.   You can perform basic arithmetic operations with single and any of the following other classes. The result is always single:
//       single
//       double
//       char
//       logical
// Arithmetic Operations on Integer Classes
// MATLAB can perform integer arithmetic on the following types of data:
//       Integers or integer arrays of the same integer data type. This yields a result that has the same data type as the operands:
//          x = uint32([132 347 528]) .* uint32(75);
//       Integers or integer arrays and scalar double-precision floating-point numbers. This yields a result that has the same data type as the integer operands:
//          x = uint32([132 347 528]) .* 75.49;
// For all binary operations in which one operand is an array of integer data type and the other is a scalar double, MATLAB computes the operation using elementwise double-precision arithmetic, and then converts the result back to the original integer data type.

static inline bool IsIntegerDataClass(const Array &Ain) {
  return ((Ain.dataClass() >= Int8) && (Ain.dataClass() <= UInt64));
}

template <class Op>
Array DotOp(const Array &Ain, const Array &Bin) {
  if ((Ain.isSparse() || Bin.isSparse()) && !IsZeroPreserving<Op>())
    return ToSparse(DotOp<Op>(Ain.asDenseArray(),Bin.asDenseArray()));
  DataClass via_type;
  DataClass out_type;
  ComputeTypes(Ain,Bin,via_type,out_type);
  if ((out_type == Float) && (IsIntegerDataClass(Ain) || 
			      IsIntegerDataClass(Bin)))
      throw Exception("Cannot combine single precision and integer class data");
  if (IsIntegerDataClass(Ain) && IsIntegerDataClass(Bin) &&
      Ain.dataClass() != Bin.dataClass()) 
    throw Exception("Cannot combine data of different integer data classes");
  if (IsIntegerDataClass(Ain)) out_type = Ain.dataClass();
  if (IsIntegerDataClass(Bin)) out_type = Bin.dataClass();
  Array F;
  if (via_type == Float)
    F = DotOp<float,Op>(Ain,Bin,Float);
  else
    F = DotOp<double,Op>(Ain,Bin,Double);
  return F.toClass(out_type);
}

template <class Op>
Array BoolOp(const Array &Ain, const Array &Bin) {
  return DotOp<bool,Op>(Ain,Bin,Bool).toClass(Bool);
}

// Comparison op -- uses real part only (used by ordinal operators)
template <typename T, class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Bool,NTuple(0,0));
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			 Bcast.constRealSparse<T>());
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (!Bcast.isScalar()) Bcast = Bcast.asDenseArray();
  if (Acast.isScalar() && Bcast.isScalar()) {
    F = Array::create(Op::func(Acast.constRealScalar<T>(),
			      Bcast.constRealScalar<T>()));
  } else if (Acast.isScalar()) {
    F = Array::create(Bool,Bcast.dimensions());
    bool* ret = F.real<bool>().data();
    const T& Ap = Acast.constRealScalar<T>();
    const T* Bp = Bcast.constReal<T>().constData();
    uint64 q = uint64(Bcast.length());
    for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap,Bp[i]);
  } else if (Bcast.isScalar()) {
    F = Array::create(Bool,Acast.dimensions());
    bool* ret = F.real<bool>().data();
    const T* Ap = Acast.constReal<T>().constData();
    const T& Bp = Bcast.constRealScalar<T>();
    uint64 q = uint64(Acast.length());
    for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp);
  } else {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    F = Array::create(Bool,Acast.dimensions());
    bool* ret = F.real<bool>().data();
    const T* Ap = Acast.constReal<T>().constData();
    const T* Bp = Bcast.constReal<T>().constData();
    uint64 q = uint64(Acast.length());
    for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp[i]);
  }
  return F;
}

template <class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin) {
  DataClass via_type;
  DataClass out_type;
  ComputeTypes(Ain,Bin,via_type,out_type);
  Array F;
  // Check for non zero-preserving cmp functions
  if ((Ain.isSparse() || Bin.isSparse()) && !IsZeroPreservingCmp<Op>())
    return ToSparse(CmpOp<Op>(Ain.asDenseArray(),Bin.asDenseArray()));
  if (via_type == Float) 
    F = CmpOp<float,Op>(Ain,Bin,Float);
  else
    F = CmpOp<double,Op>(Ain,Bin,Double);
  return F.toClass(Bool);
}

// Comparison operator -- uses real and imaginary parts
template <typename T, class Op>
static inline Array EqOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Bool,NTuple(0,0));
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			   Bcast.constRealSparse<T>());
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			   Acast.constImagSparse<T>(),
			   Bcast.constRealSparse<T>(),
			   Bcast.constImagSparse<T>());
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (!Bcast.isScalar()) Bcast = Bcast.asDenseArray();
  if (Acast.isScalar() && Bcast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::create(Op::func(Acast.constRealScalar<T>(),
				Bcast.constRealScalar<T>()));
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Op::func(Acast.constRealScalar<T>(),
				Acast.constImagScalar<T>(),
				Bcast.constRealScalar<T>(),
				Bcast.constImagScalar<T>()));
    }
  } else if (Acast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::create(Bool,Bcast.dimensions());
      bool* ret = F.real<bool>().data();
      const T& Ap = Acast.constRealScalar<T>();
      const T* Bp = Bcast.constReal<T>().constData();
      uint64 q = uint64(Bcast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap,Bp[i]);
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Bool,Bcast.dimensions());
      bool* ret = F.real<bool>().data();
      const T& Ar = Acast.constRealScalar<T>();
      const T& Ai = Acast.constImagScalar<T>();
      const T* Br = Bcast.constReal<T>().constData();
      const T* Bi = Bcast.constImag<T>().constData();
      uint64 q = uint64(Bcast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ar,Ai,Br[i],Bi[i]);
    }
  } else if (Bcast.isScalar()) {
    if (Bcast.allReal() && Acast.allReal()) {
      F = Array::create(Bool,Acast.dimensions());
      bool* ret = F.real<bool>().data();
      const T* Ap = Acast.constReal<T>().constData();
      const T& Bp = Bcast.constRealScalar<T>();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp);
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Bool,Acast.dimensions());
      bool* ret = F.real<bool>().data();
      const T* Ar = Acast.constReal<T>().constData();
      const T* Ai = Acast.constImag<T>().constData();
      const T& Br = Bcast.constRealScalar<T>();
      const T& Bi = Bcast.constImagScalar<T>();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ar[i],Ai[i],Br,Bi);
    }
  } else {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = Array::create(Bool,Acast.dimensions());
      bool* ret = F.real<bool>().data();
      const T* Ap = Acast.constReal<T>().constData();
      const T* Bp = Bcast.constReal<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i],Bp[i]);
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = Array::create(Bool,Acast.dimensions());
      bool* ret = F.real<bool>().data();
      const T* Ar = Acast.constReal<T>().constData();
      const T* Ai = Acast.constImag<T>().constData();
      const T* Br = Bcast.constReal<T>().constData();
      const T* Bi = Bcast.constImag<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ar[i],Ai[i],Br[i],Bi[i]);
    }
  }
  return F;
}

template <class Op>
static inline Array EqOp(const Array &Ain, const Array &Bin) {
  if ((Ain.isSparse() || Bin.isSparse()) && !IsZeroPreservingCmp<Op>())
    return (ToSparse(EqOp<Op>(Ain.asDenseArray(),Bin.asDenseArray())));
  DataClass via_type;
  DataClass out_type;
  ComputeTypes(Ain,Bin,via_type,out_type);
  Array F;
  if (via_type == Float) 
    F = EqOp<float,Op>(Ain,Bin,Float);
  else
    F = EqOp<double,Op>(Ain,Bin,Double);
  return F.toClass(Bool);
}

// Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> UnaryOp(const SparseMatrix<T>& A) {
  ConstSparseIterator<T> aspin(&A);
  SparseMatrix<S> retval(A.dimensions());
  while (aspin.isValid()) {
    retval.set(aspin.pos(),Op::func(aspin.value()));
    aspin.next();
  }
  return retval;
}
	
// Complex --> Complex
template <typename S, typename T, class Op>
static inline void UnaryOp(const SparseMatrix<T>& A_real, 
			   const SparseMatrix<T>& A_imag,
			   SparseMatrix<S>& C_real,
			   SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions());
  C_imag = SparseMatrix<S>(A_imag.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid()) {
    S value_c_real, value_c_imag;
    Op::func(a_spin.realValue(),a_spin.imagValue(),
	     value_c_real,value_c_imag);
    C_real.set(a_spin.pos(),value_c_real);
    C_imag.set(a_spin.pos(),value_c_imag);
    a_spin.next();
  }
}

// Perform the operation via a typed intermediary
template <typename T, class Op>
static inline Array UnaryOp(const Array &Ain, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F(Tclass,NTuple(0,0));
  if (Acast.isSparse()) {
    if (Acast.allReal()) {
      F = UnaryOp<T,T,Op>(Acast.constRealSparse<T>());
    } else {
      SparseMatrix<T> Freal(Acast.dimensions());
      SparseMatrix<T> Fimag(Acast.dimensions());
      UnaryOp<T,T,Op>(Acast.constRealSparse<T>(),
		      Acast.constImagSparse<T>(),
		      Freal,Fimag);
      F = Array(Freal,Fimag);
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.isScalar()) {
    if (Acast.allReal()) {
      F = Array::create(Op::func(Acast.constRealScalar<T>()));
    } else {
      F = Array::create(T(0),T(0));
      Op::func(Acast.constRealScalar<T>(),
	       Acast.constImagScalar<T>(),
	       F.realScalar<T>(),F.imagScalar<T>());
    }
  } else {
    if (Acast.allReal()) {
      F = Array(Tclass,Acast.dimensions());
      T* ret = F.real<T>().data();
      const T* Ap = Acast.constReal<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) ret[i] = Op::func(Ap[i]);
    } else {
      F = Array(Tclass,Acast.dimensions());
      T* Cr = F.real<T>().data();
      T* Ci = F.imag<T>().data();
      const T* Ar = Acast.constReal<T>().constData();
      const T* Ai = Acast.constImag<T>().constData();
      uint64 q = uint64(Acast.length());
      for (uint64 i=0;i<q;i++) Op::func(Ar[i],Ai[i],Cr[i],Ci[i]);
    }
  }
  return F;
}

template <class Op>
static inline Array UnaryOp(const Array &Ain) {
  // Check for non zero-preserving unary functions
  if (Ain.isSparse() && !IsZeroPreservingUnary<Op>())
    return ToSparse(UnaryOp<Op>(Ain.asDenseArray()));
  if (Ain.dataClass() == Float)
    return UnaryOp<float,Op>(Ain,Float);
  else
    return UnaryOp<double,Op>(Ain,Double);
}

template <typename T, class Op>
static inline Array VectorOp(const BasicArray<T> &real,
			     const BasicArray<T> &imag, 
			     index_t out, int dim) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F_real(outdims); 
  BasicArray<T> F_imag(outdims);
  ConstBasicIterator<T> source_real(&real,dim);
  ConstBasicIterator<T> source_imag(&imag,dim);
  BasicIterator<T> dest_real(&F_real,dim);
  BasicIterator<T> dest_imag(&F_imag,dim);
  BasicArray<T> in_buffer_real(NTuple(real.dimensions()[dim],1));
  BasicArray<T> in_buffer_imag(NTuple(imag.dimensions()[dim],1));
  BasicArray<T> out_buffer_real(NTuple(out,1));
  BasicArray<T> out_buffer_imag(NTuple(out,1));
  while (source_real.isValid() && dest_real.isValid()) {
    for (index_t i=1;i<=source_real.size();i++) {
      in_buffer_real[i] = source_real.get();
      in_buffer_imag[i] = source_imag.get();
      source_real.next(); source_imag.next();
    }
    Op::func(in_buffer_real,in_buffer_imag,
	     out_buffer_real,out_buffer_imag);
    for (index_t i=1;i<=out;i++) {
      dest_real.set(out_buffer_real[i]);
      dest_imag.set(out_buffer_imag[i]);
      dest_real.next(); dest_imag.next();
    }
    source_real.nextSlice(); source_imag.nextSlice();
    dest_real.nextSlice(); dest_imag.nextSlice();
  }
  return Array(F_real,F_imag);
}

template <typename T, class Op>
static inline Array VectorOp(const SparseMatrix<T>& real, index_t out, int dim) {
  if (dim == 0) {
    ConstSparseIterator<T> spin_real(&real);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval(outdims);
    while (spin_real.isValid()) {
      SparseSlice<T> this_col;
      index_t col_number = spin_real.col();
      Op::func(spin_real,this_col);
      retval.data()[col_number] = this_col;
    }
    return Array(retval);
  } else
    return Transpose(VectorOp<T,Op>(Transpose(real),out,0));
}

template <typename T, class Op>
static inline Array VectorOp(const SparseMatrix<T> &real,
			     const SparseMatrix<T> &imag, 
			     index_t out, int dim) {
  if (dim == 0) {
    ConstComplexSparseIterator<T> spin_complex(&real,&imag);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval_real(outdims);
    SparseMatrix<T> retval_imag(outdims);
    while (spin_complex.isValid()) {
      SparseSlice<T> this_real_col;
      SparseSlice<T> this_imag_col;
      index_t col_number = spin_complex.col();
      Op::func(spin_complex,this_real_col,this_imag_col);
      retval_real.data()[col_number] = this_real_col;
      retval_imag.data()[col_number] = this_imag_col;
    }
    return Array(retval_real,retval_imag);
  } else
    return Transpose(VectorOp<T,Op>(Transpose(real),
				    Transpose(imag),out,0));
}

template <typename T, class Op>
static inline Array VectorOp(const BasicArray<T> &real, index_t out, int dim) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  ConstBasicIterator<T> source(&real,dim);
  BasicIterator<T> dest(&F,dim);
  BasicArray<T> in_buffer(NTuple(real.dimensions()[dim],1));
  BasicArray<T> out_buffer(NTuple(out,1));
  while (source.isValid() && dest.isValid()) {
    for (index_t i=1;i<=source.size();i++) {
      in_buffer[i] = source.get();
      source.next();
    }
    Op::func(in_buffer,out_buffer);
    for (index_t i=1;i<=out;i++) {
      dest.set(out_buffer[i]);
      dest.next();
    }
    source.nextSlice(); dest.nextSlice();
  }
  return Array(F);
}

template <typename T, class Op>
static inline Array VectorOp(const Array &Ain, index_t out, int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  if (Acast.isSparse()) {
    if (Acast.allReal())
      return VectorOp<T,Op>(Acast.constRealSparse<T>(),out,dim);
    else
      return VectorOp<T,Op>(Acast.constRealSparse<T>(),
			    Acast.constImagSparse<T>(),out,dim);
  } else {
    if (Acast.isScalar()) Acast = Acast.asDenseArray();
    if (Acast.allReal()) {
      return VectorOp<T,Op>(Acast.constReal<T>(),out,dim);
    } else {
      return VectorOp<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim);
    }
  }
}

template <class Op>
  static inline Array VectorOp(const Array &Ain, int out, int dim) {
  out = qMax(0,out);
  if (Ain.dataClass() == Float)
    return VectorOp<float,Op>(Ain,out,dim,Float);
  else
    return VectorOp<double,Op>(Ain,out,dim,Double);
}


template <typename T, class Op>
static inline Array VectorOpDynamic(const BasicArray<T> &real,
				    const BasicArray<T> &imag, 
				    index_t out, int dim, Op &op) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F_real(outdims); 
  BasicArray<T> F_imag(outdims);
  ConstBasicIterator<T> source_real(&real,dim);
  ConstBasicIterator<T> source_imag(&imag,dim);
  BasicIterator<T> dest_real(&F_real,dim);
  BasicIterator<T> dest_imag(&F_imag,dim);
  BasicArray<T> in_buffer_real(NTuple(real.dimensions()[dim],1));
  BasicArray<T> in_buffer_imag(NTuple(imag.dimensions()[dim],1));
  BasicArray<T> out_buffer_real(NTuple(out,1));
  BasicArray<T> out_buffer_imag(NTuple(out,1));
  while (source_real.isValid() && dest_real.isValid()) {
    for (index_t i=1;i<=source_real.size();i++) {
      in_buffer_real[i] = source_real.get();
      in_buffer_imag[i] = source_imag.get();
      source_real.next(); source_imag.next();
    }
    op.func(in_buffer_real,in_buffer_imag,
	    out_buffer_real,out_buffer_imag);
    for (index_t i=1;i<=out;i++) {
      dest_real.set(out_buffer_real[i]);
      dest_imag.set(out_buffer_imag[i]);
      dest_real.next(); dest_imag.next();
    }
    source_real.nextSlice(); source_imag.nextSlice();
    dest_real.nextSlice(); dest_imag.nextSlice();
  }
  return Array(F_real,F_imag);
}

template <typename T, class Op>
static inline Array VectorOpDynamic(const SparseMatrix<T>& real, index_t out, 
				    int dim, Op &op) {
  if (dim == 0) {
    ConstSparseIterator<T> spin_real(&real);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval(outdims);
    while (spin_real.isValid()) {
      SparseSlice<T> this_col;
      index_t col_number = spin_real.col();
      op.func(spin_real,this_col);
      retval.data()[col_number] = this_col;
    }
    return Array(retval);
  } else
    return Transpose(VectorOpDynamic<T,Op>(Transpose(real),out,0,op));
}

template <typename T, class Op>
static inline Array VectorOpDynamic(const SparseMatrix<T> &real,
				    const SparseMatrix<T> &imag, 
				    index_t out, int dim,
				    Op &op) {
  if (dim == 0) {
    ConstComplexSparseIterator<T> spin_complex(&real,&imag);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval_real(outdims);
    SparseMatrix<T> retval_imag(outdims);
    while (spin_complex.isValid()) {
      SparseSlice<T> this_real_col;
      SparseSlice<T> this_imag_col;
      index_t col_number = spin_complex.col();
      op.func(spin_complex,this_real_col,this_imag_col);
      retval_real.data()[col_number] = this_real_col;
      retval_imag.data()[col_number] = this_imag_col;
    }
    return Array(retval_real,retval_imag);
  } else
    return Transpose(VectorOpDynamic<T,Op>(Transpose(real),
					   Transpose(imag),out,0,op));
}

template <typename T, class Op>
static inline Array VectorOpDynamic(const BasicArray<T> &real, 
				    index_t out, int dim,
				    Op &op) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  ConstBasicIterator<T> source(&real,dim);
  BasicIterator<T> dest(&F,dim);
  BasicArray<T> in_buffer(NTuple(real.dimensions()[dim],1));
  BasicArray<T> out_buffer(NTuple(out,1));
  while (source.isValid() && dest.isValid()) {
    for (index_t i=1;i<=source.size();i++) {
      in_buffer[i] = source.get();
      source.next();
    }
    op.func(in_buffer,out_buffer);
    for (index_t i=1;i<=out;i++) {
      dest.set(out_buffer[i]);
      dest.next();
    }
    source.nextSlice(); dest.nextSlice();
  }
  return Array(F);
}

template <typename T, class Op>
static inline Array VectorOpDynamic(const Array &Ain, index_t out, int dim, 
				    DataClass Tclass, Op &op) {
  Array Acast(Ain.toClass(Tclass));
  if (Acast.isSparse()) {
    if (Acast.allReal())
      return VectorOpDynamic<T,Op>(Acast.constRealSparse<T>(),out,dim,op);
    else
      return VectorOpDynamic<T,Op>(Acast.constRealSparse<T>(),
				   Acast.constImagSparse<T>(),out,dim,op);
  } else {
    if (Acast.isScalar()) Acast = Acast.asDenseArray();
    if (Acast.allReal()) {
      return VectorOpDynamic<T,Op>(Acast.constReal<T>(),out,dim,op);
    } else {
      return VectorOpDynamic<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim,op);
    }
  }
}

template <class Op>
static inline Array VectorOpDynamic(const Array &Ain, int out, int dim,
				    Op &op) {
  out = qMax(0,out);
  if (Ain.dataClass() == Float)
    return VectorOpDynamic<float,Op>(Ain,out,dim,Float,op);
  else
    return VectorOpDynamic<double,Op>(Ain,out,dim,Double,op);
}

template <typename T, class Op>
static inline Array BiVectorOp(const BasicArray<T> &real,
			       const BasicArray<T> &imag, 
			       index_t out, int dim,
			       Array &D) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F_real(outdims); 
  BasicArray<T> F_imag(outdims);
  BasicArray<index_t> Ddata(outdims);
  ConstBasicIterator<T> source_real(&real,dim);
  ConstBasicIterator<T> source_imag(&imag,dim);
  BasicIterator<T> dest_real(&F_real,dim);
  BasicIterator<T> dest_imag(&F_imag,dim);
  BasicArray<T> in_buffer_real(NTuple(real.dimensions()[dim],1));
  BasicArray<T> in_buffer_imag(NTuple(imag.dimensions()[dim],1));
  BasicArray<T> out_buffer_real(NTuple(out,1));
  BasicArray<T> out_buffer_imag(NTuple(out,1));
  BasicArray<index_t> out_buffer_index(NTuple(out,1));
  BasicIterator<index_t> D_iter(&Ddata,dim);
  while (source_real.isValid() && dest_real.isValid()) {
    for (index_t i=1;i<=source_real.size();i++) {
      in_buffer_real[i] = source_real.get();
      in_buffer_imag[i] = source_imag.get();
      source_real.next(); source_imag.next();
    }
    Op::func(in_buffer_real,in_buffer_imag,
	     out_buffer_real,out_buffer_imag,
	     out_buffer_index);
    for (index_t i=1;i<=out;i++) {
      dest_real.set(out_buffer_real[i]);
      dest_imag.set(out_buffer_imag[i]);
      D_iter.set(out_buffer_index[i]);
      dest_real.next(); dest_imag.next();
      D_iter.next();
    }
    source_real.nextSlice(); source_imag.nextSlice();
    dest_real.nextSlice(); dest_imag.nextSlice();
    D_iter.nextSlice();
  }
  D = Array(Ddata);
  return Array(F_real,F_imag);
}

template <typename T, class Op>
static inline Array BiVectorOp(const SparseMatrix<T>& real, index_t out, int dim,
			       Array &D) {
  if (dim == 0) {
    ConstSparseIterator<T> spin_real(&real);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval(outdims);
    SparseMatrix<index_t> Dval(outdims);
    while (spin_real.isValid()) {
      SparseSlice<T> this_col;
      SparseSlice<index_t> this_index;
      index_t col_number = spin_real.col();
      Op::func(spin_real,this_col,this_index);
      retval.data()[col_number] = this_col;
      Dval.data()[col_number] = this_index;
    }
    D = Array(Dval);
    return Array(retval);
  } else {
    Array F = Transpose(BiVectorOp<T,Op>(Transpose(real),out,0,D));
    D = Transpose(D);
    return F;
  }
}

template <typename T, class Op>
static inline Array BiVectorOp(const SparseMatrix<T> &real,
			       const SparseMatrix<T> &imag, index_t out, int dim,
			       Array &D) {
  if (dim == 0) {
    ConstComplexSparseIterator<T> spin_complex(&real,&imag);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval_real(outdims);
    SparseMatrix<T> retval_imag(outdims);
    SparseMatrix<index_t> Dval(outdims);
    while (spin_complex.isValid()) {
      SparseSlice<T> this_real_col;
      SparseSlice<T> this_imag_col;
      SparseSlice<index_t> this_index;
      index_t col_number = spin_complex.col();
      Op::func(spin_complex,this_real_col,this_imag_col,this_index);
      retval_real.data()[col_number] = this_real_col;
      retval_imag.data()[col_number] = this_imag_col;
      Dval.data()[col_number] = this_index;
    }
    D = Array(Dval);
    return Array(retval_real,retval_imag);
  } else {
    Array F = Transpose(BiVectorOp<T,Op>(Transpose(real),
					 Transpose(imag),out,0,D));
    D = Transpose(D);
    return F;
  }
}

template <typename T, class Op>
static inline Array BiVectorOp(const BasicArray<T> &real, index_t out, int dim,
			       Array &D) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  BasicArray<index_t> Ddata(outdims);
  ConstBasicIterator<T> source(&real,dim);
  BasicIterator<T> dest(&F,dim);
  BasicIterator<index_t> destD(&Ddata,dim);
  BasicArray<T> in_buffer(NTuple(real.dimensions()[dim],1));
  BasicArray<T> out_buffer1(NTuple(out,1));
  BasicArray<index_t> out_buffer2(NTuple(out,1));
  while (source.isValid() && dest.isValid() && destD.isValid()) {
    for (index_t i=1;i<=source.size();i++) {
      in_buffer[i] = source.get();
      source.next();
    }
    Op::func(in_buffer,out_buffer1,out_buffer2);
    for (index_t i=1;i<=out;i++) {
      dest.set(out_buffer1[i]);
      destD.set(out_buffer2[i]);
      dest.next();
      destD.next();
    }
    source.nextSlice(); dest.nextSlice(); destD.nextSlice();
  }
  D = Array(Ddata);
  return Array(F);
}

template <typename T, class Op>
static inline ArrayVector BiVectorOp(const Array &Ain, index_t out, 
				     int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F, D;
  if (Acast.isSparse()) {
    if (Acast.allReal())
      F = BiVectorOp<T,Op>(Acast.constRealSparse<T>(),out,dim,D);
    else
      F = BiVectorOp<T,Op>(Acast.constRealSparse<T>(),
			 Acast.constImagSparse<T>(),out,dim,D);
  }
  if (Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    F = BiVectorOp<T,Op>(Acast.constReal<T>(),out,dim,D);
  } else {
    F = BiVectorOp<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim,D);
  }
  F.toClass(Ain.dataClass());
  ArrayVector ret;
  ret.push_back(F);
  ret.push_back(D);
  return ret;
}

template <class Op>
static inline ArrayVector BiVectorOp(const Array &Ain, int out, int dim) {
  out = qMax(0,out);
  if (Ain.dataClass() == Float) {
    ArrayVector Ret(BiVectorOp<float,Op>(Ain,out,dim,Float));
    Ret[0] = Ret[0].toClass(Ain.dataClass());
    return Ret;
  } else {
    ArrayVector Ret(BiVectorOp<double,Op>(Ain,out,dim,Double));
    if (Ain.dataClass() != StringArray)
      Ret[0] = Ret[0].toClass(Ain.dataClass());
    return Ret;
  }
}

template <typename T, class XIter, class YIter, class Op>
static inline Array StringOp(XIter xi, YIter yi, const NTuple &dims, const ArrayVector &eargs) {
  BasicArray<T> retvec(dims);
  for (index_t i=1;i<=dims.count();i++) {
    ArrayVector t;
    t.push_back(xi.get());
    t.push_back(yi.get());
    for (int j=0;j<eargs.size();j++)
      t.push_back(eargs[j]);
    retvec.set(i,Op::func(t));
    xi.next();
    yi.next();
  }
  return retvec;
}

template <typename T, class Op>
static inline Array StringOp(const ArrayVector& arg) {
  if ((arg[0].dataClass() != CellArray) && (arg[1].dataClass() != CellArray))
    return Array(Op::func(arg));
  Array ax(arg[0]);
  Array ay(arg[1]);
  if (ax.dataClass() != CellArray) ax = CellArrayFromArray(ax);
  if (ay.dataClass() != CellArray) ay = CellArrayFromArray(ay);
  const BasicArray<Array> &x(ax.constReal<Array>());
  const BasicArray<Array> &y(ay.constReal<Array>());
  ArrayVector eargs(arg);
  eargs.pop_front();
  eargs.pop_front();
  if (x.isScalar() && !y.isScalar()) {
    ConstSpinIterator<Array> ix(&x);
    ConstBasicIterator<Array> iy(&y,0);
    return StringOp<T,ConstSpinIterator<Array>,ConstBasicIterator<Array>,Op>(ix,iy,y.dimensions(),eargs);
  } else if (!x.isScalar() && y.isScalar()) {
    ConstBasicIterator<Array> ix(&x,0);
    ConstSpinIterator<Array> iy(&y);
    return StringOp<T,ConstBasicIterator<Array>,ConstSpinIterator<Array>,Op>(ix,iy,x.dimensions(),eargs);
  } else {
    if (x.dimensions() != y.dimensions())
      throw Exception("cell array arguments to string functions must be the same size");
    ConstBasicIterator<Array> ix(&x,0);
    ConstBasicIterator<Array> iy(&y,0);
    return StringOp<T,ConstBasicIterator<Array>,ConstBasicIterator<Array>,Op>(ix,iy,x.dimensions(),eargs);
  }
}

template <class Op>
static inline Array StringOp(const Array &arg) {
  if (arg.isString() && !arg.isVector()) {
    StringVector p(StringVectorFromArray(arg));
    StringVector q;
    for (int i=0;i<p.size();i++) {
      QString x(p[i]);
      Array y(Op::func(Array(x)));
      q.push_back(y.asString());
    }
    return StringArrayFromStringVector(q,QChar(' '));
  }
  if (arg.dataClass() != CellArray)
    return Array(Op::func(arg));
  if (arg.isScalar())
    return Array(Op::func(ArrayFromCellArray(arg)));
  Array ax(arg);
  const BasicArray<Array> &x(ax.constReal<Array>());
  BasicArray<Array> y(ax.dimensions());
  for (index_t i=1;i<=ax.length();i++)
    y[i] = Op::func(x[i]);
  return Array(y);
}


// These macros are defined here -- they are used to map
// operators to jit scalar functions under special circumstances.
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

#define JitScalarFunc2(name,func) \
  extern "C" { EXPORT double _jit_ ## name(double a, double b) { return func(a,b); } }

#define JitScalarFunc1(name,func) \
  extern "C" { EXPORT double _jit_ ## name(double a) { return func(a); } }

#define JitScalarFunc0(name,func) \
  extern "C" { EXPORT double _jit_ ## name() { return func; } }


#endif
