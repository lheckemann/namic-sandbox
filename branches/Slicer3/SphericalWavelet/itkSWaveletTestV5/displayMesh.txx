#ifndef _displayMesh_txx_
#define _displayMesh_txx_


#ifndef __vtkPolyDataNormals_h
#include "vtkPolyDataNormals.h"
#endif

#ifndef __vtkLookupTable_h
#include "vtkLookupTable.h"
#endif

#ifndef _vtkPointData__h
#include "vtkPointData.h"
#endif

vtkPolyData* ITKMeshToVtkPolyData(MeshType::Pointer mesh)
{
  //Creat a new vtkPolyData
  vtkPolyData* newPolyData = vtkPolyData::New();

  //Creat vtkPoints for insertion into newPolyData
  vtkPoints *points = vtkPoints::New();

  //  std::cout<<"Points = "<<mesh->GetNumberOfPoints()<<std::endl;

  //Copy all points into the vtkPolyData structure
  PointIterator pntIterator = mesh->GetPoints()->Begin();
  PointIterator pntItEnd = mesh->GetPoints()->End();

  for (int i = 0; pntIterator != pntItEnd; ++i, ++pntIterator)
    {
      ItkPoint pnt = pntIterator.Value();
      points->InsertPoint(i, pnt[0], pnt[1], pnt[2]);
      //       std::cout<<i<<"-th point:  ";
      //       std::cout<<pnt[0]<<std::endl;
      //       std::cout<<"               "<<pntIterator.Value()<<std::endl;
      //      ++pntIterator;
    }
  newPolyData->SetPoints(points);
  points->Delete();


  //Copy all cells into the vtkPolyData structure
  //Creat vtkCellArray into which the cells are copied
  vtkCellArray* triangle = vtkCellArray::New();
  
  CellIterator cellIt = mesh->GetCells()->Begin();
  CellIterator cellItEnd = mesh->GetCells()->End();

  for (int it = 0; cellIt != cellItEnd; ++it, ++cellIt)
    {
      CellType * cellptr = cellIt.Value();
      //    LineType * line = dynamic_cast<LineType *>( cellptr );
      //    std::cout << line->GetNumberOfPoints() << std::endl;
      //      std::cout << cellptr->GetNumberOfPoints() << std::endl;

      PointIdIterator pntIdIter = cellptr->PointIdsBegin();
      PointIdIterator pntIdEnd = cellptr->PointIdsEnd();
      vtkIdList* pts = vtkIdList::New();

      for (; pntIdIter != pntIdEnd; ++pntIdIter)
        {
          pts->InsertNextId( *pntIdIter );
          //          std::cout<<"           "<<tempCell[it1]<<std::endl;
        }
      triangle->InsertNextCell(pts);
    }
  newPolyData->SetPolys(triangle);
  triangle->Delete();

  // return the vtkUnstructuredGrid
  return newPolyData;
}

void Display(vtkPolyData* polyData)
{
  vtkPolyDataNormals* norm = vtkPolyDataNormals::New();
  norm->SetInput( polyData );
  norm->SetFeatureAngle( 30 );
  norm->GetOutput()->GetPointData()->SetScalars( polyData->GetPointData()->GetScalars() );

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  //  mapper->SetScalarRange( -0.01, 0.01 );
  mapper->SetInput( norm->GetOutput() );

  vtkLookupTable* lut1 = vtkLookupTable::New();
  lut1->SetNumberOfColors(256);
  //   lut1->SetHueRange(0.2, 0); //0:red, 0.2: yellow, 0.7: blue, 1:red again.
  //   lut1->SetSaturationRange(0.2, 1.0);
  //   lut1->SetValueRange(1.0, 0.3);

  lut1->SetHueRange(0.15, 1.0); //0:red, 0.2: yellow, 0.7: blue, 1:red again.
  lut1->SetSaturationRange(1.0, 1.0);



  lut1->SetAlphaRange(1.0, 1.0);
  lut1->SetRange(-20, 20); //-20: left value above, 20: right value above

  mapper->SetLookupTable(lut1);
  mapper->SetUseLookupTableScalarRange(1);

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);

  //   vtkCamera *camera = vtkCamera::New();
  //       camera->SetPosition(1,1,1);
  //       camera->SetFocalPoint(0,0,0);

  vtkRenderer* ren = vtkRenderer::New();
  ren->AddActor( actor );
  //ren->SetActiveCamera(camera);
  //ren->ResetCamera();
  ren->SetBackground( 1,1,1 );

  vtkRenderWindow* renWin = vtkRenderWindow::New();
  renWin->AddRenderer( ren );
  renWin->SetSize( 300,300 );

  vtkRenderWindowInteractor* inter = vtkRenderWindowInteractor::New();
  inter->SetRenderWindow( renWin );

  renWin->Render();
  inter->Start();
}


void displayITKmesh(MeshType::Pointer mesh)
{
  vtkPolyData* newPolyData = ITKMeshToVtkPolyData(mesh);
  Display(newPolyData);

  return;
}
#endif
