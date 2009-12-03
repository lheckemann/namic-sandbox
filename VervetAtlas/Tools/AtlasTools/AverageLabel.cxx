/*
 * Take the list of binary images, and compute N average images, where N is
 * the number of distinctive labels in the input image. The voxel value at the
 * average image is computed as the sum of the given label over all images
 * divided by the number of images and the label value, so that the
 * result is in [0,1].
 *
 * Andriy Fedorov, SPL
 * 2 Dec 2009
 */


#include "itkAddImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::DivideByConstantImageFilter<ImageType,float,ImageType> DivideType;
typedef itk::AddImageFilter<ImageType,ImageType> AddType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThresholdType;
typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> GaussianType;

// labels are fixed for now
const float labelList[] = {1,2,3,11,12,13,14,15,16,17,18,19,20};
const int labelListSize = 13;


int main(int argc, char** argv){
  int i;

  if(argc<4){
    std::cout << "Usage: " << argv[0] << "label_id label_image1 ... label_imageN output" << std::endl;
    return -1;
  }

  


  DivideType::Pointer divider = DivideType::New();
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  ImageType::Pointer accumulator;

  int label = atoi(argv[1]);

  reader->SetFileName(argv[2]);
  reader->Update();

  ImageType::Pointer inputImage = reader->GetOutput();
  ImageType::SpacingType spacing = inputImage->GetSpacing();
  GaussianType::ArrayType smoothVariance;
  smoothVariance[0] = spacing[0];
  smoothVariance[1] = spacing[1];
  smoothVariance[2] = spacing[2];

  
  ThresholdType::Pointer thresh = ThresholdType::New();
  thresh->SetInput(reader->GetOutput());
  thresh->SetUpperThreshold(label);
  thresh->SetLowerThreshold(label);
  thresh->SetInsideValue(label);
  thresh->SetOutsideValue(0);

  GaussianType::Pointer gauss = GaussianType::New();
  gauss->SetInput(thresh->GetOutput());
  gauss->SetUseImageSpacing(1);
  gauss->SetVariance(smoothVariance);
  gauss->Update();

  accumulator = gauss->GetOutput();


  for(i=3;i<argc-1;i++){
    AddType::Pointer adder = AddType::New();
    reader = ReaderType::New();
    reader->SetFileName(argv[i]);
    reader->Update();
    std::cout << "Read " << argv[i] << std::endl;

    ThresholdType::Pointer thresh = ThresholdType::New();
    thresh->SetInput(reader->GetOutput());
    thresh->SetUpperThreshold(label);
    thresh->SetLowerThreshold(label);
    thresh->SetInsideValue(label);
    thresh->SetOutsideValue(0);

    GaussianType::Pointer gauss = GaussianType::New();
    gauss->SetUseImageSpacing(1);
    gauss->SetInput(thresh->GetOutput());
    gauss->SetVariance(smoothVariance);
    gauss->Update();

    adder->SetInput1(accumulator);
    adder->SetInput2(gauss->GetOutput());
    adder->Update();
    accumulator = adder->GetOutput();
  }

  divider->SetInput(accumulator);
  divider->SetConstant(label*(argc-3));
  divider->Update();

  writer->SetInput(divider->GetOutput());
  writer->SetFileName(argv[argc-1]);
  writer->Update();

  return 0;
}
