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
#include "itkMembershipFunctionBase.h"
#include "itkDecisionRule.h"
#include "itkProcessObject.h"

namespace itk {
namespace Statistics {

/** \class SampleClassifierFilter 
 *  \brief This filter takes as input a Sample and produces as output a
 *  classification in the form of a MembershipSample object.
 *
 */

template< class TSample >
class ITK_EXPORT SampleClassifierFilter : 
      public ProcessObject
{
public:
  /** Standard class typedef */
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
  typedef typename SampleType::MeasurementType            MeasurementType;
  typedef typename SampleType::MeasurementVectorType      MeasurementVectorType;

  /** typedef for the MembershipFunction */
  typedef MembershipFunctionBase< MeasurementVectorType > MembershipFunctionType;
  typedef typename MembershipFunctionType::ConstPointer   MembershipFunctionPointer;
  typedef std::vector< MembershipFunctionPointer >        MembershipFunctionVectorType;

  /** Types required for the pipeline infrastructure */
  typedef typename DataObject::Pointer                DataObjectPointer;
  
  typedef unsigned long                               ClassLabelType;
  typedef std::vector< ClassLabelType >               ClassLabelVectorType;

  /** type of the decision rule */
  typedef DecisionRule                                DecisionRuleType;
  typedef DecisionRuleType::Pointer                   DecisionRulePointer;

  /** Sets the input sample that will be classified by this filter. */
  void SetInput(const SampleType * sample);

  /** Returns the input sample data */
  const SampleType * GetInput() const;

  /** Returns the classification result */
  const OutputType * GetOutput() const;

  /** Number of classes. This must match the number of labels and membership
   * functions provided by the user, otherwise an exception will be thrown at
   * run time */
  itkSetMacro( NumberOfClasses, unsigned int );
  itkGetMacro( NumberOfClasses, unsigned int );

  /** Set/Get the vector of membership functions. The number of membership
   * functions in this vector must match the number of classes set in the
   * classifier, otherwise and exception will be thrown at run time. */
  // FIXME: This probably should be decorated as a DataObject and be the input
  // to the filter, presumably it will also be the output of the Estimator
  // filters 
  itkSetMacro( MembershipFunctions, MembershipFunctionVectorType );
  itkGetMacro( MembershipFunctions, MembershipFunctionVectorType );

  /** Set/Get the vector of membership class labels. The number of membership
   * labels in this vector must match the number of classes set in the
   * classifier, otherwise and exception will be thrown at run time. */
  // FIXME: This probably should be decorated as a DataObject and be the input
  // to the filter, presumably it will also be the output of the Estimator
  // filters 
  itkSetMacro( ClassLabels, ClassLabelVectorType );
  itkGetMacro( ClassLabels, ClassLabelVectorType );

  /** Set/Get the decision rule. */
  itkSetConstObjectMacro( DecisionRule, DecisionRuleType );
  itkGetConstObjectMacro( DecisionRule, DecisionRuleType );


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
 
  unsigned int                      m_NumberOfClasses;

  /** User given class labels for membership functions */
  ClassLabelVectorType              m_ClassLabels;

  /** Container of membership functions */
  MembershipFunctionVectorType      m_MembershipFunctions;

  /** Decision Rule */
  DecisionRulePointer              m_DecisionRule;

}; // end of class


} // end of namespace Statistics 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleClassifierFilter.txx"
#endif

#endif
