#include "apc.h"

int main( int argc, char * argv [] )
{

  clock_t time = clock();

  if( argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
    return -1;
    }

  vtkPolyData *polyData1 = readDataToPolyData( argv[1] );
  vtkPolyData *polyData2 = readDataToPolyData( argv[2] );
  
  //Begin convert from vtkPolyData to ITKMesh
  MeshType::Pointer  mesh1 = vtkPolyDataToITKMesh(polyData1);
  MeshType::Pointer  mesh2 = vtkPolyDataToITKMesh(polyData2);
    
  anglePreserveCheck( mesh1, mesh2 );

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


void anglePreserveCheck(MeshType::Pointer mesh1, MeshType::Pointer mesh2) {

  std::cerr<<"Checking number of nodes...";
  if ( mesh1->GetNumberOfPoints() == mesh2->GetNumberOfPoints() )
    {
    std::cerr<<"Same!"<<std::endl;
    }
  else
    {
    std::cerr<<"NOT same!"<<std::endl;
    }
    
  std::cerr<<"Checking number of triangles...";
  if ( mesh1->GetNumberOfCells() == mesh2->GetNumberOfCells() )
    {
    std::cerr<<"Same!"<<std::endl;
    }
  else
    {
    std::cerr<<"NOT same!"<<std::endl;
    }
    
  std::cerr<<"Checking angles in triangles...";
        
  int numOfPoints = mesh1->GetNumberOfPoints();
  int numOfCells = mesh1->GetNumberOfCells();
  
  // 1. store the points coordinates: pointXYZ
  std::vector< std::vector<double> > pointXYZ1( numOfPoints, std::vector<double>(3, 0) );
  std::vector< std::vector<double> > pointXYZ2( numOfPoints, std::vector<double>(3, 0) );

  PointIterator pntIterator1 = mesh1->GetPoints()->Begin();
  PointIterator pntIterator2 = mesh2->GetPoints()->Begin();

  for ( int it = 0; it < numOfPoints; ++it, ++pntIterator1, ++pntIterator2) 
    {
    ItkPoint pnt1 = pntIterator1.Value();
    ItkPoint pnt2 = pntIterator2.Value();
      
    pointXYZ1[it][0] = pnt1[0];
    pointXYZ1[it][1] = pnt1[1];
    pointXYZ1[it][2] = pnt1[2];
    
    pointXYZ2[it][0] = pnt2[0];
    pointXYZ2[it][1] = pnt2[1];
    pointXYZ2[it][2] = pnt2[2];
    }
  
  CellIterator cellIt = mesh1->GetCells()->Begin(); 
  
  std::vector< std::vector<double> > angleDiff( numOfCells, std::vector<double>(3, 0) );
  
  ofstream myfile;
  myfile.open ("angleDiff.dat");
  
  for ( int itCell = 0;
        itCell < numOfCells;
        ++itCell, ++cellIt)
    {
    CellType * cellptr = cellIt.Value(); 
    // cellptr will point to each cell in the mesh
    // std::cout << cellptr->GetNumberOfPoints() << std::endl;
  
    PointIdIterator pntIdIter = cellptr->PointIdsBegin(); 
    // A, B and C are the three points in the triangle.
    int Aid = *pntIdIter;
    int Bid = *(++pntIdIter);
    int Cid = *(++pntIdIter);
    
    double Ax1 = pointXYZ1[ Aid ][0];
    double Ay1 = pointXYZ1[ Aid ][1];
    double Az1 = pointXYZ1[ Aid ][2];
    double Bx1 = pointXYZ1[ Bid ][0];
    double By1 = pointXYZ1[ Bid ][1];
    double Bz1 = pointXYZ1[ Bid ][2];
    double Cx1 = pointXYZ1[ Cid ][0];
    double Cy1 = pointXYZ1[ Cid ][1];
    double Cz1 = pointXYZ1[ Cid ][2];  

    double Ax2 = pointXYZ2[ Aid ][0];
    double Ay2 = pointXYZ2[ Aid ][1];
    double Az2 = pointXYZ2[ Aid ][2];
    double Bx2 = pointXYZ2[ Bid ][0];
    double By2 = pointXYZ2[ Bid ][1];
    double Bz2 = pointXYZ2[ Bid ][2];
    double Cx2 = pointXYZ2[ Cid ][0];
    double Cy2 = pointXYZ2[ Cid ][1];
    double Cz2 = pointXYZ2[ Cid ][2];          
    
    double lAB1 = sqrt( (Ax1 - Bx1)*(Ax1 - Bx1) + (Ay1 - By1)*(Ay1 - By1) + (Az1 - Bz1)*(Az1 - Bz1) );
    double lAC1 = sqrt( (Ax1 - Cx1)*(Ax1 - Cx1) + (Ay1 - Cy1)*(Ay1 - Cy1) + (Az1 - Cz1)*(Az1 - Cz1) );
    double lBC1 = sqrt( (Cx1 - Bx1)*(Cx1 - Bx1) + (Cy1 - By1)*(Cy1 - By1) + (Cz1 - Bz1)*(Cz1 - Bz1) );
    
    double lAB2 = sqrt( (Ax2 - Bx2)*(Ax2 - Bx2) + (Ay2 - By2)*(Ay2 - By2) + (Az2 - Bz2)*(Az2 - Bz2) );
    double lAC2 = sqrt( (Ax2 - Cx2)*(Ax2 - Cx2) + (Ay2 - Cy2)*(Ay2 - Cy2) + (Az2 - Cz2)*(Az2 - Cz2) );
    double lBC2 = sqrt( (Cx2 - Bx2)*(Cx2 - Bx2) + (Cy2 - By2)*(Cy2 - By2) + (Cz2 - Bz2)*(Cz2 - Bz2) );    
      
    angleDiff[itCell][0] = lAB1/lAB2;
    angleDiff[itCell][1] = lBC1/lBC2;
    angleDiff[itCell][2] = lAC1/lAC2;
     
    myfile << lAB1/lAB2 <<" "<< lBC1/lBC2 <<" "<< lAC1/lAC2 << std::endl;
    }
  myfile.close();
  
  std::cerr<<"Done!"<<std::endl<<"Results are written into the angleDiff.dat file."<<std::endl<<"Each line of the file contains three scaling factors of the three edges in each cell before and after flattening. Theoretically they three should be identical to preserve angle."<<std::endl;

  return; 
}
