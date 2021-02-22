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

#include "Serialize.hpp"
#include "Exception.hpp"
#include "Algorithms.hpp"
#include "Struct.hpp"
#include <stdio.h>
#include "SparseCCS.hpp"
#include <QtEndian>

Serialize::Serialize(QIODevice *sck) {
  endianSwap = false;
  s = sck;
}

Serialize::~Serialize() {
}

void Serialize::handshakeServer() {
  try {
    char hand = 'A';
    s->write((const char*) &hand,sizeof(char));
    hand = 'Y';
    s->write((const char*) &hand,sizeof(char));
    hand = 'B';
    s->write((const char*) &hand,sizeof(char));
    hand = 'S';
    s->write((const char*) &hand,sizeof(char));
    unsigned short t;
    t = 1;
    s->write((const char *)&t,sizeof(short));
  } catch (Exception& e) {
    throw;
  }
}

void Serialize::handshakeClient() {
  try {
    char hand;
    s->read((char*) &hand,sizeof(char));
    if (hand != 'A')
      throw Exception("Handshaking error! Unable to establish serialization");
    s->read((char*) &hand,sizeof(char));
    if (hand != 'Y')
      throw Exception("Handshaking error! Unable to establish serialization");
    s->read((char*) &hand,sizeof(char));
    if (hand != 'B')
      throw Exception("Handshaking error! Unable to establish serialization");
    s->read((char*) &hand,sizeof(char));
    if (hand != 'S')
      throw Exception("Handshaking error! Unable to establish serialization");
    unsigned short t = 1;
    s->read((char*) &t,sizeof(short));
    if (t == 1)
      endianSwap = false;
    else if (t == 256)
      endianSwap = true;
    else
      throw Exception("Handshaking error! Unable to resolve byte ordering between server/client");    
  } catch (Exception &e) {
    throw ;
  }
}

void Serialize::sendSignature(char type, int count) {
  s->write((const char*) &type,1);
  int netcount;
  netcount = qToBigEndian(count);
  s->write((const char*) &netcount,sizeof(int));
}

void Serialize::checkSignature(char type, int count) {
  char rtype;
  int rcount;
  s->read((char*) &rtype,1);
  s->read((char*) &rcount,sizeof(int));
  rcount = qFromBigEndian(rcount);
  if (!((type == rtype) && (count == rcount))) {
    char buffer[1000];
    sprintf(buffer,"Serialization Mismatch: expected <%c,%d>, got <%c,%d>",
	    type,count,rtype,rcount);
    throw Exception(buffer);
  }
}

// Send a stream of base objects
void Serialize::putBytes(const char *ptr, int count) {
  sendSignature('c',count);
  s->write((const char*) ptr,count*sizeof(char));
}

void Serialize::putShorts(const short *ptr, int count) {
  sendSignature('s',count);
  s->write((const char*) ptr,count*sizeof(short));
}

void Serialize::putI64s(const int64 *ptr, int count) {
  sendSignature('z',count);
  s->write((const char*) ptr,count*sizeof(int64));
}

void Serialize::putInts(const int *ptr, int count) {
  sendSignature('i',count);
  s->write((const char*) ptr,count*sizeof(int));
}

void Serialize::putFloats(const float *ptr, int count) {
  sendSignature('f',count);
  s->write((const char*) ptr,count*sizeof(float));
}

void Serialize::putDoubles(const double *ptr, int count) {
  sendSignature('d',count);
  s->write((const char*) ptr,count*sizeof(double));
}

void Serialize::putString(QString p) {
  unsigned int len;
  sendSignature('x',0);
  len = p.size()+1;
  putInts((int*)&len,1);
  putBytes(qPrintable(p),len);
}

void Serialize::putBool(bool b) {
  if (b)
    putByte(1);
  else
    putByte(0);
}

bool Serialize::getBool() {
  char b;
  b = getByte();
  return (b == 1);
}

void Serialize::putByte(char t) {
  putBytes(&t,1);
}

void Serialize::putStringVector(StringVector t) {
  sendSignature('S',1);
  putInt(t.size());
  for (int i=0;i<t.size();i++)
    putString(t[i]);
}

StringVector Serialize::getStringVector() {
  checkSignature('S',1);
  int L = getInt();
  int i;
  StringVector N;
  for (i=0;i<L;i++)
    N.push_back(getString());
  return N;
}

void Serialize::putShort(short t) {
  putShorts(&t,1);
}

void Serialize::putInt(int t) {
  putInts(&t,1);
}

void Serialize::putFloat(float t) {
  putFloats(&t,1);
}

void Serialize::putDouble(double t) {
  putDoubles(&t,1);
}

void Serialize::getBytes(char *ptr, int count) {
  checkSignature('c',count);
  s->read((char*) ptr,count*sizeof(char));
}

#define SWAP(a,b) {tmp = a; a = b; b = tmp;}
void Serialize::getShorts(short *ptr, int count) {
  checkSignature('s',count);
  s->read((char*) ptr,count*sizeof(short));
  if (endianSwap) {
    char *cptr = (char *) ptr;
    char tmp;
    for (int i=0;i<2*count;i+=2)
      SWAP(cptr[i],cptr[i+1]);
  }
}

void Serialize::getI64s(int64 *ptr, int count) {
  checkSignature('z',count);
  s->read((char*) ptr,count*sizeof(int64));
  if (endianSwap) {
    char *cptr = (char *) ptr;
    char tmp;
    for (int i=0;i<8*count;i+=8) {
      SWAP(cptr[i],cptr[i+7]);
      SWAP(cptr[i+1],cptr[i+6]);
      SWAP(cptr[i+2],cptr[i+5]);
      SWAP(cptr[i+3],cptr[i+4]);
    }
  }
}

void Serialize::getInts(int *ptr, int count) {
  checkSignature('i',count);
  s->read((char*) ptr,count*sizeof(int));
  if (endianSwap) {
    char *cptr = (char *) ptr;
    char tmp;
    for (int i=0;i<4*count;i+=4) {
      SWAP(cptr[i],cptr[i+3]);
      SWAP(cptr[i+1],cptr[i+2]);
    }
  }
}

void Serialize::getFloats(float *ptr, int count) {
  checkSignature('f',count);
  s->read((char*) ptr,count*sizeof(float));
  if (endianSwap) {
    char *cptr = (char *) ptr;
    char tmp;
    for (int i=0;i<4*count;i+=4) {
      SWAP(cptr[i],cptr[i+3]);
      SWAP(cptr[i+1],cptr[i+2]);
    }
  }
}

void Serialize::getDoubles(double *ptr, int count) {
  checkSignature('d',count);
  s->read((char*)ptr,count*sizeof(double));
  if (endianSwap) {
    char *cptr = (char *) ptr;
    char tmp;
    for (int i=0;i<8*count;i+=8) {
      SWAP(cptr[i],cptr[i+7]);
      SWAP(cptr[i+1],cptr[i+6]);
      SWAP(cptr[i+2],cptr[i+5]);
      SWAP(cptr[i+3],cptr[i+4]);
    }
  }
}

QString Serialize::getString() {
  checkSignature('x',0);
  unsigned int len;
  getInts((int*) &len,1);
  if (len == 0) return QString();
  char *cp = (char*) malloc(len*sizeof(char));
  getBytes(cp,len);
  QString ret(cp);
  free(cp);
  return ret;
}

char Serialize::getByte() {
  char t;
  getBytes(&t,1);
  return t;
}

short Serialize::getShort() {
  short t;
  getShorts(&t,1);
  return t;
}

int Serialize::getInt() {
  int t;
  getInts(&t,1);
  return t;
}

float Serialize::getFloat() {
  float t;
  getFloats(&t,1);
  return t;
}

double Serialize::getDouble() {
  double t;
  getDoubles(&t,1);
  return t;
}

DataClass Serialize::getDataClass(bool& sparseflag, QString& className, bool& complexflag) {
  checkSignature('a',1);
  unsigned char a = getByte();
  sparseflag = (a & 16) > 0;
  complexflag = false;
  // For compatibility reasons, the sparse flag is stuck at
  // 16.  Which is binary:
  //   0001 0000
  // To mask out this bit, we need the following number:
  //   1110 1111 = 255 - 16 = 239
  a = a & 239;
  switch (a) {
  case 1:
    return CellArray;
  case 2:
    return Struct;
  case 3:
    return Bool;
  case 4:
    return UInt8;
  case 5:
    return Int8;
  case 6:
    return UInt16;
  case 7:
    return Int16;
  case 8:
    return UInt32;
  case 9:
    return Int32;
  case 32:
    return UInt64;
  case 33:
    return Int64;
  case 10:
    return Float;
  case 11:
    return Double;
  case 12:
    complexflag = true;
    return Float;
  case 13:
    complexflag = true;
    return Double;
  case 14:
    return StringArray;
  case 128: {
    int cnt(getInt());
    for (int i=0;i<cnt;i++)
      className.push_back(getString());
    return Struct;
  }    
  default:
    throw Exception("Unrecognized array type received!");
  }
}

void Serialize::putDataClass(DataClass cls, bool issparse, bool isuserclass, StringVector className, bool complexflag) {
  char sparseval;
  sparseval = issparse ? 16 : 0;
  sendSignature('a',1);
  switch (cls) {
  default:
    throw Exception("Unhandled type in putDataClass.");
  case CellArray:
    putByte(1);
    return;
  case Struct:
    if (!isuserclass)
      putByte(2);
    else {
      putByte(128);
      putInt(className.size());
      for (int i=0;i<className.size();i++)
	putString(className.at(i));
    }
    return;
  case Bool:
    putByte(3);
    return;
  case UInt8:
    if (complexflag) throw Exception("complex uint8 not supported");
    if (sparseval) throw Exception("sparse uint8 not supported");
    putByte(4);
    return;
  case Int8:
    if (complexflag) throw Exception("complex int8 not supported");
    if (sparseval) throw Exception("sparse int8 not supported");
    putByte(5);
    return;
  case UInt16:
    if (complexflag) throw Exception("complex uint16 not supported");
    if (sparseval) throw Exception("sparse uint16 not supported");
    putByte(6);
    return;
  case Int16:
    if (complexflag) throw Exception("complex int16 not supported");
    if (sparseval) throw Exception("sparse int16 not supported");
    putByte(7);
    return;
  case UInt32:
    if (complexflag) throw Exception("complex uint32 not supported");
    if (sparseval) throw Exception("sparse uint32 not supported");
    putByte(8);
    return;
  case Int32:
    if (complexflag) throw Exception("complex int32 not supported");
    putByte(9 | sparseval);
    return;
  case UInt64:
    if (complexflag) throw Exception("complex uint64 not supported");
    if (sparseval) throw Exception("sparse uint64 not supported");
    putByte(32);
    return;
  case Int64:
    if (complexflag) throw Exception("complex int64 not supported");
    putByte(33 | sparseval);
    return;
  case Float:
    if (complexflag) {
      putByte(12 | sparseval);
    } else {
      putByte(10 | sparseval);
    }
    return;
  case Double:
    if (complexflag) {
      putByte(13 | sparseval);
    } else {
      putByte(11 | sparseval);
    }
    return;
  case StringArray:
    putByte(14);
    return;
  }
}

void Serialize::putDimensions(const NTuple& dim) {
  sendSignature('D',1);
  putInt(dim.lastNotOne());
  for (int i=0;i<dim.lastNotOne();i++)
    putInt(int(dim[i]));
}

NTuple Serialize::getDimensions() {
  checkSignature('D',1);
  NTuple dim;
  int len;
  len = getInt();
  for (int i=0;i<len;i++)
    dim[i] = getInt();
  return dim;
}

#define MacroPutCase(func,ctype,class,ftype)				\
  {									\
    const Array &t(dat.asDenseArray().toClass(class));			\
    func((const ctype*)t.constReal<ftype>().constData(),int(dat.length())); \
    return;								\
  }


#define MacroPutComplexCase(func,ctype,class,ftype)			\
  {									\
    const Array &t(dat.asDenseArray().toClass(class)); \
    func((const ctype*)t.fortran<ftype>().constData(),int(2*dat.length()));		\
    return;								\
  }


void Serialize::putArray(const Array& dat) {
  sendSignature('A',1);
  DataClass dclass(dat.dataClass());
  putDataClass(dclass,dat.isSparse(),dat.isUserClass(),StringVector(dat.className()),dat.isComplex());
  putDimensions(dat.dimensions());
  if (dat.isEmpty()) return;
  switch(dclass) {
  default: throw Exception("unhandled type in putArray");
  case CellArray: {
    const BasicArray<Array> &rp(dat.constReal<Array>());
    for (index_t i=1;i<=rp.length();i++)
      putArray(rp[i]);
    return;
  }
  case Struct: {    
    StringVector fnames(FieldNames(dat));
    int ncount(fnames.size());
    putInt(ncount);
    for (int i=0;i<ncount;i++)
      putString(fnames.at(i));
    const StructArray &rp(dat.constStructPtr());
    for (index_t j=1;j<=dat.length();j++)
      for (int i=0;i<ncount;i++) {
	const BasicArray<Array>& dp(rp[i]);
	putArray(dp[j]);
      }
    return;
  }
  case Bool: MacroPutCase(putBytes,char,Int8,int8);
  case StringArray: MacroPutCase(putBytes,char,UInt8,uint8);
  case UInt8: MacroPutCase(putBytes,char,UInt8,uint8);
  case UInt16: MacroPutCase(putShorts,short,UInt16,uint16);
  case UInt32: MacroPutCase(putInts,int,UInt32,uint32);
  case UInt64: MacroPutCase(putI64s,int64,UInt64,uint64);
  case Int8: MacroPutCase(putBytes,char,Int8,int8);
  case Int16: MacroPutCase(putShorts,short,Int16,int16);
  case Int32: 
    if (dat.isSparse()) {
      QVector<QVector<int32> > dp(SparseFM3(dat.constRealSparse<int32>()));
      for (int i=0;i<dat.cols();i++) {
	putInt(1+dp[i][0]);
	putInts((const int*) dp[i].constData(),int(1+dp[i][0]));
      } 
      return;
    } else
       MacroPutCase(putInts,int,Int32,int32);
  case Int64: MacroPutCase(putI64s,int64,Int64,int64);
  case Float: 
    if (dat.allReal()) {
      if (dat.isSparse()) {
	QVector<QVector<float> > dp(SparseFM3(dat.constRealSparse<float>()));
	for (int i=0;i<dat.cols();i++) {
	  putFloat(1+dp[i][0]);
	  putFloats((const float*) dp[i].constData(),int(1+dp[i][0]));
	} 
	return;
      } else
	MacroPutCase(putFloats,float,Float,float);
    } else {
      if (dat.isSparse()) {
	QVector<QVector<float> > dp(SparseFM3(dat.constRealSparse<float>(),dat.constImagSparse<float>()));
	for (int i=0;i<dat.cols();i++) {
	  putFloat(1+dp[i][0]);
	  putFloats((const float*) dp[i].constData(),int(1+dp[i][0]));
	} 	
      } else
	MacroPutComplexCase(putFloats,float,Float,float);
    }
  case Double:
    if (dat.allReal()) {
      if (dat.isSparse()) {
	QVector<QVector<double> > dp(SparseFM3(dat.constRealSparse<double>()));
	for (int i=0;i<dat.cols();i++) {
	  putDouble(1+dp[i][0]);
	  putDoubles((const double*) dp[i].constData(),int(1+dp[i][0]));
	} 
	return;
      } else
	MacroPutCase(putDoubles,double,Double,double);
    } else {
      if (dat.isSparse()) {
	QVector<QVector<double> > dp(SparseFM3(dat.constRealSparse<double>(),dat.constImagSparse<double>()));
	for (int i=0;i<dat.cols();i++) {
	  putDouble(1+dp[i][0]);
	  putDoubles((const double*) dp[i].constData(),int(1+dp[i][0]));
	} 	
      } else
	MacroPutComplexCase(putDoubles,double,Double,double);
    }
  }
}

#define MacroGetCase(func,ctype,class,ftype)	\
  {						\
    BasicArray<ftype> rp(dims);			\
    func((ctype*) rp.data(),int(dims.count()));	\
    dat = Array(rp).toClass(class);		\
    return;					\
  }

#define MacroGetComplexCase(func,ctype,class,ftype)		\
  {								\
    BasicArray<ftype> rp(dims.replace(0,dims[0]*2));		\
    func((ctype*) rp.data(),int(dat.length()));			\
    dat = Array(SplitReal(rp),SplitImag(rp)).toClass(class);	\
    return;							\
  }

void Serialize::getArray(Array& dat) {
  checkSignature('A',1);
  bool sparseflag;
  QString className;
  bool complexflag;
  DataClass dclass(getDataClass(sparseflag,className,complexflag));
  NTuple dims(getDimensions());
  int elCount(int(dims.count()));
  if (elCount == 0) {
    dat = EmptyConstructor();
    return;
  }
  switch(dclass) {
  default:
    throw Exception("Unhandled type in getArray");
  case CellArray: {
    BasicArray<Array> rp(dims);
    for (index_t i=1;i<=rp.length();i++)
      getArray(rp[i]);
    dat = Array(rp);
    return;
  }
  case Struct: {
    StringVector fnames;
    int ncount(getInt());
    StructArray rp;
    for (int i=0;i<ncount;i++) 
      rp.insert(getString(),BasicArray<Array>(dims));
    rp.setClassName(className);
    for (index_t j=1;j<=dims.count();j++)
      for (int i=0;i<ncount;i++)
	getArray(rp[i][j]);
    rp.updateDims();
    dat = Array(rp);
    return;
  }
  case Bool: MacroGetCase(getBytes,char,Bool,int8);
  case StringArray: MacroGetCase(getBytes,char,StringArray,uint8);
  case UInt8: MacroGetCase(getBytes,char,UInt8,uint8);
  case UInt16: MacroGetCase(getShorts,short,UInt16,uint16);
  case UInt32: MacroGetCase(getInts,int,UInt32,uint32);
  case UInt64: MacroGetCase(getI64s,int64,UInt64,uint64);
  case Int64: MacroGetCase(getI64s,int64,Int64,int64);
  case Int32:
    if (sparseflag) {
      QVector<QVector<int32> > dp;
      for (index_t i=1;i<=dims.cols();i++) {
	int len = getInt();
	QVector<int32> col(len);
	getInts(col.data(),len);
	dp << col;
      }
      dat = FM3Sparse(dp,dims);
      return;
    } else
      MacroGetCase(getInts,int,Int32,int32);
  case Float:
    if (!complexflag) {
      if (sparseflag) {
	QVector<QVector<float> > dp;
	for (index_t i=1;i<=dims.cols();i++) {
	  int len = int(getFloat());
	  QVector<float> col(len);
	  getFloats(col.data(),len);
	  dp << col;
	}
	dat = FM3Sparse(dp,dims);
	return;
      } else
	MacroGetCase(getFloats,float,Float,float);
    } else {
      if (sparseflag) {
	QVector<QVector<float> > dp;
	for (index_t i=1;i<=dims.cols();i++) {
	  int len = int(getFloat());
	  QVector<float> col(len);
	  getFloats(col.data(),len);
	  dp << col;
	}
	dat = FM3SparseComplex(dp,dims);
	return;
      } else
	MacroGetComplexCase(getFloats,float,Float,float);
    }
  case Double:
    if (!complexflag) {
      if (sparseflag) {
	QVector<QVector<double> > dp;
	for (index_t i=1;i<=dims.cols();i++) {
	  int len = int(getDouble());
	  QVector<double> col(len);
	  getDoubles(col.data(),len);
	  dp << col;
	}
	dat = FM3Sparse(dp,dims);
	return;
      } else
	MacroGetCase(getDoubles,double,Double,double);
    } else {
      if (sparseflag) {
	QVector<QVector<double> > dp;
	for (index_t i=1;i<=dims.cols();i++) {
	  int len = int(getDouble());
	  QVector<double> col(len);
	  getDoubles(col.data(),len);
	  dp << col;
	}
	dat = FM3SparseComplex(dp,dims);
	return;
      } else
	MacroGetComplexCase(getDoubles,double,Double,double);
    }
  }
}

			      
			      
