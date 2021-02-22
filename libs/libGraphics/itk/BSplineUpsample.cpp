#include "ITKWrap.hpp"
#include <itkBSplineUpsampleImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array BSplineUpsampleImage(const ArrayVector &arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BSplineUpsampleImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction bsplineupsample BSplineUpsampleFunction
//input x
//output y
ArrayVector BSplineUpsampleFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(BSplineUpsampleImage,arg);
}
