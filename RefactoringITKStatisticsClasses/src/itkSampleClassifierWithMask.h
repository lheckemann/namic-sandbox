/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleClassifierWithMask.h,v $
  Language:  C++
  Date:      $Date: 2003/12/09 16:53:08 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleClassifierWithMask_h
#define __itkSampleClassifierWithMask_h

#include <vector>

#include "itkObject.h"
#include "itkExceptionObject.h"
#include "itkSubsample.h"
#include "itkMembershipSample.h"
#include "itkSampleClassifier.h"

namespace itk{ 
  namespace Statistics{

/** \class SampleClassifierWithMask 
 *  \brief Integration point for MembershipCalculator, DecisionRule, and 
 * target sample data. This class is functionally identical to the 
 * SampleClassifier, except that users can perform only part of the
 * input sample that belongs to the subset of classes. 
 * 
 * To this purpose, this class needs a class mask sample that has
 * class labels as measurement vectors. Using SetMask method, users can
 * provide the class mask sample.
 *
 * To specify which classes should be included for classification, users
 * must call SetSelectedClassLabels method with class labels that will be
 * included. All measurement vectors that belong to the non-selected
 * classes will be classified to the class label that has been given 
 * by the SetOtherClassLabel method.
 *
 * Except for the modifications mentioned above, the basic behavior and
 * methods are identical to those of SampleClassifier.
 * 
 * \sa SampleClassifier
 */

template< class TSample, class TMaskSample >
class ITK_EXPORT SampleClassifierWithMask : 
      public SampleClassifier< TSample >
{
public:
  /** Standard class typedef*/
  typedef SampleClassifierWithMask Self;
  typedef SampleClassifier< TSample > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer<const Self> ConstPointer;

 /** Standard macros */
  itkTypeMacro(SampleClassifierWithMask, SampleClassifier);
  itkNewMacro(Self) ;

  /** Superclass typedefs */
  typedef typename Superclass::OutputType OutputType ;
  typedef typename Superclass::ClassLabelType ClassLabelType ;
  typedef typename Superclass::ClassLabelVectorType ClassLabelVectorType ;

  /** typedefs from TSample object */
  typedef typename TSample::MeasurementType MeasurementType ;
  typedef typename TSample::MeasurementVectorType MeasurementVectorType ;

  
  /** REMOVED: THE StaticConstMacro for this method has been removed to 
   * allow the measurement vector length to be specified at run time.
   * It is now obtained at run time from the sample that is plugged in
   * as input to the classifier.
   *
   * Please use the Get macros to access the MeasurementVectorLength
   * instead. Note that GetMeasurementVectorSize() will return 0 unless
   * you have plugged in the input sample using the SetSample() 
   * method
   *
   * NOTE: This means that you will no longer be able to get the 
   * MeasurementVectorLength as a static const member any more.
   */
  //itkStaticConstMacro(MeasurementVectorSize, unsigned int,
  //                    TSample::MeasurementVectorSize) ;


  /** typedefs from Superclass */
  typedef typename Superclass::MembershipFunctionPointerVector 
    MembershipFunctionPointerVector ;

  void SetMask( TMaskSample* mask ) ;

  TMaskSample* GetMask()
  { return m_Mask.GetPointer() ; }

  void SetSelectedClassLabels( ClassLabelVectorType& labels)
  { m_SelectedClassLabels = labels ; }

  void SetOtherClassLabel( ClassLabelType label) 
  { m_OtherClassLabel = label ; }
 
protected:
  SampleClassifierWithMask() ;
  virtual ~SampleClassifierWithMask() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Starts the classification process */
  void GenerateData() ;

private:
  /** Mask sample pointer*/
  typename TMaskSample::Pointer m_Mask ;
  ClassLabelVectorType m_SelectedClassLabels ;
  ClassLabelType m_OtherClassLabel ;
} ; // end of class


  } // end of namespace Statistics 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleClassifierWithMask.txx"
#endif

#endif







