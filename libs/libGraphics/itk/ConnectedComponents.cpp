#include "ITKWrap.hpp"
#include <itkConnectedComponentImageFilter.h>

template <int dims, 
	  DataClass srcdataclass, class srcPixelClass,
	  DataClass dstdataclass, class dstPixelClass >
static Array TConnectedComponentsFunction(Array A, bool fullyconnected)
{
  typedef itk::Image<srcPixelClass, dims> srcITKType;
  typedef itk::Image<dstPixelClass, dims> dstITKType;
  typename srcITKType::Pointer imageIn = CreateITKFromArray<dims,srcdataclass,srcPixelClass>(A);
  typedef itk::ConnectedComponentImageFilter<srcITKType, dstITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetFullyConnected(fullyconnected);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dstdataclass,dstPixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction connectedcomponents ConnectedComponentsFunction
//input x fullyconnected
//output y
ArrayVector ConnectedComponentsFunction(int nargout, const ArrayVector& arg)
{
  bool fullyconnected = false;
  if (arg.size() >= 2) fullyconnected = arg[1].asInteger() > 0;
  if (arg.size() == 0) return ArrayVector();
  if (arg[0].is2D())
    return TConnectedComponentsFunction<2,Bool,bool,Int32,int32>(arg[0],fullyconnected);
  else
    return TConnectedComponentsFunction<3,Bool,bool,Int32,int32>(arg[0],fullyconnected);
}
