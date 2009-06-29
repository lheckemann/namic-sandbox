#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "set"

#include "itkPoint.h"
#include "itkImage.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkSignedMaurerDistanceMapImageFilter.h" 
#include "itkSphereSpatialFunction.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h" 
#include "itkBinaryThresholdImageFunction.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkScalarImageToListAdaptor.h"
#include "itkListSampleToHistogramGenerator.h"

#include "itkFuzzyClassificationImageFilter.h"

#include "TissueClassificationCLP.h"

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::ImageFileReader< ImageType  > ImageReaderType;

int main (int argc, char* argv[])
{

  PARSE_ARGS;
  
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputVolume.c_str() );
  ImageType::Pointer image;
  try
    {
    reader->Update();
    image = reader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageReaderType::Pointer maskreader = ImageReaderType::New();
  maskreader->SetFileName( maskVolume.c_str() );
  ImageType::Pointer mask;
  try
    {
    maskreader->Update();
    mask = maskreader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::FuzzyClassificationImageFilter<ImageType, ImageType> ClassifierType;
  ClassifierType::Pointer classifier = ClassifierType::New();
  classifier->SetInput( image );

  classifier->SetNumberOfClasses( nClasses );
  classifier->SetBiasCorrectionOption( nBiasOption );
  classifier->SetImageMask( mask );

  try
    {
    classifier->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageType::Pointer label = ImageType::New();
  label->CopyInformation( image );
  label->SetRegions( label->GetLargestPossibleRegion() );
  label->Allocate();
  label->FillBuffer( 0 );

  itk::ImageRegionIteratorWithIndex<ImageType> it( label, label->GetLargestPossibleRegion() );
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    ImageType::IndexType idx = it.GetIndex();
    ImageType::PixelType mLabel = 0.25;
    int assignedLabel = -1;
    for (int k = 0; k < nClasses; k++)
    {
      ImageType::PixelType p = classifier->GetOutput(k)->GetPixel(idx);
      if (p > mLabel)
      {
        mLabel = p;
        assignedLabel = k;
      }
    }
    if (assignedLabel >=0)
    {
      it.Set( assignedLabel+1 );
    }
  }
  itk::ImageFileWriter<ImageType>::Pointer wlabel = itk::ImageFileWriter<ImageType>::New();
  wlabel->SetInput( label );
  wlabel->SetFileName( brainLabel.c_str() );

  try
    {
    wlabel->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageType::Pointer biasField = classifier->GetBiasField();

  wlabel->SetInput( biasField );
  wlabel->SetFileName( biasFieldFile.c_str() );

  try
    {
    wlabel->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  const std::vector<float>& classCenter = classifier->GetClassCentroid();
  const std::vector<float>& classStd = classifier->GetClassStandardDeviation();

  for (int k = 0; k < nClasses; k++)
  {
    std::cout << "Class " << k << ":\n\t";
    std::cout << classCenter[k] << " +/- " << classStd[k] << std::endl;
  }

  //wlabel->SetInput( classifier->GetOutput(0) );
  //wlabel->SetFileName( "mem0.mha" );
  //wlabel->Update();
  //wlabel->SetInput( classifier->GetOutput(1) );
  //wlabel->SetFileName( "mem1.mha" );
  //wlabel->Update();
  //wlabel->SetInput( classifier->GetOutput(2) );
  //wlabel->SetFileName( "mem2.mha" );
  //wlabel->Update();

  return EXIT_SUCCESS;
}


