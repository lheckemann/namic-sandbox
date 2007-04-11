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
#include "itkHistogram.h"
#include "itkListSampleToHistogramFilter.h"

int itkListSampleToHistogramFilterTest(int argc, char *argv[] ) 
{
    
  const unsigned int numberOfComponents = 3;
  typedef float      MeasurementType;

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::Histogram< MeasurementType, 
          numberOfComponents, 
          itk::Statistics::DenseFrequencyContainer > HistogramType;

  typedef itk::Statistics::ListSampleToHistogramFilter< 
    SampleType, HistogramType > FilterType;

  typedef FilterType::InputHistogramSizeObjectType   InputHistogramSizeObjectType;
  typedef FilterType::HistogramSizeType              HistogramSizeType;

  FilterType::Pointer filter = FilterType::New();  

  SampleType::Pointer sample = SampleType::New();

  // Test GetInput() before setting the input
  if( filter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should have returned NULL" << std::endl;
    return EXIT_FAILURE;
    }

  // Test GetOutput() before creating the output
  if( filter->GetOutput() == NULL )
    {
    std::cerr << "GetOutput() should have returned NON-NULL" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetInput( sample );

  if( filter->GetInput() != sample.GetPointer() )
    {
    std::cerr << "GetInput() didn't matched SetInput()" << std::endl;
    return EXIT_FAILURE;
    }

  // Exercise the Print method.
  filter->Print( std::cout );


  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // Test GetOutput() after creating the output
  if( filter->GetOutput() == NULL )
    {
    std::cerr << "GetOutput() should have returned NON-NULL" << std::endl;
    return EXIT_FAILURE;
    }


  HistogramSizeType histogramSize1;
  histogramSize1[0] = 256;
  histogramSize1[1] = 256;
  histogramSize1[2] = 256;

  HistogramSizeType histogramSize2;
  histogramSize2[0] = 128;
  histogramSize2[1] = 128;
  histogramSize2[2] = 128;


  filter->SetHistogramSize( histogramSize1 );

  const InputHistogramSizeObjectType * returnedHistogramSizeObject =
    filter->GetHistogramSizeInput();

  if( returnedHistogramSizeObject == NULL )
    {
    std::cerr << "SetHistogramSize() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  HistogramSizeType returnedHistogramSize = returnedHistogramSizeObject->Get();

  for( unsigned int k1 = 0; k1 < numberOfComponents; k1++ )
    {
    if( returnedHistogramSize[k1] != histogramSize1[k1] )
      {
      std::cerr << "Get/Set HistogramSize() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetHistogramSize( histogramSize2 );

  returnedHistogramSizeObject =
      filter->GetHistogramSizeInput();

  returnedHistogramSize = returnedHistogramSizeObject->Get();

  for( unsigned int k2 = 0; k2 < numberOfComponents; k2++ )
    {
    if( returnedHistogramSize[k2] != histogramSize2[k2] )
      {
      std::cerr << "Get/Set HistogramSize() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  InputHistogramSizeObjectType::Pointer histogramSizeObject =
    InputHistogramSizeObjectType::New();

  histogramSizeObject->Set( histogramSize1 );

  filter->SetHistogramSizeInput( histogramSizeObject );

  returnedHistogramSizeObject = filter->GetHistogramSizeInput();

  if( returnedHistogramSizeObject != histogramSizeObject )
    {
    std::cerr << "Get/Set HistogramSizeInput() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramSize = returnedHistogramSizeObject->Get();

  for( unsigned int k3 = 0; k3 < numberOfComponents; k3++ )
    {
    if( returnedHistogramSize[k3] != histogramSize1[k3] )
      {
      std::cerr << "Get/Set HistogramSizeInput() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }

  histogramSizeObject->Set( histogramSize2 );

  filter->SetInput1( histogramSizeObject );

  returnedHistogramSizeObject = filter->GetInput1();

  if( returnedHistogramSizeObject != histogramSizeObject )
    {
    std::cerr << "Get/Set HistogramSizeInput() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramSize = returnedHistogramSizeObject->Get();

  for( unsigned int k4 = 0; k4 < numberOfComponents; k4++ )
    {
    if( returnedHistogramSize[k4] != histogramSize2[k4] )
      {
      std::cerr << "Get/Set HistogramSizeInput() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetHistogramSize( histogramSize1 );
  filter->Update();
  unsigned long modifiedTime = filter->GetMTime();
  filter->SetHistogramSize( histogramSize1 );

  if( filter->GetMTime() != modifiedTime )
    {
    std::cerr << "SetHistogramSize() failed modified Test 1" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetHistogramSize( histogramSize2 );

  if( filter->GetMTime() == modifiedTime )
    {
    std::cerr << "SetHistogramSize() failed modified Test 2" << std::endl;
    return EXIT_FAILURE;
    }


  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



