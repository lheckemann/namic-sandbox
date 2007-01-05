/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLinearSVMKernel.txx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkLinearSVMKernel_txx
#define __itkLinearSVMKernel_txx

#include "itkLinearSVMKernel.h"

#include <math.h>

namespace itk
{
namespace Statistics
{

/** Constructor */
template<class TVector, class TOutput>
LinearSVMKernel<TVector,TOutput>
::LinearSVMKernel()
{}

/** Destructor */
template<class TVector, class TOutput>
LinearSVMKernel<TVector,TOutput>
::~LinearSVMKernel()
{}

/** Evaluate function */
template<class TVector, class TOutput>
TOutput LinearSVMKernel<TVector,TOutput>
::Evaluate(const TVector& x1,const TVector& x2 ) const
{ 
  double innerProduct = 0;
  for(unsigned int i = 0; i < x1.Size(); ++i)
  {
    innerProduct += x1[i] * x2[i];
  }
  return innerProduct;
}

/** Print the object */
template<class TVector, class TOutput>
void  
LinearSVMKernel<TVector, TOutput>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
  os << indent << "LinearSVMKernel(" << this << ")" << std::endl; 
} 

} // end namespace Statistics
} // end namespace itk

#endif // __itkLinearSVMKernel_txx
