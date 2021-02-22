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
#include "Algorithms.hpp"
#include "Interpreter.hpp"
#include "SparseCCS.hpp"

//@@Signature
//function spones SponesFunction
//inputs x
//outputs y
//DOCBLOCK sparse_spones

template <typename T>
static Array SponesSparse(const SparseMatrix<T> &A) {
  ConstSparseIterator<T> iter(&A);
  SparseMatrix<double> ret(NTuple(A.rows(),A.cols()));
  while (iter.isValid()) {
    ret[iter.pos()] = 1;
    iter.next();
  }
  return Array(ret);
}

template <typename T>
static Array SponesDense(const BasicArray<T> &A) {
  ConstBasicIterator<T> iter(&A,0);
  SparseMatrix<double> ret(NTuple(A.rows(),A.cols()));
  while (iter.isValid()) {
    for (index_t i=1;i<=iter.size();i++) {
      ret[iter.pos()] = 1.0;
      iter.next();
    }
    iter.nextSlice();
  }
  return Array(ret);
}

template <typename T>
static Array Spones(const Array &A) {
  if (A.isSparse())
    return SponesSparse(A.constRealSparse<T>());
  else {
    const Array &Adense(A.asDenseArray());
    return SponesDense(Adense.constReal<T>());
  }
}

#define MacroSpones(ctype,cls)				\
  case cls: return ArrayVector(Spones<ctype>(arg[0]));

ArrayVector SponesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("spones function requires a sparse matrix template argument");
  if (arg[0].isEmpty())
    return ArrayVector(EmptyConstructor());
  if(arg[0].isReferenceType())
    throw Exception("spones function requires a numeric sparse matrix argument");
  switch (arg[0].dataClass()) {
  default: throw Exception("Unhandled type for spones");
    MacroExpandCasesSimple(MacroSpones);
  }
}

//@@Signature
//sfunction sparse SparseFunction
//inputs i j v m n
//outputs y
//DOCBLOCK sparse_sparse
ArrayVector SparseFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 1) 
    return ArrayVector(ToSparse(arg[0]));
  else if (arg.size() == 2) 
    return ArrayVector(Array(SparseMatrix<double>(NTuple(arg[0].asInteger(),
							 arg[1].asInteger()))));
  else if (arg.size() == 3) {
    const Array &ip_typed(arg[0].asDenseArray().toClass(Index));
    const Array &jp_typed(arg[1].asDenseArray().toClass(Index));
    const BasicArray<index_t> &ip(ip_typed.constReal<index_t>());
    const BasicArray<index_t> &jp(jp_typed.constReal<index_t>());
    return ArrayVector(IJVToSparse(ip,jp,arg[2]));
  } else if (arg.size() >= 5) {
    if (arg.size() > 5)
      eval->warningMessage("extra arguments to sparse (nnz to reserve) ignored");
    const Array &ip_typed(arg[0].asDenseArray().toClass(Index));
    const Array &jp_typed(arg[1].asDenseArray().toClass(Index));
    const BasicArray<index_t> &ip(ip_typed.constReal<index_t>());
    const BasicArray<index_t> &jp(jp_typed.constReal<index_t>());
    return ArrayVector(IJVToSparse(ip,jp,arg[2],arg[3].asDouble(),arg[4].asDouble()));
  }
  return ArrayVector();
}

//@@Signature
//function full FullFunction
//inputs x
//outputs y
//DOCBLOCK sparse_full
ArrayVector FullFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Need one argument to full function");
  return ArrayVector(arg[0].asDenseArray());
}
