/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDistanceMetric.txx,v $
  Language:  C++
  Date:      $Date: 2004/09/11 03:19:00 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDistanceMetric_txx
#define __itkDistanceMetric_txx

#include "itkDistanceMetric.h"

namespace itk{ 
namespace Statistics{

template< class TVector >
void 
DistanceMetric< TVector >
::SetOrigin(const OriginType &x)
{
  m_Origin = x ;
}

template< class TVector >
void 
DistanceMetric< TVector >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Origin: " << m_Origin << std::endl;
  
}
} // end of namespace Statistics 
} // end of namespace itk

#endif







