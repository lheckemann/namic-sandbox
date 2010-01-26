/*
 * Add an arbitrary number of float pixel type images
 *
 */


#include "itkOrientedImage.h"
#include "itkImageDuplicator.h"
#include "itkSubtractImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::SubtractImageFilter<ImageType,ImageType> SubType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ImageDuplicator<ImageType> DuplicatorType;

int main(int argc, char** argv){
  int i;

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " input_float_image const output_float_image" << std::endl;
    return -1;
  }

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  ImageType::Pointer accumulator;

  reader->SetFileName(argv[1]);
  reader->Update();

  float max = atoi(argv[2]);

  DuplicatorType::Pointer dup = DuplicatorType::New();
  dup->SetInputImage(reader->GetOutput());
  dup->Update();

  ImageType::Pointer ones = dup->GetOutput();
  ones->FillBuffer(max);

  SubType::Pointer sub = SubType::New();
  sub->SetInput1(ones);
  sub->SetInput2(reader->GetOutput());
  sub->Update();

  writer->SetInput(sub->GetOutput());
  writer->SetFileName(argv[argc-1]);
  writer->Update();

  return 0;
}
