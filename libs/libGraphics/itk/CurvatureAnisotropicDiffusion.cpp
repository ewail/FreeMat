#include "ITKWrap.hpp"
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>

template <int dims, DataClass dataclass, class PixelClass>
static Array CurvatureAnisotropicDiffusionFilter(const ArrayVector & arg)
{
  if (arg.size() < 4) throw Exception("Diffusion filters require 4 arguments: image, iterations, timestep, conductance");
  int iterations = arg[1].asInteger();
  double timestep = arg[2].asDouble();
  double conductance = arg[3].asDouble();
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  try
    {
      filter->SetNumberOfIterations(iterations);
      filter->SetTimeStep(timestep);
      filter->SetConductanceParameter(conductance);
      filter->SetInput(imageIn);
      filter->Update();
      return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
    }
  CATCH_ITK;
}


//try a = randn(16)*.1, a(4:10,4:10) = 5 + randn(7)*.2, b = curvature(a,30,1/30,.1)
// Disabled until this works...
//Signature
//gfunction curvatureanisotropicdiffusionfilter CurvatureAnisotropicDiffusionFilterFunction
//input x iterations timestep conductance
//output y
ArrayVector CurvatureAnisotropicDiffusionFilterFunction(int nargout, const ArrayVector& arg)
{
  ITKCasesFloatOnly(CurvatureAnisotropicDiffusionFilter,arg);
}
