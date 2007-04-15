/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkListSampleTest.cxx,v $
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
#include "itkStatisticsAlgorithm.h"

int itkStatisticsAlgorithmTest(int argc, char *argv[] ) 
{
  std::cout << "StatisticsAlgorithm Test \n \n"; 

  const unsigned int measurementVectorSize = 2;
    
  typedef itk::Array< float > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( measurementVectorSize );

  MeasurementVectorType measure( measurementVectorSize );

  MeasurementVectorType realUpper( measurementVectorSize );
  MeasurementVectorType realLower( measurementVectorSize );

  const unsigned int numberOfSamples = 25;
  
  realLower.Fill( 1000 );
  realUpper.Fill(    0 );
    
  for( unsigned int i = 0; i < numberOfSamples; i++ )
    {
    float value = i + 3;
    measure[0] = value;
    measure[1] = value * value;
    sample->PushBack( measure );

    for(unsigned int j = 0; j < measurementVectorSize; j++ )
      {
      if( measure[j] < realLower[j] )
        {
        realLower[j] = measure[j];
        }
      if( measure[j] > realUpper[j] )
        {
        realUpper[j] = measure[j];
        }
      }
    }

  SampleType::MeasurementVectorType lower( measurementVectorSize );
  SampleType::MeasurementVectorType upper( measurementVectorSize );

  const SampleType * constSample = sample.GetPointer();

  // testing the equivalent of an empty sample by passing 
  // the Begin() iterator inlieu of the End() iterator.
  try
    {
    itk::Statistics::Algorithm::FindSampleBound( 
      constSample,  
      constSample->Begin(), constSample->Begin(), 
      lower, upper
      );
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Now testing the real algorithm
  try
    {
    itk::Statistics::Algorithm::FindSampleBound( 
      constSample,  
      constSample->Begin(), constSample->End(), 
      lower, upper
      );
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << excp << std::endl;
    return EXIT_FAILURE;
    }


  const float epsilon = 1e-5;

  for(unsigned int j = 0; j < measurementVectorSize; j++ )
    {
    if( vnl_math_abs( lower[j] - realLower[j] ) > epsilon )
      {
      std::cerr << "FindSampleBound() failed" << std::endl;
      std::cerr << "Expected lower = " << realLower << std::endl;
      std::cerr << "Computed lower = " << lower << std::endl;
      return EXIT_FAILURE;
      }
    if( vnl_math_abs( upper[j] - realUpper[j] ) > epsilon )
      {
      std::cerr << "FindSampleBound() failed" << std::endl;
      std::cerr << "Expected upper = " << realUpper << std::endl;
      std::cerr << "Computed upper = " << upper << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



