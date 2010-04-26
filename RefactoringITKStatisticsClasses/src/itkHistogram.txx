/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogram.txx,v $
  Language:  C++
  Date:      $Date: 2005/05/11 18:01:47 $
  Version:   $Revision: 1.36 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkHistogram_txx
#define _itkHistogram_txx

#include "itkHistogram.h"
#include "itkNumericTraits.h"

namespace itk{ 
namespace Statistics{

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Histogram()
{
  m_ClipBinsAtEnds = true;
  m_FrequencyContainer = FrequencyContainerType::New() ;
  for (unsigned int i = 0 ; i < (MeasurementVectorSize + 1) ; ++i )
    {
    m_OffsetTable[i] = 0 ;
    } 
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
unsigned int
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Size() const
{
  unsigned int size = 1 ;
  for (unsigned int i = 0 ; i < MeasurementVectorSize ; i++)
    {
    size *= m_Size[i] ;
    }
  return size ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size)
{
  m_Size = size ;
  
  // creates offset table which will be used for generation of
  // instance identifiers.
  InstanceIdentifier num = 1 ;
  
  m_OffsetTable[0] = num ;
  for (unsigned int i = 0 ; i < MeasurementVectorSize ; i++)
    {
    num *= m_Size[i] ;
    m_OffsetTable[i + 1] = num ;
    }

  m_NumberOfInstances = num ;

  // adjust the sizes of min max value containers 
  unsigned int dim;
  m_Min.resize(MeasurementVectorSize);
  for ( dim = 0; dim < MeasurementVectorSize; dim++)
    {
    m_Min[dim].resize(m_Size[dim]);
    } 

  m_Max.resize(MeasurementVectorSize);
  for ( dim = 0; dim < MeasurementVectorSize; dim++)
    {
    m_Max[dim].resize(m_Size[dim]);
    } 

  // initialize the frequency container
  m_FrequencyContainer->Initialize(m_OffsetTable[VMeasurementVectorSize]) ;
  this->SetToZero();
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void 
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::SetToZero()
{
  m_FrequencyContainer->SetToZero();
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void 
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size, MeasurementVectorType& lowerBound,
             MeasurementVectorType& upperBound)
{
  this->Initialize(size) ;

  float interval ;
  for ( unsigned int i = 0 ; i < MeasurementVectorSize ; i++)
    {
    interval = (float) (upperBound[i] - lowerBound[i]) 
                       / static_cast< MeasurementType >(size[i]) ;

    // Set the min vector and max vector
    for (unsigned int j = 0; j < (size[i] - 1) ; j++)
      {
      this->SetBinMin(i, j, (MeasurementType)(lowerBound[i] +  
                                              ((float)j * interval))) ;
      this->SetBinMax(i, j, (MeasurementType)(lowerBound[i] +  
                                              (((float)j + 1) * interval)));
      }
    this->SetBinMin(i, size[i] - 1, 
                    (MeasurementType)(lowerBound[i] + 
                                      (((float) size[i] - 1) * interval))) ;
    this->SetBinMax(i, size[i] - 1, 
                    (MeasurementType)(upperBound[i])) ;
    }
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer>
inline const typename Histogram<TMeasurement, VMeasurementVectorSize,
                                TFrequencyContainer>::IndexType &
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const MeasurementVectorType& measurement) const
{
  // now using something similar to binary search to find
  // index.
  unsigned int dim ;
  
  int begin, mid, end ;
  MeasurementType median ;
  MeasurementType tempMeasurement ;

  for (dim = 0 ; dim < MeasurementVectorSize ; dim++)
    {
    tempMeasurement = measurement[dim] ;
    begin = 0 ;
    if (tempMeasurement < m_Min[dim][begin])
      {
      // one of measurement is below the minimum
      if(!m_ClipBinsAtEnds)
        {
        m_TempIndex[dim] = (long) 0 ;
        }
      else
        {
        // If clipping and this is below, then set to an illegal value
        // then set to m_Size[dim] to flag this as an illegal index;
        // Must be set to m_Size[dim] since -1 might not be a valid value.
        // This is why you must call IsIndexOutOfBounds before using the
        // index returned by this function.
        m_TempIndex[dim] = (long) m_Size[dim] ;
        itkWarningMacro(<<"One of the measurements is below the minimum");
        }
      }

    end = m_Min[dim].size() - 1 ;
    if (tempMeasurement >= m_Max[dim][end])
      {
      // one of measurement is above the maximum
      if(!m_ClipBinsAtEnds)
        {
        m_TempIndex[dim] = (long) m_Size[dim] - 1;
        }
      else
        {
        // If clipping and this is below, then set to an illegal value
        // then set to m_Size[dim] to flag this as an illegal index;
        // Must be set to m_Size[dim] since -1 might not be a valid value.
        // This is why you must call IsIndexOutOfBounds before using the
        // index returned by this function.
        m_TempIndex[dim] = (long) m_Size[dim] ;
        itkWarningMacro(<<"One of the measurements is above the maximum");
        }
      }

    mid = (end + 1) / 2 ;
    median = m_Min[dim][mid] ;
    while(true)
      {
      if (tempMeasurement < median )
        {
        end = mid - 1 ;
        } 
      else if (tempMeasurement > median)
        {
        if (tempMeasurement < m_Max[dim][mid])
          {
          m_TempIndex[dim] = mid ;
          break ;
          }
              
        begin = mid + 1 ;
        }
      else
        {
        // measurement[dim] = m_Min[dim][med] 
        m_TempIndex[dim] = mid ;
        break ;
        }
      mid = begin + (end - begin) / 2 ;
      median = m_Min[dim][mid] ;
      } // end of while
    } // end of for()
  return m_TempIndex;
}



/** */
template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
bool Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const MeasurementVectorType & measurement,IndexType & index ) const
{
  // now using something similar to binary search to find
  // index.
  unsigned int dim ;
  
  int begin, mid, end ;
  MeasurementType median ;
  MeasurementType tempMeasurement ;

  for (dim = 0 ; dim < MeasurementVectorSize ; dim++)
    {
    tempMeasurement = measurement[dim] ;
    begin = 0 ;
    if (tempMeasurement < m_Min[dim][begin])
      {
      // one of measurement is below the minimum
      index[dim] = (long) m_Size[dim] ;
      return false;
      }

    end = m_Min[dim].size() - 1 ;
    if (tempMeasurement >= m_Max[dim][end])
      {
      // one of measurement is above the maximum
      index[dim] = (long) m_Size[dim] ;
      return false;
      }

    mid = (end + 1) / 2 ;
    median = m_Min[dim][mid];

    while(true)
      {
      if (tempMeasurement < median )
        {
        end = mid - 1 ;
        } 
      else if (tempMeasurement > median)
        {
        if (tempMeasurement < m_Max[dim][mid])
          {
          index[dim] = mid ;
          break ;
          }
              
        begin = mid + 1 ;
        }
      else
        {
        // measurement[dim] = m_Min[dim][med] 
        index[dim] = mid ;
        break ;
        }
      mid = begin + (end - begin) / 2 ;
      median = m_Min[dim][mid] ;
      } // end of while
    } // end of for()
  return true;
}



template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
inline const typename Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>::IndexType&
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetIndex(const InstanceIdentifier &id)  const
{
  InstanceIdentifier id2 = id ;

  for (int i = MeasurementVectorSize - 1 ; i > 0 ; i--)
    {
    m_TempIndex[i] = static_cast<IndexValueType>(id2 / m_OffsetTable[i]);
    id2 -= (m_TempIndex[i] * m_OffsetTable[i]);
    }
  m_TempIndex[0] = static_cast<IndexValueType>(id2);
  
  return m_TempIndex;
}


template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline bool
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::IsIndexOutOfBounds(const IndexType &index) const
{
  for (unsigned int dim = 0 ; dim < MeasurementVectorSize ; dim++)
    {
    if (index[dim] < 0 || index[dim] >= static_cast<IndexValueType>(m_Size[dim]))
      {
      return true ;
      }
    }
  return false ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline typename Histogram<TMeasurement, VMeasurementVectorSize,
                          TFrequencyContainer>::InstanceIdentifier
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetInstanceIdentifier(const IndexType &index) const
{
  InstanceIdentifier id = 0 ;
  for (int i= MeasurementVectorSize - 1 ; i > 0 ; i-- )
    {
    id += index[i] * m_OffsetTable[i];
    }
  
  id += index[0] ;
  
  return id ;
}


template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline const typename Histogram<TMeasurement, VMeasurementVectorSize, 
                          TFrequencyContainer>::MeasurementType&
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetBinMinFromValue(const unsigned int dimension, const float value ) const
{
  // If the value is lower than any of min value in the Histogram,
  // it returns the lowest min value
  if ( value <= this->m_Min[dimension][0] )
    {
    return this->m_Min[dimension][0];
    }

  // If the value is higher than any of min value in the Histogram,
  // it returns the highest min value
  if ( value >= m_Min[dimension][m_Size[dimension]-1] )
    {
    return m_Min[dimension][this->m_Size[dimension]-1];
    }

  for ( int i=0; i < this->m_Size[dimension]; i++ )
    {
    if (  (value >= this->m_Min[dimension][i])
          && (value <  this->m_Max[dimension][i])  )
      {
      return this->m_Min[dimension][i];
      }
    }
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline const typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementType&
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetBinMaxFromValue(const unsigned int dimension, const float value ) const
{
  // If the value is lower than any of max value in the Histogram,
  // it returns the lowest max value
  if ( value <= this->m_Max[dimension][0] )
    {
    return this->m_Max[dimension][0];
    }

  // If the value is higher than any of max value in the Histogram,
  // it returns the highest max value
  if ( value >= m_Max[dimension][m_Size[dimension]-1] )
    {
    return m_Max[dimension][this->m_Size[dimension]-1];
    }

  for ( int i = 0 ; i < this->m_Size[dimension]; i++ )
    {
    if (  (value >= this->m_Min[dimension][i])
          && (value <  this->m_Max[dimension][i])  )
      {
      return this->m_Max[dimension][i];
      }
    }
}

/*template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementVectorType&
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetHistogramMinFromValue(const MeasurementVectorType &measurement) 
{
  for ( int i = 0; i < MeasurementVectorSize; i++ )
    {
    m_TempMeasurementVector[i] = this->GetDimensionMinByValue(i,measurement[i]);
    }
  return m_TempMeasurementVector ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize,
                           TFrequencyContainer >::MeasurementVectorType&
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::GetHistogramMaxFromValue(const MeasurementVectorType &measurement) 
{
  for ( int i=0; i < MeasurementVectorSize; i++ )
    {
    m_TempMeasurementVector[i] = this->GetDimensionMaxByValue(i,measurement[i]);
    }
  return m_TempMeasurementVector ;

}
*/
template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize,
                           TFrequencyContainer >::MeasurementVectorType&
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetHistogramMinFromIndex(const IndexType &index) 
{
  for ( int i=0; i < MeasurementVectorSize; i++ )
    {
    m_TempMeasurementVector[i] = this->GetBinMin(i, index[i]) ;
    }
  return m_TempMeasurementVector ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize,
                           TFrequencyContainer >::MeasurementVectorType&
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetHistogramMaxFromIndex(const IndexType &index) 
{
  for ( int i=0; i < MeasurementVectorSize; i++ )
    {
    m_TempMeasurementVector[i] = this->GetBinMax(i, index[i]) ;
    }
  return m_TempMeasurementVector ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline const typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementVectorType &
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetMeasurementVector(const IndexType &index) const
{
  for ( unsigned int i = 0; i < MeasurementVectorSize; i++)
    {
    MeasurementType value = (m_Min[i][index[i]] + m_Max[i][index[i]]);
    m_TempMeasurementVector[i] =  static_cast< MeasurementType >( value / 2.0 );
    }
  return m_TempMeasurementVector ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline const typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementVectorType &
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetMeasurementVector(const InstanceIdentifier &id) const
{
  return this->GetMeasurementVector( this->GetIndex(id) ) ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline void
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::SetFrequency(const FrequencyType value) 
{
  typename Self::Iterator iter = this->Begin() ;
  typename Self::Iterator end = this->End() ;
  
  while ( iter != end )
    {
    iter.SetFrequency(value) ;
    ++iter ;
    }
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::SetFrequency(const IndexType &index, const FrequencyType value) 
{
  return this->SetFrequency( this->GetInstanceIdentifier(index), value) ;
}
  
template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::SetFrequency(const MeasurementVectorType &measurement, const FrequencyType value) 
{
  return this->SetFrequency( this->GetInstanceIdentifier(GetIndex(measurement)), value) ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline bool
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::IncreaseFrequency(const IndexType &index, const FrequencyType value)
{
  const bool result = 
      this->IncreaseFrequency( this->GetInstanceIdentifier(index), value) ;
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
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize,
                           TFrequencyContainer >::FrequencyType
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetFrequency(const IndexType &index) const
{
  return ( this->GetFrequency( this->GetInstanceIdentifier(index)) ) ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer>
inline typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::MeasurementType 
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetMeasurement(const unsigned long n, const unsigned int dimension) const
{
  return static_cast< MeasurementType >((m_Min[dimension][n] + 
                                         m_Max[dimension][n]) / 2) ; 
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::FrequencyType
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetFrequency(const unsigned long n, const unsigned int dimension) const
{
  InstanceIdentifier nextOffset = m_OffsetTable[dimension + 1] ;
  InstanceIdentifier current = m_OffsetTable[dimension] * n ;
  InstanceIdentifier includeLength = m_OffsetTable[dimension] ;
  InstanceIdentifier include ;
  InstanceIdentifier includeEnd ;
  InstanceIdentifier last = m_OffsetTable[VMeasurementVectorSize] ;

  FrequencyType frequency = 0 ;
  while (current < last)
    {
    include = current ;
    includeEnd = include + includeLength ;
    while(include < includeEnd)
      {
      frequency += GetFrequency(include) ;
      include++ ;
      }
    current += nextOffset ;
    }
  return frequency ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
inline typename Histogram< TMeasurement, VMeasurementVectorSize, 
                           TFrequencyContainer >::FrequencyType
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::GetTotalFrequency() const
{
  return m_FrequencyContainer->GetTotalFrequency() ;
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
double
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::Quantile(const unsigned int dimension, const double &p) const
{
  InstanceIdentifier n ;
  const unsigned int size = this->GetSize(dimension) ;
  double p_n_prev ;
  double p_n ;
  double f_n ;
  double cumulated = 0 ;
  double totalFrequency = double( this->GetTotalFrequency() ) ;
  double binProportion ;
  double min, max, interval ;

  if ( p < 0.5 )
    {
    n = 0 ;
    p_n = NumericTraits< double >::Zero ;
    do 
      {
      f_n = this->GetFrequency(n, dimension) ;
      cumulated += f_n ;
      p_n_prev = p_n ;
      p_n = cumulated / totalFrequency ;
      n++ ;
      } 
    while( n < size && p_n < p) ;

    binProportion = f_n / totalFrequency ;

    min = double( this->GetBinMin(dimension, n - 1) ) ;
    max = double( this->GetBinMax(dimension, n - 1) ) ;
    interval = max - min ;
    return min + ((p - p_n_prev) / binProportion) * interval ;
    }
  else
    {
    n = size - 1 ;
    InstanceIdentifier m = NumericTraits< InstanceIdentifier >::Zero;
    p_n      = NumericTraits< double >::One ;
    do 
      {
      f_n = this->GetFrequency(n, dimension) ;
      cumulated += f_n ;
      p_n_prev = p_n ;
      p_n = NumericTraits< double >::One - cumulated / totalFrequency ;
      n--;
      m++;
      } 
    while( m < size && p_n > p);

    binProportion = f_n / totalFrequency ;
    double min = double( this->GetBinMin(dimension, n + 1) ) ;
    double max = double( this->GetBinMax(dimension, n + 1) ) ;
    double interval = max - min ;
    return max - ((p_n_prev - p) / binProportion) * interval ;
    }
}

template< class TMeasurement, unsigned int VMeasurementVectorSize, 
          class TFrequencyContainer >
void 
Histogram< TMeasurement, VMeasurementVectorSize, TFrequencyContainer >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "OffsetTable: " << *m_OffsetTable << std::endl;
  if(m_ClipBinsAtEnds)
    {
    os << indent << "ClipBinsAtEnds: True" << std::endl;
    }
  else
    {
    os << indent << "ClipBinsAtEnds: False" << std::endl;
    }
  os << indent << "FrequencyContainerPointer: " << m_FrequencyContainer
     << std::endl;
}


} // end of namespace Statistics 
} // end of namespace itk 

#endif