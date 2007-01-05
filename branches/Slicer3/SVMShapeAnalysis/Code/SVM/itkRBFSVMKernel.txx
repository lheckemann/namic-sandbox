/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRBFSVMKernel.txx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 11:48:50 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkRBFSVMKernel_txx
#define __itkRBFSVMKernel_txx

#include "itkRBFSVMKernel.h"

#include <math.h>

namespace itk
{
namespace Statistics
{

/** Constructor */
template<class TVector, class TOutput>
RBFSVMKernel<TVector,TOutput>
::RBFSVMKernel()
  : m_Gamma(0.5)
{}

/** Destructor */
template<class TVector, class TOutput>
RBFSVMKernel<TVector,TOutput>
::~RBFSVMKernel()
{}

/** Evaluate function */
template<class TVector, class TOutput>
TOutput RBFSVMKernel<TVector,TOutput>
::Evaluate(const TVector& x1,const TVector& x2 ) const
{ 
  double innerProduct = 0;
  double x1L2Sq = 0;
  double x2L2Sq = 0;

  for(unsigned int i = 0; i < x1.Size(); ++i)
  {
    innerProduct += x1[i] * x2[i];
    x1L2Sq       += x1[i] * x1[i];
    x2L2Sq       += x2[i] * x2[i];
  }

  return exp(-m_Gamma * (x1L2Sq - 2*innerProduct + x2L2Sq));
}

/** Print the object */
template<class TVector, class TOutput>
void  
RBFSVMKernel<TVector, TOutput>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
  os << indent << "RBFSVMKernel(" << this << ")" << std::endl; 
} 

} // end namespace Statistics
} // end namespace itk

#endif // __itkRBFSVMKernel_txx
