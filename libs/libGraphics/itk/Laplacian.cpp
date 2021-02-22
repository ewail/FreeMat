#include "ITKWrap.hpp"
#include <itkLaplacianImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TLaplacianFilter(const ArrayVector &arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::LaplacianImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction laplacianfilter LaplacianFilterFunction
//input x
//output y
ArrayVector LaplacianFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCasesFloatOnly(TLaplacianFilter,arg);
}
