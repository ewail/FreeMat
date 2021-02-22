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

#ifndef __Utils_hpp__
#define __Utils_hpp__

#include "Types.hpp"
#include "Array.hpp"
#include <QString>
#include <QFile>

bool contains(StringVector& list, QString s, bool regexpmode);

NTuple ArrayVectorAsDimensions(const ArrayVector &arg);

double ArrayRange(const Array& dp);
double ArrayMin(const Array& dp);
double ArrayMax(const Array& dp);

Array DoubleVectorFromQList(QList<uint32> &ref);

int digitvalue(char x);

void SwapBuffer(char* cp, int count, int elsize);

QString ReadQStringFromFile(QFile *fp);

void WriteQStringToFile(QFile *fp, QString txt);

double QFileReadInteger(QFile *fp, int base, int nMaxDigits = 1024 );

double QFileReadFloat(QFile *fp, int nMaxDigits = 1024 );

QString QFileReadString(QFile *fp, int nMaxChars = 65535 );

char QFileReadChar(QFile *fp);
#endif
