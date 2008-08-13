/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToHistogramFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/01/17 15:13:42 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToHistogramFilter_txx
#define __itkImageToHistogramFilter_txx

#include "itkImageToHistogramFilter.h"


namespace itk { 
namespace Statistics {


template < class TImage >
ImageToHistogramFilter< TImage >
::ImageToHistogramFilter() 
{
  m_ImageToListAdaptor = AdaptorType::New();
  m_HistogramGenerator = GeneratorType::New();
  m_HistogramGenerator->SetListSample( m_ImageToListAdaptor );

  // set a usable default value
  SizeType size;
  size.Fill( 128 );
  this->SetNumberOfBins( size );

}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetInput( const ImageType * image ) 
{
  m_ImageToListAdaptor->SetImage( image );
}


template < class TImage >
const typename ImageToHistogramFilter< TImage >::HistogramType *
ImageToHistogramFilter< TImage >
::GetOutput() const
{
  return m_HistogramGenerator->GetOutput();
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::Compute() 
{
  m_HistogramGenerator->Update();
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetNumberOfBins( const SizeType & size ) 
{
  m_HistogramGenerator->SetNumberOfBins( size );
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetMarginalScale( double marginalScale )
{
  m_HistogramGenerator->SetMarginalScale( marginalScale );
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetHistogramMin(const MeasurementVectorType & histogramMin)
{
  m_HistogramGenerator->SetHistogramMin(histogramMin);
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetHistogramMax(const MeasurementVectorType & histogramMax)
{
  m_HistogramGenerator->SetHistogramMax(histogramMax);
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetAutoMinMax(bool autoMinMax)
{
  m_HistogramGenerator->SetAutoMinMax(autoMinMax);
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "ImageToListSample adaptor = " << m_ImageToListAdaptor << std::endl;
  os << "HistogramGenerator = " << m_HistogramGenerator << std::endl;
}


} // end of namespace Statistics 
} // end of namespace itk

#endif
