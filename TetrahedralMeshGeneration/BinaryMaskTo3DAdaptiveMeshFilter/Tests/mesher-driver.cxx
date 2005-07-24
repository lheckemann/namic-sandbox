//#include "itkBinaryMask3DMeshSource.h"
//#define MODEL_SPHERE 1
#include "itkBinaryMaskTo3DAdaptiveMeshFilter.h"
#include "itkVolumeBoundaryCompressionMeshFilter.h"
#include "itkImageFileReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "MeshUtil.h"
#include <assert.h>
#include "itkTetrahedronCell.h"
#include "itkAutomaticTopologyMeshSource.h"

#ifdef __cplusplus
extern "C" {
#endif 
#include <petsc.h>
#include <petscksp.h>
#ifdef __cplusplus
}
#endif

typedef unsigned short PixelType;
typedef itk::Mesh<PixelType,3> MeshType;
typedef itk::AutomaticTopologyMeshSource<MeshType> MeshSourceType;
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
  
  PetscInitialize(NULL, NULL, (char*)0, "");
  KSP ksp;
  PC pc;
  KSPCreate(PETSC_COMM_WORLD, &ksp);

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

  // Save the cut across the mesh along one of the axis
  MeshType::PointsContainer::ConstIterator inPointsI;
  MeshType::CellsContainer::ConstIterator inCellsI;
  MeshType::Pointer mesh_in = compressor->GetOutput();
  unsigned i, pos;

  std::map<unsigned,unsigned> id2pos;
  std::vector<MeshType::PointType> mesh_out_points;
  i = 0; pos = 0;
  inPointsI = mesh_in->GetPoints()->Begin();
  while(inPointsI != mesh_in->GetPoints()->End()){
    MeshType::PointType curPoint;
    curPoint = inPointsI.Value();
    if(curPoint[0]>=115){
      id2pos[i] = pos;
      pos++;
      mesh_out_points.push_back(curPoint);
    }
    i++;
    inPointsI++;
  }
  
  inCellsI = mesh_in->GetCells()->Begin();
  typedef itk::TetrahedronCell<MeshType::CellType> TetrahedronType;
  std::vector<TetrahedronType*> mesh_out_tets;
  while(inCellsI != mesh_in->GetCells()->End()){
    TetrahedronType *curTet;
    TetrahedronType::PointIdIterator ptI;
    unsigned points_in = 0;
    
    curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
    ptI = curTet->PointIdsBegin();
    for(i=0;i<4;i++){
      if(id2pos.find(*ptI++)==id2pos.end())
        break;
      points_in++;
    }
    if(points_in==4)
      mesh_out_tets.push_back(curTet);
    inCellsI++;
  }
  
  MeshSourceType::Pointer mesh_src_out = MeshSourceType::New();
  MeshSourceType::IdentifierArrayType idArray(mesh_out_points.size());
  i = 0;
  
  for(std::vector<MeshType::PointType>::iterator pI=mesh_out_points.begin();
    pI!=mesh_out_points.end();pI++,i++)
    idArray[i] = mesh_src_out->AddPoint((*pI)[0], (*pI)[1], (*pI)[2]);
    
  for(std::vector<TetrahedronType*>::iterator tI=mesh_out_tets.begin();
    tI!=mesh_out_tets.end();tI++)
    mesh_src_out->AddTetrahedron(
      idArray[id2pos[(*tI)->PointIdsBegin()[0]]],
      idArray[id2pos[(*tI)->PointIdsBegin()[1]]], 
      idArray[id2pos[(*tI)->PointIdsBegin()[2]]], 
      idArray[id2pos[(*tI)->PointIdsBegin()[3]]]); 

  MeshType::Pointer mesh_out = mesh_src_out->GetOutput();
  
  vtkUnstructuredGrid* vtk_cut_tetra_mesh =
    MeshUtilType::meshToUnstructuredGrid(mesh_out);
  
  vtk_tetra_mesh_writer->SetFileName((std::string("/tmp/")+argv[1]+"-deformed-cut.vtk").c_str());
  vtk_tetra_mesh_writer->SetInput(vtk_cut_tetra_mesh);
  vtk_tetra_mesh_writer->Update();

  return 0;
}
