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

#include "LUDecompose.hpp"
#include "Algorithms.hpp"

//@@Signature
//function lu LUFunction
//inputs x
//outputs l u p q r
//DOCBLOCK transforms_lu
ArrayVector LUFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("lu function requires at least one argument - the matrix to decompose.");
  Array A(arg[0]);
  if (A.isReferenceType())
    throw Exception("cannot apply lu decomposition to reference types.");
  if (!A.is2D())
    throw Exception("cannot apply matrix operations to N-dimensional arrays");
  if (!A.isSparse()) {
    if (AnyNotFinite(A))
      throw Exception("lu decomposition only defined for matrices with finite entries.");
    return LUDecompose(nargout,A);
  } else {
    return SparseLUDecompose(nargout,A);
  }
  return ArrayVector();
}
