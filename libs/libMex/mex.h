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

#ifndef __mex_h__
#define __mex_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

// Define a mex interface into FreeMat...
//
// How does this work? we provide a library libmex that we link candidate code to
// to form a new module/dll.  When we load that library, we establish a bridge 
// from it to the current interpreter.  That bridge is used to manipulate the 
// interpreter.
typedef enum {
  mxUNKNOWN_CLASS = 0,
  mxCELL_CLASS,
  mxSTRUCT_CLASS,
  mxLOGICAL_CLASS,
  mxCHAR_CLASS,
  dummy_CLASS,
  mxDOUBLE_CLASS,
  mxSINGLE_CLASS,
  mxINT8_CLASS,
  mxUINT8_CLASS,
  mxINT16_CLASS,
  mxUINT16_CLASS,
  mxINT32_CLASS,
  mxUINT32_CLASS,
  mxINT64_CLASS,
  mxUINT64_CLASS,
  mxFUNCTION_CLASS,
} mxClassID;

typedef unsigned short mxChar;

typedef enum {
  mxREAL=0, 
  mxCOMPLEX
} mxComplexity;

typedef struct {
  mxClassID classID;
  int *dims;
  int number_of_dims;
  int issparse;
  int n_fields;
  int iscomplex;
  const char **fieldnames;
  void *realdata;
  void *imagdata;
  int refID;
} mxArray;

#ifndef __cplusplus
typedef int bool;
#endif

typedef bool mxLogical;

typedef short mxCHAR;

int mxAddField(mxArray *array_ptr, const char *field_name);
char* mxArrayToString(const mxArray *array_ptr);
void mxAssert(int expr, char *error_message);
void mxAssertS(int expr, char *error_message);
int mxCalcSingleSubscript(const mxArray *array_ptr, int nsubs, int *subs);
void *mxCalloc(size_t n, size_t size);
mxArray *mxCreateCellArray(int ndim, const int *dims);
mxArray *mxCreateCellMatrix(int m, int n);
mxArray *mxCreateCharArray(int ndim, const int *dims);
mxArray *mxCreateCharMatrixFromStrings(int m, const char **str);
mxArray *mxCreateDoubleMatrix(int m, int n, mxComplexity ComplexFlag);
mxArray *mxCreateDoubleScalar(double value);
mxArray *mxCreateLogicalArray(int ndim, const int *dims);
mxArray *mxCreateLogicalMatrix(int m, int n);
mxArray *mxCreateLogicalScalar(mxLogical value);
mxArray *mxCreateNumericArray(int ndim, const int *dims, mxClassID clss, mxComplexity ComplexFlag);
mxArray *mxCreateNumericMatrix(int m, int n, mxClassID clss, mxComplexity ComplexFlag);
mxArray *mxCreateScalarDouble(double value);
mxArray *mxCreateSparse(int m, int n, int nzmax, mxComplexity ComplexFlag);
mxArray *mxCreateSparseLogicalMatrix(int m, int n, int nzmax);
mxArray *mxCreateString(const char *str);
mxArray *mxCreateStructArray(int ndim, const int *dims, int nfields, const char **field_names);
mxArray *mxCreateStructMatrix(int m, int n, int nfields, const char **field_names);
void mxDestroyArray(mxArray *array_ptr);
mxArray *mxDuplicateArray(const mxArray *in);
void mxFree(void *ptr);
mxArray *mxGetCell(const mxArray *array_ptr, int index);
mxChar *mxGetChars(const mxArray *array_ptr);
mxClassID mxGetClassID(const mxArray *array_ptr);
const char *mxGetClassName(const mxArray *array_ptr);
void *mxGetData(const mxArray *array_ptr);
const int *mxGetDimensions(const mxArray *array_ptr);
int mxGetElementSize(const mxArray *array_ptr);
double mxGetEps(void);
mxArray *mxGetField(const mxArray *array_ptr, int index, const char *field_name);
mxArray *mxGetFieldByNumber(const mxArray *array_ptr, int index, int field_number);
const char *mxGetFieldNameByNumber(const mxArray *array_ptr, int field_number);
int mxGetFieldNumber(const mxArray *array_ptr, const char *field_name);
void *mxGetImagData(const mxArray *array_ptr);
double mxGetInf(void);
int *mxGetIr(const mxArray *array_ptr);
int *mxGetJc(const mxArray *array_ptr);
mxLogical *mxGetLogicals(const mxArray *array_ptr);
int mxGetM(const mxArray *array_ptr);
int mxGetN(const mxArray *array_ptr);
double mxGetNaN(void);
int mxGetNumberOfDimensions(const mxArray *array_ptr);
int mxGetNumberOfElements(const mxArray *array_ptr);
int mxGetNumberOfFields(const mxArray *array_ptr);
int mxGetNzmax(const mxArray *array_ptr);
double *mxGetPi(const mxArray *array_ptr);
double *mxGetPr(const mxArray *array_ptr);
double mxGetScalar(const mxArray *array_ptr);
int mxGetString(const mxArray *array_ptr, char *buf, int buflen);
bool mxIsCell(const mxArray *array_ptr);
bool mxIsChar(const mxArray *array_ptr);
bool mxIsClass(const mxArray *array_ptr, const char *name);
bool mxIsComplex(const mxArray *array_ptr);
bool mxIsDouble(const mxArray *array_ptr);
bool mxIsEmpty(const mxArray *array_ptr);
bool mxIsFinite(double value);
bool mxIsFromGlobalWS(const mxArray *array_ptr);
bool mxIsInf(double value);
bool mxIsInt8(const mxArray *array_ptr);
bool mxIsInt16(const mxArray *array_ptr);
bool mxIsInt32(const mxArray *array_ptr);
bool mxIsInt64(const mxArray *array_ptr);
bool mxIsLogical(const mxArray *array_ptr);
bool mxIsLogicalScalar(const mxArray *array_ptr);
bool mxIsLogicalScalarTrue(const mxArray *array_ptr);
bool mxIsNaN(double value);
bool mxIsNumeric(const mxArray *array_ptr);
bool mxIsSingle(const mxArray *array_ptr);
bool mxIsSparse(const mxArray *array_ptr);
bool mxIsStruct(const mxArray *array_ptr);
bool mxIsUint8(const mxArray *array_ptr);
bool mxIsUint16(const mxArray *array_ptr);
bool mxIsUint32(const mxArray *array_ptr);
bool mxIsUint64(const mxArray *array_ptr);
void *mxMalloc(size_t n);
void *mxRealloc(void* ptr, size_t size);
void mxRemoveField(mxArray *array_ptr, int field_number);
void mxSetCell(mxArray *array_ptr, int index, mxArray *value);
int mxSetClassName(mxArray *array_ptr, const char *classname);
void mxSetData(mxArray *array_ptr, void *data_ptr);
int mxSetDimensions(mxArray *array_ptr, const int *dims, int ndim);
void mxSetField(mxArray *array_ptr, int index,  const char *field_name, mxArray *value);
void mxSetFieldByNumber(mxArray *array_ptr, int index,  int field_number, mxArray *value);
void mxSetImagData(mxArray *array_ptr, void *pi);
void mxSetIr(mxArray *array_ptr, int *ir);
void mxSetJc(mxArray *array_ptr, int *jc);
void mxSetM(mxArray *array_ptr, int m);
void mxSetN(mxArray *array_ptr, int n);
void mxSetNzmax(mxArray *array_ptr, int nzmax);
void mxSetPi(mxArray *array_ptr, double *pi);
void mxSetPr(mxArray *array_ptr, double *pr);
int mexAtExit(void (*ExitFcn)(void));
int mexCallMATLAB(int nlhs, mxArray *plhs[], int nrhs,  mxArray *prhs[], const char *command_name);
void mexErrMsgIdAndTxt(const char *identifier, const char *error_msg, ...);
void mexErrMsgTxt(const char *error_msg);
int mexEvalString(const char *command);
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
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

#ifdef __cplusplus
}
#endif

#endif
