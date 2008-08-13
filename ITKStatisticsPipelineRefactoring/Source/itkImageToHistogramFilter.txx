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
  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(1);

  this->ProcessObject::SetNthOutput(0, this->MakeOutput(0).GetPointer() );

  this->m_ImageToListAdaptor = AdaptorType::New();
  this->m_HistogramGenerator = GeneratorType::New();
  this->m_HistogramGenerator->SetListSample( this->m_ImageToListAdaptor );
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::SetInput( const ImageType * image ) 
{
  this->ProcessObject::SetNthInput(0, const_cast< ImageType * >( image ) );
}


template< class TImage >
const TImage *
ImageToHistogramFilter< TImage >
::GetInput( ) const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }

  return static_cast<const ImageType * >(this->ProcessObject::GetInput(0) );
}


template < class TImage >
const typename ImageToHistogramFilter< TImage >::HistogramType *
ImageToHistogramFilter< TImage >
::MakeOutput(unsigned int itkNotUsed(idx))
{
  typename HistogramType::Pointer output = HistogramType::New();
  return static_cast< DataObject * >( output );
}


template < class TImage >
const typename ImageToHistogramFilter< TImage >::HistogramType *
ImageToHistogramFilter< TImage >
::GetOutput() const
{
  const HistogramType * output = 
    static_cast< const HistogramType * >( this->ProcessObject::GetOutput(0));
  return output;
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::GenerateData() 
{
  this->m_ImageToListAdaptor->SetImage( this->GetInput( ) );

  this->m_HistogramGenerator->SetNumberOfBins( this->m_HistogramSize );
  this->m_HistogramGenerator->SetMarginalScale( this->m_MarginalScale );
  this->m_HistogramGenerator->SetAutoMinMax( this->m_AutoMinMax );
  this->m_HistogramGenerator->SetHistogramMin( this->m_HistogramMin );
  this->m_HistogramGenerator->SetHistogramMax( this->m_HistogramMax );

  this->m_HistogramGenerator->GraftOutput( 
    static_cast< ListSampleType * >( this->ProcessObject::GetOutput(0)) );

  this->m_HistogramGenerator->Update();

  /** graft the minipipeline output back into this filter's output */
  this->GraftOutput( this->m_HistogramGenerator->GetOutput() );
}


template < class TImage >
void
ImageToHistogramFilter< TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "ImageToListSample adaptor = " << this->m_ImageToListAdaptor << std::endl;
  os << "HistogramGenerator = " << this->m_HistogramGenerator << std::endl;
}


} // end of namespace Statistics 
} // end of namespace itk

#endif
