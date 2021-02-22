#ifndef __CJitFuncClang_hpp__
#define __CJitFuncClang_hpp__

#include <stdint.h>
#include <string>

#include "llvm/Function.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/LLVMContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "CJitFunc.hpp"

class CJitFuncClang : public JITFuncBase
{
protected:
  llvm::LLVMContext *ctxt;
  llvm::ExecutionEngine* EE;
  clang::CompilerInstance* comp;
  llvm::Function* func;
  bool compile(const std::string &filename,
	       const std::string &funcname);
  Interpreter* m_eval;
  JITControlFlag m_flag;
public:
  CJitFuncClang(Interpreter* eval);
  ~CJitFuncClang();
  bool compile(const Tree & t, JITControlFlag flag);
  int run();
};

#endif
