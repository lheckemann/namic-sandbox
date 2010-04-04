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


//
// A more interesting testing case, but still a basic one.
//
int lsqrTest2( int , char * [] )
{

  lsqr solver;

  const unsigned int mm = 3;
  const unsigned int nn = 2;

  double bb[nn];
  double xx[mm];

  bb[0] = 1.0;
  bb[1] = 7.0;
  bb[2] = 3.0;

  solver.SetStandardErrorEstimatesFlag( true );
  double se[nn];

  solver.SetStandardErrorEstimates( se );

  solver.Solve( mm, nn, bb, xx );

  std::cout << "Stopped because " << solver.GetStoppingReason() << std::endl;
  std::cout << "Used " << solver.GetNumberOfIterationsPerformed() << " Iterations" << std::endl;
  std::cout << "Frobenius norm estimation of Abar = " << solver.GetFrobeniusNormEstimateOfAbar() << std::endl;
  std::cout << "Condition number estimation of Abar = " << solver.GetConditionNumberEstimateOfAbar() << std::endl;
  std::cout << "Estimate of final value of norm(rbar) = " << solver.GetFinalEstimateOfNormRbar() << std::endl;
  std::cout << "Estimate of final value of norm of residuals = " << solver.GetFinalEstimateOfNormOfResiduals() << std::endl;
  std::cout << "Estimate of norm of final solution = " << solver.GetFinalEstimateOfNormOfX() << std::endl;

  return EXIT_SUCCESS;
}
