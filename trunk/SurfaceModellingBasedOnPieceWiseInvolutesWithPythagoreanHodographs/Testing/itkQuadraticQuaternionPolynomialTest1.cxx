/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: BSplineWarping1.cxx,v $
  Language:  C++
  Date:      $Date: 2008-04-21 17:40:10 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadraticQuaternionPolynomial.h"

int main( int argc, char * argv[] )
{

  if( argc < 2 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    return EXIT_FAILURE;
    }

  typedef itk::QuadraticQuaternionPolynomial<double> PolynomialType;

  PolynomialType::Pointer polynomial = PolynomialType::New();

  typedef PolynomialType::QuaternionType  QuaternionType;

  QuaternionType q0;
  QuaternionType q1;
  QuaternionType q2;

  polynomial->SetQuaternion0( q0 ); 
  polynomial->SetQuaternion1( q1 ); 
  polynomial->SetQuaternion2( q2 ); 

  QuaternionType Q = polynomial->Evaluate( 0.5 );

  return EXIT_SUCCESS;
}

