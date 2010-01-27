/*=========================================================================


=========================================================================*/
#ifndef __itkTetrahedralMeshWriter_h
#define __itkTetrahedralMeshWriter_h

#include "itkMesh.h"
#include "itkCellInterface.h"
#include "itkTetrahedronCell.h"
#include "itkMeshSource.h"

namespace itk
{

/** \class itkTetrahedralMeshWriter
 * 
 * 
 * \par
 * This class writes an ITK mesh into a file in the specified format. This is 
 * supposed to handle different formats, depending on the name set by SetFileName().
 * The mesh should be tetrahedral. 
 *
 * \par PARAMETERS
 * 
 * \par REFERENCE
 *
 * \par INPUT
 * ITK tetrahedral mesh
 *
 * */
  
template <class TInputMesh>
class ITK_EXPORT TetrahedralMeshWriter : public MeshSource<TInputMesh>
{
public:
  typedef TetrahedralMeshWriter Self;
  typedef MeshSource<TInputMesh> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(TetrahedralMeshWriter, MeshSource);

  typedef TInputMesh InputMeshType;
  typedef typename InputMeshType::MeshTraits   IMeshTraits;
  typedef typename InputMeshType::CellTraits   ICellTraits;
  typedef typename InputMeshType::PointType    IPointType;
  typedef typename InputMeshType::CellType     ICellType;
  typedef typename IMeshTraits::PixelType       IPixelType;
  
  typedef typename TInputMesh::PointsContainer InputPointsContainer;
  typedef typename TInputMesh::PointsContainerPointer InputPointsContainerPointer;
 
  typedef CellInterface<IPixelType, ICellTraits> TCellInterface;
  typedef TetrahedronCell<TCellInterface> TetCell;
  typedef typename TetCell::CellAutoPointer TetCellAutoPointer;
  typename InputMeshType::Pointer m_InputMesh;

  itkSetMacro(FileName, std::string);

  virtual void SetInput(const InputMeshType*);

protected:
  TetrahedralMeshWriter();
  ~TetrahedralMeshWriter();

  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();
  virtual void GenerateOutputInformation(){};

private:
  std::string m_FileName;

  void TestFileExistanceAndReadability();
  void WriteVTKMesh();      // .vtk
  void WriteGRUMMPMesh();   // .vmesh
  void WriteINRIAMesh();    // .vol (maybe .tet3d later)
  void WriteSMugridMesh();
  void WriteTGMesh();
  void WriteMSHMesh();      // .msh for Gmsh
  void WriteAbaqus();
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTetrahedralMeshWriter.txx"
#endif 

#endif // __itkTetrahedralMeshWriter_h
