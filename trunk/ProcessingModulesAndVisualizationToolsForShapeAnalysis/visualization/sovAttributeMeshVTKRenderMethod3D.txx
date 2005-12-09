// sovSurfaceAttributeMeshVTKRenderMethod3D.txx: implementation of the sovSurfaceAttributeMeshVTKRenderMethod3D class.
//
//////////////////////////////////////////////////////////////////////

#include "sovAttributeMeshVTKRenderMethod3D.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkPointSource.h>
#include <vtkSphereSource.h>
#include <vtkCellType.h>
#include <vtkPoints.h>

namespace sov 
{
template < class TMesh, class TAttributeType>
AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::AttributeMeshVTKRenderMethod3D()
{
  m_SupportedTypeList->clear();
  AddSupportedType("MeshSpatialObject");
  AddSupportedType("AttributeMeshSpatialObject");
  this->m_PixelType = typeid(PixelType).name();
}

template < class TMesh, class TAttributeType>
AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::~AttributeMeshVTKRenderMethod3D()
{
}

template < class TMesh, class TAttributeType>
void
AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::Update()
{
  //cout << "Correct method is used" << endl ;

  ObjectListType::const_iterator            currentObject,lastObject;
  currentObject = m_ObjectList.begin();
  lastObject = m_ObjectList.end();

  for(; currentObject!=lastObject; currentObject++)
  { 

  if((*currentObject)->IsModified())
    {
    vtkPolyData* polyData = vtkPolyData::New();
    vtkPoints* polyPoints = vtkPoints::New();
    vtkFloatArray *scalars = vtkFloatArray::New() ;

    RemoveActor(*currentObject); // remove the actor before creating the new one.
    SurfaceMeshType* SurfaceMeshSO = dynamic_cast<SurfaceMeshType *>((*currentObject)->GetObject());
    AttributeMeshSOType* AttributeMeshSO = dynamic_cast<AttributeMeshSOType *>((*currentObject)->GetObject());

    VisualMeshProperty* prop = static_cast<VisualMeshProperty *>((*currentObject)->GetProperty());

    MeshType* mesh = SurfaceMeshSO->GetMesh();
    typename MeshType::PointsContainerPointer points = mesh->GetPoints();
    typename MeshType::PointsContainer::const_iterator it = points->begin();

    unsigned long nPoints = points->size();
    unsigned long id = 0;
    polyPoints->SetNumberOfPoints(nPoints);

    for(int i=0;it != points->end();it++, i++)
      {
      polyPoints->SetPoint((*it).first,(*it).second[0],(*it).second[1],(*it).second[2]);
      // well this wont be a scalar in the future
      scalars->InsertTuple1 (i, AttributeMeshSO->GetAttribute(i)) ;
      }    

    typename MeshType::CellsContainer* cells = mesh->GetCells();
    typename MeshType::CellsContainer::ConstIterator it_cells = cells->Begin();
    unsigned long nCells = cells->size();
    
    polyData->Allocate(nCells);
    
    for(;it_cells != cells->End();it_cells++)
      {
      vtkIdList* pts = vtkIdList::New();     
      typename MeshType::CellTraits::PointIdConstIterator itptids = (*it_cells)->Value()->GetPointIds();
      unsigned int id =0;
      unsigned long ptsSize = (*it_cells)->Value()->GetNumberOfPoints();
      pts->SetNumberOfIds(ptsSize);
      while(itptids != (*it_cells)->Value()->PointIdsEnd())
        {
        pts->SetId(id,*itptids);
        itptids++;
        id++;
        }

      switch(ptsSize)
        {
        case 3: polyData->InsertNextCell(VTK_TRIANGLE,pts);
        break;
        case 4: polyData->InsertNextCell(VTK_TETRA,pts);
        break;
        default:
        std::cout << "SurfaceMeshVTKRenderMethod3D: Don't know how to represent cells of size " << ptsSize << std::endl;      
        }
      pts->Delete();
      }
  
    vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
    vtkActor* meshActor = vtkActor::New();

    polyData->SetPoints(polyPoints);
    vtkPointData *pd ;
    pd = polyData->GetPointData () ;
    
    pd->SetScalars ( scalars ) ;
    
    vtkLoopSubdivisionFilter *lsf = vtkLoopSubdivisionFilter::New () ;
    lsf->SetInput ( polyData ) ;
    lsf->SetNumberOfSubdivisions ( 2 ) ;


    pointMapper->SetInput(lsf->GetOutput());
    //ColorScheme::MakeDistanceMap (pointMapper, 0, 1) ;
    ColorScheme::MakeSignificanceMap (pointMapper) ;
    meshActor->SetMapper(pointMapper);
    

    scalars->Delete() ;
    polyPoints->Delete();
    polyData->Delete();
    pointMapper->Delete();

      
    (*currentObject)->GetProperty()->AddUserDefined(meshActor);
    m_ActorToBeAddedList.push_back(meshActor);
    m_ActorList.push_back(meshActor);
    (*currentObject)->UpdateMTime();

    }

  if((*currentObject)->IsPropertyModified())
    {
      ChangeProperty(* currentObject);
      (*currentObject)->UpdateObjectPropertyMTime();
    }

  if((*currentObject)->IsObjectPropertyModified())
    {
      ChangeProperty(* currentObject);
      (*currentObject)->UpdateObjectPropertyMTime();
    }

  if((*currentObject)->IsTransformModified())
    {
    ApplyTransform(* currentObject);
    (*currentObject)->UpdateTransformMTime();
    }
  }
 
}


}

