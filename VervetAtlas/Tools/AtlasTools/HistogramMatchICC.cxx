/*
 * Take two images and two binary ICC labels. Perform intensity normalization
 * for the masked ICC regions in the images.
 * 
 * Andriy Fedorov, SPL
 * 19 Nov 2009
 */

#include "itkMaskImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::HistogramMatchingImageFilter<ImageType,ImageType> HistMatchType;
typedef itk::MaskImageFilter<ImageType,ImageType> MaskType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

int main(int argc, char** argv){
  int i;

  if(argc<6){
    std::cerr << "Usage: " << argv[0] << " referenceImage referenceImageMask inputImage inputImageMask outputImage" << std::endl;    
    return -1;
  }

  ReaderType::Pointer refReader = ReaderType::New();
  ReaderType::Pointer refMaskReader = ReaderType::New();
  ReaderType::Pointer inReader = ReaderType::New();
  ReaderType::Pointer inMaskReader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[5]);

  refReader->SetFileName(argv[1]);
  refMaskReader->SetFileName(argv[2]);
  inReader->SetFileName(argv[3]);
  inMaskReader->SetFileName(argv[4]);
  refReader->Update();
  refMaskReader->Update();
  inReader->Update();
  inMaskReader->Update();

  MaskType::Pointer refMasker = MaskType::New();
  refMasker->SetInput1(refReader->GetOutput());
  refMasker->SetInput2(refMaskReader->GetOutput());
  refMasker->Update();

  MaskType::Pointer inMasker = MaskType::New();
  inMasker->SetInput1(inReader->GetOutput());
  inMasker->SetInput2(inMaskReader->GetOutput());
  inMasker->Update();

  HistMatchType::Pointer histM = HistMatchType::New();
  histM->SetInput(inMasker->GetOutput());
  histM->SetReferenceImage(refMasker->GetOutput());
  histM->SetNumberOfHistogramLevels(64);
  histM->Update();

  writer->SetInput(histM->GetOutput());
  writer->Update();

  return 0;
}
