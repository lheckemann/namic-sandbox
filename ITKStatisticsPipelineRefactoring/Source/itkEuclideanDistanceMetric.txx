/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkEuclideanDistanceMetric.txx,v $
  Language:  C++
  Date:      $Date: 2006/03/19 04:37:20 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkEuclideanDistanceMetric_txx
#define __itkEuclideanDistanceMetric_txx


namespace itk  { 
namespace Statistics  {

template< class TVector >
inline double
EuclideanDistanceMetric< TVector >
::Evaluate(const TVector &x) const
{
  MeasurementVectorSizeType 
    measurementVectorSize = this->GetMeasurementVectorSize();
  if(measurementVectorSize == 0) 
    {
    itkExceptionMacro( << "Please set the MeasurementVectorSize first" );
    }
  MeasurementVectorTraits::Assert( this->GetOrigin(), measurementVectorSize, 
    "EuclideanDistanceMetric::Evaluate Origin and input vector have different lengths");
  
  double temp, distance = NumericTraits< double >::Zero;
  
  for(unsigned int i = 0; i < measurementVectorSize; i++)
    {
    temp = this->GetOrigin()[i] - x[i];
    distance += temp * temp;
    }
  
  return vcl_sqrt(distance);
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
