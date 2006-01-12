/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMeshObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 20:48:04 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
MeshObject::MeshObject():m_StateMachine(this)
{
  m_Mesh = MeshType::New();
  m_Mesh->SetCellsAllocationMethod( MeshType::CellsAllocatedDynamicallyCellByCell );
  
  // Create the mesh Spatial Object
  m_MeshSpatialObject = MeshSpatialObjectType::New();
  m_MeshSpatialObject->SetMesh(m_Mesh);
  this->RequestSetSpatialObject( m_MeshSpatialObject );
} 

/** Destructor */
MeshObject::~MeshObject()  
{
}

/** Add a point to the mesh */
bool MeshObject::AddPoint(unsigned int id,float x, float y,float z)
{ 
  PointType pt;
  pt[0] = x;
  pt[1] = y;
  pt[2] = z;
  m_Mesh->SetPoint(id,pt);
  return true;
}

/** Add a triangle cell to the mesh */
bool MeshObject::AddTriangleCell(unsigned int id,unsigned int vertex1,unsigned int vertex2,unsigned int vertex3)
{
  CellAutoPointer cell; 
  cell.TakeOwnership(  new TriangleCellType );

  unsigned long trianglePoints[3];
  trianglePoints[0]=vertex1;
  trianglePoints[1]=vertex2;
  trianglePoints[2]=vertex3;
  cell->SetPointIds(trianglePoints);
  m_Mesh->SetCell(id, cell );

  return true;
}

/** Add a tetrahedron cell to the mesh */
bool MeshObject::AddTetrahedronCell(unsigned int id,unsigned int vertex1,unsigned int vertex2,unsigned int vertex3,unsigned int vertex4)
{
  CellAutoPointer cell; 
  cell.TakeOwnership(  new TetraCellType );

  unsigned long tetraPoints[4];
  tetraPoints[0]=vertex1;
  tetraPoints[1]=vertex2;
  tetraPoints[2]=vertex3;
  tetraPoints[3]=vertex4;
  cell->SetPointIds(tetraPoints);
  m_Mesh->SetCell(id, cell );

  return true;
}

/** Return the points */  
const MeshObject::PointsContainerPointer MeshObject::GetPoints() const
{
  return m_Mesh->GetPoints();
}

/** Return the cells */  
const MeshObject::CellsContainerPointer MeshObject::GetCells() const
{
  return m_Mesh->GetCells();
}

/** Print object information */
void MeshObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_MeshSpatialObject )
    {
    os << indent << this->m_MeshSpatialObject << std::endl;
    }
   os << indent << this->m_Mesh << std::endl;
}


} // end namespace igstk

