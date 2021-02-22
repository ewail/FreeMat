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
//function cell CellFunction
//inputs varargin
//outputs y
//DOCBLOCK array_cell
ArrayVector CellFunction(int nargout, const ArrayVector& arg) {
  Array p(CellArray,ArrayVectorAsDimensions(arg));
  BasicArray<Array> &dp(p.real<Array>());
  for (index_t i=1;i<=dp.length();i++) {
    dp[i] = Array(Double);
  }
  return ArrayVector(p);
}
