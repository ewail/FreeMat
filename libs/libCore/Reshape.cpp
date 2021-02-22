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
//function reshape ReshapeFunction jitsafe
//inputs x varargin
//outputs y
//DOCBLOCK array_reshape
ArrayVector ReshapeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("reshape function requires at least one argument");
  if (arg.size() == 1) return arg;
  ArrayVector arg_copy(arg);
  arg_copy.pop_front();
  // Check for the special case of exactly one of the dimensions being
  // empty.
  int empty_count = 0;
  int empty_slot = 0;
  for (int i=0;i<arg_copy.size();i++)
    if (arg_copy[i].isEmpty()) 
      {
	empty_count++;
	empty_slot = i;
      }
  if (empty_count > 1)
    throw Exception("reshape can take exactly one empty dimension argument");
  if (empty_count == 1)
    arg_copy[empty_slot] = Array(int(1));
  NTuple dims(ArrayVectorAsDimensions(arg_copy));
  if (empty_count == 1)
    {
      int A_count = arg[0].length();
      int Desired_count = dims.count();
      // Check to see if A_count is a multiple of Desired_count
      if (A_count % Desired_count != 0)
	throw Exception("reshape cannot be done");
      dims[empty_slot] = int(A_count/Desired_count);
    }
  Array y(arg[0]);
  y.reshape(dims);
  return ArrayVector(y);
}
