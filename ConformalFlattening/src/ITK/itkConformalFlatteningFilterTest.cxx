/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransformMeshFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:30:08 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkConformalFlatteningFilterTest.h"

int main( int argc, char * argv [] )
{


  if( argc < 2 )
    {
      std::cerr << "Missing arguments" << std::endl;
      std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
      return -1;
    }




  vtkPolyData *polyData = readDataToPolyData( argv[1] );
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

  //  MeshType::Pointer newMesh = mapping(mesh);
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  typedef itk::ConformalFlatteningFilter< MeshType, MeshType>  FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  // Connect the inputs
  filter->SetInput( mesh ); 

  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output 
  //  MeshType::Pointer outputMesh = filter->GetOutput();
    ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////

  //Begin convert from ITKMesh to vtkPolyData
  vtkPolyData* newPolyData = ITKMeshToVtkPolyData(filter->GetOutput());

  //Display the new polydata

  newPolyData->GetPointData()->SetScalars(curv1->GetOutput()->GetPointData()->GetScalars());
  
  Display(newPolyData);

//   // Declare the type for PointsContainer
//   typedef MeshType::PointsContainer     PointsContainerType;

//   // Declare the type for PointsContainerPointer
//   typedef MeshType::PointsContainerPointer     
//                                         PointsContainerPointer;
//   // Declare the type for Points
//   typedef MeshType::PointType           PointType;

   // Create an input Mesh


//   // Insert data on the Mesh
//   PointsContainerPointer  points = inputMesh->GetPoints();

//   // Fill a cube with points , just to get some data
//   int n = 1;  // let's start with a few of them
//   PointsContainerType::ElementIdentifier  count = 0; // count them

//   for(int x= -n; x <= n; x++)
//     {
//     for(int y= -n; y <= n; y++)
//       {
//       for(int z= -n; z <= n; z++)
//         {
//         PointType p;
//         p[0] = x;
//         p[1] = y;
//         p[2] = z;
//         std::cout << "Inserting point # ";
//         std::cout.width( 3); std::cout << count << "  = ";
//         std::cout.width( 4); std::cout << p[0] << ", ";
//         std::cout.width( 4); std::cout << p[1] << ", ";
//         std::cout.width( 4); std::cout << p[2] << std::endl;
//         points->InsertElement( count, p );
//         count++;
//         }
//       }
//     }
  
//   std::cout << "Input Mesh has " << inputMesh->GetNumberOfPoints();
//   std::cout << "   points " << std::endl;

  
//   // Declare the transform type
//   typedef itk::AffineTransform<float,3> TransformType;
  

//   // Declare the type for the filter
//   typedef itk::TransformMeshFilter<
//                                 MeshType,
//                                 MeshType,
//                                 TransformType  >       FilterType;
            

//   // Create a Filter                                
//   FilterType::Pointer filter = FilterType::New();
  
//   // Create an  Transform 
//   // (it doesn't use smart pointers)
//   TransformType::Pointer   affineTransform = TransformType::New();
//   affineTransform->Scale( 3.5 );
//   TransformType::OffsetType::ValueType tInit[3] = {100,200,300};
//   TransformType::OffsetType   translation = tInit;
//   affineTransform->Translate( translation );

//   // Connect the inputs
//   filter->SetInput( inputMesh ); 
//   filter->SetTransform( affineTransform ); 

//   // Execute the filter
//   filter->Update();
//   std::cout << "Filter: " << filter;

//   // Get the Smart Pointer to the Filter Output 
//   MeshType::Pointer outputMesh = filter->GetOutput();

//   std::cout << "Output Mesh has " << outputMesh->GetNumberOfPoints();
//   std::cout << "   points " << std::endl;

//   // Get the the point container
//   MeshType::PointsContainerPointer  
//                   transformedPoints = outputMesh->GetPoints();


//   PointsContainerType::ConstIterator it = transformedPoints->Begin();
//   while( it != transformedPoints->End() )
//     {
//     PointType p = it.Value();
//     std::cout.width( 5 ); std::cout << p[0] << ", ";
//     std::cout.width( 5 ); std::cout << p[1] << ", ";
//     std::cout.width( 5 ); std::cout << p[2] << std::endl;
//     ++it;
//     }
  
//   // All objects should be automatically destroyed at this point

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

