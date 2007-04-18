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
#include "itkMahalanobisDistanceMembershipFunction.h"

int itkCovarianceFilterTest3(int, char* [] ) 
{
  std::cout << "CovarianceFilter test \n \n";
  bool pass = true;
  std::string failureMeassage= "";

  const unsigned int                  MeasurementVectorSize = 5;
  const unsigned int                  numberOfMeasurementVectors = 1000;
  unsigned int                        counter = 0;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >             MeasurementVectorType;
  typedef itk::Statistics::ListSample< 
    MeasurementVectorType >                    SampleType;

  typedef itk::Statistics::MahalanobisDistanceMembershipFunction< MeasurementVectorType > MembershipFunctionType;

  MembershipFunctionType::Pointer memberFunction = MembershipFunctionType::New();


  typedef MembershipFunctionType::MeanVectorType            MeanVectorType;
  typedef MembershipFunctionType::CovarianceMatrixType      CovarianceMatrixType;
 
  MeanVectorType mean( MeasurementVectorSize );
  CovarianceMatrixType covariance( MeasurementVectorSize, MeasurementVectorSize );

  mean[0] = 29;
  mean[1] = 31;
  mean[2] = 37;
  mean[3] = 41;
  mean[4] = 47;

  covariance[0][0] =  2.0;
  covariance[0][1] =  5.0;
  covariance[0][2] =  7.0;
  covariance[0][3] = 11.0;
  covariance[0][4] = 13.0;

  covariance[1][1] = 17.0;
  covariance[1][2] = 19.0;
  covariance[1][3] = 23.0;
  covariance[1][4] = 29.0;

  covariance[2][2] = 31.0;
  covariance[2][3] = 37.0;
  covariance[2][4] = 39.0;

  covariance[3][3] = 41.0;
  covariance[3][4] = 43.0;

  covariance[4][4] = 47.0;

  for( unsigned int i=0; i < MeasurementVectorSize; i++ )
    {
    for( unsigned int j=i; j < MeasurementVectorSize; j++ )
      {
      covariance[j][i] = covariance[i][j];
      }
    }

  std::cout << "Initial Mean = " << std::endl << mean << std::endl;
  std::cout << "Initial Covariance = " << std::endl << covariance << std::endl;

  memberFunction->SetMean( mean );
  memberFunction->SetCovariance( covariance );


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


  if( !pass )
    {
    std::cout << "Test failed." << failureMeassage << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}

