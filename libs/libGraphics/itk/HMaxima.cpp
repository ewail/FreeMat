#include "ITKWrap.hpp"
#include <itkHMaximaImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array HMaximaFunction(const ArrayVector &arg)
{
  if (arg.size() == 0) return Array();
  if (arg.size() < 2) throw Exception("must supply image and height");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::HMaximaImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->SetHeight(arg[1].asDouble());
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction hmaxima HMaximaFilterFunction
//input x height
//output y
ArrayVector HMaximaFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(HMaximaFunction,arg);
}
