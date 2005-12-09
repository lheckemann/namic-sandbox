/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkChiSquareDistribution.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkChiSquareDistribution.h"
#include "itkGaussianDistribution.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_erf.h"

extern "C" double dgami_(double *a, double *x);
extern "C" double dgamma_(double *x);

namespace itk{ 
namespace Statistics{

double
ChiSquareDistribution
::PDF(double x, long degreesOfFreedom)
{
  double dof = static_cast<double>(degreesOfFreedom);
  double dofon2 = 0.5*dof;
  double pdf = 0.0;

  if (x >= 0.0)
    {
    pdf = exp(-0.5*x) * pow(x, dofon2 - 1.0)
      / (pow(2.0, dofon2) * dgamma_(&dofon2));
    }
  
  return pdf;
}

double
ChiSquareDistribution
::CDF(double x, long degreesOfFreedom)
{
  // Based on Abramowitz and Stegun 26.4.19 which relates the
  // cumulative of the chi-square to incomplete (and complete) gamma
  // function.
  if (x < 0)
    {
    return 0.0;
    }

  double dofon2 = 0.5*degreesOfFreedom;
  double xon2 = 0.5*x;
  
  return dgami_(&dofon2, &xon2) / dgamma_(&dofon2);
}


double
ChiSquareDistribution
::InverseCDF(double p, long degreesOfFreedom)
{
  if (p <= 0.0)
    {
    return itk::NumericTraits<double>::Zero;
    }
  else if (p >= 1.0)
    {
    return itk::NumericTraits<double>::max();
    }

  double x;
  double dof;
  double nx;

  // Based on Abramowitz and Stegun 26.4.17
  dof = static_cast<double>(degreesOfFreedom);
  nx = GaussianDistribution::InverseCDF(p);

  double f = 2.0 / (9.0*dof);
  x = dof*pow(1.0 - f + nx*sqrt(f), 3.0);


  // The approximation above is only accurate for large degrees of
  // freedom. We'll improve the approximation by a few Newton iterations.
  //
  //   0 iterations, error = 10^-1  at 1 degree of freedom
  //   3 iterations, error = 10^-11 at 1 degree of freedom 
  //  10 iterations, erorr = 10^-13 at 1 degree of freedom
  //  20 iterations, erorr = 10^-13 at 1 degree of freedom
  //
  //   0 iterations, error = 10^-1  at 11 degrees of freedom
  //   3 iterations, error = 10^-8  at 11 degrees of freedom 
  //  10 iterations, erorr = 10^-13 at 11 degrees of freedom
  //  20 iterations, erorr = 10^-13 at 11 degrees of freedom
  //
  //   0 iterations, error = 10^-1  at 100 degrees of freedom
  //   3 iterations, error = 10^-9  at 100 degrees of freedom 
  //  10 iterations, erorr = 10^-10 at 100 degrees of freedom
  //  20 iterations, erorr = 10^-9  at 100 degrees of freedom
  
  
  // We are trying to find the zero of
  //
  // f(x) = p - chisquarecdf(x) = 0;
  //
  // So,
  //
  // x(n+1) = x(n) - f(x(n)) / f'(x(n))
  //        = x(n) + (p - chisquarecdf(x)) / chisquarepdf(x)
  //
  // Note that f'(x) = - chisquarepdf(x)
  //
  double delta;
  for (unsigned int newt = 0; newt < 10; ++newt)
    {
    delta = (p - ChiSquareDistribution::CDF(x, degreesOfFreedom))
      / ChiSquareDistribution::PDF(x, degreesOfFreedom);
    x += delta;
    }

  return x;
}




double
ChiSquareDistribution
::EvaluatePDF(double x) const
{
  return ChiSquareDistribution::PDF(x, m_DegreesOfFreedom);
}

double
ChiSquareDistribution
::EvaluateCDF(double x) const
{
  return ChiSquareDistribution::CDF(x, m_DegreesOfFreedom);
}

double
ChiSquareDistribution
::EvaluateInverseCDF(double p) const
{
  return ChiSquareDistribution::InverseCDF(p, m_DegreesOfFreedom);
}


double
ChiSquareDistribution
::GetMean() const
{
  return static_cast<double>(m_DegreesOfFreedom);
}

double
ChiSquareDistribution
::GetVariance() const
{
  return 2.0*static_cast<double>(m_DegreesOfFreedom);
}

void  
ChiSquareDistribution
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Degrees of freedom: "  << m_DegreesOfFreedom << std::endl;
}

} // end of namespace Statistics
} // end namespace itk
