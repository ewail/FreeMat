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
#include "Struct.hpp"
#include "Algorithms.hpp"

//@@Signature
//function struct StructFunction
//inputs varargin
//outputs struct
//DOCBLOCK variables_struct
ArrayVector StructFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) {
    Array t(arg[0]);
    if (!t.isUserClass() && t.dataClass() == Struct)
      return ArrayVector(t);
    if (!t.isUserClass())
      throw Exception("can only convert objects (user-defined types) into structs");
    if (t.structPtr().isHandleClass())
      throw Exception("Do not convert handle classes to structs...");
    t.structPtr().setClassName("");
    return ArrayVector(t);
  }
  if (arg.size() % 2)
    throw Exception("struct function requires pairs of field names and values");
  int pairCount = arg.size() / 2;
  StringVector names;
  ArrayVector values;
  for (int i=0;i<pairCount;i++) 
    values.push_back(Array());
  for (int i=0;i<pairCount*2;i+=2) {
    if (!(arg[i].isString()))
      throw Exception("struct function requires pairs of field names and values");
    names.push_back(arg[i].asString());
    values[i/2] = arg[i+1];
  }
  return ArrayVector(StructConstructor(names,values));
}
