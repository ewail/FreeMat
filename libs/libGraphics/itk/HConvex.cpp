#include "ITKWrap.hpp"
#include <itkHConvexImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array HConvexFunction(const ArrayVector &arg)
{
  if (arg.size() == 0) return Array();
  if (arg.size() < 2) throw Exception("must supply image and height");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::HConvexImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->SetHeight(arg[1].asDouble());
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction hconvex HConvexFilterFunction
//input x height
//output y
ArrayVector HConvexFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(HConvexFunction,arg);
}
