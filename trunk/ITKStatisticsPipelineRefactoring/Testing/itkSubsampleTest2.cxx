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
#include "itkSubsample.h"

int itkSubsampleTest2(int argc, char *argv[] ) 
{
  std::cout << "Subsample Test \n \n"; 
    
  typedef itk::Array< float > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  SampleType::MeasurementVectorSizeType measurementVectorSize = 3;
  std::cerr << "Measurement vector size: " << measurementVectorSize 
            << std::endl;

  unsigned int sampleSize = 10;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( measurementVectorSize );

  MeasurementVectorType mv( measurementVectorSize );
  for ( unsigned int i = 0; i < sampleSize; i++ )
    {
    for (unsigned int j = 0; j < measurementVectorSize; j++ )
      {
      mv[j] = j + i * measurementVectorSize;
      }
    std::cout << "Adding measurement vector: " << mv << std::endl;
    sample->PushBack(mv);
    }

  // tests begin
  typedef itk::Statistics::Subsample< SampleType >   SubsampleType;
  SubsampleType::Pointer   subSample = SubsampleType::New();
  subSample->SetSample( sample );

  //Add measurment vectors in sample with even id number to subSample
  for ( unsigned int i=0 ; i < sample->Size() ; i= i+2 ) 
    {
    subSample->AddInstance( i );
    std::cout << "Adding instance: " << i << " to subSample" << std::endl;
    }    

  if ( subSample->Size() != 5 )
    {
    std::cerr << "Size of the subsample container should be 5" << std::endl;
    return EXIT_FAILURE;
    }
 
  for ( unsigned int i=0 ; i < subSample->Size() ; i++ ) 
    {
    std::cout << "Measurment Vector: " << i << "\t" 
              << subSample->GetMeasurementVector( i ) << std::endl;

    if ( subSample->GetMeasurementVector( i ) != 
                sample->GetMeasurementVector( i*2 ) )
      {
      std::cerr << "Subsampling is not correctly done!" << std::endl;
      return EXIT_FAILURE;
      }
    }
  
  typedef itk::Statistics::Subsample< SubsampleType >   CascadedSubsampleType;
  CascadedSubsampleType::Pointer  subSample2  = CascadedSubsampleType::New(); 
  subSample2->SetSample( subSample );

  //Add measurment vectors in subsample with even id number to subSample2
  for ( unsigned int i=0 ; i < subSample->Size() ; i= i+2 ) 
    {
    std::cout << "Adding instance: " << i << " to subSample2" << std::endl;
    subSample2->AddInstance( i );
    }    

  if ( subSample2->Size() != 3 )
    {
    std::cerr << "Size of the subsample2 container should be 3" << std::endl;
    return EXIT_FAILURE;
    }

  for ( unsigned int i=0 ; i < subSample2->Size() ; i++ ) 
    {
    std::cout << "Measurment Vector: " << i << "\t" 
              << subSample2->GetMeasurementVector( i ) << std::endl;

    if ( subSample2->GetMeasurementVector( i ) != 
                sample->GetMeasurementVector( i*4 ) )
      {
      std::cerr << "Subsampling of a subsample is not correctly done!" << std::endl;
      return EXIT_FAILURE;
      }
    }
 
  return EXIT_SUCCESS; 
}



