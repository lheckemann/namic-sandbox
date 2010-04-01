/*
 * Take the list of images, and compute image where a pixel is assigned a
 * non-zero label value iff each of the input images has this label value at
 * the location. 
 *
 * Andriy Fedorov, SPL
 * 3 Dec 2009
 */


#include "itkImageRegionIteratorWithIndex.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<char, 3> ImageType;
typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

int main(int argc, char** argv){
  int i;

  if(argc<4){
    std::cout << "Usage: " << argv[0] << " input1 ... inputN output" << std::endl;
    return -1;
  }

  ReaderType::Pointer readers[argc-2];
  ImageType::Pointer inputs[argc-2];
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[argc-1]);

  for(i=1;i<argc-1;i++){
    readers[i-1] = ReaderType::New();
    readers[i-1]->SetFileName(argv[i]);
    readers[i-1]->Update();
    std::cout << argv[i] << " has been read" << std::endl;
    inputs[i-1] = readers[i-1]->GetOutput();
  }

  IteratorType it(inputs[0], inputs[0]->GetLargestPossibleRegion());
  it.GoToBegin();
  for(;!it.IsAtEnd();++it){
    ImageType::PixelType label = inputs[0]->GetPixel(it.GetIndex());
    for(i=1;i<argc-2;i++)
      if(inputs[i]->GetPixel(it.GetIndex())!=label){
        label = 0;
        break;
      }
    inputs[0]->SetPixel(it.GetIndex(),label);
  }

  writer->SetInput(inputs[0]);
  writer->Update();

  return 0;
}