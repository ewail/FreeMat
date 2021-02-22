#include "ITKWrap.hpp"
#include <itkRelabelComponentImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TRelabelComponentsFunction(Array A, int minsize)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(A);
  typedef itk::RelabelComponentImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->SetMinimumObjectSize(minsize);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction relabelcomponents RelabelComponentsFunction
//input x minsize
//output y
ArrayVector RelabelComponentsFunction(int nargout, const ArrayVector& arg)
{
  int minsize = 0;
  if (arg.size() >= 2) minsize = arg[1].asInteger();
  if (arg[0].is2D())
    return TRelabelComponentsFunction<2,Int32,int32>(arg[0],minsize);
  else
    return TRelabelComponentsFunction<3,Int32,int32>(arg[0],minsize);
}
