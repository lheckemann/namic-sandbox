/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKullbackLeiblerDivergenceImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:27 $
  Version:   $Revision: 1.46 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkKullbackLeiblerDivergenceImageToImageMetric_txx
#define _itkKullbackLeiblerDivergenceImageToImageMetric_txx

#include "itkKullbackLeiblerDivergenceImageToImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage> 
KullbackLeiblerDivergenceImageToImageMetric<TFixedImage,TMovingImage>
::KullbackLeiblerDivergenceImageToImageMetric()
{
  itkDebugMacro("Constructor");
}

/*
 * Get the match Measure
 */
template <class TFixedImage, class TMovingImage> 
typename KullbackLeiblerDivergenceImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
KullbackLeiblerDivergenceImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  itkDebugMacro("GetValue( " << parameters << " ) ");

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  const unsigned int numberOfClasses = fixedImage->GetVectorLength();

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const RealType epsilon = NumericTraits< RealType >::min();
  const RealType logEpsilon = log( epsilon );

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    typename Superclass::InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) )
      {
      ++ti;
      continue;
      }

    typename Superclass::OutputPointType transformedPoint = 
                    this->m_Transform->TransformPoint( inputPoint );

    if(  this->m_MovingImageMask && 
        !this->m_MovingImageMask->IsInside( transformedPoint ) )
      {
      ++ti;
      continue;
      }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();
      this->m_NumberOfPixelsCounted++;

      RealType sumProduct = NumericTraits< RealType >::ZeroValue();
      RealType sumMoving  = NumericTraits< RealType >::ZeroValue();

      for( unsigned int i = 0; i < numberOfClasses; i++)
        {
        if( fixedValue[i] > epsilon )
          {
          if( movingValue[i] > epsilon )
            {
            sumMoving  += movingValue[i];
            sumProduct += log( movingValue[i] ) * fixedValue[i]; 
            }
          else
            {
            sumMoving  += epsilon;
            sumProduct += logEpsilon * fixedValue[i]; 
            }
          }
        else
          {
          sumMoving  += movingValue[i];
          }
        }

      if( sumMoving >= epsilon )
        {
        measure += sumProduct - log( sumMoving );
        }

      }

    ++ti;
    }

  if( !this->m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
    }
  else
    {
    measure /= this->m_NumberOfPixelsCounted;
    }

  return measure;

}



} // end namespace itk


#endif
