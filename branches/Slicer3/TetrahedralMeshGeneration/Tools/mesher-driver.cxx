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
#include "itkTetrahedralMeshWriter.h"
#include <fstream>
#include <string>
#include <getopt.h>
#include <sstream>

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
  std::cout << "--resolution=<number>   specifies the number of resolutions; default is 2" << std::endl;
  std::cout << "--tmp-dir=<name>        specifies the name for directory temporary data will be saved (to speed up the computation for the same input image)" << std::endl;
  std::cout << "--bcc-spacing=<number>  will set the spacing for the initial BCC lattice; default is 1/10th of the smallest dimension of the image" << std::endl;
  std::cout << "--max-iterations=<number>   specifies the number of iterations to use during the surface compression" << std::endl;
  std::cout << "--tri-surface=<name>    specifies .raw file describing triangular surface of the object" << std::endl;
  std::cout << "--compression-method=<number> specifies which boundary compression method to use. Allowed values: 0 (default, physics-based FEM compression) and 1 (swapping/smoothing optimization using GRUMMP)" << std::endl;
  std::cout << "--max-error=<number>    specifies the maximum allowed distance of a mesh surface point from the object surface (default is 0.01)";
  std::cout << "--subdiv-incl={0|1}     switches the inclusion subdivision test; default \"on\"" << std::endl;
  std::cout << "--subdiv-curv={0|1}     switches the curvature subdivision test; default \"on\"" << std::endl;
  std::cout << "--save-cut={0|1|2|3}    saves median cut of the mesh by plane ZY (=1), ZX (=2), or XY (=3) (default 0, no cut is saved)" << std::endl;
  std::cout << "--undeformed-output=<name>  saves the mesh before compression to the boundary to file <name>" << std::endl;
}


static int verbose_flag = 0;
char *inria_fname = NULL, *vtk_fname = NULL, 
     *tmp_dir = NULL, *undeformed_mesh_fname = NULL,
     *tmp_fname = NULL, *surf_fname = NULL;
bool subdiv_incl = true, subdiv_curv = true;
unsigned resolution = 2, bcc_spacing = 10, iterations = 3, compression_method = 0, save_cut_plane = 0;
float max_error = 0.01;
std::string image_file_prefix = "";
std::string input_image = "";

int main(int argc, char** argv){
  
  while(1){
    static struct option long_options[] =
      {
      /* These options set a flag. */
        {"verbose", no_argument, &verbose_flag, 1},
        /* These options don't set a flag.
           We distinguish them by their indices. */
        {"inria-output", required_argument, 0, 'a'},
        {"vtk-output", required_argument, 0, 'b'},
        {"input-image", required_argument, 0, 'c'},
        {"resolution", required_argument, 0, 'd'},
        {"save-cut", required_argument, 0, 'e'},
        {"tmp-dir", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'g'},
        {"bcc-spacing", required_argument, 0, 'h'},
        {"max-iterations", required_argument, 0, 'i'},
        {"tri-surface", required_argument, 0, 'j'},
        {"compression-method", required_argument, 0, 'k'},
        {"max-error", required_argument, 0, 'l'},
        {"subdiv-curv", required_argument, 0, 'm'},
        {"subdiv-incl", required_argument, 0, 'n'},
        {"undeformed-output", required_argument, 0, 'o'},
        {0, 0, 0, 0}
      };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    int c = getopt_long (argc, argv, "a:b:c:d:e:f:g:h:i:j:k:l:",
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

      // --inria-output
    case 'a':
      inria_fname = optarg;
      break;
      // --vtk-output
    case 'b':
      vtk_fname = optarg;
      break;
      // --input-image
    case 'c':
      input_image = optarg;
      break;
      // --resolution
    case 'd':
      resolution = atoi(optarg);
      break;
      // --save-cut
    case 'e':
      save_cut_plane = atoi(optarg);
      break;
      // --tmp-dir
    case 'f':
      tmp_dir = optarg;
      break;
      // --help
    case 'g':
      print_help();
      exit(0);
      break;
      // --bcc-spacing
    case 'h':
      bcc_spacing = atoi(optarg);
      break;
      // --max-iterations
    case 'i':
      iterations = atoi(optarg);
      break;
      // --tri-surface
    case 'j':
      surf_fname = optarg;
      break;
      // --compression-method
    case 'k':
      compression_method = atoi(optarg);
      break;
      // --max-error
    case 'l':
      max_error = atof(optarg);
      break;
      // --subdiv-curv
    case 'm':
      subdiv_curv = bool(atoi(optarg));
      break;
      // --subdiv-incl
    case 'n':
      subdiv_incl = bool(atoi(optarg));
      break;
      // --undeformed-output
    case 'o':
      undeformed_mesh_fname = optarg;
      break;

    case '?':
      /* getopt_long already printed an error message. */
      print_help();
      break;

    default:
      abort ();
      }
  }

  if(!resolution || input_image=="" || save_cut_plane>3){
    print_help();
    exit(0);
  }
  
  PetscInitialize(NULL, NULL, (char*)0, "");

  MeshFilterType1::Pointer mesher = MeshFilterType1::New();
  ImageReaderType::Pointer reader = ImageReaderType::New();
  CompressionFilterType::Pointer compressor = CompressionFilterType::New();
  
  reader->SetFileName(input_image.c_str());
  try{
    reader->Update();
  }catch(itk::ExceptionObject &e){
    assert(0);
  };
  mesher->SetInput(reader->GetOutput());
  mesher->SetNResolutions(resolution);
  mesher->AddSubdivisionTest(MySubdivTest);
  mesher->SetBCCSpacing(bcc_spacing);
  if(tmp_dir){
    image_file_prefix = 
      input_image.substr(0, input_image.rfind("."));
    if(image_file_prefix.rfind("/")>0)
      image_file_prefix = 
        image_file_prefix.substr(image_file_prefix.rfind("/"), 
          image_file_prefix.size());
    mesher->SetInputImagePrefix(image_file_prefix);
    mesher->SetTmpDirectory(tmp_dir);
  }
  
  mesher->SetSubdivInclusion(subdiv_incl);
  mesher->SetSubdivCurvature(subdiv_curv);
  try{
    mesher->Update();
  }catch(itk::ExceptionObject &e){
    std::cerr << "Failed to update the mesher: " << e << std::endl;
  }

  if(undeformed_mesh_fname){
    itk::TetrahedralMeshWriter<MeshType>::Pointer mesh_writer = 
      itk::TetrahedralMeshWriter<MeshType>::New();
    mesh_writer->SetFileName(undeformed_mesh_fname);
    mesh_writer->SetInput(mesher->GetOutput());
    try{
      mesh_writer->Update();
    } catch(itk::ExceptionObject &e){
      std::cerr << "Failed to save undeformed mesh to " << undeformed_mesh_fname << std::endl;
      return -1;
    }
  }
  
  if(surf_fname)
    compressor->SetSurfaceFileName(surf_fname);
  compressor->SetInput(mesher->GetOutput());
  compressor->SetInput(reader->GetOutput());
  compressor->SetCompressionIterations(iterations);
  compressor->SetMaxError(max_error);
  
  if(tmp_dir){
    compressor->SetInputImagePrefix(image_file_prefix);
    compressor->SetTmpDirectory(tmp_dir);
  }
  
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
  
  if(vtk_fname){
    itk::TetrahedralMeshWriter<MeshType>::Pointer mesh_writer = 
      itk::TetrahedralMeshWriter<MeshType>::New();
    mesh_writer->SetFileName(vtk_fname);
    mesh_writer->SetInput(compressor->GetOutput());
    try{
      mesh_writer->Update();
    } catch(itk::ExceptionObject &e){
      std::cerr << "Failed to save VTK mesh to " << vtk_fname << std::endl;
      return -1;
    }
  }
  
  if(inria_fname){
    itk::TetrahedralMeshWriter<MeshType>::Pointer inria_writer = 
      itk::TetrahedralMeshWriter<MeshType>::New();
    inria_writer->SetFileName(inria_fname);
    inria_writer->SetInput(compressor->GetOutput());
    try{
      inria_writer->Update();
    } catch(itk::ExceptionObject &e){
      std::cerr << "Cannot save INRIA mesh to " << inria_fname << std::endl;
      return -1;
    }
  }

  if(save_cut_plane){
    std::cout << "Saving the mesh cut along plane " << save_cut_plane << std::endl;
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
    cut_plane = (reader->GetOutput())->GetLargestPossibleRegion().GetSize()[save_cut_plane-1]/2;
    while(inPointsI != mesh_in->GetPoints()->End()){
      MeshType::PointType curPoint;
      curPoint = inPointsI.Value();
      if(curPoint[save_cut_plane-1]>=cut_plane){
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

    std::ostringstream cut_fname;
    cut_fname << "./mesh_cut-plane" << save_cut_plane << ".vtk";
    vtkUnstructuredGridWriter *vtk_tetra_mesh_writer =
      vtkUnstructuredGridWriter::New();
    vtk_tetra_mesh_writer->SetFileName(cut_fname.str().c_str());
    vtk_tetra_mesh_writer->SetInput(vtk_cut_tetra_mesh);
    vtk_tetra_mesh_writer->Update();
  }
  
  return 0;
}
