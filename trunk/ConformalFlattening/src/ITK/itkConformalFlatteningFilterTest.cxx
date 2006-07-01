#include "itkConformalFlatteningFilterTest.h"

int itkTransformMeshFilterTest(char* fileName) {
  
  vtkPolyData *polyData = readDataToPolyData(fileName);//argv[1] );
  //read in the data from the .vtk file.
  
  // Set the color for the mesh according to local mean/gaussian
  // curvature.
  vtkCurvatures* curv1 = vtkCurvatures::New();
  curv1->SetInput(polyData);
  //  curv1->SetCurvatureTypeToGaussian();
  curv1->SetCurvatureTypeToMean();

  Display(curv1->GetOutput());


  //  Display(polyData);  
  //Begin convert from vtkPolyData to ITKMesh
  MeshType::Pointer  mesh = vtkPolyDataToITKMesh(polyData);

  ///////////////////////////////////////////////////////////////////////
  // itkConformalFlatteningFilter
  typedef itk::ConformalFlatteningFilter< MeshType, MeshType>  FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  // Connect the inputs
  filter->SetInput( mesh ); 
  filter->setPointP(1000);
  //filter->mapToPlane();
  filter->mapToSphere();

  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output 
  MeshType::Pointer newMesh = filter->GetOutput();
  // itkConformalFlatteningFilter
  ///////////////////////////////////////////////////////////////////////
  

  //Begin convert from ITKMesh to vtkPolyData
  vtkPolyData* newPolyData = ITKMeshToVtkPolyData(newMesh);

  //Display the new polydata

  newPolyData->GetPointData()->SetScalars(curv1->GetOutput()->GetPointData()->GetScalars());
  
  Display(newPolyData);

  return 0;
  
  
}



int main( int argc, char * argv [] )
{


     if( argc < 2 )
       {
         std::cerr << "Missing arguments" << std::endl;
         std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
         return -1;
       }

  int tmp = itkTransformMeshFilterTest(argv[1]);
  //int tmp = itkTransformMeshFilterTest("nice.vtk");

  return 0;


}




/////////////////////////////////////////////////////////////////////



vtkPolyData* readDataToPolyData(char* fName)
{
  vtkPolyDataReader * reader = vtkPolyDataReader::New();

  reader->SetFileName( fName);
  reader->Update();

  vtkPolyData * polyData = reader->GetOutput();
  
  return polyData;  
}



MeshType::Pointer vtkPolyDataToITKMesh(vtkPolyData *polyData)
{
  // Create a new mesh
  MeshType::Pointer mesh = MeshType::New();

  //
  // Transfer the points from the vtkPolyData into the itk::Mesh
  //
  const unsigned int numberOfPoints = polyData->GetNumberOfPoints();
  
  vtkPoints * vtkpoints = polyData->GetPoints();

  mesh->GetPoints()->Reserve( numberOfPoints );
  
  for(unsigned int p =0; p < numberOfPoints; p++)
    {

      vtkFloatingPointType * apoint = vtkpoints->GetPoint( p );
    
      mesh->SetPoint( p, MeshType::PointType( apoint ));
    
    }

  
  //
  // Transfer the cells from the vtkPolyData into the itk::Mesh
  //
  vtkCellArray * triangleStrips = polyData->GetStrips();


  vtkIdType  * cellPoints;
  vtkIdType    numberOfCellPoints;

  //
  // First count the total number of triangles from all the triangle strips.
  //
  unsigned int numberOfTriangles = 0;

  triangleStrips->InitTraversal();

  while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
      numberOfTriangles += numberOfCellPoints-2;
    }


  vtkCellArray * polygons = polyData->GetPolys();

  polygons->InitTraversal();

  while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
      if( numberOfCellPoints == 3 )
        {
          numberOfTriangles ++;
        }
    }


  //
  // Reserve memory in the itk::Mesh for all those triangles
  //
  mesh->GetCells()->Reserve( numberOfTriangles );

  
  // 
  // Copy the triangles from vtkPolyData into the itk::Mesh
  //
  //

  int cellId = 0;

  // first copy the triangle strips
  triangleStrips->InitTraversal();
  while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
    
      unsigned int numberOfTrianglesInStrip = numberOfCellPoints - 2;

      unsigned long pointIds[3];
      pointIds[0] = cellPoints[0];
      pointIds[1] = cellPoints[1];
      pointIds[2] = cellPoints[2];

      for( unsigned int t=0; t < numberOfTrianglesInStrip; t++ )
        {
          MeshType::CellAutoPointer c;
          TriangleCellType * tcell = new TriangleCellType;
          tcell->SetPointIds( pointIds );
          c.TakeOwnership( tcell );
          mesh->SetCell( cellId, c );
          cellId++;
          pointIds[0] = pointIds[1];
          pointIds[1] = pointIds[2];
          pointIds[2] = cellPoints[t+3];
        }
    }


  // then copy the normal triangles
  polygons->InitTraversal();
  while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
      if( numberOfCellPoints !=3 ) // skip any non-triangle.
        {
          continue;
        }
      MeshType::CellAutoPointer c;
      TriangleCellType * t = new TriangleCellType;
      t->SetPointIds( (unsigned long*)cellPoints );
      c.TakeOwnership( t );
      mesh->SetCell( cellId, c );
      cellId++;
    }

  return mesh;
}

vtkPolyData* ITKMeshToVtkPolyData(MeshType::Pointer mesh)
{
  //Creat a new vtkPolyData
  vtkPolyData* newPolyData = vtkPolyData::New();

  //Creat vtkPoints for insertion into newPolyData
  vtkPoints *points = vtkPoints::New();

  // std::cout<<"Points = "<<mesh->GetNumberOfPoints()<<std::endl;

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
  norm->SetInput(polyData);
  norm->SetFeatureAngle(45);


  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput(norm->GetOutput());

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
  ren->AddActor(actor);
  //ren->SetActiveCamera(camera);
  //ren->ResetCamera();
  ren->SetBackground(1,1,1);

  vtkRenderWindow* renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren);
  renWin->SetSize(300,300);

  vtkRenderWindowInteractor* inter = vtkRenderWindowInteractor::New();
  inter->SetRenderWindow(renWin);

  renWin->Render();
  inter->Start();
}

