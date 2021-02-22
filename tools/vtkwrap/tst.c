#include "VTKWrap.hpp"

// FreeMat wrapper for vtkDataArray object
//
#define VTK_STREAMS_FWD_ONLY
#include "vtkDataArray.h"

//@@Signature
//gfunction @vtkDataArray:vtkDataArray vtkDataArrayConstructorFunction
//input a
//output p
ArrayVector vtkDataArrayConstructorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) {
    Array ret(MakeVTKPointer(vtkDataArray::New()));
    ret.structPtr().setClassPath(StringVector() << "vtkDataArray");
    return ret;
  } else if (arg[0].className() == "vtkDataArray") {
    return arg[0];
  } else {
    vtkObjectBase *p = GetVTKPointer<vtkObjectBase>(arg[0]);
    vtkDataArray*q = dynamic_cast<vtkDataArray*>(p);
    if (!q)
      throw Exception("Unable to type convert supplied object to an instance of type vtkDataArray");
    Array ret(arg[0]);
    ret.structPtr().setClassPath(StringVector() << "vtkDataArray");
    return ret;
  }
}
//@@Signature
//gfunction @vtkDataArray:GetClassName vtkDataArrayGetClassNameFunction
//input a0 
//output y
ArrayVector vtkDataArrayGetClassNameFunction(int nargout, const ArrayVector& arg) {
  /* Signature const char *GetClassName (); */
  if (arg.size() < 1) 
    throw Exception("Function GetClassName for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  const char    *temp20;
  temp20 = (vtk_pointer)->GetClassName();
  retval = Array(QString(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:IsA vtkDataArrayIsAFunction
//input a0 a1 
//output y
ArrayVector vtkDataArrayIsAFunction(int nargout, const ArrayVector& arg) {
  /* Signature int IsA (const char *name); */
  if (arg.size() < 2) 
    throw Exception("Function IsA for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  char    *temp0;
  int      temp20;
  char tmp_string0[1024];
  strcpy(tmp_string0,qPrintable(arg[1].asString())); 
  temp0 = tmp_string0;
  temp20 = (vtk_pointer)->IsA(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:NewInstance vtkDataArrayNewInstanceFunction
//input a0 
//output y
ArrayVector vtkDataArrayNewInstanceFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkDataArray *NewInstance (); */
  if (arg.size() < 1) 
    throw Exception("Function NewInstance for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkDataArray  *temp20;
  temp20 = (vtk_pointer)->NewInstance();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SafeDownCast vtkDataArraySafeDownCastFunction
//input a0 a1 
//output y
ArrayVector vtkDataArraySafeDownCastFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkDataArray *SafeDownCast (vtkObject* o); */
  if (arg.size() < 2) 
    throw Exception("Function SafeDownCast for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkObject  *temp0;
  vtkDataArray  *temp20;
  temp0 = GetVTKPointer<vtkObject>(arg[1]);
  temp20 = (vtk_pointer)->SafeDownCast(temp0);
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:IsNumeric vtkDataArrayIsNumericFunction
//input a0 
//output y
ArrayVector vtkDataArrayIsNumericFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual int IsNumeric ();return  */
  if (arg.size() < 1) 
    throw Exception("Function IsNumeric for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->IsNumeric();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetElementComponentSize vtkDataArrayGetElementComponentSizeFunction
//input a0 
//output y
ArrayVector vtkDataArrayGetElementComponentSizeFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual int GetElementComponentSize ();return this GetDataTypeSize  */
  if (arg.size() < 1) 
    throw Exception("Function GetElementComponentSize for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetElementComponentSize();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple__1 vtkDataArraySetTuple__1Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArraySetTuple__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void SetTuple (vtkIdType i, vtkIdType j, vtkAbstractArray *source) = 0; */
  if (arg.size() < 4) 
    throw Exception("Function SetTuple__1 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdType   temp1;
  vtkAbstractArray  *temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp2 = GetVTKPointer<vtkAbstractArray>(arg[3]);
  vtk_pointer->SetTuple(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple__1 vtkDataArrayInsertTuple__1Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayInsertTuple__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InsertTuple (vtkIdType i, vtkIdType j, vtkAbstractArray *source) = 0; */
  if (arg.size() < 4) 
    throw Exception("Function InsertTuple__1 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdType   temp1;
  vtkAbstractArray  *temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp2 = GetVTKPointer<vtkAbstractArray>(arg[3]);
  vtk_pointer->InsertTuple(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple__1 vtkDataArrayInsertNextTuple__1Function
//input a0 a1 a2 
//output y
ArrayVector vtkDataArrayInsertNextTuple__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkIdType InsertNextTuple (vtkIdType j, vtkAbstractArray *source) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function InsertNextTuple__1 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkAbstractArray  *temp1;
  vtkIdType   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp1 = GetVTKPointer<vtkAbstractArray>(arg[2]);
  temp20 = (vtk_pointer)->InsertNextTuple(temp0,temp1);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuples__1 vtkDataArrayGetTuples__1Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayGetTuples__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void GetTuples (vtkIdList *ptIds, vtkAbstractArray *output); */
  if (arg.size() < 3) 
    throw Exception("Function GetTuples__1 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdList  *temp0;
  vtkAbstractArray  *temp1;
  temp0 = GetVTKPointer<vtkIdList>(arg[1]);
  temp1 = GetVTKPointer<vtkAbstractArray>(arg[2]);
  vtk_pointer->GetTuples(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuples__2 vtkDataArrayGetTuples__2Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayGetTuples__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void GetTuples (vtkIdType p1, vtkIdType p2, vtkAbstractArray *output); */
  if (arg.size() < 4) 
    throw Exception("Function GetTuples__2 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdType   temp1;
  vtkAbstractArray  *temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp2 = GetVTKPointer<vtkAbstractArray>(arg[3]);
  vtk_pointer->GetTuples(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InterpolateTuple__1 vtkDataArrayInterpolateTuple__1Function
//input a0 a1 a2 a3 a4 
//output none
ArrayVector vtkDataArrayInterpolateTuple__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InterpolateTuple (vtkIdType i, vtkIdList *ptIndices, vtkAbstractArray *source, double *weights); */
  if (arg.size() < 5) 
    throw Exception("Function InterpolateTuple__1 for class vtkDataArray requires 5 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdList  *temp1;
  vtkAbstractArray  *temp2;
  double  *temp3;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp1 = GetVTKPointer<vtkIdList>(arg[2]);
  temp2 = GetVTKPointer<vtkAbstractArray>(arg[3]);
    if ( !arg[4].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[4].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp3 = new double[length];
      for ( int k = 0; k < length; k++ ) temp3[k] = (double)data[k+1];
      }
  vtk_pointer->InterpolateTuple(temp0,temp1,temp2,temp3);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InterpolateTuple__2 vtkDataArrayInterpolateTuple__2Function
//input a0 a1 a2 a3 a4 a5 a6 
//output none
ArrayVector vtkDataArrayInterpolateTuple__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InterpolateTuple (vtkIdType i, vtkIdType id1, vtkAbstractArray *source1, vtkIdType id2, vtkAbstractArray *source2, double t); */
  if (arg.size() < 7) 
    throw Exception("Function InterpolateTuple__2 for class vtkDataArray requires 7 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdType   temp1;
  vtkAbstractArray  *temp2;
  vtkIdType   temp3;
  vtkAbstractArray  *temp4;
  double   temp5;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp2 = GetVTKPointer<vtkAbstractArray>(arg[3]);
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (int) arg[4].asDouble();
  temp4 = GetVTKPointer<vtkAbstractArray>(arg[5]);
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  vtk_pointer->InterpolateTuple(temp0,temp1,temp2,temp3,temp4,temp5);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple__2 vtkDataArrayGetTuple__2Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayGetTuple__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void GetTuple (vtkIdType i, double *tuple) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function GetTuple__2 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
    if ( !arg[2].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new double[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (double)data[k+1];
      }
  vtk_pointer->GetTuple(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple1 vtkDataArrayGetTuple1Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetTuple1Function(int nargout, const ArrayVector& arg) {
  /* Signature double GetTuple1 (vtkIdType i); */
  if (arg.size() < 2) 
    throw Exception("Function GetTuple1 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetTuple1(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple2 vtkDataArrayGetTuple2Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetTuple2Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetTuple2 (vtkIdType i); */
  if (arg.size() < 2) 
    throw Exception("Function GetTuple2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetTuple2(temp0);
  BasicArray<double> tempResult(NTuple(2,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple3 vtkDataArrayGetTuple3Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetTuple3Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetTuple3 (vtkIdType i); */
  if (arg.size() < 2) 
    throw Exception("Function GetTuple3 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetTuple3(temp0);
  BasicArray<double> tempResult(NTuple(3,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  tempResult[3] = (double)temp20[2];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple4 vtkDataArrayGetTuple4Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetTuple4Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetTuple4 (vtkIdType i); */
  if (arg.size() < 2) 
    throw Exception("Function GetTuple4 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetTuple4(temp0);
  BasicArray<double> tempResult(NTuple(4,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  tempResult[3] = (double)temp20[2];
  tempResult[4] = (double)temp20[3];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetTuple9 vtkDataArrayGetTuple9Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetTuple9Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetTuple9 (vtkIdType i); */
  if (arg.size() < 2) 
    throw Exception("Function GetTuple9 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetTuple9(temp0);
  BasicArray<double> tempResult(NTuple(9,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  tempResult[3] = (double)temp20[2];
  tempResult[4] = (double)temp20[3];
  tempResult[5] = (double)temp20[4];
  tempResult[6] = (double)temp20[5];
  tempResult[7] = (double)temp20[6];
  tempResult[8] = (double)temp20[7];
  tempResult[9] = (double)temp20[8];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple__2 vtkDataArraySetTuple__2Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArraySetTuple__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void SetTuple (vtkIdType i, const float *tuple) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function SetTuple__2 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  float   *temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
    if ( !arg[2].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new float[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (float)data[k+1];
      }
  vtk_pointer->SetTuple(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple__3 vtkDataArraySetTuple__3Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArraySetTuple__3Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void SetTuple (vtkIdType i, const double *tuple) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function SetTuple__3 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
    if ( !arg[2].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new double[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (double)data[k+1];
      }
  vtk_pointer->SetTuple(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple1 vtkDataArraySetTuple1Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArraySetTuple1Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetTuple1 (vtkIdType i, double value); */
  if (arg.size() < 3) 
    throw Exception("Function SetTuple1 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  vtk_pointer->SetTuple1(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple2 vtkDataArraySetTuple2Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArraySetTuple2Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetTuple2 (vtkIdType i, double val0, double val1); */
  if (arg.size() < 4) 
    throw Exception("Function SetTuple2 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->SetTuple2(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple3 vtkDataArraySetTuple3Function
//input a0 a1 a2 a3 a4 
//output none
ArrayVector vtkDataArraySetTuple3Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetTuple3 (vtkIdType i, double val0, double val1, double val2); */
  if (arg.size() < 5) 
    throw Exception("Function SetTuple3 for class vtkDataArray requires 5 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  vtk_pointer->SetTuple3(temp0,temp1,temp2,temp3);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple4 vtkDataArraySetTuple4Function
//input a0 a1 a2 a3 a4 a5 
//output none
ArrayVector vtkDataArraySetTuple4Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetTuple4 (vtkIdType i, double val0, double val1, double val2, double val3); */
  if (arg.size() < 6) 
    throw Exception("Function SetTuple4 for class vtkDataArray requires 6 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  vtk_pointer->SetTuple4(temp0,temp1,temp2,temp3,temp4);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple9 vtkDataArraySetTuple9Function
//input a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 
//output none
ArrayVector vtkDataArraySetTuple9Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetTuple9 (vtkIdType i, double val0, double val1, double val2, double val3, double val4, double val5, double val6, double val7, double val8); */
  if (arg.size() < 11) 
    throw Exception("Function SetTuple9 for class vtkDataArray requires 11 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  double   temp5;
  double   temp6;
  double   temp7;
  double   temp8;
  double   temp9;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  if ( !arg[7].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp6 = (double) arg[7].asDouble();
  if ( !arg[8].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp7 = (double) arg[8].asDouble();
  if ( !arg[9].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp8 = (double) arg[9].asDouble();
  if ( !arg[10].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp9 = (double) arg[10].asDouble();
  vtk_pointer->SetTuple9(temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple__2 vtkDataArrayInsertTuple__2Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayInsertTuple__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InsertTuple (vtkIdType i, const float *tuple) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function InsertTuple__2 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  float   *temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
    if ( !arg[2].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new float[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (float)data[k+1];
      }
  vtk_pointer->InsertTuple(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple__3 vtkDataArrayInsertTuple__3Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayInsertTuple__3Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InsertTuple (vtkIdType i, const double *tuple) = 0; */
  if (arg.size() < 3) 
    throw Exception("Function InsertTuple__3 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double  *temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
    if ( !arg[2].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new double[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (double)data[k+1];
      }
  vtk_pointer->InsertTuple(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple1 vtkDataArrayInsertTuple1Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayInsertTuple1Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertTuple1 (vtkIdType i, double value); */
  if (arg.size() < 3) 
    throw Exception("Function InsertTuple1 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  vtk_pointer->InsertTuple1(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple2 vtkDataArrayInsertTuple2Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayInsertTuple2Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertTuple2 (vtkIdType i, double val0, double val1); */
  if (arg.size() < 4) 
    throw Exception("Function InsertTuple2 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->InsertTuple2(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple3 vtkDataArrayInsertTuple3Function
//input a0 a1 a2 a3 a4 
//output none
ArrayVector vtkDataArrayInsertTuple3Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertTuple3 (vtkIdType i, double val0, double val1, double val2); */
  if (arg.size() < 5) 
    throw Exception("Function InsertTuple3 for class vtkDataArray requires 5 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  vtk_pointer->InsertTuple3(temp0,temp1,temp2,temp3);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple4 vtkDataArrayInsertTuple4Function
//input a0 a1 a2 a3 a4 a5 
//output none
ArrayVector vtkDataArrayInsertTuple4Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertTuple4 (vtkIdType i, double val0, double val1, double val2, double val3); */
  if (arg.size() < 6) 
    throw Exception("Function InsertTuple4 for class vtkDataArray requires 6 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  vtk_pointer->InsertTuple4(temp0,temp1,temp2,temp3,temp4);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple9 vtkDataArrayInsertTuple9Function
//input a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 
//output none
ArrayVector vtkDataArrayInsertTuple9Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertTuple9 (vtkIdType i, double val0, double val1, double val2, double val3, double val4, double val5, double val6, double val7, double val8); */
  if (arg.size() < 11) 
    throw Exception("Function InsertTuple9 for class vtkDataArray requires 11 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  double   temp5;
  double   temp6;
  double   temp7;
  double   temp8;
  double   temp9;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  if ( !arg[7].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp6 = (double) arg[7].asDouble();
  if ( !arg[8].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp7 = (double) arg[8].asDouble();
  if ( !arg[9].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp8 = (double) arg[9].asDouble();
  if ( !arg[10].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp9 = (double) arg[10].asDouble();
  vtk_pointer->InsertTuple9(temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple__2 vtkDataArrayInsertNextTuple__2Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayInsertNextTuple__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkIdType InsertNextTuple (const float *tuple) = 0; */
  if (arg.size() < 2) 
    throw Exception("Function InsertNextTuple__2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  float   *temp0;
  vtkIdType   temp20;
    if ( !arg[1].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[1].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp0 = new float[length];
      for ( int k = 0; k < length; k++ ) temp0[k] = (float)data[k+1];
      }
  temp20 = (vtk_pointer)->InsertNextTuple(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple__3 vtkDataArrayInsertNextTuple__3Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayInsertNextTuple__3Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkIdType InsertNextTuple (const double *tuple) = 0; */
  if (arg.size() < 2) 
    throw Exception("Function InsertNextTuple__3 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double  *temp0;
  vtkIdType   temp20;
    if ( !arg[1].isVector() )
       throw Exception("expecting a vector");
    else 
      {
      Array vect = arg[1].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp0 = new double[length];
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
  temp20 = (vtk_pointer)->InsertNextTuple(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple1 vtkDataArrayInsertNextTuple1Function
//input a0 a1 
//output none
ArrayVector vtkDataArrayInsertNextTuple1Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertNextTuple1 (double value); */
  if (arg.size() < 2) 
    throw Exception("Function InsertNextTuple1 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  vtk_pointer->InsertNextTuple1(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple2 vtkDataArrayInsertNextTuple2Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayInsertNextTuple2Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertNextTuple2 (double val0, double val1); */
  if (arg.size() < 3) 
    throw Exception("Function InsertNextTuple2 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  vtk_pointer->InsertNextTuple2(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple3 vtkDataArrayInsertNextTuple3Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayInsertNextTuple3Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertNextTuple3 (double val0, double val1, double val2); */
  if (arg.size() < 4) 
    throw Exception("Function InsertNextTuple3 for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->InsertNextTuple3(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple4 vtkDataArrayInsertNextTuple4Function
//input a0 a1 a2 a3 a4 
//output none
ArrayVector vtkDataArrayInsertNextTuple4Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertNextTuple4 (double val0, double val1, double val2, double val3); */
  if (arg.size() < 5) 
    throw Exception("Function InsertNextTuple4 for class vtkDataArray requires 5 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  vtk_pointer->InsertNextTuple4(temp0,temp1,temp2,temp3);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple9 vtkDataArrayInsertNextTuple9Function
//input a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 
//output none
ArrayVector vtkDataArrayInsertNextTuple9Function(int nargout, const ArrayVector& arg) {
  /* Signature void InsertNextTuple9 (double val0, double val1, double val2, double val3, double val4, double val5, double val6, double val7, double val8); */
  if (arg.size() < 10) 
    throw Exception("Function InsertNextTuple9 for class vtkDataArray requires 10 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  double   temp5;
  double   temp6;
  double   temp7;
  double   temp8;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  if ( !arg[7].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp6 = (double) arg[7].asDouble();
  if ( !arg[8].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp7 = (double) arg[8].asDouble();
  if ( !arg[9].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp8 = (double) arg[9].asDouble();
  vtk_pointer->InsertNextTuple9(temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:RemoveTuple vtkDataArrayRemoveTupleFunction
//input a0 a1 
//output none
ArrayVector vtkDataArrayRemoveTupleFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void RemoveTuple (vtkIdType id) = 0; */
  if (arg.size() < 2) 
    throw Exception("Function RemoveTuple for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->RemoveTuple(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:RemoveFirstTuple vtkDataArrayRemoveFirstTupleFunction
//input a0 
//output none
ArrayVector vtkDataArrayRemoveFirstTupleFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void RemoveFirstTuple () = 0; */
  if (arg.size() < 1) 
    throw Exception("Function RemoveFirstTuple for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtk_pointer->RemoveFirstTuple();
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:RemoveLastTuple vtkDataArrayRemoveLastTupleFunction
//input a0 
//output none
ArrayVector vtkDataArrayRemoveLastTupleFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void RemoveLastTuple () = 0; */
  if (arg.size() < 1) 
    throw Exception("Function RemoveLastTuple for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtk_pointer->RemoveLastTuple();
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetComponent vtkDataArrayGetComponentFunction
//input a0 a1 a2 
//output y
ArrayVector vtkDataArrayGetComponentFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual double GetComponent (vtkIdType i, int j); */
  if (arg.size() < 3) 
    throw Exception("Function GetComponent for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  int      temp1;
  double   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp20 = (vtk_pointer)->GetComponent(temp0,temp1);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetComponent vtkDataArraySetComponentFunction
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArraySetComponentFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void SetComponent (vtkIdType i, int j, double c); */
  if (arg.size() < 4) 
    throw Exception("Function SetComponent for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  int      temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->SetComponent(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:InsertComponent vtkDataArrayInsertComponentFunction
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayInsertComponentFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void InsertComponent (vtkIdType i, int j, double c); */
  if (arg.size() < 4) 
    throw Exception("Function InsertComponent for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  int      temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->InsertComponent(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetData vtkDataArrayGetDataFunction
//input a0 a1 a2 a3 a4 a5 
//output none
ArrayVector vtkDataArrayGetDataFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void GetData (vtkIdType tupleMin, vtkIdType tupleMax, int compMin, int compMax, vtkDoubleArray *data); */
  if (arg.size() < 6) 
    throw Exception("Function GetData for class vtkDataArray requires 6 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkIdType   temp0;
  vtkIdType   temp1;
  int      temp2;
  int      temp3;
  vtkDoubleArray  *temp4;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (int) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (int) arg[4].asDouble();
  temp4 = GetVTKPointer<vtkDoubleArray>(arg[5]);
  vtk_pointer->GetData(temp0,temp1,temp2,temp3,temp4);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:DeepCopy__1 vtkDataArrayDeepCopy__1Function
//input a0 a1 
//output none
ArrayVector vtkDataArrayDeepCopy__1Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void DeepCopy (vtkAbstractArray *aa); */
  if (arg.size() < 2) 
    throw Exception("Function DeepCopy__1 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkAbstractArray  *temp0;
  temp0 = GetVTKPointer<vtkAbstractArray>(arg[1]);
  vtk_pointer->DeepCopy(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:DeepCopy__2 vtkDataArrayDeepCopy__2Function
//input a0 a1 
//output none
ArrayVector vtkDataArrayDeepCopy__2Function(int nargout, const ArrayVector& arg) {
  /* Signature virtual void DeepCopy (vtkDataArray *da); */
  if (arg.size() < 2) 
    throw Exception("Function DeepCopy__2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkDataArray  *temp0;
  temp0 = GetVTKPointer<vtkDataArray>(arg[1]);
  vtk_pointer->DeepCopy(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:FillComponent vtkDataArrayFillComponentFunction
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayFillComponentFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void FillComponent (int j, double c); */
  if (arg.size() < 3) 
    throw Exception("Function FillComponent for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  double   temp1;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  vtk_pointer->FillComponent(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:CopyComponent vtkDataArrayCopyComponentFunction
//input a0 a1 a2 a3 
//output none
ArrayVector vtkDataArrayCopyComponentFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void CopyComponent (int j, vtkDataArray *from, int fromComponent); */
  if (arg.size() < 4) 
    throw Exception("Function CopyComponent for class vtkDataArray requires 4 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  vtkDataArray  *temp1;
  int      temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp1 = GetVTKPointer<vtkDataArray>(arg[2]);
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (int) arg[3].asDouble();
  vtk_pointer->CopyComponent(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetActualMemorySize vtkDataArrayGetActualMemorySizeFunction
//input a0 
//output y
ArrayVector vtkDataArrayGetActualMemorySizeFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual unsigned long GetActualMemorySize (); */
  if (arg.size() < 1) 
    throw Exception("Function GetActualMemorySize for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  unsigned long     temp20;
  temp20 = (vtk_pointer)->GetActualMemorySize();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:CreateDefaultLookupTable vtkDataArrayCreateDefaultLookupTableFunction
//input a0 
//output none
ArrayVector vtkDataArrayCreateDefaultLookupTableFunction(int nargout, const ArrayVector& arg) {
  /* Signature void CreateDefaultLookupTable (); */
  if (arg.size() < 1) 
    throw Exception("Function CreateDefaultLookupTable for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtk_pointer->CreateDefaultLookupTable();
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetLookupTable vtkDataArraySetLookupTableFunction
//input a0 a1 
//output none
ArrayVector vtkDataArraySetLookupTableFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetLookupTable (vtkLookupTable *lut); */
  if (arg.size() < 2) 
    throw Exception("Function SetLookupTable for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkLookupTable  *temp0;
  temp0 = GetVTKPointer<vtkLookupTable>(arg[1]);
  vtk_pointer->SetLookupTable(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetLookupTable vtkDataArrayGetLookupTableFunction
//input a0 
//output y
ArrayVector vtkDataArrayGetLookupTableFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkLookupTable *GetLookupTable (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLookupTable for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkLookupTable  *temp20;
  temp20 = (vtk_pointer)->GetLookupTable();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetRange__1 vtkDataArrayGetRange__1Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayGetRange__1Function(int nargout, const ArrayVector& arg) {
  /* Signature void GetRange (double range[2], int comp);this ComputeRange comp memcpy range this Range sizeof double  */
  if (arg.size() < 3) 
    throw Exception("Function GetRange__1 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double temp0[2];
  int      temp1;
  if ( !(arg[1].isVector() && arg[1].length() == 2) )
    throw Exception("Mismatch in vector lengths");
  {
    Array vect = arg[1].asDenseArray().toClass(Double);
    BasicArray<double> data = vect.real<double>();
    int length = data.length();
    for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  }
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[3].asDouble();
  vtk_pointer->GetRange(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetRange__2 vtkDataArrayGetRange__2Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetRange__2Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetRange (int comp);this ComputeRange comp return this Range  */
  if (arg.size() < 2) 
    throw Exception("Function GetRange__2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  double  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetRange(temp0);
  BasicArray<double> tempResult(NTuple(2,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetRange__3 vtkDataArrayGetRange__3Function
//input a0 
//output y
ArrayVector vtkDataArrayGetRange__3Function(int nargout, const ArrayVector& arg) {
  /* Signature double *GetRange ();this ComputeRange return this Range  */
  if (arg.size() < 1) 
    throw Exception("Function GetRange__3 for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double  *temp20;
  temp20 = (vtk_pointer)->GetRange();
  BasicArray<double> tempResult(NTuple(2,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetRange__4 vtkDataArrayGetRange__4Function
//input a0 a1 
//output none
ArrayVector vtkDataArrayGetRange__4Function(int nargout, const ArrayVector& arg) {
  /* Signature void GetRange (double range[2]);this GetRange range  */
  if (arg.size() < 2) 
    throw Exception("Function GetRange__4 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double temp0[2];
  if ( !(arg[1].isVector() && arg[1].length() == 2) )
    throw Exception("Mismatch in vector lengths");
  {
    Array vect = arg[1].asDenseArray().toClass(Double);
    BasicArray<double> data = vect.real<double>();
    int length = data.length();
    for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  }
  vtk_pointer->GetRange(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeRange__1 vtkDataArrayGetDataTypeRange__1Function
//input a0 a1 
//output none
ArrayVector vtkDataArrayGetDataTypeRange__1Function(int nargout, const ArrayVector& arg) {
  /* Signature void GetDataTypeRange (double range[2]); */
  if (arg.size() < 2) 
    throw Exception("Function GetDataTypeRange__1 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double temp0[2];
  if ( !(arg[1].isVector() && arg[1].length() == 2) )
    throw Exception("Mismatch in vector lengths");
  {
    Array vect = arg[1].asDenseArray().toClass(Double);
    BasicArray<double> data = vect.real<double>();
    int length = data.length();
    for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  }
  vtk_pointer->GetDataTypeRange(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMin__1 vtkDataArrayGetDataTypeMin__1Function
//input a0 
//output y
ArrayVector vtkDataArrayGetDataTypeMin__1Function(int nargout, const ArrayVector& arg) {
  /* Signature double GetDataTypeMin (); */
  if (arg.size() < 1) 
    throw Exception("Function GetDataTypeMin__1 for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetDataTypeMin();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMax__1 vtkDataArrayGetDataTypeMax__1Function
//input a0 
//output y
ArrayVector vtkDataArrayGetDataTypeMax__1Function(int nargout, const ArrayVector& arg) {
  /* Signature double GetDataTypeMax (); */
  if (arg.size() < 1) 
    throw Exception("Function GetDataTypeMax__1 for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetDataTypeMax();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeRange__2 vtkDataArrayGetDataTypeRange__2Function
//input a0 a1 a2 
//output none
ArrayVector vtkDataArrayGetDataTypeRange__2Function(int nargout, const ArrayVector& arg) {
  /* Signature static void GetDataTypeRange (int type, double range[2]); */
  if (arg.size() < 3) 
    throw Exception("Function GetDataTypeRange__2 for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  double temp1[2];
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !(arg[2].isVector() && arg[2].length() == 2) )
    throw Exception("Mismatch in vector lengths");
  {
    Array vect = arg[2].asDenseArray().toClass(Double);
    BasicArray<double> data = vect.real<double>();
    int length = data.length();
    for ( int k = 0; k < length; k++ ) temp1[k] = (double)data[k+1];
  }
  vtk_pointer->GetDataTypeRange(temp0,temp1);
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMin__2 vtkDataArrayGetDataTypeMin__2Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetDataTypeMin__2Function(int nargout, const ArrayVector& arg) {
  /* Signature static double GetDataTypeMin (int type); */
  if (arg.size() < 2) 
    throw Exception("Function GetDataTypeMin__2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  double   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetDataTypeMin(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMax__2 vtkDataArrayGetDataTypeMax__2Function
//input a0 a1 
//output y
ArrayVector vtkDataArrayGetDataTypeMax__2Function(int nargout, const ArrayVector& arg) {
  /* Signature static double GetDataTypeMax (int type); */
  if (arg.size() < 2) 
    throw Exception("Function GetDataTypeMax__2 for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  double   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->GetDataTypeMax(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:GetMaxNorm vtkDataArrayGetMaxNormFunction
//input a0 
//output y
ArrayVector vtkDataArrayGetMaxNormFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual double GetMaxNorm (); */
  if (arg.size() < 1) 
    throw Exception("Function GetMaxNorm for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetMaxNorm();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:CreateDataArray vtkDataArrayCreateDataArrayFunction
//input a0 a1 
//output y
ArrayVector vtkDataArrayCreateDataArrayFunction(int nargout, const ArrayVector& arg) {
  /* Signature static vtkDataArray *CreateDataArray (int dataType); */
  if (arg.size() < 2) 
    throw Exception("Function CreateDataArray for class vtkDataArray requires 2 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  int      temp0;
  vtkDataArray  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  temp20 = (vtk_pointer)->CreateDataArray(temp0);
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:PER_COMPONENT vtkDataArrayPER_COMPONENTFunction
//input a0 
//output y
ArrayVector vtkDataArrayPER_COMPONENTFunction(int nargout, const ArrayVector& arg) {
  /* Signature static vtkInformationInformationVectorKey *PER_COMPONENT (); */
  if (arg.size() < 1) 
    throw Exception("Function PER_COMPONENT for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkInformationInformationVectorKey  *temp20;
  temp20 = (vtk_pointer)->PER_COMPONENT();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:COMPONENT_RANGE vtkDataArrayCOMPONENT_RANGEFunction
//input a0 
//output y
ArrayVector vtkDataArrayCOMPONENT_RANGEFunction(int nargout, const ArrayVector& arg) {
  /* Signature static vtkInformationDoubleVectorKey *COMPONENT_RANGE (); */
  if (arg.size() < 1) 
    throw Exception("Function COMPONENT_RANGE for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkInformationDoubleVectorKey  *temp20;
  temp20 = (vtk_pointer)->COMPONENT_RANGE();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:L2_NORM_RANGE vtkDataArrayL2_NORM_RANGEFunction
//input a0 
//output y
ArrayVector vtkDataArrayL2_NORM_RANGEFunction(int nargout, const ArrayVector& arg) {
  /* Signature static vtkInformationDoubleVectorKey *L2_NORM_RANGE (); */
  if (arg.size() < 1) 
    throw Exception("Function L2_NORM_RANGE for class vtkDataArray requires 1 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkInformationDoubleVectorKey  *temp20;
  temp20 = (vtk_pointer)->L2_NORM_RANGE();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:CopyInformation vtkDataArrayCopyInformationFunction
//input a0 a1 a2 
//output y
ArrayVector vtkDataArrayCopyInformationFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual int CopyInformation (vtkInformation *infoFrom, int deep); */
  if (arg.size() < 3) 
    throw Exception("Function CopyInformation for class vtkDataArray requires 3 argument(s)");
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  Array retval;
  vtkInformation  *temp0;
  int      temp1;
  int      temp20;
  temp0 = GetVTKPointer<vtkInformation>(arg[1]);
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp20 = (vtk_pointer)->CopyInformation(temp0,temp1);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkDataArray:SetTuple vtkDataArraySetTupleFunction
//input varargin
//output varargout
ArrayVector vtkDataArraySetTupleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 4) return vtkDataArraySetTuple__1Function(nargout,arg);
  if (arg.size() == 3) return vtkDataArraySetTuple__2Function(nargout,arg);
  if (arg.size() == 3) return vtkDataArraySetTuple__3Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArraySetTuple");
}

//@@Signature
//gfunction @vtkDataArray:InsertTuple vtkDataArrayInsertTupleFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayInsertTupleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 4) return vtkDataArrayInsertTuple__1Function(nargout,arg);
  if (arg.size() == 3) return vtkDataArrayInsertTuple__2Function(nargout,arg);
  if (arg.size() == 3) return vtkDataArrayInsertTuple__3Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayInsertTuple");
}

//@@Signature
//gfunction @vtkDataArray:InsertNextTuple vtkDataArrayInsertNextTupleFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayInsertNextTupleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 3) return vtkDataArrayInsertNextTuple__1Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayInsertNextTuple__2Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayInsertNextTuple__3Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayInsertNextTuple");
}

//@@Signature
//gfunction @vtkDataArray:GetTuples vtkDataArrayGetTuplesFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetTuplesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 3) return vtkDataArrayGetTuples__1Function(nargout,arg);
  if (arg.size() == 4) return vtkDataArrayGetTuples__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetTuples");
}

//@@Signature
//gfunction @vtkDataArray:InterpolateTuple vtkDataArrayInterpolateTupleFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayInterpolateTupleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 5) return vtkDataArrayInterpolateTuple__1Function(nargout,arg);
  if (arg.size() == 7) return vtkDataArrayInterpolateTuple__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayInterpolateTuple");
}

//@@Signature
//gfunction @vtkDataArray:GetTuple vtkDataArrayGetTupleFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetTupleFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 3) return vtkDataArrayGetTuple__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetTuple");
}

//@@Signature
//gfunction @vtkDataArray:DeepCopy vtkDataArrayDeepCopyFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayDeepCopyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 2) return vtkDataArrayDeepCopy__1Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayDeepCopy__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayDeepCopy");
}

//@@Signature
//gfunction @vtkDataArray:GetRange vtkDataArrayGetRangeFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetRangeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 3) return vtkDataArrayGetRange__1Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayGetRange__2Function(nargout,arg);
  if (arg.size() == 1) return vtkDataArrayGetRange__3Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayGetRange__4Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetRange");
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeRange vtkDataArrayGetDataTypeRangeFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetDataTypeRangeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 2) return vtkDataArrayGetDataTypeRange__1Function(nargout,arg);
  if (arg.size() == 3) return vtkDataArrayGetDataTypeRange__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetDataTypeRange");
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMin vtkDataArrayGetDataTypeMinFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetDataTypeMinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkDataArrayGetDataTypeMin__1Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayGetDataTypeMin__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetDataTypeMin");
}

//@@Signature
//gfunction @vtkDataArray:GetDataTypeMax vtkDataArrayGetDataTypeMaxFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayGetDataTypeMaxFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkDataArrayGetDataTypeMax__1Function(nargout,arg);
  if (arg.size() == 2) return vtkDataArrayGetDataTypeMax__2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkDataArrayGetDataTypeMax");
}

//@@Signature
//sgfunction @vtkDataArray:display vtkDataArrayDisplayFunction
//input varargin
//output varargout
ArrayVector vtkDataArrayDisplayFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  if (arg[0].length() > 1) {
     PrintArrayClassic(arg[0],100,eval);
     return ArrayVector();
  }
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  std::ostringstream ss;
  vtkIndent ind;
  vtk_pointer->PrintSelf(ss,ind);
  eval->outputMessage(QString::fromStdString(ss.str()));
  return ArrayVector();
}
//@@Signature
//gfunction @vtkDataArray:subsref vtkDataArraySubsrefFunction
//input varargin
//output varargout
ArrayVector vtkDataArraySubsrefFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2) return ArrayVector();
  vtkDataArray* vtk_pointer = GetVTKPointer<vtkDataArray>(arg[0]);
  const StructArray& sa = arg[1].constStructPtr();
  const BasicArray<Array>& typea = sa["type"];
  const BasicArray<Array>& subsa = sa["subs"];
  if ((typea.length() == 1) && (typea[1].asString() == "."))
  {
    if (subsa[1].asString() == "ClassName")
      return vtkDataArrayGetClassNameFunction(nargout,arg);
    if (subsa[1].asString() == "ElementComponentSize")
      return vtkDataArrayGetElementComponentSizeFunction(nargout,arg);
    if (subsa[1].asString() == "ActualMemorySize")
      return vtkDataArrayGetActualMemorySizeFunction(nargout,arg);
    if (subsa[1].asString() == "LookupTable")
      return vtkDataArrayGetLookupTableFunction(nargout,arg);
    if (subsa[1].asString() == "Range")
      return vtkDataArrayGetRangeFunction(nargout,arg);
    if (subsa[1].asString() == "DataTypeMin")
      return vtkDataArrayGetDataTypeMinFunction(nargout,arg);
    if (subsa[1].asString() == "DataTypeMax")
      return vtkDataArrayGetDataTypeMaxFunction(nargout,arg);
    if (subsa[1].asString() == "MaxNorm")
      return vtkDataArrayGetMaxNormFunction(nargout,arg);
  }
  if ((typea.length() == 2) && (typea[1].asString() == ".")
      && (typea[2].asString() == "()"))
  {
    if (subsa[1].asString() == "Tuples")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuplesFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuples")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuplesFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTupleFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple1")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuple1Function(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple2")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuple2Function(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple3")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuple3Function(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple4")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuple4Function(nargout,tmp);
    }
    if (subsa[1].asString() == "Tuple9")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetTuple9Function(nargout,tmp);
    }
    if (subsa[1].asString() == "Component")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetComponentFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Data")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetDataFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Range")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetRangeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Range")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetRangeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Range")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetRangeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "DataTypeRange")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetDataTypeRangeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "DataTypeRange")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetDataTypeRangeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "DataTypeMin")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetDataTypeMinFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "DataTypeMax")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkDataArrayGetDataTypeMaxFunction(nargout,tmp);
    }
  }
}

