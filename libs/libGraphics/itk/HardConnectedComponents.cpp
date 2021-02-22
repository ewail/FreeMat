#include "ITKWrap.hpp"
#include <itkHardConnectedComponentImageFilter.h>

template <int dims, 
	  DataClass srcdataclass, class srcPixelClass,
	  DataClass dstdataclass, class dstPixelClass >
static Array THardConnectedComponentsFunction(Array A)
{
  typedef itk::Image<srcPixelClass, dims> srcITKType;
  typedef itk::Image<dstPixelClass, dims> dstITKType;
  typename srcITKType::Pointer imageIn = CreateITKFromArray<dims,srcdataclass,srcPixelClass>(A);
  typedef itk::HardConnectedComponentImageFilter<srcITKType, dstITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dstdataclass,dstPixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction hardconnectedcomponents HardConnectedComponentsFunction
//input x fullyconnected
//output y
ArrayVector HardConnectedComponentsFunction(int nargout, const ArrayVector& arg)
{
  if (arg.size() == 0) return ArrayVector();
  if (arg[0].is2D())
    return THardConnectedComponentsFunction<2,Bool,bool,Int32,int32>(arg[0]);
  else
    return THardConnectedComponentsFunction<3,Bool,bool,Int32,int32>(arg[0]);
}
