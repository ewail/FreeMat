#include "ITKWrap.hpp"
#include <itkAdaptiveHistogramEqualizationImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array AdaptiveHistogramEqualizationFilter(const ArrayVector &arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::AdaptiveHistogramEqualizationImageFilter<ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  if (arg.size() > 1) filter->SetAlpha(arg[1].asDouble());
  if (arg.size() > 2) filter->SetBeta(arg[2].asDouble());
  if (arg.size() > 3) 
    {
      if (arg[3].isScalar())
	{
	  typename ITKType::SizeType radius;
	  for (int i=0;i<dims;i++)
	    radius[i] = arg[3].asDouble();
	  filter->SetRadius(radius);
	}
      else
	{
	  typename ITKType::SizeType radius;
	  for (int i=0;i<dims;i++)
	    radius[i] = arg[3].get(i+1).asDouble();
	  filter->SetRadius(radius);
	}
    }
  if (arg.size() > 4) filter->SetUseLookupTable(arg[4].asInteger() != 0);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());  
}

//@@Signature
//gfunction adaptivehistogramequalization AdaptiveHistogramEqualizationFunction
//input x alpha beta radius uselookup
//output y
ArrayVector AdaptiveHistogramEqualizationFunction(int nargout, const ArrayVector &arg)
{
  ITKCases(AdaptiveHistogramEqualizationFilter,arg);
}
