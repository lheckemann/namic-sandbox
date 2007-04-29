/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptor.txx,v $
  Language:  C++
  Date:      $Date: 2006/10/14 19:58:32 $
  Version:   $Revision: 1.23 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToListSampleAdaptor_txx
#define __itkImageToListSampleAdaptor_txx

namespace itk { 
namespace Statistics {

template < class TImage, class TMeasurementVector >
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::ImageToListSampleAdaptor()
{
  m_UseBuffer = true;
  m_Image = 0;
  m_PixelContainer = 0;
}

template < class TImage, class TMeasurementVector >
inline const TMeasurementVector &
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::GetMeasurementVector(const InstanceIdentifier &id) const
{
  if ( m_UseBuffer )
    {
    return *( reinterpret_cast< const MeasurementVectorType* >(&(*m_PixelContainer)[id]) );
    }
  else
    {
    return *(reinterpret_cast< const MeasurementVectorType* >
             ( &(m_Image->GetPixel( m_Image->ComputeIndex( id ) ) ) ) );
    }
}

/** returns the number of measurement vectors in this container*/
template < class TImage, class TMeasurementVector >
inline unsigned int
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::Size() const
{
  return m_PixelContainer->Size();
}

template < class TImage, class TMeasurementVector >
inline typename ImageToListSampleAdaptor< TImage, TMeasurementVector >::FrequencyType
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::GetFrequency(const InstanceIdentifier &) const 
{
  return NumericTraits< FrequencyType >::One;
}


template < class TImage, class TMeasurementVector >
void
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Image: ";
  if ( m_Image.IsNotNull() )
    {
    os << m_Image << std::endl;
    }
  else
    {
    os << "not set." << std::endl;
    }
   
  os << indent << "PixelContainer: ";
  if ( m_PixelContainer.IsNotNull() )
    {
    os << m_PixelContainer << std::endl;
    }
  else
    {
    os << "not set." << std::endl;
    }
  
  os << indent << "Use buffer: " << m_UseBuffer << std::endl;
}

template < class TImage, class TMeasurementVector >
void
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::SetImage(const TImage* image) 
{ 
  m_Image = image; 
  m_PixelContainer = image->GetPixelContainer();
  m_ImageBeginIndex = image->GetLargestPossibleRegion().GetIndex();

  for ( unsigned int i = 0; i < TImage::ImageDimension; ++i )
    {
    m_ImageEndIndex[i] = 
      m_ImageBeginIndex[i] + image->GetLargestPossibleRegion().GetSize()[i] - 1;
    }

  if ( strcmp( m_Image->GetNameOfClass(), "Image" ) != 0 )
    {
    m_UseBuffer = false;
    }
  else
    {
    m_UseBuffer = true;
    }
}

template < class TImage, class TMeasurementVector >
const TImage*
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::GetImage() const
{
  return m_Image.GetPointer(); 
}  

template < class TImage, class TMeasurementVector >
typename ImageToListSampleAdaptor< TImage, TMeasurementVector >::TotalFrequencyType
ImageToListSampleAdaptor< TImage, TMeasurementVector >
::GetTotalFrequency() const
{ 
  return this->Size(); 
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
