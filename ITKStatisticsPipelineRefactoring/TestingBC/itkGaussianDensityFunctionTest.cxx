/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDensityFunctionTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-04-05 23:54:28 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkGaussianDensityFunction.h"
#include "itkVector.h"

int itkGaussianDensityFunctionTest(int, char* [] ) 
{
  std::cout << "itkGaussianDensityFunctionTest Test \n \n"; 

  typedef itk::Vector< double, 1 >   MeasurementVectorType;

  typedef itk::Statistics::GaussianDensityFunction< 
                             MeasurementVectorType 
                                          > DensityFunctionType;

  typedef DensityFunctionType::MeasurementVectorType MeasurementVectorType;
  typedef DensityFunctionType::CovarianceType        CovarianceType;
  typedef DensityFunctionType::MeanType              MeanType;

  MeanType mean(1);           // size = 1 because this is a scalar case.
  CovarianceType  covariance;

  covariance.SetSize( 1, 1 ); // because this is a scalar case
    
  const double Sigma = 7.0;

  mean[0] = 19.0;
  covariance[0][0] = Sigma * Sigma;

  DensityFunctionType::Pointer densityFunction = DensityFunctionType::New();

  densityFunction->SetMean( &mean );
  densityFunction->SetCovariance( &covariance );

  MeasurementVectorType inputValue;
  inputValue[0] = mean[0];

  const double value1 = densityFunction->Evaluate( inputValue );

  const double gaussianNorm = 1.0 / ( vcl_sqrt( 2 * vnl_math::pi ) * Sigma );

  if( vcl_fabs( gaussianNorm - value1 ) > 1e-6 )
    {
    std::cerr << "ERROR in computation of the Gaussian " << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cout << "Evaluate at the mean = " << value1 << std::endl;
  std::cout << "Expected at the mean = " << gaussianNorm << std::endl;


  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



