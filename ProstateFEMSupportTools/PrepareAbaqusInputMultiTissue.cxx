/*
 * From the volume mesh, surface mesh, and image that contains labels
 * corresponding to the different tissues, produce the abaqus file for
 * simulation input. 
 */

#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "itkTetrahedralMeshReader.h"
#include "vtkTetra.h"
#include <iostream>
#include "vtkPoints.h"
#include "vtkIntArray.h"

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkTetrahedralMeshWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include "MeshUtil.h"

typedef float PixelType;
typedef itk::OrientedImage<PixelType,3> ImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ImageIterator;

int main(int argc, char **argv){

  if(argc<7){
    std::cerr << "Usage: " << "input_image input_surface_mesh input_volume_mesh output_mesh_abaqus " << std::endl << 
      " output_displacements_file output_mesh_vtk [skip_every_nth_vertex]" << std::endl;
    return -1;
  }


  char* inputImageName = argv[1];
  char* inputSurfaceName = argv[2];
  char* inputMeshName = argv[3];
  char* outAbaqusName = argv[4];
  char* outDisplName = argv[5];
  char* outVTKmesh = argv[6];

  int takeEveryNthPoint = 1; // change this to 2 to skip every second point, etc.
  if(argc>7){
    takeEveryNthPoint = atoi(argv[7]);
    std::cout << "During surface displacement initialization will take every " << takeEveryNthPoint << " point" << std::endl;
  }

  
  std::cout << "Input image: " << inputImageName << std::endl;
  std::cout << "Input surface mesh: " << inputSurfaceName << std::endl;
  std::cout << "Input volume mesh: " << inputMeshName << std::endl;
  std::cout << "Output Abaqus mesh: " << outAbaqusName << std::endl;
  std::cout << "Output displacement file: " << outDisplName << std::endl;
  std::cout << "Output VTK mesh: " << outVTKmesh << std::endl;

  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageName);
  imageReader->Update();
  ImageType::Pointer image = imageReader->GetOutput();

  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName(inputMeshName);
  meshReader->Update();
  vtkUnstructuredGrid *mesh = meshReader->GetOutput();

  vtkPolyData *surf;
  vtkPolyDataReader *surfReader = vtkPolyDataReader::New();
  surfReader->SetFileName(inputSurfaceName);
  surfReader->Update();
  surf = surfReader->GetOutput();

  vtkPoints *meshPoints = mesh->GetPoints();
  vtkCellArray *cells = mesh->GetCells();
  vtkIntArray *tissueType = vtkIntArray::New();

  // allocate an array to keep tissue type for each cell
  tissueType->SetNumberOfComponents(1);
  tissueType->SetNumberOfTuples(mesh->GetNumberOfCells());
  tissueType->SetName("TissueType");
  mesh->GetCellData()->AddArray(tissueType);
  tissueType->Delete();
  
  // array to keep displacement vectors for surface nodes
  vtkFloatArray *displ = vtkFloatArray::New();
  displ->SetNumberOfComponents(3);
  displ->SetNumberOfTuples(mesh->GetNumberOfPoints());
  displ->SetName("SurfaceDisplacement");
  mesh->GetPointData()->AddArray(displ);
  displ->Delete();

  int i;
  for(i=0;i<surf->GetNumberOfPoints();i++){
    double surfPt[3], volPt[3], displVal[3];
    surf->GetPoint(i, &surfPt[0]);
    mesh->GetPoint(i, &volPt[0]);
    displVal[0] = surfPt[0]-volPt[0];
    displVal[1] = surfPt[1]-volPt[1];
    displVal[2] = surfPt[2]-volPt[2];
    if(i%takeEveryNthPoint == 0)
      displ->InsertTuple3(i, displVal[0], displVal[1], displVal[2]);
    else
      displ->InsertTuple3(i, 0,0,0);
  }
  // insert 0 displacements for the inner vertices, otherwise the mesh cannot
  // be read by VTK
  for(;i<mesh->GetNumberOfPoints();i++)
    displ->InsertTuple3(i,0,0,0);


  vtkIdType npts, *pts;
  int cellId=0, label4 = 0, label5 = 0;
  std::vector<std::string> label4Cells, label5Cells;

  // iterate over the cells of the mesh and query the image for the label
  // value corrseponding to the center of the cell
  for(cells->InitTraversal();cells->GetNextCell(npts,pts);cellId++){
    vtkTetra* tetra = vtkTetra::New();
    tetra->Initialize(npts, pts, meshPoints);
    
    ImageType::IndexType tetCenterIdx;
    ImageType::PointType tetCenter;
    float maxLabel = 0;
    vtkPoints *tetraPoints = tetra->GetPoints();

    tetCenter[0] = 0;
    tetCenter[1] = 0;
    tetCenter[2] = 0;

    for(i=0;i<4;i++){
      ImageType::PointType point;
      ImageType::IndexType tetNodeIdx;
      float nodeLabel;
      //ImageType::IndexType index;
      point[0] = -tetraPoints->GetPoint(i)[0];
      point[1] = -tetraPoints->GetPoint(i)[1];
      point[2] = tetraPoints->GetPoint(i)[2];
      tetCenter[0] += point[0];
      tetCenter[1] += point[1];
      tetCenter[2] += point[2];
      image->TransformPhysicalPointToIndex(point, tetNodeIdx);
      nodeLabel = image->GetPixel(tetNodeIdx);
      if(nodeLabel>maxLabel)
        maxLabel = nodeLabel;
    }

    tetCenter[0] /= 4.;
    tetCenter[1] /= 4.;
    tetCenter[2] /= 4.;

    image->TransformPhysicalPointToIndex(tetCenter,tetCenterIdx);

    float labelValue = image->GetPixel(tetCenterIdx);

    // if it turns out that all of the cell nodes are located in the zero
    // label, assign 4 arbitrarily
    if(labelValue == 0)
      labelValue = maxLabel > 0 ? maxLabel : 4;

    std::ostringstream outEntryStream;
    outEntryStream << "   " << cellId+1 << ", " << pts[0]+1 << ", " << pts[1]+1 << ", " <<
      pts[2]+1 << ", " << pts[3]+1;
    std::string outEntry = outEntryStream.str();

    switch(int(labelValue)){
      case 4: tissueType->InsertTuple(cellId, &labelValue); 
              label4++; 
              label4Cells.push_back(outEntry);
              break;
      case 5: tissueType->InsertTuple(cellId, &labelValue); 
              label5++; 
              label5Cells.push_back(outEntry);
              break;
      default: assert(0);
    }
  }

  // prepare the abaqus output file
  std::ofstream abaqusMeshFile(outAbaqusName);
  abaqusMeshFile << "*NODE" << std::endl;

  for(i=0;i<mesh->GetNumberOfPoints();i++)
    abaqusMeshFile << "   " << i+1 << ", " << mesh->GetPoint(i)[0] << ", " << 
      mesh->GetPoint(i)[1] << ", " << mesh->GetPoint(i)[2] << std::endl;
  
  abaqusMeshFile << "*ELEMENT,TYPE=C3D4ANP,ELSET=Prostate_CG" << std::endl;
  for(std::vector<std::string>::iterator it=label4Cells.begin();it!=label4Cells.end();++it){
    abaqusMeshFile << *it << std::endl;
  }

  abaqusMeshFile << "*ELEMENT,TYPE=C3D4ANP,ELSET=Prostate_PZ" << std::endl;
  for(std::vector<std::string>::iterator it=label5Cells.begin();it!=label5Cells.end();++it){
    abaqusMeshFile << *it << std::endl;
  }

  std::ofstream displFile(outDisplName);
  for(i=0;i<surf->GetNumberOfPoints();i+=takeEveryNthPoint){
    double *displVal = displ->GetTuple3(i);
    displFile << i+1 << "   " << displVal[0] << " " << displVal[1] << " " << displVal[2] << std::endl;
  }
  abaqusMeshFile.close();
 
//  std::cout << "Tets with label 0: " << label0 << std::endl;
//  std::cout << "Tets with label 4: " << label4 << std::endl;
//  std::cout << "Tets with label 5: " << label5 << std::endl;

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(mesh);
  meshWriter->SetFileName(outVTKmesh);
  meshWriter->Update();

  return 0;
}

