/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTDistributionFunctionTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/09/27 15:06:14 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkChiSquareDistribution.h"

int main(int, char* [] ) 
{
  std::cout << "itkChiSquareDistribution Test \n \n"; 
  
  typedef itk::Statistics::ChiSquareDistribution DistributionType;

  DistributionType::Pointer distributionFunction = DistributionType::New();


  int i;
  double x;
  double value;
  double diff;
  
  int status = EXIT_SUCCESS;

  // Tolerance for the values.
  double tol;

  // expected values for Chi-Square cdf with 1 degree of freedom at
  // values of 0:1:5
  double expected1[] = {0,
                        6.826894921370859e-001,
                        8.427007929497149e-001,
                        9.167354833364458e-001,
                        9.544997361036416e-001,
                        9.746526813225318e-001};


  std::cout << "Testing distribution with 1 degree of freedom" << std::endl;

  std::cout << "Chi-Square CDF" << std::endl;
  tol = 1e-14;
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  distributionFunction->SetDegreesOfFreedom( 1 );
  for (i = 0; i <= 5; ++i)
    {
    x = static_cast<double>(i);

    value = distributionFunction->EvaluateCDF( x );

    diff = fabs(value - expected1[i]);

    std::cout << "Chi-Square cdf at ";
    std::cout.width(2);
    std::cout << std::right << x << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(20);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(20);
    std::cout << std::left << expected1[i]
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }
  std::cout << std::endl;
  
  std::cout << "Inverse Chi-Square CDF" << std::endl;
  tol = 1e-13;   
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  for (i = 0; i <= 5; ++i)
    {

    value = distributionFunction->EvaluateInverseCDF( expected1[i] );

    diff = fabs(value - double(i));

    std::cout << "Chi-Square cdf at ";
    std::cout.width(20);
    std::cout << std::right << expected1[i] << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(22);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(22);
    std::cout << std::left << double(i)
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }
  std::cout << std::endl;

  
  // expected values for Chi-Square cdf with 11 degrees of freedom at
  // values of 0:2:20
  double expected11[] = {0,
                         1.504118282583805e-003,
                         3.008297612122607e-002,
                         1.266357467726155e-001,
                         2.866961703699681e-001,
                         4.696128489989594e-001,
                         6.363567794831719e-001,
                         7.670065225437422e-001,
                         8.588691197329420e-001,
                         9.184193863071046e-001,
                         9.546593255659396e-001};
    
  std::cout << "-----------------------------------------------"
            << std::endl << std::endl;
  std::cout << "Testing distribution with 11 degrees of freedom" << std::endl;
  
  std::cout << "Chi-Square CDF" << std::endl;
  tol = 1e-14;   
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  distributionFunction->SetDegreesOfFreedom( 11 );
  for (i = 0; i <= 10; ++i)
    {
    x = static_cast<double>(2*i);

    value = distributionFunction->EvaluateCDF( x );

    diff = fabs(value - expected11[i]);

    std::cout << "Chi-Square cdf at ";
    std::cout.width(2);
    std::cout << std::right << x << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(20);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(20);
    std::cout << std::left << expected11[i]
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }
  std::cout << std::endl;
  
  std::cout << "Inverse Chi-Square CDF" << std::endl;
  tol = 1e-13;
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  for (i = 0; i <= 10; ++i)
    {

    value = distributionFunction->EvaluateInverseCDF( expected11[i] );

    diff = fabs(value - double(2*i));

    std::cout << "Chi-Square cdf at ";
    std::cout.width(20);
    std::cout << std::right << expected11[i] << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(22);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(22);
    std::cout << std::left << double(2*i)
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }


  // expected values for Chi-Square cdf with 100 degrees of freedom at
  // values of 50:20:150
  double expected100[] = {6.953305247616148e-006,
                          9.845502476408603e-003,
                          2.468020344001694e-001,
                          7.677952194991408e-001,
                          9.764876021901918e-001,
                          9.990960679576461e-001};
    
  std::cout << "-----------------------------------------------"
            << std::endl << std::endl;
  std::cout << "Testing distribution with 100 degrees of freedom" << std::endl;
  
  std::cout << "Chi-Square CDF" << std::endl;
  tol = 1e-13;   
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  distributionFunction->SetDegreesOfFreedom( 100 );
  for (i = 0; i <= 5; ++i)
    {
    x = static_cast<double>(50+20*i);

    value = distributionFunction->EvaluateCDF( x );

    diff = fabs(value - expected100[i]);

    std::cout << "Chi-Square cdf at ";
    std::cout.width(2);
    std::cout << std::right << x << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(20);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(20);
    std::cout << std::left << expected100[i]
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }
  std::cout << std::endl;
  
  std::cout << "Inverse Chi-Square CDF" << std::endl;
  tol = 1e-10;
  std::cout << "Tolerance used for test: ";
  std::cout.width(20);
  std::cout.precision(15);
  std::cout << std::left << tol << std::endl;
  for (i = 0; i <= 5; ++i)
    {

    value = distributionFunction->EvaluateInverseCDF( expected100[i] );

    diff = fabs(value - double(50+20*i));

    std::cout << "Chi-Square cdf at ";
    std::cout.width(20);
    std::cout << std::right << expected100[i] << " with ";
    std::cout.width(2);
    std::cout << std::right << distributionFunction->GetDegreesOfFreedom()
              << " degrees of freedom = ";
    std::cout.width(22);
    std::cout << std::left << value
              << ", expected value = ";
    std::cout.width(22);
    std::cout << std::left << double(50+20*i)
              << ", error = ";
    std::cout.width(22);
    std::cout << std::left << diff;
    if (diff < tol)
      {
      std::cout << ", Passed." << std::endl;
      }
    else
      {
      std::cout << ", Failed." << std::endl;
      status = EXIT_FAILURE;
      }
    }
  
  return status;


}



