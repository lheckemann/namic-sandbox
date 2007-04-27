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
#include "itkVariableLengthVector.h"
#include "itkListSample.h"
#include "itkHistogram.h"
#include "itkSampleToHistogramFilter.h"

int itkSampleToHistogramFilterTest2(int argc, char *argv[] )
{

  const unsigned int numberOfComponents = 3;
  typedef float      MeasurementType;

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::Histogram< MeasurementType,
          numberOfComponents,
          itk::Statistics::DenseFrequencyContainer > HistogramType;

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

  minimum[0] = -17.5;
  minimum[1] = -19.5;
  minimum[2] = -23.5;

  maximum[0] =  17.5;
  maximum[1] =  19.5;
  maximum[2] =  23.5;

  HistogramSizeType histogramSize;

  histogramSize[0] = 35;
  histogramSize[1] = 39;
  histogramSize[2] = 47;

  MeasurementVectorType measure( numberOfComponents );

  sample->SetMeasurementVectorSize( numberOfComponents );

  // Populate the Sample
  for( unsigned int i=0; i < histogramSize[0]; i++ )
    {
    measure[0] = minimum[0] + 0.5 + i;
    for( unsigned int j=0; j < histogramSize[1]; j++ )
      {
      measure[1] = minimum[1] + 0.5 + j;
      for( unsigned int k=0; k < histogramSize[2]; k++ )
        {
        measure[2] = minimum[2] + 0.5 + k;
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

  if( histogram->Size() )
    {
    std::cerr << "Histogram Size should have been zero" << std::endl;
    return EXIT_FAILURE;
    }

//  filter->SetAutoMinimumMaximum( true );
  filter->SetHistogramBinMinimum( minimum );
  filter->SetHistogramBinMaximum( maximum );
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



  HistogramType::ConstIterator histogramItr = histogram->Begin();
  HistogramType::ConstIterator histogramEnd = histogram->Begin();

  unsigned int expectedFrequency = 1;
  while( histogramItr != histogramEnd )
    {
    std::cout << histogramItr.GetMeasurementVector() << " " << histogramItr.GetFrequency() << std::endl;
    if( histogramItr.GetFrequency() != expectedFrequency )
      {
      std::cerr << "Histogram bin error for measure " << std::endl;
      std::cerr << histogramItr.GetMeasurementVector() << std::endl;
      std::cerr << "Expected frequency = " << expectedFrequency << std::endl;
      std::cerr << "Computed frequency = " << histogramItr.GetFrequency() << std::endl;
      }
    ++histogramItr;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



