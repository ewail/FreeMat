/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include "Exception.hpp"
#include "Math.hpp"
#include "MatrixMultiply.hpp"
#include "LinearEqSolver.hpp"
#include "LeastSquaresSolver.hpp"
#include "EigenDecompose.hpp"
#include "LUDecompose.hpp"
#include "SparseMatrix.hpp"
#include "Complex.hpp"
#include "Operators.hpp"
#include <math.h>
#include "IEEEFP.hpp"

template <typename T>
static inline T powi(T a, int b) {
  T p, x;
  int n;
  unsigned long u;
    
  p = 1;
  x = a;
  n = b;
  
  if(n != 0) {
    if(n < 0) {
      n = -n;
      x = 1/x;
    }
    for(u = n; ; )  {
      if(u & 01)
	p *= x;
      if(u >>= 1)
	x *= x;
      else
	break;
    }
  }
  return(p);
}  

template <typename T>
static inline void powi(const T& ar, const T& ai, int b,
			T& pr, T& pi) {
  int n;
  unsigned long u;
  T t;
  T q[2], x[2];
  static T one[2] = {1.0, 0.0};
  n = b;
  q[0] = 1;
  q[1] = 0;
    
  if(n == 0)
    goto done;
  if(n < 0)
    {
      n = -n;
      complex_divide<T>(one[0],one[1],ar,ai,x[0],x[1]);
    }
  else
    {
      x[0] = ar;
      x[1] = ai;
    }
  for(u = n; ; )
    {
      if(u & 01)
	{
	  t = q[0] * x[0] - q[1] * x[1];
	  q[1] = q[0] * x[1] + q[1] * x[0];
	  q[0] = t;
	}
      if(u >>= 1)
	{
	  t = x[0] * x[0] - x[1] * x[1];
	  x[1] = 2 * x[0] * x[1];
	  x[0] = t;
	}
      else
	break;
    }
 done:
  pi = q[1];
  pr = q[0];
}

template <typename T>
static inline void pow(const T& ar, const T& ai,
		       const T& br, const T& bi,
		       T& cr, T& ci) {
  T logr, logi, x, y;
  T mag = complex_abs<T>(ar, ai);
  if (mag == 0) {
    cr = 0;
    ci = 0;
    return;
  }
  logr = log(mag);
  logi = atan2(ai, ar);
  
  x = exp( logr * br - logi * bi );
  y = logr * bi + logi * br;
  
  cr = x * cos(y);
  ci = x * sin(y);
}

struct OpAdd {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1+v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar + br;
    ci = ai + bi;
  }
};

struct OpSubtract {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1-v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar - br;
    ci = ai - bi;
  }
};

struct OpNegate {
  template <typename T>
  static inline T func(const T& v1) {
    return -v1;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  T& cr, T& ci) {
    cr = -ar;
    ci = -ai;
  }
};

struct OpMultiply {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1*v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    complex_multiply(ar,ai,br,bi,cr,ci);
  }
};

struct OpDivide {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1/v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& c0, T& c1) {
    complex_divide<T>(ar,ai,br,bi,c0,c1);
  }
};

struct OpLessThan {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 < v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_lt<T>(ar,ai,br,bi);
  }
};

struct OpLessEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 <= v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_le<T>(ar,ai,br,bi);
  }
};

struct OpEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 == v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {

    return complex_eq<T>(ar,ai,br,bi);
  }
};

struct OpNotEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 != v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_ne<T>(ar,ai,br,bi);
  }
};

struct OpGreaterThan {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 > v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_gt<T>(ar,ai,br,bi);
  }
};

struct OpGreaterEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 >= v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_ge<T>(ar,ai,br,bi);
  }
};

struct OpAnd {
  static inline bool func(const bool& v1, const bool& v2) {
    return (v1 && v2);
  }
  static inline void func(const bool&, const bool&, const bool&, const bool&, bool &, bool &) {
    // Unused...
  }
};

struct OpOr {
  static inline bool func(const bool& v1, const bool& v2) {
    return (v1 || v2);
  }
  static inline void func(const bool&, const bool&, const bool&, const bool&, bool &, bool &) {
    // Unused...
  }
};

struct OpPower {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (v2 == int(v2))
      return powi(v1,int(v2));
    else
      return pow(v1,v2);
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    if ((br == int(br)) && (bi == 0))
      powi(ar,ai,int(br),cr,ci);
    else
      pow(ar,ai,br,bi,cr,ci);
  }
};

//DOCBLOCK operators_typerules

/**
 * Add two objects.
 */
//DOCBLOCK operators_plus

Array Add(const Array& A, const Array& B) {
  return DotOp<OpAdd>(A,B);
}


/**
 * Subtract two objects.
 */
//DOCBLOCK operators_minus
Array Subtract(const Array& A, const Array &B) {
  return DotOp<OpSubtract>(A,B);
}

/**
 * Element-wise multiplication.
 */
//DOCBLOCK operators_dottimes
Array DotMultiply(const Array& A, const Array &B) {
  return DotOp<OpMultiply>(A,B);
}
 
/**
 * Element-wise right divide.
 */
//DOCBLOCK operators_dotrightdivide
Array DotRightDivide(const Array& A, const Array& B) {
  return DotOp<OpDivide>(A,B);
}

/**
 * Element-wise left divide.
 */
//DOCBLOCK operators_dotleftdivide
Array DotLeftDivide(const Array& A, const Array& B) {
  return DotOp<OpDivide>(B,A);
}

/**
 * Element-wise power.
 */
//DOCBLOCK operators_dotpower

// Simplified test -- if A & B are both real
// then if A is negative and B is non-integer,
// then the complex case is used.
Array DotPower(const Array& A, const Array& B) {
  if (A.allReal() && B.allReal() &&
      !IsNonNegative(A) && !IsInteger(B)) {
    return DotOp<OpPower>(A.asComplex(),B.asComplex());
  }
  return DotOp<OpPower>(A,B);
}

/**
 * Element-wise less than.
 */
//DOCBLOCK operators_comparisonops
Array LessThan(const Array& A, const Array& B) {
  return CmpOp<OpLessThan>(A,B);
}

/**
 * Element-wise less equals.
 */
Array LessEquals(const Array& A, const Array& B) {
  return CmpOp<OpLessEquals>(A,B);
}
  
/**
 * Element-wise greater than.
 */
Array GreaterThan(const Array& A, const Array& B) {
  return CmpOp<OpGreaterThan>(A,B);
}

/**
 * Element-wise greater equals.
 */
Array GreaterEquals(const Array& A, const Array& B) {
  return CmpOp<OpGreaterEquals>(A,B);
}

/**
 * Element-wise equals.
 */
Array Equals(const Array& A, const Array& B) {
  return EqOp<OpEquals>(A,B);
}

/**
 * Element-wise notEquals.
 */
Array NotEquals(const Array& A, const Array& B) {
  return EqOp<OpNotEquals>(A,B);
}

/**
 * Element-wise and
 */
//DOCBLOCK operators_logicalops
Array And(const Array& A, const Array& B) {
  return BoolOp<OpAnd>(A,B);
}

/**
 * Element-wise or
 */
Array Or(const Array& A, const Array& B) {
  return BoolOp<OpOr>(A,B);
}

/**
 * Element-wise not
 */
static bool notfunc(bool t) {
  return (!t);
}

Array Not(const Array& A) {
  if (A.isScalar())
    return Array::create(!A.toClass(Bool).constRealScalar<bool>());
  const Array &Abool(A.toClass(Bool));
  return Array::create(Apply(Abool.constReal<bool>(),notfunc));
}

Array Plus(const Array& A) {
  return A;
}

/**
 * Element-wise negate - this one is a custom job, so to speak.
 * 
 */
Array Negate(const Array& A){
  if (A.isReferenceType())
    throw Exception("Cannot negate non-numeric types.");
  if (IsUnsigned(A))
    throw Exception("negation not supported for unsigned types.");
  return UnaryOp<OpNegate>(A);
}


/**
 * We want to perform a matrix-matrix operation between two data objects.
 * The following checks are required:
 *  1. If A or B is a scalar, then return false - this is really a 
 *     vector operation, and the arguments should be passed to a 
 *     vector checker (like VectorCheck).
 *  2. Both A & B must be numeric
 *  3. Both A & B must be the same type (if not, the lesser type is
 *     automatically promoted).
 *  4. Both A & B must be 2-Dimensional.
 *  5. A & B must be conformant, i.e. the number of columns in A must
 *     match the number of rows in B.
 */


// Invert a square matrix - Should check for diagonal matrices
// as a special case
Array InvertMatrix(const Array &a) {
  if (!a.is2D())
    throw Exception("Cannot invert N-dimensional arrays.");
  if (!a.isSquare())
    throw Exception("Cannot invert non-square matrices");
  if (a.isScalar())
    return DotRightDivide(Array(1.0),a);
  return Invert(a);
}
 
template <typename T>
static inline SparseMatrix<T> SparseOnesFunc(const NTuple & dims) {
  SparseMatrix<T> ret(dims);
  for (index_t i=1;i<=qMin(dims[0],dims[1]);i++)
    ret.set(NTuple(i,i),T(1));
  return ret;
}

#define MacroSparseOnes(ctype,cls) \
  case cls: return Array(SparseOnesFunc<ctype>(dims));
 
Array SparseOnes(DataClass type, const NTuple & dims) {
  switch (type) {
  default: throw Exception("Unsupported type for sparse ones function");
  MacroExpandCasesAll(MacroSparseOnes);
  }
}

#undef MacroSparseOnes

// Handle matrix powers for sparse matrices
Array MatrixPowerSparse(Array a, Array b) {
  // The expression a^B where a is a scalar, and B is sparse is not handled
  if (a.isScalar() && !b.isScalar())
    throw Exception("expression a^B, where a is a scalar and B is a sparse matrix is not supported (use full to convert B to non-sparse matrix");
  // The expression A^B is not supported
  if (!a.isScalar() && !b.isScalar())
    throw Exception("expression A^B where A and B are both sparse matrices is not supported (or defined)");
  // The expression A^b where b is not an integer is not supported
  if (!b.allReal())
    throw Exception("expression A^b where b is complex and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  if (!IsInteger(b))
    throw Exception("expression A^b where b is non-integer and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  if (!a.isSquare())
    throw Exception("expression A^b requires A to be square.");
  int32 power = b.toClass(Int32).constRealScalar<int32>();
  if (power < 0) {
    a = InvertMatrix(a);
    power = -power;
  }
  if (power == 0)
    return SparseOnes(a.dataClass(),a.dimensions());
  Array c(a);
  for (int i=1;i<power;i++)
    c = Multiply(c,a);
  return c;
}


/**
 * Matrix-matrix multiply
 */
//DOCBLOCK operators_times

template <typename T>
static inline Array RealMultiply(const Array & A, const Array& B) {
  if (A.isSparse() && !B.isSparse()) {
    return Array(MatrixMultiply<T>(A.constRealSparse<T>(),B.constReal<T>()));
  } else if (!A.isSparse() && B.isSparse()) {
    return Array(MatrixMultiply<T>(A.constReal<T>(),
				   B.constRealSparse<T>()));
  } else if (A.isSparse() && B.isSparse()) {
    return Array(MatrixMultiply<T>(A.constRealSparse<T>(),
				   B.constRealSparse<T>()));
  } else 
    return MatrixMultiply(A,B);
}

template <typename T>
static inline Array ComplexMultiply(const Array & A, const Array & B) {
  if (A.isSparse() && !B.isSparse()) {
    return Array(DotOp<T,T,OpSubtract>(MatrixMultiply<T>(A.constRealSparse<T>(),
							 B.constReal<T>()),
				       MatrixMultiply<T>(A.constImagSparse<T>(),
							 B.constImag<T>())),
		 DotOp<T,T,OpAdd>(MatrixMultiply<T>(A.constRealSparse<T>(),B.constImag<T>()),
				  MatrixMultiply<T>(A.constImagSparse<T>(),B.constReal<T>())));
  } else if (!A.isSparse() && B.isSparse()) {
    return Array(DotOp<T,T,OpSubtract>(MatrixMultiply<T>(A.constReal<T>(),
							 B.constRealSparse<T>()),
				       MatrixMultiply<T>(A.constImag<T>(),
							 B.constImagSparse<T>())),
		 DotOp<T,T,OpAdd>(MatrixMultiply<T>(A.constReal<T>(),
						    B.constImagSparse<T>()),
				  MatrixMultiply<T>(A.constImag<T>(),
						    B.constRealSparse<T>())));
  } else if (A.isSparse() && B.isSparse()) {
    return Array(DotOp<T,T,OpSubtract>(MatrixMultiply<T>(A.constRealSparse<T>(),
							 B.constRealSparse<T>()),
				       MatrixMultiply<T>(A.constImagSparse<T>(),
							 B.constImagSparse<T>())),
		 DotOp<T,T,OpAdd>(MatrixMultiply<T>(A.constRealSparse<T>(),
						    B.constImagSparse<T>()),
				  MatrixMultiply<T>(A.constImagSparse<T>(),
						    B.constRealSparse<T>())));
  } else 
    return MatrixMultiply(A,B);
}

template <typename T>
static inline Array Multiply(const Array& A, const Array& B) {
  if (A.allReal() && B.allReal())
    return RealMultiply<T>(A,B);
  else
    return ComplexMultiply<T>(A.asComplex(),B.asComplex());
}

Array Multiply(const Array& A, const Array& B){
  // Process our arguments
  if (A.isScalar() || B.isScalar())
    // Its really a vector product, pass...
    return DotMultiply(A,B);
  
  // Test for conformancy
  if (A.columns() != B.rows()) 
    throw Exception("Requested matrix multiplication requires arguments to be conformant.");

  DataClass via, out;
  ComputeTypes(A,B,via,out);
  if (via == Float)
    return Multiply<float>(A.toClass(Float),B.toClass(Float)).toClass(out);
  else 
    return Multiply<double>(A.toClass(Double),B.toClass(Double)).toClass(out);
}
    
/**
 * Matrix-matrix divide
 */
//DOCBLOCK operators_leftdivide
Array LeftDivide(const Array& A, const Array& B) {
  // Process our arguments
  if (A.isScalar() || (B.isScalar() && (!A.is2D() || (A.rows() != 1))))
    // Its really a vector product, pass...
    return DotLeftDivide(A,B);
  if (A.isSquare())
    return SolveLinearEq(A,B);
  else
    return SolveLeastSquares(A,B);
}
    
/**
 * Matrix-matrix divide
 */
//DOCBLOCK operators_rightdivide
Array RightDivide(const Array& A, const Array& B) {
  Array C;

  // Process our arguments
  if (B.isScalar() || (A.isScalar() && (!B.is2D() || (B.cols() != 1))))
    // Its really a vector product, pass...
    return DotRightDivide(A,B);
  return Transpose(LeftDivide(Transpose(B),Transpose(A)));
}


//DOCBLOCK operators_hermitian

//DOCBLOCK operators_transpose


/**
 * Matrix-matrix power - These are the cases to consider:
 *   1. scalar-scalar defer to dotpower
 *   2. square-scalar - if an integer, iteratively multiply
 *          To raise A^N power, we do a successive factorization
 *          A -> A^2 -> A^4
 * 13 -> 8 + 4 + 2 + 1
 *  5 -> 4 + 2 + 1
 *  1 -> 1
 *
 * 23 -> 16 + 8 + 4 + 2 + 1
 *  7 -> 4 + 2 + 1
 *  3 -> 2 + 1
 *  1 -> 1
 *
 * 
 *   3. scalar-square - Decompose 
 */
//DOCBLOCK operators_matrixpower
// Commented out this test for the 4.0 release (reference file was lost).
//Tests
//$near#y1=x1^x2#(any(loopi==[2,18,64,50:52])||(any(loopj==[50:52])))

// Raises Scalar^Matrix
static Array PowerScalarMatrix(const Array &A, const Array &B) {
  // Do an eigendecomposition of B
  Array V, D;
  if (IsSymmetric(B))
    EigenDecomposeFullSymmetric(B,V,D);
  else
    EigenDecomposeFullGeneral(B,V,D,false);
  // Get the diagonal part of D
  Array E = GetDiagonal(D);
  // Call the vector version of the exponential
  Array F = DotPower(A,E); // B, V, D, E, F
  // Construct a diagonal matrix from F
  Array G = DiagonalArray(F); // B, V, D, G, E, F
  // The output is (V*G)/V
  E = Multiply(V,G); // B, V, D, E, F
  return RightDivide(E,V); // B, D, F
}

// Raises Matrix^Scalar
static Array PowerMatrixScalar(const Array &A, const Array &B) {
  // Do an eigendecomposition of A
  Array V, D;
  if (IsSymmetric(A))
    EigenDecomposeFullSymmetric(A,V,D); //A, B, V, D
  else
    EigenDecomposeFullGeneral(A,V,D,false);
  // Get the diagonal part of D
  Array E = GetDiagonal(D); // A, B, V, D, E
  // Call the vector version of the exponential
  Array F = DotPower(E,B); // F, A, V, D
  // Construct a diagonal matrix from F
  Array G = DiagonalArray(F); // G, A, V, D, F
  // The output is (V*G)/V
  E = Multiply(V,G); // A, V, D, E, F
  return RightDivide(E,V); // C, A, D, F
}

Array Power(const Array& Ain, const Array& Bin){
  if (Ain.isEmpty() || Bin.isEmpty())
    return EmptyConstructor();
  DataClass via_type;
  DataClass out_type;
  ComputeTypes(Ain,Bin,via_type,out_type);
  Array A = Ain.toClass(via_type);
  Array B = Bin.toClass(via_type);
  if (A.isScalar() && B.isScalar()) 
    return DotPower(A,B).toClass(out_type);
  if (!A.is2D() || !B.is2D()) 
    throw Exception("Cannot apply exponential operator to N-Dimensional arrays.");
  if (B.allReal() && B.isScalar() && (B.asDouble() == -1))
    return InvertMatrix(A).toClass(out_type);;
  // Both arguments must be square
  if (!(A.isSquare() && B.isSquare()))
    throw Exception("Power (^) operator can only be applied to scalar and square arguments.");
  if (A.isSparse() || B.isSparse())
    return MatrixPowerSparse(A,B).toClass(out_type);
  // OK - check for A a scalar - if so, do a decomposition of B
  if (A.isScalar())
    return PowerScalarMatrix(A,B).toClass(out_type);
  else if (B.isScalar())
    return PowerMatrixScalar(A,B).toClass(out_type);
  else 
    throw Exception("One of the arguments to (^) must be a scalar.");
}

Array UnitColon(const Array& A, const Array& B) {
  Array C;
  if (!A.isScalar() || !B.isScalar())
    throw Exception("Both arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex())
    throw Exception("Both arguments to (:) operator must be real.");
  if ((A.dataClass() == Float) || (B.dataClass() == Float))
    return RangeConstructor(A.asDouble(),1,B.asDouble(),false).toClass(Float);
  return RangeConstructor(A.asDouble(),1,B.asDouble(),false);
}

Array DoubleColon(const Array& A, const Array& B, const Array& C){
  Array D;
  if (!A.isScalar() || !B.isScalar() || !C.isScalar())
    throw Exception("All three arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex() || C.isComplex())
    throw Exception("All arguments to (:) operator must be real.");
  if ((A.dataClass() == Float) || (B.dataClass() == Float) || (C.dataClass() == Float))
    return RangeConstructor(A.asDouble(),B.asDouble(),C.asDouble(),false).toClass(Float);
  return RangeConstructor(A.asDouble(),B.asDouble(),C.asDouble(),false);
}
