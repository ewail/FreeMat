#include "ITKWrap.hpp"
#include <itkBinomialBlurImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array BinomialBlurFunction(const ArrayVector &arg)
{
  if (arg.size() == 0) return Array();
  int repcount = 1;
  if (arg.size() >= 2) repcount = arg[1].asInteger();
  if (repcount < 0) throw Exception("Invalid repcount");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BinomialBlurImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->SetRepetitions(repcount);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction binomialblur BinomialBlurFilterFunction
//input x repcount
//output y
ArrayVector BinomialBlurFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(BinomialBlurFunction,arg);
}
