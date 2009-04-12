/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:43 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkCovarianceCalculator_h
#define __itkCovarianceCalculator_h

#include "itkCovarianceSampleFilter.h"
#include "itkVariableSizeMatrix.h"

namespace itk { 
namespace Statistics {
  
/** \class CovarianceCalculator
 * \brief Calculates the covariance matrix of the target sample data.
 *
 * If there is a mean vector provided by the SetMean method, this
 * calculator will do the caculation as follows:
 * Let \f$\Sigma\f$ denotes covariance matrix for the sample, then:
 * When \f$x_{i}\f$ is \f$i\f$th component of a measurement vector 
 * \f$\vec x\f$, \f$\mu_{i}\f$ is the \f$i\f$th componet of the \f$\vec\mu\f$, 
 * and the \f$\sigma_{ij}\f$ is the \f$ij\f$th componet \f$\Sigma\f$,
 * \f$\sigma_{ij} = (x_{i} - \mu_{i})(x_{j} - \mu_{j})\f$ 
 *
 * Without the plugged in mean vector, this calculator will perform
 * the single pass mean and covariance calculation algorithm.  
 * 
 * Recent API changes:
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained from the input sample.
 * Please use the function GetMeasurementVectorSize() to obtain the length. 
 * The mean output is an Array rather than a Vector. The covariance matrix is 
 * represented by a VariableSizeMatrix rather than a Matrix.
 */

template< class TSample >
class CovarianceCalculator :
    public CovarianceSampleFilter< TSample >
{
public:
  /** Standard class typedefs. */
  typedef CovarianceCalculator              Self;
  typedef CovarianceSampleFilter< TSample > Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  typedef typename Superclass::MatrixType            MatrixType;
  typedef typename Superclass::MeasurementVectorType MeasurementVectorType;
  /** Standard Macros */
  itkTypeMacro(CovarianceCalculator, CovarianceSampleFilter);
  itkNewMacro(Self);
  
  void SetInputSample( const TSample * sample ) 
    {
    Superclass::SetInput(sample);
    }

  typedef typename Superclass::MeasurementVectorType MeanType;
  void SetMean( MeanType* mean ) {}
  const MeasurementVectorType *GetMean()
    {
    m_MeanInternal = Superclass::GetMean();
    return &m_MeanInternal;
    }


  /** Typedef for Covariance output */
  typedef VariableSizeMatrix< double >               OutputType;

  OutputType* GetOutput()
    {
    m_CovarianceMatrixInternal = Superclass::GetCovarianceMatrix();
    return &m_CovarianceMatrixInternal;
    }

protected:
  CovarianceCalculator(){}
  virtual ~CovarianceCalculator(){};

private:
  MeasurementVectorType m_MeanInternal;
  MatrixType m_CovarianceMatrixInternal;
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#endif
