/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedMeanCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 19:29:55 $
  Version:   $Revision: 1.8 $

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

#include "itkWeightedMeanSampleFilter.h"

namespace itk { 
namespace Statistics {
  
/** \class WeightedMeanCalculator
 * \brief calculates sample mean where each measurement vector has
 * associated weight value
 *
 * To run this algorithm, you have plug in the target sample data 
 * using SetInputSample method and provides weight by an array or function.
 *. Then call the Update method to run the alogithm.
 * 
 * Recent API changes:
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained from the input sample.
 * Please use the function GetMeasurementVectorSize() to obtain the length. 
 * The mean output is an Array rather than a Vector.

 * \sa MeanCalculator SampleAlgorithmBase
 */

template< class TSample >
class WeightedMeanCalculator :
    public WeightedMeanSampleFilter< TSample >
{
public:
  /**Standard class typedefs. */
  typedef WeightedMeanCalculator              Self;
  typedef WeightedMeanSampleFilter< TSample > Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /**Standard Macros */
  itkTypeMacro(WeightedMeanCalculator, WeightedMeanSampleFilter);
  itkNewMacro(Self);
  
  /** Typedef for the mean output */
  typedef typename Superclass::MeasurementVectorType   OutputType;

  /** Returns the mean vector */
  OutputType* GetOutput()
    {
    m_MeanInternal = Superclass::GetMean();
    return &m_MeanInternal;
    }

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

protected:
  WeightedMeanCalculator(){};
  virtual ~WeightedMeanCalculator() {}

private:
  OutputType m_MeanInternal;

}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#endif
