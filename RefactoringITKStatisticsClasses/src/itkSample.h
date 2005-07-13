/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSample.h,v $
  Language:  C++
  Date:      $Date: 2005/05/20 14:55:54 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSample_h
#define __itkSample_h

#include <vector>

#include "itkMacro.h"
#include "itkPoint.h"
#include "itkSize.h"
#include "itkObject.h"
#include "itkFixedArray.h"

namespace itk{ 
namespace Statistics{

/** \class Sample 
 *  \brief A collection of measurements for statistical analysis
 *
 * Sample represents a set of measurements for statistical
 * analysis. Sample is templated over a measurement vector. The
 * measurement vector encapsulates the set of values associated with a
 * single measurement.  For instance, a measurement vector may contain
 * an image intensity of a pixel and the gradient magnitude at that pixel.
 * 
 * Data within a sample can be accessed via an
 * InstanceIdentfier. InstanceIdentifiers have different forms and
 * meanings depending on the type of sample.  For ListSamples, the
 * InstanceIdentifier is an index into the corresponding list. In this
 * case, the InstanceIndentifier corresponds to a particular
 * measurement stored in the Sample. For Histograms, an
 * InstanceIdentifier corresponds to a particular bin in the
 * N-dimensional histogram. In other words, the InstanceIdentifier in
 * a histogram does not correspond to a specific measurement used to
 * build the histogram but to the "bin" in which a number of original
 * measurements were "accumulated".
 *
 */

template < class TMeasurementVector >
class ITK_EXPORT Sample : public Object
{
public:
  /** Standard class typedefs */
  typedef Sample  Self;  
  typedef Object Superclass ;
  typedef SmartPointer< Self > Pointer ;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(Sample, Object);

  /** MeasurementVector typedef support */ 
  typedef TMeasurementVector MeasurementVectorType ;

  /** ValueType of a measurement (ValueType of a component of the
   * MeasurementVector */ 
  typedef typename TMeasurementVector::ValueType MeasurementType ;

  /** Frequency value type*/
  typedef float FrequencyType ;

  /** InstanceIdentifier typedef. This identifier is a unique
   * sequential id for each measurement vector in a Sample subclass.*/ 
  typedef unsigned long InstanceIdentifier ;

  /** Typedef for the length of each measurement vector */
  typedef unsigned int  MeasurementVectorSizeType;

  /** REMOVED: THE StaticConstMacro for this method has been removed to 
   * allow the measurement vector length to be specified at run time.
   *
   * Please use the Set/Get macros to access the MeasurementVectorLength
   * instead. 
   *
   * NOTE: This means that you will no longer be able to get the 
   * MeasurementVectorLength as a static const member any more.
   */

  

  /** Get the size of the sample (number of measurements) */
  virtual unsigned int Size() const = 0 ;

  /** Get the measurement associated with a particular
   * InstanceIdentifier. */
  virtual const MeasurementVectorType & 
    GetMeasurementVector(const InstanceIdentifier &id) const = 0 ;

  /** Get the frequency of a measurement specified by instance
   * identifier. */
  virtual FrequencyType GetFrequency(const InstanceIdentifier &id) const = 0 ;

  /** Get the total frequency of the sample. */
  virtual FrequencyType GetTotalFrequency() const 
    = 0 ;

  
  /** Set/Get macros for the length of the measurement vector */
  itkSetMacro( MeasurementVectorSize, unsigned int );
  itkGetConstMacro( MeasurementVectorSize, unsigned int );

  
protected:
  Sample():m_MeasurementVectorSize( 0 ) {}
  virtual ~Sample() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }


  
private:
  Sample(const Self&) ; //purposely not implemented
  void operator=(const Self&) ; //purposely not implemented

  unsigned int m_MeasurementVectorSize;
} ; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#endif
