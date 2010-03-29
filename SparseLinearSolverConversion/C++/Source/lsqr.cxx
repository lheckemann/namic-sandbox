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
#include <math.h>

#define Abs(x) ((x) >= 0 ? (x) : -(x))

#define CopyVector(n,x,y) \
  { \
  const double * xp = x; \
  const double * xend = xp + n; \
  double * yp = y; \
  while( xp != xend ) \
    { \
    *yp++ = *xp++; \
    } \
  }

#define AccumulateVector(n,x,y) \
  { \
  const double * xp = x; \
  const double * xend = xp + n; \
  double * yp = y; \
  while( xp != xend ) \
    { \
    *yp++ += *xp++; \
    } \
  }

#define AssignValueToVectorElements(n1,n2,v,x) \
  { \
  double * xp   = x + n1; \
  double * xend = x + n2; \
  while( xp != xend ) \
    { \
    *xp++ = v; \
    } \
  }

#define ElementWiseProductVector(n1,n2,x,y,z) \
  { \
  const double * xp   = x + n1; \
  const double * xend = x + n2; \
  double * yp = y; \
  double * zp = z; \
  while( xp != xend ) \
    { \
    *zp++ = *xp++ * *yp++; \
    } \
  }



lsqr::lsqr()
{
}


lsqr::~lsqr()
{
}


/**
 *  returns sqrt( a**2 + b**2 )
 *  with precautions to avoid overflow.
 */
double
lsqr::D2Norm( double a, double b ) const
{
  const double scale = Abs(a) + Abs(b);
  const double zero = 0.0;

  if( scale == zero )
    {
    return zero;
    }
  
  const double sa = a / scale;
  const double sb = b / scale;

  return scale * sqrt( sa * sa + sb * sb );
}


/* 

  returns x = (I - 2*z*z')*x.

  Implemented as x = x - z * ( 2*( z'*x ) )

*/
void lsqr::
HouseholderTransformation(unsigned int n, const double * z, double * x ) const
{
  // First, compute z'*x as a scalar product.
  double scalarProduct = 0.0;
  const double * zp = z;
  const double * zend = zp + n;
  double * xp = x;
  while( zp != zend )
    {
    scalarProduct += (*zp++) * (*xp++);
    }

  // Second, double the value of the scalar product.
  scalarProduct += scalarProduct;

  // Last, compute x = x - z * (2*z'*x) This subtract from x, double
  // the componenent of x that is parallel to z, effectively reflecting
  // x across the hyperplane whose normal is defined by z.
  zp = z;
  xp = x;
  zend = zp + n;
  while( zp != zend )
    {
    *xp++ -= scalarProduct * (*zp++);
    }
}


/**
 * computes x = x + A'*y without altering y,
 * where A is a test matrix of the form  A = Y*D*Z,
 * and the matrices D, Y, Z are represented by
 * the allocatable vectors d, hy, hz in this module. */
void lsqr::
Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const
{
  CopyVector( n, y, this->wm );
  this->HouseholderTransformation(m,this->hy,this->wm);
  const unsigned int minmn = ( m > n ) ? n : m;
  ElementWiseProductVector( 0, minmn, this->d, this->wm, this->wn );
  AssignValueToVectorElements(m,n,0.0,this->wn);
  this->HouseholderTransformation(n,this->hz,this->wn);
  AccumulateVector( n, this->wn, x );
}
