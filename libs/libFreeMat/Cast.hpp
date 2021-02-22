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
#ifndef __Cast_hpp__
#define __Cast_hpp__
#include "Types.hpp"
#include "IEEEFP.hpp"

# define FMINT8_MIN		(-128)
# define FMINT16_MIN		(-32767-1)
# define FMINT32_MIN		(-2147483647-1)
# define FMINT64_MIN		(-Q_INT64_C(9223372036854775807)-1)
/* Maximum of signed integral types.  */
# define FMINT8_MAX		(127)
# define FMINT16_MAX		(32767)
# define FMINT32_MAX		(2147483647)
# define FMINT64_MAX		(Q_INT64_C(9223372036854775807))

/* Maximum of unsigned integral types.  */
# define FMUINT8_MAX		(255)
# define FMUINT16_MAX		(65535)
# define FMUINT32_MAX		(4294967295U)
# define FMUINT64_MAX		(Q_UINT64_C(18446744073709551615))


#if defined(_MSC_VER)  
double round( double x );
float roundf( float x );
#endif

template <typename T, typename S>
inline T CastConvert(S val);

#define MacroBoolIn(outtype)			\
  template <>					\
  inline outtype CastConvert(bool val) {	\
    if (val)					\
      return 1;					\
    else					\
      return 0;					\
  }

MacroBoolIn(int8);
MacroBoolIn(int16);
MacroBoolIn(int32);
MacroBoolIn(int64);
MacroBoolIn(uint8);
MacroBoolIn(uint16);
MacroBoolIn(uint32);
MacroBoolIn(uint64);
MacroBoolIn(float);
MacroBoolIn(double);

#define MacroBoolOut(intype)			\
  template <>					\
  inline bool CastConvert(intype val) {		\
    if (val == 0) return false;			\
    return true;				\
  }

MacroBoolOut(int8);
MacroBoolOut(int16);
MacroBoolOut(int32);
MacroBoolOut(int64);
MacroBoolOut(uint8);
MacroBoolOut(uint16);
MacroBoolOut(uint32);
MacroBoolOut(uint64);
MacroBoolOut(float);
MacroBoolOut(double);

#define MacroCastLimitXform(outtype,intype,lowlimit,hilimit,roundfunc)	\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (IsNaN(val)) return 0;						\
    if (val < lowlimit) return lowlimit;				\
    if (val > hilimit) return hilimit;					\
    return outtype(roundfunc(val));					\
  }

#define MacroCastLimitXformAll(source,round)				\
  MacroCastLimitXform(uint64,source,0,FMUINT64_MAX,round); \
  MacroCastLimitXform(uint32,source,0,FMUINT32_MAX,round);	\
  MacroCastLimitXform(uint16,source,0,FMUINT16_MAX,round);		\
  MacroCastLimitXform(uint8,source,0,FMUINT8_MAX,round);			\
  MacroCastLimitXform(int64,source,FMINT64_MIN,FMINT64_MAX,round); \
  MacroCastLimitXform(int32,source,FMINT32_MIN,FMINT32_MAX,round);	\
  MacroCastLimitXform(int16,source,FMINT16_MIN,FMINT16_MAX,round);	\
  MacroCastLimitXform(int8,source,FMINT8_MIN,FMINT8_MAX,round);		

MacroCastLimitXformAll(double,round);
MacroCastLimitXformAll(float,roundf);

#define MacroCastLimit(outtype,intype,lowlimit,hilimit)			\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (IsNaN(val)) return 0;						\
    if (val < lowlimit) return lowlimit;				\
    if (val > hilimit) return hilimit;					\
    return outtype(val);						\
  }

#define MacroCastUpLimit(outtype,intype,hilimit)			\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (IsNaN(val)) return 0;						\
    if (val > hilimit) return hilimit;					\
    return outtype(val);						\
  }

#define MacroCastPosLimit(outtype,intype)				\
  template <>								\
  inline outtype CastConvert(intype val) {				\
    if (IsNaN(val)) return 0;						\
    if (val < 0) return 0;						\
    return outtype(val);						\
  }

#define MacroCastNoOp(outtype,intype)		\
  template <>					\
  inline outtype CastConvert(intype val) {	\
    return outtype(val);			\
  }

#define MacroCastNoLimit(outtype,intype)		\
  template <>						\
  inline outtype CastConvert(intype val) {		\
    return outtype(val);				\
  }

MacroCastNoOp(uint64,uint64);
MacroCastUpLimit(uint32,uint64,FMUINT32_MAX);			
MacroCastUpLimit(uint16,uint64,FMUINT16_MAX);			
MacroCastUpLimit(uint8,uint64,FMUINT8_MAX);				
MacroCastUpLimit(int64,uint64,FMINT64_MAX); 
MacroCastUpLimit(int32,uint64,FMINT32_MAX);	
MacroCastUpLimit(int16,uint64,FMINT16_MAX);		
MacroCastUpLimit(int8,uint64,FMINT8_MAX);

MacroCastNoLimit(uint64,uint32);
MacroCastNoOp(uint32,uint32);
MacroCastUpLimit(uint16,uint32,FMUINT16_MAX);			
MacroCastUpLimit(uint8,uint32,FMUINT8_MAX);				
MacroCastNoLimit(int64,uint32);
MacroCastUpLimit(int32,uint32,FMINT32_MAX);	
MacroCastUpLimit(int16,uint32,FMINT16_MAX);		
MacroCastUpLimit(int8,uint32,FMINT8_MAX);

MacroCastNoLimit(uint64,uint16);
MacroCastNoLimit(uint32,uint16);
MacroCastNoOp(uint16,uint16);
MacroCastUpLimit(uint8,uint16,FMUINT8_MAX);				
MacroCastNoLimit(int64,uint16);
MacroCastNoLimit(int32,uint16);
MacroCastUpLimit(int16,uint16,FMINT16_MAX);		
MacroCastUpLimit(int8,uint16,FMINT8_MAX);

MacroCastNoLimit(uint64,uint8);
MacroCastNoLimit(uint32,uint8);
MacroCastNoLimit(uint16,uint8);
MacroCastNoOp(uint8,uint8);
MacroCastNoLimit(int64,uint8);
MacroCastNoLimit(int32,uint8);
MacroCastNoLimit(int16,uint8);
MacroCastUpLimit(int8,uint8,FMINT8_MAX);

MacroCastLimit(uint64,int64,0,FMINT64_MAX);	
MacroCastLimit(uint32,int64,0,FMUINT32_MAX);			
MacroCastLimit(uint16,int64,0,FMUINT16_MAX);			
MacroCastLimit(uint8,int64,0,FMUINT8_MAX);				
MacroCastNoOp(int64,int64);
MacroCastLimit(int32,int64,FMINT32_MIN,FMINT32_MAX);	
MacroCastLimit(int16,int64,FMINT16_MIN,FMINT16_MAX);		
MacroCastLimit(int8,int64,FMINT8_MIN,FMINT8_MAX);

MacroCastPosLimit(uint64,int32);	
MacroCastPosLimit(uint32,int32);
MacroCastLimit(uint16,int32,0,FMUINT16_MAX);			
MacroCastLimit(uint8,int32,0,FMUINT8_MAX);				
MacroCastNoLimit(int64,int32);
MacroCastNoOp(int32,int32);
MacroCastLimit(int16,int32,FMINT16_MIN,FMINT16_MAX);		
MacroCastLimit(int8,int32,FMINT8_MIN,FMINT8_MAX);

MacroCastPosLimit(uint64,int16);	
MacroCastPosLimit(uint32,int16);			
MacroCastPosLimit(uint16,int16);
MacroCastLimit(uint8,int16,0,FMUINT8_MAX);				
MacroCastNoLimit(int64,int16);
MacroCastNoLimit(int32,int16);
MacroCastNoOp(int16,int16);
MacroCastLimit(int8,int16,FMINT8_MIN,FMINT8_MAX);

MacroCastPosLimit(uint64,int8);
MacroCastPosLimit(uint32,int8);
MacroCastPosLimit(uint16,int8);
MacroCastPosLimit(uint8,int8);
MacroCastNoLimit(int64,int8);
MacroCastNoLimit(int32,int8);
MacroCastNoLimit(int16,int8);
MacroCastNoOp(int8,int8);

MacroCastNoLimit(float,double);
MacroCastNoLimit(double,float);

MacroCastNoLimit(float,int8);
MacroCastNoLimit(float,int16);
MacroCastNoLimit(float,int32);
MacroCastNoLimit(float,int64);

MacroCastNoLimit(float,uint8);
MacroCastNoLimit(float,uint16);
MacroCastNoLimit(float,uint32);
MacroCastNoLimit(float,uint64);

MacroCastNoLimit(double,int8);
MacroCastNoLimit(double,int16);
MacroCastNoLimit(double,int32);
MacroCastNoLimit(double,int64);

MacroCastNoLimit(double,uint8);
MacroCastNoLimit(double,uint16);
MacroCastNoLimit(double,uint32);
MacroCastNoLimit(double,uint64);

MacroCastNoOp(double,double);
MacroCastNoOp(float,float);
MacroCastNoOp(QChar,QChar);
MacroCastNoOp(bool,bool);

template <>
inline bool CastConvert(QChar val) {
  return (val != QChar(0));
}

template <>
inline QChar CastConvert(bool val) {
  if (val) return QChar(1); 
  return QChar(0);
}

template <>
inline uint8 CastConvert(QChar val) {
  return uint8(val.toAscii());
}

template <>
inline int8 CastConvert(QChar val) {
  return int8(val.toAscii());
}

#define MacroCastFromQChar(outtype)			\
  template <>						\
  inline outtype CastConvert(QChar val) {		\
    return outtype(val.unicode());			\
  }

MacroCastFromQChar(int16);
MacroCastFromQChar(int32);
MacroCastFromQChar(int64);
MacroCastFromQChar(uint16);
MacroCastFromQChar(uint32);
MacroCastFromQChar(uint64);
MacroCastFromQChar(float);
MacroCastFromQChar(double);

#define MacroCastToQChar(intype)			\
  template <>						\
  inline QChar CastConvert(intype val) {		\
    return QChar(uint32(val));				\
  }

MacroCastToQChar(uint8);

#define MacroCastToQCharLower(intype)			\
  template <>						\
  inline QChar CastConvert(intype val) {		\
    if (val < 0) val = 0;				\
    return QChar(uint32(val));				\
  }

MacroCastToQCharLower(int8);
MacroCastToQCharLower(int16);

#define MacroCastToQCharUpper(intype)			\
  template <>						\
  inline QChar CastConvert(intype val) {		\
    if (val > 16367) val = 16367;			\
    return QChar(uint32(val));				\
  }

MacroCastToQCharUpper(uint16);
MacroCastToQCharUpper(uint32);
MacroCastToQCharUpper(uint64);


#define MacroCastToQCharViaInt(intype)			\
  template <>						\
  inline QChar CastConvert(intype val) {		\
    if (val < 0) val = 0;				\
    if (val > 16367) val = 16367;			\
    return QChar(uint32(val));				\
  }

MacroCastToQCharViaInt(int32);
MacroCastToQCharViaInt(int64);
MacroCastToQCharViaInt(float);
MacroCastToQCharViaInt(double);

#endif
