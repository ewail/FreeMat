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

struct OpConj {
  static inline float func(float t) { return t; }
  static inline double func(double t) {return t; }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = x; ry = -y;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = x; ry = -y;
  }
};

//@@Signature
//function conj ConjFunction
//inputs x
//outputs y
//DOCBLOCK elementary_conj
ArrayVector ConjFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("conj function requires 1 argument");
  return ArrayVector(UnaryOp<OpConj>(arg[0]));
}
