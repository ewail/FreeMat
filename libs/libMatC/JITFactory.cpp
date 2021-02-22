#include "JITFactory.hpp"
#include "CJitFuncClang.hpp"

JITFuncBase* JITFactory::GetJITFunc(Interpreter *eval)
{
#ifdef HAVE_LLVM
  return new CJitFuncClang(eval);
#else
  return NULL;
#endif
}
