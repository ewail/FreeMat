#include "ITKWrap.hpp"
#include <itkSimpleContourExtractorImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array SimpleContourExtractor(const ArrayVector & arg)
{
  if (arg.size() < 6) throw Exception("simplecontourextractor requires at least six arguments; image, radius, inputfg, inputbg, outputfg, outputbg");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::SimpleContourExtractorImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  typename ITKType::SizeType radius;
  if (arg[1].isScalar())
    for (int i=0;i<dims;i++)
      radius[i] = arg[1].asDouble();
  else
    for (int i=0;i<dims;i++)
      radius[i] = arg[1].get(i+1).asDouble();
  filter->SetRadius(radius);
  filter->SetInputForegroundValue(arg[2].asDouble());
  filter->SetInputBackgroundValue(arg[3].asDouble());
  filter->SetOutputForegroundValue(arg[4].asDouble());
  filter->SetOutputBackgroundValue(arg[5].asDouble());
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction simplecontourextractor SimpleContourExtractorFunction
//input x radius inputforeground inputbackground outputforeground outputbackground
//output y
ArrayVector SimpleContourExtractorFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(SimpleContourExtractor,arg);
}
