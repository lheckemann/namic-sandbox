/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDenseFrequencyContainer.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:44 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDenseFrequencyContainer_h
#define __itkDenseFrequencyContainer_h

#include "itkDenseFrequencyContainer2.h"

namespace itk { 
namespace Statistics {

/** \class DenseFrequencyContainer 
 *  \brief his class is a container for frequencies of bins in an histogram.
 *
 * This class uses the ValarrayImageContainer class to store
 * frequencies. If the histogram has many zero frequency bins. 
 * use SparseFrequencyContainer.  You should access each bin 
 * by (InstanceIdentifier)index or measurement vector.
 *
 * \sa Histogram, SparseFrequencyContainer
 */
    
class ITK_EXPORT DenseFrequencyContainer 
  : public DenseFrequencyContainer2
{
public:
  /** Standard class typedefs */
  typedef DenseFrequencyContainer  Self;
  typedef DenseFrequencyContainer2 Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(DenseFrequencyContainer, DenseFrequencyContainer2);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** InstanceIdenfitifer type alias */
  typedef unsigned long InstanceIdentifier;

  /** Frequency type alias */
  typedef Superclass::AbsoluteFrequencyType FrequencyType;
  
  /** Total frequency type */
  typedef Superclass::TotalAbsoluteFrequencyType TotalFrequencyType;

  /** Internal storage class typedefs */
  typedef Superclass::FrequencyContainerType    FrequencyContainerType;
  typedef Superclass::FrequencyContainerPointer FrequencyContainerPointer;

protected:
  DenseFrequencyContainer () {}
  virtual ~DenseFrequencyContainer () {}

private:
  DenseFrequencyContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
}; // end of class

} // end of namespace Statistics
} // end of namespace itk

#endif
