#include "ITKWrap.hpp"
#include <itkLaplacianRecursiveGaussianImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array LaplacianRecursiveGaussian(const ArrayVector & arg)
{
  if (arg.size() < 2) throw Exception("laplacian recursive gaussian filter requires a sigma value or vector");
  bool normalizeflag = false;
  if (arg.size() > 3) normalizeflag = arg[2].asInteger();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::LaplacianRecursiveGaussianImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->SetSigma(arg[1].asDouble());
  filter->SetNormalizeAcrossScale(normalizeflag);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction laplacianrecursivegaussian LaplacianRecursiveGaussianFunction
//input x sigma normalizeflag 
//output y
ArrayVector LaplacianRecursiveGaussianFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(LaplacianRecursiveGaussian,arg);
}
