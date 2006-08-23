// Object consisting of a mesh that contains all the information neccessary for my wavelet coefficient computation
#ifndef __ButterflySchemeSphereSource_h
#define __ButterflySchemeSphereSource_h

#include "itkMesh.h"
#include "itkMeshSource.h"
#include "itkTriangleCell.h"
#include "itkMapContainer.h"
#include "itkButterflyStructure.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkPointSet.h"
#include "itkDataObject.h"
using namespace std;

namespace itk
{

template 
<typename Type, 
unsigned int VDimension = 3>
class ButterflySchemeSphereSource : public Object 
{
public:
  
  typedef ButterflySchemeSphereSource         Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(ButterflySchemeSphereSource, Object);

  /** Hold on to the type information specified by the template parameters. */
  const static unsigned int MaxTopoDim=2;
  //typename ButterflyStructure
  typedef double PixelType;
  typedef double CoordinateType;
  typedef double InterpolationWeightType;
  typedef  itk::DefaultStaticMeshTraits<PixelType, VDimension, MaxTopoDim, CoordinateType, InterpolationWeightType> MeshTraits;
  typedef  itk::Mesh <PixelType, VDimension, MeshTraits> MeshType;
  
  
  
  //typedef typename MeshType::MeshTraits   MeshTraits;
  typedef typename MeshType::PointType    PointType;  

  /** Some convenient typedefs. */
  typedef typename MeshType::Pointer MeshPointer;
  typedef typename MeshType::CellType CellType;
  typedef typename MeshType::CellsContainer::ConstIterator CellIterator;
  typedef typename CellType::PointIdIterator PointIdIterator;
  typedef typename MeshType::CellTraits CellTraits;
  typedef typename MeshType::PointsContainerPointer PointsContainerPointer;
  typedef typename MeshType::PointsContainer   PointsContainer;
  typedef typename MeshType::PointsContainer::Iterator PointsIterator;
  
  
  /** Define the triangular cell types which form the surface  */
  typedef itk::CellInterface<PixelType, CellTraits>   CellInterfaceType;
  typedef itk::TriangleCell<CellInterfaceType>         TriCellType;
  typedef typename TriCellType::SelfAutoPointer       TriCellAutoPointer;
  typedef typename TriCellType::CellAutoPointer       CellAutoPointer;

  typedef std::pair<unsigned long,unsigned long> IndexPairType;
  typedef itk::MapContainer<IndexPairType, unsigned long> PointMapType;
  typedef typename PointType::VectorType VectorType;
    

  typedef typename MeshTraits::PointIdentifier          PointIdentifier;
  typedef typename MeshTraits::PointDataContainer       PointDataContainer;
  typedef typename PointDataContainer::Pointer       PointDataContainerPointer;
  typedef typename PointDataContainer::Pointer       PointDataContainerPointer;


  typedef std::vector< std::vector<unsigned long> > Matrice;
 
  /** Set the resolution level to be used for generating cells in the Sphere.
   *  High values of this parameter will produce sphere with more triangles. */
  itkSetMacro(Resolution, unsigned int);
  itkGetMacro(Resolution, unsigned int);

  /** Set/Get Coordinates of the Sphere center. */
  itkSetMacro( Center, PointType  );
  itkGetMacro( Center, PointType  );

  /** Set/Get scales of the Sphere. This is a vector of values that can
   * actually be used for generating ellipsoids aligned with the coordinate
   * axis. */
  itkSetMacro( Scale,  VectorType );
  itkGetMacro( Scale,  VectorType );
 
  /** Set/Get signal */

  void setSignalMatrix (int res);
  typename Matrice* GetSignalMatrix (int i); 
 /** Set and Get the PointDataStructure */
 
  itkSetMacro ( DataPointStructure, PixelType );
  itkGetMacro ( DataPointStructure, PixelType );
  //itkGetMacro ( SignalMatrix, vector <Type> );

  /** Set/Get coefficients */
  //itkSetMacro ( CoefficientMatrix, Matrice);
  //itkGetMacro ( CoefficientMatrix, std::vector< std::vector<double> >);
 
  bool IsSignalVectorAvailable(); //whether the signal information is available 
  bool IsCoefficientVectorAvailable(); //whether the coefficients are available 
  //itkSetMacro ( IndicesSetAtResolution, std::vector<vector<unsigned long> >);
  //itkGetMacro ( IndicesSetAtResolution, std::vector<vector<double> >);
  //GetIndicesSetAtResolution(int k ) //this the K set, used for the forward/backward filters 
  //GetDifferenceIndicesSetAtResolution(int k) //this is the M set, used for the forward/backward filters


  //typename MeshPointer GetTemplateMesh();
void SetSubdivisionLevel(int subdiv);
void GenerateData();

//void SetPointDataS(PointIdentifier, PixelType, MeshType *);
//void SetPointDataS(PointDataContainer*, MeshType *);



  ButterflySchemeSphereSource();
  ~ButterflySchemeSphereSource() {}
  //void PrintSelf(std::ostream& os, itk::Indent indent) const;

protected:
  PointType Divide( const PointType & p1, const PointType & p2) const;

  void AddCell( MeshType * mesh, const unsigned long * pointIds, unsigned long idx);

  /** model center */
  PointType m_Center; 

  /** models resolution */
  unsigned int m_Resolution;

  /** Pointtype Container */
  PointDataContainerPointer  m_PointDataContainer;
 
  /** model scales */
  VectorType m_Scale;
  
  /**dataStructure */
  
  PixelType m_DataPointStructure;

  std::vector < std::vector < unsigned long> > K;
  std::vector < std::vector < unsigned long> > M;

  // For each point in the mesh, generate a vector, storing the id
  // number of triangles containing this point.
  // The vectors of all the points form a vector: pointList.
  std::vector< std::vector<unsigned long> > m_pointList;
  std::vector< std::vector<unsigned long> > m_cellPoint;

  std::vector< std::vector<Type> > m_SignalMatrix;
  //std::vector<vector<double> > m_CoefficientMatrix;

  std::vector < std::vector < double > > tricross;

private:

 ButterflySchemeSphereSource(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkButterflySchemeSphereSource.txx.cpp"
#endif

#endif //_itkRegularSphereMeshSourceButterflyScheme_h

