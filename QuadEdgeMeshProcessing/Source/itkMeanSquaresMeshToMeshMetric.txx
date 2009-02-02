/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanSquaresMeshToMeshMetric.txx,v $
  Language:  C++
  Date:      $Date:  $
  Version:   $Revision:  $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMeanSquaresMeshToMeshMetric_txx
#define __itkMeanSquaresMeshToMeshMetric_txx

// First make sure that the configuration is available.
// This line can be removed once the optimized versions
// gets integrated into the main directories.
#include "itkConfigure.h"

#include "itkMeanSquaresMeshToMeshMetric.h"

namespace itk
{

/**
 * Constructor
 */
template <class TFixedMesh, class TMovingMesh> 
MeanSquaresMeshToMeshMetric<TFixedMesh,TMovingMesh>
::MeanSquaresMeshToMeshMetric()
{
  itkDebugMacro("Constructor");
}

/**
 * Get the match Measure
 */
template <class TFixedMesh, class TMovingMesh> 
typename MeanSquaresMeshToMeshMetric<TFixedMesh,TMovingMesh>::MeasureType
MeanSquaresMeshToMeshMetric<TFixedMesh,TMovingMesh>
::GetValue( const TransformParametersType & parameters ) const
{

  FixedMeshConstPointer fixedMesh = this->GetFixedMesh();

  if( !fixedMesh ) 
    {
    itkExceptionMacro( << "Fixed point set has not been assigned" );
    }


  PointIterator pointItr = fixedMesh->GetPoints()->Begin();
  PointIterator pointEnd = fixedMesh->GetPoints()->End();

  PointDataIterator pointDataItr = fixedMesh->GetPointData()->Begin();
  PointDataIterator pointDataEnd = fixedMesh->GetPointData()->End();

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    InputPointType  inputPoint;
    inputPoint.CastFrom( pointItr.Value() );
    OutputPointType transformedPoint = 
      this->m_Transform->TransformPoint( inputPoint );

    typedef typename InterpolatorType::RealType   RealType;

    // FIXME: if( this->m_Interpolator->IsInsideSurface( transformedPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = pointDataItr.Value();
      const RealType diff = movingValue - fixedValue; 
      measure += diff * diff; 
      this->m_NumberOfPixelsCounted++;
      }

    ++pointItr;
    ++pointDataItr;
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


/**
 * Get the Derivative Measure
 */
template < class TFixedMesh, class TMovingMesh> 
void
MeanSquaresMeshToMeshMetric<TFixedMesh,TMovingMesh>
::GetDerivative( const TransformParametersType & parameters,
                 DerivativeType & derivative  ) const
{

#ifdef MICHEL_LATER

  itkDebugMacro("GetDerivative( " << parameters << " ) ");
  
  if( !this->GetGradientMesh() )
    {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedMeshConstPointer fixedMesh = this->m_FixedMesh;

  if( !fixedMesh ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  const unsigned int MeshDimension = FixedMeshType::MeshDimension;


  typedef  itk::MeshRegionConstIteratorWithIndex<
    FixedMeshType> FixedIteratorType;

  typedef  itk::MeshRegionConstIteratorWithIndex<GradientMeshType> GradientIteratorType;


  FixedIteratorType ti( fixedMesh, this->GetFixedMeshRegion() );

  typename FixedMeshType::IndexType index;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    InputPointType inputPoint;
    fixedMesh->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedMeshMask && !this->m_FixedMeshMask->IsInside( inputPoint ) )
      {
      ++ti;
      continue;
      }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingMeshMask && !this->m_MovingMeshMask->IsInside( transformedPoint ) )
      {
      ++ti;
      continue;
      }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );

      const TransformJacobianType & jacobian =
        this->m_Transform->GetJacobian( inputPoint ); 

      
      const RealType fixedValue     = ti.Value();
      this->m_NumberOfPixelsCounted++;
      const RealType diff = movingValue - fixedValue; 

      // Get the gradient by NearestNeighboorInterpolation: 
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType,MovingMeshType::MeshDimension>
        MovingMeshContinuousIndexType;

      MovingMeshContinuousIndexType tempIndex;
      this->m_MovingMesh->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingMeshType::IndexType mappedIndex; 
      mappedIndex.CopyWithRound( tempIndex );

      const GradientPixelType gradient = 
        this->GetGradientMesh()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++)
        {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<MeshDimension; dim++)
          {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
          }
        derivative[par] += sum;
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
    for(unsigned int i=0; i<ParametersDimension; i++)
      {
      derivative[i] /= this->m_NumberOfPixelsCounted;
      }
    }

#endif

}


/*
 * Get both the match Measure and theDerivative Measure 
 */
template <class TFixedMesh, class TMovingMesh> 
void
MeanSquaresMeshToMeshMetric<TFixedMesh,TMovingMesh>
::GetValueAndDerivative(const TransformParametersType & parameters, 
                        MeasureType & value, DerivativeType  & derivative) const
{

#ifdef MICHEL_LATER

  itkDebugMacro("GetValueAndDerivative( " << parameters << " ) ");

  if( !this->GetGradientMesh() )
    {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedMeshConstPointer fixedMesh = this->m_FixedMesh;

  if( !fixedMesh ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  const unsigned int MeshDimension = FixedMeshType::MeshDimension;

  typedef  itk::MeshRegionConstIteratorWithIndex<
    FixedMeshType> FixedIteratorType;

  typedef  itk::MeshRegionConstIteratorWithIndex<GradientMeshType> GradientIteratorType;


  FixedIteratorType ti( fixedMesh, this->GetFixedMeshRegion() );

  typename FixedMeshType::IndexType index;

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    InputPointType inputPoint;
    fixedMesh->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedMeshMask && !this->m_FixedMeshMask->IsInside( inputPoint ) )
      {
      ++ti;
      continue;
      }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingMeshMask && !this->m_MovingMeshMask->IsInside( transformedPoint ) )
      {
      ++ti;
      continue;
      }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );

      const TransformJacobianType & jacobian =
        this->m_Transform->GetJacobian( inputPoint ); 

      
      const RealType fixedValue     = ti.Value();
      this->m_NumberOfPixelsCounted++;

      const RealType diff = movingValue - fixedValue; 
  
      measure += diff * diff;

      // Get the gradient by NearestNeighboorInterpolation: 
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType,MovingMeshType::MeshDimension>
        MovingMeshContinuousIndexType;

      MovingMeshContinuousIndexType tempIndex;
      this->m_MovingMesh->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingMeshType::IndexType mappedIndex; 
      mappedIndex.CopyWithRound( tempIndex );

      const GradientPixelType gradient = 
        this->GetGradientMesh()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++)
        {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<MeshDimension; dim++)
          {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
          }
        derivative[par] += sum;
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
    for(unsigned int i=0; i<ParametersDimension; i++)
      {
      derivative[i] /= this->m_NumberOfPixelsCounted;
      }
    measure /= this->m_NumberOfPixelsCounted;
    }

  value = measure;

#endif

}

} // end namespace itk

#endif
