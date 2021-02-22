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
#include "SparseMatrix.hpp"

struct OpBitAnd {
  template <typename T>
  static inline T func(T A, T B) {return T(uint32(A) & uint32(B));}
  template <typename T>
  static inline void func(const T& Ar, const T& Ai,
		   const T& Br, const T& Bi,
		   T& cr, T& ci) {
    cr = T(uint32(Ar) & uint32(Br));
    ci = T(uint32(Ai) & uint32(Bi));
  }
};

struct OpBitOr {
  template <typename T>
  static inline T func(T A, T B) {return T(uint32(A) | uint32(B));}
  template <typename T>
  static inline void func(const T& Ar, const T& Ai,
		   const T& Br, const T& Bi,
		   T& cr, T& ci) {
    cr = T(uint32(Ar) | uint32(Br));
    ci = T(uint32(Ai) | uint32(Bi));
  }
};

struct OpBitXor {
  template <typename T>
  static inline T func(T A, T B) {return T(uint32(A) ^ uint32(B));}
  template <typename T>
  static inline void func(const T& Ar, const T& Ai,
		   const T& Br, const T& Bi,
		   T& cr, T& ci) {
    cr = T(uint32(Ar) ^ uint32(Br));
    ci = T(uint32(Ai) ^ uint32(Bi));
  }
};
  
struct OpBitCmp {
  template <typename T>
  static inline T func(T A) {return T(~A);}
  template <typename T>
  static inline void func(const T &Ar, const T &Ai, T& Cr, T& Ci) {
    Cr = T(~uint32(Ar));
    Ci = T(~uint32(Ai));
  }
};

//@@Signature
//function bitand BitandFunction jitsafe
//inputs a b
//outputs y
//DOCBLOCK binary_bitand
ArrayVector BitandFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("bitand requires at least two arguments");
  return ArrayVector(DotOp<OpBitAnd>(arg[0],arg[1]));
}

JitScalarFunc2(bitand,OpBitAnd::func);

//@@Signature
//function bitor BitorFunction jitsafe
//inputs a b
//outputs y
//DOCBLOCK binary_bitor
ArrayVector BitorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("bitor requires at least two arguments");
  return ArrayVector(DotOp<OpBitOr>(arg[0],arg[1]));
}

JitScalarFunc2(bitor,OpBitOr::func);

//@@Signature
//function bitxor BitxorFunction jitsafe
//inputs a b
//outputs y
//DOCBLOCK binary_bitxor
ArrayVector BitxorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("bitxor requires at least two arguments");
  return ArrayVector(DotOp<OpBitXor>(arg[0],arg[1]));
}

JitScalarFunc2(bitxor,OpBitXor::func);

//@@Signature
//function bitcmp BitcmpFunction jitsafe
//inputs a n
//outputs y
//DOCBLOCK binary_bitcmp

template <typename T>
static Array TBitCmpFunc(const BasicArray<T> &x, double maxval) {
  BasicArray<T> y(x.dimensions());
  for (index_t i=1;i<=y.length();i++) {
    y[i] = maxval - 1 - x[i];
  }
  return Array(y);
}

#define MacroBitCmp(ctype,cls) \
  case cls: return ArrayVector(TBitCmpFunc<ctype>(y.constReal<ctype>(),maxval));

ArrayVector BitcmpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    return ArrayVector(Array());
  if (arg.size() == 1) {
    switch (arg[0].dataClass()) {
    case UInt8: return ArrayVector(UnaryOp<uint8,OpBitCmp>(arg[0],UInt8));
    case UInt16: return ArrayVector(UnaryOp<uint16,OpBitCmp>(arg[0],UInt16));
    case UInt32: return ArrayVector(UnaryOp<uint32,OpBitCmp>(arg[0],UInt32));
    case UInt64: return ArrayVector(UnaryOp<uint64,OpBitCmp>(arg[0],UInt64));
    default:
      throw Exception("bitcmp is only defiled for unsigned integer types.");
    }
  }
  int bits = arg[1].asInteger();
  if (bits <= 0) throw Exception("bitcmp bits must be positive");
  double maxval = pow(2.0,double(bits));
  if (!IsInteger(arg[0])) throw Exception("bitcmp can only be applied to integer arguments");
  if (!IsNonNegative(arg[0])) throw Exception("bitcmp argument must be nonnegative");
  if (arg[0].isComplex()) throw Exception("bitcmp argument must be real valued");
  if (arg[0].isSparse()) throw Exception("bitcmp is not defined for sparse matrices");
  Array y(arg[0]);
  y.ensureNotScalarEncoded();
  switch (arg[0].dataClass()) {
  default:
    throw Exception("type not supported by bitcmp");
    MacroExpandCasesNoBool(MacroBitCmp);
  }
}

//@@Signature
//function int2bin Int2BinFunction
//inputs x bits
//outputs y
//DOCBLOCK typecast_int2bin

struct OpInt2Bin {
  template <typename T>
  static inline void func(const ConstSparseIterator<T> &, SparseSlice<T>&) {
    throw Exception("int2bin not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&) {
    throw Exception("int2bin not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src, BasicArray<T>& dest) {
    int numbits = int(dest.length());
    if (numbits < 1) numbits = 1;
    int64 value = int64(src[1]);
    for (int i=0;i<numbits;i++) {
      dest[numbits-i] = T(value & 1);
      value >>= 1;
    }
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src_real, const BasicArray<T> &src_imag,
			  BasicArray<T>& dest_real, BasicArray<T>& dest_imag) {
    func(src_real,dest_real);
    func(src_imag,dest_imag);
  }
};

ArrayVector Int2BinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("int2bin requires at least two arguments");
  int n = arg[1].asInteger();
  n = qMax(0,qMin(64,n));
  return ArrayVector(VectorOp<OpInt2Bin>(arg[0],n,
					 qMax(1,arg[0].dimensions().lastSingular())).toClass(Bool));
}

//@@Signature
//function bin2int Bin2IntFunction
//inputs x flag
//outputs y
//DOCBLOCK typecast_bin2int
struct OpBin2Int {
  template <typename T>
  static inline void func(const ConstSparseIterator<T> &, SparseSlice<T>&) {
    throw Exception("int2bin not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const ConstComplexSparseIterator<T> &,
			  SparseSlice<T>&, SparseSlice<T>&) {
    throw Exception("int2bin not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src, BasicArray<T>& dest) {
    int numbits = int(src.length());
    uint64 value = (src[1] != 0) ? 1 : 0;
    for (int j=1;j<numbits;j++) {
      value <<= 1;
      value |= (src[j+1] != 0) ? 1 : 0;
    }
    dest[1] = value;
  }
  template <typename T>
  static inline void func(const BasicArray<T> &src_real, const BasicArray<T> &src_imag,
			  BasicArray<T>& dest_real, BasicArray<T>& dest_imag) {
    func(src_real,dest_real);
    func(src_imag,dest_imag);
  }
};

ArrayVector Bin2IntFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("bin2int requires at least one arguments");
  if ((arg.size() == 2) && (arg[1].asString().toUpper() == "SIGNED"))
    return ArrayVector(VectorOp<OpBin2Int>(arg[0].toClass(Int32),1,
					   arg[0].dimensions().lastNotOne()-1));
  else
    return ArrayVector(VectorOp<OpBin2Int>(arg[0].toClass(UInt32),1,
					   arg[0].dimensions().lastNotOne()-1));
}

