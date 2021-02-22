#include "ITKWrap.hpp"
#include <itkBinaryDilateImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TDilateBallFunction(Array A, int radius)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(A);
  typedef itk::BinaryBallStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::BinaryDilateImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.SetRadius(radius);
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

template <int dims, DataClass dataclass, class PixelClass>
static Array TDilateCrossFunction(Array A)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(A);
  typedef itk::BinaryCrossStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::BinaryDilateImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction dilate DilateFunction
//input x type radius
//output y
ArrayVector DilateFunction(int nargout, const ArrayVector &arg)
{
  int radius = 1;
  QString type = "ball";
  if (arg.size() >= 2) type = arg[1].asString().toLower();
  if (type == "ball" && arg.size() >= 3)
    radius = arg[2].asInteger();
  if (arg.size() == 0)
    return ArrayVector();
  if (type == "ball")
    {
      if (arg[0].is2D())
	return TDilateBallFunction<2,Bool,bool>(arg[0],radius);
      else
	return TDilateBallFunction<3,Bool,bool>(arg[0],radius);
    }
  if (type == "cross")
    {
      if (arg[0].is2D())
	return TDilateCrossFunction<2,Bool,bool>(arg[0]);
      else
	return TDilateCrossFunction<3,Bool,bool>(arg[0]);
    }
  throw Exception("Unsupported kernel type -- it must be either 'ball' or 'cross'");
}
