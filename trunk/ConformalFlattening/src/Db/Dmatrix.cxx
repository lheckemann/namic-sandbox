#include "Dmatrix.h"


int main( int argc, char * argv [] )
{

  clock_t time = clock();

  //   if( argc < 2 )
  //     {
  //       std::cerr << "Missing arguments" << std::endl;
  //       std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
  //       return -1;
  //     }

  vtkPolyData *polyData = readDataToPolyData( "../../../data/cube.vtk" );//argv[1] );
  
  //Begin convert from vtkPolyData to ITKMesh
  MeshType::Pointer  mesh = vtkPolyDataToITKMesh(polyData);

  vnl_sparse_matrix<vtkFloatingPointType> D(mesh->GetNumberOfPoints(),
                                            mesh->GetNumberOfPoints());
  vnl_vector<vtkFloatingPointType> bR(mesh->GetNumberOfPoints(), 0);
  vnl_vector<vtkFloatingPointType> bI(mesh->GetNumberOfPoints(), 0);

  getDb( mesh, D , bR, bI);

  std::cerr<<(clock() - time)/CLOCKS_PER_SEC<<std::endl;

//     //  ------------------------------------------------------------
//     //  print out matrix D and b
//       for (int itRow = 0; itRow<D.rows(); ++itRow ) {
//         for (int itCol = 0; itCol<D.columns(); ++itCol ) {
//           std::cerr<<D(itRow, itCol)<<"\t\t";
//         }
//         std::cerr<<" b "<<bR(itRow)<<" + i"<<bI(itRow);
//         std::cerr<<std::endl;
//       }
//     //------------------------------------------------------------

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



void getDb(MeshType::Pointer mesh, 
                vnl_sparse_matrix<vtkFloatingPointType> &D,
                vnl_vector<vtkFloatingPointType> &bR,
                vnl_vector<vtkFloatingPointType> &bI) {

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


  //   //--------------------------------------------------------------
  //   // print out the result
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
  //   //---------------------------------------------------------------


  // 1. Iterate point P from 0 to the last point in the mesh. 
  // 2. For each P, find its neighbors, each neighbor must:
  //    1) has at least two triangles containing P and itself ---not the boundary.
  //    2) has larger pointId, to avoid re-calculation.
  // 3. For each of P's neighbors, Q, calculate R, S
  // 4. Write the value in matrix.
  std::vector< std::vector<int> >::iterator itP, itPEnd = pointCell.end();
  int idP = 0;
  for ( itP = pointCell.begin(); itP != itPEnd; ++itP, ++idP) {
    std::vector<int> neighborOfP;
    // for each point P, traverse all cells containing it.
    std::vector<int>::iterator itCell = (*itP).begin();
    std::vector<int>::iterator itCellEnd = (*itP).end();

    for (; itCell != itCellEnd; ++itCell) {
      // for each cell containing P, store the point with larger point Id.
      // only three points, don't use for-loop to save time.
      if ( cellPoint[*itCell][0] > idP ) 
        neighborOfP.push_back(cellPoint[*itCell][0]);
      if ( cellPoint[*itCell][1] > idP ) 
        neighborOfP.push_back(cellPoint[*itCell][1]);
      if ( cellPoint[*itCell][2] > idP ) 
        neighborOfP.push_back(cellPoint[*itCell][2]);
    }// ok, now all neighbors of P is stored in neighborOfP;
    
    sort(neighborOfP.begin(), neighborOfP.end());
    std::vector<int>::iterator it;
    it = unique(neighborOfP.begin(), neighborOfP.end());
    neighborOfP.erase(it, neighborOfP.end());

    //-----------------------------------------------
    // print out the neighbors
    //     std::vector<int>::iterator itNeighbor = neighborOfP.begin();
    //     std::vector<int>::iterator itNeighborEnd = neighborOfP.end();
    //     std::cerr<<"The neighbors of "<<idP<<" are: ";
    //     for (; itNeighbor != itNeighborEnd; ++itNeighbor) {
    //       std::cerr<<*itNeighbor<<" , ";
    //     }
    //     std::cerr<<std::endl;
    // ----------------------------------------------------

    // next, from P to each neighbor...
    // note: itP and itQ point at different type of vectors...
    // *itP is a vector containing a list of points Ids
    // idP is the point Id of P
    // *itQ is the point Id of Q (so idP and *itQ are same type)
    std::vector<int>::iterator itQ, itQEnd = neighborOfP.end();
    for ( itQ = neighborOfP.begin(); itQ != itQEnd; ++itQ) {
      // first check whether PQ is a boundary edge:
      std::vector<int> cellsContainingP(*itP), cellsContainingQ(pointCell[*itQ]);
      std::vector<int> cells(cellsContainingP.size() + cellsContainingQ.size());
      std::vector<int>::iterator itv, endIter;

      sort(cellsContainingP.begin(), cellsContainingP.end());
      sort(cellsContainingQ.begin(), cellsContainingQ.end());

      endIter = set_intersection(cellsContainingP.begin(), cellsContainingP.end(),
                                 cellsContainingQ.begin(), cellsContainingQ.end(),
                                 cells.begin());
      cells.erase(endIter, cells.end());
      if (cells.size() != 2) continue;
      // If P and Q are not shared by two triangles, i.e. 1: are not
      // connected by and edge, or, 2: are on the surface boundary
      // thus only shared by one triangle. then skip.  However, in
      // this paper the surface is closed thus there is not boundary.

      // If passed test above, then P and Q are two valid points.
      // i.e. PQ is a valid edge.  i.e. cells now contain two int's,
      // which are the Id of the triangles containing P and Q


      //       //------------------------------------------------------------
      //       //print out valid edge
      //       std::cerr<<idP<<" and "<<*itQ<<" are two valid points"<<std::endl;
      //       std::cerr<<(endIter == cells.end())<<std::endl;
      //       //-----------------------------------------------------------


      // Next we extract R and S from cells
      int itS, itR; // the Id of point S and R;
      for (int it = 0; it < 3; ++it) {
        if (cellPoint[cells[0]][it] != idP && cellPoint[cells[0]][it] != *itQ) 
          itS = cellPoint[cells[0]][it];
        if (cellPoint[cells[1]][it] != idP && cellPoint[cells[1]][it] != *itQ) 
          itR = cellPoint[cells[1]][it];
      }

      std::vector<double> P(pointXYZ[idP]), 
        Q(pointXYZ[*itQ]), 
        R(pointXYZ[itR]),
        S(pointXYZ[itS]);

      std::vector<double> SP(3), SQ(3), RP(3), RQ(3);
      double SPnorm = 0, SQnorm = 0, RPnorm = 0, RQnorm = 0, SPSQinnerProd = 0, RPRQinnerProd = 0;
      for (int it = 0; it<3; ++it) {
        SP[it] = P[it] - S[it]; SPnorm += SP[it]*SP[it];
        SQ[it] = Q[it] - S[it]; SQnorm += SQ[it]*SQ[it]; SPSQinnerProd += SP[it]*SQ[it];
        RP[it] = P[it] - R[it]; RPnorm += RP[it]*RP[it];
        RQ[it] = Q[it] - R[it]; RQnorm += RQ[it]*RQ[it]; RPRQinnerProd += RP[it]*RQ[it];
      }
      SPnorm = sqrt(SPnorm);
      SQnorm = sqrt(SQnorm);
      RPnorm = sqrt(RPnorm);
      RQnorm = sqrt(RQnorm);

      double cosS = SPSQinnerProd / (SPnorm * SQnorm); 
      double cosR = RPRQinnerProd / (RPnorm * RQnorm);
      double ctgS = cosS/sqrt(1-cosS*cosS), ctgR = cosR/sqrt(1-cosR*cosR);

      D(idP, *itQ) = -0.5*(ctgS + ctgR);
      D(idP, idP) += 0.5*(ctgS + ctgR); 
      // add to the diagonal element of this line.

      D(*itQ, idP) = -0.5*(ctgS + ctgR); // symmetric
      D(*itQ, *itQ) += 0.5*(ctgS + ctgR); 
      // add to the diagonal element of this line.
    }
  }

  // compute b = bR + i*bI separately
  std::vector<double> A( pointXYZ[0] ), B( pointXYZ[1] ), C( pointXYZ[2] );
  double ABnorm, CA_BAip; // the inner product of vector C-A and B-A;
  ABnorm = (A[0] - B[0]) * (A[0] - B[0])
    + (A[1] - B[1]) * (A[1] - B[1])
    + (A[2] - B[2]) * (A[2] - B[2]);

  CA_BAip = (C[0] - A[0]) * (B[0] - A[0])
    + (C[1] - A[1]) * (B[1] - A[1])
    + (C[2] - A[2]) * (B[2] - A[2]);

  double theta = CA_BAip / ABnorm; 
  // Here ABnorm is actually the square of AB's norm, which is what we
  // want. So don't bother square the square root.
  
  ABnorm = sqrt(ABnorm); // This is real norm of vector AB.

  std::vector<double> E(3);
  for (int it = 0; it < 3; ++it) 
    E[it] = A[it] + theta*(B[it] - A[it]);

  double CEnorm;
  CEnorm = (C[0] - E[0]) * (C[0] - E[0])
    + (C[1] - E[1]) * (C[1] - E[1])
    + (C[2] - E[2]) * (C[2] - E[2]);
  CEnorm = sqrt(CEnorm); // This is real norm of vector CE.

  bR(0) = -1 / ABnorm;
  bR(1) = 1 / ABnorm;

  bR(0) = (1-theta)/ CEnorm;
  bR(1) = theta/ CEnorm;
  bR(2) = -1 / CEnorm;
  
  return; 
}
