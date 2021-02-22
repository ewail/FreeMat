#include "ITKWrap.hpp"
#include <itkGrayscaleGrindPeakImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array GrayscaleGrindPeakFilter(const ArrayVector & arg)
{
  if (arg.size() == 0) throw Exception("Grayscale grind peak requires 2 arguments: image fully connected");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::GrayscaleGrindPeakImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  if (arg.size() > 1)
    filter->SetFullyConnected(arg[1].asInteger());
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction grayscalegrindpeak GrayscaleGrindPeakFunction
//input x fullyconnected
//output y
ArrayVector GrayscaleGrindPeakFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(GrayscaleGrindPeakFilter,arg);
}
