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
#include "itkShiftScaleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include <iostream>

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::OrientedImage<short, 3> OutputImageType;
typedef itk::HistogramMatchingImageFilter<ImageType,ImageType> HistMatchType;
typedef itk::MaskImageFilter<ImageType,ImageType> MaskType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<OutputImageType> WriterType;
typedef itk::MinimumMaximumImageFilter<ImageType> MinMaxType;
typedef itk::ShiftScaleImageFilter<ImageType,ImageType> RescaleType;
typedef itk::CastImageFilter<ImageType,OutputImageType> CastType;

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
  histM->SetNumberOfHistogramLevels(128);
  histM->Update();

  MinMaxType::Pointer minMax = MinMaxType::New();
  minMax->SetInput(histM->GetOutput());
  minMax->Update();

  std::cout << "Min = " << minMax->GetMinimum() << 
    " Max = " << minMax->GetMaximum() << std::endl;
  
  RescaleType::Pointer rescaler = RescaleType::New();
  rescaler->SetInput(histM->GetOutput());
  rescaler->SetShift(-minMax->GetMinimum());
  rescaler->SetScale(256.*64./(minMax->GetMaximum()-minMax->GetMinimum()));
  rescaler->Update();

  MinMaxType::Pointer minMax1 = MinMaxType::New();
  minMax1->SetInput(rescaler->GetOutput());
  minMax1->Update();

  std::cout << "Scale = " << rescaler->GetScale() << 
    " Shift = " << rescaler->GetShift() << std::endl;
  std::cout << "Min = " << minMax1->GetMinimum() << 
    " Max = " << minMax1->GetMaximum() << std::endl;

  CastType::Pointer cast = CastType::New();
  cast->SetInput(rescaler->GetOutput());  

  writer->SetInput(cast->GetOutput());
  writer->Update();

  return 0;
}
