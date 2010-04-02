/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Language:  C++
  Date:      $Date: 2009-06-14 11:52:00 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "lsqr.h"

#include <iostream>

#include <stdlib.h>
#include <math.h>

int lsqrTest1( int , char * [] )
{

  lsqr solver;

  const unsigned int n = 2;
  double x[n];
  double z[n];

  x[0] = 3.0;
  x[1] = 5.0;

  z[0] = 0.0;
  z[1] = 1.0;

  solver.HouseholderTransformation(n,z,x);

  std::cout << x[0] << " " << x[1] << std::endl;

  { // Test 1 Dnrm2()
  unsigned int n1 = 5;
  double x1[n1];
  x1[0] = 1.0;
  x1[1] = 1.0;
  x1[2] = 1.0;
  x1[3] = 1.0;
  x1[4] = 1.0;

  const double norm =solver.Dnrm2( n1, x1 );
  const double expectedNorm = sqrt(5.0);
  if( norm != expectedNorm )
    {
    std::cerr << "Error in Dnrm2() test 1 " << std::endl;
    std::cerr << "Expected = " << expectedNorm << std::endl;
    std::cerr << "Received = " << norm << std::endl;
    return EXIT_FAILURE;
    } 
  std::cout << "Dnrm2 test1 passed " << std::endl;
  }

  { // Test 2 Dnrm2()
  unsigned int n2 = 5;

  const double dominantValue = 1e+300;

  double x2[n2];
  x2[0] = 1e+30;
  x2[1] = 1e+200;
  x2[2] = dominantValue;
  x2[3] = 1e+2;
  x2[4] = 1e+1;

  const double norm =solver.Dnrm2( n2, x2 );
  const double expectedNorm = dominantValue;

  if( norm != expectedNorm )
    {
    std::cerr << "Error in Dnrm2() test 2 " << std::endl;
    std::cerr << "Expected = " << expectedNorm << std::endl;
    std::cerr << "Received = " << norm << std::endl;
    return EXIT_FAILURE;
    } 
  std::cout << "Dnrm2 test2 passed " << std::endl;
  }


  solver.SetOutputStream( std::cout );

  const double eps = 1e-15;

  solver.SetEpsilon( eps );

  solver.SetMaximumNumberOfIterations( 100 );

  solver.SetToleranceA( 1e-16 );
  solver.SetToleranceB( 1e-16 );

  solver.SetUpperLimitOnConditional( 1.0 / ( 10 * sqrt( eps ) ) );

  solver.SetStandardErrorEstimatesFlag( true );

  std::cout << "Stopped because " << solver.GetStoppingReason() << std::endl;
  std::cout << "Used " << solver.GetNumberOfIterationsPerformed() << " Iterations" << std::endl;
  std::cout << "Frobenius norm estimation of Abar = " << solver.GetFrobeniusNormEstimateOfAbar() << std::endl;
  std::cout << "Condition number estimation of Abar = " << solver.GetConditionNumberEstimateOfAbar() << std::endl;
  std::cout << "Estimate of final value of norm(rbar) = " << solver.GetFinalEstimateOfNormRbar() << std::endl;
  std::cout << "Estimate of final value of norm of residuals = " << solver.GetFinalEstimateOfNormOfResiduals() << std::endl;
  std::cout << "Estimate of norm of final solution = " << solver.GetFinalEstimateOfNormOfX() << std::endl;

  return EXIT_SUCCESS;
}
