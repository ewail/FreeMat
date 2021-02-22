#include "ITKWrap.hpp"
#include <itkNeighborhoodConnectedImageFilter.h>


template <int dims, DataClass dataclass, class PixelClass>
static Array NeighborhoodConnectedFilter(const ArrayVector & arg)
{
  if (arg.size() < 4) throw Exception("connectedthreshold filter requires at least four arguments");
  if (arg[1].rows() != dims) throw Exception("seedlist must be dims x N array of seed locations, where dims matches the dimensionality of the input image");
  typedef itk::Image<PixelClass, dims> ITKType;
  typename ITKType::Pointer imageIn = CreateITKFromArray<dims,dataclass,PixelClass>(arg[0]);
  typedef itk::NeighborhoodConnectedImageFilter<ITKType, ITKType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  for (int i=0;i<arg[1].cols();i++)
    {
      typename ITKType::IndexType seed;
      for (int j=0;j<dims;j++)
	seed[j] = arg[1].get(i*dims + j + 1).asDouble() - 1;
      filter->AddSeed(seed);
    }
  filter->SetUpper(arg[3].asDouble());
  filter->SetLower(arg[2].asDouble());
  if (arg.size() > 4)
    filter->SetReplaceValue(arg[4].asDouble());
  filter->SetInput(imageIn);
  itk::Indent ndt;
  filter->PrintSelf(std::cout,ndt);
  filter->Update();
  return CreateArrayFromITK<dims,dataclass,PixelClass>(filter->GetOutput());
}

//@@Signature
//gfunction neighborhoodconnected NeighborhoodConnectedFunction
//input x seedlist low high replacevalue
//output y
ArrayVector NeighborhoodConnectedFunction(int nargout, const ArrayVector& arg)
{
  ITKCases(NeighborhoodConnectedFilter,arg);
}
