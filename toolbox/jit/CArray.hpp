/*
 * Copyright (c) 2011 Samit Basu
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

// This provides a C-callable interface into the Array class for 
// the JIT to use.
#ifndef __CArray_hpp__
#define __CArray_hpp__

extern "C"
{
  const int CArray_Bool = 4;
  const int CArray_Float = 13;
  const int CArray_Double = 14;

  const int CJIT_Success = 0;
  const int CJIT_Prepfail = 1;
  const int CJIT_Runfail = -1;

  // Create an empty array
  void* carray_empty();
  // Create an array from a scalar
  void* carray_scalar(double data, int typecode);
  // Create an array of a given size and type
  void* carray_create(void* interp, double rows, double cols, int typecode, bool* flag);
  // Download variables from the interpreter
  bool carray_download_scalar(void* interp, void* ptr, const char *name, int typecode);
  // Download arrays from the interpreter
  bool carray_download_array(void* interp, void *ptr, const char *name, int typecode);
  // Download function from the interpreter
  bool carray_download_function(void* interp, void *ptr, const char *name);
  // Upload variables to the interpreter
  bool carray_upload_scalar(void* interp, void* ptr, const char *name, int typecode);
  // Upload arrays from the interpreter
  bool carray_upload_array(void* interp, void *ptr, const char *name);
  // Copy an array
  void* carray_copy(void* arg);
  // Delete an array
  void carray_free(void* arg);
  // Get the number of rows in the array
  double carray_rows(void* arg);
  // Get the number of cols in the array
  double carray_cols(void* arg);
  // Set an element in the array -- resizes if necessary
  bool carray_set_ss(void* interp, void* arg, double row, double col, double val);
  // Set an element in the array -- vector mode -- resizes if necessary
  bool carray_set_s(void* interp, void* arg, double row, double val);
  // Set with two array arguments
  bool carray_set_aa(void* interp, void* arg, void *ndxr, void *ndxc, void *val);
  // Set with a single array argument
  bool carray_set_a(void* interp, void* arg, void *ndx, void *val);
  // Get an element from the array -- returns false if the index is
  // out of bounds
  double carray_get_ss(void* interp, void* arg, double row, double col, bool *error);
  // Get an element from the array -- vector mode -- returns false if
  // the index is out of bounds
  double carray_get_s(void* interp, void* arg, double row, bool *error);
  // Get with two array arguments
  void* carray_get_aa(void* interp, void *arg, void *ndxr, void *ndxc, bool *error);
  // Get with one array argument
  void* carray_get_a(void* interp, void *arg, void *ndxr, bool *error);
  // A <- B
  bool carray_duplicate(void* interp, void* a, void *b);
  // Basic operators
  void* carray_add(void* interp, void *a, void *b, bool *flag);
  void* carray_hcat(void* interp, void *a, void *b, bool *flag);
  void* carray_vcat(void* interp, void *a, void *b, bool *flag);
  void* carray_sub(void* interp, void *a, void *b, bool *flag);
  void* carray_times(void* interp, void *a, void *b, bool *flag);
  void* carray_pow(void* interp, void *a, void *b, bool *flag);
  void* carray_dpow(void* interp, void *a, void *b, bool *flag);
  void* carray_dtimes(void* interp, void *a, void *b, bool *flag);
  void* carray_rdiv(void* interp, void *a, void *b, bool *flag);
  void* carray_drdiv(void* interp, void *a, void *b, bool *flag);
  void* carray_ldiv(void* interp, void *a, void *b, bool *flag);
  void* carray_dldiv(void* interp, void *a, void *b, bool *flag);
  void* carray_colon(void* interp, double a, double b, bool *flag);
  void* carray_dcolon(void* interp, double a, double b, double c, bool *flag);
  // Boolean ops
  void* carray_or(void* interp, void *a, void *b, bool *flag);
  void* carray_and(void* interp, void *a, void *b, bool *flag);
  void* carray_lt(void* interp, void *a, void *b, bool *flag);
  void* carray_le(void* interp, void *a, void *b, bool *flag);
  void* carray_gt(void* interp, void *a, void *b, bool *flag);
  void* carray_ge(void* interp, void *a, void *b, bool *flag);
  void* carray_eq(void* interp, void *a, void *b, bool *flag);
  void* carray_neq(void* interp, void *a, void *b, bool *flag);
  // Unary ops
  void* carray_pos(void* interp, void *a, bool *flag);
  void* carray_neg(void* interp, void *a, bool *flag);
  void* carray_not(void* interp, void *a, bool *flag);
  void* carray_transpose(void* interp, void *a, bool *flag);
  void* carray_dottranspose(void* interp, void *a, bool *flag);
  bool carray_any(void* interp, void *a, bool *flag);
  bool carray_all(void* interp, void *a, bool *flag);
  // Compute iterations for a loop
  double num_for_loop_iter(double first, double step, double last);
  void* carray_invoke_1(void* interp, void* func, void* arg, bool *flag);
  void* carray_invoke_2(void* interp, void* func, void* arg1, void *arg2, bool *flag);

#include "CScalarFuncs.hpp"

}

template <typename V>
inline int MapTypeToDataClass(V c);

template <>
inline int MapTypeToDataClass(float c) {return CArray_Float;}

template <>
inline int MapTypeToDataClass(double c) {return CArray_Double;}

template <>
inline int MapTypeToDataClass(bool c) {return CArray_Bool;}

// This is the JITArray class - it isn't used by the FreeMat application
// but is used by the CJIT code to wrap these carray functions.  I am
// placing here to keep it in sync with the carray functions.

template <class T>
class JITArray
{
  void *dp;
public:
  JITArray() {dp = carray_empty();}
  JITArray(void* p) {dp = p;}
  explicit JITArray(T p)
  {
    dp = carray_scalar(p,MapTypeToDataClass(T(0)));
  }
  JITArray(double rows, double cols)
  {
    dp = carray_create(rows,cols,MapTypeToDataClass(T(0)));
  }
  JITArray(const JITArray<T>& copy) 
  { 
    dp = carray_copy(copy.dp); 
  }
  ~JITArray() {carray_free(dp);}
  double rows() const {return carray_rows(dp);}
  double cols() const {return carray_cols(dp);}
  double len() const {return rows()*cols();}
  void* data() const {return dp;}
  JITArray<T>& operator=(const JITArray<T>& copy)
  {
    carray_free(dp);
    dp = carray_copy(copy.dp);
    return *this;
  }
};

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<T>& B)
{
  return carray_duplicate(interp,A.data(),B.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, double val, const JITArray<double>& B)
{
  return carray_set_a(interp,A.data(),B.data(),JITArray<double>(val).data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<double> &val, double B)
{
  return carray_set_a(interp,A.data(),JITArray<double>(B).data(),val.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<double> &val, const JITArray<double>& B)
{
  return carray_set_a(interp,A.data(),B.data(),val.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, double val, const JITArray<double>& B, const JITArray<double>& C)
{
  return carray_set_aa(interp,A.data(),B.data(),C.data(),JITArray<double>(val).data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, double val, double B, const JITArray<double>& C)
{
  return carray_set_aa(interp,A.data(),JITArray<double>(B).data(),C.data(),JITArray<double>(val).data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, double val, const JITArray<double>& B, double C)
{
  return carray_set_aa(interp,A.data(),B.data(),JITArray<double>(C).data(),JITArray<double>(val).data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<double> &val, const JITArray<double>& B, const JITArray<double>& C)
{
  return carray_set_aa(interp,A.data(),B.data(),C.data(),val.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<double> &val, double B, const JITArray<double>& C)
{
  return carray_set_aa(interp,A.data(),JITArray<double>(B).data(),C.data(),val.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T>& A, const JITArray<double> &val, const JITArray<double>& B, double C)
{
  return carray_set_aa(interp,A.data(),B.data(),JITArray<double>(C).data(),val.data());
}

template <class T>
inline bool Set(void* interp, JITArray<T> &A, double val, double row, double col)
{
  return carray_set_ss(interp,A.data(),row,col,val);
}

template <class T>
inline bool Set(void* interp, JITArray<T> &A, double val, double row)
{
  return carray_set_s(interp,A.data(),row,val);
}

template <class T>
bool Set(void*, T& A, T val)
{
  A = val;
  return true;
}

template <class T>
inline T Get(void*interp, const JITArray<T> & A, double row, double col, bool *error)
{
  return carray_get_ss(interp,A.data(),row,col,error);
}

template <class T>
inline T Get(void*interp, const JITArray<T> & A, double row, bool *error)
{
  return carray_get_s(interp,A.data(),row,error);
}

template <class T>
inline JITArray<T> Get(void*interp, const JITArray<T> & A, const JITArray<double> &rndx, bool *error)
{
  return JITArray<T>(carray_get_a(interp,A.data(),rndx.data(),error));
}

template <class T>
inline JITArray<T> Get(void*interp, const JITArray<T> & A, const JITArray<double> &rndx, const JITArray<double> &cndx, bool *error)
{
  return JITArray<T>(carray_get_aa(interp,A.data(),rndx.data(),cndx.data(),error));
}

template <class T>
inline JITArray<T> Get(void*interp, const JITArray<T> & A, double rndx, const JITArray<double> &cndx, bool *error)
{
  return Get(interp,A,JITArray<double>(rndx),cndx,error);
}

template <class T>
inline JITArray<T> Get(void*interp, const JITArray<T> & A, const JITArray<double> &rndx, double cndx, bool *error)
{
  return Get(interp,A,rndx,JITArray<double>(cndx),error);
}

inline JITArray<double> Invoke(void *interp, void *ptr, double a, bool *error)
{
  return JITArray<double>(carray_invoke_1(interp,ptr,JITArray<double>(a).data(),error));
}

inline JITArray<double> Invoke(void *interp, void *ptr, const JITArray<double> &a, bool *error)
{
  return JITArray<double>(carray_invoke_1(interp,ptr,a.data(),error));
}

inline JITArray<double> Invoke(void *interp, void *ptr, double a, const JITArray<double> &b, bool *error)
{
  return JITArray<double>(carray_invoke_2(interp,ptr,JITArray<double>(a).data(),b.data(),error));
}

inline JITArray<double> Invoke(void *interp, void *ptr, const JITArray<double> &a, double b, bool *error)
{
  return JITArray<double>(carray_invoke_2(interp,ptr,a.data(),JITArray<double>(b).data(),error));
}

inline JITArray<double> Invoke(void *interp, void *ptr, double a, double b, bool *error)
{
  return JITArray<double>(carray_invoke_2(interp,ptr,JITArray<double>(a).data(),JITArray<double>(b).data(),error));
}

inline JITArray<double> Invoke(void *interp, void *ptr, const JITArray<double> &a, const JITArray<double> &b, bool *error)
{
  return JITArray<double>(carray_invoke_2(interp,ptr,a.data(),b.data(),error));
}

template <class T> bool Any(void*, T a, bool *error) {return ((bool) a);}
template <class T> bool All(void*, T a, bool *error) {return ((bool) a);}
template <class T> bool Not(void*, T a, bool *error) {return !((bool) a);}
template <class T> T Neg(void*, T a, bool *error) {return -a;}
template <class T> T Pos(void*, T a, bool *error) {return a;}
template <class T> T Transpose(void*, T a, bool *error) {return a;}
template <class T> T DotTranspose(void*, T a, bool *error) {return a;}
template <class T> T Power(void*, T a, T b, bool *error) {return pow(a,b);}
template <class T> T DotPower(void*, T a, T b, bool *error) {return pow(a,b);}
template <class T> T Add(void*, T a, T b, bool *error) {return a+b;}
template <class T> T Sub(void*, T a, T b, bool *error) {return a-b;}
template <class T> T Times(void*, T a, T b, bool *error) {return a*b;}
template <class T> T DotTimes(void*, T a, T b, bool *error){return a*b;}
template <class T> T RightDivide(void*, T a, T b, bool *error) {return a/b;}
template <class T> T LeftDivide(void*, T a, T b, bool *error) {return b/a;}
template <class T> T DotRightDivide(void*, T a, T b, bool *error) {return a/b;}
template <class T> T DotLeftDivide(void*, T a, T b, bool *error) {return b/a;}
template <class T> bool Or(void*, T a, T b, bool *error) {return (a || b);}
template <class T> bool And(void*, T a, T b, bool *error) {return (a && b);}
template <class T> bool LT(void*, T a, T b, bool *error) {return (a < b);}
template <class T> bool LE(void*, T a, T b, bool *error) {return (a <= b);}
template <class T> bool GT(void*, T a, T b, bool *error) {return (a > b);}
template <class T> bool GE(void*, T a, T b, bool *error) {return (a >= b);}
template <class T> bool EQ(void*, T a, T b, bool *error) {return (a == b);}
template <class T> bool NEQ(void*, T a, T b, bool *error) {return (a != b);}
template <class T> JITArray<T> Colon(void* interp, T a, T b, bool *error) {
  return JITArray<T>(carray_colon(interp,a,b,error));
}
template <class T> JITArray<T> DoubleColon(void* interp, T a, T b, T c, bool *error) {
  return JITArray<T>(carray_dcolon(interp,a,b,c,error));
}

#define OpCases(name,mappedname)					\
  template <class T>							\
  inline JITArray<T> name(void *interp, const JITArray<T> &a, const JITArray<T> &b, bool *error) \
  {									\
    return JITArray<T>(mappedname(interp,a.data(),b.data(),error));	\
  }									\
									\
  template <class T>							\
  inline JITArray<T> name(void *interp, const JITArray<T> &a, T b, bool *error)	\
  {									\
    return name(interp,a,JITArray<T>(b), error);				\
  }									\
									\
  template <class T>							\
  inline JITArray<T> name(void *interp,T a, const JITArray<T> &b, bool *error)	\
  {									\
    return name(interp,JITArray<T>(a),b, error);				\
  }

template <class T>
inline JITArray<T> VCat(void* interp,T a, T b, bool *error)
{
  return VCat(interp,JITArray<T>(a),JITArray<T>(b),error);
}

template <class T>
inline JITArray<T> HCat(void* interp,T a, T b, bool *error)
{
  return HCat(interp,JITArray<T>(a),JITArray<T>(b),error);
}

OpCases(VCat,carray_vcat);
OpCases(HCat,carray_hcat);
OpCases(Add,carray_add);
OpCases(Power,carray_pow);
OpCases(DotPower,carray_dpow);
OpCases(Sub,carray_sub);
OpCases(Times,carray_times);
OpCases(DotTimes,carray_dtimes);
OpCases(RightDivide,carray_rdiv);
OpCases(DotRightDivide,carray_drdiv);
OpCases(LeftDivide,carray_ldiv);
OpCases(DotLeftDivide,carray_dldiv);

#define BinOpCases(name,mappedname)					\
  template <class T>							\
  inline JITArray<bool> name(void *interp, const JITArray<T> &a, const JITArray<T> &b, bool *error) \
  {									\
  return JITArray<bool>(mappedname(interp,a.data(),b.data(),error)); \
  }									\
  									\
  template <class T>							\
  inline JITArray<bool> name(void *interp, const JITArray<T> &a, T b, bool *error)	\
  {									\
    return name(interp, a,JITArray<T>(b),error);				\
  }									\
									\
  template <class T>							\
  inline JITArray<bool> name(void *interp, T a, const JITArray<T> &b, bool *error)	\
  {									\
    return name(interp, JITArray<T>(a),b,error);				\
  }

BinOpCases(Or,carray_or);
BinOpCases(And,carray_and);
BinOpCases(LT,carray_lt);
BinOpCases(LE,carray_le);
BinOpCases(GT,carray_gt);
BinOpCases(GE,carray_ge);
BinOpCases(EQ,carray_eq);
BinOpCases(NEQ,carray_neq);

template <class T> bool Any(void *interp, const JITArray<T> &a, bool *error) 
{
  return carray_any(interp,a.data(),error);
}

template <class T> bool All(void *interp, const JITArray<T> &a, bool *error) 
{
  return carray_all(interp,a.data(),error);
}

template <class T> JITArray<bool> Not(void *interp, const JITArray<T> &a, bool *error) 
{
  return carray_not(interp,a.data(),error);
}

template <class T> JITArray<T> Neg(void *interp, const JITArray<T> &a, bool *error) 
{
  return JITArray<T>(carray_neg(interp,a.data(),error));
}

template <class T> JITArray<T> Pos(void *interp, const JITArray<T> &a, bool *error) 
{
  return JITArray<T>(carray_pos(interp,a.data(),error));
}

template <class T> JITArray<T> Transpose(void *interp, const JITArray<T> &a, bool *error) 
{
  return JITArray<T>(carray_transpose(interp,a.data(),error));
}

template <class T> JITArray<T> DotTranspose(void *interp, const JITArray<T> &a, bool *error) 
{
  return JITArray<T>(carray_dottranspose(interp,a.data(),error));
}

#endif
