#ifndef __JITFuncBase_hpp__
#define __JITFuncBase_hpp__

#include "Interpreter.hpp"

// Abstract base class for JIT implementations
class JITFuncBase
{
public:
  virtual bool compile(const Tree &t, JITControlFlag flag) = 0;
  virtual int run() = 0;
};

#endif
