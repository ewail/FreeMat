#include "ITKWrap.hpp"
#include <itkMorphologicalGradientImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array TMorphologicalGradientBallFunction(const ArrayVector & arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BinaryBallStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::MorphologicalGradientImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  if (arg.size() > 2)
    kernel.SetRadius(arg[2].asInteger());
  else
    kernel.SetRadius(1);
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

template <int dims, DataClass dataclass, class PixelClass>
static Array TMorphologicalGradientCrossFunction(const ArrayVector & arg)
{
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::BinaryCrossStructuringElement<PixelClass, dims> KernelType;
  KernelType kernel;
  typedef itk::MorphologicalGradientImageFilter<ITKType, ITKType, KernelType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  kernel.CreateStructuringElement();
  filter->SetKernel(kernel);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction morphologicalgradient MorphologicalGradientFunction
//input x type radius
//output y
ArrayVector MorphologicalGradientFunction(int nargout, const ArrayVector &arg)
{
  QString type = "ball";
  if (arg.size() >= 2) type = arg[1].asString().toLower();
  if (arg.size() == 0)
    return ArrayVector();
  if (type == "ball")
    {
      ITKCases(TMorphologicalGradientBallFunction,arg);
    }
  if (type == "cross")
    {
      ITKCases(TMorphologicalGradientCrossFunction,arg);
    }
  throw Exception("Unsupported kernel type -- it must be either 'ball' or 'cross'");
}
