/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMembershipSample.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:24:45 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMembershipSample_h
#define __itkMembershipSample_h

#include "itk_hash_map.h"
#include "itkSample.h"
#include "itkSubsample.h"

namespace itk {
namespace Statistics {

/** \class MembershipSample
 * \brief Container for storing the instance-identifiers of other sample with 
 * their associated class labels.
 *
 * This class does not store any measurement data. In a sense, you can
 * think it as an additional information to basic samples (such as Histogram,
 * PointSetListSampleAdaptor, and ImageToListAdaptor). The additional
 * information is a class label for a measurement vector. Obviously without
 * such basic types of sample, this one is meaningless. You can call any
 * basic methods that has been defined in the Sample class such as 
 * GetMeasurementVector and GetFrequency. You can query the class label for
 * an instance using an instance-identifier. Another new and important method 
 * is the GetClassSample method. With a given class label, it returns a
 * pointer to the Subsample object that has all the instance-identifiers 
 * of instances that belong to the class.
 *
 * This class is templated over the type of the basic sample. To use all
 * the method, you should first plug in a basic type sample using the
 * SetSample method
 * 
 */
  
template< class TSample >
class ITK_EXPORT MembershipSample : public DataObject
{
public:
  /** Standard class typedefs. */
  typedef MembershipSample            Self;
  typedef DataObject                  Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Standard macros */ 
  itkTypeMacro(MembershipSample, DataObject);
  itkNewMacro(Self);
  
  /** Typedefs for Measurement vector, measurement, Instance Identifier, 
   * frequency, size, size element value from the template argument TSample*/
  typedef TSample                                    SampleType;
  typedef typename SampleType::MeasurementVectorType MeasurementVectorType;
  typedef typename SampleType::MeasurementType       MeasurementType;
  typedef typename SampleType::InstanceIdentifier    InstanceIdentifier;
  typedef typename SampleType::ConstPointer          SampleConstPointer;

  typedef typename SampleType::AbsoluteFrequencyType         AbsoluteFrequencyType;
  typedef typename SampleType::TotalAbsoluteFrequencyType    TotalAbsoluteFrequencyType;
  
  
  /** vector of unique class labels that will be used for mapping internal
   * continuous class label with real class labels */
  typedef std::vector< unsigned int > UniqueClassLabelsType;

  /** Typedef for the storage that holds a class label for each instance.
   * The relationship between instances and class label is one-to-one */
  typedef itk::hash_map< InstanceIdentifier, unsigned int> ClassLabelHolderType;
  
  /** Typedef for each subsample that stores instance identifers of instances
   * that belong to a class */
  typedef Subsample< SampleType >                   ClassSampleType;
  typedef typename ClassSampleType::Pointer         ClassSamplePointer;
  typedef typename ClassSampleType::ConstPointer    ClassSampleConstPointer;
  
  /** Set/Get the actual sample data */
  itkSetConstObjectMacro( Sample, SampleType );
  itkGetConstObjectMacro( Sample, SampleType );

  /** Sets the number of classes (class labels) */
  void SetNumberOfClasses(unsigned int numberOfClasses);
  
  /** Gets the number of classes (class labels) */
  itkGetMacro( NumberOfClasses, unsigned int );

 
protected:
  MembershipSample();
  virtual ~MembershipSample() {}
  void PrintSelf(std::ostream& os, Indent indent) const;  
  
private:
  MembershipSample(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  SampleConstPointer              m_Sample;
  unsigned int                    m_NumberOfClasses;

}; // end of class


} // end of namespace Statistics 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMembershipSample.txx"
#endif

#endif
