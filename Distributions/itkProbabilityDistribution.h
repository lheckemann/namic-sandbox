/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkProbabilityDistribution.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkProbabilityDistribution_h
#define __itkProbabilityDistribution_h

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk{ 
namespace Statistics{

/** \class ProbabilityDistribution
 * \brief ProbabilityDistribution class defines common interface for statistical distributions (pdfs, cdfs, etc.).
 *
 * ProbabilityDistribution defines a common interface for parameteric
 * and non-parametric distributions.  ProbabilityDistribution provides
 * access to the probability density function (pdf), the cumulative
 * distribution function (cdf), and the inverse cumulative
 * distribution function.
 *
 * ProbabilityDistribution also defines an abstract interface for
 * setting parameters of distribution (mean/variance for a Gaussian,
 * degrees of freedom for Student-t, etc.).
 *
 * Note that nonparametric subclasses of ProbabilityDistribution are
 * possible.  For instance, a nonparametric implementation may use a
 * histogram or kernel density function to model the distribution.
 *
 * The EvaluatePDF(), EvaluateCDF, EvaluateInverseCDF() methods are
 * all virtual, allowing algorithms to be written with an abstract
 * interface to a distribution (with said distribution provided to the
 * algorithm at run-time).  Static methods, not requiring an instance
 * of the distribution, are also allowed.  The static methods allow
 * for optimized access to distributions when the distribution is
 * known a priori to the algorithm.
 *
 * ProbabilityDistributions are univariate.  Multivariate versions may
 * be provided under a separate superclass (since the parameters to the
 * pdf and cdf would have to be vectors not scalars). Perhaps this
 * class will be named MultivariateProbabilityDistribution.
 *
 * ProbabilityDistributions can be used for standard statistical
 * tests: Z-scores, t-tests, chi-squared tests, F-tests, etc.
 *
 * \note This work is part of the National Alliance for Medical Image
 * Computing (NAMIC), funded by the National Institutes of Health
 * through the NIH Roadmap for Medical Research, Grant U54 EB005149.
 * Information on the National Centers for Biomedical Computing
 * can be obtained from http://nihroadmap.nih.gov/bioinformatics.  
 */
class ITK_EXPORT ProbabilityDistribution :
    public Object
{
public:
  /** Standard class typedefs */
  typedef ProbabilityDistribution Self;
  typedef Object Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkTypeMacro(ProbabilityDistribution, Object);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Evaluate the probability density function (pdf) */
  virtual double EvaluatePDF(double x) const;

  /** Evaluate the cumulative distribution function (cdf) */
  virtual double EvaluateCDF(double x) const;

  /** Evaluate the inverse cumulative distribution function (inverse
   * cdf).  Parameter p must be between 0.0 and 1.0 */
  virtual double EvaluateInverseCDF(double p) const;

  /** Does this distribution have a mean? */
  virtual bool HasMean() const {return true;}

  /** Does this distribution have a variance? */
  virtual bool HasVariance() const {return true;}

  /** Get the mean of the distribution.  If the mean does not exist,
   * then quiet_NaN is returned. */
  virtual double GetMean() const { return 0.0; }

  /** Get the variance of the distribution. If the variance does not
   * exist, then quiet_NaN is returned. */
  virtual double GetVariance() const { return 0.0; }
  
protected:
  ProbabilityDistribution(void) {}
  virtual ~ProbabilityDistribution(void) {}

private:
} ; // end of class

} // end of namespace Statistics
} // end namespace itk

#endif





