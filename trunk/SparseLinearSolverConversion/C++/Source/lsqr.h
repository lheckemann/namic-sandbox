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
  
private:
  double    *  hy;
  double    *  hz;
  double    *  d;
  double    *  wm;  // work vector
  double    *  wn;  // work vector
};

#endif 
