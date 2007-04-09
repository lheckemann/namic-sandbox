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
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


} // end of namespace StatisticsNew 
} // end of namespace itk

#endif
