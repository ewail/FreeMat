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
#ifndef __ArrayPrivate_hpp__
#define __ArrayPrivate_hpp__

#define MacroExpandCasesIntSigned(func)		\
  func(int64,Int64);				\
  func(int32,Int32);				\
  func(int16,Int16);				\
  func(int8,Int8);				

#define MacroExpandCasesIntUnsigned(func)	\
  func(uint64,UInt64);				\
  func(uint32,UInt32);				\
  func(uint16,UInt16);				\
  func(uint8,UInt8);				

#define MacroExpandCasesSigned(func)		\
  func(double,Double);				\
  func(float,Float);				\
  MacroExpandCasesIntSigned(func)		

#define MacroExpandCasesNoBool(func)		\
  MacroExpandCasesSigned(func)			\
  MacroExpandCasesIntUnsigned(func);		

#define MacroExpandCases(func)			\
  MacroExpandCasesNoBool(func);			\
  func(bool,Bool);				

#define MacroExpandCasesSimple(func)		\
  MacroExpandCases(func);			

#define MacroExpandCasesNoCell(func)		\
  MacroExpandCasesSimple(func);			\
  func(QChar,StringArray);

#define MacroExpandCasesAll(func)		\
  MacroExpandCasesNoCell(func);			\
  func(Array,CellArray);

#define MacroGetDataClass(ctype,cls)		\
  template <>					\
  inline DataClass GetDataClass(ctype c) { \
    return cls;					\
  } 

#define MacroArrayRealScalar(ctype,cls)			\
  template <>						\
  inline ctype & Array::realScalar() {			\
    if (dataClass() != cls)				\
      throw Exception("type mismatch");			\
    if (m_type.Scalar == 1)				\
      return m_real.cls;				\
    else if (m_type.Sparse == 1)			\
      return realSparse<ctype>()[1];			\
    else						\
      return real<ctype>()[1];				\
  }

#define MacroArrayImagScalar(ctype,cls)					\
  template <>								\
  inline ctype & Array::imagScalar() {					\
    if (cls == Bool)							\
      throw Exception("imaginary part not defined for logical types");	\
    if (dataClass() != cls)						\
      throw Exception("type mismatch");					\
    m_type.Complex = 1;							\
    if (m_type.Scalar == 1)						\
      return m_imag.cls;						\
    else if (m_type.Sparse == 1)					\
      return imagSparse<ctype>()[1];					\
    else								\
      return imag<ctype>()[1];						\
  }

#define MacroArrayConstRealScalar(ctype,cls)	\
  template <>					\
  inline ctype Array::constRealScalar() const {	\
    if (dataClass() != cls)			\
      throw Exception("type mismatch");		\
    if (m_type.Scalar == 1)			\
      return m_real.cls;			\
    else if (m_type.Sparse == 1)		\
      return constRealSparse<ctype>()[1];	\
    else					\
      return constReal<ctype>()[1];		\
  }

#define MacroArrayConstImagScalar(ctype,cls)				\
  template <>								\
  inline ctype Array::constImagScalar() const {				\
    if (cls == Bool)							\
      throw Exception("imaginary part not defined for logical types");	\
    if (dataClass() != cls)						\
      throw Exception("type mismatch");					\
    if (m_type.Complex == 0)						\
      return 0;								\
    if (m_type.Scalar == 1)						\
      return m_imag.cls;						\
    else if (m_type.Sparse == 1)					\
      return constImagSparse<ctype>()[1];				\
    else								\
      return constImag<ctype>()[1];					\
  }

#define MacroArrayComplexConstructor(ctype,cls)		     \
  template <>						     \
  inline Array::Array(ctype real, ctype imag) {		     \
    if (cls == Bool)					     \
      throw Exception("Complex logical values not allowed"); \
    m_real.cls = real; m_imag.cls = imag;		     \
    m_type.Class = cls;	 m_type.Complex = 1;		     \
    m_type.Sparse = 0; m_type.Scalar = 1;		     \
  };

#define MacroArrayRealConstructor(ctype,cls)				\
  template <>								\
  inline Array::Array(ctype real) {					\
    m_real.cls = real; m_imag.cls = 0;					\
    m_type.Class = cls;  m_type.Complex = 0;				\
    m_type.Sparse = 0;   m_type.Scalar = 1;				\
  };

#define MacroArrayRealScalarArrayEncodedOnly(ctype,cls) \
  template <>						\
  inline ctype& Array::realScalar() {			\
    if (dataClass() != cls)				\
      throw Exception("type mismatch");			\
    return real<ctype>()[1];				\
  }

#define MacroArrayImagScalarArrayEncodedOnly(ctype,cls) \
  template <>						\
  inline ctype& Array::imagScalar() {			\
    if (dataClass() !=  cls)				\
      throw Exception("type mismatch");			\
    return imag<ctype>()[1];				\
  }

#define MacroArrayConstRealScalarArrayEncodedOnly(ctype,cls)	\
  template <>							\
  inline ctype Array::constRealScalar() const {			\
    if (dataClass() != cls)					\
      throw Exception("type mismatch");				\
    return constReal<ctype>()[1];				\
  }

#define MacroArrayConstImagScalarArrayEncodedOnly(ctype,cls)	\
  template <>							\
  inline ctype Array::constImagScalar() const {			\
    if (dataClass() != cls)					\
      throw Exception("type mismatch");				\
    return constImag<ctype>()[1];				\
  }


MacroExpandCases(MacroArrayRealScalar);
MacroExpandCases(MacroArrayImagScalar);
MacroExpandCases(MacroArrayConstRealScalar);
MacroExpandCases(MacroArrayConstImagScalar);
MacroExpandCases(MacroArrayComplexConstructor);
MacroArrayRealScalarArrayEncodedOnly(Array,CellArray);
MacroArrayRealScalarArrayEncodedOnly(QChar,StringArray);
MacroArrayImagScalarArrayEncodedOnly(Array,CellArray);
MacroArrayImagScalarArrayEncodedOnly(QChar,StringArray);
MacroArrayConstRealScalarArrayEncodedOnly(Array,CellArray);
MacroArrayConstRealScalarArrayEncodedOnly(QChar,StringArray);
MacroArrayConstImagScalarArrayEncodedOnly(Array,CellArray);
MacroArrayConstImagScalarArrayEncodedOnly(QChar,StringArray);
MacroExpandCases(MacroArrayRealConstructor);
MacroExpandCases(MacroGetDataClass);
MacroGetDataClass(QChar,StringArray);
MacroGetDataClass(Array,CellArray);

#undef MacroArrayRealScalar
#undef MacroArrayImagScalar
#undef MacroArrayConstRealScalar
#undef MacroArrayConstImagScalar
#undef MacroArrayRealScalarArrayEncodedOnly
#undef MacroArrayImagScalarArrayEncodedOnly
#undef MacroArrayConstRealScalarArrayEncodedOnly
#undef MacroArrayConstImagScalarArrayEncodedOnly
#undef MacroArrayComplexConstructor
#undef MacroArrayRealConstructor
#undef MacroGetDataClass

#define MacroByteSize(ctype,cls)		\
  case cls: return sizeof(ctype);

inline size_t ByteSizeOfDataClass(DataClass x) {
  switch(x) {
  default:
    throw Exception("Unhandled dataclass for ByteSizeOfDataClass");
    MacroExpandCasesSimple(MacroByteSize);
  }
}

#define MacroSizeSimple(type)			\
  template <>					\
  inline index_t TSizeOf(type x) {		\
    return (index_t)(sizeof(x));		\
  }

MacroSizeSimple(bool);
MacroSizeSimple(int8);
MacroSizeSimple(int16);
MacroSizeSimple(int32);
MacroSizeSimple(int64);
MacroSizeSimple(uint8);
MacroSizeSimple(uint16);
MacroSizeSimple(uint32);
MacroSizeSimple(uint64);
MacroSizeSimple(float);
MacroSizeSimple(double);
MacroSizeSimple(QChar);

template <>
inline index_t TSizeOf(Array x) { 
  return x.bytes();
}

#endif
