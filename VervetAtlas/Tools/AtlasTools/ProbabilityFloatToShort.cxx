/*
 * Given input image with the voxel intensities in [0,1], convert them to
 * short scalar type with the range [0,255] (EM Segmenter expects that all the
 * input images have the same scalar type)
 * 
 */

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> FloatImageType;
typedef itk::OrientedImage<short, 3> ShortImageType;
typedef itk::ImageFileReader<FloatImageType> ReaderType;
typedef itk::ImageFileWriter<ShortImageType> WriterType;
typedef itk::MultiplyByConstantImageFilter<FloatImageType,float,FloatImageType> MulType;
typedef itk::CastImageFilter<FloatImageType,ShortImageType> CastType;

int main(int argc, char** argv){
  float maxProb = 255;

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " inputImage outputImage [maxProb=255]" << std::endl;    
    return -1;
  }

  if(argc>3)
    maxProb = atof(argv[3]);

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  reader->SetFileName(argv[1]);
  writer->SetFileName(argv[2]);
  reader->Update();

  MulType::Pointer mul = MulType::New();
  CastType::Pointer cast = CastType::New();

  mul->SetInput(reader->GetOutput());
  mul->SetConstant(maxProb);
  cast->SetInput(mul->GetOutput());
  writer->SetInput(cast->GetOutput());

  try{
    writer->Update();
  } catch(itk::ExceptionObject &e){
    std::cerr << "Execution failed with the exception " << e << std::endl;
    return -1;
  }

  return 0;
}
