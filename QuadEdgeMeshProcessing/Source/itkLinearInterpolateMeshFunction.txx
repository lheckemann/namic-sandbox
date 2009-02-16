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

#include "itkLinearInterpolateMeshFunction.h"

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



/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
void
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::EvaluateDerivative( const PointType& point, DerivativeType & derivative ) const
{
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
  typedef typename Superclass::InstanceIdentifierVectorType InstanceIdentifierVectorType;

  const unsigned int numberOfNeighbors = 3;
  InstanceIdentifierVectorType result;

  this->Search( point, numberOfNeighbors, result );

  const InputMeshType * mesh = this->GetInputMesh(); 

  typedef typename InputMeshType::PointsContainer    PointsContainer;
  typedef typename PointType::VectorType             VectorType;

  const PointsContainer * points = mesh->GetPoints();

  typedef typename InputMeshType::QEPrimal    EdgeType;

  PointIdentifier pointId1 = result[0];
  PointIdentifier pointId2 = result[1];
  PointIdentifier pointId3 = result[2];

  //
  // Find the edge connected to the closest point.
  //
  EdgeType * edge1 = mesh->FindEdge( pointId1 );

  // 
  // Explore triangles around pointId1
  //
  EdgeType * temp1 = edge1->GetOnext();

  while( temp1 != edge1 )
    {
    EdgeType * temp2 = temp1->GetOnext();
    PointIdentifier pointId2 = temp1->GetDestination();
    PointIdentifier pointId3 = temp2->GetDestination();

    //
    // Get the vertexes of this triangle
    //
    PointType pt1 = points->GetElement( pointId1 );
    PointType pt2 = points->GetElement( pointId2 );
    PointType pt3 = points->GetElement( pointId3 );

    //
    // Compute Vectors along the edges.
    // These two vectors form a vector base for the 2D space of the triangle cell.
    //
    VectorType v12 = pt1 - pt2;
    VectorType v32 = pt3 - pt2;

    //
    // Compute Vectors in the dual vector base inside the 2D space of the triangle cell.
    // u12 is orthogonal to v32
    // u32 is orthogonal to v12
    //
    const double dotproduct =  v12 * v32;
    VectorType u12 = v12 - v32 * ( dotproduct / v32.GetSquaredNorm() );
    VectorType u32 = v32 - v12 * ( dotproduct / v12.GetSquaredNorm() );

    //
    // Add normalizations for making {u12,u32} a vector basis orthonormal to {v12, v32}.
    //
    u12 /= ( u12 * v12 );
    u32 /= ( u32 * v32 );

    //
    // Project point to plane, by using the dual vector base
    //
    // Compute components of the input point in the 2D
    // space defined by v12 and v32
    //
    VectorType xo = point - pt2;

    const double u12p = xo * u12;
    const double u32p = xo * u32;

    VectorType x12 = v12 * u12p;
    VectorType x32 = v32 * u32p;

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

    //
    // Since the three barycentric coordinates are interdependent
    // only three tests should be necessary. That is, we only need
    // to test against the equations of three lines (half-spaces).
    //
    if( ( b1 >= zwe ) && ( b2 >= zwe ) && ( b3 >= zwe ) )
      {
      // The points is inside this triangle !!
      break;
      }
   
    temp1 = temp2;
    }

  typedef typename InputMeshType::CellType    CellInterfaceType;

  typedef itk::TriangleCell< CellInterfaceType >  TriangleCellType;

  //
  // FIXME: This is missing the search for the cell in which the query point is embedded.
  //        a temporary computation is included here, but doesn't really corresponds to
  //        a linear interpolation in a cell yet.
  //
  PixelType pixelValue1 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue2 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue3 = itk::NumericTraits< PixelType >::Zero;

  this->GetPointData( pointId1, &pixelValue1 ); 
  this->GetPointData( pointId2, &pixelValue2 ); 
  this->GetPointData( pointId3, &pixelValue3 ); 

  RealType pixelValueReal1 = static_cast< RealType >( pixelValue1 );
  RealType pixelValueReal2 = static_cast< RealType >( pixelValue2 );
  RealType pixelValueReal3 = static_cast< RealType >( pixelValue3 );

  RealType returnValue = ( pixelValueReal1 + pixelValueReal2 + pixelValueReal3 ) / 3.0;

  return returnValue;
}

/* FROM meeting with Alex
void toto()
{
  list_of_id_points = PointLocator->FincNclosestPoint( p, n );
  for( ::iterator it = list_of_id_points.begin();
{
   edge = mesh->FindEdge( *it );
temp = edge;
   do
{
  // std::set to avoid duplicates!
  list_of_id_points_ring_to_be_tested.push_back( temp->GetDestination() );
temp = temp->GetOnext();
} while( temp != edge );

//iterate on list_of_id_points_ring
}
}

*/

} // end namespace itk

#endif
