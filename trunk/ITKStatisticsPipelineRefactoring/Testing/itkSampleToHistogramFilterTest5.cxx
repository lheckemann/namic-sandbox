/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSampleTest.cxx,v $
Language:  C++
Date:      $Date: 2007/04/06 15:26:57 $
Version:   $Revision: 1.12 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkRGBPixel.h"
#include "itkVariableLengthVector.h"
#include "itkListSample.h"
#include "itkHistogram.h"
#include "itkSampleToHistogramFilter.h"
#include "itkImageToListSampleFilter.h"
#include "itkImageFileReader.h"

int itkSampleToHistogramFilterTest5(int argc, char *argv[] )
{

  const unsigned int numberOfComponents = 3;
  const unsigned int imageDimension = 2;

  if( argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFilename " << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Note: 
  //
  // The following two types are made different here.
  //
  // The purpose of this test is to verify that the 
  // MeasurementVectorType can use an integer type for its 
  // components, while the Histogram can use a float type for
  // it measurement type.
  //
  typedef signed int  VMeasurementType;  // integer type for the samples
  typedef float       HMeasurementType;  // float type for the histogram


  typedef itk::RGBPixel< VMeasurementType > MeasurementVectorType;

  typedef MeasurementVectorType  PixelType;
  typedef itk::Image< PixelType, imageDimension >   ImageType;

  typedef itk::Statistics::ImageToListSampleFilter< 
    ImageType > ImageToListSampleFilterType;
   
  typedef ImageToListSampleFilterType::ListSampleType  SampleType;

  typedef itk::Statistics::Histogram< HMeasurementType,
          numberOfComponents,
          itk::Statistics::DenseFrequencyContainer > HistogramType;

  typedef itk::Statistics::SampleToHistogramFilter<
    SampleType, HistogramType > FilterType;

  typedef itk::ImageFileReader< ImageType >    ReaderType;

  typedef FilterType::InputHistogramSizeObjectType         InputHistogramSizeObjectType;
  typedef FilterType::HistogramSizeType                    HistogramSizeType;
  typedef FilterType::HistogramMeasurementType             HistogramMeasurementType;
  typedef FilterType::HistogramMeasurementVectorType       HistogramMeasurementVectorType;
  typedef FilterType::InputHistogramMeasurementObjectType  InputHistogramMeasurementObjectType;
  typedef FilterType::
    InputHistogramMeasurementVectorObjectType  InputHistogramMeasurementVectorObjectType;

  ReaderType::Pointer reader = ReaderType::New();

  ImageToListSampleFilterType::Pointer imageToSampleFilter = ImageToListSampleFilterType::New();

  FilterType::Pointer filter = FilterType::New();

  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();  // FIXME : This shouldn't be needed here, the pipeline sould trigger Update().
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  imageToSampleFilter->SetInput( reader->GetOutput() );
  filter->SetInput( imageToSampleFilter->GetOutput() );
  
  HistogramMeasurementVectorType minimum;
  HistogramMeasurementVectorType maximum;

  minimum[0] = -17.5;
  minimum[1] = -19.5;
  minimum[2] = -24.5;

  maximum[0] =  17.5;
  maximum[1] =  19.5;
  maximum[2] =  24.5;

  HistogramSizeType histogramSize;

  histogramSize[0] = 36;
  histogramSize[1] = 40;
  histogramSize[2] = 50;

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  const HistogramType * histogram = filter->GetOutput();

  if( histogram->Size() )
    {
    std::cerr << "Histogram Size should have been zero" << std::endl;
    return EXIT_FAILURE;
    }


  filter->SetHistogramSize( histogramSize );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int expectedHistogramSize1 = 
    histogramSize[0] * histogramSize[1] * histogramSize[2];

  if( histogram->Size() != expectedHistogramSize1 )
    {
    std::cerr << "Histogram Size error" << std::endl;
    std::cerr << "We expected " << expectedHistogramSize1 << std::endl;
    std::cerr << "We received " << histogram->Size() << std::endl;
    return EXIT_FAILURE;
    }



  HistogramType::ConstIterator histogramItr = histogram->Begin();
  HistogramType::ConstIterator histogramEnd = histogram->End();

  const unsigned int expectedFrequency1 = 1;
  while( histogramItr != histogramEnd )
    {
    if( histogramItr.GetFrequency() != expectedFrequency1 )
      {
      std::cerr << "Histogram bin error for measure " << std::endl;
      std::cerr << histogramItr.GetMeasurementVector() << std::endl;
      std::cerr << "Expected frequency = " << expectedFrequency1 << std::endl;
      std::cerr << "Computed frequency = " << histogramItr.GetFrequency() << std::endl;
      }
    ++histogramItr;
    }



  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



