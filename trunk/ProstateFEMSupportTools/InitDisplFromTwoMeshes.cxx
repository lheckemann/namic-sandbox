/*
 * From two volume meshes, produce third volume mesh, with the coordinates
 * taken from the first mesh, and displacements at each vertex initialized as
 * the difference in coordinates at each node.
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

  if(argc<4){
    std::cerr << "Usage: " << "initial_mesh deformed_mesh output_mesh" << std::endl;
    return -1;
  }


  char* initMeshName = argv[1];
  char* deformedMeshName = argv[2];
  char* outputMeshName = argv[3];

  vtkUnstructuredGridReader *initMeshReader = vtkUnstructuredGridReader::New();
  initMeshReader->SetFileName(initMeshName);
  initMeshReader->Update();
  vtkUnstructuredGrid *initMesh = initMeshReader->GetOutput();

  vtkUnstructuredGridReader *deformedMeshReader = vtkUnstructuredGridReader::New();
  deformedMeshReader->SetFileName(deformedMeshName);
  deformedMeshReader->Update();
  vtkUnstructuredGrid *deformedMesh = deformedMeshReader->GetOutput();

  // array to keep displacement vectors for surface nodes
  vtkFloatArray *displ = vtkFloatArray::New();
  displ->SetNumberOfComponents(3);
  displ->SetNumberOfTuples(initMesh->GetNumberOfPoints());
  displ->SetName("Displacement");
  initMesh->GetPointData()->AddArray(displ);
  displ->Delete();

  for(int i=0;i<initMesh->GetNumberOfPoints();i++){
    double initPt[3], defPt[3], displVal[3];
    initMesh->GetPoint(i, &initPt[0]);
    deformedMesh->GetPoint(i, &defPt[0]);
    displVal[0] = defPt[0]-initPt[0];
    displVal[1] = defPt[1]-initPt[1];
    displVal[2] = defPt[2]-initPt[2];
    displ->InsertTuple3(i,displVal[0],displVal[1],displVal[2]);
  }

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(initMesh);
  meshWriter->SetFileName(outputMeshName);
  meshWriter->Update();

  return 0;
}
