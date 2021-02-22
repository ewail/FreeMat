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
#ifndef __IEEEFP_HPP__
#define __IEEEFP_HPP__

#include "Types.hpp"

bool IsInfinite(float t);
bool IsInfinite(double t);
bool IsInfinite(int8);
bool IsInfinite(int16);
bool IsInfinite(int32);
bool IsInfinite(int64);
bool IsInfinite(uint8);
bool IsInfinite(uint16);
bool IsInfinite(uint32);
bool IsInfinite(uint64);
bool IsNaN(int32 t);
bool IsNaN(int64 t);
bool IsNaN(uint32 t);
bool IsNaN(uint64 t);
bool IsNaN(double t);
bool IsFinite(float t);
bool IsFinite(double t);
bool IsFinite(int8);
bool IsFinite(int16);
bool IsFinite(int32);
bool IsFinite(int64);
bool IsFinite(uint8);
bool IsFinite(uint16);
bool IsFinite(uint32);
bool IsFinite(uint64);

QString ToHexString(float t);
QString ToHexString(double t);
double NaN(void);
double Inf(void);

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#include <float.h>
#include <math.h>

#if defined(_MSC_VER )

extern "C"{
	__declspec( dllexport ) double rint (double x);
	__declspec( dllexport ) float rintf (float x);
	__declspec( dllexport ) double log1p(double x);
	__declspec( dllexport ) double expm1 (double x);
//        __declspec( dllexport ) double nextafter( double x, double y )  { return _nextafter( x, y ); }
	__declspec( dllexport ) float nextafterf (float x, float y);
	__declspec( dllexport ) float log1pf(float x);
	__declspec( dllexport ) float expm1f(float x);
	__declspec( dllexport ) float asinhf(float x);
	__declspec( dllexport ) double asinh(double x);
	__declspec( dllexport ) double acosh (double x);
	__declspec( dllexport ) float acoshf (float x);
	__declspec( dllexport ) double atanh(double x);
	__declspec( dllexport ) float atanhf (float x);
}
#define nextafter _nextafter
#endif

//returns distance to the next nearest double value
inline double feps( double x ){
  if (x >= 0)
    return nextafter( x , DBL_MAX ) - x;
  else
    return x - nextafter( x, -DBL_MAX);
}

//returns distance to the next nearest float value
inline float fepsf( float x ){
  if (x >= 0)
    return nextafterf( x , FLT_MAX ) - x;
  else
    return x - nextafterf(x, -FLT_MAX);
}

#endif
