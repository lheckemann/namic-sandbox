/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMeshObjectRepresentation.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.7 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>

namespace igstk
{ 

/** Constructor */
MeshObjectRepresentation
::MeshObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_MeshObject = NULL;
  this->RequestSetSpatialObject( m_MeshObject );
  
  igstkAddInputMacro( ValidMeshObject );
  igstkAddInputMacro( NullMeshObject );

  igstkAddStateMacro( NullMeshObject );
  igstkAddStateMacro( ValidMeshObject );

  igstkAddTransitionMacro( NullMeshObject, NullMeshObject, NullMeshObject,  No );
  igstkAddTransitionMacro( NullMeshObject, ValidMeshObject, ValidMeshObject,  SetMeshObject );
  igstkAddTransitionMacro( ValidMeshObject, NullMeshObject, NullMeshObject,  No ); 
  igstkAddTransitionMacro( ValidMeshObject, ValidMeshObject, ValidMeshObject,  No ); 

  igstkSetInitialStateMacro( NullMeshObject );

  m_StateMachine.SetReadyToRun();


} 

/** Destructor */
MeshObjectRepresentation::~MeshObjectRepresentation()  
{
  // This must be called in order to avoid memory leaks.
  this->DeleteActors();
}


/** Set the Meshal Spatial Object */
void MeshObjectRepresentation::RequestSetMeshObject( const MeshObjectType * Mesh )
{
  m_MeshObjectToAdd = Mesh;
  if( !m_MeshObjectToAdd )
    {
    igstkPushInputMacro( NullMeshObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidMeshObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Null operation for a State Machine transition */
void MeshObjectRepresentation::NoProcessing()
{
}


/** Set the Cylindrical Spatial Object */
void MeshObjectRepresentation::SetMeshObjectProcessing()
{
  // We create the ellipse spatial object
  m_MeshObject = m_MeshObjectToAdd;
  this->RequestSetSpatialObject( m_MeshObject );
} 


/** Print Self function */
void MeshObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 *  object */
void MeshObjectRepresentation::UpdateRepresentationProcessing()
{
}

/** Create the vtk Actors */
void MeshObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkUnstructuredGrid* polyData = vtkUnstructuredGrid::New();
  vtkPoints* polyPoints = vtkPoints::New();

  MeshObjectType::PointsContainerPointer points = m_MeshObject->GetPoints();
  MeshObjectType::PointsContainer::const_iterator it = points->begin();

  unsigned long nPoints = points->size();

  polyPoints->SetNumberOfPoints(nPoints);

  for(;it != points->end();it++)
    {
    polyPoints->SetPoint((*it).first,(*it).second[0],(*it).second[1],(*it).second[2]);
    }    

  MeshObjectType::CellsContainer* cells = m_MeshObject->GetCells();
  MeshObjectType::CellsContainer::ConstIterator it_cells = cells->Begin();
  unsigned long nCells = cells->size();
    
  polyData->Allocate(nCells);
    
  for(;it_cells != cells->End();it_cells++)
    {
    vtkIdList* pts = vtkIdList::New();     
    MeshObjectType::CellTraits::PointIdConstIterator itptids = (*it_cells)->Value()->GetPointIds();
    unsigned int id =0;
    const unsigned long ptsSize = (*it_cells)->Value()->GetNumberOfPoints();
    pts->SetNumberOfIds(ptsSize);
    while(itptids != (*it_cells)->Value()->PointIdsEnd())
      {
      pts->SetId(id,*itptids);
      itptids++;
      id++;
      }

    switch( ptsSize )
      {
      case 2: 
        polyData->InsertNextCell( VTK_LINE, pts );
        break;
      case 3: 
        polyData->InsertNextCell( VTK_TRIANGLE, pts );
        break;
      case 4: 
        polyData->InsertNextCell( VTK_TETRA, pts );
        break;
      default:
        igstkLogMacro( CRITICAL, "MeshObjectRepresentation: "
            << "Don't know how to represent cells of size "
            << ptsSize << " \n" );
      }
    pts->Delete();
    }
  
  vtkDataSetMapper *pointMapper = vtkDataSetMapper::New();
  vtkActor* meshActor = vtkActor::New();

  meshActor->GetProperty()->SetColor(this->GetRed(),
                                     this->GetGreen(),
                                     this->GetBlue()); 
    
  polyData->SetPoints(polyPoints);
  pointMapper->SetInput(polyData);
  meshActor->SetMapper(pointMapper);
 
  this->AddActor( meshActor );

  polyPoints->Delete();
  polyData->Delete();
  pointMapper->Delete();
}

/** Create a copy of the current object representation */
MeshObjectRepresentation::Pointer
MeshObjectRepresentation::Copy() const
{
  Pointer newOR = MeshObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetMeshObject(m_MeshObject);

  return newOR;
}


} // end namespace igstk

