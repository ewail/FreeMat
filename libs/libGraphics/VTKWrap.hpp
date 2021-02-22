#ifndef __VTKWrap_hpp__
#define __VTKWrap_hpp__

#ifdef HAVE_VTK

#include "Array.hpp"
#include "Struct.hpp"
#include "Interpreter.hpp"
#include "Algorithms.hpp"
#include "Print.hpp"
#include "vtkObjectBase.h"
#include <sstream>

template <class T>
inline T* GetVTKPointer(const Array &arg) {
  if (arg.dataClass() != Struct)
    throw Exception("Cannot convert non vtk-objects to vtk-objects");
  if (!arg.className().startsWith("vtk"))
    throw Exception("Cannot convert non vtk-objects to vtk-objects");
  if (!arg.isScalar())
    throw Exception("only scalar instances are supported currently");
  Array acopy(arg);
  StructArray sa = acopy.structPtr();
  int pindex = sa.fieldIndex("pointer");
  BasicArray<Array>& val = sa[pindex];
  return reinterpret_cast<T*>(val[1].constRealScalar<uint64>());
}

Array MakeVTKPointer(vtkObjectBase *p, QString cname, Interpreter *eval, bool nodelete = false);

void DeleteVTKObject(const Array &arg);

bool VTKIsScalarArg(const Array &g);

bool VTKIsVectorArg(const Array &g);

bool VTKIsClass(const Array &g, QString cname);

#endif

#endif

