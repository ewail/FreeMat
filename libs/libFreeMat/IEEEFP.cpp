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
#include "IEEEFP.hpp"

static bool endianDetected = false;
static bool bigEndian = false;

union lc_t {
  long l;
  char c[sizeof (long)];
} u;

void CheckBigEndian() {
  u.l = 1;
  endianDetected = true;
  bigEndian = (u.c[sizeof(long) - 1] == 1);
  
}

bool IsInfinite(float t) {
  union {
    float f;
    unsigned int i;
  } u;             
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) == 0))
    return true;
  return false;
}

bool IsInfinite(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;
  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) == 0) && (u.i[0] == 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) == 0) && (u.i[1] == 0)))
      return true;
  }
  return false;

}

bool IsInfinite(int8) {return false;}
bool IsInfinite(int16) {return false;}
bool IsInfinite(int32) {return false;}
bool IsInfinite(int64) {return false;}
bool IsInfinite(uint8) {return false;}
bool IsInfinite(uint16) {return false;}
bool IsInfinite(uint32) {return false;}
bool IsInfinite(uint64) {return false;}

bool IsFinite(int8) {return true;}
bool IsFinite(int16) {return true;}
bool IsFinite(int32) {return true;}
bool IsFinite(int64) {return true;}
bool IsFinite(uint8) {return true;}
bool IsFinite(uint16) {return true;}
bool IsFinite(uint32) {return true;}
bool IsFinite(uint64) {return true;}

bool IsNaN(int32 t) {
  return false;
}

bool IsNaN(int64 t) {
  return false;
}

bool IsNaN(uint32 t) {
  return false;
}

bool IsNaN(uint64 t) {
  return false;
}

bool IsNaN(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) != 0))
    return true; 
  return false;
}

bool IsNaN(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;

  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) != 0) || (u.i[0] != 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) != 0) || (u.i[1] != 0)))
      return true;
  }
  return false;
}

bool IsFinite(float t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}

bool IsFinite(double t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}

double NaN() {
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7fC00000;
  return u.f;
}

double Inf() {
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7f800000;
  return u.f;
}

QString ToHexString(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  if (IsNaN(t))
    return QString("fff80000");
  u.f = t;
  return QString("%1").arg(uint(u.i),int(8),int(16),QChar('0'));
}

QString ToHexString(double t) {
  union {
    double f;
    unsigned int i[2];
  } u;
  if (IsNaN(t))
    return QString("fff8000000000000");
  u.f = t;
  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) 
    return QString("%1%2").arg(uint(u.i[1]),int(8),int(16),QChar('0')).arg(uint(u.i[0]),int(8),int(16),QChar('0'));
  else
    return QString("%1%2").arg(uint(u.i[0]),int(8),int(16),QChar('0')).arg(uint(u.i[1]),int(8),int(16),QChar('0'));
}


#if defined(_MSC_VER )

#include <math.h>

extern "C" { 
__declspec( dllexport ) double rint (double x)
{
  __asm{
	  fld x
	  frndint
	  fstp x
  };
  return x;
}

__declspec( dllexport ) float rintf( float x ) { return rint(x); }

//VC doesn't define log1p so we borrow the definition from libm (mingw)
__declspec( dllexport ) double log1p(double x){

	const double limit = 0.29;
	const double one = 1.0;

	__asm{
	/* BASED ON log1p.S from mingw
	 * Written by J.T. Conklin <jtc@netbsd.org>.
	 * Public domain.
	 * Removed header file dependency for use in libmingwex.a by
	 *   Danny Smith <dannysmith@users.sourceforge.net>
	 */

			/* The fyl2xp1 can only be used for values in
			   -1 + sqrt(2) / 2 <= x <= 1 - sqrt(2) / 2
			   0.29 is a safe value.
			 */
			fldln2
			fld     x //  [esp+4]
			fxam
			fnstsw	ax
			fld        st
			sahf
			jc        l3        // in case x is NaN or �Inf

	l4:      
			fabs
			fcomp        limit
			fnstsw	ax
			sahf
			jc        l2
			fadd        one
			fyl2x
			fstp x
			jmp l5

	l2:      
			fyl2xp1
			fstp x
			jmp l5

	l3:      
			jp        l4        // in case x is �Inf
			fstp        st(1)
			fstp        st(1)
	l5:
	};
return x;
}

const long double LOGE2L  = 6.9314718055994530941723E-1L;
const long double LOG2EL  = 1.4426950408889634073599E0L;
/* BASED ON log1p.S from mingw
 * Written 2005 by Gregory W. Chicares <chicares@cox.net>.
 * Adapted to double by Danny Smith <dannysmith@users.sourceforge.net>. 
 * Public domain.
 */
__declspec( dllexport ) double expm1 (double x)
{
  if (fabs(x) < LOGE2L)
    {
      x *= LOG2EL;
	  __asm{ 
		  fld x
		  f2xm1 
		  fstp x
	  };

      return x;
    }
  else
    return exp(x) - 1.0;
}

__declspec( dllexport ) float nextafterf (float x, float y)
{
	union
	{
		float f;
		unsigned int i;
	} u;
	if (_isnan (y) || _isnan (x))
		return x + y;
	if (x == y )
		/* nextafter (0.0, -O.0) should return -0.0.  */
		return y;
	u.f = x; 
	if (x == 0.0F)
	{
		u.i = 1;
		return y > 0.0F ? u.f : -u.f;
	}
	if (((x > 0.0F) ^ (y > x)) == 0)
		u.i++;
	else
		u.i--;
	return u.f;
}

//TODO: implement actual floating point log1pf, expm1f
__declspec( dllexport ) float log1pf (float x){
    return log1p( x );
}
__declspec( dllexport ) float expm1f (float x){
    return expm1( x );
}

//Based on asinhf routine  from cygwin
/* asinh(x) = copysign(log(fabs(x) + sqrt(x * x + 1.0)), x) */
__declspec( dllexport ) float asinhf(float x)
{
  float z;
  if (!_finite (x))
    return x;
  z = fabsf (x);

  /* Use log1p to avoid cancellation with small x. Put
     x * x in denom, so overflow is harmless. 
     asinh(x) = log1p (x + sqrt (x * x + 1.0) - 1.0)
              = log1p (x + x * x / (sqrt (x * x + 1.0) + 1.0))  */

  z = log1p (z + z * z / (sqrt (z * z + 1.0) + 1.0));

  return ( x > 0.0 ? z : -z);
}

//Based on asinh routine  from cygwin
/* asinh(x) = copysign(log(fabs(x) + sqrt(x * x + 1.0)), x) */
__declspec( dllexport ) double asinh(double x)
{
  double z;
  if (!_finite (x))
    return x;
  z = fabs (x);

  /* Use log1p to avoid cancellation with small x. Put
     x * x in denom, so overflow is harmless. 
     asinh(x) = log1p (x + sqrt (x * x + 1.0) - 1.0)
              = log1p (x + x * x / (sqrt (x * x + 1.0) + 1.0))  */

  z = log1p (z + z * z / (sqrt (z * z + 1.0) + 1.0));

  return ( x > 0.0 ? z : -z);
}

//Based on routine  from cygwin
/* acosh(x) = log (x + sqrt(x * x - 1)) */
__declspec( dllexport ) double acosh (double x)
{
  if (_isnan (x)) 
    return x;

  if (x < 1.0)
    {
      errno = EDOM;
      return NaN();
    }

  if (x > 4294967296.f)
    /*  Avoid overflow (and unnecessary calculation when
        sqrt (x * x - 1) == x). GCC optimizes by replacing
        the long double M_LN2 const with a fldln2 insn.  */ 
    return log (x) + 6.9314718055994530941723E-1L;

  /* Since  x >= 1, the arg to log will always be greater than
     the fyl2xp1 limit (approx 0.29) so just use logl. */ 
  return log (x + sqrt((x + 1.0) * (x - 1.0)));
}

//Based on routine  from cygwin
/* acosh(x) = log (x + sqrt(x * x - 1)) */
__declspec( dllexport ) float acoshf (float x)
{
  if (_isnan (x)) 
    return x;
  if (x < 1.0f)
    {
      errno = EDOM;
      return NaN();
    }

 if (x > 4294967296.f)
    /*  Avoid overflow (and unnecessary calculation when
        sqrt (x * x - 1) == x). GCC optimizes by replacing
        the long double M_LN2 const with a fldln2 insn.  */ 
    return log (x) + 6.9314718055994530941723E-1L;

  /* Since  x >= 1, the arg to log will always be greater than
     the fyl2xp1 limit (approx 0.29) so just use logl. */ 
  return log (x + sqrt((x + 1.0) * (x - 1.0)));
}

//Based on routine  from cygwin
/* atanh (x) = 0.5 * log ((1.0 + x)/(1.0 - x)) */
__declspec( dllexport ) double atanh(double x)
{
  double z;
  if( _isnan (x))
    return x;
  z = fabs (x);
  if (z == 1.0)
    {
      errno  = ERANGE;
      return (x > 0 ? Inf() : -Inf());
    }
  if (z > 1.0)
    {
      errno = EDOM;
      return NaN();
    }
  /* Rearrange formula to avoid precision loss for small x.

  atanh(x) = 0.5 * log ((1.0 + x)/(1.0 - x))
           = 0.5 * log1p ((1.0 + x)/(1.0 - x) - 1.0)
           = 0.5 * log1p ((1.0 + x - 1.0 + x) /(1.0 - x)) 
           = 0.5 * log1p ((2.0 * x ) / (1.0 - x))  */
  z = 0.5 * log1p ((z + z) / (1.0 - z));
  return x >= 0 ? z : -z;
}

//Based on routine  from cygwin
/* atanh (x) = 0.5 * log ((1.0 + x)/(1.0 - x)) */
__declspec( dllexport ) float atanhf (float x)
{
  float z;
  if( _isnan (x))
    return x;
  z = fabsf (x);
  if (z == 1.0)
    {
      errno  = ERANGE;
      return (x > 0 ? Inf() : -Inf());
    }
  if ( z > 1.0)
    {
      errno = EDOM;
      return NaN();
    }
  /* Rearrange formula to avoid precision loss for small x.

  atanh(x) = 0.5 * log ((1.0 + x)/(1.0 - x))
           = 0.5 * log1p ((1.0 + x)/(1.0 - x) - 1.0)
           = 0.5 * log1p ((1.0 + x - 1.0 + x) /(1.0 - x)) 
           = 0.5 * log1p ((2.0 * x ) / (1.0 - x))  */
  z = 0.5 * log1p ((z + z) / (1.0 - z));
  return x >= 0 ? z : -z;
}


}


#endif 
