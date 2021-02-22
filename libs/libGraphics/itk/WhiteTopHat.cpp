#include "ITKWrap.hpp"
#include <itkWhiteTopHatImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TWhiteTopHatBallImage(const ArrayVector &arg)
{
  int radius = 1;
  if (arg.size() >= 3) radius = arg[2].asInteger();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BinaryBallStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;  
  typedef itk::WhiteTopHatImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.SetRadius(radius);
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

template <int dims, DataClass dataclass, class PixelClass>
static Array TWhiteTopHatCrossImage(const ArrayVector &arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BinaryCrossStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;  
  typedef itk::WhiteTopHatImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction whitetophat WhiteTopHatFunction
//input x type radius
//output y
ArrayVector WhiteTopHatFunction(int nargout, const ArrayVector &arg)
{
  QString type = "ball";
  if (arg.size() >= 2) type = arg[1].asString().toLower();
  if (type == "ball")
    {
      ITKCases(TWhiteTopHatBallImage,arg);
    }
  if (type == "cross")
    {
      ITKCases(TWhiteTopHatCrossImage,arg);
    }
  throw Exception("Unsupported kernel type -- it must be either 'ball' or 'cross'");  
}
