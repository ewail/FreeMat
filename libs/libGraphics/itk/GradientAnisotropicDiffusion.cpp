#include "ITKWrap.hpp"
#include <itkGradientAnisotropicDiffusionImageFilter.h>


template <int dims, DataClass dataclass, class PixelClass>
static Array GradientAnisotropicDiffusionFilter(const ArrayVector & arg)
{
  if (arg.size() < 4) throw Exception("Diffusion filters require 4 arguments: image, iterations, timestep, conductance");
  int iterations = arg[1].asInteger();
  double timestep = arg[2].asDouble();
  double conductance = arg[3].asDouble();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::GradientAnisotropicDiffusionImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNumberOfIterations(iterations);
  filter->SetTimeStep(timestep);
  filter->SetConductanceParameter(conductance);
  filter->SetInput(imageIn);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}


//try a = randn(16)*.1, a(4:10,4:10) = 5 + randn(7)*.2, b = gradient(a,30,1/30,.1)
//Disabled for now.
//Signature
//gfunction gradientanisotropicdiffusionfilter GradientAnisotropicDiffusionFilterFunction
//input x iterations timestep conductance
//output y
ArrayVector GradientAnisotropicDiffusionFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCasesFloatOnly(GradientAnisotropicDiffusionFilter,arg);
}
