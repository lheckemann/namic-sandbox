/*=========================================================================
 
  Copyright (c) 2005 Andriy Fedorov,
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School

=========================================================================*/
#ifndef __itkVolumeBoundaryCompressionMeshFilter_h
#define __itkVolumeBoundaryCompressionMeshFilter_h

#include "itkImage.h"
#include "itkMesh.h"
#include "itkMeshToMeshFilter.h"
#include "itkVector.h"
#include "itkCellInterface.h"
#include "itkTetrahedronCell.h"
#include "itkCovariantVector.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkImageRegionConstIterator.h"

#include "itkFEM.h"
#include "itkFEMSolver.h"
#include "itkFEMLinearSystemWrappers.h"

#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkInterpolateImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// M.E.S.H.
#include <xalloc.h>
#include <model_analysis.h>
#include <compute_error.h>
#include <model_in.h>
#include <geomutils.h>
#include <mesh_run.h>

#include <string>
#include <list>
#include <map>
#include <vector>

#define USE_PETSC 1

#ifdef USE_PETSC
#include "tetra_mesh.h"
#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
#include <petsc.h>
#include <petscksp.h>
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // USE_PETSC

namespace itk
{

/** \class itkVolumeBoundaryCompressionMeshFilter
 * 
 * 
 * \par
 * This class takes a 3D tetrahedral mesh and binary mask of an object, and
 * tries to adjust the geometry of the mesh by moving the boundary nodes which
 * are not aligned to the boundary using the FEM deformation model.
 *
 * \par
 * The displacements for the boundary nodes are found from the signed distance
 * function calculated from the binary input image. These displacements define
 * the boundary condition for the finite element  model.
 * 
 * We start from a Body-Centered Cubic (BCC) lattice, with the initial BCC
 * spacing defined (TODO: use Medial Axis to detect the smallest feature size and
 * chose the spacing appropriately). Next selected tetrahedra are subdivided
 * (based on user-specified criteria). Neighbouring tetrahedra that have to be
 * subdivided to enforce the conformant mesh. The process is repeated up to the
 * user-specified number of resolutions.
 * 
 * \par PARAMETERS
 * 
 *
 * \par REFERENCE
 *
 * Molino, N., Bridson, R., Teran, J., and Fedkiw, R. "A crystalline, red green 
 * strategy for meshing highly deformable objects with tetrahedra". 12th International 
 * Meshing Roundtable, Sandia National Laboratories, pp.103-114, Sept. 2003.
 * 
 * \par INPUT
 * The input should be a 3D binary image and a 3D tetrahedral mesh.
 *
 *  */
  
//#ifndef ITK_EXPLICIT_INSTANTIATION
//template class Image<unsigned short,3>;
//#endif

template <class TInputMesh, class TOutputMesh, class TInputImage>
class ITK_EXPORT VolumeBoundaryCompressionMeshFilter : public MeshToMeshFilter<TInputMesh,TOutputMesh>
{
public:
  /** Standard "Self" typedef. */
  typedef VolumeBoundaryCompressionMeshFilter         Self;
  typedef MeshToMeshFilter<TInputMesh,TOutputMesh>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(VolumeBoundaryCompressionMeshFilter, MeshToMeshFilter);

  /** Hold on to the type information specified by the template parameters. */
  typedef TOutputMesh OutputMeshType;
  typedef typename OutputMeshType::MeshTraits   OMeshTraits;
  typedef typename OutputMeshType::CellTraits   OCellTraits;
  typedef typename OutputMeshType::PointType    OPointType;
  typedef typename OutputMeshType::CellType     OCellType;
  typedef typename OMeshTraits::PixelType       OPixelType;
  
  typedef TInputMesh InputMeshType;
  typedef typename InputMeshType::MeshTraits   IMeshTraits;
  typedef typename InputMeshType::CellTraits   ICellTraits;
  typedef typename InputMeshType::PointType    IPointType;
  typedef typename InputMeshType::CellType     ICellType;
  typedef typename IMeshTraits::PixelType       IPixelType;

  /** Define tetrahedral cell type */
  typedef CellInterface<IPixelType, ICellTraits> TCellInterface;
  typedef TetrahedronCell<TCellInterface> TetCell;
  typedef typename TetCell::CellAutoPointer TetCellAutoPointer;

  /** Some convenient typedefs. */
  typedef typename OutputMeshType::Pointer OutputMeshPointer;
  typedef typename OutputMeshType::CellTraits CellTraits;
  typedef typename OutputMeshType::PointsContainerPointer PointsContainerPointer;
  typedef typename OutputMeshType::PointsContainer   PointsContainer;
  typedef typename OutputMeshType::CellsContainerPointer CellsContainerPointer;
  typedef typename OutputMeshType::CellsContainer   CellsContainer;
  typedef CovariantVector<double, 2>     doubleVector;
  typedef CovariantVector<int, 2>        intVector;
  
  /** Define the triangular cell types which forms the surface of the model
   * and will be used in FEM application. */
  typedef CellInterface<OPixelType, CellTraits>  TCellInterface;
  typedef TriangleCell<TCellInterface> TriCell;
  typedef typename TriCell::SelfAutoPointer TriCellAutoPointer;

  typedef typename TInputMesh::PointsContainer InputPointsContainer;
  typedef typename TOutputMesh::PointsContainer OutputPointsContainer;
  typedef typename TInputMesh::PointsContainerPointer InputPointsContainerPointer;
  typedef typename TOutputMesh::PointsContainerPointer OutputPointsContainerPointer;
  typedef typename TInputMesh::PointsContainer::ConstIterator InputPointsContainerIterator;
  
  typedef typename TInputMesh::CellsContainer::ConstIterator InputCellsContainerIterator;
  typedef TetrahedronCell<ICellType> InputTetrahedronType;
  typedef TetrahedronCell<OCellType> OutputTetrahedronType;

  /** Input Image Type Definition. */
  typedef TInputImage InputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
     
  /** Type definition for the classified image index type. */
  typedef typename InputImageType::IndexType       InputImageIndexType;
  typedef ImageRegionConstIterator<InputImageType> InputImageIterator;

  /** Type definition of the subdivision test function */
  typedef bool (SubdivisionTestFunctionType)(double* v0, double* v1,
    double* v2, double* v3, Self*);
  typedef SubdivisionTestFunctionType* SubdivisionTestFunctionPointer;
  
  // TODO: document debug feature
  itkSetMacro(InputImagePrefix, std::string); 
  itkSetMacro(TmpDirectory, std::string);

  itkSetMacro(CompressionMethod, int);

#ifndef USE_PETSC
  itkSetMacro(LinearSystemWrapperType, std::string);
#endif
  itkSetMacro(CompressionIterations, unsigned);
  itkSetMacro(SurfaceFileName, std::string);

  itkGetMacro(NumberOfPoints, unsigned);
  itkGetMacro(NumberOfTets, unsigned); // NAME: NumberOfTetras
  itkSetMacro(MaxError, float);

  static const int OPTIMIZATION_COMPRESSION = 0;
  static const int FEM_COMPRESSION = 1;

  /** accept the input image */
  virtual void SetInput( const InputImageType * inputImage );
  virtual void SetInput( const InputMeshType * inputMesh );

protected:
  
  VolumeBoundaryCompressionMeshFilter();
  ~VolumeBoundaryCompressionMeshFilter();
    
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();
  virtual void GenerateOutputInformation(){}; // do nothing

  
private:

  class TetFace{
  public:
    TetFace(){};
    TetFace(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3){
      fourth = v3;
      if(v0<v1)
        if(v1<v2){
          nodes[0] = v0; nodes[1] = v1; nodes[2] = v2;
        } else {
          if(v2<v0){
            nodes[0] = v2; nodes[1] = v0; nodes[2] = v1;
          } else {
            nodes[0] = v0; nodes[1] = v2; nodes[2] = v1;
          }
        }
      else
        if(v0<v2){
          nodes[0] = v1; nodes[1] = v0; nodes[2] = v2;
        } else {
          if(v1<v2){
            nodes[0] = v1; nodes[1] = v2; nodes[2] = v0;
          } else {
            nodes[0] = v2; nodes[1] = v1; nodes[2] = v0;
          }
        }
    }
    
    ~TetFace(){};
    
    bool operator==(const TetFace &f) const{
      return ((f.nodes[0]==this->nodes[0]) && (f.nodes[1]==this->nodes[1]) &&
        (f.nodes[2]==this->nodes[2]));
    }
    
    bool operator<(const TetFace &face1) const{
      if(this->nodes[0]<face1.nodes[0]) 
        return true;
      if(face1.nodes[0]<this->nodes[0])
        return false;
      if(this->nodes[1]<face1.nodes[1])
        return true;
      if(face1.nodes[1]<this->nodes[1])
        return false;
      return this->nodes[2]<face1.nodes[2];
    }

    unsigned int nodes[3];
    unsigned int fourth;
  };
    
  VolumeBoundaryCompressionMeshFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typedef float InternalPixelType;
  typedef Image<InternalPixelType,3> InternalImageType;
  typedef InternalImageType::SizeType InternalImageSizeType;
  typedef InternalImageType::IndexType InternalImageIndexType;
  typedef InternalImageType::PointType InternalImagePointType;
  typedef CastImageFilter<InputImageType,InternalImageType> CastFilterType;
  typedef SignedDanielssonDistanceMapImageFilter<InternalImageType,InternalImageType> DistanceFilterType;
  typedef LinearInterpolateImageFunction<InternalImageType,double> InterpolatorType;
  typedef RecursiveGaussianImageFilter<InternalImageType,InternalImageType> RGFType;
  typedef ResampleImageFilter<InternalImageType,InternalImageType> ResamplerType;
  typedef IdentityTransform<double,3> IdentityTransformType;

  typedef ImageFileReader<InternalImageType> InternalImageReaderType;
  typedef ImageFileWriter<InternalImageType> InternalImageWriterType;

  typedef Mesh<InternalPixelType,3> SurfaceMeshType;
  typedef SurfaceMeshType::CellType SurfaceMeshCellType;
  typedef TriangleCell<SurfaceMeshCellType> SurfaceTriType;
  
  double m_dimX, m_dimY, m_dimZ;
  
  InternalImageType::Pointer m_DistanceImage;
  typename InterpolatorType::Pointer m_Interpolator;
  // the following is the resampled to unit voxel input mask
  typename InputImageType::Pointer m_InputImage;
  InternalImageType::Pointer m_ReadyInputImage;
  InternalImageSizeType m_InputSize;
  InternalImagePointType m_InputOrigin;

  typename InputMeshType::Pointer m_InputMesh;
  typename OutputMeshType::Pointer m_OutputMesh;
  
  
  std::string m_InputImagePrefix;
  std::string m_TmpDirectory;
  int m_CompressionMethod;

  std::string m_SurfaceFileName;
  unsigned m_CompressionIterations;
  float m_MaxError;

  std::vector<unsigned int> m_SurfaceVertices;
  double* m_SurfaceDisplacements;
  std::map<unsigned int,unsigned int> m_SurfaceVertex2Pos;
  std::map<void*,unsigned int> m_SurfaceNode2Pos;
  std::vector<TetFace> m_SurfaceFaces;

  unsigned long m_NumberOfPoints;
  unsigned long m_NumberOfTets;

  // Triangular surface (M.E.S.H. data structures)
  struct model_error surf_error;
  struct model_info* surf_info;
  struct model *surf_model;
  dvertex_t bbox_min;
  struct triangle_list *tl;
  struct t_in_cell_list *fic;
//  struct sample_list ts;
  double cell_sz;
  struct size3d grid_sz;
  struct dist_cell_lists *dcl;
  int *dcl_buf;
  int dcl_buf_sz;
  dvertex_t prev_p;
  double prev_d;

  
  // Surface triangular mesh (when available)
  SurfaceMeshType::Pointer m_SurfaceMesh;
  
#if USE_PETSC
  PETScDeformWrapper m_PETScWrapper;
//  struct tetra_mesh* m_ft_mesh;
#else // USE_PETSC
  // Solver
  fem::Solver m_Solver;
  std::string m_LinearSystemWrapperType;
#endif // USE_PETSC


  bool Initialize();
  void PrepareInputImage();
  void PrepareDistanceImage();
  void Deform();      // compress with physical model
  void Optimize();    // compress using geom. optimization (GRUMMP)
  void UpdateSurfaceDisplacements();
  float MaxSurfaceVoxelDistance();
  void UpdateSurfaceVerticesAndFaces();
  void OrientMesh(OutputMeshType*,bool);
  // Utility functions
  float DistanceAtPoint(double* coords);
  float DistanceAtPointVoxel(double* coords);
  float DistanceBwPoints(double *coord0, double* coord1);
  // -1 if negative, +1 if positive, 0 if inconsistent
  int  GetMeshOrientation();
  int  GetMeshOrientation(const OutputMeshType*);

  /*
  // These are static in M.E.S.H....
  void init_triangle(const vertex_t*, const vertex_t*, 
    const vertex_t*, struct triangle_info*);
  struct triangle_list* model_to_triangle_list(const struct model*);
  */
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVolumeBoundaryCompressionMeshFilter.txx"
#endif

#endif
