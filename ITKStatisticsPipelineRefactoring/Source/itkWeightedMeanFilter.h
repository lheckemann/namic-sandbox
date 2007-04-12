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

#include "itkProcessObject.h"
#include "itkArray.h"
#include "itkFunctionBase.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itkDataObject.h"

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
 * Recent API changes:
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. It is now obtained from the input sample.
 * Please use the function GetMeasurementVectorSize() to obtain the length. 
 * The mean output is an Array rather than a Vector.

 * \sa MeanFilter 
 
*/

template< class TSample >
class ITK_EXPORT WeightedMeanFilter : public ProcessObject
{
public:
  /**Standard class typedefs. */
  typedef WeightedMeanFilter                      Self;
  typedef ProcessObject                           Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;
  typedef TSample                                 SampleType;

  /**Standard Macros */
  itkTypeMacro(WeightedMeanFilter, ProcessObject);
  itkNewMacro(Self);

  /** Length of a measurement vector */
  typedef   unsigned int                              MeasurementVectorSizeType;
  typedef   typename TSample::MeasurementVectorType   MeasurementVectorType;
  typedef   typename TSample::MeasurementType         MeasurementType;

  /** Method to set/get the sample */
  void SetInput( const SampleType * sample );
  const SampleType *  GetInput() const;

  /** Array typedef for weights */
  typedef Array< double > WeightArrayType;

  /** Set/Get the weights using an array */
  itkSetMacro( Weights,WeightArrayType );
  itkGetMacro( Weights,WeightArrayType )

  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightFunctionType;

  /** Set the weights using an function
   * the function should have a method, 
   * Evaluate(MeasurementVectorType&) */
  itkSetObjectMacro(WeightFunction, WeightFunctionType );
  const WeightFunctionType * GetWeightFunction();

  /** DataObject pointer */
  typedef DataObject::Pointer DataObjectPointer;

  /** MeasurementVector is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MeasurementVectorType >  MeasurementVectorDecoratedType;

  /** Get the mean measurement vector */
  const MeasurementVectorDecoratedType * GetOutput() const;

protected:
  WeightedMeanFilter();
  virtual ~WeightedMeanFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual DataObjectPointer MakeOutput(unsigned int idx);

  void GenerateData();

private:
  WeightedMeanFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  WeightArrayType                      m_Weights;
  WeightFunctionType*                  m_WeightFunction;
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedMeanFilter.txx"
#endif

#endif
