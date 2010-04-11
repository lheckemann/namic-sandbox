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

#include "lsqrDense.h"

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

#define AssignScalarValueToVectorElements(n1,n2,v,x) \
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



lsqrDense::lsqrDense()
{
  this->A = 0;
}


lsqrDense::~lsqrDense()
{
}


void
lsqrDense::SetMatrix( double ** inputA )
{
  this->A = inputA;
}


/**
 * computes y = y + A*x without altering x.
 */
void lsqrDense::
Aprod1(unsigned int m, unsigned int n, const double * x, double * y ) const
{
  for ( unsigned int row = 0; row < m; row++ )
    {
    const double * rowA = this->A[row];
    double sum = 0.0;

    for ( unsigned int col = 0; col < n; col++ )
      {
      sum += rowA[col] * x[col];
      }

    y[row] +=  sum;
    }
}


/**
 * computes x = x + A'*y without altering y.
 */
void lsqrDense::
Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const
{
  for ( unsigned int col = 0; col < n; col++ )
    {
    double sum = 0.0;

    for ( unsigned int row = 0; row < m; row++ )
      {
      sum += this->A[row][col] * y[row];
      }

    x[col] +=  sum;
    }
}


