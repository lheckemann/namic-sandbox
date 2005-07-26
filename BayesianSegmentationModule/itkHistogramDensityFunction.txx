/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramDensityFunction.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/25 01:00:46 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogramDensityFunction_txx
#define __itkHistogramDensityFunction_txx

#include "itkHistogramDensityFunction.h"

namespace itk{ 
namespace Statistics{

template < class TMeasurementVector, class THistogram >
HistogramDensityFunction< TMeasurementVector, THistogram >
::HistogramDensityFunction()
{
  m_Class = 0;
}

template < class TMeasurementVector, class THistogram >
HistogramDensityFunction< TMeasurementVector, THistogram >
::~HistogramDensityFunction()
{
}

template < class TMeasurementVector, class THistogram >
void  
HistogramDensityFunction< TMeasurementVector, THistogram >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


template < class TMeasurementVector, class THistogram >
void
HistogramDensityFunction< TMeasurementVector, THistogram >
::SetHistogram(const HistogramType * histogram)
{ 
  m_Histogram = histogram;
  
  HistogramSizeType size = histogram->GetSize();
  m_FrequencyPerLabel = ArrayType( size[1] );
}

template < class TMeasurementVector, class THistogram >
inline double
HistogramDensityFunction< TMeasurementVector, THistogram >
::Evaluate(const MeasurementVectorType &measurement) const
{ 
  typedef typename THistogram::MeasurementVectorType HistogramMeasurementVectorType;
  typedef typename THistogram::IndexType             HistogramIndexType;

  HistogramMeasurementVectorType m;
  m[0] = measurement[0];
  m[1] = m_Class;
  
  HistogramIndexType index = m_Histogram->GetIndex( m );
  const double frequency = m_Histogram->GetFrequency( index );

  // REMEMBER AFTER THE STATISTICS FRAMEWORK IS REFACTORED, THE NUMBER "1"
  // IN THE FOLLOWING LINE SHOULD BE CHANGED FOR : length-1 of the measurement
  // vector.
  const double totalFrequency = m_Histogram->GetFrequency( m_Class, 1 );
  const double probability = frequency / totalFrequency;
  return probability;
}
  

} // end namespace Statistics
} // end of namespace itk



#endif
