/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSample.h,v $
  Language:  C++
  Date:      $Date: 2006/02/21 18:53:18 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkListSampleToHistogramFilter_txx
#define __itkListSampleToHistogramFilter_txx

#include "itkListSampleToHistogramFilter.h"

namespace itk { 
namespace Statistics {

template < class TSample, class THistogram >
ListSampleToHistogramFilter< TSample, THistogram >
::ListSampleToHistogramFilter()
{
}

template < class TSample, class THistogram >
ListSampleToHistogramFilter< TSample, THistogram >
::~ListSampleToHistogramFilter()
{
}

template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::SetInput( const SampleType * sample )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType * >( sample ) );
}

template < class TSample, class THistogram >
const typename 
ListSampleToHistogramFilter< TSample, THistogram >::SampleType *
ListSampleToHistogramFilter< TSample, THistogram >
::GetInput() const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return NULL;
    }
  const SampleType * input = 
    static_cast<const SampleType * >(this->ProcessObject::GetInput(0) );

  return input;
}

template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


} // end of namespace StatisticsNew 
} // end of namespace itk

#endif
