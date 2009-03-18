/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLinearInterpolateMeshFunction.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLinearInterpolateMeshFunction_txx
#define __itkLinearInterpolateMeshFunction_txx

#include "itkVector.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleBasisSystemCalculator.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkTriangleCell.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputMesh, class TCoordRep>
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::LinearInterpolateMeshFunction()
{
}


/**
 * Destructor
 */
template <class TInputMesh, class TCoordRep>
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::~LinearInterpolateMeshFunction()
{
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TCoordRep>
void
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}

#include <iostream>


/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
void
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::EvaluateDerivative( const PointType& point, DerivativeType & derivative ) const
{

  InstanceIdentifierVectorType pointIds;

  this->FindTriangle( point, pointIds );

  PixelType pixelValue1 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue2 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue3 = itk::NumericTraits< PixelType >::Zero;

  this->GetPointData( pointIds[0], &pixelValue1 ); 
  this->GetPointData( pointIds[1], &pixelValue2 ); 
  this->GetPointData( pointIds[2], &pixelValue3 );

  const RealType pixelValueReal1 = static_cast< RealType >( pixelValue1 );
  const RealType pixelValueReal2 = static_cast< RealType >( pixelValue2 );
  const RealType pixelValueReal3 = static_cast< RealType >( pixelValue3 );

  const RealType variation12 = pixelValueReal1 - pixelValueReal2;
  const RealType variation32 = pixelValueReal3 - pixelValueReal2;

  derivative = m_U12 * variation12  + m_U32 * variation32;
}


/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
typename 
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>::OutputType
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::Evaluate( const PointType& point ) const
{
  InstanceIdentifierVectorType pointIds;

  this->FindTriangle( point, pointIds );

  PixelType pixelValue1 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue2 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue3 = itk::NumericTraits< PixelType >::Zero;

  this->GetPointData( pointIds[0], &pixelValue1 ); 
  this->GetPointData( pointIds[1], &pixelValue2 ); 
  this->GetPointData( pointIds[2], &pixelValue3 ); 

  RealType pixelValueReal1 = static_cast< RealType >( pixelValue1 );
  RealType pixelValueReal2 = static_cast< RealType >( pixelValue2 );
  RealType pixelValueReal3 = static_cast< RealType >( pixelValue3 );

  RealType returnValue = 
    pixelValueReal1 * m_InterpolationWeights[0] +
    pixelValueReal2 * m_InterpolationWeights[1] +
    pixelValueReal3 * m_InterpolationWeights[2];

  return returnValue;
}


/**
 * Find corresponding triangle, vector base and barycentric coordinates
 */
template <class TInputMesh, class TCoordRep>
void 
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::FindTriangle( const PointType& point, InstanceIdentifierVectorType & pointIds ) const
{
  const unsigned int numberOfNeighbors = 3;

  this->Search( point, numberOfNeighbors, pointIds );

  const InputMeshType * mesh = this->GetInputMesh(); 

  typedef typename InputMeshType::QEPrimal    EdgeType;

  //
  // Find the edge connected to the closest point.
  //
  EdgeType * edge1 = mesh->FindEdge( pointIds[0] );

  // 
  // Explore triangles around pointIds[0]
  //
  EdgeType * temp1 = edge1->GetOnext();

  while( temp1 != edge1 )
    {
    EdgeType * temp2 = temp1->GetOnext();
    pointIds[1] = temp1->GetDestination();
    pointIds[2] = temp2->GetDestination();

    const bool isInside = this->ComputeWeights( point, pointIds );

    if( isInside )
      {
      break;
      }

    temp1 = temp2;
    }

}


/**
 * Compute interpolation weights and verify if the input point is inside the
 * triangle formed by the three identifiers.
 */
template <class TInputMesh, class TCoordRep>
bool
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::ComputeWeights( const PointType & point, 
  const InstanceIdentifierVectorType & pointIds ) const
{
  const InputMeshType * mesh = this->GetInputMesh(); 

  typedef typename InputMeshType::PointsContainer    PointsContainer;
  typedef typename PointType::VectorType             VectorType;

  const PointsContainer * points = mesh->GetPoints();

  //
  // Get the vertexes of this triangle
  //
  PointType pt1 = points->GetElement( pointIds[0] );
  PointType pt2 = points->GetElement( pointIds[1] );
  PointType pt3 = points->GetElement( pointIds[2] );

  const unsigned int SurfaceDimension = 2; 
  typedef typename itk::TriangleBasisSystem< VectorType, SurfaceDimension>  TriangleBasisSystemType;
  TriangleBasisSystemType *triangleBasisSystem= TriangleBasisSystemType::New();

  typedef typename itk::TriangleBasisSystemCalculator< TInputMesh, TriangleBasisSystemType >  TriangleBasisSystemCalculatorType;
  TriangleBasisSystemCalculatorType *triangleBasisSystemCalculator = TriangleBasisSystemCalculatorType::New(); 

  triangleBasisSystemCalculator->CalculateBasis(pt1, pt2, pt3, triangleBasisSystem );
  m_U12= triangleBasisSystem.GetVector(0); 
  m_U32= triangleBasisSystem.GetVector(1); 

#if 0
  //
  // Compute Vectors along the edges.
  // These two vectors form a vector base for the 2D space of the triangle cell.
  //
  VectorType m_V12 = pt1 - pt2;
  VectorType m_V32 = pt3 - pt2;

  //
  // Compute Vectors in the dual vector base inside the 2D space of the triangle cell.
  // m_U12 is orthogonal to m_V32
  // m_U32 is orthogonal to m_V12
  //
  const double dotproduct =  m_V12 * m_V32;
  m_U12 = m_V12 - m_V32 * ( dotproduct / m_V32.GetSquaredNorm() );
  m_U32 = m_V32 - m_V12 * ( dotproduct / m_V12.GetSquaredNorm() );

  //
  // Add normalizations for making {m_U12,m_U32} a vector basis orthonormal to {m_V12, m_V32}.
  //
  m_U12 /= ( m_U12 * m_V12 );
  m_U32 /= ( m_U32 * m_V32 );

#endif
  //
  // Project point to plane, by using the dual vector base
  //
  // Compute components of the input point in the 2D
  // space defined by m_V12 and m_V32
  //
  VectorType xo = point - pt2;

  const double u12p = xo * m_U12;
  const double u32p = xo * m_U32;

  VectorType x12 = m_V12 * u12p;
  VectorType x32 = m_V32 * u32p;

  //
  // The projection of point X in the plane is cp
  //
  PointType cp = pt2 + x12 + x32;

  //
  // Compute barycentric coordinates in the Triangle
  //
  const double b1 = u12p;
  const double b2 = 1.0 - u12p - u32p;
  const double b3 = u32p;

  //
  // Test if the projected point is inside the cell.
  //
  // Zero with epsilon
  const double zwe = -NumericTraits<double>::min();

  bool isInside = false;

  //
  // Since the three barycentric coordinates are interdependent
  // only three tests should be necessary. That is, we only need
  // to test against the equations of three lines (half-spaces).
  //
  if( ( b1 >= zwe ) && ( b2 >= zwe ) && ( b3 >= zwe ) )
    {
    // The points is inside this triangle 
    m_InterpolationWeights[0] = b1;
    m_InterpolationWeights[1] = b2;
    m_InterpolationWeights[2] = b3;
    isInside = true;
    }

  return isInside;
}
 

} // end namespace itk

#endif
