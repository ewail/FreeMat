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
#ifndef __BasicArray_hpp__
#define __BasicArray_hpp__

#include <QVector>
#include "CommonIterator.hpp"
#include "NTuple.hpp"
#include <QDebug>
#include "Types.hpp"
#include "FastList.hpp"
#include "Cast.hpp"
#include "IEEEFP.hpp"
#include "Vector.hpp"

template <typename T>
class BasicArray;

typedef BasicArray<index_t> IndexArray;
typedef FastList<IndexArray> IndexArrayVector;
BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length);

bool IsColonOp(const IndexArray& ndx);
bool DimensionCovered(const IndexArray& ndx, index_t length);

template <typename T>
class BasicIterator;

template <typename T>
class ConstBasicIterator;

template <typename T>
class BasicArray {
  Vector<T> m_data;
  NTuple m_dims;
  index_t m_offset;
  index_t m_count;
public:
  BasicArray() : m_data(), m_dims(0,0), m_offset(0), m_count(0) {}
  explicit BasicArray(const NTuple& dim) : 
    m_data(size_t(dim.count())), m_dims(dim), m_offset(0), m_count(dim.count()) {
  }
  explicit BasicArray(T val) : m_data(1) {
    m_data[0] = val;
    m_dims = NTuple(1,1); 
    m_offset = 0;
    m_count = 1;
  }
  ~BasicArray() {}
  inline index_t offset() const {return m_offset;}
  inline const NTuple dimensions() const {return m_dims;}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t columns() const {return m_dims.cols();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t length() const {return m_count;}
  inline index_t isScalar() const {return length() == 1;}
  inline bool is2D() const {return m_dims.is2D();}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[(size_t)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[(size_t)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T operator[](index_t pos) const {
    if ((pos > 0) && (pos <= length()))
      return m_data[(size_t)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](index_t pos) {
    if ((pos > 0) && (pos <= length()))
      return m_data[(size_t)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T* constData() const {
    if (length() > 0)
      return m_data.data() + size_t(m_offset);
    else
      return NULL;
  }
  inline T* data() {
    if (length() > 0)
      return &(m_data[size_t(m_offset)]);
    else
      return NULL;
  }
  void fill(T val) {
    for (index_t i=1;i<=length();i++) 
      m_data[(size_t)(i+m_offset-1)] = val;
  }
  bool operator==(const BasicArray<T>& b) const {
    for (index_t i=1;i<=b.length();i++) 
      if (get(i) != b.get(i)) return false;
    return true;
  }
  inline void set(const NTuple& pos, const T& val) {
    if (!(pos <= dimensions())) resize(max(dimensions(),pos));
    m_data[(size_t)(m_dims.map(pos)+m_offset-1)] = val;
  }
  inline void set(index_t pos, const T& val) {
    if (m_count < pos) resize(pos);
    m_data[(size_t)(pos+m_offset-1)] = val;
  }
  inline void setNoBoundsCheck(index_t pos, const T& val) {
    m_data.data()[(size_t)(pos+m_offset-1)] = val;
  }
  inline const T get(index_t pos) const {
    if ((pos<1) || (pos>length())) 
      throw Exception("index is out of bounds");
    return m_data[(size_t)(pos+m_offset-1)];
  }
  inline const T get(const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[(size_t)(m_dims.map(pos)+m_offset-1)];
    else
      throw Exception("index is out of bounds");
  }
  inline const T getNoBoundsCheck(index_t pos) const {
    return m_data.data()[(size_t)(pos+m_offset-1)];    
  }
  BasicArray<T> slice(const IndexArrayVector& index) const {
    index_t offset = getSliceIndex(dimensions(),index);
    BasicArray<T> retvec;
    retvec.m_dims = NTuple(dimensions()[0],1);
    retvec.m_offset = m_offset + offset;
    retvec.m_data = m_data;
    retvec.m_count = retvec.m_dims.count();
    return retvec;
  }
  BasicArray<T> slice(const NTuple& pos) const {
    index_t offset = dimensions().map(pos)-1;
    BasicArray<T> retvec;
    retvec.m_dims = NTuple(dimensions()[0],1);
    retvec.m_offset = m_offset + offset;
    retvec.m_data = m_data;
    retvec.m_count = retvec.m_dims.count();
    return retvec;
  }
  void del(const IndexArrayVector& index) {
    // The strategy for dealing with deletions is simplified relative
    // to 3.x code.  An NDim deletion is only valid if there is one
    // dimension that is not covered.
    QVector<bool> covered(index.size());
    for (int i=0;i<index.size();i++) {
      index_t dimLen = dimensions()[i];
      covered[i] = (IsColonOp(index[i]) || DimensionCovered(index[i],dimLen));
    }
    // Count the number of uncovered dimensions
    int uncovered_count = 0;
    int first_uncovered = 0;
    for (int i=0;i<index.size();i++)
      if (!covered[i]) {
	first_uncovered = i;
	uncovered_count++;
      }
    if (uncovered_count == 0) {
      m_data.clear();
      m_dims = NTuple(0,0);
      m_offset = 0;
      m_count = 0;
      return;
    }
    if (uncovered_count > 1)
      throw Exception("Deletion A(:,...,:) = [] cannot have more than one non-singular dimension");
    index_t dimLen = dimensions()[first_uncovered];
    BasicArray<bool> map(GetDeletionMap(index[first_uncovered],dimLen));
    int newSize = 0;
    for (index_t i=1;i<=map.length();i++)
      if (!map.get(i)) newSize++;
    // Create an output array..
    NTuple outdims(dimensions());
    outdims[first_uncovered] = newSize;
    BasicArray<T> retvec(outdims);
    ConstBasicIterator<T> source(this,first_uncovered);
    BasicIterator<T> dest(&retvec,first_uncovered);
    while (source.isValid() && dest.isValid()) {
      for (index_t i=1;i<=dimLen;i++) {
	if (!map.get(i)) {
	  dest.set(source.get());
	  dest.next();
	}
	source.next();
      }
      source.nextSlice();
      dest.nextSlice();
    }
    m_data = retvec.m_data;
    m_offset = retvec.m_offset;
    m_dims = retvec.m_dims;
    m_count = m_dims.count();
  }
  void del(const IndexArray& index) {
    if (IsColonOp(index)) {
      m_data.clear();
      m_dims = NTuple(0,0);
      m_offset = 0;
      m_count = m_dims.count();
      return;
    }
    BasicArray<bool> map(GetDeletionMap(index,length()));
    index_t newSize = 0;
    for (index_t i=1;i<=map.length();i++)
      if (!map[i]) newSize++;
    if (newSize == length()) return;
    NTuple newDim;
    if (isRowVector())
      newDim = NTuple(1,newSize);
    else
      newDim = NTuple(newSize,1);
    Vector<T> rdata((size_t)newSize);
    int j=0;
    for (index_t i=1;i<=map.length();i++)
      if (!map[i]) rdata[j++] = get(i);
    m_data = rdata;
    m_offset = 0;
    m_dims = newDim;
    m_count = m_dims.count();
  }
  void resize(const NTuple& pos) {
    if (pos.extends(m_dims))
      {
	m_dims = pos;
	m_count = pos.count();
	m_data.resize(pos.count());
	return;
      }
    BasicArray<T> retval(pos);
    Transformer<BasicArray<T>,T> transform(&retval,this);
    while (transform.isValid()) {
      for (int i=0;i<transform.size();i++) {
	transform.set(transform.get());
	transform.next();
      }
      transform.nextSlice();
    }
    *this = retval;
  }
  void resize(index_t len) {
    if (len > length()) {
      NTuple newDim;
      if (isEmpty() || isScalar()) {
	newDim = NTuple(1,len);
      } else if (isVector()) {
	if (rows() != 1)
	  newDim = NTuple(len,1);
	else
	  newDim = NTuple(1,len);
      } else {
	m_dims = NTuple(1,length());
	newDim = NTuple(1,len);
      }
      resize(newDim);
    }
    m_count = m_dims.count();
  }
  void printMe(std::ostream& o) const {
    o << dimensions() << "[";
    for (index_t i=1;i<=length();i++)
      o << get(i) << " ";
    o << "]\n";
  }
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
  inline index_t bytes() const {
    if (sizeof(T) <= 8)
      return sizeof(T)*length();
    index_t total = 0;
    for (index_t i=1;i<=length();i++)
      total += TSizeOf<T>(get(i));
    return total;
  }
};

template <typename T>
BasicArray<T> Uniform(const NTuple &dims, T val) {
  BasicArray<T> retval(dims);
  retval.fill(val);
  return retval;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const BasicArray<T>& arg) {
  arg.printMe(o);
  return o;
}

template <typename T>
BasicArray<T> MergeComplex(const BasicArray<T>& real, const BasicArray<T>& imag) {
  NTuple retdim(real.dimensions());
  if (retdim != imag.dimensions()) 
    throw Exception("Cannot merge arrays of unequal size into complex array");
  retdim[0] = retdim[0]*2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i<=real.length();i++) {
    retval.setNoBoundsCheck(2*i-1,real.getNoBoundsCheck(i));
    retval.setNoBoundsCheck(2*i,imag.getNoBoundsCheck(i));
  }
  return retval;
}

template <typename T>
BasicArray<T> MergeComplex(const BasicArray<T>& real) {
  NTuple retdim(real.dimensions());
  retdim[0] = retdim[0]*2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i<=real.length();i++)
    retval.setNoBoundsCheck(2*i-1,real.getNoBoundsCheck(i));
  return retval;
}

template <typename T>
BasicArray<T> SplitReal(const BasicArray<T>& A) {
  NTuple retdim(A.dimensions());
  retdim[0] = retdim[0]/2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i<=retval.length();i++) 
    retval.setNoBoundsCheck(i,A.getNoBoundsCheck(2*i-1));
  return retval;
}

template <typename T>
BasicArray<T> SplitImag(const BasicArray<T>& A) {
  NTuple retdim(A.dimensions());
  retdim[0] = retdim[0]/2;
  BasicArray<T> retval(retdim);
  for (index_t i=1;i<=retval.length();i++) 
    retval.setNoBoundsCheck(i,A.getNoBoundsCheck(2*i));
  return retval;
}

template <typename T>
bool MergedArrayHasComplexComponents(const BasicArray<T>& arg) {
  for (index_t i=2;i<=arg.length();i+=2) 
    if (arg.getNoBoundsCheck(i) != 0) return true;
  return false;
}

template <typename T>
BasicArray<T> Apply(const BasicArray<T>& arg, T (*func)(T)) {
  BasicArray<T> retval(arg.dimensions());
  Transformer<BasicArray<T>,T> transform(&retval,&arg);
  while (transform.isValid()) {
    for (int i=0;i<transform.size();i++) {
      transform.set(func(transform.get()));
      transform.next();
    }
    transform.nextSlice();
  }
  return retval;
}

template <typename T>
BasicArray<T> Transpose(const BasicArray<T>& arg) {
  if (!arg.dimensions().is2D())
    throw Exception("Cannot transpose multi-dimensional arrays");
  BasicArray<T> retval(NTuple(arg.cols(),arg.rows()));
  Transformer<BasicArray<T>,T> transform(&retval,&arg,0,1);
  while (transform.isValid()) {
    for (int i=0;i<transform.size();i++) {
      transform.set(transform.get());
      transform.next();
    }
    transform.nextSlice();
  }
  return retval;
}

template <typename T>
static inline T neg(T v) { return -v;}

template <typename T>
BasicArray<T> Negate(const BasicArray<T>& arg) {
  return Apply<T>(arg,neg);
}

template <typename T>
bool IsPositive(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++) 
    if (arg.getNoBoundsCheck(i) <= 0) return false;
  return true;
}

template <typename T>
bool IsPositiveOrNaN(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++) 
    if (!IsNaN(i) && (arg.getNoBoundsCheck(i) <= 0)) return false;
  return true;
}

template <typename T>
bool IsNonNegative(const BasicArray<T> &arg) {
  for (index_t i=1;i<=arg.length();i++)
    if (!IsNonNegative(arg.getNoBoundsCheck(i))) return false;
  return true;
}

template <typename T>
bool IsNonNegativeOrNaN(const BasicArray<T> &arg) {
  for (index_t i=1;i<=arg.length();i++)
    if (!IsNaN(i) && (arg.getNoBoundsCheck(i) < 0)) return false;
  return true;
}

template <typename T>
bool IsInteger(const BasicArray<T> &arg) {
  for (index_t i=1;i<=arg.length();++i)
    if (!IsInteger(arg.getNoBoundsCheck(i))) return false;
  return true;
}

template <typename T>
bool IsSymmetric(const BasicArray<T>& arg) {
  if (!arg.dimensions().is2D()) 
    throw Exception("Symmetry check not valid for N-dimensional arrays");
  for (index_t i=1;i<=arg.cols();i++) 
    for (index_t j=i+1;j<=arg.rows();j++) 
      if (arg[NTuple(i,j)] != arg[NTuple(j,i)])
	return false;
  return true;
}

template <typename T>
bool IsSymmetric(const BasicArray<T>& arg, const BasicArray<T>& img) {
  if (!arg.dimensions().is2D() || !img.dimensions().is2D())
    throw Exception("Symmetry check not valid for N-dimensional arrays");
  if (arg.dimensions() != img.dimensions())
    throw Exception("Symmetry check requires both arrays must be the same size");
  for (index_t i=1;i<=arg.cols();i++) 
    for (index_t j=i;j<=arg.rows();j++) 
      if ((arg[NTuple(i,j)] != arg[NTuple(j,i)]) ||
	  (img[NTuple(i,j)] != -img[NTuple(j,i)]))
	return false;
  return true;
}

template <typename T>
BasicArray<T> GetDiagonal(const BasicArray<T>& arg, int diagonal) {
  if (!arg.dimensions().is2D())
    throw Exception("Cannot get diagonal for N-dimensional arrays");
  index_t outLen;
  if (diagonal < 0) {
    outLen = qMax(index_t(0),qMin(arg.rows()+diagonal,arg.cols()));
    if (outLen == 0) return BasicArray<T>();
    BasicArray<T> retvec(NTuple(outLen,1));
    for (index_t i=1;i<=outLen;i++)
      retvec[i] = arg[NTuple(i-diagonal,i)];
    return retvec;
  } else {
    outLen = qMax(index_t(0),qMin(arg.rows(),arg.cols()-diagonal));
    if (outLen == 0) return BasicArray<T>();
    BasicArray<T> retvec(NTuple(outLen,1));
    for (index_t i=1;i<=outLen;i++)
      retvec[i] = arg[NTuple(i,i+diagonal)];
    return retvec;
  }
}

template <typename T>
BasicArray<T> DiagonalArray(const BasicArray<T> &arg, int diagonal) {
  index_t outLen = arg.length();
  index_t M = outLen + abs(diagonal);
  BasicArray<T> retval(NTuple(M,M));
  if (diagonal < 0) {
    for (index_t i=1;i<=outLen;i++)
      retval[NTuple(i-diagonal,i)] = arg[i];
  } else {
    for (index_t i=1;i<=outLen;i++)
      retval[NTuple(i,i+diagonal)] = arg[i];
  }
  return retval;  
}

template <typename T>
BasicArray<T> Permute(const BasicArray<T>& arg, NTuple perm) {
  BasicArray<T> retval(arg.dimensions().permute(perm));
  ConstBasicIterator<T> iter(&arg,0);
  while (iter.isValid()) {
    for (index_t i=1;i<=iter.size();i++) {
      retval[iter.pos().replace(0,i).permute(perm)] = iter.get();
      iter.next();
    }
    iter.nextSlice();
  }
  return retval;
}

template <typename T>
bool AllZeros(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++)
    if (arg.getNoBoundsCheck(i) != T(0)) return false;
  return true;
}

template <typename T>
bool AllNonZeros(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++)
    if (arg.getNoBoundsCheck(i) == T(0)) return false;
  return true;
}

template <typename T>
bool AnyNotFinite(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++) 
    if (!IsFinite(arg.getNoBoundsCheck(i))) return true;
  return false;
}

template <typename T>
bool AnyNaN(const BasicArray<T>& arg) {
  for (index_t i=1;i<=arg.length();i++) 
    if (IsNaN(arg.getNoBoundsCheck(i))) return true;
  return false;
}

template <typename T>
T MinValue(const BasicArray<T>& arg) {
  if (arg.isEmpty())
    throw Exception("Cannot use this min function on empty arrays.");
  T retval = arg[1];
  for (int i=2;i<=arg.length();i++)
    if (retval > arg[i])
      retval = arg[i];
  return retval;
}

template <typename T>
T MaxValue(const BasicArray<T>& arg) {
  if (arg.isEmpty()) 
    throw Exception("Cannot use this max function on empty arrays.");
  T retval = arg[1];
  for (int i=2;i<=arg.length();i++)
    if (retval < arg[i])
      retval = arg[i];
  return retval;
}

template <typename T>
IndexArray Find(const BasicArray<T>& vec) {
  index_t count = 0;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) count++;
  IndexArray retvec(NTuple(1,count));
  count = 1;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) {
      retvec.setNoBoundsCheck(count,i);
      count++;
    }
  return retvec;
}

inline bool AllTrue(const BasicArray<bool>& arg) {
  for (int i=1;i<=arg.length();i++)
    if (!arg.getNoBoundsCheck(i)) return false;
  return true;
}


template <typename T, typename S>
BasicArray<T> ConvertBasicArray(const BasicArray<S>& source) {
  BasicArray<T> dest(source.dimensions());
  T* ret = dest.data();
  const S* src = source.constData();
  size_t len = size_t(source.length());
  for (size_t i=0;i<len;i++)
    ret[i] = CastConvert<T,S>(src[i]);
  return dest;
}

template <typename S>
class BasicIterator : public BaseIterator<BasicArray<S>,S> {
public:
  BasicIterator(BasicArray<S> *ptr, int dim) : 
    BaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

template <typename S>
class ConstBasicIterator : public ConstBaseIterator<BasicArray<S>,S> {
public:
  ConstBasicIterator(const BasicArray<S> *ptr, int dim) : 
    ConstBaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

template <typename T>
class ConstSpinIterator {
  const BasicArray<T> *m_ptr;
public:
  ConstSpinIterator(const BasicArray<T> *ptr) : m_ptr(ptr) {}
  inline T get() const {return m_ptr->get(1);}
  inline void next() {}
};

#endif
