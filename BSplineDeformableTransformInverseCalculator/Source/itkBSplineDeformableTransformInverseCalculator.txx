/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBSplineDeformableTransformInverseCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2006/12/11 20:41:23 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBSplineDeformableTransformInverseCalculator_txx
#define __itkBSplineDeformableTransformInverseCalculator_txx

#include "itkBSplineDeformableTransformInverseCalculator.h"

namespace itk
{


template < class TDirectBSplineDeformableTransform, class TInverseBSplineDeformableTransform >
BSplineDeformableTransformInverseCalculator<TDirectBSplineDeformableTransform, TInverseBSplineDeformableTransform >
::BSplineDeformableTransformInverseCalculator() 
{
}


template < class TDirectBSplineDeformableTransform, class TInverseBSplineDeformableTransform >
void 
BSplineDeformableTransformInverseCalculator<TDirectBSplineDeformableTransform, TInverseBSplineDeformableTransform >
::Compute() const
{
  // Sanity check
  if( !this->m_DirectTransform )
    {
    itkExceptionMacro( "Direct Transform has not been set" );
    return;
    }
  if( !this->m_InverseTransform )
    {
    itkExceptionMacro( "Inverse Transform has not been set" );
    return;
    }

  const unsigned int directTransformNumberGridPoints = 
    this->m_DirectTransform->GetNumberOfParameters() / 
    DirectSpaceDimension;

  const unsigned int inverseTransformNumberGridPoints = 
    this->m_InverseTransform->GetNumberOfParameters() / 
    InverseSpaceDimension;

  //
  //  For each one of the dimensions, solve the problem for that component of
  //  the deformation field.
  // 
  for( unsigned int dim = 0; dim < DirectSpaceDimension; dim++)
    {
    // Allocate a matrix of size 
    //
    //  directTransformNumberGridPoints X inverseTransformNumberGridPoints
    //
    vnl_matrix< float >  matrixOfWeights( 
      directTransformNumberGridPoints, inverseTransformNumberGridPoints );

    matrixOfWeights.fill( 0.0 );

    // FIXME : it should be const pointer
    typename DirectTransformType::ImagePointer wrappedImage = NULL;
    // FIXME : Add this public method to the BSplineDef transform:  this->m_DirectTransform->GetWrappedImage( dim );

    typedef typename DirectTransformType::ImageType    WrappedImageType;

    typedef itk::ImageRegionIterator< WrappedImageType >  NodeIteratorType;

    NodeIteratorType nodeItr( wrappedImage, wrappedImage->GetLargestPossibleRegion() );

    typedef typename WrappedImageType::PointType      NodePointType;

    NodePointType nodePoint;
    NodePointType mappedNodePoint;
    NodePointType roundTripNodePoint;

    typename InverseTransformType::WeightsType              weights;
    typename InverseTransformType::ParameterIndexArrayType  indices;

    bool isInside;

    while( !nodeItr.IsAtEnd() )
      {
      //
      // Take the coordinates of each grid node and map it to the destination space
      //
      wrappedImage->TransformIndexToPhysicalPoint( nodeItr.GetIndex(), nodePoint );

      mappedNodePoint = this->m_DirectTransform->TransformPoint( nodePoint );

      //
      // Compute the weights and indices that the mappedNodePoint will have in the
      // destination grid.
      //
      this->m_InverseTransform->TransformPoint( mappedNodePoint, roundTripNodePoint,
        weights, indices, isInside );

      if( isInside )
        {
        // Fill the matrix with the weights that the direct BSpline grid nodes
        // will have in the inverse BSpline grid when mapped
        for( unsigned int destinationNode =0; 
             destinationNode < inverseTransformNumberGridPoints; 
             destinationNode++ )
          {
          matrixOfWeights( indices[ destinationNode ], dim ) = weights[ dim ];
          }
        
        //  Put the parameters of one dimension from the direct BSpline in an array
        }

      ++nodeItr;
      }

    //  Solve the linear equation:   -directParameters = MatrixOfWeights . inverseParameters

    //  Store the results in the dimension section of the full array of parameters
    //  of the inverse transform.
    }

  //  Do the Dance !
}
  

template < class TDirectBSplineDeformableTransform, class TInverseBSplineDeformableTransform >
void 
BSplineDeformableTransformInverseCalculator<TDirectBSplineDeformableTransform, TInverseBSplineDeformableTransform >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
     
  os << indent << "DirectTransform  = " << std::endl;
  if (this->m_DirectTransform)
    { 
    os << indent << this->m_DirectTransform << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

  os << indent << "Inverse Transform   = " << std::endl;
  if (this->m_InverseTransform)
    { 
    os << indent << this->m_InverseTransform << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }


}
 
}  // namespace itk

#endif /* __itkBSplineDeformableTransformInverseCalculator_txx */
