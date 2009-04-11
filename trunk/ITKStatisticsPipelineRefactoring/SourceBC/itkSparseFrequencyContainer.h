/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseFrequencyContainer.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 19:29:54 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSparseFrequencyContainer_h
#define __itkSparseFrequencyContainer_h

#include "itkSparseFrequencyContainer2.h"

namespace itk { 
namespace Statistics {

/** \class SparseFrequencyContainer 
 *  \brief his class is a container for an histogram.
 *
 *  This class uses an map to store histogram. If your histogram is dense
 *  use DenseHistogram.  You should access each bin by 
 * (InstanceIdentifier)index or measurement vector.
 */
    
class ITK_EXPORT SparseFrequencyContainer : public SparseFrequencyContainer2
{
public:
  /** Standard class typedefs. */
  typedef SparseFrequencyContainer  Self;
  typedef SparseFrequencyContainer2 Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Standard macros */
  itkTypeMacro(SparseFrequencyContainer, SparseFrequencyContainer2);
  itkNewMacro(Self);

protected:
  SparseFrequencyContainer(){};
  virtual ~SparseFrequencyContainer() {}

private:
  SparseFrequencyContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end of namespace Statistics
} // end of namespace itk

#endif
