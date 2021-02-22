#ifndef __ITKWrap_hpp__
#define __ITKWrap_hpp__

#include <itkImportImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryCrossStructuringElement.h>
#include "Array.hpp"

template <int dims, typename pixelclass>
void PrintITK(typename itk::Image<pixelclass,dims>::Pointer p)
{
  int rows = p->GetLargestPossibleRegion().GetSize()[0];
  int cols = p->GetLargestPossibleRegion().GetSize()[1];
  pixelclass* ptr = p->GetBufferPointer();
  for (int i=0;i<rows;i++)
    {
      for (int j=0;j<cols;j++)
	{
	  std::cout << ptr[i*cols+j] << " ";
	}
      std::cout << "\n";
    }
}

template <int dims, int dataclass, typename pixelclass>
typename itk::Image<pixelclass, dims>::Pointer CreateITKFromArray(const Array& z)
{
  typename itk::Image<pixelclass, dims>::Pointer vol;
  Array p = z.toClass(static_cast<DataClass>(dataclass));
  p.ensureNotScalarEncoded();
  typedef itk::Image<pixelclass, dims> ITKType;
  typedef itk::ImportImageFilter<pixelclass, dims> ImageImportType;
  typename ImageImportType::Pointer importer = ImageImportType::New();
  typename ImageImportType::RegionType region;
  for (int i=0;i<dims;i++)
    region.SetSize(i, p.dimensions()[i]);
  importer->SetRegion(region);
  typename ITKType::SpacingType vol_spacing;
  for (int i=0;i<dims;i++)
    vol_spacing[i] = 1;
  importer->SetSpacing(vol_spacing);
  typename ITKType::PointType vol_origin;
  for (int i=0;i<dims;i++)
    vol_origin[i] = 0;
  importer->SetOrigin(vol_origin);
  importer->SetImportPointer(reinterpret_cast<pixelclass*>(p.getVoidPointer()), p.length(), false);
  importer->Update();
  typedef itk::CastImageFilter<ITKType,ITKType> CastType;
  typename CastType::Pointer caster = CastType::New();
  caster->SetInput(importer->GetOutput());
  caster->Update();
  vol = caster->GetOutput();
  return vol;
}

template <int dims, int dataclass, typename pixelclass>
Array CreateArrayFromITK(typename itk::Image<pixelclass,dims>::Pointer p)
{
  NTuple d;
  for (int i=0;i<dims;i++)
    d.set(i,p->GetLargestPossibleRegion().GetSize()[i]);
  Array ret(static_cast<DataClass>(dataclass),d);
  memcpy(ret.getVoidPointer(),p->GetBufferPointer(),
	 sizeof(pixelclass)*ret.length());
  return ret;
}

#define ITKCases(func,arg)			\
  switch(arg[0].dataClass()) {			\
 case Int32:					\
  {						\
  if (arg[0].is2D())				\
    return func<2,Int32,int32_t>(arg);	\
  else						\
    return func<3,Int32,int32_t>(arg);	\
  break;					\
  }						\
 case Float:					\
 {						\
   if (arg[0].is2D())				\
     return func<2,Float,float>(arg);	\
   else						\
     return func<3,Float,float>(arg);	\
   break;					\
 }						\
 case Double:					\
 {						\
   if (arg[0].is2D())				\
     return func<2,Double,double>(arg);	\
   else						\
     return func<3,Double,double>(arg);	\
   break;					\
 }						\
 default:					\
 throw Exception("Unhandle ITK volume type.");	\
 }

#define ITKCasesFloatOnly(func,arg)			\
  switch(arg[0].dataClass()) {				\
  case Float:						\
    {							\
      if (arg[0].is2D())				\
	return func<2,Float,float>(arg);		\
      else						\
	return func<3,Float,float>(arg);		\
      break;						\
    }							\
  case Double:						\
    {							\
      if (arg[0].is2D())				\
	return func<2,Double,double>(arg);		\
      else						\
	return func<3,Double,double>(arg);		\
      break;						\
    }							\
  default:						\
    throw Exception("Unhandle ITK volume type (only float or double types allowed for this operation).");	\
  }

#define CATCH_ITK \
  catch(itk::ExceptionObject &err) \
    {									\
      throw Exception(QString("ITK Exception Caught %1").arg(err.GetDescription())); \
    }

#endif
