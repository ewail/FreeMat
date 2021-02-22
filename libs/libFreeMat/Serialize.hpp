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

#ifndef __Serialize_hpp__
#define __Serialize_hpp__

#include "Stream.hpp"
#include "Array.hpp"
#include <QIODevice>

// This is the cross-platform serialization object...

class Serialize {
  QIODevice *s;
  bool endianSwap;
public:
  Serialize(QIODevice*);
  ~Serialize();
  // Handshake between two serialize objects
  void handshakeServer();
  void handshakeClient();
  // Send a signature
  void sendSignature(char sig, int count);
  // Check a signature
  void checkSignature(char sig, int count);
  // Send a stream of base objects
  void putBytes(const char* ptr, int count);
  void putShorts(const short* ptr, int count);
  void putInts(const int* ptr, int count);
  void putI64s(const int64* ptr, int count);
  void putFloats(const float* ptr, int count);
  void putDoubles(const double* ptr, int count);
  // Send a scalar object
  void putString(QString t);
  void putByte(char t);
  void putShort(short t);
  void putInt(int t);
  void putFloat(float t);
  void putDouble(double t);
  void putBool(bool t);
  void putStringVector(StringVector t);
  // Receive a stream of base objects
  void getBytes(char* ptr, int count);
  void getShorts(short* ptr, int count);
  void getInts(int* ptr, int count);
  void getI64s(int64* ptr, int count);
  void getFloats(float* ptr, int count);
  void getDoubles(double* ptr, int count);
  // Get a scalar object
  QString  getString();
  char   getByte();
  short  getShort();
  int    getInt();
  float  getFloat();
  double getDouble();
  bool   getBool();
  StringVector getStringVector();
  // Put an array
  void putDataClass(DataClass cls, bool sparseflag, bool isuserclass, StringVector className, bool complexflag);
  void putArray(const Array& dat);
  void putDimensions(const NTuple& dim);
  // Get an array
  DataClass getDataClass(bool& sparseflag, QString& className, bool& complexflag);
  void getArray(Array& dat);
  NTuple getDimensions();
};

#endif
