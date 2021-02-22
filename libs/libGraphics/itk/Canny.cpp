#include "ITKWrap.hpp"
#include <itkCannyEdgeDetectionImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array CannyEdgeDetectorFilter(const ArrayVector & arg)
{
  if (arg.size() < 4) throw Exception("Canny edge detector filter require 4 arguments: image, variance, maxerror, threshold (outsidevalue is optional)");
  double variance = arg[1].asDouble();
  double maxerror = arg[2].asDouble();
  double threshold = arg[3].asDouble();
  double outsidevalue = 0;
  if (arg.size() > 4) outsidevalue = arg[4].asDouble();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::CannyEdgeDetectionImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetVariance(variance);
  filter->SetMaximumError(maxerror);
  filter->SetUpperThreshold(threshold);
  filter->SetOutsideValue(outsidevalue);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction cannyedgedetector CannyEdgeDetectorFunction
//input x variance maxerror threshold outsidevalue
//output y
ArrayVector CannyEdgeDetectorFunction(int nargout, const ArrayVector& arg)
{
  ITKCasesFloatOnly(CannyEdgeDetectorFilter,arg);
}
