#include "ITKWrap.hpp"
#include <itkLaplacianSharpeningImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array LaplacianSharpening(const ArrayVector & arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::LaplacianSharpeningImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction laplaciansharpening LaplacianSharpeningFunction
//input x 
//output y
ArrayVector LaplacianSharpeningFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(LaplacianSharpening,arg);
}
