#include "CArray.hpp"
#include "Array.hpp"
#include "Interpreter.hpp"
#include "Algorithms.hpp"
#include "Math.hpp"

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
    //extern "C" __declspec(dllexport) void* __cxa_allocate_exception(unsigned);
    //extern "C" __declspec(dllexport) void *__cxa_allocate_exception(size_t thrown_size) throw();
#else
    #define EXPORT
#endif

// Copy on write is causing a problem now...
// if dp is set to the const pointer, then
// doing a write into the array may change
// two arrays.  For example:
//  A = zeros(1,100);
//  B = A;
//  for i=1:100; A(i) = i; end;
//
struct CArray
{
  size_t rows;
  size_t cols;
  void *dp;
  Array sp;
  int typecode;
  bool valid_for_writes;
};

struct CFunc
{
  FuncPtr fp;
  bool    scalar;
  void    *sp;
};

static int ByteCount(int typecode)
{
  switch (typecode)
    {
    case CArray_Bool : return sizeof(bool);
    case CArray_Float : return sizeof(float);
    case CArray_Double : return sizeof(double);
    default:
      throw Exception("Unknown typecode");
    }
}

CArray* cast(void*p)
{
  return ((CArray*)p);
}

extern "C" EXPORT
void* carray_read_ptr(void* p)
{
  CArray* q = cast(p);
  return q->dp;
}

void validate_writes(CArray* q)
{
  q->dp = q->sp.getVoidPointer();
  q->valid_for_writes = true;
}

extern "C" EXPORT
void* carray_write_ptr(void* p)
{
  CArray* q = cast(p);
  if (!q->valid_for_writes) validate_writes(q);
  return q->dp;
}

extern "C" EXPORT
void* carray_scalar(double data, int typecode)
{
  CArray *p = new CArray;
  p->rows = 1;
  p->cols = 1;
  p->sp = Array::create((DataClass)(typecode),NTuple(1,1));
  p->dp = p->sp.getVoidPointer();
  switch (typecode)
    {
    case CArray_Bool:
      ((bool*)(p->dp))[0] = data;
      break;
    case CArray_Float:
      ((float*)(p->dp))[0] = data;
      break;
    case CArray_Double:
      ((double*)(p->dp))[0] = data;
      break;
    default:
      ;
    }
  p->valid_for_writes = true;
  p->typecode = typecode;
  return p;
}

extern "C" EXPORT
void* carray_create(void* interp, double rows, double cols, int typecode, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *p = new CArray;
      p->rows = rows;
      p->cols = cols;
      p->sp = Array::create((DataClass)(typecode),NTuple(rows,cols));
      p->dp = p->sp.getVoidPointer();
      p->valid_for_writes = true;
      p->typecode = typecode;
      return p;
    }
  catch (Exception &e)
    {
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      *flag = true;
    }
}

static void* carray_capture(const Array &a)
{
  CArray *p = new CArray;
  p->sp = a;
  p->rows = a.rows();
  p->cols = a.cols();
  p->dp = (void*) a.getConstVoidPointer();
  p->valid_for_writes = false;
  p->typecode = a.dataClass();
  return p;
}

void update_cache(void* arg)
{
  CArray *cp = cast(arg);
  cp->rows = cp->sp.rows();
  cp->cols = cp->sp.cols();
  cp->dp = (void*) cp->sp.getConstVoidPointer();
  cp->valid_for_writes = false;
  cp->typecode = cp->sp.dataClass();
}

extern "C" EXPORT
void* carray_empty()
{
  CArray *cp = new CArray;
  cp->sp = EmptyConstructor();
  return cp;
}

extern "C" EXPORT
void* carray_copy(void* arg)
{
  return new CArray(*((CArray*)arg));
}

extern "C" EXPORT
bool carray_download_function(void *interp, void *dst, const char *name)
{
  try
    {
      Interpreter *eval = (Interpreter*) interp;
      FuncPtr fp;
      if (!eval->getContext()->lookupFunction(name,fp))
	throw Exception("Unable to find function");
      fp->updateCode(eval);
      CFunc *cp = new CFunc;
      cp->fp = fp;
      *((FunctionDef**)dst) = (FunctionDef*)fp;
      return true;
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
void* carray_invoke_1(void* interp, void* func, void* arg, bool *flag)
{
  if (*flag) return NULL;
  Interpreter *eval = (Interpreter*) interp;
  try
    {
      FunctionDef *fp = (FunctionDef*) func;
      ArrayVector args;
      CArray *a1 = cast(arg);
      args.push_back(a1->sp);
      ArrayVector ret = fp->evaluateFunc(eval,args,1);
      if (ret.size() > 0)
	return carray_capture(ret[0]);
      else
	return carray_empty();
    }
  catch (Exception &e)
    {
      eval->setLastErrorString(e.msg());
      *flag = true;
    }
}

extern "C" EXPORT
void* carray_invoke_2(void* interp, void* func, void* arg1, void* arg2, bool *flag)
{
  if (*flag) return NULL;
  Interpreter *eval = (Interpreter*) interp;
  try
    {
      FunctionDef *fp = (FunctionDef*) func;
      ArrayVector arg;
      CArray *a1 = cast(arg1);
      CArray *a2 = cast(arg2);
      arg.push_back(a1->sp);
      arg.push_back(a2->sp);
      ArrayVector ret = fp->evaluateFunc(eval,arg,1);
      if (ret.size() > 0)
	return carray_capture(ret[0]);
      else
	return carray_empty();
    }
  catch (Exception &e)
    {
      eval->setLastErrorString(e.msg());
      *flag = true;
    }
}

extern "C" EXPORT
bool carray_download_scalar(void *interp, void* dst, const char *name, int typecode)
{
  try
    {
      Interpreter *eval = (Interpreter*) interp;
      ArrayReference ptr(eval->getContext()->lookupVariable(name));
      if (!ptr.valid())
	{
	  eval->getContext()->insertVariable(name,Array((DataClass)(typecode),NTuple(1,1)));
	  ptr = eval->getContext()->lookupVariable(name);
	}
      if (!ptr->isScalar()) throw Exception("Ugh");
      if (ptr->dataClass() != typecode) throw Exception("Ugh");
      memcpy(dst,ptr->getVoidPointer(),ByteCount(typecode));
      return true;
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
bool carray_download_array(void *interp, void *data, const char *name, int typecode)
{
  try
    {
      Interpreter *eval = (Interpreter*) interp;
      ArrayReference ptr(eval->getContext()->lookupVariable(name));
      if (!ptr.valid())
	{
	  eval->getContext()->insertVariable(name,Array((DataClass)(typecode),NTuple(1,1)));
	  ptr = eval->getContext()->lookupVariable(name);
	}
      if (ptr->dataClass() != (DataClass)(typecode)) throw Exception("Ugh");
      CArray *cp = cast(data);
      cp->sp = *ptr;
      update_cache(data);
      return true;
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
bool carray_upload_scalar(void *interp, void* data, const char *name,
			  int typecode)
{
  Interpreter *eval = (Interpreter*) interp;
  try
    {
      ArrayReference ptr(eval->getContext()->lookupVariable(name));
      if (!ptr.valid())
	{
	  eval->getContext()->insertVariable(name,Array((DataClass)(typecode),NTuple(1,1)));
	  ptr = eval->getContext()->lookupVariable(name);
	}
      if (!ptr->isScalar()) throw Exception("Internal JIT failure!");
      if (ptr->dataClass() != (DataClass)(typecode))
	throw Exception("Internal JIT failure!");
      memcpy(ptr->getVoidPointer(),data,ByteCount(typecode));
      return true;
    }
  catch (Exception &e)
    {
      eval->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
bool carray_upload_array(void *interp, void *data, const char *name)
{
  Interpreter *eval = (Interpreter*) interp;
  try
    {
      ArrayReference ptr(eval->getContext()->lookupVariable(name));
      CArray *cp = cast(data);
      if (!ptr.valid())
	{
	  eval->getContext()->insertVariable(name,cp->sp);
	  return true;
	}
      *ptr = cp->sp;
      return true;
    }
  catch (Exception &e)
    {
      eval->setLastErrorString(e.msg());
      return false;
    }
}

// Delete an array
extern "C" EXPORT
void carray_free(void* arg)
{
  delete (cast(arg));
}

// Get the number of rows in the array
extern "C" EXPORT
double carray_rows(void* arg)
{
  return (cast(arg)->rows);
}

// Get the number of cols in the array
extern "C" EXPORT
double carray_cols(void* arg)
{
  return (cast(arg)->cols);
}

extern "C" EXPORT
bool carray_set_ss(void* interp, void *arg, double row, double col, double val)
{
  try
    {
      CArray *cp = cast(arg);
      if (!cp->valid_for_writes) validate_writes(cp);
      if ((row < 1) || (col < 1))
	{
	  Interpreter *eval = (Interpreter*) interp;
	  eval->setLastErrorString("index values must be >= 1");
	  return false;
	}
      if ((row > cp->rows) || (col > cp->cols))
	{
	  cp->sp.set(NTuple(row,col),Array(val));
	  update_cache(arg);
	  validate_writes(cast(arg));
	  return true;
	}
      size_t irow = (size_t) row-1;
      size_t icol = (size_t) col-1;
      switch (cp->typecode)
	{
	case CArray_Bool:
	  ((bool*)(cp->dp))[irow+icol*cp->rows] = val;
	  return true;
	case CArray_Float:
	  ((float*)(cp->dp))[irow+icol*cp->rows] = val;
	  return true;
	case CArray_Double:
	  ((double*)(cp->dp))[irow+icol*cp->rows] = val;
	  return true;
	default:
	  return false;
	}
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
    }
  return false;
}

extern "C" EXPORT
bool carray_set_s(void* interp, void *arg, double row, double val)
{
  try
    {
      CArray *cp = cast(arg);
      if (!cp->valid_for_writes) validate_writes(cp);
      if (row < 1)
	{
	  Interpreter *eval = (Interpreter*) interp;
	  eval->setLastErrorString("index values must be >= 1");
	  return false;
	}
      if (row > cp->rows*cp->cols)
	{
	  cp->sp.set(row,Array(val));
	  update_cache(arg);
	  validate_writes(cast(arg));
	  return true;
	}
      size_t irow = (size_t) row-1;
      switch (cp->typecode)
	{
	case CArray_Bool:
	  ((bool*)(cp->dp))[irow] = val;
	  return true;
	case CArray_Float:
	  ((float*)(cp->dp))[irow] = val;
	  return true;
	case CArray_Double:
	  ((double*)(cp->dp))[irow] = val;
	  return true;
	default:
	  return false;
	}
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
    }
  return false;
}

extern "C" EXPORT
bool carray_set_a(void* interp, void* arg, void* ndx, void *val)
{
  try
    {
      CArray *ap = cast(arg);
      CArray *nd = cast(ndx);
      CArray *vc = cast(val);
      if (!ap->valid_for_writes) validate_writes(ap);
      ap->sp.set(nd->sp,vc->sp);
      update_cache(ap);
      return true;
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
    }
  return false;
}

extern "C" EXPORT
bool carray_set_aa(void* interp, void* arg, void* ndxr, void* ndxc, void *val)
{
  try
    {
      CArray *ap = cast(arg);
      CArray *ndr = cast(ndxr);
      CArray *ndc = cast(ndxc);
      CArray *vc = cast(val);
      if (!ap->valid_for_writes) validate_writes(ap);
      ArrayVector ndx;
      ndx.push_back(ndr->sp);
      ndx.push_back(ndc->sp);
      ap->sp.set(ndx,vc->sp);
      update_cache(ap);
      return true;
    }
  catch (Exception &e)
    {
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
    }
  return false;
}

extern "C" EXPORT
void* carray_get_a(void* interp, void* arg, void* ndx, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *ap = cast(arg);
      CArray *nd = cast(ndx);
      return carray_capture(ap->sp.get(nd->sp));
    }
  catch (Exception &e)
    {
      *flag = true;
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return NULL;
    }
}

extern "C" EXPORT
void* carray_get_aa(void* interp, void* arg, void* rndx, void* cndx, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *ap = cast(arg);
      CArray *ndr = cast(rndx);
      CArray *ndc = cast(cndx);
      ArrayVector ndx;
      ndx.push_back(ndr->sp);
      ndx.push_back(ndc->sp);
      return carray_capture(ap->sp.get(ndx));
    }
  catch (Exception &e)
    {
      *flag = true;
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return NULL;
    }
}

extern "C" EXPORT
double carray_get_ss(void* interp, void* arg, double row, double col, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *cp = cast(arg);
      if ((row < 1) || (col < 1) || (row > cp->rows) || (col > cp->cols))
	{
	  *flag = true;
	  Interpreter *eval = (Interpreter*) interp;
	  eval->setLastErrorString("out of range");
	  return 0;
	}
      size_t irow = (size_t) row-1;
      size_t icol = (size_t) col-1;
      switch (cp->typecode)
	{
	case CArray_Bool:
	  return ((bool*)(cp->dp))[irow+icol*cp->rows];
	case CArray_Float:
	  return ((float*)(cp->dp))[irow+icol*cp->rows];
	case CArray_Double:
	  return ((double*)(cp->dp))[irow+icol*cp->rows];
	}
    }
  catch (Exception &e)
    {
      *flag = true;
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
     return 0;
    }
}

extern "C" EXPORT
double carray_get_s(void* interp, void* arg, double row, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *cp = cast(arg);
      if ((row < 1) || (row > (cp->rows)*(cp->cols)))
	{
	  *flag = true;
	  Interpreter *eval = (Interpreter*) interp;
	  eval->setLastErrorString("out of range");
	  return 0;
	}
      size_t irow = (size_t) row - 1;
      switch (cp->typecode)
	{
	case CArray_Bool:
	  return ((bool*)(cp->dp))[irow];
	case CArray_Float:
	  return ((float*)(cp->dp))[irow];
	case CArray_Double:
	  return ((double*)(cp->dp))[irow];
	}
    }
  catch (Exception &e)
    {
      *flag = true;
      Interpreter *eval = (Interpreter*) interp;
      eval->setLastErrorString(e.msg());
      return 0;
    }
}

extern "C" EXPORT
bool carray_duplicate(void *interp, void *a, void *b)
{
  try
    {
      CArray *ap = cast(a);
      CArray *bp = cast(b);
      ap->sp = bp->sp;
      update_cache(ap);
      return true;
    }
  catch (Exception &e)
    {
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
void* carray_colon(void *interp, double a, double b, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *cp = cast(carray_empty());
      cp->sp = RangeConstructor(a,1,b,false);
      update_cache(cp);
      return cp;
    }
  catch (Exception &e)
    {
      *flag = true;
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      return NULL;
    }
}

extern "C" EXPORT
void* carray_dcolon(void *interp, double a, double b, double c, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *cp = cast(carray_empty());
      cp->sp = RangeConstructor(a,b,c,false);
      update_cache(cp);
      return cp;
    }
  catch (Exception &e)
    {
      *flag = true;
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      return NULL;
    }
}

extern "C" EXPORT
bool carray_any(void *interp, void *p, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *pp = cast(p);
      return !RealAllZeros(pp->sp);
    }
  catch (Exception &e)
    {
      *flag = true;
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      return false;
    }
}

extern "C" EXPORT
bool carray_all(void *interp, void *p, bool *flag)
{
  if (*flag) return NULL;
  try
    {
      CArray *pp = cast(p);
      return RealAllNonZeros(pp->sp);
    }
  catch (Exception &e)
    {
      *flag = true;
      reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg());
      return false;
    }
}

#define WrapUnaryOp(wrapped,func)					\
  extern "C" EXPORT                                                     \
  void* wrapped(void* interp, void *a, bool *flag)			\
  {									\
    if (*flag) return NULL;						\
    try									\
      {									\
	CArray *ap = cast(a);						\
	CArray *cp = cast(carray_empty());				\
	cp->sp = func(ap->sp);						\
	update_cache(cp);						\
	return cp;							\
      }									\
    catch (Exception&e)							\
      {									\
	reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg()); \
	*flag = true;							\
	return NULL;							\
      }									\
  }

WrapUnaryOp(carray_neg,Negate);
WrapUnaryOp(carray_pos,Plus);
WrapUnaryOp(carray_not,Not);
WrapUnaryOp(carray_transpose,Hermitian);
WrapUnaryOp(carray_dottranspose,Transpose);

#define WrapOp(wrapped,func)						\
  extern "C" EXPORT								\
  void* wrapped(void* interp, void *a, void *b, bool *flag)		\
  {									\
    if (*flag) return NULL;						\
    try									\
      {									\
	CArray *ap = cast(a);						\
	CArray *bp = cast(b);						\
	CArray *cp = cast(carray_empty());				\
	cp->sp = func(ap->sp,bp->sp);					\
	update_cache(cp);						\
	return cp;							\
      }									\
    catch (Exception &e)						\
      {									\
	*flag = true;							\
	reinterpret_cast<Interpreter*>(interp)->setLastErrorString(e.msg()); \
	return NULL;							\
      }									\
  }

Array HCat(const Array& A, const Array& B)
{
  ArrayVector p;
  p.push_back(A);
  p.push_back(B);
  return NCat(p,1);
}

Array VCat(const Array& A, const Array& B)
{
  ArrayVector p;
  p.push_back(A);
  p.push_back(B);
  return NCat(p,0);
}

WrapOp(carray_hcat,HCat);
WrapOp(carray_vcat,VCat);
WrapOp(carray_add,Add);
WrapOp(carray_pow,Power);
WrapOp(carray_dpow,DotPower);
WrapOp(carray_sub,Subtract);
WrapOp(carray_times,Multiply);
WrapOp(carray_dtimes,DotMultiply);
WrapOp(carray_rdiv,RightDivide);
WrapOp(carray_drdiv,DotRightDivide);
WrapOp(carray_ldiv,LeftDivide);
WrapOp(carray_dldiv,DotLeftDivide);
WrapOp(carray_or,Or);
WrapOp(carray_and,And);
WrapOp(carray_lt,LessThan);
WrapOp(carray_le,LessEquals);
WrapOp(carray_gt,GreaterThan);
WrapOp(carray_ge,GreaterEquals);
WrapOp(carray_eq,Equals);
WrapOp(carray_neq,NotEquals);

