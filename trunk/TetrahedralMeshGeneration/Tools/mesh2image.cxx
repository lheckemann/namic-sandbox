#include "itkTetrahedralMeshReader.h"
#include "itkTetrahedralMeshWriter.h"
#include "itkImageFileWriter.h"
#include "itkPointSetToImageFilter.h"
#include "itkMeshSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include <iostream>

typedef unsigned short PixelType;
typedef itk::Image<PixelType,3> ImageType;
typedef itk::Mesh<PixelType,3> MeshType;
typedef itk::TetrahedralMeshReader<MeshType> MeshReaderType;
typedef itk::TetrahedralMeshWriter<MeshType> MeshWriterType;
typedef itk::PointSetToImageFilter<MeshType,ImageType> MeshFilterType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::MeshSpatialObject<MeshType> SpatialObjectType;
typedef itk::SpatialObjectToImageFilter<SpatialObjectType,ImageType> ObjectToImageType;

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
  std::cout << "Mesh points: " << mesh->GetPoints()->Size() << std::endl;
  std::cout << "Mesh cells: " << mesh->GetCells()->Size() << std::endl;

  SpatialObjectType::Pointer object = SpatialObjectType::New();
  object->SetMesh(mesh);
  ObjectToImageType::Pointer obj2image = ObjectToImageType::New();
  obj2image->SetInput(object);
  
  MeshFilterType::Pointer meshFilter = MeshFilterType::New();
  meshFilter->SetInput(mesh);
  ImageType::SpacingType spacing;
  spacing.Fill(1.0);
  ImageType::PointType origin;
  origin.Fill(0.0);
  meshFilter->SetSpacing(spacing);
  meshFilter->SetOrigin(origin);
  try{
    meshFilter->Update();
  }catch(itk::ExceptionObject &e){
    std::cout << "MeshFilter failed!" << std::endl;
    assert(0);
  }

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(meshFilter->GetOutput());
  writer->SetFileName("output1.mhd");

  try{
    writer->Update();
  }catch(itk::ExceptionObject &e){
    std::cout << "Failed to update writer!" << std::endl;
    assert(0);
  }
  
  writer->SetInput(obj2image->GetOutput());
  writer->SetFileName("output2.mhd");
  try{
    writer->Update();
  }catch(itk::ExceptionObject &e){
    std::cout << "Failed to update writer!" << std::endl;
    assert(0);
  }

  return 0;
}
