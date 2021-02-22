#ifndef __JITFactory_hpp__
#define __JITFactory_hpp__

#include "JITFuncBase.hpp"

class JITFactory
{
public:
  static JITFuncBase* GetJITFunc(Interpreter *eval);
};

#endif
