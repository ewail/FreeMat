#include "ITKWrap.hpp"
#include <itkGrayscaleFillholeImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array GrayscaleFillholeFilter(const ArrayVector & arg)
{
  if (arg.size() == 0) throw Exception("Grayscale fill hole requires 2 arguments: image fully connected");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::GrayscaleFillholeImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  if (arg.size() > 1)
    filter->SetFullyConnected(arg[1].asInteger());
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction grayscalefillhole GrayscaleFillholeFunction
//input x fullyconnected
//output y
ArrayVector GrayscaleFillholeFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(GrayscaleFillholeFilter,arg);
}
