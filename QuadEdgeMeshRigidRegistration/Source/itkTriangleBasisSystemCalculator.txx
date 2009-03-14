
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystemCalculator.txx,v $
  Language:  C++
  Date:      $Date:  $
  Version:   $Revision:  $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleBasisSystemCalculator_txx
#define __itkTriangleBasisSystemCalculator_txx

#include "itkTriangleBasisSystemCalculator.h"
#include "itkPointLocator2.h"

namespace itk
{

/**
 * Constructor
 */
template <class TMesh, class TBasisSystem >
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::TriangleBasisSystemCalculator()
{
  itkDebugMacro("Constructor");
  this->m_InputMesh= NULL; 
}

/**
 * Destructor
 */
template <class TMesh, class TBasisSystem >
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::~TriangleBasisSystemCalculator()
{
  itkDebugMacro("Destructor");
}


template <class TMesh, class TBasisSystem >
void
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::CalculateTriangle( unsigned int cellIndex, TBasisSystem & bs ) const
{

  if( this->m_InputMesh.IsNull() ) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator CalculateTriangle  m_InputMesh is NULL.");
    }

  if (cellIndex >= this->m_InputMesh->GetNumberOfCells()) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator CalculateTriangle  cellIndex is too high.");
    }

  typedef typename MeshType::CellType           CellType; 
  typedef typename MeshType::PointType          PointType; 
  typedef typename MeshType::CellsContainer     CellsContainer; 
  typedef typename CellType::PointsContainer    PointsContainer;
  typedef typename PointType::VectorType        VectorType;

  VectorType  u12;
  VectorType  u32;

  typedef PointLocator2< MeshType >           PointLocatorType;
  typedef typename PointLocatorType::Pointer    PointLocatorPointer;
  typedef typename PointLocatorType::InstanceIdentifierVectorType InstanceIdentifierVectorType;
  InstanceIdentifierVectorType pointIds;

  const PointsContainer * points = this->m_InputMesh->GetCell(cellIndex)->GetPoints();

  //
  // Get the vertexes of this triangle
  //
  PointType pt1 = points->GetElement( pointIds[0] );
  PointType pt2 = points->GetElement( pointIds[1] );
  PointType pt3 = points->GetElement( pointIds[2] );

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
  u12 = v12 - v32 * ( dotproduct / v32.GetSquaredNorm() );
  u32 = v32 - v12 * ( dotproduct / v12.GetSquaredNorm() );

  //
  // Add normalizations for making {u12,u32} a vector basis orthonormal to {v12, v32}.
  //
  u12 /= ( u12 * v12 );
  u32 /= ( u32 * v32 );

  bs.SetVector( 0, u12 ); 
  bs.SetVector( 1, u32 ); 
}

} // end namespace itk

#endif






