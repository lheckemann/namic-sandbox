/*
 * Take the list of images, and compute average image. Read images with float
 * pixel type to avoid overflow.
 *
 * Rescaling may not be a good idea because of the following reasons:
 *   1) intensities outside the ICC should not be used for rescaling. We don't
 *   have ICCs to mask them out.
 *   2) there are differences between the subjects inside ICC: vessels,
 *   artifacts. Resampling may actually bias the average based on the artifact
 *   intensities.
 *
 * Andriy Fedorov, SPL
 * 11 Nov 2009
 */


#include "itkAddImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::DivideByConstantImageFilter<ImageType,float,ImageType> DivideType;
typedef itk::AddImageFilter<ImageType,ImageType> AddType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

int main(int argc, char** argv){
  int i;

  if(argc<4)
    return -1;

  DivideType::Pointer divider = DivideType::New();
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  ImageType::Pointer accumulator;

  reader->SetFileName(argv[1]);
  reader->Update();
  accumulator = reader->GetOutput();


  for(i=2;i<argc-1;i++){
    AddType::Pointer adder = AddType::New();
    reader = ReaderType::New();
    reader->SetFileName(argv[i]);
    reader->Update();
    std::cout << "Read " << argv[i] << std::endl;

    adder->SetInput1(accumulator);
    adder->SetInput2(reader->GetOutput());
    adder->Update();
    accumulator = adder->GetOutput();
  }

  divider->SetInput(accumulator);
  divider->SetConstant(argc-2);
  divider->Update();

  writer->SetInput(divider->GetOutput());
  writer->SetFileName(argv[argc-1]);
  writer->Update();

  return 0;
}
