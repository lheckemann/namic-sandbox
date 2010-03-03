
#include "itkImageRegionIterator.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<short, 3> ImageType;
typedef itk::ImageRegionIterator<ImageType> IteratorType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

int main(int argc, char** argv){
  int i;

  if(argc<5){
    std::cout << "Usage: " << argv[0] << " input_image label_in label_out output_image";
    return -1;
  }

  ReaderType::Pointer reader = ReaderType::New();
  ImageType::Pointer input;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[argc-1]);

  std::map<int,int> labelInOut;

  for(i=2;i<argc-1;i+=2){
    labelInOut[atoi(argv[i])] = atoi(argv[i+1]);
    std::cout << "Label in: " << atoi(argv[i]) << 
      ", out: " << atoi(argv[i+1]) << std::endl;
  }
  
  reader->SetFileName(argv[1]);
  reader->Update();
  input = reader->GetOutput();

  IteratorType it(input, input->GetLargestPossibleRegion());
  it.GoToBegin();
  for(;!it.IsAtEnd();++it){
    ImageType::PixelType p = it.Get();
    if(p)
      if(labelInOut[p])
        it.Set(labelInOut[p]);
  }

  writer->SetInput(input);
  writer->SetUseCompression(1);
  writer->Update();

  return 0;
}
