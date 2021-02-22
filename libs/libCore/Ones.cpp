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

//@@Signature
//function ones OnesFunction
//inputs varargin
//outputs x
//DOCBLOCK array_ones
ArrayVector OnesFunction(int nargout, const ArrayVector& arg) {
  NTuple dims(ArrayVectorAsDimensions(arg));
  Array r(Double,dims);
  BasicArray<double> &rp(r.real<double>());
  for (index_t i=1;i<=rp.length();i++) rp[i] = 1;
  return ArrayVector(r);
}

//@@Signature
//function true TrueFunction
//inputs varargin
//outputs y
//DOCBLOCK constants_true
ArrayVector TrueFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(bool(true)));
  NTuple dims(ArrayVectorAsDimensions(arg));
  Array r(Bool,dims);
  BasicArray<bool> &rp(r.real<bool>());
  for (index_t i=1;i<=rp.length();i++) rp[i] = true;
  return ArrayVector(r);  
}
