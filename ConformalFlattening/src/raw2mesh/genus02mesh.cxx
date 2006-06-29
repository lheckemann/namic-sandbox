#include "genus02mesh.h"

int main( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: InputImage vtkPolyDataOutputFile" << std::endl;
    return -1;
    }

  /* Read genus 0 binary image from file */
  std::cerr << "Read genus 0 binary image from file..." << std::endl;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  /* Convert ITK pipeline to a VTK pipeline*/
  std::cerr << "Convert ITK pipeline to a VTK pipeline..." << std::endl;
  VTKImageExportType::Pointer vtkImageExportFilter = VTKImageExportType::New();
  vtkImageExportFilter->SetInput( reader->GetOutput() );
  vtkImageImport* vtkImportImageObject = vtkImageImport::New();
  ConnectITKToVTK( vtkImageExportFilter, vtkImportImageObject );
  
//  vtkImageShrink3D  *imShrink = vtkImageShrink3D ::New();
//  imShrink->SetInput( vtkImportImageObject->GetOutput() );
//  imShrink->SetShrinkFactors (5, 5, 5);  

  /* Run marching cubes to generate the mesh*/
  std::cerr << "Run marching cubes to generate the mesh..." << std::endl;
  vtkMarchingCubes *cubes = vtkMarchingCubes::New();
//  cubes->SetInput( imShrink->GetOutput() );
  cubes->SetInput( vtkImportImageObject->GetOutput() );
  cubes->SetValue(0, 0.5);
  cubes->Update();

  std::cerr<<"Number of Cells after shrink: "<<cubes->GetOutput()->GetNumberOfCells()<<std::endl;
  
//  Display( cubes->GetOutput() );
  
//  /* Smooth the mesh */
//  std::cerr << "Smooth the mesh..." << std::endl;
//  vtkWindowedSincPolyDataFilter* smooth = vtkWindowedSincPolyDataFilter::New();
//  smooth->SetInput( cubes->GetOutput() );
//  smooth->Update();
//  
//  /* Decimate the mesh */
//  std::cout<<"Points, before decimation = "<< smooth->GetOutput()->GetNumberOfPoints() <<std::endl;
//  std::cout<<"Cells, before decimation = "<< smooth->GetOutput()->GetNumberOfCells() <<std::endl;
//  std::cerr << "Decimate the mesh..." << std::endl;
//  vtkQuadricDecimation *decimator = vtkQuadricDecimation::New();
//  decimator->SetInput( smooth->GetOutput() );
//  decimator->SetTargetReduction( 0.1 );
//  std::cout<<"Points, after decimation = "<< decimator->GetOutput()->GetNumberOfPoints() <<std::endl;
//  std::cout<<"Cells, after decimation = "<< decimator->GetOutput()->GetNumberOfCells() <<std::endl;
//
//  /* Set the color according to local mean/gaussian curvature */
//  std::cerr << "Set the color according to local mean/gaussian curvature..." << std::endl;
//  vtkCurvatures* meanCurvatures = vtkCurvatures::New();
//  meanCurvatures->SetInput( decimator->GetOutput() );
//  //  meanCurvatures->SetCurvatureTypeToGaussian();
//  meanCurvatures->SetCurvatureTypeToMean();
//  Display( meanCurvatures->GetOutput() );

  /* Convert back to an ITK mesh */
  std::cerr << "Convert back to an ITK mesh..." << std::endl;
  //MeshType::Pointer smoothedMesh = vtkPolyDataToITKMesh( decimator->GetOutput() );
  MeshType::Pointer smoothedMesh = vtkPolyDataToITKMesh( cubes->GetOutput() );

  /* Execute the conformal flattening */
  std::cerr << "Execute the conformal flattening..." << std::endl;
  ConformalFlatteningFilterType::Pointer conformalFlatteningFilter = ConformalFlatteningFilterType::New();
  conformalFlatteningFilter->SetInput( smoothedMesh );
  conformalFlatteningFilter->Update();   

  /* Begin convert from ITKMesh to vtkPolyData */
  std::cerr << "Begin convert from ITKMesh to vtkPolyData..." << std::endl;
  vtkPolyData* conformallyFlattenedPolyData = ITKMeshToVtkPolyData( conformalFlatteningFilter->GetOutput() );

  /* Display the new polydata */
  std::cerr << "Display the new polydata..." << std::endl;
  // conformallyFlattenedPolyData->GetPointData()->SetScalars( meanCurvatures->GetOutput()->GetPointData()->GetScalars() );
  Display( conformallyFlattenedPolyData );
  
  return EXIT_SUCCESS;
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


vtkPolyData* ITKMeshToVtkPolyData( MeshType::Pointer mesh )
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

void ConnectITKToVTK( VTKImageExportType* in, vtkImageImport* out )
{
  out->SetUpdateInformationCallback(in->GetUpdateInformationCallback());
  out->SetPipelineModifiedCallback(in->GetPipelineModifiedCallback());
  out->SetWholeExtentCallback(in->GetWholeExtentCallback());
  out->SetSpacingCallback(in->GetSpacingCallback());
  out->SetOriginCallback(in->GetOriginCallback());
  out->SetScalarTypeCallback(in->GetScalarTypeCallback());
  out->SetNumberOfComponentsCallback(in->GetNumberOfComponentsCallback());
  out->SetPropagateUpdateExtentCallback(in->GetPropagateUpdateExtentCallback());
  out->SetUpdateDataCallback(in->GetUpdateDataCallback());
  out->SetDataExtentCallback(in->GetDataExtentCallback());
  out->SetBufferPointerCallback(in->GetBufferPointerCallback());
  out->SetCallbackUserData(in->GetCallbackUserData());
};
