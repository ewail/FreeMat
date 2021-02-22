#include "ITKWrap.hpp"
#include <itkThresholdLabelerImageFilter.h>


template <int dims, DataClass dataclass, class PixelClass>
static Array ThresholdLabelerFilter(const ArrayVector & arg)
{
  if (arg.size() < 2) throw Exception("thresholdlabel requires two arguments: image, thresholds");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::ThresholdLabelerImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  typename FilterType::ThresholdVector thresh;
  for (int i=0;i<arg[1].length();i++)
    thresh.push_back(arg[1].get(i+1).asDouble());
  filter->SetThresholds(thresh);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction thresholdlabel ThresholdLabelFunction
//input x thresholds
//output y
ArrayVector ThresholdLabelFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(ThresholdLabelerFilter,arg);
}
