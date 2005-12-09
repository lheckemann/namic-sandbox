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
#ifndef __itkChiSquareDistribution_h
#define __itkChiSquareDistribution_h

#include "itkProbabilityDistribution.h"
#include "itkNumericTraits.h"

namespace itk{ 
namespace Statistics{

/** \class ChiSquareDistribution
 * \brief ChiSquareDistribution class defines the interface for a univariate Chi-Square distribution (pdfs, cdfs, etc.).
 *
 * ChiSquareDistribution provides access to the probability density
 * function (pdf), the cumulative distribution function (cdf), and the
 * inverse cumulative distribution function for a Chi-Square distribution.
 *
 * The EvaluatePDF(), EvaluateCDF, EvaluateInverseCDF() methods are
 * all virtual, allowing algorithms to be written with an abstract
 * interface to a distribution (with said distribution provided to the
 * algorithm at run-time).  Static methods, not requiring an instance
 * of the distribution, are also provided.  The static methods allow
 * for optimized access to distributions when the distribution is
 * known a priori to the algorithm.
 *
 * ChiSquareDistributions are univariate.  Multivariate versions may
 * be provided under a separate superclass (since the parameters to the
 * pdf and cdf would have to be vectors not scalars).
 *
 * ChiSquareDistributions can be used for Chi-Square tests.
 *
 * \note This work is part of the National Alliance for Medical Image
 * Computing (NAMIC), funded by the National Institutes of Health
 * through the NIH Roadmap for Medical Research, Grant U54 EB005149.
 * Information on the National Centers for Biomedical Computing
 * can be obtained from http://nihroadmap.nih.gov/bioinformatics.  
 */
class ITK_EXPORT ChiSquareDistribution :
    public ProbabilityDistribution
{
public:
  /** Standard class typedefs */
  typedef ChiSquareDistribution Self;
  typedef ProbabilityDistribution Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Strandard macros */
  itkTypeMacro(ChiSquareDistribution, ProbabilityDistribution);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Evaluate the probability density function (pdf) */
  virtual double EvaluatePDF(double x) const;

  /** Evaluate the cumulative distribution function (cdf) */
  virtual double EvaluateCDF(double x) const;

  /** Evaluate the inverse cumulative distribution function (inverse
   * cdf).  Parameter p must be between 0.0 and 1.0 */
  virtual double EvaluateInverseCDF(double p) const;

  /** Set the number of degrees of freedom in the chi square distribution.
   * Defaults to 1 */
  itkSetClampMacro(DegreesOfFreedom, long, 1, NumericTraits<long>::max());

  /** Get the number of degrees of freedom in the chi-square
   * distribution. Defaults to 1 */
  itkGetConstMacro(DegreesOfFreedom, long);

  /** Get the mean of the distribution. */
  double GetMean() const;

  /** Get the variance of the distribution. */ 
  double GetVariance() const;
    
  /** Static method to evaluate the probability density function (pdf)
   * of a Chi-Square with a specified number of degrees of freedom. The
   * static method provides optimized access without requiring an
   * instance of the class. */
  static double PDF(double x, long degreesOfFreedom);
  
  /** Static method to evaluate the cumulative distribution function
   * (cdf) of a Chi-Square with a specified number of degrees of
   * freedom. The static method provides optimized access without
   * requiring an instance of the class.
   *
   * This is based on Abramowitz and Stegun 26.7.1. Accuracy is
   * approximately 10^-14.
   */
  static double CDF(double x, long degreesOfFreedom);

  /** Static method to evaluate the inverse cumulative distribution
   * function of a Chi-Square with a specified number of degrees of
   * freedom.  The static method provides optimized access without
   * requiring an instance of the class. Parameter p must be between
   * 0.0 and 1.0.
   *
   * This is based on Abramowitz and Stegun 26.7.5 followed by a few
   * Newton iterations to improve the precision at low degrees of
   * freedom. Accuracy is approximately 10^-10.
   **/
  static double InverseCDF(double p, long degreesOfFreedom);

protected:
  ChiSquareDistribution(void) : m_DegreesOfFreedom(1) {}
  virtual ~ChiSquareDistribution(void) {}

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  long m_DegreesOfFreedom;
  
} ; // end of class

} // end of namespace Statistics
} // end namespace itk

#endif





