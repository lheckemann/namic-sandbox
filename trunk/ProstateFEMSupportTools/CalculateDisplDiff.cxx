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
    std::cerr << "Usage: " << "mesh1 mesh2 output_mesh" << std::endl;
    return -1;
  }


  char* mesh1Name = argv[1];
  char* mesh2Name = argv[2];
  char* outputMeshName = argv[3];

  vtkUnstructuredGridReader *initMeshReader = vtkUnstructuredGridReader::New();
  initMeshReader->SetFileName(mesh1Name);
  initMeshReader->Update();
  vtkUnstructuredGrid *mesh1 = initMeshReader->GetOutput();

  vtkUnstructuredGridReader *deformedMeshReader = vtkUnstructuredGridReader::New();
  deformedMeshReader->SetFileName(mesh2Name);
  deformedMeshReader->Update();
  vtkUnstructuredGrid *mesh2 = deformedMeshReader->GetOutput();

  // array to keep displacement vectors for surface nodes
  vtkDataArray *displ1 = mesh1->GetPointData()->GetArray("Displacement");
  vtkDataArray *displ2 = mesh2->GetPointData()->GetArray("Displacement");

  // array to keep displacement vectors for surface nodes
  vtkFloatArray *displDiff = vtkFloatArray::New();
  displDiff->SetNumberOfComponents(3);
  displDiff->SetNumberOfTuples(mesh1->GetNumberOfPoints());
  displDiff->SetName("DisplacementDifference");
  mesh1->GetPointData()->AddArray(displDiff);
  displDiff->Delete();

  float mean = 0, min = 1000, max = 0;
  std::vector<float> diffList;
  for(int i=0;i<mesh1->GetNumberOfPoints();i++){
    double *displVal1, *displVal2;
    double diff[3];
    displVal1 = displ1->GetTuple3(i);
    displVal2 = displ2->GetTuple3(i);
    diff[0] = displVal1[0]-displVal2[0];
    diff[1] = displVal1[1]-displVal2[1];
    diff[2] = displVal1[2]-displVal2[2];
    displDiff->InsertTuple3(i,diff[0],diff[1],diff[2]);
    float magn = sqrt(diff[0]*diff[0]+diff[1]*diff[1]+diff[2]*diff[2]);
    mean += magn;
    if(magn<min)
      min=magn;
    if(magn>max)
      max = magn;
    diffList.push_back(magn);
  }

  std::sort(diffList.begin(),diffList.end());

  std::cout << "Mean difference magnitude: " << mean/(float)mesh1->GetNumberOfPoints() << std::endl;
  std::cout << "Min : "<< min << ", Max: " << max << std::endl;
  std::cout << "Median: " << diffList[diffList.size()/2] << std::endl;
  std::cout << "1st quartile: " << diffList[diffList.size()/4] << std::endl;
  std::cout << "3rd quartile: " << diffList[diffList.size()*3/4] << std::endl;
  std::cout << "Inter-quartile range: " << diffList[diffList.size()*3/4]-diffList[diffList.size()/4] << std::endl;

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(mesh1);
  meshWriter->SetFileName(outputMeshName);
  meshWriter->Update();

  return 0;
}
