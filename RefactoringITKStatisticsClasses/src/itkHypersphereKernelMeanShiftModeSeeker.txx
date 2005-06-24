/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHypersphereKernelMeanShiftModeSeeker.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/15 01:00:46 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHypersphereKernelMeanShiftModeSeeker_txx
#define __itkHypersphereKernelMeanShiftModeSeeker_txx

#include "vnl/vnl_math.h"

namespace itk{ 
namespace Statistics{

template< class TSample >
HypersphereKernelMeanShiftModeSeeker< TSample >
::HypersphereKernelMeanShiftModeSeeker()
{
  m_SearchRadius = 0.0 ;
}

template< class TSample >
HypersphereKernelMeanShiftModeSeeker< TSample >
::~HypersphereKernelMeanShiftModeSeeker()
{
}

template< class TSample >
void
HypersphereKernelMeanShiftModeSeeker< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Search radius: " << m_SearchRadius << std::endl ;
}

template< class TSample >
void
HypersphereKernelMeanShiftModeSeeker< TSample >
::SetSearchRadius(double radius)
{
  if ( m_SearchRadius != radius )
    {
    m_SearchRadius = radius ;
    this->Modified() ;
    }
}

template< class TSample >
inline bool
HypersphereKernelMeanShiftModeSeeker< TSample >
::ComputeMode(MeasurementVectorType queryPoint, 
              MeasurementVectorType& newPoint)
{
  const TSample* inputSample = this->GetInputSample() ;
  float frequencySum ;

  SearchResultVectorType result ;
  inputSample->Search( queryPoint, m_SearchRadius, result ) ;
  
  frequencySum = 0.0f ;
  m_TempVectorSum.Fill( NumericTraits< RealMeasurementType >::Zero ) ;

  typename SearchResultVectorType::const_iterator iter =  result.begin() ;
  while ( iter != result.end() )
    {
    m_TempVector = inputSample->GetMeasurementVector(*iter) ;
    for ( unsigned int i = 0 ; i < MeasurementVectorSize ; ++i )
      {
      m_TempVectorSum[i] += m_TempVector[i] ;
      }
    frequencySum += inputSample->GetFrequency(*iter) ;
    ++iter ;
    }

  if ( frequencySum == 0 )
    {
    return false ;
    }

  for ( unsigned int i = 0 ; i < MeasurementVectorSize ; ++i )
    {
    newPoint[i] =  (MeasurementType) (m_TempVectorSum[i] / frequencySum) ;
    }
  
  return true ;
}


} // end of namespace Statistics 
} // end of namespace itk

#endif

