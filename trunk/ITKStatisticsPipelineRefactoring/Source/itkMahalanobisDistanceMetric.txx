/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMahalanobisDistanceMetric.txx,v $
  Language:  C++
  Date:      $Date: 2008/01/15 12:37:56 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMahalanobisDistanceMetric_txx
#define __itkMahalanobisDistanceMetric_txx

#include "itkMahalanobisDistanceMetric.h"

namespace itk {
namespace Statistics {

template < class TVector >
MahalanobisDistanceMetric< TVector >
::MahalanobisDistanceMetric():
  m_Epsilon( 1e-100 ),
  m_DoubleMax( 1e+20 )
{
  MeasurementVectorSizeType size;
  size = this->GetMeasurementVectorSize();

  this->m_Covariance.set_size( size,size );
  this->m_InverseCovariance.set_size( size,size );

  m_Covariance.set_identity();
  m_InverseCovariance.set_identity();
}

template < class TVector >
void
MahalanobisDistanceMetric< TVector >
::SetMean(const MeanVectorType & mean)
{
  Superclass::SetOrigin( mean );
}

template < class TVector >
const typename
MahalanobisDistanceMetric< TVector >::MeanVectorType &
MahalanobisDistanceMetric< TVector >
::GetMean() const
{
  return Superclass::GetOrigin();
}

template< class TVector >
void 
MahalanobisDistanceMetric< TVector >
::SetMeasurementVectorSize( MeasurementVectorSizeType size )
{
  this->Superclass::SetMeasurementVectorSize( size );
  this->m_Covariance.set_size( size,size );
  this->m_InverseCovariance.set_size( size,size );

  this->m_Covariance.set_identity();
  this->m_InverseCovariance.set_identity();
  this->Modified();
}


template < class TVector >
void
MahalanobisDistanceMetric< TVector >
::SetCovariance(const CovarianceMatrixType &cov)
{
  if( this->GetMeasurementVectorSize() != 0 )
    {
    if( cov.rows() != this->GetMeasurementVectorSize() ||
        cov.cols() != this->GetMeasurementVectorSize())
      {
      itkExceptionMacro( << "Size of the covariance matrix must be same as the length of"
          << " the measurement vector.");
      }
    }

  m_Covariance = cov;
  this->CalculateInverseCovariance();
}


template < class TVector >
void
MahalanobisDistanceMetric< TVector >
::SetInverseCovariance(const CovarianceMatrixType &invcov)
{
  if( this->GetMeasurementVectorSize() != 0 )
    {
    if( invcov.rows() != this->GetMeasurementVectorSize() ||
        invcov.cols() != this->GetMeasurementVectorSize() )
      {
      itkExceptionMacro( << "Size of the covariance matrix xcmust be same as the length of"
          << " each measurement vector.");
      }
    }

  // use the inverse computation
  m_Covariance = invcov;
  this->CalculateInverseCovariance();
  m_Covariance = m_InverseCovariance;
  m_InverseCovariance = invcov;
}


template < class TVector >
void
MahalanobisDistanceMetric< TVector >
::CalculateInverseCovariance()
{
  // pack the cov matrix from in_model to tmp_cov_mat
  double cov_sum = 0;
  for(unsigned int band_x = 0; band_x < m_Covariance.cols(); band_x++)
    {
    for(unsigned int band_y = 0; band_y < m_Covariance.rows(); band_y++)
      {
      cov_sum += vnl_math_abs( m_Covariance[band_x][band_y] );
      }
    }
  // check if it is a zero covariance, if it is, we make its
  // inverse as an identity matrix with diagonal elements as
  // a very large number; otherwise, inverse it
  if( cov_sum < m_Epsilon )
    {
    m_InverseCovariance.set_size( m_Covariance.rows(), m_Covariance.cols() );
    m_InverseCovariance.set_identity();
    m_InverseCovariance *= m_DoubleMax;
    }
  else
    {
    // check if num_bands == 1, if it is, we just use 1 to divide it
    if( m_Covariance.rows() < 2 )
      {
      m_InverseCovariance.set_size(1,1);
      m_InverseCovariance[0][0] = 1.0 / m_Covariance[0][0];
      }
    else
      {
      m_InverseCovariance = vnl_matrix_inverse<double>(m_Covariance);
      }
    }// end inverse calculations

}

template < class TVector >
double
MahalanobisDistanceMetric< TVector >
::Evaluate(const MeasurementVectorType &measurement) const
{

  vnl_matrix < double >  tempVec;
  vnl_matrix < double >  tempMat;

  tempVec.set_size( 1, this->GetMeasurementVectorSize());
  tempMat.set_size( 1, this->GetMeasurementVectorSize());

  // Compute |y - mean |
  for ( unsigned int i = 0; i < this->GetMeasurementVectorSize(); i++ )
    {
    tempVec[0][i] = measurement[i] - this->GetOrigin()[i];
    }

  // Compute |y - mean | * inverse(cov)
  tempMat= tempVec * m_InverseCovariance;

  // Compute |y - mean | * inverse(cov) * |y - mean|^T
  double temp;
  temp = vcl_sqrt( dot_product( tempMat.as_ref(), tempVec.as_ref()) );

  return temp;
}
template< class TVector >
inline double
MahalanobisDistanceMetric< TVector >
::Evaluate(const MeasurementVectorType &x1, const MeasurementVectorType &x2) const
{
  itkExceptionMacro( << "Mahalanobis distance is not defined between two measurement vectors " );

  double distance = 0.0;

  return distance;
} 
template < class TVector >
void
MahalanobisDistanceMetric< TVector >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Covariance:        " << std::endl;
  os << m_Covariance << std::endl;
  os << indent << "Inverse covariance:        " << std::endl;
  os << m_InverseCovariance << std::endl;
}
} // end namespace Statistics
} // end of namespace itk


#endif
