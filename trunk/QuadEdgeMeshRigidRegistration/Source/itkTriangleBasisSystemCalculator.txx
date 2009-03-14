
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
  m_InputMesh= NULL; 
}

template <class TMesh, class TBasisSystem >
void
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::SetInputMesh( const TMesh & m)
{
  if (m==NULL) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator  SetInputMesh input parameter is NULL.");
    }
  m_InputMesh= m; 
}

template <class TMesh, class TBasisSystem >
const TMesh & 
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::GetInputMesh(void) const
{
  if (m_InputMesh!=NULL) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator  GetInputMesh m_InputMesh is NULL.");
    }
  const MeshType & result= m_InputMesh; 
  return (result);
}

template <class TMesh, class TBasisSystem >
void
TriangleBasisSystemCalculator<TMesh, TBasisSystem>
::CalculateTriangle( unsigned int cellIndex, TBasisSystem & bs ) const
{

  if (m_InputMesh!=NULL) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator CalculateTriangle  m_InputMesh is NULL.");
    }

  if (cellIndex>=this->m_InputMesh->GetNumberOfCells()) 
    {
    itkExceptionMacro(<<"TriangleBasisSystemCalculator CalculateTriangle  cellIndex is too high.");
    }

  typedef typename MeshType::CellType           CellType; 
  typedef typename MeshType::PointType           PointType; 
  typedef typename MeshType::CellsContainer     CellsContainer; 
  typedef typename CellType::PointsContainer    PointsContainer;
  typedef typename PointType::VectorType        VectorType;

  VectorType  m_U12;
  VectorType  m_U32;

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

  bs->SetVector(0, m_U12); 
  bs->SetVector(1, m_U32); 

}

} // end namespace itk

#endif






