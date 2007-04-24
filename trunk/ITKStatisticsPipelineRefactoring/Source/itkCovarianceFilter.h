/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkCovarianceFilter_h
#define __itkCovarianceFilter_h

#include "itkProcessObject.h"

#include "itkArray.h"
#include "itkVariableSizeMatrix.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk { 
namespace Statistics {
  
/** \class CovarianceFilter
 * \brief Calculates the covariance matrix of the target sample data.
 *
 * The filter calculates first the sample mean and use it in the covariance
 * calculation. The covariance is computed as follows
 * Let \f$\Sigma\f$ denotes covariance matrix for the sample, then:
 * When \f$x_{i}\f$ is \f$i\f$th component of a measurement vector 
 * \f$\vec x\f$, \f$\mu_{i}\f$ is the \f$i\f$th componet of the \f$\vec\mu\f$, 
 * and the \f$\sigma_{ij}\f$ is the \f$ij\f$th componet \f$\Sigma\f$,
 * \f$\sigma_{ij} = (x_{i} - \mu_{i})(x_{j} - \mu_{j})\f$ 
 *
 * Without the plugged in mean vector, this calculator will perform
 * the single pass mean and covariance calculation algorithm.  
 * 
 */

template< class TSample >
class CovarianceFilter :
    public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef CovarianceFilter                Self;
  typedef ProcessObject                   Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  typedef TSample                         SampleType;

  /** Standard Macros */
  itkTypeMacro(CovarianceFilter, ProcessObject);
  itkNewMacro(Self);
  
  /** Length of a measurement vector */
  typedef typename TSample::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Measurement vector type */
  typedef typename TSample::MeasurementVectorType MeasurementVectorType;
  
  /** Typedef for Covariance output */
  typedef VariableSizeMatrix< double >               MatrixType;

  /** Method to set/get the sample */
  void SetInput( const SampleType * sample );
  const SampleType *  GetInput() const;

  /** VariableSizeMatrix is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MatrixType >  MatrixDecoratedType;

  /** MeasurementVector is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MeasurementVectorType >  MeasurementVectorDecoratedType;

  /** Return the covariance matrix */
  const MatrixType GetCovarianceMatrix() const;
  const MatrixDecoratedType* GetCovarianceMatrixOutput() const;

  /** Return the mean vector */
  const MeasurementVectorType GetMean() const;
  const MeasurementVectorDecoratedType* GetMeanOutput() const;

protected:
  CovarianceFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  CovarianceFilter();
  virtual ~CovarianceFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** DataObject pointer */
  typedef DataObject::Pointer DataObjectPointer;

  virtual DataObjectPointer MakeOutput(unsigned int idx);

  void GenerateData();

private:
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCovarianceFilter.txx"
#endif

#endif
