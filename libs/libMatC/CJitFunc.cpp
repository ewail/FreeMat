#include "CJitFunc.hpp"
#include "CArray.hpp"
#include "CScalarFuncs.hpp"
#include "Array.hpp"
#include "Interpreter.hpp"
#include <iostream>
#include <fstream>

// abs() is not supported at the moment
// Need to bridge to scalar and array functions!!
// Best option is to create scalar versions of functions that
// are meaningful to inline/accelerate (like add), and then
// have bridges to other functions.

// Why can't the JIT operate via a proxy?

// TODO
// Add or, and, lt, le, gt, ge, eq, neq - done
// Add neg, pos, not, power, dotpower - done
// Add transpose - done
// Add while loops - done
// Calling FM Functions - done
// Using colon in gets as well as sets - done
// Add A(:,4)
// Add Matrix definitions - done

// What happens with something like this:
// A(i) = func(x)
// One can assume that func must be a scalar valued
// function.  One option is to assume that if
// a function has all scalar arguments that the
// result is scalar valued.  This seems generally
// true, but not always.  On the other hand,
// if we assume that the expression is well formed
// then func(x) _must_ be a scalar.  

// For performance reasons, it seems necessary to
// special case scalar functions for a bunch of
// operations.  It would be best to distribute these
// operations throughout the code base instead of
// e.g., duplicating them here.

// The last step is the JIT-ing of functions.  
// 
// This could be tricky.  The simplest way is to 
// inline the functions, but this can be really
// expensive.  
//
// One option is to 

class RescanDisabler
{
  Interpreter *m_eval;
  bool m_flag;
public:
  RescanDisabler(Interpreter *eval) : m_eval(eval)
  {
    m_flag = m_eval->getDisableRescan();
    m_eval->setDisableRescan(true);
  }
  ~RescanDisabler() 
  {
    m_eval->setDisableRescan(m_flag);
  }
};


std::string MapCTypeToC(CType t)
{
  switch (t)
    {
    case CDOUBLE: return "double";
    case CFLOAT: return "float";
    case CINT32: return "int32_t";
    case CBOOL: return "bool";
    default:
      throw Exception("Unsupported type encountered");
    }
}

std::string MapCTypeToC(CTypeInfo t)
{
  if (t.isscalar())
    return MapCTypeToC(t.type());
  else
    return "JITArray<" + MapCTypeToC(t.type()) + ">";
}

int MapCTypeToDC(CType t)
{
  switch (t)
    {
    case CDOUBLE: return CArray_Double;
    case CFLOAT: return CArray_Float;
    case CBOOL: return CArray_Bool;
      throw Exception("Unsupported type encountered");
    }
}

void CWriter::AddPrereq(std::string txt)
{
  prereq += txt;
}

void CWriter::FuncSig(std::string sig)
{
  q << "extern \"C\" " << sig << "\n";
  q << "{\n";
}

void CWriter::Declare(CSymbol sym) {
  if (!sym.isFunction())
    {
      if (sym.isscalar())
	q << "  " << MapCTypeToC(sym.typeinfo().type()) << " " << sym.name() << " = 0;\n";
      else
	q << "  JITArray<" << MapCTypeToC(sym.typeinfo().type()) << "> " << sym.name() << ";\n";
    }
  else
    {
      q << "  void* _" << sym.name() << ";\n";
    }
}

void CWriter::Download(CSymbol sym) {
  if (sym.isFunction())
    {
      if (sym.isused())
	{
	  q << "  if (!carray_download_function(interp,&_" 
	    << sym.name() << ",\"" << sym.name() 
	    << "\")) return CJIT_Prepfail;\n";
	}
    }
  else
    {
      if (!sym.istemporary())
	{
	  if (sym.isscalar())
	    q << "  if (!carray_download_scalar(interp,&" << sym.name() 
	      << ",\"" << sym.name() 
	      << "\"," << MapCTypeToDC(sym.typeinfo().type()) 
	      << ")) return CJIT_Prepfail;\n";
	  else
	    q << "  if (!carray_download_array(interp," << sym.name() << ".data(),\"" << sym.name() 
	      << "\"," << MapCTypeToDC(sym.typeinfo().type()) 
	      << ")) return CJIT_Prepfail;\n";
	}
    }
}

void CWriter::Upload(CSymbol sym) {
  if (!sym.istemporary() && !sym.isFunction())
    {
      if (sym.isscalar())
	o << "  if (!carray_upload_scalar(interp,&" << sym.name() << ",\"" << sym.name() << "\"," 
	  << MapCTypeToDC(sym.typeinfo().type()) << ")) return CJIT_Runfail;\n";
      else
	o << "  if (!carray_upload_array(interp," << sym.name() << ".data(),\"" << sym.name() 
	  << "\")) return CJIT_Runfail;\n";
    }
}

void CWriter::Indent() {for (int i=0;i<m_indent;i++) o << "  "; }
void CWriter::Operator(std::string op) { o << op; }
void CWriter::BeginWhile() { o << "while (All(interp,";}
void CWriter::EndWhile() {o << "))";}
void CWriter::BeginParen() { o << "("; }
void CWriter::EndParen() { o << ")"; }
void CWriter::BoolValue(bool t) {o << "((bool)" << t << ")";}
void CWriter::FloatValue(float t) {o << "((float)" << t << ")";}
void CWriter::DoubleValue(double t) {o << "((double)" << t << ")";}
void CWriter::BeginScope() {Indent(); o << "{\n"; m_indent++; }
void CWriter::BeginMemberFunction(CSymbol name, std::string member) {o << name.name() << "." << member << "(";}
void CWriter::EndMemberFunction() {o << ")";}
void CWriter::BeginAssign(CSymbol varname) { Indent(); o << "if (!Set(interp," << varname.name() << ","; }
void CWriter::EndAssign() { o << ")) return CJIT_Runfail;\n"; }
void CWriter::BeginFuncCall(std::string funcname) {o << funcname << "(";}
void CWriter::AddErrorCheck() {o << ",&_errorflag";}
void CWriter::BlockErrorCheck() {o << "  if (_errorflag) return CJIT_Runfail;\n";}
void CWriter::LookupVariable(CSymbol varname) {o << varname.name();}
void CWriter::NextArg() {o << ",";}
void CWriter::EndFuncCall() {o << ")";}
CSymbol CWriter::BeginLoop(CSymbol loopvar, CSymbol loopcount) { 
  Indent();
  o << "for (double " << loopvar.name() << "=0;" << loopvar.name() << "<" ;
  o<< loopcount.name() << ";" << loopvar.name() << "++)\n"; 
  BeginScope();
  return loopvar;
}
void CWriter::BeginIf() {Indent(); o << "if (All(interp,";}
void CWriter::EndIf() {o << "))\n";}
void CWriter::BeginElseIf() {Indent(); o << "else if (All(interp,";};
void CWriter::EndElseIf() {o << "))\n";}
void CWriter::BeginElse() {Indent(); o << "else\n";}
void CWriter::EndElse() {}
void CWriter::EndLoop() { EndScope(); }

void CWriter::EndScope() {m_indent--; Indent(); o << "}\n";}

void CWriter::CloseStatement() { o << ";\n";}

void CWriter::Dump() {
  //  qDebug() << QString::fromStdString(q.str() + "\n" + o.str() + "\n");
}

void CWriter::WriteCode(std::string filename) 
{
  std::ofstream fp(filename.c_str());
  fp << GetCode();
  fp.close();
}

std::string CWriter::GetInternalCode()
{
  std::stringstream p;
  p << prereq << "\n" << q.str() + "\n" + o.str() + "\n";
  return p.str();
}

std::string CWriter::GetCode()
{
  return "#include \"CArray.hpp\"\n\nstatic bool _errorflag = false;\n" 
    + GetInternalCode();
}

void CWriter::OperatorStatement(std::string keyword)
{
  Indent(); Operator(keyword); CloseStatement();
}

void CWriter::EmitString(std::string txt)
{
  o << txt;
}

void CWriter::Push()
{
  //  qDebug() << "Push: " << QString::fromStdString(o.str());
  stack.push_back(o.str());
}

void CWriter::Pop()
{
  //  o.str(stack.back());
  o.str("");
  o << stack.back();
  //  qDebug() << "Pop: " << QString::fromStdString(o.str());
  stack.pop_back();
}

CSymbol CJitFunc::declare_temp_variable(CTypeInfo type) {
  std::stringstream p;
  p << "_var" << m_tmp++;
  CSymbol s(p.str(),type);
  m_symbols.push_back(s);
  return s;
}

CSymbol CJitFunc::declare_variable(CTypeInfo type, std::string name) {
  CSymbol s(name,type);
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name) throw Exception("How do we handle this one?  Reuse of an existing variable as an index variable");
  m_symbols.push_back(s);
  return s;
}

void CJitFunc::reclass_scalar_to_array(std::string name) {
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name)
      m_symbols[i].setIsScalar(false);
}

void CJitFunc::compile_assignment(const Tree & t) {
  const Tree & sym(t.first());
  QString symname(sym.first().text());
  CSymbol csym = lookup_symbol(symname, true /* create if Not Defined */);
  cs.BeginAssign(csym);
  CTypeInfo rhs(compile_expression(t.second()));
  if (csym.isscalar() && (rhs.type() != csym.typeinfo().type())) 
    throw Exception("polymorphic assignment to variable detected");
  if (csym.typeinfo().type() == CUNKNOWN)
    {
      cast_symbol(symname.toStdString(),rhs);
      csym = lookup_symbol(symname, false);
    }
  if (sym.numChildren() == 1) {
    if (!rhs.isscalar() && csym.isscalar()) 
      {
	reclass_scalar_to_array(symname.toStdString());
	csym = lookup_symbol(symname, false);
      }
  } else if (sym.numChildren() > 2) {
    throw Exception("multiple levels of dereference not handled yet...");
  } else {
    if (csym.isscalar()) 
      {
	reclass_scalar_to_array(symname.toStdString());
	csym = lookup_symbol(symname, false);
      }
    const Tree & q(sym.second());
    if (!q.is(TOK_PARENS))
      throw Exception("non parenthetical dereferences not handled yet...");
    if (q.numChildren() == 0)
      throw Exception("Expecting at least 1 array reference for dereference...");
    if (q.numChildren() > 2)
      throw Exception("Expecting at most 2 array references for dereference...");
    cs.NextArg();
    CTypeInfo arg1 = compile_expression(q.first());
    if (q.numChildren() == 2) {
      cs.NextArg();
      CTypeInfo arg2 = compile_expression(q.second());
    }
  }  
  cs.EndAssign();
}

CTypeInfo CJitFunc::compile_unop(const Tree & t, std::string op) {
  cs.Operator(op);
  cs.BeginParen(); 
  cs.EmitString("interp,");
  CTypeInfo ret = compile_expression(t);
  cs.AddErrorCheck();
  cs.EndParen();
  return ret;
}

CTypeInfo CJitFunc::compile_unop_logical(const Tree & t, std::string op) {
  cs.Operator(op);
  cs.BeginParen();
  cs.EmitString("interp,");
  CTypeInfo ret = compile_expression(t);
  cs.AddErrorCheck();
  cs.EndParen();
  return CTypeInfo(CBOOL,ret.isscalar());
}

void CJitFunc::compile_if_statement(const Tree & t) {
  cs.BeginIf();
  compile_expression(t.first());
  cs.AddErrorCheck();
  cs.EndIf();
  cs.BeginScope();
  compile_block(t.second());
  cs.EndScope();
  int n=2;
  while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
    cs.BeginElseIf();
    compile_expression(t.child(n).first());
    cs.AddErrorCheck();
    cs.EndElseIf();
    cs.BeginScope();
    compile_block(t.child(n).second());
    cs.EndScope();
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    cs.BeginElse();
    cs.EndElse();
    cs.BeginScope();
    compile_block(t.last().first());
    cs.EndScope();
  }
}

CTypeInfo CJitFunc::compile_binop(const Tree & t_first,
				  const Tree & t_second, 
				  std::string op) {
  cs.BeginFuncCall(op);
  cs.Operator("interp");
  cs.NextArg();
  CTypeInfo A = compile_expression(t_first);
  cs.NextArg();
  CTypeInfo B = compile_expression(t_second);
  cs.AddErrorCheck();
  cs.EndParen();
  bool scalar = A.isscalar() && B.isscalar();
  if (A.type() == CFLOAT || B.type() == CFLOAT)
    return CTypeInfo(CFLOAT,scalar);
  return CTypeInfo(CDOUBLE,scalar);
}

CTypeInfo CJitFunc::compile_binop_logical(const Tree & t_first,
					  const Tree & t_second, 
					  std::string op) {
  cs.BeginFuncCall(op);
  cs.Operator("interp");
  cs.NextArg();
  CTypeInfo A = compile_expression(t_first);
  cs.NextArg();
  CTypeInfo B = compile_expression(t_second);
  cs.AddErrorCheck();
  cs.EndParen();
  bool scalar = A.isscalar() && B.isscalar();
  return CTypeInfo(CBOOL,scalar);
}

CType MapDataClassToCType(DataClass t)
{
  switch (t)
    {
    case Double: return CDOUBLE;
    case Float: return CFLOAT;
    case Int32: return CINT32;
    case Bool: return CBOOL;
    default:
      throw Exception("Unsupported type encountered");
    }
}

bool CJitFunc::exists_symbol(std::string name)
{
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name) return true;
  return false;
}

void CJitFunc::cast_symbol(std::string name, CTypeInfo type)
{
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name)
      {
	m_symbols[i].settypeinfo(type);
	return;
      }
}

void CJitFunc::use_function(std::string name)
{
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name)
      {
	m_symbols[i].use();
	return;
      }
}

CSymbol CJitFunc::lookup_symbol(QString name, bool createIfNotDefined)
{
  for (int i=0;i<m_symbols.size();i++)
    if (m_symbols[i].name() == name.toStdString()) return m_symbols[i];
  ArrayReference ptr;
  if (!m_functionmode)
    ptr = m_eval->getContext()->lookupVariable(name);
  if (!ptr.valid())
    {
      // Could be a function.  Look for a function with this name
      FuncPtr fptr;
      if (m_eval->lookupFunction(name,fptr))
	{
	  fptr->updateCode(m_eval);
	  CSymbol res(name.toStdString(),CTypeInfo(CUNKNOWN,false),true /*is function */);
	  res.setargcount(fptr->inputArgCount());
	  res.setretcount(fptr->outputArgCount());
	  res.setjitsafeflag(fptr->jitsafe);
	  res.setmfunctionflag((fptr->type() == FM_M_FUNCTION) &&
			       (!((MFunctionDef*)(fptr))->scriptFlag));
	  m_symbols.push_back(res);
	  return res;
	}
    }
  if (!ptr.valid() && !createIfNotDefined)
    throw Exception("Unable to find definition for symbol "+name);
  if (!ptr.valid() && createIfNotDefined)
    {
      CSymbol res(name.toStdString(),CTypeInfo(CUNKNOWN,false));
      m_symbols.push_back(res);
      return res;
    }
  if (!ptr->is2D())
    throw Exception("Cannot JIT multidimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  if (ptr->isSparse())
    throw Exception("Cannot JIT with sparse array:" + name);
  CTypeInfo typ(MapDataClassToCType(ptr->dataClass()),ptr->isScalar());
  CSymbol res(name.toStdString(),typ);
  m_symbols.push_back(res);
  return res;
}

CTypeInfo CJitFunc::compile_inline_function(QString symname, MFunctionDef *mptr, 
					    std::vector<CTypeInfo> argtypes) 
{
  // Functions must contain all of their variables (i.e., they form
  // a scope).
  RescanDisabler dis(m_eval);
  m_functionmode = true;
  if (argtypes.size() > mptr->arguments.size())
    throw Exception("Cannot JIT calls with more arguments than expected");
  for (int i=0;i<argtypes.size();i++)
    m_symbols.push_back(CSymbol(mptr->arguments[i].toStdString(),argtypes[i]));
  compile_block(mptr->code);
  cs.EmitString("  return " + mptr->returnVals[0].toStdString() + ";\n");
  cs.EndScope();
  CSymbol ret = lookup_symbol(mptr->returnVals[0], false /*createIfNotDefined*/);
  std::stringstream p;
  p << MapCTypeToC(ret.typeinfo()) << " " << symname.toStdString() << "(void *interp";
  for (int i=0;i<argtypes.size();i++)
    {
      p << ", ";
      p << MapCTypeToC(argtypes[i]) << " " << mptr->arguments[i].toStdString();
    }
  p << ",bool*)";
  cs.FuncSig(p.str());
  for (int i=0;i<m_symbols.size();i++)
    if (std::count(mptr->arguments.begin(),
		   mptr->arguments.end(),QString::fromStdString(m_symbols[i].name())) == 0)
      cs.Declare(m_symbols[i]);
  return ret.typeinfo();
}

bool CJitFunc::compile_mfunction(QString symname, std::vector<CTypeInfo> argtypes, CTypeInfo &ret) {
  try 
    {
      CJitFunc cfunc;
      cfunc.set_interpreter(m_eval);
      FuncPtr fptr;
      if (!m_eval->lookupFunction(symname,fptr)) return false;
      MFunctionDef *mptr = (MFunctionDef*) (fptr);
      if (m_prereqs.count(mptr)) return true;
      m_prereqs.insert(mptr);
      ret = cfunc.compile_inline_function(symname,mptr,argtypes);
      cs.AddPrereq(cfunc.GetInternalCode());
      return true;
    }
  catch (Exception &e)
    {
      return false;
    }
  return false;
}

static bool isjitscalarfunc(QString name)
{
  std::string sname = name.toStdString();
  for (int i=0;i<jit_scalar_list_len;i++)
    if (std::string(jit_scalar_list[i]) == sname) return true;
  return false;
}

CTypeInfo CJitFunc::compile_function_call(const Tree & t) {
  QString symname(t.first().text());
  CSymbol symbol = lookup_symbol(symname,false);
  if (!symbol.isFunction()) 
    throw Exception("function call compile called without a function...");
  if (!symbol.isMFunction() && !symbol.isJITsafe())
    throw Exception("Call to function " + symname + " is not JIT-safe");
  if (symbol.retcount() < 1)
    throw Exception("function must return 1 value to be JIT compiled");
  // Check for the case of an MFunction
  if (symbol.isMFunction() && !isjitscalarfunc(symname))
    {
      std::vector<CTypeInfo> argtypes;
      if (t.numChildren() > 1)
	{
	  const Tree &q(t.second());
	  cs.Push();
	  for (int i=0;i<q.numChildren();i++)
	    {
	      cs.NextArg();
	      argtypes.push_back(compile_expression(q.child(i)));
	    }
	  cs.Pop();
	}
      CTypeInfo rettype;
      if (compile_mfunction(symname,argtypes,rettype))
	{
	  cs.BeginFuncCall(symname.toStdString());
	  cs.Operator("interp");
	  if (t.numChildren() > 1)
	    {
	      const Tree &q(t.second());
	      for (int i=0;i<q.numChildren();i++)
		{
		  cs.NextArg();
		  compile_expression(q.child(i));
		}
	    }
	  cs.AddErrorCheck();
	  cs.EndFuncCall();
	  return rettype;
	}
    }
  // We want to find the special case of a JIT special case scalar function
  // with scalar arguments.  To do so requires that we scan through
  // the arguments to determine if they are all scalars.  In that
  // case, we can emit special code.
  bool scalarSpecialCase = false;
  if (isjitscalarfunc(symname))
    {
      cs.Push();
      scalarSpecialCase = true;
      if (t.numChildren() > 1)
	{
	  const Tree &q(t.second());
	  if (!q.is(TOK_PARENS))
	    throw Exception("Function calls!");
	  for (int i=0;i<q.numChildren();i++)
	    {
	      cs.NextArg();
	      CTypeInfo p = compile_expression(q.child(i));
	      if (p.type() != CDOUBLE)
		throw Exception("function calls only support double type arguments in JIT");
	      scalarSpecialCase = scalarSpecialCase && p.isscalar();
	    }
	}
      cs.Pop();
      if (scalarSpecialCase)
	{
	  cs.BeginFuncCall("_jit_" + symname.toStdString());
	  if (t.numChildren() > 1)
	    {
	      const Tree &q(t.second());
	      for (int i=0;i<q.numChildren();i++)
		{
		  if (i != 0) cs.NextArg();
		  compile_expression(q.child(i));
		}
	    }
	  cs.EndFuncCall();
	  return CTypeInfo(CDOUBLE,true);
	}
    }
  cs.BeginFuncCall("Invoke");
  cs.Operator("interp");
  cs.NextArg();
  use_function(symname.toStdString());
  cs.Operator("_"+symname.toStdString());
  CTypeInfo p;
  // At the moment, there is no type information for functions.
  // But there should be.  So in this incarnation, we cowardly
  // refuse to handle any function invokation that isn't all
  // double arguments.
  if (t.numChildren() > 1)
    {
      const Tree &q(t.second());
      if (!q.is(TOK_PARENS))
	throw Exception("Function calls!");
      for (int i=0;i<q.numChildren();i++)
	{
	  cs.NextArg();
	  p = compile_expression(q.child(i));
	  if (p.type() != CDOUBLE)
	    throw Exception("function calls only support double type arguments in JIT");
	}
    }
  cs.AddErrorCheck();  
  cs.EndFuncCall();
  return CTypeInfo(CDOUBLE,false);
}

CTypeInfo CJitFunc::compile_rhs(const Tree &t) {
  QString symname(t.first().text());
  CSymbol symbol = lookup_symbol(symname,false /* must exist */);
  if (symbol.isFunction()) return compile_function_call(t);
  if (t.numChildren() == 1) 
    {
      cs.LookupVariable(symbol);
      return symbol.typeinfo();
    }
  if (t.numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (symbol.isscalar())
    throw Exception("array indexing of scalar values...");
  const Tree & s(t.second());
  cs.BeginFuncCall("Get");
  cs.EmitString("interp,");
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (symbol.isscalar()) 
    throw Exception("Array indexing into a scalar is not JIT compiled");
  cs.LookupVariable(symbol);
  cs.NextArg();
  bool rettype_scalar = true;
  CTypeInfo arg1 = compile_expression(s.first());
  rettype_scalar = rettype_scalar && arg1.isscalar();
  if (s.numChildren() == 2)
    {
      cs.NextArg();
      CTypeInfo arg2 = compile_expression(s.second());
      rettype_scalar = rettype_scalar && arg2.isscalar();
    }
  cs.AddErrorCheck();
  cs.EndFuncCall();
  CTypeInfo rettype = symbol.typeinfo();
  rettype.setIsScalar(rettype_scalar);
  return rettype;
}

CTypeInfo CJitFunc::compile_colon(const Tree & t) {
  cs.BeginFuncCall("Colon");
  cs.Operator("interp");
  cs.NextArg();
  CTypeInfo arg1 = compile_expression(t.first());
  cs.NextArg();
  CTypeInfo arg2 = compile_expression(t.second());
  cs.AddErrorCheck();
  cs.EndFuncCall();
  if (!arg1.isscalar() || !arg2.isscalar())
    throw Exception("Arguments to colon operator must be scalars");
  if (arg1.type() != CDOUBLE || arg2.type() != CDOUBLE)
    throw Exception("Arguments to colon operator must be double precision");
  return CTypeInfo(CDOUBLE,false);  
}

CTypeInfo CJitFunc::compile_dcolon(const Tree & t) {
  cs.BeginFuncCall("DoubleColon");
  cs.Operator("interp");
  cs.NextArg();
  CTypeInfo arg1 = compile_expression(t.first().first());
  cs.NextArg();
  CTypeInfo arg2 = compile_expression(t.first().second());
  cs.NextArg();
  CTypeInfo arg3 = compile_expression(t.second());
  cs.AddErrorCheck();
  cs.EndFuncCall();
  if (!arg1.isscalar() || !arg2.isscalar() || !arg3.isscalar())
    throw Exception("Arguments to colon operator must be scalars");
  if (arg1.type() != CDOUBLE || arg2.type() != CDOUBLE || arg3.type() != CDOUBLE)
    throw Exception("Arguments to colon operator must be double precision");
  return CTypeInfo(CDOUBLE,false);
}

CTypeInfo CJitFunc::compile_rowdef(const Tree & t) {
  int N = t.numChildren();
  if (N == 1)
    {
      compile_expression(t.child(0));
      return CTypeInfo(CDOUBLE,false);
    }
  for (int i=0;i<N-1;i++)
    {
      cs.BeginFuncCall("HCat");
      cs.Operator("interp");
      cs.NextArg();
    }
  compile_expression(t.child(0));
  for (int i=0;i<N-1;i++)
    {
      cs.NextArg();
      compile_expression(t.child(i+1));
      cs.AddErrorCheck();      
      cs.EndFuncCall();
    }  
  return CTypeInfo(CDOUBLE,false);
}

CTypeInfo CJitFunc::compile_matdef(const Tree & t) {
  int N = t.numChildren();
  if (N == 0)
    {
      cs.BeginFuncCall("JITArray<double>");
      cs.EndFuncCall();
      return CTypeInfo(CDOUBLE,false);
    }
  for (int i=0;i<N-1;i++)
    {
      cs.BeginFuncCall("VCat");
      cs.Operator("interp");
      cs.NextArg();
    }
  compile_rowdef(t.child(0));
  for (int i=0;i<N-1;i++)
    {
      cs.NextArg();
      compile_rowdef(t.child(i+1));
      cs.AddErrorCheck();
      cs.EndFuncCall();
    }
  return CTypeInfo(CDOUBLE,false);
}

CTypeInfo CJitFunc::compile_expression(const Tree & t) {
  switch(t.token()) {
  case TOK_VARIABLE:     
    return compile_rhs(t);
  case TOK_REAL:
  case TOK_REALF:
    if( t.array().isScalar() ){
      switch( t.array().dataClass() ){
      case Bool:
	cs.BoolValue( t.array().constRealScalar<bool>() );
	return CTypeInfo(CBOOL,true);
      case Float:
	cs.FloatValue( t.array().constRealScalar<float>() );
	return CTypeInfo(CFLOAT,true);
      case Double:
	cs.DoubleValue( t.array().constRealScalar<double>() );
	return CTypeInfo(CDOUBLE,true);
      default:
	throw Exception("Unsupported scalar type.");
      }
    }
    else
      throw Exception("Unsupported type.");
  case TOK_MATDEF:
    return compile_matdef(t);
  case TOK_STRING:
  case TOK_END:
  case TOK_CELLDEF:      
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case ':': 
    if (t.numChildren() == 0)
      throw Exception("Unhandled colon case");
    else if (t.first().is(':'))
      return compile_dcolon(t);
    else
      return compile_colon(t);
  case '+': return compile_binop(t.first(),t.second(),"Add");
  case '-': return compile_binop(t.first(),t.second(),"Sub");
  case '*': return compile_binop(t.first(),t.second(),"Times");
  case TOK_DOTTIMES: return compile_binop(t.first(),t.second(),"DotTimes");
  case '/': return compile_binop(t.first(),t.second(),"RightDivide");
  case TOK_DOTRDIV: return compile_binop(t.first(),t.second(),"DotRightDivide");
  case '\\': return compile_binop(t.first(),t.second(),"LeftDivide");
  case TOK_DOTLDIV: return compile_binop(t.first(),t.second(),"DotLeftDivide");
  case TOK_SOR:
    {
      cs.BeginParen();
      cs.BeginParen();
      CTypeInfo A = compile_expression(t.first());
      cs.EndParen();
      cs.Operator("||");
      cs.BeginParen();
      CTypeInfo B = compile_expression(t.second());
      cs.EndParen();
      cs.EndParen();
      if (!A.isscalar() || !B.isscalar()) throw Exception("Use | instead of || for array arguments");
      return CTypeInfo(CBOOL,true);
    }
  case '|': return compile_binop_logical(t.first(),t.second(),"or");
  case TOK_SAND: 
    {
      cs.BeginParen();
      cs.BeginParen();
      CTypeInfo A = compile_expression(t.first());
      cs.EndParen();
      cs.Operator("&&");
      cs.BeginParen();
      CTypeInfo B = compile_expression(t.second());
      cs.EndParen();
      cs.EndParen();
      if (!A.isscalar() || !B.isscalar()) throw Exception("Use & instead of && for array arguments");
      return CTypeInfo(CBOOL,true);
    }
  case '&': return compile_binop_logical(t.first(),t.second(),"And");
  case '<': return compile_binop_logical(t.first(),t.second(),"LT");
  case TOK_LE: return compile_binop_logical(t.first(),t.second(),"LE");
  case '>': return compile_binop_logical(t.first(),t.second(),"GT");
  case TOK_GE: return compile_binop_logical(t.first(),t.second(),"GE");
  case TOK_EQ: return compile_binop_logical(t.first(),t.second(),"EQ");
  case TOK_NE: return compile_binop_logical(t.first(),t.second(),"NEQ");
  case TOK_UNARY_MINUS: return compile_unop(t.first(),"Neg");
  case TOK_UNARY_PLUS: return compile_unop(t.first(),"Pos");
  case '~': return compile_unop_logical(t.first(),"Not");
  case '^': 
    throw Exception("^ is not currently handled by the JIT compiler");
  case TOK_DOTPOWER: 
    throw Exception(".^ is not currently handled by the JIT compiler");
  case '\'': return compile_unop(t.first(),"Transpose");
  case TOK_DOTTRANSPOSE: return compile_unop(t.first(),"DotTranspose");
  case '@':               
    throw Exception("@ is not currently handled by the JIT compiler");
  default:                throw Exception("Unrecognized expression!");
  }  
}

#define TAssignExpression(var,root)					\
  {									\
    cs.BeginAssign(var);						\
    CTypeInfo p = compile_expression(root);				\
    if (!p.isscalar()) throw Exception("Loop arguments must be scalars."); \
    cs.EndAssign();							\
  }

void CJitFunc::compile_while_block(const Tree & t)
{
  cs.BeginWhile();
  compile_expression(t.first());
  cs.AddErrorCheck();
  cs.EndWhile();
  cs.BeginScope();
  compile_block(t.second());
  cs.EndScope();
}

void CJitFunc::compile_for_block(const Tree & t)
{
  cs.BeginScope();
  if (!(t.first().is('=') && t.first().second().is(':'))) 
    throw Exception("For loop cannot be compiled - need scalar bounds");
  CSymbol loop_var = declare_temp_variable(CTypeInfo(CDOUBLE,true));
  CSymbol loop_count = declare_temp_variable(CTypeInfo(CDOUBLE,true));
  CSymbol loop_start = declare_temp_variable(CTypeInfo(CDOUBLE,true));
  CSymbol loop_step = declare_temp_variable(CTypeInfo(CDOUBLE,true));
  CSymbol loop_stop = declare_temp_variable(CTypeInfo(CDOUBLE,true));
  CSymbol index_var = declare_variable(CTypeInfo(CDOUBLE,true),t.first().first().text().toStdString());
  if (t.first().second().first().is(':')){ //triple format 
    TAssignExpression(loop_start,t.first().second().first().first());
    TAssignExpression(loop_step,t.first().second().first().second());
    TAssignExpression(loop_stop,t.first().second().second());
  } else {
    TAssignExpression(loop_start,t.first().second().first());
    cs.BeginAssign(loop_step);
    cs.DoubleValue(1);
    cs.EndAssign();
    TAssignExpression(loop_stop,t.first().second().second());
  }
  cs.BeginAssign(loop_count);
  cs.BeginFuncCall("num_for_loop_iter");
  cs.LookupVariable(loop_start);
  cs.NextArg();
  cs.LookupVariable(loop_step);
  cs.NextArg();
  cs.LookupVariable(loop_stop);
  cs.EndFuncCall();
  cs.EndAssign();
  cs.BeginLoop(loop_var,loop_count);
  cs.BeginAssign(index_var);
  cs.LookupVariable(loop_start);
  cs.Operator("+");
  cs.LookupVariable(loop_var);
  cs.Operator("*");
  cs.LookupVariable(loop_step);
  cs.EndAssign();
  compile_block(t.second());
  cs.EndLoop();
  cs.EndScope();
}


void CJitFunc::compile_block(const Tree & t) 
{
  const TreeList &statements(t.children());
  for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++) 
    compile_statement(*i);  
  cs.BlockErrorCheck();
}

void CJitFunc::compile_statement(const Tree & t) {
  if (t.is(TOK_STATEMENT) && 
      (t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
       t.first().is(TOK_MULTI) || t.first().is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t.first());
}

void CJitFunc::compile_statement_type(const Tree & t) {
  switch(t.token()) {
  case '=': 
    compile_assignment(t);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    compile_for_block(t);
    break;
  case TOK_WHILE:
    compile_while_block(t);
    break;
  case TOK_IF:
    compile_if_statement(t);
    break;
  case TOK_BREAK:
    cs.OperatorStatement("break");
    break;
  case TOK_CONTINUE:
    cs.OperatorStatement("continue");
    break;
  case TOK_DBSTEP:      throw Exception("dbstep is not currently handled by the JIT compiler");
  case TOK_DBTRACE:     throw Exception("dbtrace is not currently handled by the JIT compiler");
  case TOK_RETURN:      throw Exception("return is not currently handled by the JIT compiler");
  case TOK_SWITCH:      throw Exception("switch is not currently handled by the JIT compiler");
  case TOK_TRY:         throw Exception("try is not currently handled by the JIT compiler");
  case TOK_QUIT:        throw Exception("quit is not currently handled by the JIT compiler");
  case TOK_RETALL:      throw Exception("retall is not currently handled by the JIT compiler");
  case TOK_KEYBOARD:    throw Exception("keyboard is not currently handled by the JIT compiler");
  case TOK_GLOBAL:      throw Exception("global is not currently handled by the JIT compiler");
  case TOK_PERSISTENT:  throw Exception("persistent is not currently handled by the JIT compiler");
  case TOK_EXPR:
    compile_expression(t.first());
    cs.CloseStatement();
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void CJitFunc::compile_tree(const Tree & t, std::string funcname)
{
  RescanDisabler dis(m_eval);
  cs.FuncSig(" int " + funcname + "(void* interp)");
  cs.EmitString("  _errorflag = false;\n");
  switch (t.token())
    {
    case TOK_FOR:
      compile_for_block(t);
      break;
    case TOK_WHILE:
      compile_while_block(t);
      break;
    default:
      compile_block(t);
      break;
    }
  for (int i=0;i<m_symbols.size();i++)
    cs.Declare(m_symbols[i]);
  for (int i=0;i<m_symbols.size();i++)
    cs.Download(m_symbols[i]);
  for (int i=0;i<m_symbols.size();i++)
    cs.Upload(m_symbols[i]);
  cs.EmitString("  return CJIT_Success;\n");
  cs.EndScope();
}

void CJitFunc::writeCode(std::string filename)
{
  cs.WriteCode(filename);
}

std::string CJitFunc::GetCode()
{
  return cs.GetCode();
}

std::string CJitFunc::GetInternalCode()
{
  return cs.GetInternalCode();
}
