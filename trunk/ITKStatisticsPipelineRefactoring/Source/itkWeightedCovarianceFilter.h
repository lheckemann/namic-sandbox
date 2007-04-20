/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkWeightedCovarianceFilter_h
#define __itkWeightedCovarianceFilter_h

#include "itkProcessObject.h"
#include "itkFunctionBase.h"

#include "itkArray.h"
#include "itkVariableSizeMatrix.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk { 
namespace Statistics {
  
/** \class WeightedCovarianceFilter
 * \brief Calculates the covariance matrix of the target sample data.
 *  where each measurement vector has an associated weight value
 *  
 * Weight values can be specified in two ways: using a weighting function
 * or an array containing weight values. If none of these two is specified, 
 * the covariance matrix is generated with equal weights.  
 *
 * \sa CovarianceFilter
 *
 */

template< class TSample >
class WeightedCovarianceFilter :
    public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef WeightedCovarianceFilter        Self;
  typedef ProcessObject                   Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  typedef TSample                         SampleType;

  /** Standard Macros */
  itkTypeMacro(WeightedCovarianceFilter, ProcessObject);
  itkNewMacro(Self);
  
  /** Length of a measurement vector */
  typedef typename TSample::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Measurement vector type */
  typedef typename TSample::MeasurementVectorType MeasurementVectorType;
  
  /** Typedef for WeightedCovariance output */
  typedef VariableSizeMatrix< double >               MatrixType;

  /** Method to set/get the sample */
  void SetInput( const SampleType * sample );
  const SampleType *  GetInput() const;

  /** DataObject pointer */
  typedef DataObject::Pointer DataObjectPointer;

  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightFunctionType;

  /** VariableSizeMatrix is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MatrixType >  MatrixDecoratedType;

  /** MeasurementVector is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MeasurementVectorType >  MeasurementVectorDecoratedType;

  /** Return the covariance matrix */
  const MatrixType GetWeightedCovarianceMatrix() const;
  const MatrixDecoratedType* GetWeightedCovarianceMatrixOutput() const;

  /** Return the mean vector */
  const MeasurementVectorType GetMean() const;
  const MeasurementVectorDecoratedType* GetMeanOutput() const;

  /** Array typedef for weights */
  typedef Array< double > WeightArrayType;

  /** Sets the weights using an array */
  void SetWeights(WeightArrayType array);

  /** Gets the weights array */
  WeightArrayType GetWeights();

  /** Sets the weights using an function the function should have a method, 
   * Evaluate(MeasurementVectorType&).  */
  void SetWeightFunction(WeightFunctionType* func);

  /** Gets the weight function */
  WeightFunctionType* GetWeightFunction();
 
protected:
  WeightedCovarianceFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  WeightedCovarianceFilter();
  virtual ~WeightedCovarianceFilter() {} 
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual DataObjectPointer MakeOutput(unsigned int idx);

  void GenerateData();

  /** Compute covariance matrix with weights computed from a function */
  void ComputeCovarianceMatrixWithWeightingFunction();

  /** Compute covariance matrix with weights specified in an array */
  void ComputeCovarianceMatrixWithWeights();

private:
  WeightArrayType     m_Weights;
  WeightFunctionType* m_WeightFunction;
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedCovarianceFilter.txx"
#endif

#endif
