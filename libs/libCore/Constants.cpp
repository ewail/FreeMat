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
#include "LAPACK.hpp"
#include "IEEEFP.hpp"
#include <math.h>
#include <float.h>
#include "Operators.hpp"

//@@Signature
//function i IFunction
//inputs none
//outputs y
//@@Signature
//function j IFunction
//inputs none
//outputs y
//DOCBLOCK constants_i
ArrayVector IFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(0),double(1)));
}

//@@Signature
//function pi PiFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK constants_pi
ArrayVector PiFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(4.0*atan(1.0))));
}  

JitScalarFunc0(pi,double(4.0*atan(1.0)));

//@@Signature
//function e EFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK constants_e
ArrayVector EFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(exp(1.0)));
}  

JitScalarFunc0(e,double(exp(1.0)));

struct OpEps {
  static inline float func(float t) {
    if (IsInfinite(t)) return NaN();
    return fepsf(t);
  }
  static inline double func(double t) {
    if (IsInfinite(t)) return NaN();
    return feps(t);
  }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = qMax(fepsf(x),fepsf(y)); 
    ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = qMax(feps(x),feps(y)); 
    ry = 0;
  }
};

//@@Signature
//function eps EpsFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK constants_eps
ArrayVector EpsFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  
  if( arg.size()> 1 )
    throw Exception("eps takes no more than 1 argument");
  if( arg.size()==1 ){
    Array a( arg[0] );
    if( a.isString() ){
      QString str = a.asString().toLower();
      if( str == QString( "double" ) ){
	retval << Array( feps( 1. ) ); 
      }
      else if( str == QString( "single" ) ){
	retval << Array( fepsf( 1. ) );
      }
      else{
	throw Exception("Class must be 'double' or 'single'");
      }
    }
    else { //numeric argument
      return ArrayVector(Real(UnaryOp<OpEps>(a)));
    }
  } else{
    retval << Array( feps( 1. ) );
  }
  return retval;
}

//@@Signature
//function feps FepsFunction
//inputs none
//outputs y
//DOCBLOCK constants_feps
ArrayVector FepsFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(float(nextafterf(1.0,2.0)-1.0f)));
}

