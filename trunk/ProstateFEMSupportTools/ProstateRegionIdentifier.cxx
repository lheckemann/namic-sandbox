/*
 *  Add an int array to vtk mesh to define different tissue types
 */

#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
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

  if(argc<3){
    std::cerr << "Usage: " << "input_image input_mesh output_mesh_abaqus [output_mesh_vtk]" << std::endl;
    return -1;
  }

  char* inputImageName = argv[1];
  char* inputMeshName = argv[2];
  char* outAbaqusName = argv[3];
  char* outVTKmesh = argv[4];

  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageName);
  imageReader->Update();
  ImageType::Pointer image = imageReader->GetOutput();

  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName(inputMeshName);
  meshReader->Update();
  vtkUnstructuredGrid *mesh = meshReader->GetOutput();

  vtkPoints *meshPoints = mesh->GetPoints();
  vtkCellArray *cells = mesh->GetCells();
  vtkIntArray *tissueType = vtkIntArray::New();

  // allocate an array to keep tissue type for each cell
  tissueType->SetNumberOfComponents(1);
  tissueType->SetNumberOfTuples(mesh->GetNumberOfCells());
  tissueType->SetName("TissueType");
  mesh->GetCellData()->AddArray(tissueType);
  tissueType->Delete();

  vtkIdType npts, *pts;
  int i, cellId=0, label0 = 0, label4 = 0, label5 = 0;

  for(cells->InitTraversal();cells->GetNextCell(npts,pts);cellId++){
    assert(npts==4);
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

    switch(int(labelValue)){
      case 4: tissueType->InsertTuple(cellId, &labelValue); label4++; break;
      case 5: tissueType->InsertTuple(cellId, &labelValue); label5++; break;
      default: labelValue = maxLabel > 0 ? maxLabel : 4; 
               tissueType->InsertTuple(cellId, &labelValue); label0++;
    }
  }

//  std::cout << "Tets with label 0: " << label0 << std::endl;
//  std::cout << "Tets with label 4: " << label4 << std::endl;
//  std::cout << "Tets with label 5: " << label5 << std::endl;

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(mesh);
  meshWriter->SetFileName(outVTKmesh);
  meshWriter->Update();

  return 0;
}

