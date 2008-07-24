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

#include "itkArray.h"
#include "itkListSample.h"
#include "itkHistogram.h"
#include "itkSampleToHistogramFilter.h"

int itkSampleToHistogramFilterTest3(int argc, char *argv[] )
{

  const unsigned int numberOfComponents = 3;
  typedef signed int  MeasurementType;    // Exercise an integer type for the samples

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::Histogram< MeasurementType,
          numberOfComponents,
          itk::Statistics::DenseFrequencyContainer2 > HistogramType;

  typedef itk::Statistics::SampleToHistogramFilter<
    SampleType, HistogramType > FilterType;

  typedef FilterType::InputHistogramSizeObjectType         InputHistogramSizeObjectType;
  typedef FilterType::HistogramSizeType                    HistogramSizeType;
  typedef FilterType::HistogramMeasurementType             HistogramMeasurementType;
  typedef FilterType::HistogramMeasurementVectorType       HistogramMeasurementVectorType;
  typedef FilterType::InputHistogramMeasurementObjectType  InputHistogramMeasurementObjectType;
  typedef FilterType::
    InputHistogramMeasurementVectorObjectType  InputHistogramMeasurementVectorObjectType;

  FilterType::Pointer filter = FilterType::New();

  SampleType::Pointer sample = SampleType::New();

  
  HistogramMeasurementVectorType minimum;
  HistogramMeasurementVectorType maximum;

  minimum[0] = -17;
  minimum[1] = -19;
  minimum[2] = -24;

  maximum[0] =  17;
  maximum[1] =  19;
  maximum[2] =  24;

  HistogramSizeType histogramSize;

  histogramSize[0] = 36;
  histogramSize[1] = 40;
  histogramSize[2] = 50;

  MeasurementVectorType measure( numberOfComponents );

  sample->SetMeasurementVectorSize( numberOfComponents );

  // Populate the Sample
  for( unsigned int i=0; i < histogramSize[0]; i++ )
    {
    measure[0] = minimum[0] + i;
    for( unsigned int j=0; j < histogramSize[1]; j++ )
      {
      measure[1] = minimum[1] + j;
      for( unsigned int k=0; k < histogramSize[2]; k++ )
        {
        measure[2] = minimum[2] + k;
        sample->PushBack( measure );
        }
      }
    }


  filter->SetInput( sample );

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

  if( histogram->Size() != 1 )
    {
    std::cerr << "Histogram Size should have been one" << std::endl;
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

  // Exercise the saturation of the maximum
  maximum[0] =  itk::NumericTraits< MeasurementType >::max();
  maximum[1] =  itk::NumericTraits< MeasurementType >::max();
  maximum[2] =  itk::NumericTraits< MeasurementType >::max();
 
  minimum[0] =  itk::NumericTraits< MeasurementType >::min();
  minimum[1] =  itk::NumericTraits< MeasurementType >::min();
  minimum[2] =  itk::NumericTraits< MeasurementType >::min();
 
  filter->SetHistogramBinMaximum( maximum );
  filter->SetHistogramBinMinimum( minimum );
  filter->SetAutoMinimumMaximum( true );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  filter->SetAutoMinimumMaximum( false );

  // Add a sample that will exercise the overflow code
  //
  measure[0] = itk::NumericTraits< MeasurementType >::max();
  measure[1] = itk::NumericTraits< MeasurementType >::max();
  measure[2] = itk::NumericTraits< MeasurementType >::max();

  sample->PushBack( measure );
  sample->Modified();

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  filter->SetAutoMinimumMaximum( true );


  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



