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
 *  \brief this class is a container for frequencies of bins in an histogram.
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
  itkTypeMacro(DenseFrequencyContainer, DenseFrequencyContainer);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

protected:
  DenseFrequencyContainer(){};
  virtual ~DenseFrequencyContainer() {}

private:
  DenseFrequencyContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end of namespace Statistics
} // end of namespace itk

#endif
