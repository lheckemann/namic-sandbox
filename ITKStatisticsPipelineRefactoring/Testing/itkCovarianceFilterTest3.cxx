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

namespace itk {
namespace Statistics {
template < class TSample >
class MyCovarianceFilter : public CovarianceFilter< TSample >
{
 public:
  typedef MyCovarianceFilter                Self;
  typedef CovarianceFilter<TSample>         Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;
  typedef TSample                           SampleType;

  itkNewMacro(Self);

  //method to invoke MakeOutput with index value different
  //from one or zero. This is to check if an exception will be
  // thrown

  void CreateInvalidOutput()
    {
    unsigned int index=3;
    Superclass::MakeOutput( index );
    }
};
}
}

int itkCovarianceFilterTest3(int, char* [] ) 
{
  std::cout << "CovarianceFilter test \n \n";
  bool pass = true;
  std::string failureMeassage= "";

  typedef double                      MeasurementType;
  const unsigned int                  MeasurementVectorSize = 3;
  const unsigned int                  numberOfMeasurementVectors = 100;
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

  size.Fill(50);
  lowerBound.Fill(-350);
  upperBound.Fill(450);

  histogram->Initialize( size, lowerBound, upperBound );
  histogram->SetToZero();

  MembershipFunctionType::Pointer memberFunction = MembershipFunctionType::New();


  typedef MembershipFunctionType::MeanVectorType            MeanVectorType;
  typedef MembershipFunctionType::CovarianceMatrixType      CovarianceMatrixType;
 
  MeanVectorType mean( MeasurementVectorSize );
  CovarianceMatrixType covariance( MeasurementVectorSize, MeasurementVectorSize );

  mean[0] = 50;
  mean[1] = 52;
  mean[2] = 51; 

  covariance.set_identity();
  covariance[0][0] = 10000.0;
  covariance[1][1] = 8000.0;
  covariance[2][2] = 6000.0;


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
    const double frequency = vcl_floor( 1e5 * vcl_exp( -0.5 * MahalanobisDistance ) );
    itr.SetFrequency( frequency );
    ++itr;
    }


  typedef itk::Statistics::MyCovarianceFilter< SampleType > 
    FilterType;

  FilterType::Pointer filter = FilterType::New() ;


  //test if exception is thrown if a derived class tries to create
  // an invalid output 
  try
    {
    filter->CreateInvalidOutput();
    std::cerr << "Exception should have been thrown: " << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }
  
  filter->ResetPipeline();
  filter->SetInput( histogram );
 
  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }    
 
  const FilterType::MatrixDecoratedType * decorator = filter->GetCovarianceMatrixOutput() ;
  FilterType::MatrixType    covarianceOutput  = decorator->Get();

  FilterType::MeasurementVectorType    meanOutput = filter->GetMean();

  std::cout << "Mean: "              << meanOutput << std::endl;
  std::cout << "Covariance Matrix: " << covarianceOutput << std::endl;

  double epsilon = 1;

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
    {
    if ( fabs( meanOutput[i] - mean[i] ) > epsilon )
      {
      std::cerr << "The computed mean value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

  epsilon = 35;

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
  {
    for ( unsigned int j = 0; j < MeasurementVectorSize; j++ )
      {
      if ( fabs( covariance[i][j] - covarianceOutput[i][j] ) > epsilon )
        {
        std::cerr << "Computed covariance matrix value is incorrrect:"
                  << i << "," << j << "=" << covariance[i][j] 
                  << "," << covarianceOutput[i][j] << std::endl;
        return EXIT_FAILURE;
        }
      }
  }


  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}

