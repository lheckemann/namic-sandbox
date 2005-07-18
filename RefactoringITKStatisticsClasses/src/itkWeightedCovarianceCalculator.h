/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceCalculator.h,v $
  Language:  C++
  Date:      $Date: 2003/12/09 16:53:08 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWeightedCovarianceCalculator_h
#define __itkWeightedCovarianceCalculator_h

#include "itkArray.h"
#include "itkVariableSizeMatrix.h"
#include "itkSampleAlgorithmBase.h"

namespace itk{ 
namespace Statistics{
  
/** \class WeightedCovarianceCalculator
 * \brief Calculates the covariance matrix of the target sample data
 * where each measurement vector has an associated weight value
 *
 * \sa CovarianceCalculator SampleAlgorithmBase
 */

template< class TSample >
class WeightedCovarianceCalculator :
      public SampleAlgorithmBase< TSample >
{
public:
  /** Standard class typedefs. */
  typedef WeightedCovarianceCalculator Self;
  typedef SampleAlgorithmBase< TSample > Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard Macros */
  itkTypeMacro(WeightedCovarianceCalculator, SampleAlgorithmBase);
  itkNewMacro(Self) ;

  /** Length of a measurement vector */
  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;
  
  /** Measurement vector typedef */
  typedef typename TSample::MeasurementVectorType MeasurementVectorType ;
  
  /** DEPRECATED: The static const macro will be deprecated in a future version.
   * Please use GetMeasurementVectorSize() instead. This constant returns the 
   * length of a measurement vector for FixedArrays, Vectors and other fixed 
   * containers and zero for dynamically resizable containers. The true value for 
   * dynamically resizable containers will be obtained from the 
   * GetMeasurementVectorSize() call. 
   */
  itkStaticConstMacro(MeasurementVectorSize, unsigned int,
     MeasurementVectorTraits< MeasurementVectorType >::MeasurementVectorLength);
 
  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightFunctionType ;

  /** Typedef for the mean output */
  typedef typename MeasurementVectorTraits< MeasurementVectorType >::MeanType MeanType;

  /** Typedef for Covariance output */
  typedef typename MeasurementVectorTraits< MeasurementVectorType >::RealMatrixType OutputType;
  
  /** Array typedef for weights */
  typedef Array< double > WeightArrayType ;

  /** Sets the weights using an array */
  void SetWeights(WeightArrayType* array) ;

  /** Gets the weights array */
  WeightArrayType* GetWeights() ;

  /** Sets the weights using an function the function should have a method, 
   * Evaluate(MeasurementVectorType&).  */
  void SetWeightFunction(WeightFunctionType* func) ;

  /** Gets the weight function */
  WeightFunctionType* GetWeightFunction() ;
  
  /** Sets the mean (input) */
  void SetMean(MeanType* mean) ;

  /** Gets the mean */
  MeanType* GetMean() ;

  /** Returns the covariance matrix of the target sample data */ 
  OutputType* GetOutput() ;

protected:
  WeightedCovarianceCalculator() ;
  virtual ~WeightedCovarianceCalculator() ;
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Calculates the covariance and save it */
  void GenerateData() ;

  void ComputeCovarianceWithGivenMean() ;
  
  void ComputeCovarianceWithoutGivenMean() ;

private:
  OutputType* m_Output ;
  MeanType* m_Mean ;
  MeanType* m_InternalMean ;
  WeightArrayType* m_Weights ;
  WeightFunctionType* m_WeightFunction ;
} ; // end of class
    
  } // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedCovarianceCalculator.txx"
#endif

#endif

