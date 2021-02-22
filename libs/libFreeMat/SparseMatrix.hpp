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
#ifndef __SparseMatrix_hpp__
#define __SparseMatrix_hpp__

#include <QMap>
#include <QtGlobal>
#include <iostream>
#include "Types.hpp"
#include "NTuple.hpp"
#include "BasicArray.hpp"
#include "Cast.hpp"
#include <cmath>

template <typename T>
class SparseSlice : public QMap<index_t,T> {
};

template <typename T>
class SparseData : public QMap<index_t,SparseSlice<T> > {
};

template <typename T>
class ConstSparseIterator;

template <typename T>
class SparseMatrix {
  SparseData<T> m_data;
  NTuple m_dims;
public:
  SparseMatrix(const NTuple &dims) {
    m_dims = dims;
  }
  const SparseData<T>& constData() const {return m_data;}
  SparseData<T>& data() {return m_data;}
  SparseMatrix(QVector<index_t> row, QVector<index_t> col, QVector<T> val) {
    if (!((row.size() == col.size()) &&
	  (row.size() == val.size())))
      throw Exception("Invalid sizes for IJV style constructor");
    index_t maxcol = 0;
    index_t maxrow = 0;
    for (int i=0;i<row.size();i++) {
      m_data[col[i]][row[i]] = val[i];
      maxcol = qMax(maxcol,col[i]);
      maxrow = qMax(maxrow,row[i]);
    }
    m_dims = NTuple(maxrow,maxcol);
  }
  SparseMatrix(const BasicArray<T> &A) {
    ConstBasicIterator<T> source(&A,0);
    index_t col = 1;
    while (source.isValid()) {
      SparseSlice<T> this_col;
      for (index_t row=1;row<=source.size();row++) {
	if (source.get() != T(0))
	  this_col[row] = source.get();
	source.next();
      }
      if (this_col.count() > 0)
	m_data[col] = this_col;
      ++col;
      source.nextSlice();
    }
    m_dims = A.dimensions();
  }
  inline const NTuple dimensions() const {
    return m_dims;
  }
  inline index_t length() const {return m_dims.count();}
  inline index_t isScalar() const {return length() == 1;}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t columns() const {return m_dims.cols();}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      if (!m_data.contains(pos[1]))
	return T(0);
      const SparseSlice<T>& col = m_data.value(pos[1]);
      if (!col.contains(pos[0]))
	return T(0);
      return col.value(pos[0]);
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      return m_data[pos[1]][pos[0]];
    }
    throw Exception("Out of range");
  }
  inline const T operator[](index_t pos) const {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline T& operator[](index_t pos) {
    NTuple tpos;
    m_dims.map(pos,tpos);
    return (*this)[tpos];
  }
  inline const T get(index_t pos) const {
    return (*this)[pos];
  }
  inline const T get(const NTuple& pos) const {
    return (*this)[pos];
  }
  void erase(const NTuple& pos) {
    if (!m_data.contains(pos[1])) return;
    SparseSlice<T> &col = m_data[pos[1]];
    if (!col.contains(pos[0])) return;
    col.remove(pos[0]);
  }
  void set(const NTuple& pos, const T& val) {
    if (dimensions() <= pos) resize(pos);
    if (val == T(0)) {
      erase(pos);
      return;
    }
    (*this)[pos] = val;
  }
  void set(index_t pos, const T& val) {
    if (dimensions().count() <= pos) resize(pos);
    if (val == T(0)) {
      NTuple tpos;
      m_dims.map(pos,tpos);
      erase(tpos);
      return;
    }
    (*this)[pos] = val;
  }
  SparseMatrix<T> slice(const IndexArrayVector& index) const {
    SparseMatrix<T> ret(NTuple(m_dims[0],1));
    ret.m_data[1] = m_data[index[1].get((index_t)1)];
    return ret;
  }
  void deleteColumns(const IndexArray& index) {
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();i++)
      delete_set.insert(uint64(index.get(i)));
    SparseData<T> copy;
    index_t deleted_count = 0;
    for (index_t i=1;i<=cols();i++) {
      if (delete_set.contains(uint64(i))) 
	deleted_count++;
      else {
	if (m_data.contains(i))
	  copy[i-deleted_count] = m_data[i];
      }
    }
    m_dims = NTuple(m_dims[0],m_dims[1] - delete_set.count());
    m_data = copy;
  }
  void deleteRows(const IndexArray& index) {
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();++i)
      delete_set.insert(uint64(index.get(i)));
    for (typename SparseData<T>::iterator i=m_data.begin();i!=m_data.end();++i) {
      index_t deleted_count = 0;
      SparseSlice<T> copy;
      index_t dp = 1;
      for (typename SparseSlice<T>::iterator j=i.value().begin();j!=i.value().end();++j) {
	if (!delete_set.contains(uint64(j.key()))) {
	  while (dp < j.key()) {
	    if (delete_set.contains(uint64(dp))) ++deleted_count;
	    ++dp;
	  }
	  copy[j.key()-deleted_count] = j.value();
	}
      }
      i.value() = copy;
    }
    m_dims = NTuple(m_dims[0] - delete_set.count(),m_dims[1]);
  }
  void del(const IndexArray& index) {
    if (IsColonOp(index)) {
      *this = SparseMatrix(NTuple(0,0));
      return;
    }
    QSet<uint64> delete_set;
    for (index_t i=1;i<=index.length();i++)
      delete_set.insert(uint64(index.get(i)));
    index_t newSize = length() - delete_set.count();
    if (newSize == length()) return;
    NTuple newDim;
    if (isRowVector())
      newDim = NTuple(1,newSize);
    else
      newDim = NTuple(newSize,1);
    SparseMatrix<T> ret(newDim);
    ConstSparseIterator<T> source(this);
    index_t deleted_count = 0;
    index_t dp = 1;
    while (source.isValid()) {
      index_t source_pos = m_dims.map(source.pos());
      if (!delete_set.contains(uint64(source_pos))) {
	// This element was not deleted.  
	while (dp < source_pos) {
	  if (delete_set.contains(uint64(dp))) ++deleted_count;
	  ++dp;
	}
	ret.set(source_pos-deleted_count,source.value());
      }
      source.next();
    }
    *this = ret;
  }
  void del(const IndexArrayVector& index) {
    if (IsColonOp(index[0]) && IsColonOp(index[1])) {
      *this = SparseMatrix(NTuple(0,0));
      return;
    }
    if (IsColonOp(index[0]))
      deleteColumns(index[1]);
    else if (IsColonOp(index[1]))
      deleteRows(index[0]);
    else
      throw Exception("Unsupported deletion for sparse matrices.");
  }
  void resize(const NTuple& pos) {
    m_dims = pos;
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
	newDim = NTuple(1,len);
      }
      SparseMatrix<T> ret(newDim);
      ConstSparseIterator<T> source(this);
      while (source.isValid()) {
	index_t ndx = source.row() + (source.col()-1)*rows();
	if (newDim[0] == 1)
	  ret.set(NTuple(1,ndx),source.value());
	else
	  ret.set(NTuple(ndx,1),source.value());
	source.next();
      }
      *this = ret;
    }
  }
  void reshape(const NTuple& sze) {
    if (length() != sze.count()) 
      throw Exception("Illegal reshape");
     SparseMatrix<T> ret(sze);
     ConstSparseIterator<T> source(this);
     while (source.isValid()) {
       index_t ndx = source.row() + (source.col()-1)*rows();
       NTuple pos;
       sze.map(ndx,pos);
       ret.set(pos,source.value());
       source.next();
     }
     *this = ret;
  }
  const BasicArray<T> asDense() const {
    ConstSparseIterator<T> source(this);
    BasicArray<T> retvec(dimensions());
    while (source.isValid()) {
      retvec.set(source.pos(),source.value());
      source.next();
    }
    return retvec;
  }
  bool operator==(const SparseMatrix<T> &data) const {
    ConstSparseIterator<T> source(this);
    ConstSparseIterator<T> dest(&data);
    while (source.isValid() && dest.isValid()) {
      if (!(source.pos() == dest.pos())) return false;
      if (source.value() != dest.value()) return false;
      source.next();
      dest.next();
    }
    return true;
  }
  index_t bytes() const {
    index_t total = 0;
    ConstSparseIterator<T> source(this);
    while (source.isValid()) {
      total += TSizeOf<T>(source.value())+sizeof(index_t)*2;
      source.next();
    }
    return total;
  }
};

template <typename T>
void PrintMe(const SparseMatrix<T> &a, Interpreter* io);

template <typename T>
void PrintMe(const SparseMatrix<T> &ar, const SparseMatrix<T> &ai, Interpreter* io);

template <typename T>
inline std::ostream& operator<<(std::ostream& o, const SparseMatrix<T>& arg) {
  arg.printMe(o);
  return o;
}

template <typename T>
class ConstSparseIterator {
  const SparseMatrix<T> *m_ptr;
  typename SparseData<T>::const_iterator m_col;
  typename SparseSlice<T>::const_iterator m_row;
public:
  ConstSparseIterator(const SparseMatrix<T> *ptr) {
    m_ptr = ptr;
    m_col = m_ptr->constData().constBegin();
    if (m_col != m_ptr->constData().constEnd())
      m_row = m_col.value().constBegin();
  }
  inline index_t rows() const {
    return m_ptr->rows();
  }
  inline void next() {
    if (!isValid()) return;
    ++m_row;
    if (m_row == m_col.value().constEnd()) {
      ++m_col;
      if (m_col != m_ptr->constData().constEnd())
	m_row = m_col.value().constBegin();
    }
  }
  bool isValid() const {
    return (m_col != m_ptr->constData().constEnd());
  }
  T value() const {
    if (!isValid()) throw Exception("Error -- attempt to retrieve value of invalid iterator");
    return m_row.value();
  }
  const NTuple pos() const {
    if (isValid())
      return NTuple(m_row.key(),m_col.key());
    else
      return NTuple(1,m_ptr->cols()+1);
  }
  index_t row() const {
    if (isValid())
      return m_row.key();
    else
      return 1;
  }
  index_t col() const {
    if (isValid())
      return m_col.key();
    else 
      return m_ptr->cols()+1;
  }
};

template <typename T>
class ConstComplexSparseIterator {
  ConstSparseIterator<T> m_real;
  ConstSparseIterator<T> m_imag;
  NTuple m_dims;
public:
  ConstComplexSparseIterator(const SparseMatrix<T> *real,
			     const SparseMatrix<T> *imag) : 
    m_real(real), m_imag(imag), m_dims(real->dimensions()) {}
  inline index_t rows() const {
    return m_real.rows();
  }
  void next() {
    if (m_dims.map(m_real.pos()) < m_dims.map(m_imag.pos()))
      m_real.next();
    else if (m_dims.map(m_imag.pos()) < m_dims.map(m_real.pos()))
      m_imag.next();
    else {
      m_real.next();
      m_imag.next();
    }
  }
  const NTuple pos() const {
    if (m_dims.map(m_real.pos()) <= m_dims.map(m_imag.pos()))
      return m_real.pos();
    else
      return m_imag.pos();
  }
  index_t row() const {
    return pos()[0];
  }
  index_t col() const {
    return pos()[1];
  }
  bool isValid() const {
    return (m_real.isValid() || m_imag.isValid());
  }
  T realValue() const {
    if (m_dims.map(m_real.pos()) <= m_dims.map(m_imag.pos()))
      return m_real.value();
    else
      return 0;
  }
  T imagValue() const {
    if (m_dims.map(m_imag.pos()) <= m_dims.map(m_real.pos()))
      return m_imag.value();
    else
      return 0;
  }
};

template <typename T>
bool IsNonNegative(const SparseMatrix<T> &x) {
  ConstSparseIterator<T> i(&x);
  while (i.isValid()) {
    if (i.value() < 0) return false;
    i.next();
  }
  return true;
}

template <typename T>
bool IsInteger(const SparseMatrix<T> &x) {
  ConstSparseIterator<T> i(&x);
  while (i.isValid()) {
    if (IsInteger(i.value())) return false;
    i.next();
  }
  return true;
}

template <typename T, typename S>
SparseMatrix<T> ConvertSparseArray(const SparseMatrix<S> &x) {
  SparseMatrix<T> retvec(x.dimensions());
  ConstSparseIterator<S> i(&x);
  while (i.isValid()) {
    retvec.set(i.pos(),CastConvert<T,S>(i.value()));
    i.next();
  }
  return retvec;
}

// c_ij = sum_k a_ik * b_kj
// For j fixed
// For all i
// c_i = sum_k a_ik * b_k
template <typename T>
SparseMatrix<T> MatrixMultiply(const SparseMatrix<T> &A, const SparseMatrix<T> &B) {
  if (A.columns() != B.rows())
    throw Exception("Non conforming arrays for matrix multiply");
  typename SparseData<T>::const_iterator B_iter(B.constData().constBegin());
  SparseMatrix<T> C(NTuple(A.rows(),B.cols()));
  while (B_iter != B.constData().constEnd()) {
    SparseSlice<T> c_slice;
    SparseSlice<T> b_slice(B_iter.value());
    ConstSparseIterator<T> A_iter(&A);
    while (A_iter.isValid()) {
      c_slice[A_iter.row()] += A_iter.value() * b_slice[A_iter.col()];
      A_iter.next();
    }
    C.data()[B_iter.key()] = c_slice;
    B_iter++;
  }
  return C;
}

template <typename T>
SparseMatrix<T> MatrixMultiply(const SparseMatrix<T> &A, const BasicArray<T> &B) {
  if (A.columns() != B.rows())
    throw Exception("Non conforming arrays for matrix multiply");
  SparseMatrix<T> C(NTuple(A.rows(),B.cols()));
  for (index_t col = 1;col <= B.cols();col++) {
    SparseSlice<T> c_slice;
    ConstSparseIterator<T> A_iter(&A);
    while (A_iter.isValid()) {
      c_slice[A_iter.row()] += A_iter.value() * B.get(NTuple(A_iter.col(),col));
      A_iter.next();
    }
    C.data()[col] = c_slice;
  }
  return C;
}

template <typename T>
SparseMatrix<T> MatrixMultiply(const BasicArray<T> &A, const SparseMatrix<T> &B) {
  if (A.columns() != B.rows())
    throw Exception("Non conforming arrays for matrix multiply");
  SparseMatrix<T> C(NTuple(A.rows(),B.cols()));
  ConstSparseIterator<T> Biter(&B);
  while (Biter.isValid()) {
    SparseSlice<T> c_slice;
    index_t col_number = Biter.col();
    while (Biter.col() == col_number) {
      for (index_t i=1;i <= A.rows(); i++)
	c_slice[i] += A[NTuple(i,Biter.row())] * Biter.value();
      Biter.next();
    }
    C.data()[col_number] = c_slice;
  }
  return C;
}

template <typename T>
SparseMatrix<T> Apply(const SparseMatrix<T>& arg, T (*func)(T)) {
  SparseMatrix<T> retval(arg.dimensions());
  ConstSparseIterator<T> Aiter(&arg);
  while (Aiter.isValid()) {
    retval.set(Aiter.pos(),func(Aiter.value()));
    Aiter.next();
  }
  return retval;
}

template <typename T>
SparseMatrix<T> Negate(const SparseMatrix<T>& arg) {
  return Apply<T>(arg,neg);
}

template <typename T>
SparseMatrix<T> Transpose(const SparseMatrix<T> &arg) {
  SparseMatrix<T> retval(NTuple(arg.cols(),arg.rows()));
  ConstSparseIterator<T> Aiter(&arg);
  while (Aiter.isValid()) {
    retval.set(NTuple(Aiter.col(),Aiter.row()),Aiter.value());
    Aiter.next();
  }
  return retval;
}

template <typename T>
bool IsSymmetric(const SparseMatrix<T> &arg) {
  ConstSparseIterator<T> Aiter(&arg);
  while (Aiter.isValid()) {
    if (arg.get(NTuple(Aiter.col(),Aiter.row())) != Aiter.value())
      return false;
    Aiter.next();
  }
  return true;
}

template <typename T>
bool IsSymmetric(const SparseMatrix<T> &arg, const SparseMatrix<T> &img) {
  ConstComplexSparseIterator<T> Aiter(&arg,&img);
  while (Aiter.isValid()) {
    if (arg.get(NTuple(Aiter.col(),Aiter.row())) != Aiter.realValue()) return false;
    if (img.get(NTuple(Aiter.col(),Aiter.row())) != -Aiter.imagValue()) return false;
    Aiter.next();
  }
  return true;
}

template <typename T>
bool AnyNotFinite(const SparseMatrix<T> &arg) {
  ConstSparseIterator<T> Aiter(&arg);
  while (Aiter.isValid()) {
    if (!IsFinite(Aiter.value())) 
      return true;
    Aiter.next();
  }
  return false;
}

template <typename T>
bool AllZeros(const SparseMatrix<T> &arg) {
  ConstSparseIterator<T> Aiter(&arg);
  while (Aiter.isValid()) {
    if (Aiter.value() != 0) return false;
    Aiter.next();
  }
  return true;
}

template <typename T>
SparseMatrix<T> GetDiagonal(const SparseMatrix<T>& arg, int diagonal) {
  index_t outLen;
  if (diagonal < 0) {
    outLen = qMax(index_t(0),qMin(arg.rows()+diagonal,arg.cols()));
    if (outLen == 0) return SparseMatrix<T>(NTuple(0,0));
    SparseMatrix<T> retvec(NTuple(outLen,1));
    for (index_t i=1;i<=outLen;i++)
      retvec[i] = arg[NTuple(i-diagonal,i)];
    return retvec;
  } else {
    outLen = qMax(index_t(0),qMin(arg.rows(),arg.cols()-diagonal));
    if (outLen == 0) return SparseMatrix<T>(NTuple(0,0));
    SparseMatrix<T> retvec(NTuple(outLen,1));
    for (index_t i=1;i<=outLen;i++)
      retvec[i] = arg[NTuple(i,i+diagonal)];
    return retvec;
  }
}

template <typename T>
SparseMatrix<T> DiagonalArray(const SparseMatrix<T> &arg, int diagonal) {
  index_t outLen = arg.length();
  index_t M = outLen + abs(diagonal);
  SparseMatrix<T> retval(NTuple(M,M));
  if (diagonal < 0) {
    for (index_t i=1;i<=outLen;i++)
      retval[NTuple(i-diagonal,i)] = arg[i];
  } else {
    for (index_t i=1;i<=outLen;i++)
      retval[NTuple(i,i+diagonal)] = arg[i];
  }
  return retval;
}

#endif
