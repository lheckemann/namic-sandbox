/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleClassifierFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:55:04 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleClassifierFilter_h
#define __itkSampleClassifierFilter_h

#include <vector>

#include "itkMembershipSample.h"
#include "itkProcessObject.h"

namespace itk {
namespace Statistics {

/** \class SampleClassifierFilter 
 *  \brief This filter takes as input a Sample and produces as output a
 *  classification in the form of a MembershipSample object.
 *
 * This filter is templated over the type of sample thas is intended to
 * classify. This type will be used for the input to the filter.
 *
 * This classifier filter will assign a class label for each measurement 
 * vector. 
 *
 * Before you call the Update() method to start the classification process, you
 * should plug in all necessary parts, they include one or more membership
 * calculators, a decision rule, and the input sample data. To plug in the
 * decision rule, you use SetDecisionRule() method, for the input sample data,
 * SetInput() method, and for the membership calculators, use
 * FIXME:("AddMembershipCalculator") method.
 *
 * You can have more than one membership calculator. One for each one of the
 * classes. The order you put the membership calculator becomes the class label
 * for the class that is represented by the membership calculator.
 *
 * The classification result is stored in a vector of Subsample object.  Each
 * class has its own class sample (Subsample object) that has
 * InstanceIdentifiers for all measurement vectors belong to the class.  The
 * InstanceIdentifiers come from the target sample data. Therefore, the
 * Subsample objects act as separate class masks. 
 *
 */

template< class TSample >
class ITK_EXPORT SampleClassifierFilter : 
      public ProcessObject
{
public:
  /** Standard class typedef*/
  typedef SampleClassifierFilter         Self;
  typedef ProcessObject                  Superclass;
  typedef SmartPointer< Self >           Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  /** Standard macros */
  itkTypeMacro(SampleClassifierFilter, ProcessObject);
  itkNewMacro(Self);

  /** Type of the input Sample */
  typedef TSample                        SampleType;

  /** Output type for GetClassSample method */
  typedef MembershipSample< SampleType > MembershipSampleType;
  typedef MembershipSampleType           OutputType;

  /** typedefs from SampleType object */
  typedef typename SampleType::MeasurementType        MeasurementType;
  typedef typename SampleType::MeasurementVectorType  MeasurementVectorType;

  /** Types required for the pipeline infrastructure */
  typedef typename DataObject::Pointer                DataObjectPointer;
  
  typedef unsigned long                               ClassLabelType;
  typedef std::vector< ClassLabelType >               ClassLabelVectorType;

  /** Sets the input sample that will be classified by this filter. */
  void SetInput(const SampleType* sample);

  /** Returns the input sample data */
  const SampleType * GetInput() const;

  /** Sets the user given class labels for membership functions.
   * If users do not provide class labels for membership functions by calling
   * this function, then the index of the membership function vector for a
   * membership function will be used as class label of measurement vectors
   * belong to the membership function */ 
  // FIXME: this should be an Input  void SetMembershipFunctionClassLabels( ClassLabelVectorType& labels);

  /** Gets the user given class labels */
  // FIXME: This shouldn't exist at all:  ClassLabelVectorType& GetMembershipFunctionClassLabels() 
  // { return m_ClassLabels; }

  /** Returns the classification result */
  const OutputType * GetOutput() const;

protected:
  SampleClassifierFilter();
  virtual ~SampleClassifierFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  SampleClassifierFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  /** Starts the classification process */
  void GenerateData();

  /** Make a DataObject of the correct type to used as the specified
   * output. This method
   * is automatically called when DataObject::DisconnectPipeline() is
   * called.  
   * \sa ProcessObject
   */
  virtual DataObjectPointer MakeOutput(unsigned int idx);
 
private:
  /** Target data sample pointer*/
  // MUST be stored in the Input  const SampleType* m_Sample;

  /** Output pointer (MembershipSample) */
  // MUST be stored in the Output  typename OutputType::Pointer m_Output;

  /** User given class labels for membership functions */
  // Must be stored in the input  ClassLabelVectorType m_ClassLabels;

}; // end of class


} // end of namespace Statistics 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleClassifierFilter.txx"
#endif

#endif
