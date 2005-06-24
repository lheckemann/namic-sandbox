/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleSelectiveMeanShiftBlurringFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/09/09 22:29:49 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleSelectiveMeanShiftBlurringFilter_txx
#define __itkSampleSelectiveMeanShiftBlurringFilter_txx

namespace itk{ 
namespace Statistics{

template< class TSample >
SampleSelectiveMeanShiftBlurringFilter< TSample >
::SampleSelectiveMeanShiftBlurringFilter()
{
  m_ComponentSelections.Fill( true ) ;
}

template< class TSample >
SampleSelectiveMeanShiftBlurringFilter< TSample >
::~SampleSelectiveMeanShiftBlurringFilter()
{
}

template< class TSample >
void
SampleSelectiveMeanShiftBlurringFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Component selections: " 
     << m_ComponentSelections << std::endl ;
}

template< class TSample >
inline void
SampleSelectiveMeanShiftBlurringFilter< TSample >
::SetComponentSelections(ComponentSelectionsType selections)
{
  if ( m_ComponentSelections != selections )
    {
    m_ComponentSelections = selections ;
    }
}

template< class TSample >
inline void
SampleSelectiveMeanShiftBlurringFilter< TSample >
::GenerateData() 
{
  MeasurementVectorType queryPoint ;
  MeasurementVectorType modePoint ;
  MeasurementVectorType finalPoint ;

  typename Superclass::InputSampleType::ConstIterator iter = this->GetInputSample()->Begin() ;
  typename Superclass::InputSampleType::ConstIterator end = this->GetInputSample()->End() ;

  OutputType* output = this->GetOutput() ;
  output->Clear() ;
  MeanShiftModeSeekerType* modeSeeker = this->GetMeanShiftModeSeeker() ;
  while ( iter != end )
    {
    queryPoint = iter.GetMeasurementVector() ;
    modePoint = modeSeeker->Evolve( queryPoint ) ;
    for ( unsigned int i = 0 ; i < MeasurementVectorSize ; ++i )
      {
      if ( m_ComponentSelections[i] )
        {
        finalPoint[i] = modePoint[i] ;
        }
      else
        {
        finalPoint[i] = queryPoint[i] ; 
        }
      }
    output->PushBack( finalPoint  ) ;
    ++iter ;
    }
}

} // end of namespace Statistics 
} // end of namespace itk

#endif

