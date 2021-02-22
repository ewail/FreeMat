#include "ITKWrap.hpp"
#include <itkMeanImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array MeanFilter(const ArrayVector & arg)
{
  if (arg.size() < 2) throw Exception("median filter requires at least two arguments");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::MeanImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  typename ITKType::SizeType radius;
  if (arg[1].isScalar())
    for (int i=0;i<dims;i++)
      radius[i] = arg[1].asDouble();
  else
    for (int i=0;i<dims;i++)
      radius[i] = arg[1].get(i+1).asDouble();
  filter->SetRadius(radius);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction meanfilter MeanFilterFunction
//input x radius
//output y
ArrayVector MeanFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(MeanFilter,arg);
}
