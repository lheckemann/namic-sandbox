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
  m_ComponentSelections = 0;
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
  if( this->m_MeasurementVectorSize && 
      ( selections.GetSize() != this->m_MeasurementVectorSize ) )
    {
    itkExceptionMacro( << "Size of measurement vectors in the sample must be the same as the size of the component selections array." );
    }

  if ( m_ComponentSelections != selections )
    {
    m_ComponentSelections = selections ;
    this->Modified();
    }
}

template< class TSample >
inline void
SampleSelectiveMeanShiftBlurringFilter< TSample >
::GenerateData() 
{
  // Assert at run time that the given mean has the same length as 
  // measurement vectors in the sample and that the size is non-zero.
  if( !m_ComponentSelections )
    {
    m_ComponentSelections.SetSize( this->m_MeasurementVectorSize );
    m_ComponentSelections.Fill( true );
    }
  else if( !(this->m_MeasurementVectorSize) || 
      ( m_ComponentSelections->GetSize() != this->m_MeasurementVectorSize ) )
    {
    itkExceptionMacro( << "Size of measurement vectors in the sample must be the same as the size of the component selections array." );
    }
  
  
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
    for ( unsigned int i = 0 ; i < this->m_MeasurementVectorSize ; ++i )
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

