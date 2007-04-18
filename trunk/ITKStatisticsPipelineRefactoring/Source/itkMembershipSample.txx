/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMembershipSample.txx,v $
  Language:  C++
  Date:      $Date: 2005/09/30 14:14:18 $
  Version:   $Revision: 1.23 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMembershipSample_txx
#define __itkMembershipSample_txx

namespace itk {
namespace Statistics {

template< class TSample >
MembershipSample< TSample >
::MembershipSample()
{
}


template< class TSample >
void
MembershipSample< TSample >
::SetNumberOfClasses(unsigned int numberOfClasses)
{
  itkDebugMacro("setting NumberOfClasses to " << numberOfClasses ); 
  if (this->m_NumberOfClasses != numberOfClasses )
    {
    // FIXME : add here allocations for Subsamples
    this->m_NumberOfClasses = numberOfClasses;
    this->Modified();
    }
}

template< class TSample >
void
MembershipSample< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Sample: " << m_Sample.GetPointer() << std::endl;
  os << indent << "NumberOfClasses: " << m_NumberOfClasses << std::endl;
}


} // end of namespace Statistics 
} // end of namespace itk

#endif
