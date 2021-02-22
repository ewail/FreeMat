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
#include "Algorithms.hpp"
#include "Struct.hpp"
#include "IEEEFP.hpp"
#include "SparseCCS.hpp"

const Array StringArrayFromStringVector(const StringVector& arg, QChar pad) {
  int maxlen = 0;
  for (int i=0;i<arg.size();i++)
    maxlen = qMax(maxlen,arg[i].size());
  Array ret(StringArray,NTuple(arg.size(),maxlen));
  BasicArray<QChar> &sp(ret.real<QChar>());
  for (int i=0;i<arg.size();i++) {
    for (int j=0;j<arg[i].size();j++)
      sp.set(NTuple(index_t(i+1),index_t(j+1)),arg[i][j]);
    for (int j=arg[i].size();j<maxlen;j++)
      sp.set(NTuple(index_t(i+1),index_t(j+1)),pad);
  }
  return ret;
}

Array Vectorize(const Array& arg) {
  Array A(arg);
  A.reshape(NTuple(A.length(),1));
  return A;
}

bool IsColonOp(const Array &x) {
  return (x.dataClass() == StringArray) && (x.asString() == ":");
}

bool IsCellStringArray(const Array &arg) {
  if (arg.dataClass() != CellArray) return false;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i<=arg.length();i++)
    if (!rp[i].isString()) return false;
  return true;
}

template <typename T>
static inline bool Tis_negative(const Array &x) {
  if (x.isSparse())
    return IsNonNegative(x.constRealSparse<T>());
  else if (x.isScalar())
    return IsNonNegative(x.constRealScalar<T>());
  else
    return IsNonNegative(x.constReal<T>());
}

#define MacroIsNonNegative(ctype, cls) \
  case cls: return Tis_negative<ctype>(x);

bool IsNonNegative(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
  default:
    return false;
  case Bool:
  case UInt8:
  case UInt16:
  case StringArray:
  case UInt32:
  case UInt64:
    return true;
    MacroExpandCasesSigned(MacroIsNonNegative);
  }
}
#undef MacroIsNonNegative


bool IsUnsigned(const Array &x) {
  switch (x.dataClass()) {
  default:
    return false;
  case Bool:
  case UInt8:
  case StringArray:
  case UInt16:
  case UInt32:
  case UInt64:
    return true;
  }
}

template <typename T>
static inline bool Tis_integer(const Array &x) {
  if (x.isSparse()) {
    return IsInteger(x.constRealSparse<T>());
  } else if (x.isScalar()) {
    return IsInteger(x.constRealScalar<T>());
  } else {
    return IsInteger(x.constReal<T>());
  }
}

bool IsInteger(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
  default:
    return false;
  case Int8:
  case UInt8:
  case Int16:
  case UInt16:
  case Int32:
  case UInt32:
  case Int64:
  case UInt64:
    return true;
  case Float: return Tis_integer<float>(x);
  case Double: return Tis_integer<double>(x);
  }
}

static void ValidateIndex(const Array &d) {
  const BasicArray<index_t> &dp(d.constReal<index_t>());
  for (index_t n=1;n<=dp.length();n++) 
    if (dp[n] < 1) throw Exception("index values must be >= 1");
}

const IndexArray IndexArrayFromArray(const Array &index) {
  if ((index.dataClass() == Bool) && (index.type().Scalar == 1)) {
    const Array &index_dense(index.asDenseArray());
    return Find(index_dense.constReal<bool>());
  }
  if (index.type().Sparse == 1)
    throw Exception("Sparse indexing not supported currently");
  if (!index.allReal())
    WarningMessage("Complex part of index ignored");
  if (index.dataClass() == Double) {
    const Array &index_dense(index.asDenseArray());
    ValidateIndex(index_dense);
    return IndexArray(index_dense.constReal<index_t>());
  }
  if (index.dataClass() == Bool) {
    const Array &index_dense(index.asDenseArray());
    return Find(index_dense.constReal<logical>());
  }
  if (IsColonOp(index))
    return IndexArray(-1);
  Array index_converted(index.toClass(Double));
  if (!index_converted.allReal())
    WarningMessage("Complex part of index ignored");
  index_converted = index_converted.asDenseArray();
  ValidateIndex(index_converted);
  return index_converted.constReal<index_t>();
}

const ArrayVector ArrayVectorFromCellArray(const Array &arg) {
  if (arg.isEmpty()) return ArrayVector();
  if (arg.dataClass() != CellArray)
    throw Exception("Unsupported type for call to toArrayVector");
  ArrayVector ret;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i<=arg.length();i++)
    ret.push_back(rp.get(i));
  return ret;
}

Array ArrayFromCellArray(const Array &arg) {
  if (arg.dataClass() != CellArray)
    throw Exception("Unsupported type for call to ArrayFromCellArray");
  if (arg.length() != 1)
    throw Exception("ArrayFromCellArray called with non-scalar argument");
  return arg.constReal<Array>().get(1);
}

const Array CellArrayFromArray(const Array & arg) {
  Array ret(CellArray,NTuple(1,1));
  ret.real<Array>().set(1,arg);
  return ret;
}

const Array CellArrayFromArrayVector(ArrayVector &arg, index_t cnt) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (index_t i=1;i<=cnt;i++) {
    rp.set(i,arg.front());
    arg.pop_front();
  }
  return ret;
}

const Array CellArrayFromArrayVector(ArrayVector arg) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (index_t i=1;i<=arg.size();i++)
    rp.set(i,arg[i-1]);
  return ret;
}

Array CellConstructor(const ArrayVector &arg) {
  ArrayVector copy(arg);
  return CellArrayFromArrayVector(copy,copy.size());
}

const Array CellArrayFromStringVector(const StringVector& arg) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (int i=0;i<arg.size();i++)
    rp.set(index_t(i+1),Array(arg[i]));
  return rp;
}

StringVector StringVectorFromArray(const Array &arg) {
  if (arg.dataClass() == Double) 
    return StringVectorFromArray(arg.toClass(StringArray));
  if (arg.isString()) {
    const BasicArray<QChar> &ap(arg.constReal<QChar>());
    ConstBasicIterator<QChar> iter(&ap,1);
    StringVector ret;
    while (iter.isValid()) {
      QString t(int(iter.size()),QChar(0));
      for (index_t i=1;i<=iter.size();i++) {
	t[int(i-1)] = iter.get();
	iter.next();
      }
      iter.nextSlice();
      ret << t;
    }
    return ret;
  }
  if (!IsCellStringArray(arg)) throw Exception("Cannot convert array to a set of a strings");
  StringVector ret;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i<=rp.length();i++)
    ret.push_back(rp[i].asString());
  return ret;
}

void SetCellContents(Array &cell, const Array& index, 
		     ArrayVector& data) {
  if (cell.dataClass() != CellArray) {
    if (!cell.isEmpty())
      throw Exception("A{B} = C only supported for cell arrays.");
    else
      cell = cell.toClass(CellArray);
  }
  if (IsColonOp(index)) {
    if (cell.length() > data.size())
      throw Exception("Not enough right hand side values to satisfy left hand side expression.");
    cell.set(index,CellArrayFromArrayVector(data,data.size()));
    data.clear();
    return;
  }
  IndexArray ndx(IndexArrayFromArray(index));
  if (ndx.length() > data.size())
    throw Exception("Not enought right hand side values to satisfy left hand side expression.");
  cell.set(ndx,CellArrayFromArrayVector(data,ndx.length()));
}

void SetCellContents(Array &cell, const ArrayVector& index, 
		     ArrayVector& data) {
  if (cell.dataClass() != CellArray)
    throw Exception("A{B1,B2,...BN} = B only supported for cell arrays.");
  IndexArrayVector addr;
  NTuple dims;
  for (int i=0;i<index.size();i++) {
    addr.push_back(IndexArrayFromArray(index[i]));
    if (IsColonOp(addr[i]))
      dims[i] = cell.dimensions()[i];
    else
      dims[i] = addr[i].length();
  }
  if (data.size() < dims.count())
    throw Exception("Not enough right hand side values to satisfy left hand side expression");
  cell.set(addr,CellArrayFromArrayVector(data,dims.count()));
}

QStringList FieldNames(const Array& arg) {
  if (arg.dataClass() != Struct)
    throw Exception("fieldnames only valid for structure arrays");
  const StructArray &rp(arg.constStructPtr());
  return rp.fieldNames();
}

static inline void do_single_sided_algo_double(double a, double b,double *pvec, 
					       int adder, int count) {
  double d = a;
  for (int i=0;i<count;i++) {
    pvec[i*adder] = (double) d;
    d += b;
  }
}
  
static inline void do_double_sided_algo_double(double a, double b, double c, double *pvec, 
					       int adder, int count) {
  if (count%2) {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2+1);
  } else {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2);
  }
}

Array RangeConstructor(double minval, double stepsize, double maxval, bool vert) {
  NTuple dim;
  if (stepsize == 0) 
    throw Exception("step size must be nonzero in colon expression");
  if (!IsFinite(minval) || !IsFinite(stepsize) || !IsFinite(maxval))
    return EmptyConstructor();
  //ideally, n = (c-a)/b
  // But this really defines an interval... we let
  // n_min = min(c-a)/max(b)
  // n_max = max(c-a)/min(b)
  // where min(x) = {y \in fp | |y| is max, |y| < |x|, sign(y) = sign(x)}
  //       max(x) = {y \in fp | |y| is min, |y| > |x|, sign(y) = sign(x)}
  double ntest_min = nextafter(maxval-minval,0)/nextafter(stepsize,stepsize+stepsize);
  double ntest_max = nextafter(maxval-minval,maxval-minval+stepsize)/nextafter(stepsize,0);
  index_t npts = (index_t) floor(ntest_max);
  bool use_double_sided = (ntest_min <= npts) && (npts <= ntest_max);
  npts++;
  if (npts < 0) npts = 0;
  if (vert) {
    dim = NTuple(npts,1);
  } else {
    dim = NTuple(1,npts);
  }
  BasicArray<double> rp(dim);
  if (use_double_sided)
    do_double_sided_algo_double(minval,stepsize,maxval,rp.data(),int(1),int(npts));
  else
    do_single_sided_algo_double(minval,stepsize,rp.data(),int(1),int(npts));
  return Array(rp);
}

template <typename T>
static inline Array T_Transpose(const Array &x) {
  if (x.isScalar())
    return x;
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Transpose(x.constImagSparse<T>()));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Transpose(x.constImag<T>()));
  }
}

#define MacroTranspose(ctype,cls) \
  case cls: return T_Transpose<ctype>(A);

Array Transpose(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesAll(MacroTranspose);
  }
}

#undef MacroTranspose

template <typename T>
static inline Array T_Hermitian(const Array &x) {
  if (x.isScalar()) {
    if (x.allReal())
      return x;
    else 
      return Array(x.constRealScalar<T>(),(T)-x.constImagScalar<T>());
  }
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Negate(Transpose(x.constImagSparse<T>())));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Negate(Transpose(x.constImag<T>())));
  }
}

#define MacroHermitian(ctype,cls) \
  case cls: return T_Hermitian<ctype>(A);

Array Hermitian(const Array &A) {
  if (A.allReal())
    return Transpose(A);
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesSigned(MacroHermitian);
  }
}

#undef MacroHermitian

BasicArray<Array> ArrayVectorToBasicArray(const ArrayVector& a) {
  BasicArray<Array> retvec(NTuple(a.size(),1));
  for (int i=0;i<a.size();i++) 
    retvec.set(index_t(i+1),a.at(i));
  return retvec;
}

NTuple ConvertArrayToNTuple(const Array &A) {
  if (A.length() > NDims)
    throw Exception(QString("MAT Variable has more dimensions ") + 
		    QString("than maxDims (currently set to ") + 
		    NDims + ")."); // FIXME - more graceful ways to do this
  Array B(A.asDenseArray().toClass(Index));
  const BasicArray<index_t> &rp(B.constReal<index_t>());
  NTuple dm;
  for (index_t i=1;i<=B.length();i++)
    dm[int(i-1)] = rp[i];
  return dm;
}

Array ConvertNTupleToArray(const NTuple &D) {
  index_t Dsize(D.lastNotOne());
  BasicArray<index_t> rp(NTuple(1,Dsize));
  for (index_t i=1;i<=Dsize;i++)
    rp.set(i,D[int(i-1)]);
  return Array(rp);
}

template <typename T>
static inline Array Treal_part(const Array *ptr) {
  if (ptr->isScalar())
    return Array(ptr->constRealScalar<T>());
  else if (ptr->isSparse())
    return Array(ptr->constRealSparse<T>());
  else
    return Array(ptr->constReal<T>());
}

#define MacroReal(ctype,cls)			\
  case cls: return Treal_part<ctype>(&A);

Array Real(const Array &A) {
  if (A.isReferenceType()) return A;
  switch (A.dataClass()) {
  default: throw Exception("unhandled type for real");
    MacroExpandCasesNoCell(MacroReal);
  }
}

#undef MacroReal

template <typename T>
static inline Array Timag_part(const Array *ptr) {
  if (ptr->isScalar())
    return Array(ptr->constImagScalar<T>());
  else if (ptr->isSparse())
    return Array(ptr->constImagSparse<T>());
  else
    return Array(ptr->constImag<T>());
}

#define MacroImag(ctype,cls)			\
  case cls: return Timag_part<ctype>(&A);

Array Imag(const Array &A) {
  if (A.isReferenceType()) 
    throw Exception("unhandled type for imag");
  switch (A.dataClass()) {
  default: throw Exception("unhandled type for imag");
    MacroExpandCasesNoCell(MacroImag);
  }
}

#undef MacroImag

template <typename T>
static inline Array Tmerge(const Array *p_real, const Array *p_imag) {
  if (p_real->isScalar())
    return Array(p_real->constRealScalar<T>(),p_imag->constRealScalar<T>());
  else if (p_real->isSparse())
    return Array(p_real->constRealSparse<T>(),p_imag->constRealSparse<T>());
  else
    return Array(p_real->constReal<T>(),p_imag->constReal<T>());
}

#define MacroMerge(ctype,cls)			\
  case cls: return Tmerge<ctype>(&Real,&Imag);

Array MergeToComplex(const Array &Real, const Array &Imag) {
  if (Real.dataClass() != Imag.dataClass()) 
    throw Exception("cannot merge unlike types into a complex type");
  if (Real.isSparse() ^ Imag.isSparse())
    throw Exception("cannot merge sparse and not-sparse arrays into a complex type");
  if (Real.isReferenceType() || Imag.isReferenceType())
    throw Exception("reference types cannot be combined into a complex type");
  if (Real.dimensions() != Imag.dimensions())
    throw Exception("cannot merge different sized arrays into a complex type");
  switch (Real.dataClass()) {
  default: throw Exception("Unhandled type for complex output");
    MacroExpandCasesNoBool(MacroMerge);
  }
}
#undef MacroMerge

template <typename T>
static inline bool Tanynotfinite(const Array *ptr) {
  if (ptr->isScalar())
    if (ptr->allReal())
      return (!IsFinite(ptr->constRealScalar<T>()));
    else
      return (!IsFinite(ptr->constRealScalar<T>()) || !IsFinite(ptr->constImagScalar<T>()));
  else if (ptr->isSparse())
    if (ptr->allReal())
      return AnyNotFinite(ptr->constRealSparse<T>());
    else
      return (AnyNotFinite(ptr->constRealSparse<T>()) || AnyNotFinite(ptr->constImagSparse<T>()));
  else
    if (ptr->allReal())
      return AnyNotFinite(ptr->constReal<T>());
    else
      return (AnyNotFinite(ptr->constReal<T>()) || AnyNotFinite(ptr->constImag<T>()));
}

#define MacroAnyNotFinite(ctype,cls)		\
  case cls: return Tanynotfinite<ctype>(&A);

bool AnyNotFinite(const Array &A) {
  if (A.isReferenceType()) return false;
  switch (A.dataClass()) {
  default: return false;
    MacroExpandCasesSimple(MacroAnyNotFinite);
  }
}

#undef MacroAnyNotFinite


template <typename T>
static inline bool Tanynan(const Array *ptr) {
  if (ptr->isScalar())
    if (ptr->allReal())
      return (IsNaN(ptr->constRealScalar<T>()));
    else
      return (IsNaN(ptr->constRealScalar<T>()) || IsNaN(ptr->constImagScalar<T>()));
  else if (ptr->isSparse())
    if (ptr->allReal())
      return AnyNaN(ptr->constRealSparse<T>());
    else
      return (AnyNaN(ptr->constRealSparse<T>()) || AnyNaN(ptr->constImagSparse<T>()));
  else
    if (ptr->allReal())
      return AnyNaN(ptr->constReal<T>());
    else
      return (AnyNaN(ptr->constReal<T>()) || AnyNaN(ptr->constImag<T>()));
}


#define MacroAnyNaN(ctype,cls)			\
  case cls: return Tanynan<ctype>(&A);

bool AnyNaN(const Array &A) {
  if (A.isReferenceType()) return false;
  switch (A.dataClass()) {
  default: return false;
    MacroExpandCasesSimple(MacroAnyNaN);
  }
}

template <typename T>
static inline bool Tispositive(const Array *ptr) {
  if (ptr->isScalar()) {
    if (ptr->allReal())
      return (ptr->constRealScalar<T>() >= 0);
    else
      return false;
  } else if (ptr->isSparse())
    return IsPositive(ptr->constRealSparse<T>());
  else
    return IsPositive(ptr->constReal<T>());
}

#define MacroIsPositive(ctype,cls)		\
  case cls: return Tispositive<ctype>(&A);

bool IsPositive(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSigned(MacroIsPositive);
  }
}

#undef MacroIsPositive

template <typename T>
static inline bool Tispositiveornan(const Array *ptr) {
  if (ptr->isScalar()) {
    if (ptr->allReal())
      return (IsNaN(ptr->constRealScalar<T>()) || (ptr->constRealScalar<T>() > 0));
    else
      return false;
  } else if (ptr->isSparse())
    return IsPositiveOrNaN(ptr->constRealSparse<T>());
  else
    return IsPositiveOrNaN(ptr->constReal<T>());
}

#define MacroIsPositiveOrNaN(ctype,cls)		\
  case cls: return Tispositiveornan<ctype>(&A);

bool IsPositiveOrNaN(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSigned(MacroIsPositiveOrNaN);
  }
}


template <typename T>
static inline bool Tisnonnegativeornan(const Array *ptr) {
  if (ptr->isScalar()) {
    if (ptr->allReal())
      return (IsNaN(ptr->constRealScalar<T>()) || (ptr->constRealScalar<T>() >= 0));
    else
      return false;
  } else if (ptr->isSparse())
    return IsNonNegativeOrNaN(ptr->constRealSparse<T>());
  else
    return IsNonNegativeOrNaN(ptr->constReal<T>());
}

#define MacroIsNonNegativeOrNaN(ctype,cls)		\
  case cls: return Tisnonnegativeornan<ctype>(&A);

bool IsNonNegativeOrNaN(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSigned(MacroIsNonNegativeOrNaN);
  }
}


#undef MacroIsPositive


template <typename T>
static inline bool Tissymmetric(const Array *ptr) {
  if (ptr->isScalar())
    return (ptr->constImagScalar<T>() == 0);
  else if (ptr->isSparse())
    if (ptr->allReal())
      return IsSymmetric(ptr->constRealSparse<T>());
    else
      return IsSymmetric(ptr->constRealSparse<T>(),ptr->constImagSparse<T>());
  else 
    if (ptr->allReal())
      return IsSymmetric(ptr->constReal<T>());
    else
      return IsSymmetric(ptr->constReal<T>(),ptr->constImag<T>());
}

#define MacroIsSymmetric(ctype,cls)		\
  case cls: return Tissymmetric<ctype>(&A);

bool IsSymmetric(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSimple(MacroIsSymmetric);
  }
}

#undef MacroIsSymmetric

template <typename T>
static inline Array Tgetdiagonal(const Array *ptr, int diagonal) {
  if (ptr->isScalar())
    if (diagonal == 0) 
      return *ptr;
    else
      return Array(ptr->dataClass());
  else if (ptr->isSparse())
    if (ptr->allReal())
      return Array(GetDiagonal(ptr->constRealSparse<T>(),diagonal));
    else
      return Array(GetDiagonal(ptr->constRealSparse<T>(),diagonal),
		   GetDiagonal(ptr->constImagSparse<T>(),diagonal));
  else
    if (ptr->allReal())
      return Array(GetDiagonal(ptr->constReal<T>(),diagonal));
    else
      return Array(GetDiagonal(ptr->constReal<T>(),diagonal),
		   GetDiagonal(ptr->constImag<T>(),diagonal));
}

#define MacroGetDiagonal(ctype,cls)			\
  case cls: return Tgetdiagonal<ctype>(&a,diagonal);

Array GetDiagonal(const Array &a, int diagonal) {
  switch (a.dataClass()) {
  default: throw Exception("unsupported type for get diagonal");
    MacroExpandCasesSimple(MacroGetDiagonal);
  }
}

#undef MacroGetDiagonal

template <typename T>
static inline Array Tdiagonalarray(const Array *ptr, int diagonal) {
  if (ptr->isSparse())
    if (ptr->allReal())
      return Array(DiagonalArray(ptr->constRealSparse<T>(),diagonal));
    else
      return Array(DiagonalArray(ptr->constRealSparse<T>(),diagonal),
		   DiagonalArray(ptr->constImagSparse<T>(),diagonal));
  else
    if (ptr->allReal())
      return Array(DiagonalArray(ptr->constReal<T>(),diagonal));
    else
      return Array(DiagonalArray(ptr->constReal<T>(),diagonal),
		   DiagonalArray(ptr->constImag<T>(),diagonal));
}

#define MacroDiagonalArray(ctype,cls)			\
  case cls: return Tdiagonalarray<ctype>(&A,diagonal);

Array DiagonalArray(const Array &A, int diagonal) {
  switch (A.dataClass()) {
  default: throw Exception("unsupported type for diagonal array constructor");
    MacroExpandCasesSimple(MacroDiagonalArray);
  }
}

#undef MacroDiagonalArray


// Do a horizontal concatenation of a set of arrays
// Horizontal concatenation means that all dimensions
// except for the number of columns must match.  The
// type logic is the opposite of FreeMat 3.6!  The smallest
// type dominates the type of the output (space-preserving
// as opposed to precision-preserving).  This change is to 
// support the M-lab model.

// This is the generic version that works along any dimension.
// It only works with dense arrays, and not with structure arrays

// How this algorithm is supposed to work:
// We want to concatenate several arrays into a single array.
// [a,b,c,d]
// We want to iterate over the output array, and select which
// array an element belongs to - this
// adjusted is the index 
//
//

// #  Wes Campaigne replied on June 5th, 2007 at 11:57 pm :

// How does cat choose the dominant class, i.e., the class of its return value?

// A quick experiment with the basic types seems to reveal a descending priority of:
// char
// integer types (if there are multiple different integer-typed operands, the type of the left-most of these is dominant)
// single
// double
// logical

// Appropriate warnings are given most of the time, but truncation of floats to char doesnt throw any. And promotion of one integer type (or logicals) to a bigger integer type DOES throw a warning, even though its unnecessary.

// Is there a particular reasoning behind all this behaviour? I havent noticed it documented anywhere.

// Combining Unlike Classes

// Matrices and arrays can be composed of elements of most any MATLAB data type as long as all elements in the matrix are of the same type. If you do include elements of unlike classes when constructing a matrix, MATLAB converts some elements so that all elements of the resulting matrix are of the same type. (See Built-In Classes (Data Types) for information on any of the MATLAB classes discussed here.)

// Data type conversion is done with respect to a preset precedence of classes. The following table shows the five classes you can concatenate with an unlike type without generating an error (that is, with the exception of character and logical).

// Table is usual C,I,S,D,L, with C-L combination made illegal

// For example, concatenating a double and single matrix always yields a matrix of type single. MATLAB converts the double element to single to accomplish this.
// Combining Unlike Integer Types

// If you combine different integer types in a matrix (e.g., signed with unsigned, or 8-bit integers with 16-bit integers), MATLAB returns a matrix in which all elements are of one common type. MATLAB sets all elements of the resulting matrix to the data type of the left-most element in the input matrix. For example, the result of the following concatenation is a vector of three 16-bit signed integers:

// A = [int16(450) uint8(250) int32(1000000)]

// MATLAB also displays a warning to inform you that the result may not be what you had expected:

// A = [int16(450) uint8(250) int32(1000000)];
// Warning: Concatenation with dominant (left-most) integer class 
// may overflow other operands on conversion to return class.

// You can disable this warning by entering the following two commands directly after the operation that caused the warning. The first command retrieves the message identifier associated with the most recent warning issued by MATLAB. The second command uses this identifier to disable any further warnings of that type from being issued:

// [msg, intcat_msgid] = lastwarn;
// warning('off', intcat_msgid);

// To reenable the warning so that it will now be displayed, use

// warning('on', intcat_msgid);

// You can use these commands to disable or enable the display of any MATLAB warning.

// Example of Combining Unlike Integer Sizes.   After disabling the integer concatenation warnings as shown above, concatenate the following two numbers once, and then switch their order. The return value depends on the order in which the integers are concatenated. The left-most type determines the data type for all elements in the vector:

// A = [int16(5000) int8(50)]
// A =
//    5000   50

// B = [int8(50) int16(5000)]
// B =
//    50   127

// The first operation returns a vector of 16-bit integers. The second returns a vector of 8-bit integers. The element int16(5000) is set to 127, the maximum value for an 8-bit signed integer.

// The same rules apply to vertical concatenation:

// C = [int8(50); int16(5000)]
// C =
//     50
//    127

//       Note   You can find the maximum or minimum values for any MATLAB integer type using the intmax and intmin functions. For floating-point types, use realmax and realmin.

// Example of Combining Signed with Unsigned.   Now do the same exercise with signed and unsigned integers. Again, the left-most element determines the data type for all elements in the resulting matrix:

// A = [int8(-100) uint8(100)]
// A =
//    -100   100

// B = [uint8(100) int8(-100)]
// B =
//    100   0

// The element int8(-100) is set to zero because it is no longer signed.

// MATLAB evaluates each element prior to concatenating them into a combined array. In other words, the following statement evaluates to an 8-bit signed integer (equal to 50) and an 8-bit unsigned integer (unsigned -50 is set to zero) before the two elements are combined. Following the concatenation, the second element retains its zero value but takes on the unsigned int8 type:

// A = [int8(50), uint8(-50)]
// A =
//   50    0

// Combining Integer and Noninteger Data

// If you combine integers with double, single, or logical classes, all elements of the resulting matrix are given the data type of the left-most integer. For example, all elements of the following vector are set to int32:

// A = [true pi int32(1000000) single(17.32) uint8(250)]

// Empty Matrices

// If you construct a matrix using empty matrix elements, the empty matrices are ignored in the resulting matrix:

// A = [5.36; 7.01; []; 9.44]
// A =
//     5.3600
//     7.0100
//     9.4400

// Concatenation Examples

// Here are some examples of data type conversion during matrix construction.

// Combining Single and Double Types.   Combining single values with double values yields a single matrix. Note that 5.73*10^300 is too big to be stored as a single, thus the conversion from double to single sets it to infinity. (The class function used in this example returns the data type for the input value):

// x = [single(4.5) single(-2.8) pi 5.73*10^300]
// x =
//     4.5000   -2.8000    3.1416       Inf

// class(x)              % Display the data type of x
// ans =
//    single

// Combining Integer and Double Types.   Combining integer values with double values yields an integer matrix. Note that the fractional part of pi is rounded to the nearest integer. (The int8 function used in this example converts its numeric argument to an 8-bit integer):

// x = [int8(21) int8(-22) int8(23) pi 45/6]
// x =
//     21   -22    23     3     7

// class(x)
// ans =
//    int8

// Combining Character and Double Types.   Combining character values with double values yields a character matrix. MATLAB converts the double elements in this example to their character equivalents:

// x = ['A' 'B' 'C' 68 69 70]
// x =
//    ABCDEF

// class(x)
// ans =
//    char

// Combining Logical and Double Types.   Combining logical values with double values yields a double matrix. MATLAB converts the logical true and false elements in this example to double:

// x = [true false false pi sqrt(7)]
// x =
//     1.0000         0         0    3.1416    2.6458

// class(x)
// ans =
//    double

static bool AnyOfType(const ArrayVector& pdata, DataClass t) {
  for (int i=0;i<pdata.size();i++)
    if (pdata[i].dataClass() == t) return true;
  return false;
}

static DataClass ComputeCatType(const ArrayVector& pdata) {
  if (AnyOfType(pdata,CellArray))
    return CellArray;
  if (AnyOfType(pdata,Struct))
    return Struct;
  if (AnyOfType(pdata,StringArray))
    return StringArray;
  // Check for an integer type - choose the first one found.
  for (int i=0;i<pdata.size();i++) {
    if (pdata[i].dataClass() == Int8 ||
	pdata[i].dataClass() == UInt8 ||
	pdata[i].dataClass() == Int16 ||
	pdata[i].dataClass() == UInt16 ||
	pdata[i].dataClass() == Int32 ||
	pdata[i].dataClass() == UInt32 ||
	pdata[i].dataClass() == Int64 ||
	pdata[i].dataClass() == UInt64)
      return pdata[i].dataClass();
  }
  // So there are no integers, no cell, struct, strings
  // All that is left is logical, float, double.  Next we 
  // look for floats, then doubles, and last, bools
  if (AnyOfType(pdata,Float)) return Float;
  if (AnyOfType(pdata,Double)) return Double;
  return Bool;
}


// FIX it here... [0 0] [3 0 4] --> [3 0 4]
Array NCat(const ArrayVector& pdata, int catdim) {
  // Must have something to work with
  if (pdata.size() == 0) return EmptyConstructor();
  // First pass, compute the output class
  DataClass cls = ComputeCatType(pdata);
  // Check for the case of concatenation of user defined classes
  bool userClassCase = false;
  QString classname;
  if ((cls == Struct) && pdata[0].isUserClass()) {
    classname = pdata[0].className();
    for (int i=1;i<pdata.size();i++)
      if (!pdata[i].isUserClass() || pdata[i].className() != classname)
	throw Exception("Cannot concatenate user defined classes of different type");
    userClassCase = true;
  }
  // Next pass, if we are in the case of a cell array, we convert the arrays to cells
  // We also strip the empties
  ArrayVector data;
  for (int i=0;i<pdata.size();i++) {
    if ((pdata[i].dimensions() != NTuple(0,0)) &&
        (pdata[i].dimensions() != NTuple(1,0)) &&
        (pdata[i].dimensions() != NTuple(0,1))   ) {
      if (cls == CellArray) {
	if (pdata[i].dataClass() == CellArray)
	  data.push_back(pdata[i]);
	else
	  data.push_back(CellArrayFromArray(pdata[i]));
      } else 
	data.push_back(pdata[i]);
    }
  }
  // Next step, we compute the output dimensions
  NTuple outdims;
  // Compute the output dimensions and validate each of the elements
  bool foundNonzero = false;
  for (int i=0;i<data.size();i++) {
    NTuple eldims(data[i].dimensions());
    if (eldims != NTuple(0,0)) {
      if (!foundNonzero) {
	foundNonzero = true;
	outdims = eldims;
      } else {
	for (int j=0;j<NDims;j++)
	  if ((eldims[j] != outdims[j]) && (j != catdim))
	    throw Exception(QString("Mismatch in dimension %1 for elements being concatenated along dimension %2").arg(j+1).arg(catdim+1));
	outdims[catdim] += eldims[catdim];
      }
    }
  }
  if (!foundNonzero) outdims = NTuple(0,0);
  // Compute the output dataclass
  if (data.size() == 0) return Array(cls,outdims);
  Array retval(cls,outdims);
  ArrayIterator iter(&retval,catdim);
  while (iter.isValid()) {
    int selector = 0;
    index_t adjusted = 1;
    for (index_t i=1;i<=iter.size();i++) {
      while (adjusted > data[selector].dimensions()[catdim]) {
	adjusted -= data[selector].dimensions()[catdim];
	selector++;
      }
      iter.set(data[selector].get(iter.pos().replace(catdim,adjusted)).toClass(cls));
      adjusted++;
      iter.next();
    }
    iter.nextSlice();
  }
  if (userClassCase)
    retval.structPtr().setClassName(classname);
  return retval;
}

Array HCat(const ArrayVector& data) {
  return NCat(data,1);
}

Array VCat(const ArrayVector& data) {
  return NCat(data,0);
}

Array MatrixConstructor(const ArrayMatrix& data) {
  ArrayVector hcatresults;
  for (int i=0;i<data.size();i++)
    hcatresults.push_back(HCat(data[i]));
  return VCat(hcatresults);
}

Array CellConstructor(const ArrayMatrix& data) {
  ArrayVector hcatresults;
  for (int i=0;i<data.size();i++) 
    hcatresults.push_back(CellConstructor(data[i]));
  return VCat(hcatresults);
}

Array StructConstructor(const StringVector& fnames, const ArrayVector& values) {
  if (fnames.size() != values.size())
    throw Exception("Number of field names must match number of values in structure constructor.");
  /**
   * First, we have to make sure that each entry of "values" have 
   *  1.  cell arrays of the same size,
   *  2.  single element cell arrays,
   *  3.  single values.
   */
  NTuple dims;
  bool nonSingularFound = false;
  for (int i=0;i<values.size();i++) {
    /**
     * Check the type of the entry.  If its a non-cell array, then
     * then ignore this entry.
     */
    if (values[i].dataClass() == CellArray) {
      /**
       * This is a cell-array, so look for non-scalar cell-arrays.
       */
      if (!values[i].isScalar()) {
	if (!nonSingularFound) {
	  nonSingularFound = true;
	  dims = values[i].dimensions();
	} else
	  if (dims != values[i].dimensions())
	    throw Exception("Array dimensions of non-scalar entries must agree in structure construction.");
      }
    }
  }
  /**
   * At this point we can construct the dimensions of the output.
   */
  if (!nonSingularFound) {
    dims = NTuple(1,1);
  }
  /**
   * The dimensions of the object have been identified.  Set the
   * dimensions of the object and the field names.  Then allocate
   * the space.
   */
  StructArray qp;
  for (int i=0;i<fnames.size();i++) {
    BasicArray<Array> rp(dims);
    Array rval = values[i];
    if (!rval.isEmpty() && rval.isSparse())
      throw Exception("Sparse arrays not supported for struct constructor.");
    if (rval.isEmpty())
      rp.fill(rval);
    else {
      if (rval.dataClass() == CellArray) {
	if (rval.isScalar())
	  rp.fill(ArrayFromCellArray(rval));
	else
	  rp = rval.constReal<Array>();
      } else
	rp.fill(rval);
    }
    qp.insert(fnames[i],rp);
  }
  qp.updateDims();
  return Array(qp);
}

template <typename T>
static bool RealAllZeros(const Array &t) {
  if (t.isScalar()) 
    return (t.constRealScalar<T>() == 0);
  else if (t.isSparse())
    return AllZeros(t.constRealSparse<T>());
  else
    return AllZeros(t.constReal<T>());
}

#define MacroRealZeros(ctype,cls)		\
  case cls: return RealAllZeros<ctype>(t);

bool RealAllZeros(const Array &t) {
  switch (t.dataClass()) {
  default: throw Exception("Unhandled case -- argument must be numeric");
    MacroExpandCasesSimple(MacroRealZeros);
  }
}

template <typename T>
static bool RealAllNonZeros(const Array &t) {
  if (t.isEmpty()) return false;
  if (t.isScalar()) 
    return (t.constRealScalar<T>() != 0);
  else if (t.isSparse())
    return AllNonZeros(t.asDenseArray().constReal<T>());
  else
    return AllNonZeros(t.constReal<T>());
}

#define MacroRealNonZeros(ctype,cls)		\
  case cls: return RealAllNonZeros<ctype>(t);

bool RealAllNonZeros(const Array &t) {
  switch (t.dataClass()) {
  default: throw Exception("Unhandled case -- argument must be numeric");
    MacroExpandCasesSimple(MacroRealNonZeros);
  }
}

template <typename T>
static Array TPermute(const Array &x, const NTuple &dp) {
  if (x.allReal()) 
    return Array(Permute(x.constReal<T>(),dp));
  else 
    return Array(Permute(x.constReal<T>(),dp),Permute(x.constImag<T>(),dp));
}

static Array StructPermute(const Array &x, const NTuple &dp) {
  const StructArray &sp(x.constStructPtr());
  StructArray ret;
  for (int i=0;i<sp.fieldCount();i++)
    ret.insert(sp.fieldName(i),Permute(sp[i],dp));
  ret.updateDims();
  return Array(ret);
}

#define MacroPermute(ctype,cls)			\
  case cls: return TPermute<ctype>(A,dp);

Array Permute(const Array &A, const NTuple &dp) {
  // Check for an identity permutation
  NTuple outdims(A.dimensions().permute(dp));
  bool id_perm = true;
  for (int i=0;i<NDims;i++)
    if (dp[i] != (i+1)) id_perm = false;
  if (id_perm) return A;
  if (A.is2D()) {
    bool isTranspose = true;
    for (int  i=2;i<NDims;i++)
      if (dp[i] != (i+1)) isTranspose = false;
    if (dp[0] != 2) isTranspose = false;
    if (dp[1] != 1) isTranspose = false;
    if (isTranspose) return Transpose(A);
  }
  if (A.isSparse()) throw Exception("illegal permutation for sparse arrays");
  if (A.isScalar()) return A;
  if (A.isEmpty()) return Array(A.dataClass(),outdims);
  switch (A.dataClass()) {
  default: throw Exception("Unsupported dataclass for permute");
  case Struct:
    return StructPermute(A,dp);
    MacroExpandCasesAll(MacroPermute);
  }
}

// s is the switch value to test, x is the values to test against
bool TestForCaseMatch(const Array &s, const Array &x) {
  if (!(s.isScalar() || s.isString()))
    throw Exception("Switch argument must be a scalar or a string");
  // And we had better not be a reference type
  if (s.isReferenceType())
    throw Exception("Switch argument cannot be a reference type (struct or cell array)");
  // If x is a scalar, we just need to call the scalar version
  if (((x.dataClass() != CellArray) && x.isScalar()) || x.isString())
    return (s.toClass(Double) == x.toClass(Double));
  const BasicArray<Array> &xp(x.constReal<Array>());
  for (index_t i=1;i<=xp.length();i++) 
    if (s == xp[i]) return true;
  return false;
}

static BasicArray<index_t> DecompressCCSCols(const BasicArray<index_t> &colstart, index_t len) {
  BasicArray<index_t> retval(NTuple(1,len));
  index_t p=1;
  for (index_t i=2;i<=colstart.length();i++)
    for (index_t j=1;j<=(colstart[i]-colstart[i-1]);j++) {
      retval[p] = index_t(i-2);
      p = p + 1;
    }
  return retval;
}

template <typename T>
static SparseMatrix<T> CCSToSparse(const BasicArray<index_t> &rowstart,
				   const BasicArray<index_t> &colstart,
				   const BasicArray<T> &Adata) {
  BasicArray<index_t> cols(DecompressCCSCols(colstart,Adata.length()));
  SparseMatrix<T> retvec(NTuple(MaxValue(rowstart)+1,MaxValue(cols)+1));
  for (index_t i=1;i<=cols.length();i++) 
    retvec[NTuple(rowstart[i]+1,cols[i]+1)] = Adata[i];
  return retvec;
}

template <typename T>
static Array IJVToSparse(const BasicArray<index_t> &irp,
			 const BasicArray<index_t> &jcp,
			 const Array &pr, const Array &pi, 
			 bool complexFlag) {
  if (!complexFlag) {
    const Array &pr_dense(pr.asDenseArray());
    return Array(CCSToSparse(irp,jcp,pr_dense.constReal<T>()));
  }
  else {
    const Array &pr_dense(pr.asDenseArray());
    const Array &pi_dense(pi.asDenseArray());
    return Array(CCSToSparse(irp,jcp,pr_dense.constReal<T>()),
		 CCSToSparse(irp,jcp,pi_dense.constImag<T>()));
  }
}

#define MacroIJVToSparse(ctype,cls)					\
  case cls: return IJVToSparse<ctype>(irp,jcp,pr,pi,complexFlag);

Array MatIJVToSparse(const Array &ir, const Array &jc,
		     const Array &pr, const Array &pi, bool complexFlag) {
  const BasicArray<index_t> &irp(ir.constReal<index_t>());
  const BasicArray<index_t> &jcp(jc.constReal<index_t>());
  switch(pr.dataClass()) {
  default: throw Exception("Unsupported data type " + pr.className() + " in sparse constructor");
    MacroExpandCasesSimple(MacroIJVToSparse);
  }
}

template <typename T>
void SparseToIJVReal(const SparseMatrix<T> &x, Array &rows, Array &cols, Array &vals) {
  QVector<uint32> rowstart;
  QVector<uint32> colstart;
  QVector<T> xdata;
  SparseToCCS(x,rowstart,colstart,xdata);
  rows = Array(ToBasicArray(rowstart)).toClass(Index);
  cols = Array(ToBasicArray(colstart)).toClass(Index);
  vals = Array(ToBasicArray(xdata));
}

template <typename T>
void SparseToIJVComplex(const SparseMatrix<T> &xr, const SparseMatrix<T> &xi,
			Array &rows, Array &cols, Array &vals) {
  QVector<uint32> rowstart;
  QVector<uint32> colstart;
  QVector<T> xdata_real;
  QVector<T> xdata_imag;
  SparseToCCS(xr,rowstart,colstart,xdata_real);
  SparseToCCS(xi,rowstart,colstart,xdata_imag);
  rows = Array(ToBasicArray(rowstart)).toClass(Index);
  cols = Array(ToBasicArray(colstart)).toClass(Index);
  vals = Array(ToBasicArray(xdata_real),ToBasicArray(xdata_imag));
}

template <typename T>
void SparseToIJV(const Array &x, Array &rows, Array &cols, Array &vals) {
  if (x.allReal())
    SparseToIJVReal(x.constRealSparse<T>(),rows,cols,vals);
  else
    SparseToIJVComplex(x.constRealSparse<T>(),x.constImagSparse<T>(),rows,cols,vals);
}

#define MacroSparseToIJV(ctype,cls)				\
  case cls: return SparseToIJV<ctype>(x,rows,cols,vals);

void SparseToIJVMat(const Array &x, Array &rows, Array &cols, Array &vals) {
  if (!x.isSparse()) throw Exception("x must be sparse");
  switch (x.dataClass()) {
  default: throw Exception("unhandled case");
    MacroExpandCasesSimple(MacroSparseToIJV);
  }
}

#undef MacroSparseToIJV

template <typename T>
Array ToSparse(const Array &data) {
  if (data.allReal())
    return Array(ToRealSparse<T>(data));
  else
    return Array(ToRealSparse<T>(data),ToImagSparse<T>(data));
}

#define MacroToSparse(ctype,cls)		\
  case cls: return ToSparse<ctype>(data);

Array ToSparse(const Array& data) {
  switch (data.dataClass()) {
  default: throw Exception("unhandled case for sparse");
    MacroExpandCases(MacroToSparse);
  }
}

#undef MacroToSparse

bool Equals(const StringVector& a, const StringVector& b) {
  if (a.size() != b.size()) return false;
  for (int i=0;i<a.size();i++)
    if (a[i] != b[i]) return false;
  return true;
}
