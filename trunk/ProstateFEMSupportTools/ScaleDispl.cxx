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
    std::cerr << "Usage: " << "mesh scale_const output_mesh" << std::endl;
    return -1;
  }


  char* mesh1Name = argv[1];
  float scaleConst = atof(argv[2]);
  char* outputMeshName = argv[3];

  vtkUnstructuredGridReader *initMeshReader = vtkUnstructuredGridReader::New();
  initMeshReader->SetFileName(mesh1Name);
  initMeshReader->Update();
  vtkUnstructuredGrid *mesh1 = initMeshReader->GetOutput();

  // array to keep displacement vectors for surface nodes
  vtkDataArray *displ1 = mesh1->GetPointData()->GetArray("Displacement");

  for(int i=0;i<mesh1->GetNumberOfPoints();i++){
    double *displVal1;
    displVal1 = displ1->GetTuple3(i);
    displVal1[0] *= scaleConst;
    displVal1[1] *= scaleConst;
    displVal1[2] *= scaleConst;
    displ1->InsertTuple3(i,displVal1[0],displVal1[1],displVal1[2]);
  }

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(mesh1);
  meshWriter->SetFileName(outputMeshName);
  meshWriter->Update();

  return 0;
}
