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

#include "Array.hpp"
#include "mex.h"
#include "MexInterface.hpp"

static NTuple GetDimensions(const mxArray *dp) {
  NTuple dim;
  for (int i=0;i<dp->number_of_dims;i++)
    dim[i] = dp->dims[i];
  return dim;
}

static int* GetDimensions(const Array& dp, int &numdims) {
  numdims = dp.dimensions().lastNotOne();
  int *dim_vec = (int*) malloc(sizeof(int)*numdims);
  for (int i=0;i<numdims;i++)
    dim_vec[i] = int(dp.dimensions()[i]);
  return dim_vec;
}

template <typename T>
static BasicArray<T> BasicArrayFromMexArray(const mxArray *dp, const void *qp) {
  BasicArray<T> rp(GetDimensions(dp));
  memcpy(rp.data(),qp,size_t(sizeof(T)*rp.length()));
  return rp;
}

template <typename T>
static Array TNumericArrayFromMexArray(const mxArray *dp) {
  if (dp->issparse) 
    throw Exception("mex interface does not support sparse matrices");
  if (dp->iscomplex)
    return Array(BasicArrayFromMexArray<T>(dp,dp->realdata),
		 BasicArrayFromMexArray<T>(dp,dp->imagdata));
  else
    return Array(BasicArrayFromMexArray<T>(dp,dp->realdata));
}

static Array NumericArrayFromMexArray(const mxArray *dp) {
  switch (dp->classID) {
  default:
    throw Exception("Unhandled type for mex array conversion");
  case mxUINT32_CLASS: return TNumericArrayFromMexArray<uint32>(dp);
  case mxINT32_CLASS:  return TNumericArrayFromMexArray<int32>(dp);
  case mxUINT16_CLASS: return TNumericArrayFromMexArray<uint16>(dp);
  case mxINT16_CLASS:  return TNumericArrayFromMexArray<int16>(dp);
  case mxUINT8_CLASS:  return TNumericArrayFromMexArray<uint8>(dp);
  case mxINT8_CLASS:   return TNumericArrayFromMexArray<int8>(dp);
  case mxSINGLE_CLASS: return TNumericArrayFromMexArray<float>(dp);
  case mxDOUBLE_CLASS: return TNumericArrayFromMexArray<double>(dp);
  case mxCHAR_CLASS:   return TNumericArrayFromMexArray<int16>(dp);
  }
}

Array ArrayFromMexArray(mxArray *array_ptr) {
  if (array_ptr->classID == mxCELL_CLASS) {
    NTuple dim = GetDimensions(array_ptr);
    mxArray** dp = (mxArray**) array_ptr->realdata;
    BasicArray<Array> rp(dim);
    for (index_t i=1;i<=rp.length();i++)
      rp[i] = ArrayFromMexArray(dp[int(i-1)]);
    return Array(rp);
  } else 
    return NumericArrayFromMexArray(array_ptr);
}

static mxArray* MexArrayFromCellArray(Array array) {
  // Convert array dimensions into a simple integer array
  int num_dim = array.dimensions().lastNotOne();
  int *dim_vec = (int*) malloc(sizeof(int)*num_dim);
  for (int i=0;i<num_dim;i++)
    dim_vec[i] = int(array.dimensions()[i]);
  mxArray *ret = mxCreateCellArray(num_dim,dim_vec);
  const BasicArray<Array> &rp(array.constReal<Array>());
  mxArray **dp = (mxArray **) ret->realdata;
  for (index_t i=1;i<=array.length();i++)
    dp[int(i-1)] = MexArrayFromArray(rp[i]);
  free(dim_vec);
  return ret;
}

template <typename T>
static mxArray* TMexArrayFromNumericArray(mxClassID classID, Array array) {
  if (array.isSparse()) throw Exception("sparse case not handled");
  mxComplexity flag;
  if (array.allReal())
    flag = mxREAL;
  else
    flag = mxCOMPLEX;
  int numdims;
  int* dims = GetDimensions(array,numdims);
  mxArray* ret = mxCreateNumericArray(numdims,dims,classID,flag);
  memcpy(ret->realdata,array.constReal<T>().constData(),size_t(array.length()*sizeof(T)));
  memcpy(ret->imagdata,array.constImag<T>().constData(),size_t(array.length()*sizeof(T)));
  return ret;
}

mxArray* MexArrayFromNumericArray(Array array) {
  switch (array.dataClass()) {
  default: throw Exception("unhandled case for numeric array");
  case UInt32: return TMexArrayFromNumericArray<uint32>(mxUINT32_CLASS,array);
  case Int32: return TMexArrayFromNumericArray<int32>(mxINT32_CLASS,array);
  case UInt16: return TMexArrayFromNumericArray<uint16>(mxUINT16_CLASS,array);
  case Int16: return TMexArrayFromNumericArray<int16>(mxINT16_CLASS,array);
  case UInt8: return TMexArrayFromNumericArray<uint8>(mxUINT8_CLASS,array);
  case Int8: return TMexArrayFromNumericArray<int8>(mxINT8_CLASS,array);
  case Float: return TMexArrayFromNumericArray<float>(mxSINGLE_CLASS,array);
  case Double: return TMexArrayFromNumericArray<double>(mxDOUBLE_CLASS,array);
  case StringArray: return TMexArrayFromNumericArray<uint16>(mxCHAR_CLASS,array);
  }
}

mxArray* MexArrayFromArray(Array array) {
  if (array.dataClass() == CellArray)
    return MexArrayFromCellArray(array);
  else
    return MexArrayFromNumericArray(array);
}

