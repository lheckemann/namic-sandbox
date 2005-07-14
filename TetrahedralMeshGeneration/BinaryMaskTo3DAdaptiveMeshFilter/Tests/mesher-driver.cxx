//#include "itkBinaryMask3DMeshSource.h"
#include "itkBinaryMaskTo3DAdaptiveMeshFilter.h"
#include "itkVolumeBoundaryCompressionMeshFilter.h"
#include "itkImageFileReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "MeshUtil.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif 
#include <petsc.h>
#ifdef __cplusplus
}
#endif

typedef unsigned short PixelType;
typedef itk::Mesh<PixelType,3> MeshType;
typedef MeshUtil<MeshType> MeshUtilType;
typedef itk::Image<PixelType,3> ImageType;
//typedef itk::BinaryMask3DMeshSource<ImageType,MeshType> MeshFilterType;
typedef itk::BinaryMaskTo3DAdaptiveMeshFilter<ImageType,MeshType> MeshFilterType1;
typedef itk::VolumeBoundaryCompressionMeshFilter<MeshType,MeshType,ImageType> CompressionFilterType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;

bool MySubdivTest(double *v0, double *v1, double *v2, double *v3,
  MeshFilterType1 *mptr){
  // e.g., subdivision test based on voxel intensity
  return false;
}

int main(int argc, char** argv){
  assert(argc>1);


  PetscInitialize(&argc, &argv, (char*)0, "");
  
  MeshFilterType1::Pointer mesher = MeshFilterType1::New();
  ImageReaderType::Pointer reader = ImageReaderType::New();
  CompressionFilterType::Pointer compressor = CompressionFilterType::New();
  
  reader->SetFileName(argv[2]);
  try{
    reader->Update();
  }catch(itk::ExceptionObject &e){
    assert(0);
  };
  mesher->SetInput(reader->GetOutput());
  mesher->SetNResolutions(atoi(argv[3]));
  mesher->AddSubdivisionTest(MySubdivTest);
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

  vtk_tetra_mesh_writer->SetFileName((std::string("/tmp/")+argv[1]+"-original.vtk").c_str());
  vtk_tetra_mesh_writer->SetInput(vtk_tetra_mesh);
  vtk_tetra_mesh_writer->Update();

  std::cout << "Initializing compressor..." << std::endl;

  compressor->SetInput(tetra_mesh);
  compressor->SetInputImagePrefix(std::string(argv[1]));
  compressor->SetInput(reader->GetOutput());
  try{
    compressor->Update();
  } catch(itk::ExceptionObject &e){
    std::cerr << "Failed to update the compressor: " << e << std::endl;
  }
  
  vtkUnstructuredGrid* vtk_deformed_tetra_mesh =
    MeshUtilType::meshToUnstructuredGrid(compressor->GetOutput());
  
  vtk_tetra_mesh_writer->SetFileName((std::string("/tmp/")+argv[1]+"-deformed.vtk").c_str());
  vtk_tetra_mesh_writer->SetInput(vtk_deformed_tetra_mesh);
  vtk_tetra_mesh_writer->Update();

  return 0;
}
