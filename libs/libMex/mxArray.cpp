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
#include "mex.h"
#include <string.h>
#include <stdlib.h>
#include <set>
#include <string>

static std::set<void*> memlist;

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

static bool endianDetected = false;
static bool bigEndian = false;


static void CheckBigEndian() {
  union {
    long l;
    char c[sizeof (long)];
  } u;

  u.l = 1;
  endianDetected = true;
  bigEndian = (u.c[sizeof(long) - 1] == 1);  
}

void RegisterPointer(void *ptr) {
  memlist.insert(ptr);
}

void DeregisterPointer(void *ptr) {
  memlist.erase(ptr);
}

int CountElements(int ndim, const int *dims) {
  int count = 1;
  for (int i=0;i<ndim;i++)
    count *= dims[i];
  return count;
}

int *CopyDims(int ndim, const int *dims) {
  int *p = (int*) malloc(sizeof(int)*ndim);
  for (int i=0;i<ndim;i++)
    p[i] = dims[i];
  return p;
}

size_t SizeFromClass(mxClassID clss) {
  switch(clss) {
  case mxCELL_CLASS:
    return sizeof(mxArray*);
  case mxSTRUCT_CLASS:
    return sizeof(mxArray*);
  case mxLOGICAL_CLASS:
    return sizeof(mxLogical);
  case mxCHAR_CLASS:
    return sizeof(mxChar);
  case mxDOUBLE_CLASS:
    return sizeof(double);
  case mxSINGLE_CLASS:
    return sizeof(float);
  case mxINT8_CLASS:
    return sizeof(int8);
  case mxUINT8_CLASS:
    return sizeof(uint8);
  case mxINT16_CLASS:
    return sizeof(int16);
  case mxUINT16_CLASS:
    return sizeof(uint16);
  case mxINT32_CLASS:
    return sizeof(int32);
  case mxUINT32_CLASS:
    return sizeof(uint32);
  case mxINT64_CLASS:
    return sizeof(int64);
  case mxUINT64_CLASS:
    return sizeof(uint64);
  default:
    return 0;
  }    
}

mxArray* mxNewArray() {
  mxArray* p = (mxArray*) malloc(sizeof(mxArray));
  return p;
}

mxArray *mxAllocateRealArray(int ndim, const int *dims, size_t size, mxClassID classID ) {
  mxArray *ret = mxNewArray(); 
  ret->classID = classID;
  ret->number_of_dims = ndim;
  ret->dims = CopyDims(ndim,dims);
  ret->issparse = false;
  ret->n_fields = 0;
  ret->iscomplex = 0;
  ret->realdata = mxCalloc(CountElements(ndim,dims),size);
  ret->imagdata = NULL;
  return ret;
}

mxArray *mxAllocateComplexArray(int ndim, const int *dims, size_t size, mxClassID classID ) {
  mxArray *ret = mxNewArray(); 
  ret->classID = classID;
  ret->number_of_dims = ndim;
  ret->dims = CopyDims(ndim,dims);
  ret->issparse = false;
  ret->n_fields = 0;
  ret->iscomplex = 0;
  ret->realdata = mxCalloc(CountElements(ndim,dims),size);
  ret->imagdata = mxCalloc(CountElements(ndim,dims),size);
  return ret;
}

//int mxAddField(mxArray *array_ptr, const char *field_name);

char* mxArrayToString(const mxArray *array_ptr) {
  if (array_ptr->classID != mxCHAR_CLASS)
    return NULL;
  mxCHAR* p = (mxCHAR*) array_ptr->realdata;
  int N = mxGetNumberOfElements(array_ptr);
  char *t = (char*) malloc(N+1);
  for (int i=0;i<N;i++)
    t[i] = p[i];
  t[N] = 0;
  return t;
}

void mxAssert(int expr, char *error_message);

void mxAssertS(int expr, char *error_message);

int mxCalcSingleSubscript(const mxArray *array_ptr, int nsubs, int *subs);

void *mxCalloc(size_t n, size_t size) {
  void *p = calloc(n,size);
  RegisterPointer(p);
  return p;
}

mxArray *mxCreateCellArray(int ndim, const int *dims) {
  return mxAllocateRealArray(ndim,dims,sizeof(void*),mxCELL_CLASS);
}

mxArray *mxCreateCellMatrix(int m, int n) {
  int dims[2];
  dims[0] = m; dims[1] = n;
  return mxCreateCellArray(2,dims);
}

mxArray *mxCreateCharArray(int ndim, const int *dims) {
  return mxAllocateRealArray(ndim,dims,sizeof(mxCHAR),mxCHAR_CLASS);
}

#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

mxArray *mxCreateCharMatrixFromStrings(int m, const char **str) {
  int dims[2];
  int maxlen = 0;
  int i, j;
  mxArray *a;
  mxChar *ptr;

  for (i=0;i<m;i++) {
    maxlen = MAX(maxlen,(int)(strlen(str[i])));
  }
  dims[0] = m;
  dims[1] = maxlen;
  a = mxCreateCharArray(2,dims);
  ptr = (mxChar*) a->realdata;
  for (i=0;i<m;i++)
    for (j=0;j<((int)strlen(str[i]));j++)
      ptr[i+j*m] = str[i][j];
  return a;
}

mxArray *mxCreateDoubleMatrix(int m, int n, mxComplexity ComplexFlag) {
  int dims[2];
  dims[0] = m;
  dims[1] = n;
  if (ComplexFlag == mxREAL)
    return mxAllocateRealArray(2,dims,sizeof(double),mxDOUBLE_CLASS);
  else
    return mxAllocateComplexArray(2,dims,sizeof(double),mxDOUBLE_CLASS);
}

mxArray *mxCreateDoubleScalar(double value) {
  mxArray *ret = mxCreateDoubleMatrix(1,1,mxREAL);
  ((double*) ret->realdata)[0] = value;
  return ret;
}

mxArray *mxCreateLogicalArray(int ndim, const int *dims) {
  return mxAllocateRealArray(ndim,dims,sizeof(mxLogical),mxLOGICAL_CLASS);
}

mxArray *mxCreateLogicalMatrix(int m, int n) {
  int dims[2];
  dims[0] = m;
  dims[1] = n;
  return mxCreateLogicalArray(2,dims);
}

mxArray *mxCreateLogicalScalar(mxLogical value) {
  mxArray *ret = mxCreateLogicalMatrix(1,1);
  ((mxLogical*) ret->realdata)[0] = value;
  return ret;
}

mxArray *mxCreateNumericArray(int ndim, const int *dims, mxClassID clss, mxComplexity ComplexFlag) {
  if (ComplexFlag == mxREAL)
    return mxAllocateRealArray(ndim,dims,SizeFromClass(clss),clss);
  else
    return mxAllocateComplexArray(ndim,dims,SizeFromClass(clss),clss);
}

mxArray *mxCreateNumericMatrix(int m, int n, mxClassID clss, mxComplexity ComplexFlag) {
  int dims[2];
  dims[0] = m; dims[1] = n;
  return mxCreateNumericArray(2,dims,clss,ComplexFlag);
}

mxArray *mxCreateScalarDouble(double value) {
  return mxCreateDoubleScalar(value);
}

mxArray *mxCreateSparse(int m, int n, int nzmax, mxComplexity ComplexFlag);
mxArray *mxCreateSparseLogicalMatrix(int m, int n, int nzmax);

mxArray *mxCreateString(const char *str) {
  return mxCreateCharMatrixFromStrings(1,&str);
}

mxArray *mxCreateStructArray(int ndim, const int *dims, int nfields, const char **field_names);
mxArray *mxCreateStructMatrix(int m, int n, int nfields, const char **field_names);

void mxDestroyArray(mxArray *array_ptr) {
  if (array_ptr->classID == mxCELL_CLASS) {
    mxArray **gp = (mxArray**) array_ptr->realdata;
    int L = mxGetNumberOfElements(array_ptr);
    for (int i=0;i<L;i++)
      mxDestroyArray(gp[L]);
  }
  mxFree(array_ptr->realdata);
  mxFree(array_ptr->imagdata);
  mxFree(array_ptr);
}

mxArray *mxDuplicateArray(const mxArray *in) {
  if (in == NULL) return NULL;
  int L = mxGetNumberOfElements(in);
  mxArray *ret;
  if (in->classID != mxCELL_CLASS) {
    if (in->iscomplex) {
      ret = mxAllocateRealArray(in->number_of_dims,
				in->dims,SizeFromClass(in->classID),
				in->classID);
      memcpy(ret->realdata,in->realdata,mxGetElementSize(in)*L);
    } else {
      ret = mxAllocateComplexArray(in->number_of_dims,
				   in->dims,SizeFromClass(in->classID),
				   in->classID);
      memcpy(ret->realdata,in->realdata,mxGetElementSize(in)*L);
      memcpy(ret->imagdata,in->imagdata,mxGetElementSize(in)*L);
    }
    return ret;
  } else {
    ret = mxAllocateRealArray(in->number_of_dims,
			      in->dims,SizeFromClass(in->classID),
			      in->classID);
    mxArray **g = (mxArray**) ret->realdata;
    mxArray **h = (mxArray**) in->realdata;
    for (int i=0;i<L;i++)
      g[i] = mxDuplicateArray(h[i]);
  }
  return ret;
}

void mxFree(void *ptr) {
  DeregisterPointer(ptr);
  free(ptr);
}

mxArray *mxGetCell(const mxArray *array_ptr, int index) {
  return (((mxArray**)array_ptr->realdata)[index]);
}

mxChar *mxGetChars(const mxArray *array_ptr) {
  return (mxChar*)array_ptr->realdata;
}

mxClassID mxGetClassID(const mxArray *array_ptr) {
  return array_ptr->classID;
}

void *mxGetData(const mxArray *array_ptr) {
  return array_ptr->realdata;
}

const int *mxGetDimensions(const mxArray *array_ptr) {
  return array_ptr->dims;
}

int mxGetElementSize(const mxArray *array_ptr) {
  return SizeFromClass(array_ptr->classID);
}

double mxGetEps(void);

mxArray *mxGetField(const mxArray *array_ptr, int index, const char *field_name) {
  return mxGetFieldByNumber(array_ptr,index,mxGetFieldNumber(array_ptr,field_name));
}

mxArray *mxGetFieldByNumber(const mxArray *array_ptr, int index, int field_number) {
  return ((mxArray***)array_ptr->realdata)[field_number][index];
}

const char *mxGetFieldNameByNumber(const mxArray *array_ptr, int field_number) {
  return array_ptr->fieldnames[field_number];
}

int mxGetFieldNumber(const mxArray *array_ptr, const char *field_name) {
  int i;
  for (i=0;i<array_ptr->n_fields;i++) 
    if (strcmp(array_ptr->fieldnames[i],field_name)==0)
      return i;
  return -1;
}

void *mxGetImagData(const mxArray *array_ptr) {
  return array_ptr->imagdata;
}

double mxGetInf(void);

int *mxGetIr(const mxArray *array_ptr);
int *mxGetJc(const mxArray *array_ptr);

mxLogical *mxGetLogicals(const mxArray *array_ptr) {
  if (array_ptr->classID != mxLOGICAL_CLASS) return NULL;
  return (mxLogical*) array_ptr->realdata;
}

int mxGetM(const mxArray *array_ptr) {
  return (array_ptr->dims[0]);
}

int mxGetN(const mxArray *array_ptr) {
  return (array_ptr->dims[1]);
}

double mxGetNaN(void);

int mxGetNumberOfDimensions(const mxArray *array_ptr) {
  return (array_ptr->number_of_dims);
}

int mxGetNumberOfElements(const mxArray *array_ptr) {
  return CountElements(array_ptr->number_of_dims,array_ptr->dims);
}

int mxGetNumberOfFields(const mxArray *array_ptr) {
  return array_ptr->n_fields;
}

int mxGetNzmax(const mxArray *array_ptr);

double *mxGetPi(const mxArray *array_ptr) {
  return (double*) array_ptr->imagdata;
}

double *mxGetPr(const mxArray *array_ptr) {
  return (double*) array_ptr->realdata;
}

double mxGetScalar(const mxArray *array_ptr) {
  switch(array_ptr->classID) {
  case mxCELL_CLASS:
    return 0;
  case mxSTRUCT_CLASS:
    return 0;
  case mxLOGICAL_CLASS:
    return ((mxLogical*) array_ptr->realdata)[0];
  case mxCHAR_CLASS:
    return ((mxCHAR*) array_ptr->realdata)[0];
  case mxDOUBLE_CLASS:
    return ((double*) array_ptr->realdata)[0];
  case mxSINGLE_CLASS:
    return ((float*) array_ptr->realdata)[0];
  case mxINT8_CLASS:
    return ((int8*) array_ptr->realdata)[0];
  case mxUINT8_CLASS:
    return ((uint8*) array_ptr->realdata)[0];
  case mxINT16_CLASS:
    return ((int16*) array_ptr->realdata)[0];
  case mxUINT16_CLASS:
    return ((uint16*) array_ptr->realdata)[0];
  case mxINT32_CLASS:
    return ((int32*) array_ptr->realdata)[0];
  case mxUINT32_CLASS:
    return ((uint32*) array_ptr->realdata)[0];
  case mxINT64_CLASS:
    return ((int64*) array_ptr->realdata)[0];
  case mxUINT64_CLASS:
    return ((uint64*) array_ptr->realdata)[0];
  default:
    return 0;
  }
}

int mxGetString(const mxArray *array_ptr, char *buf, int buflen) {
  mxChar* ptr;
  int i;
  int elcount, tocopy;
  if (array_ptr->classID != mxCHAR_CLASS)
    return 1;
  ptr = (mxChar*) array_ptr->realdata;
  elcount = mxGetNumberOfElements(array_ptr);
  tocopy = MIN(elcount,buflen-1);
  for (i=0;i<tocopy;i++) {
    buf[i] = ptr[i];
  }
  buf[tocopy] = 0;
  if (tocopy < elcount) return 1;
  return 0;
}

bool mxIsCell(const mxArray *array_ptr) {
  return (array_ptr->classID == mxCELL_CLASS);
}

bool mxIsChar(const mxArray *array_ptr) {
  return (array_ptr->classID == mxCHAR_CLASS);
}

bool mxIsClass(const mxArray *array_ptr, const char *name) {
  if (strcmp(name,"cell")==0)
    return (array_ptr->classID == mxCELL_CLASS);
  if (strcmp(name,"char")==0)
    return (array_ptr->classID == mxCHAR_CLASS);
  if (strcmp(name,"double")==0)
    return (array_ptr->classID == mxDOUBLE_CLASS);
  if (strcmp(name,"int8")==0)
    return (array_ptr->classID == mxINT8_CLASS);
  if (strcmp(name,"int16")==0)
    return (array_ptr->classID == mxINT16_CLASS);
  if (strcmp(name,"int32")==0)
    return (array_ptr->classID == mxINT32_CLASS);
  if (strcmp(name,"int64")==0)
    return (array_ptr->classID == mxINT64_CLASS);
  if (strcmp(name,"logical")==0)
    return (array_ptr->classID == mxLOGICAL_CLASS);
  if (strcmp(name,"single")==0)
    return (array_ptr->classID == mxSINGLE_CLASS);
  if (strcmp(name,"struct")==0)
    return (array_ptr->classID == mxSTRUCT_CLASS);
  if (strcmp(name,"uint8")==0)
    return (array_ptr->classID == mxUINT8_CLASS);
  if (strcmp(name,"uint16")==0)
    return (array_ptr->classID == mxUINT16_CLASS);
  if (strcmp(name,"uint32")==0)
    return (array_ptr->classID == mxUINT32_CLASS);
  if (strcmp(name,"uint64")==0)
    return (array_ptr->classID == mxUINT64_CLASS);
  return false;
}

bool mxIsComplex(const mxArray *array_ptr) {
  return (array_ptr->iscomplex != 0);
}

bool mxIsDouble(const mxArray *array_ptr) {
  return (array_ptr->classID == mxDOUBLE_CLASS);
}

bool mxIsEmpty(const mxArray *array_ptr) {
  return (mxGetNumberOfElements(array_ptr) == 0);
}
bool mxIsFinite(double value) {
  return (!mxIsNaN(value) && !mxIsInf(value));
}

bool mxIsFromGlobalWS(const mxArray *array_ptr);

bool mxIsInf(double t) {
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

bool mxIsInt8(const mxArray *array_ptr) {
  return (array_ptr->classID == mxINT8_CLASS);
}

bool mxIsInt16(const mxArray *array_ptr) {
  return (array_ptr->classID == mxINT16_CLASS);
}

bool mxIsInt32(const mxArray *array_ptr) {
  return (array_ptr->classID == mxINT32_CLASS);
}

bool mxIsInt64(const mxArray *array_ptr) {
  return (array_ptr->classID == mxINT64_CLASS);
}

bool mxIsLogical(const mxArray *array_ptr) {
  return (array_ptr->classID == mxLOGICAL_CLASS);
}

bool mxIsLogicalScalar(const mxArray *array_ptr) {
  return (mxIsLogical(array_ptr) && (mxGetNumberOfElements(array_ptr) == 1));
}

bool mxIsLogicalScalarTrue(const mxArray *array_ptr) {
  return (mxIsLogical(array_ptr) &&
	  mxGetNumberOfElements(array_ptr) &&
	  ((bool) (mxGetScalar(array_ptr))));
}

bool mxIsNaN(double t) {
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

bool mxIsNumeric(const mxArray *array_ptr) {
  return (array_ptr->classID == mxDOUBLE_CLASS ||
	  array_ptr->classID == mxSINGLE_CLASS ||
	  array_ptr->classID == mxINT8_CLASS ||
	  array_ptr->classID == mxUINT8_CLASS ||
	  array_ptr->classID == mxINT16_CLASS ||
	  array_ptr->classID == mxUINT16_CLASS ||
	  array_ptr->classID == mxINT32_CLASS ||
	  array_ptr->classID == mxUINT32_CLASS ||
	  array_ptr->classID == mxINT64_CLASS ||
	  array_ptr->classID == mxUINT64_CLASS);;
}

bool mxIsSingle(const mxArray *array_ptr) {
  return (array_ptr->classID == mxSINGLE_CLASS);
}

bool mxIsSparse(const mxArray *array_ptr) {
  return (array_ptr->issparse);
}

bool mxIsStruct(const mxArray *array_ptr) {
  return (array_ptr->classID == mxSTRUCT_CLASS);
}

bool mxIsUint8(const mxArray *array_ptr) {
  return (array_ptr->classID == mxUINT8_CLASS);
}

bool mxIsUint16(const mxArray *array_ptr) {
  return (array_ptr->classID == mxUINT16_CLASS);
}

bool mxIsUint32(const mxArray *array_ptr) {
  return (array_ptr->classID == mxUINT32_CLASS);
}

bool mxIsUint64(const mxArray *array_ptr) {
  return (array_ptr->classID == mxUINT64_CLASS);
}

void *mxMalloc(size_t n) {
  void *p = malloc(n);
  RegisterPointer(p);
  return p;
}

void *mxRealloc(void* ptr, size_t size) {
  DeregisterPointer(ptr);
  ptr = realloc(ptr,size);
  RegisterPointer(ptr);
  return ptr;
}

void mxRemoveField(mxArray *array_ptr, int field_number);

void mxSetCell(mxArray *array_ptr, int index, mxArray *value) {
  if (!mxIsCell(array_ptr)) return;
  ((mxArray**) array_ptr->realdata)[index] = value;
}

int mxSetClassName(mxArray *array_ptr, const char *classname);

void mxSetData(mxArray *array_ptr, void *data_ptr) {
  array_ptr->realdata = data_ptr;
}

int mxSetDimensions(mxArray *array_ptr, const int *dims, int ndim) {
  while ((ndim>2) && (dims[ndim-1] == 1)) ndim--;
  array_ptr->number_of_dims = ndim;
  free(array_ptr->dims);
  array_ptr->dims = CopyDims(ndim,dims);
  return 0;
}

void mxSetField(mxArray *array_ptr, int index,  const char *field_name, mxArray *value) {
  mxSetFieldByNumber(array_ptr,index,mxGetFieldNumber(array_ptr,field_name),value);
}

void mxSetFieldByNumber(mxArray *array_ptr, int index,  int field_number, mxArray *value) {
  ((mxArray***)array_ptr->realdata)[field_number][index] = value;
}

void mxSetImagData(mxArray *array_ptr, void *pi) {
  array_ptr->imagdata = pi;
}

void mxSetIr(mxArray *array_ptr, int *ir);
void mxSetJc(mxArray *array_ptr, int *jc);

void mxSetM(mxArray *array_ptr, int m) {
  array_ptr->dims[0] = m;
}

void mxSetN(mxArray *array_ptr, int n) {
  array_ptr->dims[1] = n;
}

void mxSetNzmax(mxArray *array_ptr, int nzmax);

void mxSetPi(mxArray *array_ptr, double *pi) {
  array_ptr->imagdata = pi;
}

void mxSetPr(mxArray *array_ptr, double *pr) {
  array_ptr->realdata = pr;
}

int mexAtExit(void (*ExitFcn)(void));
int mexCallMATLAB(int nlhs, mxArray *plhs[], int nrhs,  mxArray *prhs[], const char *command_name);
void mexErrMsgIdAndTxt(const char *identifier, const char *error_msg, ...);

void mexErrMsgTxt(const char *error_msg) {
  throw std::string(error_msg);
}

int mexEvalString(const char *command);
const char *mexFunctionName(void);
const mxArray *mexGet(double handle, const char *property);
mxArray *mexGetVariable(const char *workspace, const char *var_name);
const mxArray *mexGetVariablePtr(const char *workspace, const char *var_name);
bool mexIsGlobal(const mxArray *array_ptr);
bool mexIsLocked(void);
void mexLock(void); 
void mexMakeArrayPersistent(mxArray *array_ptr);
void mexMakeMemoryPersistent(void *ptr);
int mexPrintf(const char *format, ...);
int mexPutVariable(const char *workspace, const char *var_name, const mxArray *array_ptr);
int mexSet(double handle, const char *property,  mxArray *value);
void mexSetTrapFlag(int trap_flag);
void mexUnlock(void); 
void mexWarnMsgIdAndTxt(const char *identifier, const char *warning_msg, ...);
void mexWarnMsgTxt(const char *warning_msg);
