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

typedef MeshType::MeshTraits::PixelType MeshPixelType;
typedef itk::CellInterface<MeshType::MeshTraits::PixelType, MeshType::CellTraits> CellInterfaceType;
typedef itk::TetrahedronCell<CellInterfaceType> TetCell;
typedef TetCell::CellAutoPointer TetCellAutoPointer;

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

  itk::TetrahedralMeshReader<MeshType>::Pointer mesh_reader = 
    itk::TetrahedralMeshReader<MeshType>::New();
  
  mesh_reader->SetFileName(argv[1]);
  mesh_reader->Update();
  
  // Save the cut across the mesh along one of the axis
  MeshType::PointsContainer::ConstIterator inPointsI;
  MeshType::CellsContainer::ConstIterator inCellsI;
  MeshType::Pointer mesh_in = mesh_reader->GetOutput();
  unsigned i, pos;

  // Get the mesh bounding box
  inPointsI = mesh_in->GetPoints()->Begin();
  MeshType::PointType bb1, bb2;
  bb1[0] = 1000; bb2[0] = 0;
  bb1[1] = 1000; bb2[1] = 0;
  bb1[2] = 1000; bb2[2] = 0;
  while(inPointsI != mesh_in->GetPoints()->End()){
    MeshType::PointType curPoint;
    curPoint = inPointsI.Value();
    for(i=0;i<3;i++){
      if(curPoint[i]<bb1[i])
        bb1[i] = curPoint[i];
      if(curPoint[i]>bb2[i])
        bb2[i] = curPoint[i];
    }
    inPointsI++;
  }
  
  std::cout << "Bounding box is: [" << bb1[0] << "," << bb1[1] << "," << bb1[2] << "], ";
  std::cout << " [" << bb2[0] << "," << bb2[1] << "," << bb2[2] << "]" << std::endl;
  
  float cut_plane[3];
  unsigned p;
  for(p=0;p<3;p++){
    std::cout << "Saving the mesh cut along plane " << p << std::endl;
    MeshType::Pointer mesh_cut = MeshType::New();
    i = 0; pos = 0;
    std::map<unsigned,unsigned> id2pos;
    cut_plane[p] = bb1[p]+(bb2[p]-bb1[p])/2.0;
    std::cout << "Cut index is " << cut_plane[p] << std::endl;
    inPointsI = mesh_in->GetPoints()->Begin();
    while(inPointsI != mesh_in->GetPoints()->End()){
      MeshType::PointType curPoint;
      curPoint = inPointsI.Value();
      if(curPoint[p]>=cut_plane[p]){
        id2pos[i] = pos;
        mesh_cut->SetPoint(pos, curPoint);
        pos++;
      }
      i++;
      inPointsI++;
    }
    std::cout << pos << " points of " << mesh_in->GetPoints()->Size() << " selected" << std::endl;

    typedef itk::TetrahedronCell<MeshType::CellType> TetrahedronType;
    unsigned cells_out = 0;
    unsigned long pointIDs[4];
    inCellsI = mesh_in->GetCells()->Begin();
    while(inCellsI != mesh_in->GetCells()->End()){
      TetrahedronType *curTet;
      TetrahedronType::PointIdIterator ptI;
      unsigned points_in = 0;

      curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
      ptI = curTet->PointIdsBegin();
      for(i=0;i<4;i++){
        unsigned long this_pt = *ptI++;
        if(id2pos.find(this_pt)==id2pos.end())
          break;
        pointIDs[points_in] = id2pos[this_pt];
        points_in++;
      }
      if(points_in==4){
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        newTet->SetPointIds(pointIDs);
        mesh_cut->SetCell(cells_out++, newTet);
      }
      inCellsI++;
    }

    std::ostringstream cut_fname;
    std::string input_prefix(argv[1]);
    input_prefix = input_prefix.substr(0, input_prefix.rfind("."));
    cut_fname << input_prefix << "-cut" << p << ".vtk";

    itk::TetrahedralMeshWriter<MeshType>::Pointer mesh_cut_writer =
      itk::TetrahedralMeshWriter<MeshType>::New();
    mesh_cut_writer->SetInput(mesh_cut);
    mesh_cut_writer->SetFileName(cut_fname.str().c_str());
    mesh_cut_writer->Update();
  }

  return 0;
}
