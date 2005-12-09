/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDummyImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/03/08 20:29:20 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkDummyImageFilter_txx
#define _itkDummyImageFilter_txx

#include "itkDummyImageFilter.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputImage>
DummyImageFilter<TInputImage, TOutputImage>
::DummyImageFilter()
{

}


/**
 * Set the mask
 */
template <class TInputImage, class TOutputImage>
void
DummyImageFilter<TInputImage, TOutputImage>
::SetMask()
{
//   m_MembershipFunctions.push_back( functionSmartPointer );
}


/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
DummyImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
DummyImageFilter<TInputImage, TOutputImage>
::GenerateData()
{

  // SETUP INPUT IMAGE
  const InputImageType * inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetBufferedRegion();
  typename InputImageType::SpacingType  imageSpacing = inputImage->GetSpacing();
  typename InputImageType::PointType    imageOrigin  = inputImage->GetOrigin();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

//  this->SetOutput( inputImage ); does this work?

} // end 
} // end namespace itk

#endif
