#include "ITKWrap.hpp"
#include <itkDerivativeImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TDerivativeFilter(const ArrayVector &arg)
{
  if (arg.size() < 3) throw Exception("Expect x order direction");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::DerivativeImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetOrder(arg[1].asInteger());
  if (arg[2].asInteger() < 1) throw Exception("derivative direction must be >= 1");
  filter->SetDirection(arg[2].asInteger()-1);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction derivativefilter DerivativeFilterFunction
//input x order direction
//output y
ArrayVector DerivativeFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCasesFloatOnly(TDerivativeFilter,arg);
}
