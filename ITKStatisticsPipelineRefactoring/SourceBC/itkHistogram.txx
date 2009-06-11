/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogram.txx,v $
  Language:  C++
  Date:      $Date: 2009-04-26 00:37:48 $
  Version:   $Revision: 1.53 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogram_txx
#define __itkHistogram_txx

#include "itkHistogram.h"

namespace itk { 
namespace Statistics {

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Histogram()
{
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size)
{
  typename Superclass::SizeType newSize;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newSize[i] = size[i];
    }
  Superclass::Initialize(newSize);  
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size, MeasurementVectorType& lowerBound,
             MeasurementVectorType& upperBound)
{
  typename Superclass::SizeType newSize;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newSize[i] = size[i];
    }
  typename Superclass::MeasurementVectorType newLowerBound;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newLowerBound[i] = lowerBound[i];
    }
  typename Superclass::MeasurementVectorType newUpperBound;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newUpperBound[i] = upperBound[i];
    }
  Superclass::Initialize(newSize, newLowerBound, newUpperBound);  
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline const typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementVectorType &
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetMeasurementVector(const IndexType &index) const
{
  typename Superclass::IndexType newIndex;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newIndex[i] = index[i];
    }
  typename Superclass::MeasurementVectorType newMeasurementVector =
    Superclass::GetMeasurementVector (newIndex);
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    m_InternalMeasurementVector[i] = newMeasurementVector[i];
    }

  return m_InternalMeasurementVector;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline const typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementVectorType &
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetMeasurementVector(const InstanceIdentifier &identifier) const
{
  return this->GetMeasurementVector( this->GetIndex(identifier) );
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::IncreaseFrequency(const InstanceIdentifier &id,
                    const FrequencyType value)
{
  return Superclass::IncreaseFrequency(id, value);
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::IncreaseFrequency(const IndexType &index, const FrequencyType value)
{
  const bool result = 
      this->IncreaseFrequency( this->GetInstanceIdentifier(index), value);
  return result;
}
  
template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::IncreaseFrequency(const MeasurementVectorType &measurement, const FrequencyType value) 
{
  IndexType index;
  this->GetIndex( measurement, index );
  return this->IncreaseFrequency( this->GetInstanceIdentifier( index ), value );
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer>
inline const typename Histogram<TMeasurement, VMeasurementVectorSize,
                                TFrequencyContainer>::IndexType &
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const MeasurementVectorType& measurement) const
{
  // Have this deprecated method call the un-deprecated one.. 
  this->GetIndex( measurement, m_TempIndex );
  return m_TempIndex;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
bool Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const MeasurementVectorType & measurement,IndexType & index ) const
{
  typename Superclass::IndexType newIndex;
  for (unsigned int dim = 0; dim < VMeasurementVectorSize; dim++)
    {
    newIndex[dim] = index[dim];
    }

  typename Superclass::MeasurementVectorType newMeasurement(VMeasurementVectorSize);
  for (unsigned int dim = 0; dim < VMeasurementVectorSize; dim++)
    {
    newMeasurement[dim] = measurement[dim];
    }
  return Superclass::GetIndex(newMeasurement,newIndex );
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
inline const typename Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>::IndexType&
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const InstanceIdentifier &id)  const
{
  Superclass::IndexType newIndex = Superclass::GetIndex(id);

  for (unsigned int dim = 0; dim < VMeasurementVectorSize; dim++)
    {
    m_TempIndex[dim] = newIndex[dim];
    }
  return m_TempIndex;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline bool
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::IsIndexOutOfBounds(const IndexType &index) const
{
  typename Superclass::IndexType newIndex;

  for (unsigned int dim = 0; dim < VMeasurementVectorSize; dim++)
    {
    newIndex[dim] = index[dim];
    }
  return Superclass::IsIndexOutOfBounds(newIndex);
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline typename Histogram<TMeasurement, VMeasurementVectorSize,
                          TFrequencyContainer>::InstanceIdentifier
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetInstanceIdentifier(const IndexType &index) const
{
  typename Superclass::IndexType newIndex;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newIndex[i] = index[i];
    }

  return Superclass::GetInstanceIdentifier(newIndex);
}

} // end of namespace Statistics 
} // end of namespace itk 

#endif
