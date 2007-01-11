#include <iostream>
#include "itkDiffusionTensor3D.h"
#include "itkImageFileReader.h"

int main( int argc, char *argv[] ){

  //create a reader
  //typedef double  PixelType;
  //typedef itk::VectorImage<PixelType, 3> DWIVectorImageType;
  
  //define the output tensor image type
  typedef itk::Image< itk::DiffusionTensor3D< float >, 3 > TensorImageType;
  
  TensorImageType::Pointer inputPtr = NULL;
  
  itk::ImageFileReader<TensorImageType>::Pointer reader 
    = itk::ImageFileReader<TensorImageType>::New();
    
  // Set the properties for NrrdReader
  reader->SetFileName(argv[1]);

  //update the reader, loading in the data
  reader->Update();
  
  //get a pointer to the data
  inputPtr = reader->GetOutput();
  TensorImageType::IndexType pixelindex;
  
  pixelindex[0]=atoi(argv[2]);
  pixelindex[1]=atoi(argv[3]);
  pixelindex[2]=atoi(argv[4]);
  
  //instantiate a pixel type
  TensorImageType::PixelType pixelvalue;
  
  pixelvalue = inputPtr->GetPixel(pixelindex);
  
  std::cout << pixelvalue <<std::endl;
  
  return 1;
}
