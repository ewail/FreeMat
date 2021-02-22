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
#include "Complex.hpp"
#include "Math.hpp"
#include <cmath>

struct OpAbs {
  static inline float func(float t) {return fabsf(t);}
  static inline double func(double t) {return fabs(t);}
  template <typename T>
  static inline void func(T x, T y, T &rx, T &ry) {
    rx = complex_abs(x,y); ry = 0;
  }
};

//@@Signature
//function abs AbsFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK elementary_abs
ArrayVector AbsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("abs function requires 1 argument");
  return ArrayVector(Real(UnaryOp<OpAbs>(arg[0])));
}

JitScalarFunc1(abs,OpAbs::func);
