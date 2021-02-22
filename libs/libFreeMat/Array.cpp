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
#include "GetSet.hpp"
#include <QStringList>
#include "Struct.hpp"
#include "Algorithms.hpp"
#include "Cast.hpp"
#include "Math.hpp"

template <typename T>
static inline void* Tconstruct(Type t, const void *copy) {
  if (t.Scalar == 1)
    throw Exception("Tconstruct called for scalar object!");
  if (t.Sparse == 1) {
    const SparseMatrix<T> *p = reinterpret_cast<const SparseMatrix<T>*>(copy);
    return reinterpret_cast<void*>(new SparseMatrix<T>(*p));
  }
  const BasicArray<T> *p = reinterpret_cast<const BasicArray<T>*>(copy);
  return reinterpret_cast<void*>(new BasicArray<T>(*p));
}

#define MacroTConstructCopy(ctype,cls)		\
  case cls: return Tconstruct<ctype>(t,copy); 

static inline void* construct(Type t, const void *copy) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported construct");
    MacroExpandCasesAll(MacroTConstructCopy);
  case Struct:
    return reinterpret_cast<void*>(new StructArray(*(reinterpret_cast<const StructArray*>(copy))));
  }
}

#undef MacroTConstructCopy

template <typename T>
static inline void* Tconstruct_sized(Type t, const NTuple &dims) {
  if (t.Scalar == 1)
    throw Exception("Tconstruct_sized called for scalar object");
  if (t.Sparse == 1) {
    return reinterpret_cast<void*>(new SparseMatrix<T>(dims));
  }
  return reinterpret_cast<void*>(new BasicArray<T>(dims));
}

#define MacroTConstructSized(ctype,cls) \
  case cls: return Tconstruct_sized<ctype>(t,dims);

static inline void* construct_sized(Type t, const NTuple &dims) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported constructor");
    MacroExpandCasesAll(MacroTConstructSized);
  case Struct: 
    {
      StructArray *p = new StructArray;
      p->setDimensions(dims);
      return reinterpret_cast<void*>(p);
    }
  }
}

#undef MacroTConstructSized

template <typename T>
static inline void Tdestruct(Type t, void *todelete) {
  if (t.Scalar == 1)
    throw Exception("Tdestruct called on scalar");
  if (t.Sparse == 1) {
    delete reinterpret_cast<SparseMatrix<T>*>(todelete);
    return;
  }
  delete reinterpret_cast<BasicArray<T>*>(todelete);
  return;
}

#define MacroTDestruct(ctype,cls) \
  case cls: return Tdestruct<ctype>(t,todelete);

static inline void destruct(Type t, void *todelete) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported construct");
    MacroExpandCasesAll(MacroTDestruct);
  case Struct:
    delete reinterpret_cast<StructArray*>(todelete);
    return;
  }
}

#undef MacroTDestruct

static inline bool AllNonBoolScalars(const ArrayVector& index) {
  for (int i=0;i<index.size();i++)
    if (!index[i].isScalar() || (index[i].dataClass() == Bool) || (index[i].dataClass() == StringArray)) return false;
  return true;
}

SharedObject::SharedObject(Type t, void* p) : m_type(t), m_p(p) {}

SharedObject::SharedObject(const SharedObject& copy) : 
  QSharedData(copy), m_type(copy.m_type) {
  m_p = construct(m_type,copy.m_p);
}

SharedObject& SharedObject::operator=(const SharedObject& copy) {
  if (this == &copy) return *this;
  destruct(m_type,m_p);
  m_type = copy.m_type;
  m_p = construct(m_type,copy.m_p);
  return *this;
}

SharedObject::~SharedObject() {
  destruct(m_type,m_p);
}

Array::Array(DataClass t, const NTuple &dims) {
  if (!dims.isValid())
    throw Exception("Illegal size for array " + dims.toString());
  m_type.Class = t;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,construct_sized(m_type,dims));
}

Array::Array(const StructArray& real) {
  m_type.Class = Struct;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,new StructArray(real));
}

Array::Array(const QChar &, const QChar &) {
  throw Exception("Complex strings are not supported");
}

Array::Array(const QChar &t) {
  m_type.Class = StringArray;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,
			      construct_sized(m_type,
					      NTuple(1,1)));
  BasicArray<QChar> &p(real<QChar>());
  p[1] = t;
}

Array::Array(const QString &text) {
  m_type.Class = StringArray;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,
			      construct_sized(m_type,
					      NTuple(1,text.size())));
  BasicArray<QChar> &p(real<QChar>());
  for (int i=0;i<text.size();i++) 
    p[i+1] = text[i];
}

template <typename T>
static inline const NTuple Tdim(const Array *ptr) {
  if (ptr->type().Sparse == 1)
    return ptr->constRealSparse<T>().dimensions();
  else
    return ptr->constReal<T>().dimensions();
}

#define MacroDimensions(ctype,cls)			\
  case cls: return Tdim<ctype>(this);

const NTuple Array::dimensions() const {
  if (m_type.Scalar == 1) return NTuple(1,1);
  switch (m_type.Class) {
  default:
    throw Exception("unhandled type");
  case Invalid:
    return NTuple(0,0);
  case Struct:
    return constStructPtr().dimensions();
    MacroExpandCasesAll(MacroDimensions);
  }
}

#undef MacroDimensions

template <typename S, typename T>
static inline void Tdel_scalar(Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    ptr->realSparse<T>().del(ScalarToIndex(ndx));
    if (!ptr->allReal())
      ptr->imagSparse<T>().del(ScalarToIndex(ndx));
    return;
  }
  ptr->real<T>().del(ScalarToIndex(ndx));
  if (!ptr->allReal())
    ptr->imag<T>().del(ScalarToIndex(ndx));
}

template <typename S, typename T>
static inline void Tset_scalar(Array *ptr, S ndx, const Array& data) {
  if (data.isEmpty())
    return Tdel_scalar<S,T>(ptr,ndx);
  if (ptr->isSparse()) {
    ptr->realSparse<T>().set(ndx,data.constRealScalar<T>());
    if (!data.allReal())
      ptr->imagSparse<T>().set(ndx,data.constImagScalar<T>());
    return;
  }
  ptr->real<T>().set(ndx,data.constRealScalar<T>());
  if (!data.allReal())
    ptr->imag<T>().set(ndx,data.constImagScalar<T>());
  else if (!ptr->allReal())
    ptr->imag<T>().set(ndx,T(0));
}

template <typename S>
static inline const void Tset_string_scalar(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.isEmpty())
    ptr->real<QChar>().del(ndx);
  else
    ptr->real<QChar>().set(ndx,rhs.constReal<QChar>()[1]);
}

template <typename S>
static inline const void Tset_struct_scalar(Array*ptr, S ndx, const Array &rhs) {
  if ((rhs.dataClass() != Struct) && !rhs.isEmpty())
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  StructArray &lp(ptr->structPtr());
  if (rhs.isEmpty()) {
    for (int i=0;i<lp.fieldCount();i++)
      lp[lp.fieldName(i)].del(ScalarToIndex(ndx));
  } else {
    // First loop through the elements
    const StructArray &rp(rhs.constStructPtr());
    for (int i=0;i<rp.fieldCount();i++) 
      lp[rp.fieldName(i)].set(ndx,rp[i].get(1));
  }
  // Loop through the output and force all arrays to be the same size
  NTuple newSize(0,0);
  for (int i=0;i<lp.fieldCount();i++)
    newSize = max(newSize,lp[i].dimensions());
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].resize(newSize);
  lp.updateDims();
}


#define MacroSetIndexT(ctype,cls)		\
  case cls: Tset_scalar<index_t,ctype>(this,index,data.toClass(this->dataClass())); return;

void Array::set(index_t index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexT);
  case Struct: Tset_struct_scalar<index_t>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n) = B");
  }
}

#undef MacroSetIndexT

#define MacroSetNTuple(ctype,cls)		\
  case cls: Tset_scalar<const NTuple&,ctype>(this,index,data.toClass(this->dataClass())); return;

void Array::set(const NTuple& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetNTuple);
  case Struct: Tset_struct_scalar<const NTuple&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n1,..,nm) = B");
  }
}

#undef MacroSetNTuple

// Handles assignment of A(ndx) = B, where ndx is an IndexArray or IndexArrayVector,
// and A is a basic array of simple types (excludes strings, cell arrays, and structs).
template <typename S, typename T>
static inline void Tset(Array* ptr, S ndx, const Array& data) {
  Array dataTyped(data.toClass(ptr->dataClass()));
  if (!ptr->allReal())
    dataTyped.forceComplex();
  if (dataTyped.isScalar()) {
    if (ptr->isSparse())
      Set(ptr->realSparse<T>(),ndx,dataTyped.constRealScalar<T>());
    else
      Set(ptr->real<T>(),ndx,dataTyped.constRealScalar<T>());
    if (!dataTyped.allReal()) {
      if (ptr->isSparse())
	Set(ptr->imagSparse<T>(),ndx,dataTyped.constImagScalar<T>());
      else
	Set(ptr->imag<T>(),ndx,dataTyped.constImagScalar<T>());
    }
  } else {
    if (ptr->isSparse())
      Set(ptr->realSparse<T>(),ndx,ToRealSparse<T>(dataTyped));
    else
      Set(ptr->real<T>(),ndx,dataTyped.constReal<T>());
    if (!dataTyped.allReal()) {
      if (ptr->isSparse())
	Set(ptr->imagSparse<T>(),ndx,ToImagSparse<T>(dataTyped));
      else
	Set(ptr->imag<T>(),ndx,dataTyped.constImag<T>());
    }
  }
}

template <typename S>
static inline const void Tset_struct(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.dataClass() != Struct)
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  // First loop through the elements
  const StructArray &rp(rhs.constStructPtr());
  StructArray &lp(ptr->structPtr());
  NTuple newSize(ptr->dimensions());
  for (int i=0;i<rp.fieldCount();i++) {
    Set(lp[rp.fieldName(i)],ndx,rp[i]);
    newSize = lp[rp.fieldName(i)].dimensions();
  }
  // Loop through the output and force all arrays to be the same size
  for (int j=0;j<lp.fieldCount();j++) 
    lp[j].resize(newSize);
  lp.updateDims();
}

#define MacroSetIndexArray(ctype,cls)		\
  case cls: return Tset<const IndexArray&,ctype>(this,index,data);

void Array::set(const IndexArray& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexArray);
  case Struct: Tset_struct<const IndexArray&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n) = B");
  }  
}

#undef MacroSetIndexArray

#define MacroSetIndexArrayVector(ctype,cls) \
  case cls: return Tset<const IndexArrayVector&,ctype>(this,index,data);

void Array::set(const IndexArrayVector& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexArrayVector);
  case Struct: Tset_struct<const IndexArrayVector&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n1,...,nm) = B");
  }  
}

#undef MacroSetIndexArrayVector

template <typename S>
static inline void Treshape_struct(Array* ptr, S ndx) {
  StructArray &lp(ptr->structPtr());
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].reshape(ndx);
  lp.updateDims();
}

template <typename S, typename T>
static inline void Treshape(Array* ptr, S ndx) {
  if (ptr->isSparse()) {
    ptr->realSparse<T>().reshape(ndx);
    if (!ptr->allReal())
      ptr->imagSparse<T>().reshape(ndx);
  } else {
    ptr->real<T>().reshape(ndx);
    if (!ptr->allReal())
      ptr->imag<T>().reshape(ndx);
  }
}
#define MacroReshapeNTuple(ctype,cls) \
  case cls: return Treshape<const NTuple&, ctype>(this,size); 

void Array::reshape(const NTuple &size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroReshapeNTuple);
  case Struct: return Treshape_struct<const NTuple&>(this,size);
  default:
    throw Exception("Unhandled case for reshape");
  }
}

#undef MacroReshapeNTuple

template <typename S>
static inline void Tresize_struct(Array* ptr, S ndx) {
  StructArray &lp(ptr->structPtr());
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].resize(ndx);
  lp.updateDims();
}

template <typename S, typename T>
static inline void Tresize(Array* ptr, S ndx) {
  if (ptr->isSparse()) {
    ptr->realSparse<T>().resize(ndx);
    if (!ptr->allReal())
      ptr->imagSparse<T>().resize(ndx);
  } else {
    ptr->real<T>().resize(ndx);
    if (!ptr->allReal())
      ptr->imag<T>().resize(ndx);
  }
}

#define MacroResizeNTuple(ctype,cls) \
  case cls: return Tresize<const NTuple&,ctype>(this,size);

void Array::resize(const NTuple &size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroResizeNTuple);
  case Struct: return Tresize_struct<const NTuple&>(this,size);
  default:
    throw Exception("Unhandled case for resize");
  }
}
#undef MacroResizeNTuple

#define MacroResizeIndex(ctype,cls) \
  case cls: return Tresize<index_t,ctype>(this,size);

void Array::resize(index_t size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroResizeIndex);
  case Struct: return Tresize_struct<index_t>(this,size);
  default:
    throw Exception("Unhandled case for resize");
  }
}

#undef MacroResizeIndex

void Array::set(const QString& field, ArrayVector& data) {
  if (isEmpty() && m_type.Class != Struct)
    *this = Array::create(Struct);
  if (m_type.Class != Struct) throw Exception("Unsupported type for A.field=B");
  StructArray &rp(structPtr());
  if (isEmpty()) 
    rp.insert(field,BasicArray<Array>(NTuple(1,1)));
  if (!rp.contains(field))
    rp.insert(field,BasicArray<Array>(dimensions()));
  if (data.size() < length())
    throw Exception("Not enough right hand values to satisfy left hand side expression.");
  BasicArray<Array> &val(rp[field]);
  for (index_t i=1;i<=length();i++) {
    val.set(i,data.front());
    data.pop_front();
  }
  rp.updateDims();
}

const ArrayVector Array::get(const QString& field) const {
  if (m_type.Class != Struct) throw Exception("Unsupported type for get(string)");
  const StructArray &rp(constStructPtr());
  if (!rp.contains(field)) throw Exception("Reference to non-existent field " + field);
  ArrayVector ret;
  const BasicArray<Array> &val(rp[field]);
  for (index_t i=1;i<=val.length();i++)
    ret.push_back(val.get(i));
  return ret;
}

template <typename S>
static inline Array Tget_struct(const Array*ptr, S ndx) {
  const StructArray &rp(ptr->constStructPtr());
  Array ret(Struct,NTuple(1,1));
  StructArray &lp(ret.structPtr());
  for (int i=0;i<rp.fieldCount();i++)
    lp[rp.fieldName(i)] = Get(rp[i],ndx);
  lp.setClassName(rp.className());
  lp.updateDims();
  return ret;
}

template <typename S, typename T>
static inline const Array Tget(const Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    if (ptr->allReal())
      return Array(Get(ptr->constRealSparse<T>(),ndx));
    else
      return Array(Get(ptr->constRealSparse<T>(),ndx),
		   Get(ptr->constImagSparse<T>(),ndx));
  } else {
    if (ptr->allReal())
      return Array(Get(ptr->constReal<T>(),ndx));
    else
      return Array(Get(ptr->constReal<T>(),ndx),
		   Get(ptr->constImag<T>(),ndx));
  }
}

#define MacroGetIndexArray(ctype,cls) \
  case cls: return Tget<const IndexArray&,ctype>(this,index);

const Array Array::get(const IndexArray& index) const {
  if (m_type.Scalar == 1)
    return asDenseArray().get(index);
  switch (m_type.Class) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
    MacroExpandCasesNoCell(MacroGetIndexArray);
  case CellArray: return Get(constReal<Array>(),index);
  case Struct: return Tget_struct<const IndexArray&>(this,index);
  }
}

#undef MacroGetIndexArray

#define MacroGetIndexArrayVector(ctype,cls) \
  case cls: return Tget<const IndexArrayVector&,ctype>(this,index);

const Array Array::get(const IndexArrayVector& index) const {
  if (m_type.Scalar == 1)
    return asDenseArray().get(index);
  switch (m_type.Class) {
  default:
    throw Exception("Unsupported type for get(indexarrayvector)");
  MacroExpandCasesNoCell(MacroGetIndexArrayVector);
  case CellArray: return Get(constReal<Array>(),index);
  case Struct: return Tget_struct<const IndexArrayVector&>(this,index);
  }
}

#undef MacroGetIndexArrayVector

template <typename T, typename S>
inline static const Array Tcast(DataClass t, const Array *ptr) {
  if (ptr->type().Scalar == 1) {
    if (ptr->allReal())
      return Array(CastConvert<T,S>(ptr->constRealScalar<S>()));
    else
      return Array(CastConvert<T,S>(ptr->constRealScalar<S>()),
		   CastConvert<T,S>(ptr->constImagScalar<S>()));
  }
  if (ptr->isSparse()) {
    if (ptr->allReal())
      return Array(ConvertSparseArray<T,S>(ptr->constRealSparse<S>()));
    else
      return Array(ConvertSparseArray<T,S>(ptr->constRealSparse<S>()),
		   ConvertSparseArray<T,S>(ptr->constImagSparse<S>()));
  }
  if (ptr->allReal())
    return Array(ConvertBasicArray<T,S>(ptr->constReal<S>()));
  else
    return Array(ConvertBasicArray<T,S>(ptr->constReal<S>()),
		 ConvertBasicArray<T,S>(ptr->constImag<S>()));
}

#define MacroClassName(ctype,cls) \
  case cls: return QString(#cls).toLower();

QString Array::className() const {
  if ((dataClass() == Struct) && (constStructPtr().isUserClass()))
    return constStructPtr().className();
  else {
    if (dataClass() == Float) return QString("single");
    switch (dataClass()) {
    default: throw Exception("Unknown class?!");
    case Invalid: return QString("");
    case CellArray: return QString("cell");
    case Struct: return QString("struct");
    case StringArray: return QString("char");
    case Bool: return QString("logical");
      MacroExpandCasesNoBool(MacroClassName);
    }
  }
}

#undef MacroClassName

bool Array::isUserClass() const {
  return ((dataClass() == Struct) && constStructPtr().isUserClass());;
}

template <typename T>
inline static const Array BcastComplex(const Array *ptr) {
  Array real(Real(*ptr).toClass(Bool));
  Array imag(Imag(*ptr).toClass(Bool));
  return Or(real,imag);
}

#define MacroBCastComplex(ctype,cls) \
  case cls: return BcastComplex<ctype>(ptr);

// Special case the complex-to-bool cast, as it has special rules
static Array BoolCastComplex(const Array *ptr) {
  switch (ptr->dataClass()) {
  default: Exception("Cannot convert given array to logical type");
  MacroExpandCasesNoCell(MacroBCastComplex);
  }
}

#undef MacroBCastComplex

#define MacroTcast(ctype,cls) \
  case cls: return Tcast<T,ctype>(t,ptr);

template <typename T>
inline static const Array TcastCase(DataClass t, const Array *ptr) {
  switch (ptr->dataClass()) {
  default:
    throw Exception("Cannot perform type conversions with this type");
    MacroExpandCases(MacroTcast);
    MacroTcast(QChar,StringArray);
  }
}

#undef MacroTcast

#define MacroTcastCase(ctype,cls) \
  case cls: return TcastCase<ctype>(t,this);

const Array Array::toClass(DataClass t) const {
  if (dataClass() == t) return *this;
  if (isEmpty()) return Array(t,dimensions());
  if ((t == Bool) && (!allReal())) return BoolCastComplex(this);
  switch (t) {
  default:
    throw Exception("unhandled case for type conversion");
    MacroExpandCasesSimple(MacroTcastCase);
    MacroTcastCase(QChar,StringArray);
  }
}

#undef MacroTcastCase

template <typename S, typename T>
static inline Array Tget_scalar(const Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    if (ptr->allReal()) 
      return Array(ptr->constRealSparse<T>().get(ndx));
    else
      return Array(ptr->constRealSparse<T>().get(ndx),
		   ptr->constImagSparse<T>().get(ndx));
  }
  if (ptr->allReal()) 
    return Array(ptr->constReal<T>().get(ndx));
  else
    return Array(ptr->constReal<T>().get(ndx),
		 ptr->constImag<T>().get(ndx));
}

template <typename S>
static inline Array Tget_struct_scalar(const Array*ptr, S ndx) {
  const StructArray &rp(ptr->constStructPtr());
  Array ret(Struct,NTuple(1,1));
  StructArray &lp(ret.structPtr());
  for (int i=0;i<rp.fieldCount();i++)
    lp[rp.fieldName(i)].set(1,rp[i].get(ndx));
  lp.setClassName(rp.className());
  lp.updateDims();
  return ret;
}

template <typename S>
static inline Array Tget_string_scalar(const Array* ptr, S ndx) {
  BasicArray<QChar> ret(NTuple(1,1));
  ret.set(1,ptr->constReal<QChar>()[ndx]);
  return ret;
}

template <typename S>
static inline Array Tget_cell_scalar(const Array* ptr, S ndx) {
  BasicArray<Array> ret(NTuple(1,1));
  ret.set(1,ptr->constReal<Array>()[ndx]);
  return ret;
}

#define MacroGetNTuple(ctype,cls) \
  case cls: return Tget_scalar<const NTuple&,ctype>(this,index);

const Array Array::get(const NTuple& index) const {
  if ((m_type.Scalar == 1) && index.isScalar())
    return *this;
  if (m_type.Scalar == 1) return asDenseArray().get(index);
  switch (m_type.Class) {
  default:
    throw Exception("Unsupported type for get(const NTuple&)");
    MacroExpandCasesSimple(MacroGetNTuple);
  case CellArray: return Tget_cell_scalar<const NTuple&>(this,index);
  case Struct: return Tget_struct_scalar<const NTuple&>(this,index);
  case StringArray: return Tget_string_scalar<const NTuple&>(this,index);
  }
}

#undef MacroGetNTuple

#define MacroGetIndexT(ctype,cls) \
  case cls: return Tget_scalar<index_t,ctype>(this,index);

const Array Array::get(index_t index) const {
  if ((m_type.Scalar == 1) && (index == 1))
    return *this;
  if (m_type.Scalar == 1) return asDenseArray().get(index);
  switch (m_type.Class) {
  default:
    throw Exception("Unhandled case for get(index)");
    MacroExpandCasesSimple(MacroGetIndexT);
  case CellArray: return Tget_cell_scalar<index_t>(this,index);
  case StringArray: return Tget_string_scalar<index_t>(this,index);
  case Struct: return Tget_struct_scalar<index_t>(this,index);
  }
}

#undef MacroGetIndexT

const Array Array::get(const Array& index) const {
  if (index.isScalar() && (index.dataClass() != Bool) && (index.dataClass() != StringArray)) {
    if (!index.allReal())
     WarningMessage("Complex part of index ignored");
    return get(index.asIndexScalar());
  } else
    return get(IndexArrayFromArray(index));
}

const Array Array::get(const ArrayVector& index) const {
  if (AllNonBoolScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    return get(addr);
  }
  else {
    IndexArrayVector addr;
    for (int i=0;i<index.size();i++)
      addr.push_back(IndexArrayFromArray(index[i]));
    return get(addr);
  }  
}

void Array::set(const Array& index, const Array& data) {
  if (isEmpty() && dataClass() != data.dataClass())
    *this = toClass(data.dataClass());
  if (index.isScalar() && (index.dataClass() != Bool)
      && (index.dataClass() != StringArray)) {
    if (!index.allReal())
      WarningMessage("Complex part of index ignored");
    set(index.asIndexScalar(),data);
  }
  else
    set(IndexArrayFromArray(index),data);
}

void Array::set(const ArrayVector& index, const Array& data) {
  if (isEmpty() && dataClass() != data.dataClass())
    *this = toClass(data.dataClass());
  if (AllNonBoolScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    set(addr,data);
  }
  else {
    IndexArrayVector addr;
    for (int i=0;i<index.size();i++)
      addr.push_back(IndexArrayFromArray(index[i]));
    set(addr,data);
  }
}

void Array::addField(QString name) {
  if (dataClass() != Struct)
    throw Exception("addField only valid for structure arrays");
  if (!structPtr().contains(name))
    structPtr().insert(name,BasicArray<Array>());
}

#define MacroGetVoidPointer(ctype,cls)					\
  case cls:								\
  if (m_type.Scalar == 1)						\
    return (void*)(&(m_real.cls));					\
  else if (m_type.Sparse == 1)						\
    throw Exception("getVoidPointer not supported for sparse arrays");	\
  else									\
    return (void*)(real<ctype>().data());

void* Array::getVoidPointer() {
  switch (dataClass()) {
  default:
    throw Exception("Unsupported type called for getVoidPointer");
    MacroExpandCasesSimple(MacroGetVoidPointer);
  }
}

#undef MacroGetVoidPointer

#define MacroGetConstVoidPointer(ctype,cls)				\
  case cls:								\
  if (m_type.Scalar == 1)						\
    return (const void*)(&(m_real.cls));				\
  else if (m_type.Sparse == 1)						\
    throw Exception("getConstVoidPointer not supported for sparse arrays"); \
  else									\
    return (const void*)(constReal<ctype>().constData());

const void* Array::getConstVoidPointer() const {
  switch (dataClass()) {
  default:
    throw Exception("Unsupported type called for getConstVoidPointer");
    MacroExpandCasesSimple(MacroGetConstVoidPointer);
  }
}

template <typename T>
static inline index_t Tbytes(const Array *ptr) {
  index_t count = 0;
  if (ptr->type().Sparse == 1)
    count = ptr->constRealSparse<T>().bytes();
  else if (ptr->type().Scalar == 1) 
    count = TSizeOf<T>(ptr->constRealScalar<T>());
  else
    count = ptr->constReal<T>().bytes();
  if (!ptr->allReal()) {
    if (ptr->type().Sparse == 1)
      count += ptr->constImagSparse<T>().bytes();
    else if (ptr->type().Scalar == 1) 
      count += TSizeOf<T>(ptr->constImagScalar<T>());
    else
      count += ptr->constImag<T>().bytes();
  }
  return count;
}

#define MacroTbytes(ctype,cls)			\
  case cls: return Tbytes<ctype>(this);

index_t Array::bytes() const {
  switch (dataClass()) {
  default:
    return 0;
    MacroExpandCasesAll(MacroTbytes);
  case Struct:
    return constStructPtr().bytes();
  }
}

index_t Array::address() const {
  if (m_type.Scalar == 1)
    return 0;
  else {
    union l {
      void *p;
      uint32 y;
    } u;
    u.p = m_real.p->ptr();
    return (index_t)(u.y);
  }
}

template <typename T>
static inline index_t T_index_scalar(const Array *ptr) {
  index_t a = ptr->constRealScalar<T>();
  if (a < 1) throw Exception("index values must be >= 1");
  return a;
}

#define MacroAsIndexScalar(ctype,cls) \
  case cls:			      \
  return T_index_scalar<ctype>(this);

const index_t Array::asIndexScalar() const {
  switch (dataClass()) {
  default:
    throw Exception("Unsupported type called on asIndexScalar");
  case Bool:
    if (constRealScalar<bool>()) return index_t(1);
    return index_t(0);
  case StringArray:
      throw Exception("Illegal indexing expression");
  MacroExpandCasesNoBool(MacroAsIndexScalar);    
  }
}

#undef MacroAsIndexScalar

template <typename T>
static inline void T_force_complex(Array *ptr) {
  if (!ptr->allReal()) return;
  if (ptr->type().Scalar == 1) {
    ptr->imagScalar<T>() = 0;
  } else if (ptr->type().Sparse == 1) {
    ptr->imagSparse<T>();
  } else {
    ptr->imag<T>();
  }
}

#define MacroForceComplex(ctype,cls) \
  case cls: return T_force_complex<ctype>(this);

void Array::forceComplex() {
  switch (dataClass()) {
  default:
    return;
    MacroExpandCasesNoBool(MacroForceComplex);
  }
}


#define MacroAsComplex(ctype,cls) \
  case cls: return T_as_complex<ctype>(this);

const Array Array::asComplex() const {
  if (!allReal()) return *this;
  Array tmp(*this);
  tmp.forceComplex();
  return tmp;
}

static inline bool Tequals_struct(const Array *pA, const Array *pB) {
  const StructArray &ap(pA->constStructPtr());
  const StructArray &bp(pB->constStructPtr());
  if (ap.fieldCount() != bp.fieldCount()) return false;
  for (int i=0;i<ap.fieldCount();i++) {
    if (!bp.contains(ap.fieldName(i))) return false;
    if (!(ap[i] == bp[ap.fieldName(i)])) return false;
  }
  return true;
}

static inline bool Tequals_string(const Array *pA, const Array *pB) {
  return (pA->constReal<QChar>() == pB->constReal<QChar>());
}

static inline bool Tequals_cell(const Array *pA, const Array *pB) {
  return (pA->constReal<Array>() == pB->constReal<Array>());
}

template <typename T>
static inline bool Tequals_scalar(const Array *pA, const Array *pB) {
  if (pA->allReal())
    return (pA->constRealScalar<T>() == pB->constRealScalar<T>());
  else
    return ((pA->constRealScalar<T>() == pB->constRealScalar<T>()) &&
	    (pA->constImagScalar<T>() == pB->constImagScalar<T>()));
}

template <typename T>
static inline bool Tequals_array(const Array *pA, const Array *pB) {
  if (pA->isSparse() && pB->isSparse()) {
    if (pA->allReal())
      return (pA->constRealSparse<T>() == pB->constRealSparse<T>());
    else
      return ((pA->constRealSparse<T>() == pB->constRealSparse<T>()) &&
	      (pA->constImagSparse<T>() == pB->constImagSparse<T>()));
  } else {
    const Array &dA(pA->asDenseArray());
    const Array &dB(pB->asDenseArray());
    if (dA.allReal())
      return (dA.constReal<T>() == dB.constReal<T>());
    else 
      return ((dA.constReal<T>() == dB.constReal<T>()) &&
	      (dA.constImag<T>() == dB.constImag<T>()));
  }
}

#define MacroScalarEquals(ctype,cls) \
  case cls: return Tequals_scalar<ctype>(this,&b);

#define MacroArrayEquals(ctype,cls) \
  case cls: return Tequals_array<ctype>(this,&b);

// Need to make this more general - how so?
bool Array::operator==(const Array &b) const {
  if (!isReferenceType() && !b.isReferenceType() && isScalar() && b.isScalar()) {
    if (allReal() ^ b.allReal()) return false;
    if (dataClass() != b.dataClass()) return false;
    switch (dataClass()) {
    default:
      throw Exception("Unhandled scalar case");
      MacroExpandCasesSimple(MacroScalarEquals);
    }
    return false;
  }
  if (!(dimensions() == b.dimensions())) return false;
  if (dataClass() != b.dataClass()) return false;
  if (allReal() ^ b.allReal()) return false;
  switch (dataClass()) {
  default:
    throw Exception("Unhandled case as argument to == operator for Array class");
    MacroExpandCasesSimple(MacroArrayEquals);
  case CellArray: return Tequals_cell(this,&b);
  case Struct: return Tequals_struct(this,&b);
  case StringArray: return Tequals_string(this,&b);
  }
  return false;
}

#undef MacroArrayEquals

#undef MacroScalarEquals

int32 Array::asInteger() const {
  if (m_type.Class == StringArray) return asString().toInt();
  return (this->toClass(Int32).constRealScalar<int32>());
}

double Array::asDouble() const {
  if (m_type.Class == StringArray) return asString().toDouble();
  return (this->toClass(Double).constRealScalar<double>());
}

QString Array::asString() const {
  if (isEmpty()) return QString();
  if (m_type.Class != StringArray) throw Exception("Cannot convert array to string");
  const BasicArray<QChar> &p(constReal<QChar>());
  QString ret;
  for (int i=0;i<p.length();i++)
    ret += p[i+1];
  return ret;
}


template <typename T>
static inline Array Tscalar_to_dense(const Array *ptr) {
  if (ptr->allReal()) {
    return Array(new BasicArray<T>(ptr->constRealScalar<T>()));
  } else {
    return Array(new BasicArray<T>(ptr->constRealScalar<T>()),
    		 new BasicArray<T>(ptr->constImagScalar<T>()));
  }
}

template <typename T>
static inline Array Tsparse_to_dense(const Array *ptr) {
  if (ptr->allReal())
    return Array(ptr->constRealSparse<T>().asDense());
  else
    return Array(ptr->constRealSparse<T>().asDense(),
		 ptr->constImagSparse<T>().asDense());
}

#define MacroScalarToDense(ctype,cls) \
  case cls: return Tscalar_to_dense<ctype>(this);

#define MacroSparseToDense(ctype,cls) \
  case cls: return Tsparse_to_dense<ctype>(this);

Array Array::asDenseArray() const {
  if ((m_type.Scalar == 0) && (m_type.Sparse == 0))
    return *this;
  if (m_type.Scalar == 1) {
    switch (m_type.Class) {
    default:
      throw Exception("Cannot convert supplied array to dense type");
      MacroExpandCasesSimple(MacroScalarToDense);
    }
  }
  switch (m_type.Class) {
  default:
    throw Exception("Cannot convert supplied sparse array to dense type");
    MacroExpandCasesSimple(MacroSparseToDense);
  }
}

#undef MacroSparseToDense
#undef MacroScalarToDense

