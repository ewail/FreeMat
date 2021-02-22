#ifndef __CJitFunc_hpp__
#define __CJitFunc_hpp__

#include "JITFuncBase.hpp"
#include "Tree.hpp"
#include <string>
#include <sstream>
#include <set>

typedef enum _CType {
  CUNKNOWN,
  CBOOL,
  CDOUBLE,
  CFLOAT,
  CINT32
} CType;

class CTypeInfo {
  CType m_type;
  bool m_isscalar;
public:
  CTypeInfo(CType type = CDOUBLE, bool isscalar = false) : m_type(type), m_isscalar(isscalar) {}
  CType type() const {return m_type;}
  void setType(CType type) {m_type = type;}
  bool isscalar() const {return m_isscalar;}
  void setIsScalar(bool flag) {m_isscalar = flag;}
};

class CSymbol {
  std::string m_name;
  CTypeInfo m_type;
  bool m_isFunction;
  bool m_isMFunction;
  bool m_isJITsafe;
  int m_argcount;
  int m_retcount;
  int m_usecount;
public:
  CSymbol(std::string name, CTypeInfo type, bool isFunction = false) :
    m_name(name), m_type(type), m_isFunction(isFunction), m_usecount(0) {}
  std::string name() {return m_name;}
  CTypeInfo typeinfo() {return m_type;}
  void settypeinfo(CTypeInfo t) {m_type = t;}
  bool isscalar() {return m_type.isscalar();}
  void setIsScalar(bool flag) {m_type.setIsScalar(flag);}
  bool istemporary() {return m_name[0] == '_';}
  bool isFunction() {return m_isFunction;}
  bool isMFunction() {return m_isMFunction;}
  void setargcount(int argcount) {m_argcount = argcount;}
  int argcount() {return m_argcount;}
  void setretcount(int retcount) {m_retcount = retcount;}
  int retcount() {return m_retcount;}
  void setmfunctionflag(bool flag) {m_isMFunction = flag;}
  bool isJITsafe() {return m_isJITsafe;}
  void setjitsafeflag(bool flag) {m_isJITsafe = flag;}
  void use() {m_usecount++;}
  bool isused() {return (m_usecount>0);}
};

class CWriter {
  std::vector<std::string> stack;
  std::stringstream o;
  std::stringstream q;
  std::string prereq;
  int m_indent;
public:
  CWriter() : m_indent(1) {}
  void FuncSig(std::string sig);
  void Declare(CSymbol sym);
  void Download(CSymbol sym);
  void Upload(CSymbol sym);
  void Operator(std::string op);
  void BeginWhile();
  void EndWhile();
  void BeginParen();
  void EndParen();
  void BoolValue(bool t);
  void FloatValue(float t);
  void DoubleValue(double t);
  void BeginScope();
  void Indent();
  void BeginAssign(CSymbol varname);
  void EndAssign();
  void BeginFuncCall(std::string funcname);
  void LookupVariable(CSymbol varname);
  void NextArg();
  void EndFuncCall();
  CSymbol BeginLoop(CSymbol loopvar, CSymbol loopcount);
  void EndLoop();
  void EndScope();
  void CloseStatement();
  void Dump();
  void BeginMemberFunction(CSymbol name, std::string member);
  void EndMemberFunction();
  void BeginIf();
  void EndIf();
  void BeginElseIf();
  void EndElseIf();
  void BeginElse();
  void EndElse();
  void OperatorStatement(std::string keyword);
  void EmitString(std::string txt);
  void BeginFunction(std::string funcname, std::string rettype);
  void WriteCode(std::string filename);
  std::string GetCode();
  std::string GetInternalCode();
  void Push();
  void Pop();
  void AddPrereq(std::string txt);
  void AddErrorCheck();
  void BlockErrorCheck();
};

// This is the abstract base class for JIT functions.  It contains
// the code necessary to build a C++ representation of the FreeMat
// function, but cannot execute it directly.  In order to make it 
// actually do something, one must inherit from  CJitFunc and 
// reimplement the run method.

class CJitFunc {
protected:
  Interpreter *m_eval;
  CWriter cs;
  int m_tmp;
  bool m_functionmode;
  std::vector<CSymbol> m_symbols;
  std::set<MFunctionDef*> m_prereqs;
  bool exists_symbol(std::string name);
  void cast_symbol(std::string name, CTypeInfo type);
  void use_function(std::string name);
  CSymbol declare_temp_variable(CTypeInfo type);
  CSymbol declare_variable(CTypeInfo type, std::string name);
  void compile_assignment(const Tree & t);
  CTypeInfo compile_unop(const Tree & t, std::string op);
  CTypeInfo compile_unop_logical(const Tree & t, std::string op);
  CTypeInfo compile_binop(const Tree & t_first, 
			  const Tree & t_second, std::string op);
  CTypeInfo compile_binop_logical(const Tree & t_first, 
			  const Tree & t_second, std::string op);
  CTypeInfo compile_rhs(const Tree &t);
  CTypeInfo compile_function_call(const Tree &t);
  CTypeInfo compile_expression(const Tree & t);
  CTypeInfo compile_rowdef(const Tree & t);
  CTypeInfo compile_matdef(const Tree & t);
  void compile_for_block(const Tree & t);
  void compile_while_block(const Tree & t);
  void compile_block(const Tree & t);
  void compile_if_statement(const Tree & t);
  void compile_statement(const Tree & t);
  void compile_statement_type(const Tree & t);
  CTypeInfo compile_dcolon(const Tree & t);
  CTypeInfo compile_colon(const Tree & t);
  CSymbol lookup_symbol(QString name, bool createIfNotDefined);
  void reclass_scalar_to_array(std::string name);
  bool compile_mfunction(QString symname, std::vector<CTypeInfo> argtypes, CTypeInfo &ret);
  CTypeInfo compile_inline_function(QString symname, MFunctionDef *mptr, std::vector<CTypeInfo> argtypes);
public:
  CJitFunc() : m_eval(0), m_tmp(0), m_functionmode(false) {}
  void set_interpreter(Interpreter* eval) {m_eval = eval;}
  virtual ~CJitFunc() {}
  void compile_tree(const Tree & t, std::string funcname);
  void writeCode(std::string filename);
  std::string GetCode();
  std::string GetInternalCode();
};

#endif
