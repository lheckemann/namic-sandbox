/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFinslerActiveContourLevelSetImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:31 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFinslerActiveContourLevelSetImageFilter_txx_
#define __itkFinslerActiveContourLevelSetImageFilter_txx_

#include "itkFinslerActiveContourLevelSetImageFilter.h"

namespace itk {


template <class TInputImage, class TFeatureImage, class TOutputType>
FinslerActiveContourLevelSetImageFilter<TInputImage, TFeatureImage, TOutputType>
::FinslerActiveContourLevelSetImageFilter()
{
  /* Instantiate a geodesic active contour function and set it as the segmentation function. */
  m_FinslerActiveContourFunction = FinslerActiveContourFunctionType::New();

  this->SetSegmentationFunction( m_FinslerActiveContourFunction );

  /* Turn off interpolation. */
  this->InterpolateSurfaceLocationOff();
}
 
template <class TInputImage, class TFeatureImage, class TOutputType>
void
FinslerActiveContourLevelSetImageFilter<TInputImage, TFeatureImage, TOutputType>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << "FinslerActiveContourFunction: " << m_FinslerActiveContourFunction.GetPointer();
}


template <class TInputImage, class TFeatureImage, class TOutputType>
void
FinslerActiveContourLevelSetImageFilter<TInputImage, TFeatureImage, TOutputType>
::GenerateData()
{

  // Make sure the SpeedImage is setup for the case when PropagationScaling
  // is zero
  if ( this->GetSegmentationFunction() && 
       this->GetSegmentationFunction()->GetPropagationWeight() == 0 )
    {
    this->GetSegmentationFunction()->AllocateSpeedImage();
    this->GetSegmentationFunction()->CalculateSpeedImage();
    }

  // Continue with Superclass implementation
  Superclass::GenerateData();

}

}// end namespace itk




#endif
