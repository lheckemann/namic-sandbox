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
#ifndef __itk_lsqr_h
#define __itk_lsqr_h

#include <iostream>


class lsqr 
{
public:

  lsqr();
  ~lsqr();

  /** Householder Transformation: reflects the vector "x" across the
   * hyperplane whose normal is defined by vector "z". The dimension of
   * the hyperspace is given by "n". */
  void HouseholderTransformation(unsigned int n, const double * z, double * x ) const;

  /**
   * computes x = x + A'*y without altering y,
   * where A is a test matrix of the form  A = Y*D*Z,
   * and the matrices D, Y, Z are represented by
   * the allocatable vectors d, hy, hz in this module.
   * The size of the vector x is n.
   * The size of the vector y is m. */
  void Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const;
  
  /**
   * returns sqrt( a**2 + b**2 )
   * with precautions to avoid overflow.
   */
  double D2Norm( double a, double b ) const;

  
  /**  A logical variable to say if the array se(*)
   *   of standard error estimates should be computed.
   *   If m > n  or  damp > 0,  the system is
   *   overdetermined and the standard errors may be
   *   useful.  (See the first LSQR reference.)
   *   Otherwise (m <= n  and  damp = 0) they do not
   *   mean much.  Some time and storage can be saved
   *   by setting wantse = .false. and using any
   *   convenient array for se(*), which won't be
   *   touched.
   */
  void SetStandardErrorEstimatesFlag( bool );

  /** An estimate of the relative error in the data
   *  defining the matrix A.  For example, if A is
   *  accurate to about 6 digits, set atol = 1.0e-6.
   */
  void SetToleranceA( double );

  /** An estimate of the relative error in the data 
   *  defining the rhs b.  For example, if b is     
   *  accurate to about 6 digits, set btol = 1.0e-6.
   */
  void SetToleranceB( double );

  /** An upper limit on cond(Abar), the apparent
   *  condition number of the matrix Abar.
   *  Iterations will be terminated if a computed
   *  estimate of cond(Abar) exceeds conlim.
   *  This is intended to prevent certain small or
   *  zero singular values of A or Abar from
   *  coming into effect and causing unwanted growth
   *  in the computed solution.
   *
   *  conlim and damp may be used separately or
   *  together to regularize ill-conditioned systems.
   *
   *  Normally, conlim should be in the range
   *  1000 to 1/eps.
   *  Suggested value:
   *  conlim = 1/(100*eps)  for compatible systems,
   *  conlim = 1/(10*sqrt(eps)) for least squares.
   *
   * Note: Any or all of atol, btol, conlim may be set to zero.
   * The effect will be the same as the values eps, eps, 1/eps.
   *
   */
  void SetUpperLimitOnConditional( double );

  /**  the relative precision of floating-point arithmetic.
   *   On most machines, eps is about 1.0e-7 and 1.0e-16
   *   in single and double precision respectively.
   *   We expect eps to be about 1e-16 always.
   */
  void SetEpsilon( double );

  /**  An upper limit on the number of iterations.
   *   Suggested value:
   *   itnlim = n/2   for well-conditioned systems
   *                  with clustered singular values,
   *   itnlim = 4*n   otherwise.
   */
  void SetMaximumNumberOfIterations( unsigned int );

  /** 
   * If provided, a summary will be printed out to this stream during
   * the execution of the Solve function.
   */
  void SetOutputStream( std::ostream & os );

  /**
   *    Execute the solver
   * 
   *    solves Ax = b or min ||Ax - b|| with or without damping,
   *
   */
  void Solve( unsigned int m, unsigned n, double * b, double damp, bool wantse,
         double * x, double * se,
         unsigned int istop, unsigned int itn
         );

private:
  double    *  hy;
  double    *  hz;
  double    *  d;
  double    *  wm;  // work vector
  double    *  wn;  // work vector

  double Anorm;
  double Acond;
  double rnorm;
  double Arnorm;
  double xnorm;

  double atol;
  double btol;
  double conlim;

  double eps;

  unsigned int itnlim;

  std::ostream * nout;

  bool   wantse;
};

#endif 
