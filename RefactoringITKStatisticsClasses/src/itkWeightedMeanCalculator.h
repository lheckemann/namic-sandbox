/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedMeanCalculator.h,v $
  Language:  C++
  Date:      $Date: 2003/12/09 16:53:08 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkWeightedMeanCalculator_h
#define __itkWeightedMeanCalculator_h

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "itkArray.h"
#include "itkFunctionBase.h"
#include "itkSampleAlgorithmBase.h"

namespace itk{ 
namespace Statistics{
  
/** \class WeightedMeanCalculator
 * \brief calculates sample mean where each measurement vector has
 * associated weight value
 *
 * To run this algorithm, you have plug in the target sample data 
 * using SetInputSample method and provides weight by an array or function.
 *. Then call the Update method to run the alogithm.
 *
 * \sa MeanCalculator SampleAlgorithmBase
 */

template< class TSample >
class WeightedMeanCalculator :
    public SampleAlgorithmBase< TSample >
{
public:
  /**Standard class typedefs. */
  typedef WeightedMeanCalculator Self;
  typedef SampleAlgorithmBase< TSample > Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /**Standard Macros */
  itkTypeMacro(WeightedMeanCalculator, SampleAlgorithmBase);
  itkNewMacro(Self) ;
  
  /** Length of a measurement vector */
  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;
  
  /** REMOVED: THE StaticConstMacro for this method has been removed to 
   * allow the measurement vector length to be specified at run time.
   *
   * Please use the Get macros to access the MeasurementVectorLength
   * instead. Note that GetMeasurementVectorSize() will return 0 unless
   * you have plugged in the input sample using the SetInputSample() 
   * method
   *
   * NOTE: This means that you will no longer be able to get the 
   * MeasurementVectorLength as a static const member any more.
   */
  //itkStaticConstMacro(MeasurementVectorSize, unsigned int,
  //                    TSample::MeasurementVectorSize) ;
  

  typedef typename TSample::MeasurementVectorType MeasurementVectorType ;

  /** Typedef for the mean output */
  typedef typename MeasurementVectorTraits< 
                    MeasurementVectorType >::MeanType OutputType;
  
  /** Array typedef for weights */
  typedef Array< double > WeightArrayType ;

  /** Sets the weights using an array */
  void SetWeights(WeightArrayType* array) ;
  
  /** Gets the weights array */
  WeightArrayType* GetWeights() ;

  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightFunctionType ;

  /** Sets the wiehts using an function
   * the function should have a method, 
   * Evaluate(MeasurementVectorType&) */
  void SetWeightFunction(WeightFunctionType* func) ;

  /** Gets the weight function */
  WeightFunctionType* GetWeightFunction() ;

  /** Returns the mean vector as the result */
  OutputType* GetOutput() ;

protected:
  WeightedMeanCalculator() ;
  virtual ~WeightedMeanCalculator() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Calculates the mean and save it */
  void GenerateData() ;

private:
  WeightArrayType* m_Weights ;
  WeightFunctionType* m_WeightFunction ;
  OutputType m_Output ;
} ; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedMeanCalculator.txx"
#endif

#endif

