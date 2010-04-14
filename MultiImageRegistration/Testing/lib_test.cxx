#include <iostream>
#include "itkMultiImageRegistrationFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <string>
#include <vector>

int main(int argc, char **argv)
{
  typedef itk::Image<float, 3>                 ImageType;
  typedef itk::ImageFileReader<ImageType>      ReadType;
  typedef itk::ImageFileWriter<ImageType>      WriterType;

  ReadType::Pointer FileReader1 = ReadType::New();
  ReadType::Pointer FileReader2 = ReadType::New();
  ReadType::Pointer FileReader3 = ReadType::New();
  if(argc != 5)
    {
    std::cout << "ERROR:  Wrong number of arguments given!" << std::endl;
    return -1;
    }
  FileReader1->SetFileName(argv[1]);
  FileReader2->SetFileName(argv[2]);
  FileReader3->SetFileName(argv[3]);
  FileReader1->Update();
  FileReader2->Update();
  FileReader3->Update();
  std::vector<ImageType::Pointer> imageArray;
  imageArray.push_back(FileReader1->GetOutput());
  imageArray.push_back(FileReader2->GetOutput());
  imageArray.push_back(FileReader3->GetOutput());
  

  itk::MultiImageRegistrationFilter::Pointer m_Registration = itk::MultiImageRegistrationFilter::New();
  m_Registration->SetImages(imageArray);
  m_Registration->SetOptAffineNumberOfIterations(10);
  m_Registration->SetOptBsplineNumberOfIterations(10);
  m_Registration->SetNumberOfSpatialSamplesBsplinePercentage(0.005);
  m_Registration->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[4]);
  writer->SetInput(m_Registration->GetOutput());
  writer->Update();  
  return 0;
}

