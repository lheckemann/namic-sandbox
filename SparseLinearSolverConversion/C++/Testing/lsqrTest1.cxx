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

  return EXIT_SUCCESS;
}
