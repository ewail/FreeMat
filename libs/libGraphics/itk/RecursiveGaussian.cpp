#include "ITKWrap.hpp"
#include <itkRecursiveGaussianImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array RecursiveGaussianFilter(const ArrayVector & arg)
{
  int direction = 0;
  double sigma = 1.0;
  int order = 0;
  bool normalizeflag = false;
  if (arg.size() >= 2) direction = arg[1].asInteger();
  if (arg.size() >= 3) sigma = arg[2].asDouble();
  if (arg.size() >= 4) order = arg[3].asInteger();
  if (order < 0 || order > 2) 
    throw Exception("Invalid order for convolution -- must be between 0 and 2 (inclusive)");
  if (direction < 1 || direction > dims)
    throw Exception("Invalid direction argument");
  if (arg.size() >= 5) normalizeflag = arg[4].asInteger();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::RecursiveGaussianImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  try
    {
      filter->SetInput(imageIn);
      filter->SetSigma(sigma);
      filter->SetDirection(direction-1);
      switch (order)
	{
	case 0: filter->SetOrder(FilterType::ZeroOrder); break;
	case 1: filter->SetOrder(FilterType::FirstOrder); break;
	case 2: filter->SetOrder(FilterType::SecondOrder); break;
	}
      filter->SetNormalizeAcrossScale(normalizeflag);
      filter->Update();
      return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
    } 
  CATCH_ITK;
}

// Add a "asBoolean" method to the Array class

//@@Signature
//gfunction recursivegaussian RecursiveGaussianFunction
//input x dimension sigma order normalizeflag 
//output y
ArrayVector RecursiveGaussianFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(RecursiveGaussianFilter,arg);
}
