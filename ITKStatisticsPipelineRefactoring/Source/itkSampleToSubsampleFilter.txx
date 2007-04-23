/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleToSubsampleFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:24:45 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleToSubsampleFilter_txx
#define __itkSampleToSubsampleFilter_txx

#include "itkSampleToSubsampleFilter.h"

namespace itk {
namespace Statistics {

template< class TSample >
SampleToSubsampleFilter< TSample >
::SampleToSubsampleFilter()
{
}

template< class TSample >
SampleToSubsampleFilter< TSample >
::~SampleToSubsampleFilter()
{
}

template < class TSample >
void
SampleToSubsampleFilter< TSample >
::SetInput( const SampleType * sample )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType * >( sample ) );
}

template < class TSample >
const typename 
SampleToSubsampleFilter< TSample >::SampleType *
SampleToSubsampleFilter< TSample >
::GetInput() const
{
  const SampleType * input = 
    static_cast< const SampleType * >( this->ProcessObject::GetInput( 0 ) );
  return input;
}


template < class TSample >
void
SampleToSubsampleFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}


} // end of namespace Statistics 
} // end of namespace itk


#endif
