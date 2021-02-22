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
#include "Interpreter.hpp"

//@@Signature
//function  version VersionFunction
//inputs none
//outputs verstring
//DOCBLOCK freemat_version
ArrayVector VersionFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(QString(FREEMAT_VERSION)));
}


//@@Signature
//function verstring VerStringFunction
//inputs none
//output verstring
//DOCBLOCK freemat_verstring
ArrayVector VerStringFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(Interpreter::getVersionString()));
}
