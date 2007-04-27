/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkListSample.txx,v $
  Language:  C++
  Date:      $Date: 2006/12/20 22:35:44 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkListSample_txx
#define __itkListSample_txx

#include "itkListSample.h"

namespace itk { 
namespace Statistics {

template< class TMeasurementVector >
ListSample< TMeasurementVector >
::ListSample()
{
}

template< class TMeasurementVector >
void 
ListSample< TMeasurementVector >
::Resize( InstanceIdentifier newsize )
{
  this->m_InternalContainer.resize( newsize );
}


template< class TMeasurementVector >
void 
ListSample< TMeasurementVector >
::Clear()
{
  this->m_InternalContainer.clear();
}

template< class TMeasurementVector >
void 
ListSample< TMeasurementVector >
::PushBack( const MeasurementVectorType & mv )
{
  if( this->GetMeasurementVectorSize() != MeasurementVectorTraits::GetLength( mv ) )
    {
    itkExceptionMacro("MeasurementVector instance doesn't match MeasurementVectorSize");
    }
  this->m_InternalContainer.push_back( mv );
}

template< class TMeasurementVector >
typename ListSample< TMeasurementVector >::InstanceIdentifier
ListSample< TMeasurementVector >
::Size() const
{
  return static_cast<InstanceIdentifier>( 
    this->m_InternalContainer.size() );
}

template< class TMeasurementVector >
typename ListSample< TMeasurementVector >::TotalFrequencyType
ListSample< TMeasurementVector >
::GetTotalFrequency() const
{
  // Since the entries are unique, the total
  // frequency is equal to the numbe of entries.
  return this->Size();
}


template< class TMeasurementVector >
const typename ListSample< TMeasurementVector >::MeasurementVectorType &
ListSample< TMeasurementVector >
::GetMeasurementVector(InstanceIdentifier id) const 
{
  if ( id < m_InternalContainer.size() )
    {
    return m_InternalContainer[id];
    }
  itkExceptionMacro("MeasurementVector " << id << " does not exist");
}

template< class TMeasurementVector >
void 
ListSample< TMeasurementVector >
::SetMeasurement( InstanceIdentifier id, 
                  unsigned int dim,
                  const MeasurementType &value)
{
  if ( id < m_InternalContainer.size() )
    {
    m_InternalContainer[id][dim] = value;
    }
}

template< class TMeasurementVector >
void
ListSample< TMeasurementVector >
::SetMeasurementVector( InstanceIdentifier id, 
                        const MeasurementVectorType &mv)
{
  if ( id < m_InternalContainer.size() )
    {
    m_InternalContainer[id] = mv;
    }
}

template< class TMeasurementVector >
typename ListSample< TMeasurementVector >::FrequencyType 
ListSample< TMeasurementVector >
::GetFrequency( InstanceIdentifier id ) const
{
  if ( id < m_InternalContainer.size() )
    {
    return 1.0;
    }
  else
    {
    return 0.0;
    }
}

template< class TMeasurementVector >
void 
ListSample< TMeasurementVector >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Internal Data Container: "
     << &m_InternalContainer << std::endl;
  os << indent << "Number of samples: " 
     << this->m_InternalContainer.size() << std::endl;
}
} // end of namespace Statistics
} // end of namespace itk 

#endif
