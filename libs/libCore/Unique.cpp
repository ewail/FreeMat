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
#include "Math.hpp"
#include "Complex.hpp"
#include "Algorithms.hpp"
#include <QtCore>

//@@Signature
//function unique UniqueFunction
//inputs A mode
//outputs y m n
//DOCBLOCK array_unique
template <class T>
class UniqueEntryReal {
public:
  BasicArray<T> real;
};

template <class T>
bool operator<(const UniqueEntryReal<T> &A, const UniqueEntryReal<T> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (B.real[i] < A.real[i]) return false;
    if (A.real[i] < B.real[i]) return true;
  }
  return false;
}

template <>
bool operator<(const UniqueEntryReal<Array> &A, const UniqueEntryReal<Array> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (B.real[i].asString() < A.real[i].asString()) return false;
    if (A.real[i].asString() < B.real[i].asString()) return true;
  }
  return false;
}

template <class T>
void UniqueFunctionReal(const BasicArray<T> &data, BasicArray<T> &u_data, 
			BasicArray<index_t> &m_array, BasicArray<index_t> &n_array) {
  QMap<UniqueEntryReal<T>,index_t> qSet;
  for (index_t i=1;i<=data.rows();i++) {
    UniqueEntryReal<T> tmp;
    tmp.real = BasicArray<T>(NTuple(1,data.cols()));
    for (index_t j=1;j<=data.cols();j++)
      tmp.real[j] = data.get(NTuple(i,j));
    qSet.insertMulti(tmp,i);
  }
  QList<UniqueEntryReal<T> > qKeys(qSet.uniqueKeys());
  u_data = BasicArray<T>(NTuple(qKeys.size(),data.cols()));
  m_array = BasicArray<index_t>(NTuple(qKeys.size(),1));
  n_array = BasicArray<index_t>(NTuple(data.rows(),1));
  for (int i=0;i<qKeys.size();i++) {
    QList<index_t> qValues(qSet.values(qKeys[i]));
    for (index_t j=1;j<=data.cols();j++)
      u_data.set(NTuple(i+1,j),qKeys[i].real[j]);
    // To add support for "least" "most" modes of occurance,
    // need to control this line:
    m_array.set(i+1,qValues.front());
    for (int k=0;k<qValues.size();k++)
      n_array.set(qValues[k],i+1);
  }
}

template <class T>
class UniqueEntryComplex {
public:
  BasicArray<T> real;
  BasicArray<T> imag;
};

template <class T>
bool operator<(const UniqueEntryComplex<T> &A, const UniqueEntryComplex<T> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (complex_lt(B.real[i],B.imag[i],A.real[i],A.imag[i])) return false;
    if (complex_lt(A.real[i],A.imag[i],B.real[i],B.imag[i])) return true;
  }
  return false;
}

template <>
bool operator<(const UniqueEntryComplex<Array> &A, const UniqueEntryComplex<Array> &B) {
  throw Exception("Unused case!");
}

template <class T>
void UniqueFunctionComplex(const BasicArray<T> &data_real, 
			   const BasicArray<T> &data_imag,
			   BasicArray<T> &u_data_real, 
			   BasicArray<T> &u_data_imag,
			   BasicArray<index_t> &m_array, 
			   BasicArray<index_t> &n_array) {
  QMap<UniqueEntryComplex<T>,index_t> qSet;
  for (index_t i=1;i<=data_real.rows();i++) {
    UniqueEntryComplex<T> tmp;
    tmp.real = BasicArray<T>(NTuple(1,data_real.cols()));
    tmp.imag = BasicArray<T>(NTuple(1,data_real.cols()));
    for (index_t j=1;j<=data_real.cols();j++) {
      tmp.real[j] = data_real.get(NTuple(i,j));
      tmp.imag[j] = data_imag.get(NTuple(i,j));
    }
    qSet.insertMulti(tmp,i);
  }
  QList<UniqueEntryComplex<T> > qKeys(qSet.uniqueKeys());
  u_data_real = BasicArray<T>(NTuple(qKeys.size(),data_real.cols()));
  u_data_imag = BasicArray<T>(NTuple(qKeys.size(),data_real.cols()));
  m_array = BasicArray<index_t>(NTuple(qKeys.size(),1));
  n_array = BasicArray<index_t>(NTuple(data_real.rows(),1));
  for (int i=0;i<qKeys.size();i++) {
    QList<index_t> qValues(qSet.values(qKeys[i]));
    for (index_t j=1;j<=data_real.cols();j++) {
      u_data_real.set(NTuple(i+1,j),qKeys[i].real[j]);
      u_data_imag.set(NTuple(i+1,j),qKeys[i].imag[j]);
    }
    // To add support for "least" "most" modes of occurance,
    // need to control this line:
    m_array.set(i+1,qValues.front());
    for (int k=0;k<qValues.size();k++)
      n_array.set(qValues[k],i+1);
  }
}

template <typename T>
static inline ArrayVector UniqueFunctionAuxReal(const Array &arg) {
  BasicArray<T> u_data;
  BasicArray<index_t> m_array;
  BasicArray<index_t> n_array;
  UniqueFunctionReal(arg.constReal<T>(),u_data,m_array,n_array);
  ArrayVector ret;
  ret.push_back(Array(u_data));
  ret.push_back(Array(m_array));
  ret.push_back(Array(n_array));
  return ret;
}

template <typename T>
static inline ArrayVector UniqueFunctionAuxComplex(const Array &arg) {
  BasicArray<T> u_data_real;
  BasicArray<T> u_data_imag;
  BasicArray<index_t> m_array;
  BasicArray<index_t> n_array;
  UniqueFunctionComplex(arg.constReal<T>(),
			arg.constImag<T>(),
			u_data_real,
			u_data_imag,
			m_array,n_array);
  ArrayVector ret;
  ret.push_back(Array(u_data_real,u_data_imag));
  ret.push_back(Array(m_array));
  ret.push_back(Array(n_array));
  return ret;    
}

template <typename T>
static inline ArrayVector UniqueFunctionAux(const Array &arg) {
  if (arg.allReal()) 
    return UniqueFunctionAuxReal<T>(arg);
  else
    return UniqueFunctionAuxComplex<T>(arg);
}

#define MacroUnique(ctype,cls) \
  case cls: return UniqueFunctionAux<ctype>(arg);

static inline ArrayVector UniqueFunctionAux(const Array &arg) {
  if (arg.dataClass() == CellArray)
    if (!IsCellStringArray(arg)) 
      throw Exception("cell array arguments to 'unique' must be all strings");
  switch (arg.dataClass()) {
  default:
    throw Exception("Unhandled type for unique function");
    MacroExpandCasesSimple(MacroUnique);
  case StringArray: return UniqueFunctionAuxReal<QChar>(arg);
  case CellArray: return UniqueFunctionAux<Array>(arg);
  }
}

#undef MacroUnique

ArrayVector ToRowVector(const ArrayVector& x) {
  ArrayVector z;
  for (int i=0;i<x.size();i++) {
    Array y(x[i]);
    y.reshape(NTuple(1,y.length()));
    z.push_back(y);
  }
  return z;
}

ArrayVector ToEmptyVector(const ArrayVector& x) {
  ArrayVector z;
  for (int i=0;i<x.size();i++) {
    Array y(x[i]);
    y.reshape(NTuple(0,1));
    z.push_back(y);
  }
  return z;
}

ArrayVector UniqueFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("unique function requires at least one argument");
  if (arg[0].isEmpty()) {
    if (nargout <= 1) {
      if (arg[0].isEmpty() && !arg[0].is2D()) {
	Array y(arg[0]);
	y.reshape(NTuple(0,1));
	return ArrayVector(y);
      } else
	return ArrayVector(arg[0]);
    } else {
      ArrayVector retval;
      retval.push_back(arg[0]);
      retval.push_back(EmptyConstructor());
      retval.push_back(EmptyConstructor());
      if (arg[0].isEmpty() && !arg[0].is2D())
	return ToEmptyVector(retval);
      return retval;
    }
  }
  bool rowmode = false;
  if (arg.size() == 2) {
    if (arg[1].asString().toLower() != "rows")
      throw Exception("second argument to unique must be 'rows'");
    rowmode = true;
  }
  if (rowmode) {
    if (!arg[0].is2D() && (arg[0].dimensions().max() == arg[0].dimensions().count())) {
      ArrayVector retval;
      retval.push_back(arg[0]);
      retval.push_back(Array(double(1)));
      retval.push_back(Array(double(1)));
      return retval;
    } else if (!arg[0].is2D())
      throw Exception("'rows' mode only works for matrix (2D) arguments");
  }
  if (rowmode && IsCellStringArray(arg[0]) && arg[0].isRowVector())
    rowmode = false;
  Array z;
  if (!rowmode) 
    z = Vectorize(arg[0].asDenseArray());
  else 
    z = arg[0].asDenseArray();
  ArrayVector x(UniqueFunctionAux(z));
  if (arg[0].isRowVector()) {
    return ToRowVector(x);
  }
  return x;
}
