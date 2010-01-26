/*
 * Add an arbitrary number of float pixel type images
 *
 */


#include "itkAddImageFilter.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::AddImageFilter<ImageType,ImageType> AddType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

int main(int argc, char** argv){
  int i;

  if(argc<4)
    return -1;

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

  writer->SetInput(accumulator);
  writer->SetFileName(argv[argc-1]);
  writer->Update();

  return 0;
}
