#include "ITKWrap.hpp"
#include <itkBinaryErodeImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TErodeBallFunction(Array A, int radius)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(A);
  typedef itk::BinaryBallStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::BinaryErodeImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.SetRadius(radius);
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  std::cout << "IN Func\n";
  PrintITK<dims,PixelClass>(imageIn);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

template <int dims, DataClass dataclass, class PixelClass>
static Array TErodeCrossFunction(Array A, int radius)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(A);
  typedef itk::BinaryCrossStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::BinaryErodeImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.SetRadius(radius);
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction erode ErodeFunction
//input x radius type
//output y
ArrayVector ErodeFunction(int nargout, const ArrayVector &arg)
{
  int radius = 1;
  QString type = "ball";
  if (arg.size() >= 3) type = arg[2].asString().toLower();
  if (arg.size() >= 2) radius = arg[1].asInteger();
  if (arg.size() == 0)
    return ArrayVector();
  if (type == "ball")
    {
      if (arg[0].is2D())
	return TErodeBallFunction<2,Bool,bool>(arg[0],radius);
      else
	return TErodeBallFunction<3,Bool,bool>(arg[0],radius);
    }
  if (type == "cross")
    {
      if (arg[0].is2D())
	return TErodeCrossFunction<2,Bool,bool>(arg[0],radius);
      else
	return TErodeCrossFunction<3,Bool,bool>(arg[0],radius);
    }
  throw Exception("Unsupported kernel type -- it must be either 'ball' or 'cross'");
}
