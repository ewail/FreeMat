#include "ITKWrap.hpp"
#include <itkSobelEdgeDetectionImageFilter.h>


template <int dims, DataClass dataclass, class PixelClass>
static Array SobelEdgeDetectorFilter(const ArrayVector & arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::SobelEdgeDetectionImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction sobeledgedetector SobelEdgeDetectorFunction
//input x
//output y
ArrayVector SobelEdgeDetectorFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(SobelEdgeDetectorFilter,arg);
}
