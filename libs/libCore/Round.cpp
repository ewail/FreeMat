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
#include "Array.hpp"
#include <cmath>

#if defined(_MSC_VER)  
double round( double x ){
    double sign = (x>0)?1:-1;
    return sign*std::floor(fabs(x)+.5);
}

float roundf( float x ){
    float sign = (x>0)?1:-1;
    return sign*std::floorf(fabsf(x)+.5);
}

double trunc( double x ){
	return ( x >= 0 )? floor( x ) : ceil( x );
}

float truncf( float x ){
	return ( x >= 0 )? floorf( x ) : ceilf( x );
}

#endif

struct OpRound {
  static inline float func(float t) { return roundf(t); }
  static inline double func(double t) {return round(t); }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = roundf(x); ry = roundf(y);
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = round(x); ry = round(y);
  }
};

//@@Signature
//function round RoundFunction jitsafe
//input x
//output y
//DOCBLOCK elementary_round
ArrayVector RoundFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("round requires one argument");
  return ArrayVector(UnaryOp<OpRound>(arg[0]));
}

JitScalarFunc1(round,OpRound::func);
