/*=========================================================================


=========================================================================*/
#ifndef __itkBinaryMaskTo3DAdaptiveMeshFilter_h
#define __itkBinaryMaskTo3DAdaptiveMeshFilter_h

#include "vnl/vnl_matrix_fixed.h"
#include "itkImage.h"
#include "itkMesh.h"
#include "itkImageToMeshFilter.h"
#include "itkVector.h"
#include "itkCellInterface.h"
#include "itkTetrahedronCell.h"
#include "itkCovariantVector.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkImageRegionConstIterator.h"

#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenSystem.h"
#include "itkInterpolateImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <string>
#include <list>
#include <map>

extern "C"{
float exactinit();
double orient3d(double*, double*, double*, double*);
}

namespace itk
{

/** \class itkBinaryMaskTo3DAdaptiveMeshFilter
 * 
 * 
 * \par
 * This class tries to construct a 3D tetrahedral volume mesh based on a binary mask.
 *
 * \par
 * We start from a Body-Centered Cubic (BCC) lattice, with the initial BCC
 * spacing defined (TODO: use Medial Axis to detect the smallest feature size and
 * chose the spacing appropriately). Next selected tetrahedra are subdivided
 * (based on user-specified criteria). Neighbouring tetrahedra that have to be
 * subdivided to enforce the conformant mesh. The process is repeated up to the
 * user-specified number of resolutions.
 *
 * \par
 * After the maximum resolution has been reached, the mesh is modified to
 * ensure that the boundary is manifold, and to enforce some properties of the
 * surface, which proved to be useful to achieve good quality when compressing
 * the surface to the boundary.
 *
 * \par
 * The algorithm is driven by the distance function computed on the input binary
 * mask. The current subdivision criteria include the proximity of the surface
 * and the surface curvature both computed from the distance image. The
 * surface compression code is separate at the moment.
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
 * The input should be a 3D binary image.
 *
 *  */
  
#ifndef ITK_EXPLICIT_INSTANTIATION
template class Image<unsigned short,3>;
#endif

template <class TInputImage, class TOutputMesh>
class ITK_EXPORT BinaryMaskTo3DAdaptiveMeshFilter : public ImageToMeshFilter<TInputImage,TOutputMesh>
{
public:
  /** Standard "Self" typedef. */
  typedef BinaryMaskTo3DAdaptiveMeshFilter         Self;
  typedef ImageToMeshFilter<TInputImage,TOutputMesh>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(BinaryMaskTo3DAdaptiveMeshFilter, ImageToMeshFilter);

  /** Hold on to the type information specified by the template parameters. */
  typedef TInputImage InputImageType;
  typedef TOutputMesh OutputMeshType;
  typedef typename OutputMeshType::MeshTraits   OMeshTraits;
  typedef typename OutputMeshType::CellTraits   OCellTraits;
  typedef typename OutputMeshType::PointType    OPointType;
  typedef typename OutputMeshType::CellType     OCellType;
  typedef typename OMeshTraits::PixelType       OPixelType;

  /** Define tetrahedral cell type */
  typedef CellInterface<OPixelType, OCellTraits> TCellInterface;
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
  
  // Subdivision criteria
  /** TODO: documentation for all public types/macros */
  itkSetMacro(SubdivInclusion, bool); // NAME: SubdivideBoundaryTetra
  itkSetMacro(SubdivCurvature, bool); // NAME: SubdivideCurvatureTetra
  // Do we need to keep the elements outside of the object?
  itkSetMacro(KeepOutside, bool); // NAME: KeepOutisdeTetra
  itkSetMacro(NResolutions, unsigned); // NAME: MaxSubdivisionLevel
  // TODO: document debug feature
  itkSetMacro(InputImagePrefix, std::string); // 
  itkSetMacro(TmpDirectory, std::string);
  // this variable defines the spacing of the initial BCC lattice. By default,
  // it is equal to 10, which means that the spacing will be 1/10th of the
  // smallest dimension of the image
  itkSetMacro(BCCSpacing, unsigned);

  itkGetMacro(NumberOfPoints, unsigned);
  itkGetMacro(NumberOfTets, unsigned); // NAME: NumberOfTetras

  /** accept the input image */
  virtual void SetInput( const InputImageType * inputImage );

  void AddSubdivisionTest(SubdivisionTestFunctionPointer);

protected:
  // Internal data structures
  struct RGMVertex;
  struct RGMEdge;
  struct RGMTetra;
  struct RGMFace;
  typedef RGMVertex* RGMVertex_ptr;
  typedef RGMEdge*    RGMEdge_ptr;
  typedef RGMTetra*   RGMTetra_ptr;
  typedef RGMFace*    RGMFace_ptr;
  
  struct RGMVertex{
    double coords[3];
  };

  struct RGMEdge{             // 
    RGMVertex_ptr             nodes[2];
    RGMEdge_ptr               children[2];  // cannot allocate two children at a time, because 
                                            // we do not keep all the levels
                                            // and children can be deallocated
                                            // arbitrarily
    RGMVertex_ptr             midv;
    std::list<RGMTetra_ptr>   neihood;
  };

  struct RGMTetra{            // 36 bytes
    RGMEdge_ptr               edges[6];
    RGMTetra_ptr              parent;
    RGMTetra_ptr              greens;       // green children; can be allocated as a consecutive region
    unsigned char             subdiv;
    unsigned char             edges_split;
    unsigned char             edges_orient;
    unsigned char             level;        
    unsigned char             num_greens;
  };

  struct RGMFace{
    void Init(RGMVertex_ptr v0, RGMVertex_ptr v1, RGMVertex_ptr v2){
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
    };
    
    RGMVertex_ptr             nodes[3];
    RGMVertex_ptr             fourth;
    
    bool operator==(const RGMFace &f) const{
      return ((f.nodes[0]==this->nodes[0]) && (f.nodes[1]==this->nodes[1]) &&
        (f.nodes[2]==this->nodes[2]));
    }
    
    bool operator<(const RGMFace &face1) const{
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

  };
  
  struct ltVertexPair{
    bool operator()(const std::pair<RGMVertex_ptr,RGMVertex_ptr> key1,
                    const std::pair<RGMVertex_ptr,RGMVertex_ptr> key2) const{
      unsigned long long catkey1, catkey2;
    
      catkey1 = (unsigned int)key1.first;
      catkey2 = (unsigned int)key2.first;
      catkey1 <<= 32;
      catkey2 <<= 32;
      catkey1 |= (unsigned int)key1.second;
      catkey2 |= (unsigned int)key2.second;
      return catkey1<catkey2;
    }
  };
 
  
  BinaryMaskTo3DAdaptiveMeshFilter();
  ~BinaryMaskTo3DAdaptiveMeshFilter();
    
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();
  virtual void GenerateOutputInformation(){}; // do nothing

  
private:
  BinaryMaskTo3DAdaptiveMeshFilter(const Self&); //purposely not implemented
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
  

  // Constants instead of "#define"s
  static const int SUBDIV_RED = 0x00;
  static const int SUBDIV_GREEN = 0x80;
  
  // Lookup tables
  signed char m_ThirdFaceEdgeLT[36];
  unsigned char m_FaceToEdgesLT[12];
  unsigned char m_IncidentEdgesLT[24];
 
  double m_dimX, m_dimY, m_dimZ;
  double m_BCCSpacing;
  
  std::list<RGMVertex_ptr> m_Vertices;  // these two lists may not be required.
  std::list<RGMEdge_ptr> m_Edges;       // I keep them for memory deallocation.
  std::list<RGMTetra_ptr> m_Tetras;     // (quick&dirty)
  std::list<RGMTetra_ptr> m_PendingTetras;
  std::set<RGMVertex_ptr> m_OutOfEnvelopeVertices;
  std::map<std::pair<RGMVertex_ptr,RGMVertex_ptr>,RGMEdge_ptr,ltVertexPair> m_TmpEdgeMap;
  std::list<SubdivisionTestFunctionPointer> m_SubdivisionCriteria;
  
  InternalImageType::Pointer m_CurvatureImage;
  InternalImageType::Pointer m_DistanceImage;
  InternalImageType::Pointer m_DistanceDerivativeImage;
  typename InterpolatorType::Pointer m_Interpolator;
  // the following is the resampled to unit voxel input mask
  typename InputImageType::Pointer m_InputImage;
  InternalImageType::Pointer m_ReadyInputImage;
  InternalImageSizeType m_InputSize;
  InternalImagePointType m_InputOrigin;
  

  // Subdivision criteria
  bool m_SubdivInclusion;
  bool m_SubdivCurvature;
  
  bool m_KeepOutside;
  unsigned m_NResolutions;
  unsigned m_CurrentResol;
  std::string m_InputImagePrefix;
  std::string m_TmpDirectory;

  unsigned long m_NumberOfPoints;
  unsigned long m_NumberOfTets;

  bool Initialize();
  double FindBCCSpacing();
  void CreateMesh();
  void CreateBCC();
  bool TetraUnderrefined(RGMTetra_ptr);
  unsigned char GetTetraEdgeConf(RGMTetra_ptr);
  unsigned char GetTetraEdgeOrient(RGMTetra_ptr);

  void PrepareInputImage();
  void PrepareDerivativeImage();
  void PrepareDistanceImage();
  void PrepareCurvatureImage();

  // Utility functions
  float DistanceAtPoint(double* coords);
  float DistanceBwPoints(double *coord0, double* coord1);
  bool SubdivInclusionTest(RGMTetra_ptr);
  bool SubdivCurvatureTest(RGMTetra_ptr);
  /*
  bool SubdivInclusionTestFunction(double*, double*, double*, double*, Self*);
  bool SubdivCurvatureTestFunction(double*, double*, double*, double*, Self*);
  */
  bool IsTetraOutside(RGMTetra_ptr);

  RGMVertex_ptr InsertVertex(double, double, double);
  RGMEdge_ptr InsertEdge(RGMVertex_ptr, RGMVertex_ptr);
  RGMTetra_ptr InsertTetraOriented(RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr,
    RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr);
  RGMTetra_ptr InsertTetra(unsigned char, RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr,
    RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr);
  RGMTetra_ptr InsertTetraAllocated(int, RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr,
    RGMEdge_ptr, RGMEdge_ptr, RGMEdge_ptr, RGMTetra_ptr);

  
  void RemoveBCCTetra(RGMTetra_ptr);
  void RemoveTetraAllocated(RGMTetra_ptr);
  void RemoveTetra(RGMTetra_ptr);
  void FinalizeRedTetra(RGMTetra_ptr);
  void FinalizeGreenTetra(RGMTetra_ptr);
  void SplitEdge(RGMEdge_ptr);
  RGMEdge_ptr GetPutTmpEdge(RGMVertex_ptr, RGMVertex_ptr);
  void FindEnvelopedVertices(float edge_inside);
  void EnforceOrientation();
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBinaryMaskTo3DAdaptiveMeshFilter.txx"
#endif

#endif

/*
 * TODO: pass displacements associated with the candidate mesh as point
 * attributes in the ITK Mesh. 
 *
 * Look at VTK LaplacianSmoothingFilter, and make deformation framework
 * similar (# of iter, convergence criteria, scale factor).
 */
