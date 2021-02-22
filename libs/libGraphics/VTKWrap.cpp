#ifdef HAVE_VTK

#include "VTKWrap.hpp"
#include "Class.hpp"

Array MakeVTKPointer(vtkObjectBase *p, QString cname, Interpreter *eval, 
		     bool nodelete)
{
  if (!p) throw Exception("Function returned an empty VTK pointer!");
  StructArray *sap = new StructArray;
  sap->insert("pointer",Array(reinterpret_cast<uint64>(p)));
  sap->insert("nodelete",Array(nodelete));
  sap->setClassName(cname);
  // Make it a handle-semantics class
  StructArray hsap(sap,eval);
  return Array(hsap);
}

void DeleteVTKObject(const Array &arg) {
  StructArray sa = arg.constStructPtr();
  BasicArray<Array>& val = sa["nodelete"];
  if (val[1].constRealScalar<bool>()) return;
  vtkObjectBase *vtk_pointer = GetVTKPointer<vtkObjectBase>(arg);
  vtk_pointer->Delete();
}

bool VTKIsScalarArg(const Array &g)
{
  return (!g.isReferenceType() && g.isScalar());
}

bool VTKIsVectorArg(const Array &g)
{
  return (!g.isReferenceType() && g.isVector());
}

bool VTKIsClass(const Array &g, QString cname)
{
  return ((g.dataClass() == Struct) && 
	  ((g.className() == cname) ||
	   isParentClass(cname,g.className())));
}

#endif
