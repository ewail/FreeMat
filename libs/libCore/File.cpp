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

#include <QtCore>
#include "Array.hpp"
#include "HandleList.hpp"
#include "IEEEFP.hpp"
#include "MemPtr.hpp"
#include "Printf.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"

HandleList<FilePtr*> fileHandles;

static bool init = false;

void InitializeFileSubsystem() {
  if (init) 
    return;
  FilePtr *fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stdin,QIODevice::ReadOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stdout,QIODevice::WriteOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stderr,QIODevice::WriteOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  init = true;
}


#define MATCH(x) (prec==x)

static DataClass processPrecisionString(QString prec) {
  prec = prec.trimmed().toLower();
  if (MATCH("uint8") || MATCH("uchar") || MATCH("unsigned char"))
    return UInt8;
  if (MATCH("int8") || MATCH("char") || MATCH("integer*1"))
    return Int8;
  if (MATCH("uint16") || MATCH("unsigned short")) 
    return UInt16;
  if (MATCH("int16") || MATCH("short") || MATCH("integer*2"))
    return Int16;
  if (MATCH("uint32") || MATCH("unsigned int")) 
    return UInt32;
  if (MATCH("int32") || MATCH("int") || MATCH("integer*4"))
    return Int32;
  if (MATCH("uint64"))
    return UInt64;
  if (MATCH("int64") || MATCH("integer*8"))
    return Int64;
  if (MATCH("single") || MATCH("float32") || MATCH("float") || MATCH("real*4"))
    return Float;
  if (MATCH("double") || MATCH("float64") || MATCH("real*8"))
    return Double;
  throw Exception("invalid precision type");
}
#undef MATCH

void ComputePrecisionString(QString cmd, DataClass &in, DataClass &out) {
  // Check for type => type
  QRegExp rxlen("(.*)=>(.*)");
  int pos = rxlen.indexIn(cmd);
  if (pos > -1) {
    in = processPrecisionString(rxlen.cap(1));
    out = processPrecisionString(rxlen.cap(2));
    if (rxlen.cap(2).trimmed().toLower() == "char") out = StringArray;
    return;
  }
  if (cmd.startsWith("*")) {
    cmd.remove(0,1);
    in = processPrecisionString(cmd);
    out = in;
    if (cmd.trimmed().toLower() == "char") out = StringArray;
    return;
  }
  in = processPrecisionString(cmd);
  out = Double;
}

//@@Signature
//function fopen FopenFunction
//inputs fname mode byteorder
//outputs handle message
//DOCBLOCK io_fopen
ArrayVector FopenFunction(int nargout, const ArrayVector& arg) {
  try {
    uint32 testEndian = 0xFEEDFACE;
    uint8 *dp;
    bool bigEndian;
    
    dp = (uint8*) &testEndian;
    bigEndian = (dp[0] == 0xFE);
    
    if (arg.size() > 3)
      throw Exception("too many arguments to fopen");
    if (arg.size() < 1)
      throw Exception("fopen requires at least one argument (a filename)");
    if (!(arg[0].isString()))
      throw Exception("First argument to fopen must be a filename");
    QString fname = arg[0].asString();
    QString mode = "r";
    if (arg.size() > 1) {
      if (!arg[1].isString())
	throw Exception("Access mode to fopen must be a string");
      mode = arg[1].asString();
    }
    bool swapendian = false;
    if (arg.size() > 2) {
      QString swapflag = arg[2].asString();
      if (swapflag=="swap") {
	swapendian = true;
      } else if ((swapflag=="le") ||
		 (swapflag=="ieee-le") ||
		 (swapflag=="little-endian") ||
		 (swapflag=="littleEndian") ||
		 (swapflag=="little") ||
		 (swapflag == "l") ||
		 (swapflag == "ieee-le.l64") ||
		 (swapflag == "s")) {
	swapendian = bigEndian;
      } else if ((swapflag=="be") ||
		 (swapflag=="ieee-be") ||
		 (swapflag=="big-endian") ||
		 (swapflag=="bigEndian") ||
		 (swapflag=="big") ||
		 (swapflag=="b") ||
		 (swapflag=="ieee-be.l64") ||
		 (swapflag=="a")) {
	swapendian = !bigEndian;
      } else if (!arg[2].isEmpty())
	throw Exception("swap flag must be 'swap' or an endian spec (see help fopen for the complete list)");
    }
    QFlags<QIODevice::OpenModeFlag> modeFlag;
    mode = mode.toLower();
    // r  means ReadOnly && MustExist
    // r+ means ReadOnly | WriteOnly && MustExist
    // w  means Writeonly | Truncate
    // w+ means ReadOnly | WriteOnly | Truncate
    // a  means Append | WriteOnly
    // a+ means Append | WriteOnly | ReadOnly
    // t  means * | Text 
    bool mustExist = false;
    bool has_r = mode.contains("r");
    bool has_p = mode.contains("+");
    bool has_w = mode.contains("w");
    bool has_a = mode.contains("a");
    bool has_t = mode.contains("t");
    if (has_r) {
      modeFlag |= QIODevice::ReadOnly;
      mustExist = true;
    }
    if (has_w) modeFlag |= QIODevice::WriteOnly;
    if (has_p) modeFlag |= QIODevice::ReadOnly;
    if (has_p && has_r)  modeFlag |= QIODevice::WriteOnly;
    if (has_w) modeFlag |= QIODevice::Truncate;
    if (has_a) modeFlag |= QIODevice::Append | QIODevice::WriteOnly;
    if (has_t) modeFlag |= QIODevice::Text;
    
    FilePtr *fptr = new FilePtr();
    fptr->fp = new QFile(fname);
    if (mustExist && !fptr->fp->exists())
      return Array(double(-1));
    if (!fptr->fp->open(modeFlag))
      throw Exception(fptr->fp->errorString() + QString(" for fopen argument ") + fname);
    fptr->swapflag = swapendian;
    unsigned int rethan = fileHandles.assignHandle(fptr);
    return ArrayVector(Array(double(rethan-1)));
  } catch (Exception& e) {
    if (nargout > 1) {
      ArrayVector ret;
      ret.push_back(Array(double(-1)));
      ret.push_back(Array(e.msg()));
      return ret;
    }
    throw;
  }
}

//@@Signature
//function fclose FcloseFunction
//inputs handle
//outputs none
//DOCBLOCK io_fclose
ArrayVector FcloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Fclose must have one argument, either 'all' or a file handle");
  bool closingAll = false;
  if (arg[0].isString()) {
    QString allflag = arg[0].asString().toLower();
    if (allflag == "all") {
      closingAll = true;
      int maxHandle(fileHandles.maxHandle());
      for (int i=3;i<maxHandle;i++) {
	try {
	  FilePtr* fptr = fileHandles.lookupHandle(i+1);
	  delete fptr->fp;
	  fileHandles.deleteHandle(i+1);
	  delete fptr;
	} catch (Exception & e) {
	}
      }
    } else
      throw Exception("Fclose must have one argument, either 'all' or a file handle");
  } else {
    Array tmp(arg[0]);
    int handle = tmp.asInteger();
    if (handle <= 2)
      throw Exception("Cannot close handles 0-2, the standard in/out/error file handles");
    FilePtr* fptr = (fileHandles.lookupHandle(handle+1));
    delete fptr->fp;
    fileHandles.deleteHandle(handle+1);
    delete fptr;
  }
  return ArrayVector();
}

//@@Signature
//function fread FreadFunction
//inputs handle size precision
//outputs A count
//DOCBLOCK io_fread

template <typename T>
Array Tread(QFile* fp, NTuple dim, bool swapflag) {
  size_t bufsize = sizeof(T)*size_t(dim.count());
  if ((fp->size() - fp->pos()) < bufsize) {
    bufsize = fp->size() - fp->pos();
    dim = NTuple(ceil((double)bufsize/sizeof(T)),1);
  }
  if (!swapflag) {
    BasicArray<T> rp(dim);
    fp->read((char*)(rp.data()),bufsize);
    return Array(rp);
  } else {
    MemBlock<T> pbuf(size_t(bufsize/sizeof(T)));
    T* pb = &pbuf;
    fp->read((char*)pb,bufsize);
    SwapBuffer((char*)pb,bufsize/sizeof(T),sizeof(T));
    BasicArray<T> rp(dim);
    memcpy(rp.data(),pb,bufsize);
    return Array(rp);
  }
}

#define MacroRead(ctype,cls)			\
  case cls: { A = Tread<ctype>(fptr->fp,dims,fptr->swapflag).toClass(dcOut); break; }

ArrayVector FreadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("fread requires three arguments, the file handle, size, and precision");
  Array tmp(arg[0]);
  int handle = tmp.asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (!arg[2].isString())
    throw Exception("second argument to fread must be a precision");
  QString prec = arg[2].asString().toLower();
  // Get the size argument
  Array sze(arg[1].asDenseArray().toClass(Index));
  // Check for a single infinity
  BasicArray<index_t> dp(sze.real<index_t>());
  bool infinityFound = false;
  index_t elementCount = 1;
  index_t infiniteDim = 0;
  for (index_t i=1;i<=dp.length();i++) {
    if (IsNaN(dp[i])) throw Exception("nan not allowed in size argument");
    if (IsInfinite(dp[i])) {
      if (infinityFound) throw Exception("only a single inf is allowed in size argument");
      infinityFound = true;
      infiniteDim = i;
    } else {
      if (dp[i] < 0) throw Exception("illegal negative size argument");
      elementCount *= dp[i];
    }
  }
  // Map the precision string to a data class
  DataClass dcIn, dcOut;
  ComputePrecisionString(prec,dcIn,dcOut);
  // If there is an infinity in the dimensions, we have to calculate the
  // appropriate value
  if (infinityFound) {
    int64 bytes_left = fptr->fp->size() - fptr->fp->pos();
    dp[infiniteDim] = ceil(double(bytes_left)/ByteSizeOfDataClass(dcIn)/elementCount);
    elementCount *= dp[infiniteDim];
  }
  NTuple dims(1,1);
  for (index_t j=1;j<=qMin(NDims,int(dp.length()));j++) 
    dims[int(j-1)] = dp[j];
  Array A;
  switch (dcIn) {
  default: throw Exception("data type not supported for fread");
    MacroExpandCasesSimple(MacroRead);
  }
  ArrayVector retval;
  retval.push_back(A);
  if (nargout == 2)
    retval.push_back(Array(double(A.length())));
  return retval;
}

//@@Signature
//function fwrite FwriteFunction
//inputs handle A type
//outputs count
//DOCBLOCK io_fwrite

template <typename T>
int64 Twrite(QFile* fp, const Array &A, bool swapflag) {
  size_t bufsize = sizeof(T)*size_t(A.length());
  if (!swapflag)
    return fp->write((const char*)(A.constReal<T>().constData()),bufsize);
  else {
    MemBlock<T> pbuf(size_t(A.length()));
    T* pb = &pbuf;
    memcpy(pb,A.constReal<T>().constData(),bufsize);
    SwapBuffer((char*)pb,size_t(A.length()),sizeof(T));
    return fp->write((const char*)pb,bufsize);
  }
}

#define MacroWrite(ctype,cls)						\
  case cls: return ArrayVector(Array(double(Twrite<ctype>(fptr->fp,x,fptr->swapflag))));

ArrayVector FwriteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("fwrite requires at least two arguments, the file handle, and the variable to be written");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  if (arg[1].isReferenceType())
    throw Exception("cannot write reference data types with fwrite");
  Array x(arg[1]);
  if (arg.size() >= 3) {
    if (!arg[2].isString())
      throw Exception("type argument must be a string");
    QString prec = arg[2].asString();
    x = x.toClass(processPrecisionString(prec));
  }
  if (x.isString()) x = x.toClass(UInt8);
  x.ensureNotScalarEncoded();
  switch (x.dataClass()) {
  default: throw Exception("data type not supported for fwrite");
    MacroExpandCasesSimple(MacroWrite);
  }
}

#undef MacroWrite

//@@Signature
//function fflush FflushFunction
//inputs handle
//outputs none
//DOCBLOCK io_fflush
 ArrayVector FflushFunction(int nargout, const ArrayVector& arg) {
   if (arg.size() != 1)
     throw Exception("fflush requires an argument, the file handle.");
   int handle = arg[0].asInteger();
   FilePtr *fptr = (fileHandles.lookupHandle(handle+1));
   fptr->fp->flush();
   return ArrayVector();
 }

//@@Signature
//function ftell FtellFunction
//inputs handle
//outputs n
//DOCBLOCK io_ftell
ArrayVector FtellFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ftell requires an argument, the file handle.");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  return ArrayVector(Array(double(fptr->fp->pos())));
}

//@@Signature
//function feof FeofFunction
//inputs handle
//outputs flag
//DOCBLOCK io_feof
ArrayVector FeofFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("feof requires an argument, the file handle.");
  Array tmp(arg[0]);
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  return ArrayVector(Array(bool(fptr->fp->atEnd())));
}
  
//@@Signature
//function fseek FseekFunction
//inputs handle offset style
//outputs none
//DOCBLOCK io_fseek
ArrayVector FseekFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("fseek requires three arguments, the file handle, the offset, and style");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  int64 offset = int64(arg[1].asDouble());
  int style;
  if (arg[2].isString()) {
    QString styleflag = arg[2].asString().toLower();
    if (styleflag=="bof")
      style = -1;
    else if (styleflag=="cof")
      style = 0;
    else if (styleflag=="eof")
      style = 1;
    else
      throw Exception("unrecognized style format for fseek");
  } else {
    style = arg[2].asInteger();
    if ((style != -1) && (style != 0) && (style != 1))
      throw Exception("unrecognized style format for fseek");	
  }
  switch (style) {
  case -1:
    if (!fptr->fp->seek(offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  case 0:
    if (!fptr->fp->seek(fptr->fp->pos() + offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  case 1:
    if (!fptr->fp->seek(fptr->fp->size() - 1 - offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  }
  return ArrayVector();
}

//@@Signature
//function fgetline FgetlineFunction
//inputs handle
//outputs string
//DOCBLOCK io_fgetline
ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("fgetline takes one argument, the file handle");
  int handle = arg[0].asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (fptr->fp->atEnd())
    return ArrayVector(EmptyConstructor());
  else 
    return ArrayVector(Array(ReadQStringFromFile(fptr->fp)));
}

ArrayVector ScanfHelperFunction( QFile *fp, const ArrayVector& arg );
//@@Signature
//function fscanf FscanfFunction
//inputs varargin
//outputs varargout
//DOCBLOCK io_fscanf
ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("fscanf requires at least two arguments, the file handle and the format string");
  int handle = arg[0].asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));

  ArrayVector helper_arg; 
  helper_arg << arg[1];
  if( arg.size() == 3 )
      helper_arg << arg[2];

  return ScanfHelperFunction(fptr->fp, helper_arg);
}
