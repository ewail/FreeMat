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

#include "Math.hpp"
#include "EigenDecompose.hpp"
#include "Algorithms.hpp"

static ArrayVector GenEigFunction(int nargout, const ArrayVector &arg) {
  Array A(arg[0]);
  Array B(arg[1]);
  if (A.isSparse() || B.isSparse())
    throw Exception("eig only defined for full matrices.");  
  if (AnyNotFinite(A) || AnyNotFinite(B))
    throw Exception("eig only defined for matrices with finite entries.");
  if (A.isReferenceType() || B.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  if (!A.is2D() || !B.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (!A.isSquare())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  if (!B.isSquare())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  if (A.rows() != B.rows())
    throw Exception("B and A must be the same size when computing a generalized eigendecomposition");

  // Handle the type of A - if it is an integer type, then promote to double
  if (A.dataClass() != B.dataClass())
    throw Exception("B and A must be the same data class when computing a generalized eigendecomposition");

  ArrayVector retval;
  Array V, D;

  if (nargout > 1) {
    if (IsSymmetric(A) && IsSymmetric(B)) {
      if (!GeneralizedEigenDecomposeFullSymmetric(A,B,V,D))
	GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
    } else
      GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
    retval.push_back(V);
    retval.push_back(D);
  } else {
    if (IsSymmetric(A) && IsSymmetric(B)) {
      if (!GeneralizedEigenDecomposeCompactSymmetric(A,B,D))
	GeneralizedEigenDecomposeCompactGeneral(A,B,D);
    } else
      GeneralizedEigenDecomposeCompactGeneral(A,B,D);
    retval.push_back(D);
  }
  return retval;
}


//@@Signature
//function eig EigFunction
//inputs A flag
//outputs V D
//DOCBLOCK transforms_eig
ArrayVector EigFunction(int nargout, const ArrayVector& arg) {
  bool balance;
  if (arg.size() == 0)
    throw Exception("eig function requires at least one argument");
  if (arg.size() == 1)
    balance = true;
  else {
    Array b(arg[1]);
    if (b.isString()) {
      QString b2 = b.asString().toUpper();
      if (b2=="NOBALANCE")
	balance = false;
    }
    else
      return GenEigFunction(nargout, arg);
  }
  Array A(arg[0]);
  A.ensureNotScalarEncoded();
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  if (AnyNotFinite(A))
    throw Exception("eig only defined for matrices with finite entries.");
  ArrayVector retval;
  if (A.isEmpty()) {
    if (nargout > 1) {
      retval.push_back(A);
      retval.push_back(A);
    } else {
      retval.push_back(Array(Double,NTuple(0,1)));
    }
    return retval;
  }
  
  Array V, D;
  if (nargout > 1) {
    if (IsSymmetric(A))
      EigenDecomposeFullSymmetric(A,V,D);
    else
      EigenDecomposeFullGeneral(A,V,D,balance);
    retval.push_back(V);
    retval.push_back(D);
  } else {
    if (IsSymmetric(A))
      EigenDecomposeCompactSymmetric(A,D);
    else
      EigenDecomposeCompactGeneral(A,D,balance);
    retval.push_back(D);
  }
  return retval;
}


//@@Signature
//function eigs EigsFunction
//inputs A k sigma
//outputs V D
//DOCBLOCK sparse_eigs
ArrayVector EigsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("eigs function requires at least one argument");
  Array A(arg[0]);
  if (!A.isSparse())
    throw Exception("eigs only applies to sparse matrix arguments");
  int k;
  if (!A.isSquare())
    throw Exception("eigs can only be applied to square matrices.");
  if (arg.size() < 2) {
    k = 6;
    if (k >= (int)A.rows())
      k = int(A.rows() - 1);
  } else {
    Array kval(arg[1]);
    k = kval.asInteger();
  }
  if (A.dataClass() != Double) 
    throw Exception("eigs only works on double data class");
  bool shiftFlag;
  QString whichflag;
  double sigma[2];
  if (arg.size() < 3) {
    shiftFlag = false;
    whichflag = "LM";
  } else {
    Array S(arg[2]);
    if (S.isString()) {
      shiftFlag = false;
      QString stxt = S.asString().toUpper();
      if ((stxt == "LM") || (stxt == "SM") || (stxt == "LA") || (stxt == "SA") ||
	  (stxt == "BE") || (stxt == "LR") || (stxt == "SR") || (stxt == "LI") ||
	  (stxt == "SI"))
	whichflag = stxt;
      else
	throw Exception("Unrecognized option for sigma - it must be either 'lm', 'sm', 'la', 'sa', 'be', 'lr', 'sr', 'li', or 'si'");
    } else {
      if (!S.isScalar())
	throw Exception("shift parameter sigma must be a scalar");
      if (S.dataClass() != Double) throw Exception("shift parameter must be a double dataclass");
      if (!S.allReal()) {
	sigma[0] = S.constRealScalar<double>();
	sigma[1] = S.constImagScalar<double>();
      } else {
	sigma[0] = S.constRealScalar<double>();
	sigma[1] = 0;
      }
      shiftFlag = true;
    }
  }
  if (!shiftFlag)
    return SparseEigDecompose(nargout,A,k,whichflag);
  else 
    return SparseEigDecomposeShifted(nargout,A,k,sigma);
}
