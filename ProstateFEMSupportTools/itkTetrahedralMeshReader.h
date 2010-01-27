/*=========================================================================


=========================================================================*/
#ifndef __itkTetrahedralMeshReader_h
#define __itkTetrahedralMeshReader_h

#include "itkMesh.h"
#include "itkCellInterface.h"
#include "itkTetrahedronCell.h"
#include "itkMeshSource.h"

namespace itk
{

/** \class itkTetrahedralMeshReader
 * 
 * 
 * \par
 * This class reads mesh from a file and produces ITK mesh. This is supposed
 * to handle different formats, depending on the name set by SetFileName().
 * The mesh should be tetrahedral. 
 *
 * \par PARAMETERS
 * 
 * \par REFERENCE
 *
 * \par INPUT
 *
 * */
  
template <class TOutputMesh>
class ITK_EXPORT TetrahedralMeshReader : public MeshSource<TOutputMesh>
{
public:
  typedef TetrahedralMeshReader Self;
  typedef MeshSource<TOutputMesh> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(TetrahedralMeshReader, MeshSource);

  typedef TOutputMesh OutputMeshType;
  typedef typename OutputMeshType::MeshTraits   OMeshTraits;
  typedef typename OutputMeshType::CellTraits   OCellTraits;
  typedef typename OutputMeshType::PointType    OPointType;
  typedef typename OutputMeshType::CellType     OCellType;
  typedef typename OMeshTraits::PixelType       OPixelType;
  
  typedef typename TOutputMesh::PointsContainer OutputPointsContainer;
  typedef typename TOutputMesh::PointsContainerPointer OutputPointsContainerPointer;
 
  typedef CellInterface<OPixelType, OCellTraits> TCellInterface;
  typedef TetrahedronCell<TCellInterface> TetCell;
  typedef typename TetCell::CellAutoPointer TetCellAutoPointer;
  typename OutputMeshType::Pointer m_OutputMesh;

  itkSetMacro(FileName, std::string);

protected:
  TetrahedralMeshReader();
  ~TetrahedralMeshReader();

  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();
  virtual void GenerateOutputInformation(){};

private:
  std::string m_FileName;

  void TestFileExistanceAndReadability();
  void ReadVTKMesh();      // .vtk
  void ReadGRUMMPMesh();   // .vmesh
  void ReadINRIAMesh();    // .vol (maybe .tet3d later)
  void ReadGHS3DMesh();
  void ReadTet3DMesh();
  void ReadSMugridMesh();
  void ReadTGMesh();
  void ReadNGMesh();
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTetrahedralMeshReader.txx"
#endif 

#endif // __itkTetrahedralMeshReader_h
