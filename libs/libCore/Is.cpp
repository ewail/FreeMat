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
#include "Interpreter.hpp"
#include "IEEEFP.hpp"
#include "Operators.hpp"
#include "Struct.hpp"
#include <math.h>
#include <float.h>

//@@Signature
//function issame IsSameFunction
//inputs a  b
//outputs y
//DOCBLOCK inspection_issame


template <typename T>
static bool IsSame(const T& a, const T& b);

#define MacroDefIsSame(ctype)			       \
  template <>					       \
  bool IsSame(const ctype& a, const ctype& b) {	       \
    return (a == b);				       \
  }

MacroDefIsSame(bool);
MacroDefIsSame(int8);
MacroDefIsSame(uint8);
MacroDefIsSame(int16);
MacroDefIsSame(uint16);
MacroDefIsSame(int32);
MacroDefIsSame(uint32);
MacroDefIsSame(int64);
MacroDefIsSame(uint64);
MacroDefIsSame(QChar);

template <>
bool IsSame(const float& a, const float& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)) || 
	  (fabsf(b-a) <= 100*fepsf(qMax(fabsf(a),fabsf(b)))));
}

template <>
bool IsSame(const double& a, const double& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)) || 
	  (fabs(b-a) <= 100*feps(qMax(fabs(a),fabs(b)))));
}

template <>
bool IsSame(const Array&, const Array&);

template <typename T>
static bool IsSame(const BasicArray<T>& ar, const BasicArray<T>& br) {
  if (ar.dimensions() != br.dimensions()) return false;
  for (index_t i=1;i<=ar.length();i++)
    if (!IsSame(ar[i],br[i])) 
      return false;
  return true;
}

static bool IsSameStruct(const Array& a, const Array& b) {
  const StructArray &ar(a.constStructPtr());
  const StructArray &br(b.constStructPtr());
  if (ar.fieldNames() != br.fieldNames()) return false;
  if (ar.className() != br.className()) return false;
  for (int i=0;i<ar.fieldCount();i++)
    if (!IsSame(ar[i],br[i])) return false;
  return true;
}

#define MacroIsSame(ctype,cls)						\
  case cls:								\
  { 									\
  if (ad.allReal() && bd.allReal())					\
    return IsSame(ad.constReal<ctype>(),bd.constReal<ctype>());		\
  else {								\
    Array ac(ad); ac.forceComplex();					\
    Array bc(bd); bc.forceComplex();					\
    return (IsSame(ac.constReal<ctype>(),bc.constReal<ctype>()) &&	\
	    IsSame(ac.constImag<ctype>(),bc.constImag<ctype>()));	\
  }									\
  }									

template <>
bool IsSame(const Array& a, const Array& b) {
  if (a.dataClass() != b.dataClass()) return false;
  if (a.dimensions() != b.dimensions()) return false;
  Array ad(a.asDenseArray());
  Array bd(b.asDenseArray());
  switch (ad.dataClass()) {
  default:
    return false;
  MacroExpandCasesAll(MacroIsSame);
  case Struct:
    return IsSameStruct(a,b);
  }
}

ArrayVector IsSameFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    return ArrayVector(Array(false));
  return ArrayVector(Array(IsSame(arg[0],arg[1])));
}

//@@Signature
//sfunction isset IsSetFunction
//inputs name
//outpus flag
//DOCBLOCK inspection_isset
ArrayVector IsSetFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("isset function takes at least one argument - the name of the variable to check for");
  QString fname = arg[0].asString();
  bool isDefed;
  ParentScopeLocker lock(eval->getContext());
  ArrayReference d = eval->getContext()->lookupVariable(fname);
  isDefed = (d.valid());
  if (isDefed && !d->isEmpty())
    return ArrayVector(Array(bool(true)));
  else
    return ArrayVector(Array(bool(false)));
}

//@@Signature
//sfunction isequal IsEqualFunction
//inputs a b
//outputs y
//DOCBLOCK inspection_isequal

template <typename T>
static bool IsEqual(const T& a, const T& b);

#define MacroDefIsEqual(ctype)			       \
  template <>					       \
  bool IsEqual(const ctype& a, const ctype& b) {	       \
    return (a == b);				       \
  }

MacroDefIsEqual(bool);
MacroDefIsEqual(int8);
MacroDefIsEqual(uint8);
MacroDefIsEqual(int16);
MacroDefIsEqual(uint16);
MacroDefIsEqual(int32);
MacroDefIsEqual(uint32);
MacroDefIsEqual(int64);
MacroDefIsEqual(uint64);
MacroDefIsEqual(QChar);
MacroDefIsEqual(float);
MacroDefIsEqual(double);

template <>
bool IsEqual(const Array&, const Array&);

template <typename T>
static bool IsEqual(const BasicArray<T>& ar, const BasicArray<T>& br) {
  if (ar.dimensions() != br.dimensions()) return false;
  for (index_t i=1;i<=ar.length();i++)
    if (!IsEqual(ar[i],br[i])) 
      return false;
  return true;
}

static bool IsEqualStruct(const Array& a, const Array& b) {
  const StructArray &ar(a.constStructPtr());
  const StructArray &br(b.constStructPtr());
  if (ar.fieldNames() != br.fieldNames()) return false;
  if (ar.className() != br.className()) return false;
  for (int i=0;i<ar.fieldCount();i++)
    if (!IsEqual(ar[i],br[i])) return false;
  return true;
}

#define MacroIsEqual(ctype,cls)						\
  case cls:								\
  { 									\
  if (ad.allReal() && bd.allReal())					\
    return IsEqual(ad.constReal<ctype>(),bd.constReal<ctype>());		\
  else {								\
    Array ac(ad); ac.forceComplex();					\
    Array bc(bd); bc.forceComplex();					\
    return (IsEqual(ac.constReal<ctype>(),bc.constReal<ctype>()) &&	\
	    IsEqual(ac.constImag<ctype>(),bc.constImag<ctype>()));	\
  }									\
  }									

template <>
bool IsEqual(const Array& a, const Array& b) {
  if (a.dataClass() != b.dataClass()) return false;
  if (a.dimensions() != b.dimensions()) return false;
  Array ad(a.asDenseArray());
  Array bd(b.asDenseArray());
  switch (ad.dataClass()) {
  default:
    return false;
  MacroExpandCasesAll(MacroIsEqual);
  case Struct:
    return IsEqualStruct(a,b);
  }
}

ArrayVector IsEqualFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2) return ArrayVector(Array(false));
  Array a(arg[0]);
  Array b(arg[1]);
  if (a.isReferenceType() || b.isReferenceType())
    return ArrayVector(Array(IsEqual(a,b)));
  return ArrayVector(Array(IsEqual(a.toClass(Double),b.toClass(Double))));
}

//@@Signature
//sfunction isequalwithequalnans IsEqualWithEqualNaNsFunction
//inputs a b
//outputs y
//DOCBLOCK inspection_isequalwithequalnans

template <typename T>
static bool IsEqualWithEqualNaNs(const T& a, const T& b);

#define MacroDefIsEqualWithEqualNaNs(ctype)			       \
  template <>					       \
  bool IsEqualWithEqualNaNs(const ctype& a, const ctype& b) {	       \
    return (a == b);				       \
  }

MacroDefIsEqualWithEqualNaNs(bool);
MacroDefIsEqualWithEqualNaNs(int8);
MacroDefIsEqualWithEqualNaNs(uint8);
MacroDefIsEqualWithEqualNaNs(int16);
MacroDefIsEqualWithEqualNaNs(uint16);
MacroDefIsEqualWithEqualNaNs(int32);
MacroDefIsEqualWithEqualNaNs(uint32);
MacroDefIsEqualWithEqualNaNs(int64);
MacroDefIsEqualWithEqualNaNs(uint64);
MacroDefIsEqualWithEqualNaNs(QChar);


template <>
bool IsEqualWithEqualNaNs(const float& a, const float& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)));
}

template <>
bool IsEqualWithEqualNaNs(const double& a, const double& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)));
}

template <>
bool IsEqualWithEqualNaNs(const Array&, const Array&);

template <typename T>
static bool IsEqualWithEqualNaNs(const BasicArray<T>& ar, const BasicArray<T>& br) {
  if (ar.dimensions() != br.dimensions()) return false;
  for (index_t i=1;i<=ar.length();i++)
    if (!IsEqualWithEqualNaNs(ar[i],br[i])) 
      return false;
  return true;
}

static bool IsEqualWithEqualNaNsStruct(const Array& a, const Array& b) {
  const StructArray &ar(a.constStructPtr());
  const StructArray &br(b.constStructPtr());
  if (ar.fieldNames() != br.fieldNames()) return false;
  if (ar.className() != br.className()) return false;
  for (int i=0;i<ar.fieldCount();i++)
    if (!IsEqualWithEqualNaNs(ar[i],br[i])) return false;
  return true;
}

#define MacroIsEqualWithEqualNaNs(ctype,cls)						\
  case cls:								\
  { 									\
  if (ad.allReal() && bd.allReal())					\
    return IsEqualWithEqualNaNs(ad.constReal<ctype>(),bd.constReal<ctype>());		\
  else {								\
    Array ac(ad); ac.forceComplex();					\
    Array bc(bd); bc.forceComplex();					\
    return (IsEqualWithEqualNaNs(ac.constReal<ctype>(),bc.constReal<ctype>()) &&	\
	    IsEqualWithEqualNaNs(ac.constImag<ctype>(),bc.constImag<ctype>()));	\
  }									\
  }									

template <>
bool IsEqualWithEqualNaNs(const Array& a, const Array& b) {
  if (a.dataClass() != b.dataClass()) return false;
  if (a.dimensions() != b.dimensions()) return false;
  Array ad(a.asDenseArray());
  Array bd(b.asDenseArray());
  switch (ad.dataClass()) {
  default:
    return false;
  MacroExpandCasesAll(MacroIsEqualWithEqualNaNs);
  case Struct:
    return IsEqualWithEqualNaNsStruct(a,b);
  }
}

ArrayVector IsEqualWithEqualNaNsFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2) return ArrayVector(Array(false));
  Array a(arg[0]);
  Array b(arg[1]);
  if (a.isReferenceType() || b.isReferenceType())
    return ArrayVector(Array(IsEqualWithEqualNaNs(a,b)));
  return ArrayVector(Array(IsEqualWithEqualNaNs(a.toClass(Double),b.toClass(Double))));
}


//@@Signature
//function issparse IsSparseFunction
//inputs x
//outputs flag
//DOCBLOCK inspection_issparse
ArrayVector IsSparseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("issparse function takes one argument - the array to test");
  return ArrayVector(Array(arg[0].isSparse()));
}

struct OpIsNaN {
  static inline float func(float t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
};


//@@Signature
//function isnan IsNaNFunction
//inputs x
//outputs y
//@@Signature
//function IsNaN IsNaNFunction
//inputs x
//outputs y
//DOCBLOCK inspection_isnan
ArrayVector IsNaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
   throw Exception("isnan function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsNaN>(arg[0]).toClass(Bool));
}

JitScalarFunc1(isnan,OpIsNaN::func);

struct OpIsInf {
  static inline float func(float t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
};

//@@Signature
//function isinf IsInfFunction
//inputs x
//outputs y
//@@Signature
//function IsInf IsInfFunction
//inputs x
//outputs y
//DOCBLOCK inspection_isinf
ArrayVector IsInfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isinf function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsInf>(arg[0]).toClass(Bool));
}

JitScalarFunc1(isinf,OpIsInf::func);

//@@Signature
//function isreal IsRealFunction
//inputs x
//outputs y
//DOCBLOCK inspection_isreal
ArrayVector IsRealFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isreal function takes one argument - the array to test");
  return ArrayVector(Array((arg[0].dataClass() != CellArray) && 
			   (arg[0].allReal())));
}

//@@Signature
//function isempty IsEmptyFunction
//inputs x
//outputs flag
//DOCBLOCK inspection_isempty
ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("isempty function requires at least input argument");
  return ArrayVector(Array(bool(arg[0].isEmpty())));
}
