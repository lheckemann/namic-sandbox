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
  this->m_GridSizeInsideTheImage.Fill( 5 );
}


template < class TTransform, class TImage >
void 
BSplineDeformableTransformInitializer<TTransform, TImage >
::InitializeTransform() const
{
  // Sanity check
  if( ! this->m_Image )
    {
    itkExceptionMacro( "Reference Image has not been set" );
    return;
    }

  if( ! this->m_Transform )
    {
    itkExceptionMacro( "Transform has not been set" );
    return;
    }

  // If the image come from a filter, then update that filter.
  if( this->m_Image->GetSource() )
    { 
    this->m_Image->GetSource()->Update();
    }

  typedef typename TransformType::RegionType RegionType;

  typename RegionType::SizeType   gridBorderSize;
  typename RegionType::SizeType   totalGridSize;

  gridBorderSize.Fill( TransformType::SplineOrder );

  totalGridSize = this->m_GridSizeInsideTheImage;
  totalGridSize += gridBorderSize;

  RegionType gridRegion;
  gridRegion.SetSize( totalGridSize );

  typedef typename TransformType::SpacingType SpacingType;
  const SpacingType & imageSpacing = this->m_Image->GetSpacing();

  typedef typename TransformType::OriginType OriginType;
  const OriginType & imageOrigin = this->m_Image->GetOrigin();;

  const typename TransformType::RegionType & imageRegion = 
    this->m_Image->GetLargestPossibleRegion();

  typename ImageType::SizeType fixedImageSize = imageRegion.GetSize();

  SpacingType gridSpacing;

  for( unsigned int r = 0; r < SpaceDimension; r++ )
    {
    gridSpacing[r] = imageSpacing[r] * 
      static_cast<double>(fixedImageSize[r]  - 1)  / 
      static_cast<double>(this->m_GridSizeInsideTheImage[r] - 1);
    }

  typename ImageType::DirectionType gridDirection = this->m_Image->GetDirection();
  SpacingType gridOriginOffset = gridDirection * gridSpacing;

  OriginType gridOrigin = imageOrigin - gridOriginOffset; 

  this->m_Transform->SetGridRegion( gridRegion );
  this->m_Transform->SetGridOrigin( gridOrigin );
  this->m_Transform->SetGridSpacing( gridSpacing );
  this->m_Transform->SetGridDirection( gridDirection );
 
}
  

template < class TTransform, class TImage >
void 
BSplineDeformableTransformInitializer<TTransform, TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
     
  os << indent << "Transform   = " << std::endl;
  if( this->m_Transform )
    { 
    os << indent << this->m_Transform  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

  os << indent << "Image   = " << std::endl;
  if( this->m_Image )
    { 
    os << indent << this->m_Image  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

}
 
}  // namespace itk

#endif /* __itkBSplineDeformableTransformInitializer_txx */
