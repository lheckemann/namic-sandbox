//#include "itkBinaryMask3DMeshSource.h"
#include "itkBinaryMaskTo3DAdaptiveMeshFilter.h"
#include "itkImageFileReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "MeshUtil.h"
#include <assert.h>

typedef unsigned short PixelType;
typedef itk::Mesh<PixelType,3> MeshType;
typedef MeshUtil<MeshType> MeshUtilType;
typedef itk::Image<PixelType,3> ImageType;
//typedef itk::BinaryMask3DMeshSource<ImageType,MeshType> MeshFilterType;
typedef itk::BinaryMaskTo3DAdaptiveMeshFilter<ImageType,MeshType> MeshFilterType1;
typedef itk::ImageFileReader<ImageType> ImageReaderType;

int main(int argc, char** argv){
  assert(argc>1);
  MeshFilterType1::Pointer mesher = MeshFilterType1::New();
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(argv[2]);
  try{
    reader->Update();
  }catch(itk::ExceptionObject &e){
    assert(0);
  };
  mesher->SetInput(reader->GetOutput());
  mesher->SetNResolutions(3);
  mesher->SetInputImagePrefix(std::string(argv[1]));
  try{
    mesher->Update();
  }catch(itk::ExceptionObject &e){
    std::cerr << "Failed to update the mesher: " << e << std::endl;
  }

  MeshType::Pointer tetra_mesh = mesher->GetOutput();
  typedef MeshType::CellsContainer::ConstIterator CellIterator;
  typedef MeshType::CellType CellType;
  CellIterator cellIterator = tetra_mesh->GetCells()->Begin();
  CellIterator cellEnd = tetra_mesh->GetCells()->End();

  while(cellIterator != cellEnd){
    CellType * cell = cellIterator.Value();
    ++cellIterator;
  }

  std::cout << "Number of points: " << mesher->GetNumberOfPoints() <<
    ", number of tets: " << mesher->GetNumberOfTets() << std::endl;

  MeshUtilType meshutil;
  vtkUnstructuredGrid* vtk_tetra_mesh =
    MeshUtilType::meshToUnstructuredGrid(tetra_mesh);
  vtkUnstructuredGridWriter *vtk_tetra_mesh_writer =
    vtkUnstructuredGridWriter::New();
  vtk_tetra_mesh_writer->SetFileName((std::string("/tmp/")+argv[1]+".vtk").c_str());
  vtk_tetra_mesh_writer->SetInput(vtk_tetra_mesh);
  vtk_tetra_mesh_writer->Update();
  return 0;
}
