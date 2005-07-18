/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkEuclideanDistance.txx,v $
  Language:  C++
  Date:      $Date: 2004/09/11 00:15:13 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkEuclideanDistance_txx
#define __itkEuclideanDistance_txx


namespace itk{ 
namespace Statistics{

template< class TVector >
inline double
EuclideanDistance< TVector >
::Evaluate(const TVector &x1, const TVector &x2) const
{
  if( x1.Size() != x2.Size() )
    {
    itkExceptionMacro( << "Vector lengths must be equal." );
    }
  
  double temp, distance = NumericTraits< double >::Zero ;
  
  for(unsigned int i = 0 ; i < MeasurementVectorTraits< TVector >::GetSize( &x1 ); i++ )
    {
    temp = x1[i] - x2[i] ;
    distance += temp * temp ;
    }
  
  return sqrt(distance) ;
}


template< class TVector >
inline double
EuclideanDistance< TVector >
::Evaluate(const TVector &x) const
{
  MeasurementVectorSizeType 
    measurementVectorSize = this->GetMeasurementVectorSize();
  if(measurementVectorSize == 0) 
    {
    itkExceptionMacro( << "Please set the MeasurementVectorSize first" );
    }
  if( measurementVectorSize != 
      MeasurementVectorTraits< OriginType >::GetSize( &m_Origin ) )
    {
    itkExceptionMacro( << "Origin and input vector have different lengths" );
    }
  
  double temp, distance = NumericTraits< double >::Zero ;
  
  for(unsigned int i = 0 ; i < measurementVectorSize ; i++ )
    {
    temp = this->GetOrigin()[i] - x[i] ;
    distance += temp * temp ;
    }
  
  return sqrt(distance) ;
}

template< class TVector >
inline double
EuclideanDistance< TVector >
::Evaluate(const ValueType &a, const ValueType &b) const
{
  double temp = a - b ;
  return sqrt(temp * temp) ;
}

template< class TVector >
inline bool
EuclideanDistance< TVector >
::IsWithinRange(const TVector &x, const double radius) const 
{
  MeasurementVectorSizeType 
    measurementVectorSize = this->GetMeasurementVectorSize();
  if(measurementVectorSize == 0) 
    {
    itkExceptionMacro( << "Please set the MeasurementVectorSize first" );
    }
  if( measurementVectorSize != m_Origin )
    {
    itkExceptionMacro( << "Origin and input vector have different lengths" );
    }
  
  double squaredRadius = radius * radius ;
  double sum = NumericTraits< double >::Zero ;
  double temp ;
  for ( unsigned int i = measurementVectorSize ; i > 0 ; --i )
    {
    temp = this->Evaluate( this->GetOrigin()[i-1], x[i-1] ) ;
    sum += temp * temp ;
    if (sum > squaredRadius)
      {
      return false ;
      }
    }
  return true ;
}

} // end of namespace Statistics 
} // end of namespace itk

#endif







