/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAffineInvariantCurvatureFlowFunction.txx,v $
  Language:  C++
  Date:      $Date: 2005/03/10 21:31:27 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAffineInvariantCurvatureFlowFunction_txx_
#define __itkAffineInvariantCurvatureFlowFunction_txx_
#include "itkAffineInvariantCurvatureFlowFunction.h"

#include "vnl/vnl_math.h"

namespace itk {

/*
 * Constructor
 */
template<class TImage>
AffineInvariantCurvatureFlowFunction<TImage>
::AffineInvariantCurvatureFlowFunction()
{

  RadiusType r;
  unsigned int j;
  for( j = 0; j < ImageDimension; j++ )
    {
    r[j] = 1;
    }

  this->SetRadius(r);

  m_TimeStep  = 0.05f;

}


/*
 * Compute the global time step
 */
template<class TImage>
typename AffineInvariantCurvatureFlowFunction<TImage>::TimeStepType
AffineInvariantCurvatureFlowFunction<TImage>
::ComputeGlobalTimeStep( void *itkNotUsed(gd) ) const
{

  return this->GetTimeStep();


  // \todo compute timestep based on CFL condition
/*
  GlobalDataStruct *globalData = (GlobalDataStruct *)gd;
  TimeStepType dt;

  if ( globalData->m_MaxChange > 0.0 )
    {
    dt = 1.0 / globalData->m_MaxChange;
    }
  else
    {
    dt = 0.0;
    }

  return dt;
*/

}


/*
 * Update the solution at pixels which lies on the data boundary.
 */
template<class TImage>
typename AffineInvariantCurvatureFlowFunction<TImage>::PixelType
AffineInvariantCurvatureFlowFunction<TImage>
::ComputeUpdate(const NeighborhoodType &it, void * itkNotUsed(gd),
                const FloatOffsetType& itkNotUsed(offset))
{
  PixelType firstderiv[ImageDimension];
  PixelType secderiv[ImageDimension];
  PixelType crossderiv[ImageDimension][ImageDimension];
  unsigned long center;
  unsigned long stride[ImageDimension];
  unsigned int i,j;

  // get the center pixel position
  center = it.Size() / 2;

  // cache the stride for each dimension
  for( i = 0; i < ImageDimension; i++ )
    {
    stride[i] = it.GetStride( (unsigned long) i );
    }

  PixelType magnitudeSqr = 0.0;
  for( i = 0; i < ImageDimension; i++ )
    {

    // compute first order derivatives
    firstderiv[i] = 0.5 * ( it.GetPixel(center + stride[i]) - 
                            it.GetPixel(center - stride[i]) );

    // compute second order derivatives
    secderiv[i] = ( it.GetPixel(center + stride[i]) - 
                    2 * it.GetPixel(center) + it.GetPixel( center - stride[i] ) );

    // compute cross derivatives
    for( j = i + 1; j < ImageDimension; j++ )
      {
      crossderiv[i][j] = 0.25 * (
        it.GetPixel( center - stride[i] - stride[j] ) 
        - it.GetPixel( center - stride[i] + stride[j] )
        - it.GetPixel( center + stride[i] - stride[j] ) 
        + it.GetPixel( center + stride[i] + stride[j] ) );
      }
      
    // accumlate the gradient magnitude squared
    //magnitudeSqr += vnl_math_sqr( (double)firstderiv[i] );

    }

  /*if ( magnitudeSqr < 1e-9 )
    {
    return NumericTraits<PixelType>::Zero; 
    }
    */
    
  // compute the update value = mean curvature^(1/3) * magnitude
  // which simplifies to ( (dx^2 * dyy) - (2 * dx * dy * dxy) + (dy^2 * dxx) )^1/3 
  PixelType update = 0.0;
  PixelType temp;

  // accumulate dx^2 * (dyy + dzz) terms
  for( i = 0; i < ImageDimension; i++ )
    {
    temp = 0.0;
    for( j = 0; j < ImageDimension; j++ )
      {
      if( j == i ) continue;
      temp += secderiv[j];
      }
    
    update += temp * vnl_math_sqr( (double)firstderiv[i] );
    }

  // accumlate -2 * dx * dy * dxy terms
  for( i = 0; i < ImageDimension; i++ )
    {
    for( j = i + 1; j < ImageDimension; j++ )
      {
      update -= 2 * firstderiv[i] * firstderiv[j] *
        crossderiv[i][j];
      }
    }
   
  //update /= magnitudeSqr;

  // the final result is raised to the 1/3
  // we are being conservative here with the casts, but just to make sure
  float exponent = (float)1.0/(float)3.0;
  
  // power doesn't handle negative numbers, so we 
  // need to handle that case
  if(update<0)
    {
    update = -update;
    update = (PixelType)pow(update,exponent);
    update = -update;
    }  else {
    update = (PixelType)pow(update,exponent);
    }
  
  // \todo compute timestep based on CFL condition
/*
  GlobalDataStruct *globalData = (GlobalDataStruct *)gd;
  globalData->m_MaxChange =
    vnl_math_max( globalData->m_MaxChange, vnl_math_abs(update) );
*/
  return update;

}

} // end namespace itk

#endif
