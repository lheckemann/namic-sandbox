/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToImageWithGradientMetric.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/22 03:05:02 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkImageToImageWithGradientMetric_txx
#define _itkImageToImageWithGradientMetric_txx


#include "itkImageToImageWithGradientMetric.h"


namespace itk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage> 
ImageToImageWithGradientMetric<TFixedImage,TMovingImage>
::ImageToImageWithGradientMetric()
{
  m_GradientImage = 0; // will receive the output of the filter;
  m_ComputeGradient = true; // metric computes gradient by default
  m_GradientImage = NULL; // computed at initialization
}






/*
 * Initialize
 */
template <class TFixedImage, class TMovingImage> 
void
ImageToImageWithGradientMetric<TFixedImage,TMovingImage>
::Initialize(void) throw ( ExceptionObject )
{

  // Note that this method will already invoke an InitializeEvent. 
  // So, an observer to this class will get two of them (the one
  // from this invocation and the one at the end of this method).
  //
  this->Superclass::Initialize();

  if ( m_ComputeGradient )
    {

    GradientImageFilterPointer gradientFilter
      = GradientImageFilterType::New();

    gradientFilter->SetInput( m_MovingImage );

    const typename MovingImageType::SpacingType&
      spacing = m_MovingImage->GetSpacing();
    double maximumSpacing=0.0;
    for(unsigned int i=0; i<MovingImageDimension; i++)
      {
      if( spacing[i] > maximumSpacing )
        {
        maximumSpacing = spacing[i];
        }
      }
    gradientFilter->SetSigma( maximumSpacing );
    gradientFilter->SetNormalizeAcrossScale( true );

    gradientFilter->Update();

    m_GradientImage = gradientFilter->GetOutput();

    }

  // If there are any observers on the metric, call them to give the
  // user code a chance to set parameters on the metric
  this->InvokeEvent( InitializeEvent() );
}
 

/*
 * PrintSelf
 */
template <class TFixedImage, class TMovingImage> 
void
ImageToImageWithGradientMetric<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "ComputeGradient: "
     << static_cast<typename NumericTraits<bool>::PrintType>(m_ComputeGradient)
     << std::endl;
  os << indent << "Gradient Image: " << m_GradientImage.GetPointer()   << std::endl;
}


} // end namespace itk

#endif
