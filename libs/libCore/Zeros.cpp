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
#include "Utils.hpp"
#include "Operators.hpp"

//@@Signature
//function inf InfFunction
//inputs varargin
//outputs y
//@@Signature
//function Inf InfFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK constants_inf
ArrayVector InfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(Inf()));
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      if (cp == "single") dataclass = Float;
      if (cp == "double") dataclass = Double;
      if (cp == "float") dataclass = Float;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to nan function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  Array p(dataclass,dims);
  if (dataclass == Float)
    p.real<float>().fill(float(Inf()));
  else
    p.real<double>().fill(Inf());
  return ArrayVector(p);
}

JitScalarFunc0(infp,Inf());

//@@Signature
//function nan NaNFunction jitsafe
//inputs varargin
//outputs y
//@@Signature
//function NaN NaNFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK array_nan
ArrayVector NaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(NaN()));
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      if (cp == "single") dataclass = Float;
      if (cp == "double") dataclass = Double;
      if (cp == "float") dataclass = Float;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to nan function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  Array p(dataclass,dims);
  if (dataclass == Float)
    p.real<float>().fill(float(NaN()));
  else
    p.real<double>().fill(NaN());
  return ArrayVector(p);
}

JitScalarFunc0(nan,NaN());
JitScalarFunc0(NaN,NaN());

//@@Signature
//function zeros ZerosFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK array_zeros

#define MacroZeros(ctype,cls) \
  if (cp == #ctype) dataclass = cls;

ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      MacroExpandCasesSimple(MacroZeros);
      if (cp == "single") dataclass = Float;
      if (cp == "logical") dataclass = Bool;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to zeros function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  return ArrayVector(Array(dataclass,dims));
}

#undef MacroZeros


//@@Signature
//function false FalseFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK constants_false
ArrayVector FalseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(bool(false)));
  NTuple dims(ArrayVectorAsDimensions(arg));
  Array r(Bool,dims);
  return ArrayVector(r);
}
