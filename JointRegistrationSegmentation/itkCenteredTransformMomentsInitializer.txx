/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCenteredTransformMomentsInitializer.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/26 02:10:04 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkCenteredTransformMomentsInitializer_txx
#define __itkCenteredTransformMomentsInitializer_txx

#include "itkCenteredTransformMomentsInitializer.h"

namespace itk
{


template < class TTransform, class TFixedImage, class TMovingImage >
CenteredTransformMomentsInitializer<TTransform, TFixedImage, TMovingImage >
::CenteredTransformMomentsInitializer() 
{
  m_FixedCalculator  = FixedImageCalculatorType::New();
  m_MovingCalculator = MovingImageCalculatorType::New();
}



template < class TTransform, class TFixedImage, class TMovingImage >
void 
CenteredTransformMomentsInitializer<TTransform, TFixedImage, TMovingImage >
::InitializeTransform() const
{
  // Sanity check
  if( !m_FixedImage )
    {
    itkExceptionMacro( "Fixed Image has not been set" );
    return;
    }
  if( !m_MovingImage )
    {
    itkExceptionMacro( "Moving Image has not been set" );
    return;
    }
  if( !m_Transform )
    {
    itkExceptionMacro( "Transform has not been set" );
    return;
    }

  // If images come from filters, then update those filters.
  if( m_FixedImage->GetSource() )
    { 
    m_FixedImage->GetSource()->Update();
    }
  if( m_MovingImage->GetSource() )
    { 
    m_MovingImage->GetSource()->Update();
    }


  InputPointType    rotationCenter;
  OutputVectorType  translationVector;

  m_FixedCalculator->SetImage(  m_FixedImage );
  m_FixedCalculator->Compute();

  m_MovingCalculator->SetImage( m_MovingImage );
  m_MovingCalculator->Compute();
  
  typename FixedImageCalculatorType::VectorType fixedCenter =
    m_FixedCalculator->GetCenterOfGravity();

  typename MovingImageCalculatorType::VectorType movingCenter =
    m_MovingCalculator->GetCenterOfGravity();

  for( unsigned int i=0; i<InputSpaceDimension; i++)
    {
    rotationCenter[i]    = fixedCenter[i];
    translationVector[i] = movingCenter[i] - fixedCenter[i];
    }

  m_Transform->SetCenter( rotationCenter );

  m_Transform->SetTranslation( translationVector );

}
  

      

template < class TTransform, class TFixedImage, class TMovingImage >
void 
CenteredTransformMomentsInitializer<TTransform, TFixedImage, TMovingImage >
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

  os << indent << "FixedImage   = " << std::endl;
  if (m_FixedImage)
    { 
    os << indent << m_FixedImage  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }      

  os << indent << "MovingImage   = " << std::endl;
  if (m_MovingImage)
    { 
    os << indent << m_MovingImage  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }      

  os << indent << "MovingMomentCalculator   = " << std::endl;
  if (m_MovingCalculator)
    { 
    os << indent << m_MovingCalculator  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }      

  os << indent << "FixedMomentCalculator   = " << std::endl;
  if (m_FixedCalculator)
    { 
    os << indent << m_FixedCalculator  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }      

}
 
}  // namespace itk

#endif /* __itkCenteredTransformMomentsInitializer_txx */
