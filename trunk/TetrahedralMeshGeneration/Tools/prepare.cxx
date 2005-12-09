#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkImageAdaptor.h"
#include "itkAddPixelAccessor.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenSystem.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkInterpolateImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMesh.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <unistd.h>

//#define MODEL_SPHERE 1
typedef unsigned short OutPixelType;
typedef itk::Image<OutPixelType,3> OutImageType;

typedef unsigned short PixelType;
typedef float InternalPixelType;
typedef itk::ImageFileWriter<OutImageType> OutImageWriterType;

typedef itk::Image<PixelType,3> ImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::CastImageFilter<ImageType,OutImageType> CastFilterType;

int main(int argc, char** argv){
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput(reader->GetOutput());
  caster->Update();

  typedef itk::ImageRegionIterator<OutImageType> IteratorType;
  IteratorType it(caster->GetOutput(), caster->GetOutput()->GetRequestedRegion());
  OutImageType::Pointer output_image = caster->GetOutput();
  for(it.GoToBegin(); !it.IsAtEnd(); ++it){
    if(it.Get()>0)
      it.Set(1);
  }
  
  OutImageWriterType::Pointer writer = OutImageWriterType::New();
  writer->SetFileName("ready_image.mhd");
  writer->SetInput(output_image);
  writer->Update();
 
  return 0;
}
