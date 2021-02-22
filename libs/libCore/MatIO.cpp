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

#include "MatIO.hpp"
#include "Exception.hpp"
#include "Print.hpp"
#include "MemPtr.hpp"
#include "Array.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"
#include <time.h>
#include "SparseCCS.hpp"
#include "Struct.hpp"
#include <QtCore>
#include <zlib.h>
#include "Algorithms.hpp"

// Things to still look at:
//  Logical/Global flags - done
//  Compressed mode write - done
//  Sparse matrices - done
//  Matrix size calculation  - done
//  Load/Save interfacing - done


const int CHUNK = 32768;

extern void SwapBuffer(char* cp, int count, int elsize);

// A class to read/write Matlab MAT files.  Implemented based on the Matlab 7.1
// version of the file matfile_format.pdf from the Mathworks web site.

uint32 ElementSize(DataClass cls) {
  switch (cls) {
  default: throw Exception("illegal type as argument to elementsize");
  case Bool:
    return sizeof(bool);
  case UInt8:
    return sizeof(uint8);
  case Int8:
    return sizeof(int8);
  case UInt16:
    return sizeof(uint16);
  case Int16:
    return sizeof(int16);
  case UInt32:
    return sizeof(uint32);
  case Int32:
    return sizeof(int32);
  case UInt64:
    return sizeof(uint64);
  case Int64:
    return sizeof(int64);
  case Float:
    return sizeof(float);
  case Double:
    return sizeof(double);
  case StringArray:
    return sizeof(short);
  }
}


MatTypes ToMatType(DataClass x) {
  switch (x) {
  default: throw Exception("unhandled type as argument to tomattype");
  case Int8:
    return miINT8;
  case UInt8:
    return miUINT8;
  case Int16:
    return miINT16;
  case UInt16:
    return miUINT16;
  case Int32:
    return miINT32;
  case UInt32:
    return miUINT32;
  case Float:
    return miSINGLE;
  case Double:
    return miDOUBLE;
  case Int64:
    return miINT64;
  case UInt64:
    return miUINT64;
  case StringArray:
    return miUTF16;
  }
}

DataClass GetArrayType(MatIO::mxArrayTypes x) {
  switch (x) {
  default: throw Exception("unhandled type as argument to getarraytype");
  case mxINT8_CLASS: return Int8;
  case mxUINT8_CLASS: return UInt8;
  case mxINT16_CLASS: return Int16;
  case mxUINT16_CLASS: return UInt16;
  case mxINT32_CLASS: return Int32;
  case mxUINT32_CLASS: return UInt32;
  case mxINT64_CLASS: return Int64;
  case mxUINT64_CLASS: return UInt64;
  case mxSINGLE_CLASS: return Float;
  case mxDOUBLE_CLASS: return Double;
  case mxCHAR_CLASS: return StringArray;
  }
}

MatIO::mxArrayTypes GetArrayType(DataClass x) {
  switch(x) {
  case CellArray:   return MatIO::mxCELL_CLASS;
  case Struct:      return MatIO::mxSTRUCT_CLASS;
  case Bool:        return MatIO::mxINT32_CLASS;
  case UInt8:       return MatIO::mxUINT8_CLASS;
  case Int8:        return MatIO::mxINT8_CLASS;
  case UInt16:      return MatIO::mxUINT16_CLASS;
  case Int16:       return MatIO::mxINT16_CLASS;
  case UInt32:      return MatIO::mxUINT32_CLASS;
  case Int32:       return MatIO::mxINT32_CLASS;
  case UInt64:      return MatIO::mxUINT64_CLASS;
  case Int64:       return MatIO::mxINT64_CLASS;
  case Float:       return MatIO::mxSINGLE_CLASS;
  case Double:      return MatIO::mxDOUBLE_CLASS;
  case StringArray: return MatIO::mxCHAR_CLASS;
  default:
    throw Exception("unhandled type in GetArrayType");
  }  
}

DataClass ToFreeMatClass(MatTypes x) {
  switch (x) {
  default: throw Exception("unhandled type as argument to tofreematclass");
  case miINT8: return Int8;
  case miUINT8: return UInt8;
  case miINT16: return Int16;
  case miUINT16: return UInt16;
  case miINT32: return Int32;
  case miUINT32: return UInt32;
  case miSINGLE: return Float;
  case miDOUBLE: return Double;
  case miINT64: return Int64;
  case miUINT64: return UInt64;
  case miUTF8: return UInt8;
  case miUTF16: return UInt16;
  case miUTF32: return UInt32;
  }
  throw Exception("Should not be here...");
}

bool isNormalClass(MatIO::mxArrayTypes type) {
  return (((type >= MatIO::mxDOUBLE_CLASS) && (type < MatIO::mxFUNCTION_CLASS)) 
	  || (type == MatIO::mxCHAR_CLASS));
}

const uint8 bcomplexFlag = 8;
const uint8 bglobalFlag = 4;
const uint8 blogicalFlag = 2;

uint32 UpperWord(uint32 x) {
  return x >> 16;
} 

uint32 LowerWord(uint32 x) {
  return x & 0xffff;
}

uint8 ByteOne(uint32 x) {
  return (x & 0xff);
}

uint8 ByteTwo(uint32 x) {
  return ((x & 0xff00) >> 8);
}

uint8 ByteThree(int32 x) {
  return ((x & 0xff0000) >> 16);
}

uint8 ByteFour(uint32 x) {
  return ((x & 0xff000000) >> 24);
}

void MatIO::putSparseArray(const Array &x) {
  Array rows, cols, vals;
  SparseToIJVMat(x,rows,cols,vals);
  putDataElement(rows.toClass(Int32));
  putDataElement(cols.toClass(Int32));
  putDataElement(Real(vals));
  if (!x.allReal())
    putDataElement(Imag(vals));
}

Array MatIO::getSparseArray(NTuple dm, bool complexFlag) {
  Array ir(getDataElement().toClass(Index));
  Array jc(getDataElement().toClass(Index));
  Array pr(getDataElement());
  Array pi;
  if (complexFlag) pi = getDataElement();
  return MatIJVToSparse(ir,jc,pr,pi,complexFlag);
}

Array MatIO::getNumericArray(mxArrayTypes arrayType, NTuple dm, bool complexFlag) {
  Array pr(getDataElement().toClass(GetArrayType(arrayType)));
  Array pi;
  if (complexFlag)
    pr = MergeToComplex(pr,getDataElement().toClass(GetArrayType(arrayType)));
  pr.reshape(dm);
  return pr;
}

Array MatIO::getClassArray(NTuple dm) {
  Array className(getDataElement());
  Array structBase(getStructArray(dm));
  StructArray& rp(structBase.structPtr());
  rp.setClassName(className.asString());
  return structBase;
}

Array MatIO::getStructArray(NTuple dm) {
  Array fieldNameLength(getDataElement());
  int fieldNameLen = fieldNameLength.asInteger();;
  Array fieldNames(getDataElement().asDenseArray().toClass(UInt16));
  index_t fieldNamesLen = fieldNames.length();
  index_t fieldNameCount = fieldNamesLen / fieldNameLen;
  const BasicArray<uint16> &dp(fieldNames.constReal<uint16>());
  StringVector names;
  for (int i=0;i<fieldNameCount;i++) {
    // Measure the length of this string
    int namelen = 0;
    for (int j=0;j<fieldNameLen;j++)
      if (dp[i*fieldNameLen+j+1])
	namelen++;
    QString buffer(namelen,QChar(0));
    for (int j=0;j<namelen;j++)
      buffer[j] = QChar(dp[i*fieldNameLen+j+1]);
    names.push_back(buffer);
  }
  QVector<BasicArray<Array> > data;
  data.resize(fieldNameCount);
  for (int j=0;j<fieldNameCount;j++) {
    BasicArray<Array> dp(dm);
    for (index_t i=1;i<=dm.count();i++) {
      bool atEof; QString name; bool match; bool global;
      dp[i] = getArray(atEof,name,match,global);
    }
    data[j] = dp;
  }
  StructArray ret_sp;
  ret_sp.setFieldNamesAndData(names,data);
  return Array(ret_sp);
}

Array MatIO::getCellArray(NTuple dm) {
  BasicArray<Array> rp(dm);
  for (index_t i=1;i<=dm.count();i++) {
    bool ateof; QString name; bool match; bool global;
    rp[i] = getArray(ateof,name,match,global);
  }
  return Array(rp);
}

Array MatIO::getDataElement() {
  uint32 tag1 = getUint32();
  uint32 DataType, ByteCount;
  // Is the upper word of tag1 zero?
  if (UpperWord(tag1) == 0) {
    // Yes, then tag1 is the datatype
    // and tag2 is the byte count
    DataType = tag1;
    ByteCount = getUint32();
  } else {
    // No, then the upper word of tag1 is the byte count
    // and the lower word of tag1 is the data type
    DataType = LowerWord(tag1);
    ByteCount = UpperWord(tag1);
  }
  DataClass fmClass(ToFreeMatClass((MatTypes)DataType));
  uint32 byteSize(ElementSize(fmClass));
  index_t len(ByteCount/byteSize);
  Array ret(fmClass,NTuple(len,1));
  ReadData(ret.getVoidPointer(),ByteCount);
  Align64Bit();
  if (m_endianSwap)
    SwapBuffer((char*)ret.getVoidPointer(),int(len),byteSize);
  return ret;
}

void MatIO::putDataElement(const Array &x) {
  DataClass fmClass(x.dataClass());
  MatTypes DataType(ToMatType(fmClass));
  uint32 ByteCount(uint32(x.length()*ElementSize(fmClass)));
  putUint32((uint32)DataType);
  putUint32(ByteCount);
  if (x.isString()) {
    const BasicArray<QChar> &xdat(x.constReal<QChar>());
    for (index_t i=1;i<=xdat.length();i++) {
      unsigned short code = xdat.get(i).unicode();
      WriteData(&code,2);
    }
  } else if (x.isScalar()) {
    Array xt(x.asDenseArray());
    WriteData(xt.getConstVoidPointer(),ByteCount);
  } else
    WriteData(x.getConstVoidPointer(),ByteCount);
  Align64Bit();
}

void MatIO::InitializeCompressor() {
  // Allocate an array to hold the compressed bytes
  m_compression_buffer = (uint8*) calloc(1,CHUNK);
  // Set up the zstream...
  zstream = (z_streamp) calloc(1,sizeof(z_stream));
  zstream->zalloc = NULL;
  zstream->zfree = NULL;
  zstream->opaque = NULL;
  zstream->next_in = NULL;
  zstream->next_out = m_compression_buffer;
  zstream->avail_in = 0;
  zstream->avail_out = CHUNK;
  int retval;
  retval = deflateInit(zstream,9);
  if (retval) throw Exception("defaultinit didn't work");
  m_compressed_data = true;
  
}

void MatIO::WriteCompressedBytes(const void *dest, uint32 towrite) {
  zstream->next_in = (Bytef*) dest;
  zstream->avail_in = towrite;
  do {
    int ret = deflate(zstream,Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR)
      throw Exception("Compression engine failed on write!");
    if (zstream->avail_out == 0) {
      WriteFileBytes(m_compression_buffer,CHUNK);
      zstream->avail_out = CHUNK;
      zstream->next_out = m_compression_buffer;
    }
  } while (zstream->avail_in);
}

void MatIO::CloseCompressor() {
  int ret;
  do {
    ret = deflate(zstream,Z_FINISH);
    if (zstream->avail_out == 0) {
      WriteFileBytes(m_compression_buffer,CHUNK);
      zstream->avail_out = CHUNK;
      zstream->next_out = m_compression_buffer;
    }
  } while (ret != Z_STREAM_END);
  WriteFileBytes(m_compression_buffer,CHUNK-zstream->avail_out);
  deflateEnd(zstream);
  free(zstream);
  m_compressed_data = false;
  free(m_compression_buffer);
}

void MatIO::InitializeDecompressor(uint32 bcount) {
  // Allocate an array to hold the compressed bytes
  m_compression_buffer = (uint8*) calloc(1,bcount);
  ReadFileBytes(m_compression_buffer,bcount);
  // Set up the zstream...
  zstream = (z_streamp) calloc(1,sizeof(z_stream));
  zstream->zalloc = NULL;
  zstream->zfree = NULL;
  zstream->opaque = NULL;
  zstream->next_in = m_compression_buffer;
  zstream->next_out = NULL;
  zstream->avail_in = bcount;
  zstream->avail_out = 0;
  int retval;
  retval = inflateInit(zstream);
  if (retval) throw Exception("inflateInit didn't work");
  m_compressed_data = true;
}

void MatIO::ReadCompressedBytes(void *dest, uint32 toread) {
  zstream->next_out = (Bytef*) dest;
  zstream->avail_out = toread;
  while (zstream->avail_out) {
    int ret = inflate(zstream,Z_SYNC_FLUSH);
    if (ret < 0)
      throw Exception(QString("inflate failed with code: ") + ret);
  }
}

void MatIO::CloseDecompressor() {
  inflateEnd(zstream);
  free(zstream);
  m_compressed_data = false;
  free(m_compression_buffer);
}

void MatIO::ReadFileBytes(void *dest, uint32 toread) {
  if (m_fp->read((char*)dest,toread) != toread) 
    throw Exception("Unable to read from MAT file");
}

void MatIO::WriteFileBytes(const void *dest, uint32 towrite) {
  if (m_fp->write((const char*)dest,towrite) != towrite) 
    throw Exception("unable to write to MAT file");
}

void MatIO::WriteData(const void *dest, uint32 towrite) {
  m_writecount += towrite;
  if (m_phantomWriteMode) return;
  if (!m_compressed_data)
    WriteFileBytes(dest,towrite);
  else
    WriteCompressedBytes(dest,towrite);
}

void MatIO::ReadData(void *dest, uint32 toread) {
  if (!m_compressed_data)
    ReadFileBytes(dest,toread);
  else
    ReadCompressedBytes(dest,toread);
}

uint16 MatIO::getUint16() {
  uint16 x;
  ReadData(&x,sizeof(uint16));
  if (m_endianSwap)
    SwapBuffer((char*)&x,1,sizeof(uint16));
  return x;
}

uint32 MatIO::getUint32() {
  uint32 x;
  ReadData(&x,sizeof(uint32));
  if (m_endianSwap)
    SwapBuffer((char*)&x,1,sizeof(uint32));
  return x;
}

void MatIO::putUint16(uint16 x) {
  WriteData(&x,sizeof(uint16));
}

void MatIO::putUint32(uint32 x) {
  WriteData(&x,sizeof(uint32));
}

void MatIO::Align64Bit() {
  if (m_mode == readMode) {
    uint32 adjustBytes;
    if (!m_compressed_data) {
      adjustBytes = ((8-(m_fp->pos()&0x7)) % 8);
      m_fp->seek(m_fp->pos()+adjustBytes);
    } else {
      uint8 dummy[8];
      adjustBytes = ((8-(zstream->total_out)) & 0x7);
      ReadCompressedBytes(dummy,adjustBytes);
    }
  } else {
    char buffer[8];
    uint32 adjustBytes;
    adjustBytes = ((8-(m_writecount&0x7)) % 8);
    WriteData(buffer,adjustBytes);
  }
}

void MatIO::putStructArray(const Array &x) {
  // Calculate the maximum field name length
  // FIXME - should we truncate to 32 byte fieldnames?
  StringVector fnames(x.constStructPtr().fieldNames()); 
  int fieldNameCount = fnames.size();
  int maxlen = 0;
  for (int i=0;i<fieldNameCount;i++)
    maxlen = max(maxlen,(int)fnames.at(i).size());
  maxlen++;
  // Write it as an int32 
  Array fieldNameLength = Array(int32(maxlen));
  putDataElement(fieldNameLength);
  Array fieldNameText(Transpose(StringArrayFromStringVector(fnames,QChar(0))));
  fieldNameText.resize(NTuple(fieldNameText.rows()+1,fieldNameText.cols()));
  putDataElement(fieldNameText.toClass(Int8));
  for (int i=0;i<fieldNameCount;i++) {
    const BasicArray<Array> &rp(x.constStructPtr()[i]);
    for (index_t j=1;j<=rp.length();j++)
      putArray(rp[j]);
  }
}

void MatIO::putClassArray(const Array &x) {
  QString className = x.constStructPtr().className();
  Array classNameArray(className);
  putDataElement(classNameArray.toClass(Int8));
  putStructArray(x);
}


// Convert from CRS-->IJV
// Replace the col_ptr array by a new one
// [1,4,8,10,13,17,20] -->
//   [1 1 1, 2 2 2 2, 3 3, 4, 4, 4, 5...]

void MatIO::putCellArray(const Array &x) {
  const BasicArray<Array> &rp(x.constReal<Array>());
  for (index_t i=1;i<=rp.length();i++)
    putArray(rp[i]);
}

void MatIO::putArrayCompressed(const Array &x, QString name) {
  int64 spos, fpos;
  // Set the write count to zero
  m_writecount = 0;
  m_phantomWriteMode = false;
  // Write out a compression flag
  putUint32(miCOMPRESSED);
  // Get the current file position
  spos = m_fp->pos();
  // Put out a dummy 0 place holder
  putUint32(0);
  InitializeCompressor();
  putArray(x,name);
  CloseCompressor();
  // Get our current position
  fpos = m_fp->pos();
  // Seek to the place holder and overwrite with a byte count
  m_fp->seek(spos);
  putUint32(fpos-spos-sizeof(int32));
  // Return to the end of the stream
  m_fp->seek(fpos);
}

//Write a matrix to the stream
void MatIO::putArray(const Array &x, QString name, bool isGlobal) {
  Array aFlags(UInt32,NTuple(1,2));
  BasicArray<uint32> &dp(aFlags.real<uint32>());
  bool isComplex = x.isComplex();
  bool isLogical = (x.dataClass() == Bool);
  mxArrayTypes arrayType = GetArrayType(x.dataClass());
  if (x.isSparse())  arrayType = mxSPARSE_CLASS;
  dp[1] = arrayType;
  if (isGlobal)   dp[1] = dp[1] | (bglobalFlag << 8);
  if (isLogical)  dp[1] = dp[1] | (blogicalFlag << 8);
  if (isComplex)  dp[1] = dp[1] | (bcomplexFlag << 8);
  putUint32(miMATRIX);
  uint32 bcount = m_writecount;
  if (!m_phantomWriteMode) {
    m_phantomWriteMode = true;
    putUint32(0);
    putArraySpecific(x,aFlags,name,arrayType);
    m_phantomWriteMode = false;
    putUint32(m_writecount-bcount-4);
    m_writecount = bcount+4;
    putArraySpecific(x,aFlags,name,arrayType);
  } else {
    putUint32(0);
    putArraySpecific(x,aFlags,name,arrayType);
  }
}


void MatIO::putArraySpecific(const Array &x, Array aFlags, 
			     QString name, mxArrayTypes arrayType) {
  putDataElement(aFlags);
  putDataElement(ConvertNTupleToArray(x.dimensions()).toClass(Int32));
  putDataElement(Array(name).toClass(Int8));
  if (x.dataClass() == Bool)
    putNumericArray(x.toClass(Int32));
  else if (isNormalClass(arrayType))
    putNumericArray(x);
  else if  (arrayType == mxCELL_CLASS) 
    putCellArray(x);
  else if (arrayType == mxSTRUCT_CLASS)
    putStructArray(x);
  else if (arrayType == mxOBJECT_CLASS)
    putClassArray(x);
  else if (arrayType == mxSPARSE_CLASS)
    putSparseArray(x);
  else throw Exception(QString("Unable to do this one :") + arrayType);
}

void MatIO::putNumericArray(const Array &x) {
  if (!x.isComplex())
    putDataElement(x);
  else {
    putDataElement(Real(x));
    putDataElement(Imag(x));
  }
}

Array MatIO::getArray(bool &atEof, QString &name, bool &match, bool &isGlobal) {
  if (!m_compressed_data && m_fp->atEnd()) {
    atEof = true;
    return Array();
  }
  uint32 tag1 = getUint32();
  atEof = false;
  uint32 DataType, ByteCount;
  // Is the upper word of tag1 zero?
  if (UpperWord(tag1) == 0) {
    // Yes, then tag1 is the datatype
    // and tag2 is the byte count
    DataType = tag1;
    ByteCount = getUint32();
  } else {
    // No, then the upper word of tag1 is the byte count
    // and the lower word of tag1 is the data type
    DataType = LowerWord(tag1);
    ByteCount = UpperWord(tag1);
  }
  // Is it a compression flag?
  if (DataType == miCOMPRESSED) {
    InitializeDecompressor(ByteCount);
    Array ret(getArray(atEof,name,match,isGlobal));
    CloseDecompressor();
    return ret;
  }
  if (DataType != miMATRIX) 
    throw Exception("Unexpected data tag when looking for an array");
  if (ByteCount == 0)
    return Array();
  Array aFlags(getDataElement());
  if ((aFlags.dataClass() != UInt32) || (aFlags.length() != 2))
    throw Exception("Corrupted MAT file - array flags");
  aFlags = aFlags.asDenseArray();
  const BasicArray<uint32> &dp(aFlags.constReal<uint32>());
  mxArrayTypes arrayType = (mxArrayTypes) (ByteOne(dp[1]));
  uint8 arrayFlags = ByteTwo(dp[1]);
  bool isComplex = (arrayFlags & bcomplexFlag) != 0;
  isGlobal = (arrayFlags & bglobalFlag) != 0;
  bool isLogical = (arrayFlags & blogicalFlag) != 0;
  Array dims(getDataElement());
  if (dims.dataClass() != Int32)
    throw Exception("Corrupted MAT file - dimensions array");
  NTuple dm(ConvertArrayToNTuple(dims));
  Array namedata(getDataElement());
  Array namearray(namedata.toClass(StringArray));
  QString tname = namearray.asString();
  match = true;
  name = tname;
  Array toret;
  if (isNormalClass(arrayType)) 
    toret = getNumericArray(arrayType,dm,isComplex);
  else if (arrayType == mxCELL_CLASS) 
    toret = getCellArray(dm);
  else if (arrayType == mxSTRUCT_CLASS)
    toret = getStructArray(dm);
  else if (arrayType == mxOBJECT_CLASS)
    toret = getClassArray(dm);
  else if (arrayType == mxSPARSE_CLASS)
    toret = getSparseArray(dm,isComplex);
  else if (arrayType == mxFUNCTION_CLASS)
    toret = getClassArray(dm);
  else
    throw Exception(QString("Unable to do this one :") + arrayType);
  if (isLogical)
    toret = toret.toClass(Bool);
  return toret;
}

MatIO::MatIO(QString filename, openMode mode) :
  m_filename(filename), m_mode(mode), m_compressed_data(false) {
  m_fp = new QFile(filename);
  if (m_mode == writeMode) {
    if (!m_fp->open(QIODevice::WriteOnly))
      throw Exception("Unable to open file " + filename + " for writing");
  } else {
    if (!m_fp->open(QIODevice::ReadOnly))
      throw Exception("Unable to open file " + filename + " for reading");
  }
  m_endianSwap = false;
  m_phantomWriteMode = false;
  m_writecount = 0;
}

QString MatIO::getHeader() {
  // Read the header...
  char hdrtxt[124];
  if (m_fp->read(hdrtxt,124) != 124) throw Exception("Unable to read header - MAT file is corrupted or truncated");
  uint16 val = getUint16();
  if ((val != 0x100) && (val != 0x1)) throw Exception("Not a valid MAT file");
  uint16 byteOrder = getUint16();
  if (byteOrder == ('M' << 8 | 'I'))
    m_endianSwap = false;
  else if (byteOrder == ('I' << 8 | 'M'))
    m_endianSwap = true;
  else 
    throw Exception("Unable to determine the byte order of the MAT file");
  return QString(hdrtxt);
}

void MatIO::putHeader(QString hdr) {
  const char *hd = qPrintable(hdr);
  char hdrtxt[124];
  memset(hdrtxt,0,124);
  memcpy(hdrtxt,hd,qMin(123,hdr.size()));
  if (m_fp->write(hdrtxt,124) != 124) throw Exception("Unable to write header - MAT file is corrupted");
  putUint16(0x100);
  putUint16('M' << 8 | 'I');
  m_writecount = 0;
}

MatIO::~MatIO() {
  delete m_fp;
}

ArrayVector MatLoadFunction(int nargout, QString filename, 
			    StringVector varnames, bool regexpmode, Interpreter *eval) {
  StringVector fieldnames;
  ArrayVector fieldvalues;
  MatIO m(filename,MatIO::readMode);
  m.getHeader();
  bool ateof = false;
  ParentScopeLocker lock(eval->getContext());
  while(!ateof) {
    QString name;
    bool globalFlag = false;
    bool match = false;
    Array a(m.getArray(ateof,name,match,globalFlag));
    if (!ateof) {
      if ((varnames.size() == 0) || (contains(varnames,name,regexpmode))) {
	if (nargout == 0) {
	  if (globalFlag)
	    eval->getContext()->addGlobalVariable(name);
	  eval->getContext()->insertVariable(name,a);
	} else {
	  fieldnames << name;
	  fieldvalues << a;
	}
      }
    }
  }
  if (nargout == 0)
    return ArrayVector();
  else
    return ArrayVector(StructConstructor(fieldnames,fieldvalues));
}

ArrayVector MatSaveFunction(QString filename, StringVector names, Interpreter *eval) {
  MatIO m(filename,MatIO::writeMode);
  Context *cntxt = eval->getContext();
  char header[116];
  time_t t = time(NULL);
  snprintf(header, 116, "MATLAB 5.0 MAT-file, Created on: %s by %s",
	   ctime(&t), qPrintable(Interpreter::getVersionString()));
  m.putHeader(header);
  for (int i=0;i<names.size();i++) {
    ArrayReference toWrite = cntxt->lookupVariable(names[i]);
    if (toWrite.valid())
      m.putArray(*toWrite,names[i],cntxt->isVariableGlobal(names[i]));
    else
      eval->warningMessage(QString("variable ") + names[i] + " does not exist to save");
  }
  return ArrayVector();
}

