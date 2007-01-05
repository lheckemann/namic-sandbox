/*=========================================================================
 
  Copyright (c) 2005 Andriy Fedorov,
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School

=========================================================================*/
#ifndef __TetrahedralMeshToSurfaceMeshFilter_h
#define __TetrahedralMeshToSurfaceMeshFilter_h

#include "itkMesh.h"
#include "itkMeshToMeshFilter.h"
#include "itkVector.h"
#include "itkCellInterface.h"
#include "itkTetrahedronCell.h"
#include "itkCovariantVector.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkImageRegionConstIterator.h"

#include <string>
#include <list>
#include <map>
#include <vector>

namespace itk
{

/** \class TetrahedralMeshToSurfaceMesh
 *
 *  */
  
template <class TInputMesh, class TOutputMesh>
class ITK_EXPORT TetrahedralMeshToSurfaceMeshFilter : public MeshToMeshFilter<TInputMesh,TOutputMesh>
{
public:
  /** Standard "Self" typedef. */
  typedef TetrahedralMeshToSurfaceMeshFilter         Self;
  typedef MeshToMeshFilter<TInputMesh,TOutputMesh>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(TetrahedralMeshToSurfaceMeshFilter, MeshToMeshFilter);

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

  /** accept the input image */
  virtual void SetInput( const InputMeshType * inputMesh );
  itkSetMacro(EnforceOrientation, bool);

  /*
  itkGetMacro(SurfacePoints, unsigned long);
  itkGetMacro(SurfaceFaces, unsigned long);
  itkGetMacro(VolumePoints, unsigned long);
  itkGetMacro(VolumeCells, unsigned long);
  */


protected:
  
  TetrahedralMeshToSurfaceMeshFilter();
  ~TetrahedralMeshToSurfaceMeshFilter();
    
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
    
  TetrahedralMeshToSurfaceMeshFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  //typedef Mesh<InternalPixelType,3> SurfaceMeshType;
  //typedef SurfaceMeshType::CellType SurfaceMeshCellType;
  //typedef TriangleCell<SurfaceMeshCellType> SurfaceTriType;
  
  typename InputMeshType::Pointer m_InputMesh;
  typename OutputMeshType::Pointer m_OutputMesh;
  
  std::vector<unsigned long> m_SurfaceVertices;
  std::vector<TetFace> m_SurfaceFaces;

  unsigned long m_SurfacePoints;
//  unsigned long m_SurfaceFaces;
  unsigned long m_VolumePoints;
  unsigned long m_VolumeCells;

  bool m_EnforceOrientation;

  void OrientSurface();
  bool Initialize();
  void UpdateSurfaceVerticesAndFaces();
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTetrahedralMeshToSurfaceMeshFilter.txx"
#endif

#endif
