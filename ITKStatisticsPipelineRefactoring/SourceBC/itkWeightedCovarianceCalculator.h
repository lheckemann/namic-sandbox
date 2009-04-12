/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 19:29:54 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWeightedCovarianceCalculator_h
#define __itkWeightedCovarianceCalculator_h

#include "itkWeightedCovarianceSampleFilter.h"

namespace itk { 
namespace Statistics {
  
/** \class WeightedCovarianceCalculator
 * \brief Calculates the covariance matrix of the target sample data
 * where each measurement vector has an associated weight value
 *
 * Recent API changes:
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained from the input sample.
 * Please use the function GetMeasurementVectorSize() to obtain the length. 
 * The mean output is an Array rather than a Vector. The covariance matrix is 
 * represented by a VariableSizeMatrix rather than a Matrix.
  * \sa CovarianceCalculator SampleAlgorithmBase
 */

template< class TSample >
class WeightedCovarianceCalculator :
      public WeightedCovarianceSampleFilter< TSample >
{
public:
  /** Standard class typedefs. */
  typedef WeightedCovarianceCalculator              Self;
  typedef WeightedCovarianceSampleFilter< TSample > Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  /** Standard Macros */
  itkTypeMacro(WeightedCovarianceCalculator, WeightedCovarianceSampleFilter);
  itkNewMacro(Self);

  typedef typename Superclass::MatrixType            MatrixType;

  void SetInputSample( const TSample * sample ) 
    {
    Superclass::SetInput(sample);
    }

  typedef typename Superclass::WeightArrayType WeightArrayType;
  void SetWeights(WeightArrayType * weights)
    {
    Superclass::SetWeights( *weights );
    }

  typedef typename Superclass::WeightingFunctionType WeightFunctionType;
  void SetWeightFunction (WeightFunctionType* func)
    {
    Superclass::SetWeightingFunction(func);
    }

  typedef typename Superclass::MeasurementVectorType MeanType;
  typedef typename Superclass::MeasurementVectorType MeasurementVectorType;
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
  WeightedCovarianceCalculator(){};
  virtual ~WeightedCovarianceCalculator(){};

private:
  MeasurementVectorType m_MeanInternal;
  MatrixType m_CovarianceMatrixInternal;
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#endif
