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

#ifndef __Types_hpp__
#define __Types_hpp__

#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QList>
#include <QRegExp>
#include <cmath>
#include "FastList.hpp"

typedef bool      logical;
typedef qint8     int8;
typedef quint8    uint8;
typedef qint16    int16;
typedef quint16   uint16;
typedef qint32    int32;
typedef quint32   uint32;
typedef qint64    int64;
typedef quint64   uint64;
typedef double index_t;
const int NDims = 6;

typedef QStringList StringVector;

#if defined(_MSC_VER )
extern "C"{
	__declspec( dllexport ) double rint (double x);
	__declspec( dllexport ) float rintf (float x);
};
#endif

inline bool IsInteger(float x) {
  return ((fabsf(x) < 8388608) && (rintf(x) == x));
}

inline bool IsInteger(double x) {
  return ((fabs(x) < Q_UINT64_C(4503599627370496)) && (rint(x) == x));
}

inline bool IsInteger(int8 x) { return true; }
inline bool IsInteger(uint8 x) { return true;}
inline bool IsInteger(int16 x) { return true; }
inline bool IsInteger(uint16 x) { return true;}
inline bool IsInteger(int32 x) { return true; }
inline bool IsInteger(uint32 x) { return true;}
inline bool IsInteger(int64 x) { return true; }
inline bool IsInteger(uint64 x) { return true;}
inline bool IsInteger(bool x) { return true; }

template <typename T>
inline bool IsNonNegative(T x) {
  return (x >= 0);
}

template <typename T>
inline index_t TSizeOf(T x);


#endif
