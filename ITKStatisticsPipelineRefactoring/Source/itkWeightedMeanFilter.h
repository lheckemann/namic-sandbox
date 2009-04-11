/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedMeanFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:59 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkWeightedMeanFilter_h
#define __itkWeightedMeanFilter_h

#include "itkMeanFilter.h"
#include "itkFunctionBase.h"
#include "itkDataObjectDecorator.h"

namespace itk { 
namespace Statistics {
  
/** \class WeightedMeanFilter
 * \brief Given a sample where each measurement vector has
 * associated weight value, this filter computes the sample mean
 *
 * To run this algorithm, you have plug in the target sample data 
 * using SetInput method and provides weight by an array or function.
 *. Then call the Update method to run the alogithm.
 * 
 * \sa MeanFilter 
 *
 */
template< class TSample >
class ITK_EXPORT WeightedMeanFilter : public MeanFilter< TSample >
{
public:
  /**Standard class typedefs. */
  typedef WeightedMeanFilter                      Self;
  typedef MeanFilter< TSample >                   Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  /**Standard Macros */
  itkTypeMacro(WeightedMeanFilter, MeanFilter);
  itkNewMacro(Self);

  /** Traits derived from the base class */
  itkSuperclassTraitMacro( SampleType )
  itkSuperclassTraitMacro( MeasurementType )
  itkSuperclassTraitMacro( MeasurementVectorType )
  itkSuperclassTraitMacro( MeasurementVectorSizeType )
  itkSuperclassTraitMacro( MeasurementVectorDecoratedType )
  itkSuperclassTraitMacro( OutputType )

  /** Array typedef for weights */
  typedef Array< double > WeightArrayType;

  /** Type of DataObjects to use for the weight array type */
  typedef SimpleDataObjectDecorator< WeightArrayType > InputWeightArrayObjectType;

  /** Method to set the input value of the weight array */
  itkSetDecoratedInputMacro( Weights, WeightArrayType, 1 );

  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightingFunctionType;

  /** Type of DataObjects to use for Weight function */
  typedef DataObjectDecorator< WeightingFunctionType > InputWeightingFunctionObjectType;

  /** Method to set the weighting function */
  itkSetDecoratedObjectInputMacro( WeightingFunction, WeightingFunctionType, 2 );

protected:
  WeightedMeanFilter();
  virtual ~WeightedMeanFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();

  // compute mean with weight array
  void ComputeMeanWithWeights();

  // compute mean using a weighting function
  void ComputeMeanWithWeightingFunction();

private:
  WeightedMeanFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedMeanFilter.txx"
#endif

#endif
