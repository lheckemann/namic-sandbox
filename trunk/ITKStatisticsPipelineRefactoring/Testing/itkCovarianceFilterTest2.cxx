/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceFilterTest.cxx,v $
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

#include "itkCovarianceFilter.h"
#include "itkListSample.h"
#include "itkFixedArray.h"
#include "itkMeasurementVectorTraits.h"

int itkCovarianceFilterTest2(int, char* [] ) 
{
  std::cout << "CovarianceFilter test \n \n";
  bool pass = true;
  std::string failureMeassage= "";

  const unsigned int                  MeasurementVectorSize = 3;
  const unsigned int                  numberOfMeasurementVectors = 3;
  unsigned int                        counter = 0;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >             MeasurementVectorType;
  typedef itk::Statistics::ListSample< 
    MeasurementVectorType >                    SampleType;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( MeasurementVectorSize ); 

  MeasurementVectorType               measure;
  
  //reset counter
  counter = 0;

  while ( counter < numberOfMeasurementVectors ) 
    {
    for( unsigned int i=0; i<MeasurementVectorSize; i++)
      {
      measure[i] = counter;
      }
    sample->PushBack( measure );
    counter++;
    }

  typedef itk::Statistics::CovarianceFilter< SampleType > 
    FilterType;

  FilterType::Pointer filter = FilterType::New() ;

  filter->SetInput( sample );

  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }    
 
  const FilterType::MatrixDecoratedType * decorator = filter->GetOutput() ;
  FilterType::MatrixType    covarianceOutput  = decorator->Get();

  std::cout << "Covariance Matrix: " << covarianceOutput << std::endl;
  FilterType::MatrixType covariance;

  if( !pass )
    {
    std::cout << "Test failed." << failureMeassage << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



