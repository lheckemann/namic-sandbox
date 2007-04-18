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
#include "itkFixedArray.h"
#include "itkMeasurementVectorTraits.h"
#include "itkMahalanobisDistanceMembershipFunction.h"
#include "itkHistogram.h"

int itkCovarianceFilterTest3(int, char* [] ) 
{
  std::cout << "CovarianceFilter test \n \n";
  bool pass = true;
  std::string failureMeassage= "";

  typedef double                      MeasurementType;
  const unsigned int                  MeasurementVectorSize = 5;
  const unsigned int                  numberOfMeasurementVectors = 1000;
  unsigned int                        counter = 0;

  typedef itk::FixedArray< 
    MeasurementType, MeasurementVectorSize >   MeasurementVectorType;

  typedef itk::Statistics::MahalanobisDistanceMembershipFunction< 
    MeasurementVectorType >                    MembershipFunctionType;

  typedef itk::Statistics::Histogram< MeasurementType, 
          MeasurementVectorSize, 
          itk::Statistics::DenseFrequencyContainer > HistogramType;

  typedef HistogramType    SampleType; 

  HistogramType::Pointer histogram = HistogramType::New();

  HistogramType::SizeType size;
  MeasurementVectorType lowerBound;
  MeasurementVectorType upperBound;

  size.Fill(5);
  lowerBound.Fill(0);
  upperBound.Fill(100);

  histogram->Initialize( size, lowerBound, upperBound );
  histogram->SetToZero();

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

/*
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
  */

  covariance.set_identity();
  covariance[0][0] = 2500.0;
  covariance[1][1] = 2500.0;
  covariance[2][2] = 2500.0;
  covariance[3][3] = 2500.0;
  covariance[4][4] = 2500.0;



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

  HistogramType::Iterator itr = histogram->Begin();
  HistogramType::Iterator end = histogram->End();

  while( itr != end )
    {
    const double MahalanobisDistance =
      memberFunction->Evaluate( itr.GetMeasurementVector() );
    const double frequency = vcl_exp( - MahalanobisDistance );
    itr.SetFrequency( frequency );
    std::cout << itr.GetMeasurementVector() << " MD = " << MahalanobisDistance << " f= " << itr.GetFrequency() << std::endl;
    ++itr;
    }


  typedef itk::Statistics::CovarianceFilter< SampleType > 
    FilterType;

  FilterType::Pointer filter = FilterType::New() ;

  filter->SetInput( histogram );

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

