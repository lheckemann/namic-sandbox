#include "Dmatrix.h"


int main( int argc, char * argv [] )
{

//   if( argc < 2 )
//     {
//       std::cerr << "Missing arguments" << std::endl;
//       std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
//       return -1;
//     }

  vtkPolyData *polyData = readDataToPolyData( "../../../data/nice.vtk" );//argv[1] );
  
  //Begin convert from vtkPolyData to ITKMesh
  MeshType::Pointer  mesh = vtkPolyDataToITKMesh(polyData);

  vnl_matrix<vtkFloatingPointType> D(mesh->GetNumberOfPoints(),
                                     mesh->GetNumberOfPoints(),
                                     0);  
  getMatrixD( mesh, D );

  //  std::cout<<D.size()<<std::endl;

  //   //Begin convert from ITKMesh to vtkPolyData
  //   vtkPolyData* newPolyData = ITKMeshToVtkPolyData(mesh);

  //   //Display the new polydata
  //   Display(newPolyData);
  
  //   std::cout << "Mesh  " << std::endl;
  //   std::cout << "Number of Points =   " << mesh->GetNumberOfPoints() << std::endl;
  //   std::cout << "Number of Cells  =   " << mesh->GetNumberOfCells()  << std::endl;


  return 0;
}


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

  typedef MeshType::CellType   CellType;

  typedef itk::TriangleCell< CellType > TriangleCellType;

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

  std::cout<<"Points = "<<mesh->GetNumberOfPoints()<<std::endl;

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

      for (int it1 = 0; pntIdIter != pntIdEnd; ++it1, ++pntIdIter)
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
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput(polyData);

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



void getMatrixD(MeshType::Pointer mesh, vnl_matrix<vtkFloatingPointType> &D) {

  int numOfPoints = mesh->GetNumberOfPoints();
  int numOfCells = mesh->GetNumberOfCells();

  // 1. store the points coordinates: pointXYZ
  std::vector< std::vector<double> > pointXYZ( numOfPoints, std::vector<double>(3, 0) );

  PointIterator pntIterator = mesh->GetPoints()->Begin();

  for ( int it = 0; it < numOfPoints; ++it, ++pntIterator) {
    ItkPoint pnt = pntIterator.Value();
    
    pointXYZ[it][0] = pnt[0];
    pointXYZ[it][1] = pnt[1];
    pointXYZ[it][2] = pnt[2];

    //    std::cout<<pnt[0];
  }

  // 2. store the relationship from point to cell, i.e. for each
  // point, which cells contain it?  For each point in the mesh,
  // generate a vector, storing the id number of triangles containing
  // this point.  The vectors of all the points form a vector:
  // pointCell

  // 3. store the relationship from cell to point, i.e. for each cell,
  // which points does it contains? store in vector: cellPoint
  std::vector< std::vector<int> > pointCell( numOfPoints );
  std::vector< std::vector<int> > cellPoint( numOfCells, std::vector<int>(3, 0) );

  CellIterator cellIt = mesh->GetCells()->Begin();

  for ( int itCell = 0;
        itCell < numOfCells;
        ++itCell, ++cellIt) {
    
    CellType * cellptr = cellIt.Value(); 
    // cellptr will point to each cell in the mesh
    // std::cout << cellptr->GetNumberOfPoints() << std::endl;

    PointIdIterator pntIdIter = cellptr->PointIdsBegin(); 
    //pntIdIter will point to each point in the current cell
    PointIdIterator pntIdEnd = cellptr->PointIdsEnd();

    for (int itPntInCell = 0; pntIdIter != pntIdEnd; ++pntIdIter, ++itPntInCell)
      {
        pointCell[ *pntIdIter ].push_back(itCell);
        cellPoint[itCell][itPntInCell] = *pntIdIter;
      }
  }

  
  // Now we're going to calculate matrix D element by element.
  for (int itRow = 0; itRow < numOfPoints; ++itRow) {
    for (int itCol = itRow+1; itCol < numOfPoints; ++itCol) { 
      // D is symmetric, only need to compute half
      // Diagonal values are calculated from all off-diagonal values.
      // i.e. computed after this for-loop for itCol
      
    }
  }


//   std::cout<<std::endl;
//   std::cout<<std::endl;
//   std::cout<<std::endl;
//   std::cout<<std::endl;

//   for (int it = 0; it < numOfPoints; ++it) {
//     std::cout<<"point# "<<it<<" :"
//              <<"       X: "<<pointXYZ[it][0]
//              <<"       Y: "<<pointXYZ[it][1]
//              <<"       Z: "<<pointXYZ[it][2]<<std::endl;
//   }

//   for (int it = 0; it < numOfPoints; ++it) {
//     std::cout<<"point# "<<it<<" is contained by    "<<pointCell[it].size()<<"   cells:"<<std::endl;
//     for (std::vector<int>::const_iterator vi = pointCell[it].begin();
//          vi != pointCell[it].end();
//          ++vi) {
//       std::cout<<*vi<<"     ";      
//     }
//     std::cout<<std::endl;
//   }

//   for (int it = 0; it < numOfCells; ++it) {
//     std::cout<<"cell# "<<it<<" has points: "
//              <<cellPoint[it][0]<<"  "
//              <<cellPoint[it][1]<<"  "
//              <<cellPoint[it][2]<<std::endl;
//   }



  return; 
}
