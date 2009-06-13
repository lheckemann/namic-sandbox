/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDensityFunction.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:47 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGaussianDensityFunction_h
#define __itkGaussianDensityFunction_h

#include "itkGaussianMembershipFunction.h"

namespace itk { 
namespace Statistics {

/** \class GaussianDensityFunction
 * \brief GaussianDensityFunction class represents Gaussian Density Function.
 *
 * This class keeps parameter to define Gaussian Density Function  and has
 * method to return the probability density 
 * of an instance (pattern) .  
 * If the all element of the covariance matrix is zero the "usual" density 
 * calculations ignored. if the measurement vector to be evaluated is equal to
 * the mean, then the Evaluate method will return maximum value of
 * double and return 0 for others 
 * 
 * <b>Recent API changes:</b>
 * The static const macro to get the length of a measurement vector,
 * \c MeasurementVectorSize  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained at run time from the
 * sample set as input. Please use the function 
 * GetMeasurementVectorSize() to get the length. The typedef for the Mean has 
 * changed from FixedArray to Array. The typedef for the covariance matrix
 * has changed from Matrix to VariableSizeMatrix.
 *
 */

template< class TMeasurementVector >
class ITK_EXPORT GaussianDensityFunction :
    public GaussianMembershipFunction< TMeasurementVector >
{
public:
  /** Standard class typedefs */
  typedef GaussianDensityFunction                          Self;
  typedef GaussianMembershipFunction< TMeasurementVector > Superclass;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;

  /** Strandard macros */
  itkTypeMacro(GaussianDensityFunction, GaussianMembershipFunction);
  itkNewMacro(Self);
  
  /** Typedef alias for the measurement vectors */
  typedef TMeasurementVector MeasurementVectorType;

  /** Type of the covariance matrix */
  typedef typename Superclass::CovarianceType CovarianceType;

  /** Type of the mean vector */
  typedef typename Superclass::MeanType       MeanType;
  
  /** Sets the mean */
  void SetMean( const MeanType * mean )
    {
    Superclass::SetMean(*mean);
    }
  
  /** Sets the covariance */
  void SetCovariance(const CovarianceType* cov)
    {
    Superclass::SetCovariance(*cov);
    } 

protected:
  GaussianDensityFunction(void) {}
  virtual ~GaussianDensityFunction(void) {}

private:
};

} // end of namespace Statistics
} // end namespace itk

#endif
