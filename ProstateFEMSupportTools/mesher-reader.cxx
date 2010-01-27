#include "itkTetrahedralMeshReader.h"
#include "itkTetrahedralMeshWriter.h"
#include <iostream>

typedef float PixelType;
typedef itk::Mesh<PixelType,3> MeshType;
typedef itk::TetrahedralMeshReader<MeshType> MeshReaderType;
typedef itk::TetrahedralMeshWriter<MeshType> MeshWriterType;

int main(int argc, char **argv){
  MeshReaderType::Pointer mesh_reader = 
    MeshReaderType::New();
  mesh_reader->SetFileName(argv[1]);
  try{
    mesh_reader->Update();
  }catch(itk::ExceptionObject &e){
    std::cout << "Exception " << e << std::endl;
    assert(0);
  }
  std::cout << "OK" << std::endl;
  MeshType::Pointer mesh = mesh_reader->GetOutput();
  mesh = mesh_reader->GetOutput();
  std::cout << "Mesh points: " << mesh->GetPoints()->Size() << std::endl;
  std::cout << "Mesh cells: " << mesh->GetCells()->Size() << std::endl;

  MeshWriterType::Pointer mesh_writer =
    MeshWriterType::New();
  mesh_writer->SetFileName(argv[2]);
  mesh_writer->SetInput(mesh);
  try{
    mesh_writer->Update();
  } catch(itk::ExceptionObject &e){
    std::cout << "Exception " << e << std::endl;
    assert(0);
  }
  return 0;
}
