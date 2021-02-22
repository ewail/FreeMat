/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include "Algorithms.hpp"
#include "Operators.hpp"

//@@Signature
//function logical LogicalFunction
//inputs x
//outputs y
//DOCBLOCK typecast_logical
ArrayVector LogicalFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Bool));
}

//@@Signature
//function string StringFunction
//inputs x
//outputs y
//DOCBLOCK typecast_string
ArrayVector StringFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(StringArray));
}

//@@Signature
//function uint8 UInt8Function
//inputs x
//outputs y
//DOCBLOCK typecast_uint8
ArrayVector UInt8Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt8));
}

//@@Signature
//function uint16 UInt16Function
//inputs x
//outputs y
//DOCBLOCK typecast_uint16
ArrayVector UInt16Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt16));
}

//@@Signature
//function uint32 UInt32Function
//inputs x
//outputs y
//DOCBLOCK typecast_uint32
ArrayVector UInt32Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt32));
}

//@@Signature
//function uint64 UInt64Function
//inputs x
//outputs y
//DOCBLOCK typecast_uint64
ArrayVector UInt64Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(UInt64));
}

//@@Signature
//function int8 Int8Function
//inputs x
//outputs y
//DOCBLOCK typecast_int8
ArrayVector Int8Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int8));
}

//@@Signature
//function int16 Int16Function
//inputs x
//outputs y
//DOCBLOCK typecast_int16
ArrayVector Int16Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int16));
}

//@@Signature
//function int32 Int32Function
//inputs x
//outputs y
//DOCBLOCK typecast_int32
ArrayVector Int32Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int32));
}

//@@Signature
//function int64 Int64Function
//inputs x
//outputs y
//DOCBLOCK typecast_int64
ArrayVector Int64Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Int64));
}

//DOCBLOCK typecast_single

//@@Signature
//function float FloatFunction
//inputs x
//outputs y
//@@Signature
//function single FloatFunction
//inputs x
//outputs y
//DOCBLOCK typecast_float
ArrayVector FloatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Float));
}

//@@Signature
//function double DoubleFunction
//inputs x
//outputs y
//DOCBLOCK typecast_double
ArrayVector DoubleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Double));
}

//@@Signature
//function complex ComplexFunction
//inputs x z
//outputs y
//DOCBLOCK typecast_complex

struct OpComplex {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
	  throw Exception("invalid type conversion");
	  return v1;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar;
    ci = br;
  }
};

ArrayVector ComplexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) 
    throw Exception("type conversion function requires at least one argument");
  if (arg.size() == 1)
    return ArrayVector(arg[0]);
  else {
    Array x(arg[0].asComplex()); 
    Array y(arg[1].asComplex());
    return ArrayVector(DotOp<OpComplex>(x,y));
  }
}

//@@Signature
//function dcomplex DcomplexFunction
//inputs x
//outputs y
//DOCBLOCK typecast_dcomplex
ArrayVector DcomplexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("type conversion function requires one argument");
  return ArrayVector(arg[0].toClass(Double));
}

//@@Signature
//function typeof TypeOfFunction
//inputs x
//outputs typename
//DOCBLOCK inspection_typeof
ArrayVector TypeOfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("typeof function requires exactly one argument");
  return ArrayVector(Array(arg[0].className()));
}
