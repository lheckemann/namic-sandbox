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
  this->m_SupportedTypeList->clear();
  this->AddSupportedType("MeshSpatialObject");
  this->AddSupportedType("AttributeMeshSpatialObject");
  this->m_PixelType = typeid(PixelType).name();

  m_ColorSchemeType = P_VALUE ;
  m_ColorScheme = new ColorScheme ;
  
}

template < class TMesh, class TAttributeType>
AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::~AttributeMeshVTKRenderMethod3D()
{
    delete ( m_ColorScheme ) ;
}

template < class TMesh, class TAttributeType>
void AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::ChangeColorMode (enum colorMode newMode) 
{
    m_ColorSchemeType = newMode ;   
}

template < class TMesh, class TAttributeType>
void
AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::Update()
{
  //cout << "Correct method is used" << endl ;

  typename ObjectListType::const_iterator  currentObject,lastObject;
  currentObject = this->m_ObjectList.begin();
  lastObject = this->m_ObjectList.end();

  for(; currentObject!=lastObject; currentObject++)
  { 

  if((*currentObject)->IsModified())
    {
    vtkPolyData* polyData = vtkPolyData::New();
    vtkPoints* polyPoints = vtkPoints::New();
    vtkFloatArray *scalars = vtkFloatArray::New() ;

    this->RemoveActor(*currentObject); // remove the actor before creating the new one.
    SurfaceMeshType* SurfaceMeshSO = dynamic_cast<SurfaceMeshType *>((*currentObject)->GetObject());
    AttributeMeshSOType* AttributeMeshSO = dynamic_cast<AttributeMeshSOType *>((*currentObject)->GetObject());

    MeshType* mesh = SurfaceMeshSO->GetMesh();
    typename MeshType::PointsContainerPointer points = mesh->GetPoints();
    typename MeshType::PointsContainer::const_iterator it = points->begin();

    unsigned long nPoints = points->size();
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
    if ( m_ColorSchemeType == P_VALUE )
    {
        m_ColorScheme->MakeSignificanceMap (pointMapper) ;
    }
    else
    {
        m_ColorScheme->MakeDistanceMap (pointMapper) ;
    }

    meshActor->SetMapper(pointMapper);
    

    scalars->Delete() ;
    polyPoints->Delete();
    polyData->Delete();
    pointMapper->Delete();

      
    //(*currentObject)->GetProperty()->AddUserDefined(meshActor);
    (*currentObject)->AddUserDefined(meshActor);

    this->m_ActorToBeAddedList.push_back(meshActor);
    this->m_ActorList.push_back(meshActor);
    (*currentObject)->UpdateMTime();

    }

  if((*currentObject)->IsPropertyModified())
    {
      this->ChangeProperty(* currentObject);
      (*currentObject)->UpdateObjectPropertyMTime();
    }

  if((*currentObject)->IsObjectPropertyModified())
    {
      this->ChangeProperty(* currentObject);
      (*currentObject)->UpdateObjectPropertyMTime();
    }

  if((*currentObject)->IsTransformModified())
    {
    this->ApplyTransform(* currentObject);
    (*currentObject)->UpdateTransformMTime();
    }
  }
 
}


template < class TMesh, class TAttributeType>
void AttributeMeshVTKRenderMethod3D<TMesh, TAttributeType>
::ChangePValueSettings ( float t, float h0, float h1, float h2 ) 
{
    m_ColorScheme->SignificanceSettings ( t, h0, h1, h2 ) ;
}

}
