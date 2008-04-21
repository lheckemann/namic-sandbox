/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBSplineDeformableTransformInitializer.txx,v $
  Language:  C++
  Date:      $Date: 2006/12/11 20:41:23 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBSplineDeformableTransformInitializer_txx
#define __itkBSplineDeformableTransformInitializer_txx

#include "itkBSplineDeformableTransformInitializer.h"

namespace itk
{


template < class TTransform, class TImage >
BSplineDeformableTransformInitializer<TTransform, TImage >
::BSplineDeformableTransformInitializer() 
{
}


template < class TTransform, class TImage >
void 
BSplineDeformableTransformInitializer<TTransform, TImage >
::InitializeTransform() const
{
  // Sanity check
  if( !m_Image )
    {
    itkExceptionMacro( "Reference Image has not been set" );
    return;
    }

  if( !m_Transform )
    {
    itkExceptionMacro( "Transform has not been set" );
    return;
    }

  // If the image come from a filter, then update that filter.
  if( m_Image->GetSource() )
    { 
    m_Image->GetSource()->Update();
    }

  unsigned int numberOfGridNodesInOneDimensionCoarse = 5;

  typedef DeformableTransformType::RegionType RegionType;
  RegionType bsplineRegion;

  RegionType::SizeType   gridSizeOnImage;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;

  gridSizeOnImage.Fill( numberOfGridNodesInOneDimensionCoarse );
  gridBorderSize.Fill( SplineOrder );

  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef TransformType::SpacingType SpacingType;
  const SpacingType & spacing = m_Image->GetSpacing();

  typedef DeformableTransformType::OriginType OriginType;
  OriginType origin = fixedImage->GetOrigin();;

  FixedImageType::SizeType fixedImageSize = fixedRegion.GetSize();

  SpacingType gridSpacing;

  for(unsigned int r=0; r<ImageDimension; r++)
    {
    gridSpacing[r] = spacing[r] * 
      static_cast<double>(fixedImageSize[r]  - 1)  / 
      static_cast<double>(gridSizeOnImage[r] - 1);
    }

  FixedImageType::DirectionType gridDirection = fixedImage->GetDirection();
  SpacingType gridOriginOffset = gridDirection * gridSpacing;

  OriginType gridOrigin = origin - gridOriginOffset; 

  m_Transform->SetGridSpacing( gridSpacing );
  m_Transform->SetGridOrigin( gridOrigin );
  m_Transform->SetGridDirection( gridDirection );
  m_Transform->SetGridRegion( bsplineRegion );
 
}
  

template < class TTransform, class TImage >
void 
BSplineDeformableTransformInitializer<TTransform, TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
     
  os << indent << "Transform   = " << std::endl;
  if (m_Transform)
    { 
    os << indent << m_Transform  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

  os << indent << "Image   = " << std::endl;
  if (m_Image)
    { 
    os << indent << m_Image  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

}
 
}  // namespace itk

#endif /* __itkBSplineDeformableTransformInitializer_txx */
