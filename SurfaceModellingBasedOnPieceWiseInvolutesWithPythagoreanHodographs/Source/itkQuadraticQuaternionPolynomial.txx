/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadraticQuaternionPolynomial.txx,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuadraticQuaternionPolynomial_txx
#define __itkQuadraticQuaternionPolynomial_txx

#include "itkQuadraticQuaternionPolynomial.h"
#include "itkNumericTraits.h" 
#include "itkExceptionObject.h"


namespace itk
{

/** Constructor */
template<class T>
QuadraticQuaternionPolynomial<T>
::QuadraticQuaternionPolynomial()
{
}

/** Destructor */
template<class T>
QuadraticQuaternionPolynomial<T>
::~QuadraticQuaternionPolynomial()
{
}

/** Evaluate the quadratic polynomial */
template<class T>
typename QuadraticQuaternionPolynomial<T>::OutputType
QuadraticQuaternionPolynomial<T>
::Evaluate( const RealType & t ) const
{
  QuaternionType Q;
  return Q;
}


} // end namespace itk

#endif
