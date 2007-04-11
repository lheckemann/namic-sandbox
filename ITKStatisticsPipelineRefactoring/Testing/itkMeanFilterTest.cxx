/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:55:14 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkMeanFilter.h"
#include "itkListSample.h"
#include "itkFixedArray.h"
#include "itkMeasurementVectorTraits.h"

int itkMeanFilterTest(int, char* [] ) 
{
  const unsigned int MeasurementVectorSize = 2;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >             MeasurementVectorType;
  typedef itk::Statistics::ListSample< 
    MeasurementVectorType >                    SampleType;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( MeasurementVectorSize ); 

  const unsigned int                  numberOfMeasurementVectors = 5;
  unsigned int                        counter = 0;
  MeasurementVectorType               measure;

  while ( counter < numberOfMeasurementVectors ) 
    {
    for( unsigned int i=0; i<MeasurementVectorSize; i++)
      {
      measure[i] = counter;
      }
    sample->PushBack( measure );
    counter++;
    }

  typedef itk::Statistics::MeanFilter< SampleType > 
    FilterType;

  FilterType::Pointer filter = FilterType::New() ;
  
  filter->SetInput( sample );

  filter->Update() ;

  FilterType::MeasurementVectorDecoratedType * decorator = filter->GetOutput() ;
  FilterType::MeasurementVectorType    meanOutput  = decorator->Get();

  FilterType::MeasurementVectorType mean;

  mean[0] = 2.0;
  mean[1] = 2.0;

  bool pass = true;

  std::cout << meanOutput[0] << " " << mean[0] << " "  
            << meanOutput[1] << " " << mean[1] << " " << std::endl;  

  if (meanOutput[0] != mean[0] || 
      meanOutput[1] != mean[1])
    {
      pass = false ;
    }
 
  if( !pass )
    {
      std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;

}



