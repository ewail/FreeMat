#include "ITKWrap.hpp"
#include <itkSmoothingRecursiveGaussianImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array SmoothingRecursiveGaussian(const ArrayVector & arg)
{
  if (arg.size() < 2) throw Exception("smoothing recursive gaussian filter requires a sigma value or vector");
  bool normalizeflag = false;
  if (arg.size() > 3) normalizeflag = arg[2].asInteger();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::SmoothingRecursiveGaussianImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  if (arg[1].isScalar())
    {
      filter->SetSigma(arg[1].asDouble());
    }
  else
    {
      typename FilterType::SigmaArrayType sigma;
      for (int i=0;i<dims;i++)
	sigma[i] = arg[1].get(i+1).asDouble();
      filter->SetSigmaArray(sigma);
    }
  filter->SetNormalizeAcrossScale(normalizeflag);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction smoothingrecursivegaussian SmoothingRecursiveGaussianFunction
//input x sigma normalizeflag 
//output y
ArrayVector SmoothingRecursiveGaussianFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(SmoothingRecursiveGaussian,arg);
}
