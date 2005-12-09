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
#include <fstream>
#include <string>
#include <getopt.h>

// M.E.S.H.
#include <xalloc.h>
#include <model_analysis.h>
#include <compute_error.h>
#include <model_in.h>
#include <geomutils.h>
// ---

#ifdef __cplusplus
extern "C" {
#endif 
#include <petsc.h>
#include <petscksp.h>

float exactinit();
double orient3d(double*, double*, double*, double*);

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

void print_help(){
  std::cout << "Usage:" << std::endl;
  std::cout << "--inria-output=<name>   will save the final mesh in INRIA .vol format in <name>" << std::endl;
  std::cout << "--vtk-output=<name>     will save the final mesh in ASCII VTK format in <name>" << std::endl;
  std::cout << "--input-image=<name>    REQUIRED specifies the input binary image <name>" << std::endl;
  std::cout << "--resolution=<number>   REQUIRED specifies the number of resolutions" << std::endl;
  std::cout << "--tmp-name=<name>       REQUIRED specifies the name for temporary data saved in /tmp (to speed up the computation; use different for each dataset)" << std::endl;
  std::cout << "--bcc-spacing=<number>  will set the spacing for the initial BCC lattice; by default it is equal to 1/10th of the smallest dimension of the image" << std::endl;
  std::cout << "--iterations=<number>   specifies the number of iterations to use during the surface compression" << std::endl;
  std::cout << "--tri-surface=<name>    specifies .raw file describing triangular surface of the object" << std::endl;
  std::cout << "--compression-method=<number> specifies which boundary compression method to use. Allowed values: 0 (default, physics-based FEM compression) and 1 (swapping/smoothing optimization using GRUMMP)" << std::endl;
}


static int verbose_flag = 0;
char *inria_fname = NULL, *vtk_fname = NULL, 
     *input_image = NULL, *tmp_name = NULL,
     *tmp_fname = NULL, *surf_fname = NULL;
unsigned resolution = 0, save_cut = 0, bcc_spacing = 10, iterations = 3, compression_method = 0;

int main(int argc, char** argv){
  
  while(1){
    static struct option long_options[] =
      {
      /* These options set a flag. */
        {"verbose", no_argument, &verbose_flag, 1},
        /* These options don't set a flag.
           We distinguish them by their indices. */
        {"inria-output", optional_argument, 0, 'a'},
        {"vtk-output", optional_argument, 0, 'b'},
        {"input-image", required_argument, 0, 'c'},
        {"resolution", required_argument, 0, 'd'},
        {"save-cut", optional_argument, 0, 'e'},
        {"tmp-name", required_argument, 0, 'f'},
        {"help", optional_argument, 0, 'g'},
        {"bcc-spacing", optional_argument, 0, 'h'},
        {"iterations", optional_argument, 0, 'i'},
        {"tri-surface", optional_argument, 0, 'j'},
        {"compression-method", optional_argument, 0, 'k'},
        {0, 0, 0, 0}
      };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "a:b:c:d:e:f:g:h:i:j:k:",
      long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
      {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
        break;
      printf ("option %s", long_options[option_index].name);
      if (optarg)
        printf (" with arg %s", optarg);
      printf ("\n");
      break;

    case 'a':
      inria_fname = optarg;
      break;

    case 'b':
      vtk_fname = optarg;
      break;

    case 'c':
      input_image = optarg;
      break;

    case 'd':
      resolution = atoi(optarg);
      break;

    case 'e':
      save_cut = atoi(optarg);
      break;

    case 'f':
      tmp_fname = optarg;
      break;

    case 'g':
      print_help();
      exit(0);
      break;

    case 'h':
      bcc_spacing = atoi(optarg);
      break;

    case 'i':
      iterations = atoi(optarg);
      break;

    case 'j':
      surf_fname = optarg;
      break;

    case 'k':
      compression_method = atoi(optarg);
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      abort ();
      }
  }

  if(!tmp_fname || !resolution || !input_image){
    print_help();
    exit(0);
  }
  
  PetscInitialize(NULL, NULL, (char*)0, "");

  MeshFilterType1::Pointer mesher = MeshFilterType1::New();
  ImageReaderType::Pointer reader = ImageReaderType::New();
  CompressionFilterType::Pointer compressor = CompressionFilterType::New();
  
  reader->SetFileName(input_image);
  try{
    reader->Update();
  }catch(itk::ExceptionObject &e){
    assert(0);
  };
  mesher->SetInput(reader->GetOutput());
  mesher->SetNResolutions(resolution);
  mesher->AddSubdivisionTest(MySubdivTest);
  mesher->SetBCCSpacing(bcc_spacing);
  mesher->SetInputImagePrefix(tmp_fname);
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
  /*
  if(vtk_fname){
    vtk_tetra_mesh_writer->SetFileName((std::string(vtk_fname)+".original.vtk").c_str());
    vtk_tetra_mesh_writer->SetInput(vtk_tetra_mesh);
    vtk_tetra_mesh_writer->Update();
  }
  */

  std::cout << "Initializing compressor..." << std::endl;

  if(surf_fname)
    compressor->SetSurfaceFileName(surf_fname);
  compressor->SetInput(tetra_mesh);
  compressor->SetInputImagePrefix(tmp_fname);
  compressor->SetInput(reader->GetOutput());
  compressor->SetCompressionIterations(iterations);
  if(compression_method == 0)
    compressor->SetCompressionMethod(CompressionFilterType::FEM_COMPRESSION);
  else if(compression_method == 1)
    compressor->SetCompressionMethod(CompressionFilterType::OPTIMIZATION_COMPRESSION);
  else {
    std::cerr << "Unknown compression method specified!" << std::endl;
    assert(0);
  }

  try{
    compressor->Update();
  } catch(itk::ExceptionObject &e){
    std::cerr << "Failed to update the compressor: " << e << std::endl;
  }
  
  vtkUnstructuredGrid* vtk_deformed_tetra_mesh =
    MeshUtilType::meshToUnstructuredGrid(compressor->GetOutput());
  
  if(vtk_fname){
    vtk_tetra_mesh_writer->SetFileName(vtk_fname);
    vtk_tetra_mesh_writer->SetInput(vtk_deformed_tetra_mesh);
    vtk_tetra_mesh_writer->Update();
  }

  // Save the cut across the mesh along one of the axis
  MeshType::PointsContainer::ConstIterator inPointsI;
  MeshType::CellsContainer::ConstIterator inCellsI;
  MeshType::Pointer mesh_in = compressor->GetOutput();
  unsigned i, pos;

  std::map<unsigned,unsigned> id2pos;
  std::vector<MeshType::PointType> mesh_out_points;
  i = 0; pos = 0;
  inPointsI = mesh_in->GetPoints()->Begin();
  unsigned cut_plane;
  cut_plane = (reader->GetOutput())->GetLargestPossibleRegion().GetSize()[0]/2;
  while(inPointsI != mesh_in->GetPoints()->End()){
    MeshType::PointType curPoint;
    curPoint = inPointsI.Value();
    if(curPoint[0]>=cut_plane){
      id2pos[i] = pos;
      pos++;
      mesh_out_points.push_back(curPoint);
    }
    i++;
    inPointsI++;
  }
  
  // Save the mesh in .vol INRIA format
  if(inria_fname){
    double* coords = new double[mesh_in->GetPoints()->Size()*3];
    std::cout << "Saving the mesh in INRIA format..." << std::endl;
    std::ofstream inria_mesh(inria_fname);
    inria_mesh << "#VERSION 1.0" << std::endl << std::endl;
    inria_mesh << "#VERTEX " << mesh_in->GetPoints()->Size() << std::endl;
    inPointsI = mesh_in->GetPoints()->Begin();
    i = 0;
    while(inPointsI != mesh_in->GetPoints()->End()){
      MeshType::PointType curPoint;
      curPoint = inPointsI.Value();
      inria_mesh << curPoint[0] << " " << curPoint[1] << " " << curPoint[2] << std::endl;
      coords[3*i] = curPoint[0];
      coords[3*i+1] = curPoint[1];
      coords[3*i+2] = curPoint[2];
      inPointsI++;
      i++;
    }
    inria_mesh << std::endl;

    MeshType::PointsContainer::ConstIterator curPointI;
    inPointsI = mesh_in->GetPoints()->Begin();
    inria_mesh << "#TETRAHEDRON " << mesh_in->GetCells()->Size() << std::endl;
    inCellsI = mesh_in->GetCells()->Begin();
    typedef itk::TetrahedronCell<MeshType::CellType> TetrahedronType;
    std::vector<TetrahedronType*> mesh_out_tets;
    while(inCellsI != mesh_in->GetCells()->End()){
      TetrahedronType *curTet;
      TetrahedronType::PointIdIterator ptI;
      unsigned points_in = 0;
      curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
      ptI = curTet->PointIdsBegin();
      unsigned point_ids[4];
      for(i=0;i<4;i++)
        point_ids[i] = *ptI++;
      
      inria_mesh << point_ids[0] << " " << point_ids[1] << " " << point_ids[2] << " " << point_ids[3] << std::endl;
      inCellsI++;
    }

    inria_mesh.close();
  }

  /*
  inCellsI = mesh_in->GetCells()->Begin();
  typedef itk::TetrahedronCell<MeshType::CellType> TetrahedronType;
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
  */
  return 0;
}
