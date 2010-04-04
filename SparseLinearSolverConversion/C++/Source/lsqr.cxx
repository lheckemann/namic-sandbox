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



lsqr::lsqr()
{
  this->eps = 1e-16;
  this->atol = 1e-6;
  this->btol = 1e-6;
  this->conlim = 1.0 / ( 10 * sqrt( eps ) );
  this->itnlim = 10;
  this->nout = NULL;
  this->istop = 0;
  this->itn = 0;
  this->Anorm = 0.0;
  this->Acond = 0.0;
  this->rnorm = 0.0;
  this->Arnorm = 0.0;
  this->xnorm = 0.0;
  this->wantse = false;
  this->se = NULL;
}


lsqr::~lsqr()
{
}


unsigned int
lsqr::GetStoppingReason() const
{
  return this->istop;
}


unsigned int
lsqr::GetNumberOfIterationsPerformed() const
{
  return this->itn;
}


double
lsqr::GetFrobeniusNormEstimateOfAbar() const
{
  return this->Anorm;
}


double
lsqr::GetConditionNumberEstimateOfAbar() const
{
  return this->Acond;
}


double
lsqr::GetFinalEstimateOfNormRbar() const
{
  return this->rnorm;
}


double 
lsqr::GetFinalEstimateOfNormOfResiduals() const
{
  return this->Arnorm;
}


double 
lsqr::GetFinalEstimateOfNormOfX() const
{
  return this->xnorm;
}


void
lsqr::SetStandardErrorEstimatesFlag(bool flag)
{
  this->wantse = flag;
}


void
lsqr::SetEpsilon( double value )
{
  this->eps = value;
}


void
lsqr::SetDamp( double value )
{
  this->damp = value;
}


void
lsqr::SetToleranceA( double value )
{
  this->atol = value;
}


void
lsqr::SetToleranceB( double value )
{
  this->btol = value;
}


void
lsqr::SetMaximumNumberOfIterations( unsigned int value )
{
  this->itnlim = value;
}


void
lsqr::SetUpperLimitOnConditional( double value )
{
  this->conlim = value;
}


void
lsqr::SetStandardErrorEstimates( double * array )
{
  this->se = array;
}


void
lsqr::SetOutputStream( std::ostream & os )
{
  this->nout = &os;
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
Aprod1(unsigned int m, unsigned int n, const double * x, double * y ) const
{
#ifdef VERSION_FOR_TESTING
  CopyVector( n, y, this->wm );
  this->HouseholderTransformation(m,this->hy,this->wm);
  const unsigned int minmn = ( m > n ) ? n : m;
  ElementWiseProductVector( 0, minmn, this->d, this->wm, this->wn );
  AssignValueToVectorElements(m,n,0.0,this->wn);
  this->HouseholderTransformation(n,this->hz,this->wn);
  AccumulateVector( n, this->wn, x );
#endif
}


/**
 * computes x = x + A'*y without altering y,
 * where A is a test matrix of the form  A = Y*D*Z,
 * and the matrices D, Y, Z are represented by
 * the allocatable vectors d, hy, hz in this module. */
void lsqr::
Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const
{
#ifdef VERSION_FOR_TESTING
  CopyVector( n, y, this->wm );
  this->HouseholderTransformation(m,this->hy,this->wm);
  const unsigned int minmn = ( m > n ) ? n : m;
  ElementWiseProductVector( 0, minmn, this->d, this->wm, this->wn );
  AssignValueToVectorElements(m,n,0.0,this->wn);
  this->HouseholderTransformation(n,this->hz,this->wn);
  AccumulateVector( n, this->wn, x );
#endif
}



/** Simplified for this use from the BLAS version. */
void
lsqr::Scale( unsigned int n, double factor, double *x ) const
{
  double * xend = x + n;
  while( x != xend )
    {
    *x++ *= factor;
    }
}


/** Simplified for this use from the BLAS version. */
double
lsqr::Dnrm2( unsigned int n, const double *x ) const
{
  double magnitudeOfLargestElement = 0.0;

  double sumOfSquaresScaled = 1.0;

  for ( unsigned int i = 0; i < n; i++ )
    {
    if ( x[i] != 0.0 ) 
      {
      double dx = x[i];
      const double absxi = Abs(dx);

      if ( magnitudeOfLargestElement < absxi ) 
        {
        // rescale the sum to the range of the new element
        dx = magnitudeOfLargestElement / absxi;
        sumOfSquaresScaled = sumOfSquaresScaled * (dx * dx) + 1.0;
        magnitudeOfLargestElement = absxi;
        }
      else
        {
        // rescale the new element to the range of the sum
        dx = absxi / magnitudeOfLargestElement;
        sumOfSquaresScaled += dx * dx;
        }
      }
    }

  const double norm = magnitudeOfLargestElement * sqrt( sumOfSquaresScaled );

  return norm;
}


/** 
 *
 *  The array b must have size m
 *
 */
void lsqr::
Solve( unsigned int m, unsigned n, double * b, double * x )
{
  const double zero = 0.0;
  const double one = 1.0;

  if( this->nout )
    {
    (*this->nout) << "Enter LSQR " << std::endl;
    (*this->nout) << m << ", " << n << std::endl;
    (*this->nout) << this->damp << ", " << this->wantse << std::endl; 
    (*this->nout) << this->atol << ", " << this->conlim << std::endl; 
    (*this->nout) << this->btol << ", " << this->itnlim << std::endl; 
  
    }

  const bool damped = ( this->damp > zero );

  this->itn = 0;
  this->istop = 0;
  
  unsigned int nstop = 0;
  unsigned int maxdx = 0;

  double ctol = zero;
  if( this->conlim > zero )
    {
    ctol = one / this->conlim;
    }

  this->Anorm = zero;
  this->Acond = zero;

  double dnorm = zero;
  double dxmax = zero;
  double res2 = zero;
  double psi = zero;

  this->xnorm = zero;

  double xnorm1 = zero;
  double cs2 = -one;
  double sn2 = zero;
  double z = zero;

  double * u = new double[m];
  double * v = new double[n];
  double * w = new double[n];

  CopyVector( m, b, u );
  AssignScalarValueToVectorElements( 0, n, zero, v );
  AssignScalarValueToVectorElements( 0, n, zero, x );

  if( this->wantse )
    {
    AssignScalarValueToVectorElements( 0, n, zero, se );
    }

  double alpha = zero;

  double beta =  this->Dnrm2( m, u );

  if( beta > zero )
    {
    this->Scale( m, ( one / beta ), u );
    this->Aprod2( m, n, v, u );
    alpha = this->Dnrm2( n, v );
    }

  if( alpha > zero )
    {
    this->Scale( n, ( one / alpha ), v );
    CopyVector( n, v, w );
    }

  this->Arnorm = alpha * beta;

  double rhobar = alpha;
  double phibar = beta;
  double bnorm = beta;

  this->rnorm = beta;

  double test1 = 0.0;
  double test2 = 0.0;


  if ( this->Arnorm != zero )
    {

    if ( this->nout )
      {
      if ( damped )
        {
        (*this->nout) << " Itn       x(1)           Function"\
        "     Compatible   LS     Norm Abar Cond Abar alfa_opt" << std::endl;
        }
      else
        {
        (*this->nout) << " Itn       x(1)           Function"\
        "     Compatible   LS        Norm A    Cond A" << std::endl;
        }

      test1 = one;
      test2 = alpha / beta;

      this->nout->width(6);
      (*this->nout) << itn;
      this->nout->precision(9);
      this->nout->precision(17);
      (*this->nout) << x[0];
      this->nout->precision(2);
      this->nout->precision(10);
      (*this->nout) << rnorm;
      this->nout->precision(1);
      this->nout->precision(9);
      (*this->nout) << test1;
      (*this->nout) << test2;
      }
    }


  //
  //  Main itertation loop
  //
  do {

    this->itn++; 

    this->Scale( m, (-alpha), u );

    this->Aprod1( m, n, v, u );   //   u = A * v

    beta = this->Dnrm2( m, u );

    } while ( istop != 0); 

  //
  //  Main itertation loop
  //

  // Release locally allocated arrays.
  delete [] u;
  delete [] v;
  delete [] w;
}

