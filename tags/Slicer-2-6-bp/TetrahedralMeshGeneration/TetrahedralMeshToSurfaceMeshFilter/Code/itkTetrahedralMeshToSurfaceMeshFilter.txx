/*=========================================================================

  Copyright (c) 2005 Andriy Fedorov, 
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School
  
=========================================================================*/

#ifndef _itkTetrahedralMeshToSurfaceMeshFilter_txx
#define _itkTetrahedralMeshToSurfaceMeshFilter_txx

#include "itkTetrahedralMeshToSurfaceMeshFilter.h"
#include "itkNumericTraits.h"
#include "itkCellInterface.h"
#include "itkTriangleCell.h"
#include <set>

extern "C"{
float exactinit();
double orient3d(double*, double*, double*, double*);
}

namespace itk
{

template<class TInputMesh, class TOutputMesh>
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::TetrahedralMeshToSurfaceMeshFilter()
{
  this->SetNumberOfRequiredInputs(1);
  m_EnforceOrientation = true;
}

template<class TInputMesh, class TOutputMesh>
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::~TetrahedralMeshToSurfaceMeshFilter()
{
}

template<class TInputMesh,class TOutputMesh>
void
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::SetInput(const InputMeshType* image)
{ 
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< InputMeshType * >( image ) );
}

/** Generate the data */
template<class TInputMesh, class TOutputMesh>
void
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::GenerateData()
{
  unsigned long i;
  
  Initialize();
  UpdateSurfaceVerticesAndFaces();
  std::cout << "Surface vertices and faces found" << std::endl;
  if(m_EnforceOrientation)
    OrientSurface();

  // prepare the output surface mesh
  m_OutputMesh = this->GetOutput();
  std::map<unsigned long, unsigned long> ptin2ptout;
  i = 0;
  m_OutputMesh = this->GetOutput();
  for(typename std::vector<unsigned long>::iterator vI=m_SurfaceVertices.begin();
    vI!=m_SurfaceVertices.end();vI++,i++){
    IPointType volPoint;
    OPointType surfPoint;
    m_InputMesh->GetPoint(*vI, &volPoint);
    surfPoint[0] = volPoint[0];
    surfPoint[1] = volPoint[1];
    surfPoint[2] = volPoint[2];
    m_OutputMesh->SetPoint(i, surfPoint);
    ptin2ptout[*vI] = i;
  }

  i = 0;
  typedef TriangleCell<OCellType> TriangleType;
  for(typename std::vector<TetFace>::iterator vI=m_SurfaceFaces.begin();
    vI!=m_SurfaceFaces.end();vI++,i++){
    typename OutputMeshType::CellType::CellAutoPointer cellPtr;
    TetFace thisFace = *vI;
    cellPtr.TakeOwnership(new TriangleType);
    cellPtr->SetPointId(0, ptin2ptout[thisFace.nodes[0]]);
    cellPtr->SetPointId(1, ptin2ptout[thisFace.nodes[1]]);
    cellPtr->SetPointId(2, ptin2ptout[thisFace.nodes[2]]);
    m_OutputMesh->SetCell(i, cellPtr);
  }

  m_OutputMesh = this->GetOutput();
  std::cout << "Num faces: " << m_SurfaceFaces.size() << ", num cells: " 
    << m_OutputMesh->GetCells()->Size() << std::endl;
  m_OutputMesh->SetBufferedRegion(m_OutputMesh->GetRequestedRegion());
}


template<class TInputMesh, class TOutputMesh>
void
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::UpdateSurfaceVerticesAndFaces(){
  // Walk through the input mesh, extract surface vertices
  typedef typename std::map<TetFace,unsigned int> Face2CntMap;
  typedef typename std::map<TetFace,unsigned int>::iterator
    Face2CntMapIterator;
  Face2CntMap face2cnt;
  Face2CntMapIterator face2cntI;
  Face2CntMap** bins;
  unsigned int i, j, k;
  
  //typename std::map<TetFace,unsigned int>::iterator face2cntI;
  const unsigned int tet_face_LUT[16] = 
    { 0, 1, 2, 3,
      0, 1, 3, 2,
      0, 2, 3, 1,
      1, 2, 3, 0 };
  typename TInputMesh::CellsContainer::ConstIterator cellIterator;
  
  std::cout << "Extracting surface vertices..." << std::endl;
  
  bins = new Face2CntMap*[m_InputMesh->GetPoints()->Size()];
  for(i=0;i<m_InputMesh->GetPoints()->Size();i++)
    bins[i] = NULL;

  cellIterator = m_InputMesh->GetCells()->Begin();
  while(cellIterator != m_InputMesh->GetCells()->End()){
    ICellType *curCell;
    InputTetrahedronType *curTet;
    
    curCell = cellIterator.Value();
    if(curCell->GetType() != ICellType::TETRAHEDRON_CELL)
      itkExceptionMacro(<<"Input mesh should be tetrahedral");
    curTet = dynamic_cast<OutputTetrahedronType *>(curCell);

    typename InputTetrahedronType::PointIdIterator ptI = curTet->PointIdsBegin();
    unsigned int face_points[4];
    for(i=0;i<4;i++)
      face_points[i] = *ptI++;
    for(i=0;i<4;i++){
      TetFace thisFace(face_points[tet_face_LUT[i*4]], 
        face_points[tet_face_LUT[i*4+1]],
        face_points[tet_face_LUT[i*4+2]],
        face_points[tet_face_LUT[i*4+3]]);
      if(bins[thisFace.nodes[0]] == NULL){
        Face2CntMap *new_bin = new Face2CntMap;
        (*new_bin)[thisFace] = 1;
        bins[thisFace.nodes[0]] = new_bin;
      } else {
        Face2CntMap *bin_ptr = bins[thisFace.nodes[0]];
        if(bin_ptr->find(thisFace)!=bin_ptr->end())
          (*bin_ptr)[thisFace]++;
        else
          (*bin_ptr)[thisFace] = 1;
      }
    }
    cellIterator++;
  }

  // TODO: add surface manifold test!
  std::set<unsigned int> surfaceVerticesSet;
  m_SurfaceFaces.clear();
  for(i=0;i<m_InputMesh->GetPoints()->Size();i++){
    if(!bins[i])
      continue;
    Face2CntMap curFaceMap = *bins[i];
    for(face2cntI=curFaceMap.begin();face2cntI!=curFaceMap.end();face2cntI++){
      unsigned int j;
      switch((*face2cntI).second){
      case 1:{
        TetFace thisFace = (*face2cntI).first;
        for(j=0;j<3;j++)
          surfaceVerticesSet.insert(thisFace.nodes[j]);
        m_SurfaceFaces.push_back(thisFace);
        break;}
      case 2: break;
      case 0:
      default:assert(0);
      }
    }
    delete bins[i];
  }
  delete [] bins;

  if(m_EnforceOrientation){
    // Make sure all faces are oriented consistently
    for(i=0;i<m_SurfaceFaces.size();i++){
      TetFace thisFace = m_SurfaceFaces[i];
      double vertices[4][3];
      typename OutputMeshType::PointType surfacePoint;

      for(j=0;j<3;j++){
        m_InputMesh->GetPoint(thisFace.nodes[j], &surfacePoint);
        for(k=0;k<3;k++)
          vertices[j][k] = surfacePoint[k];
      }
      m_InputMesh->GetPoint(thisFace.fourth, &surfacePoint);
      for(k=0;k<3;k++)
        vertices[3][k] = surfacePoint[k];
      if(orient3d(&vertices[0][0], &vertices[1][0], &vertices[2][0], &vertices[3][0])>0){
        unsigned tmp_vertex = thisFace.nodes[0];
        thisFace.nodes[0] = thisFace.nodes[1];
        thisFace.nodes[1] = tmp_vertex;
        m_SurfaceFaces[i] = thisFace;
      }
    }
  }
  
  m_SurfaceVertices.clear();
  std::insert_iterator<std::vector<unsigned long> >
    viI(m_SurfaceVertices, m_SurfaceVertices.begin());
  copy(surfaceVerticesSet.begin(), surfaceVerticesSet.end(), viI);
  surfaceVerticesSet.clear();
}


/** Initialize the class fields */
template<class TInputMesh, class TOutputMesh>
bool
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::Initialize()
{
  this->m_InputMesh =
    static_cast<InputMeshType*>(this->ProcessObject::GetInput(0));

  // Mesh input/output initialization
  std::cout << "Mesh initialization..." << std::endl;
}

/** PrintSelf */
template<class TInputMesh, class TOutputMesh>
void
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  /*
  os << indent
     << "ObjectValue: " 
     << static_cast<NumericTraits<unsigned char>::PrintType>(m_ObjectValue)
     << std::endl;

  os << indent
     << "NumberOfNodes: "
     << m_NumberOfNodes
     << std::endl;

  os << indent
     << "NumberOfCells: "
     << m_NumberOfCells
     << std::endl;
     */
}

/* Orient consistently all tetrahedra of the mesh */
template<class TInputMesh, class TOutputMesh>
void
TetrahedralMeshToSurfaceMeshFilter<TInputMesh,TOutputMesh>
::OrientSurface(){
  int i;
  /*
  typename OutputMeshType::CellsContainer::ConstIterator outCellsI = input_mesh->GetCells()->Begin();
  while(outCellsI != input_mesh->GetCells()->End()){
    OutputTetrahedronType *curMeshTet;
    typename OutputTetrahedronType::PointIdIterator ptI;
    double coords[4][3];
    unsigned pointIds[4];
    curMeshTet =
      dynamic_cast<OutputTetrahedronType*>(outCellsI.Value());
    ptI = curMeshTet->PointIdsBegin();
    for(i=0;i<4;i++){
      typename OutputMeshType::PointType tetPoint;
      pointIds[i] = *ptI;
      input_mesh->GetPoint(*ptI, &tetPoint);
      coords[i][0] = tetPoint[0];
      coords[i][1] = tetPoint[1];
      coords[i][2] = tetPoint[2];
      ptI++;
    }
    if(orient3d(&coords[0][0], &coords[1][0], &coords[2][0],
        &coords[3][0])>0)
      if(!orientation){
        typename OutputMeshType::PointType tetPoint;
        tetPoint[0] = coords[1][0];
        tetPoint[1] = coords[1][1];
        tetPoint[2] = coords[1][2];
        input_mesh->SetPoint(pointIds[0], tetPoint);
        tetPoint[0] = coords[0][0];
        tetPoint[1] = coords[0][1];
        tetPoint[2] = coords[0][2];
        input_mesh->SetPoint(pointIds[1], tetPoint);
      }
    else
      if(orientation){
        typename OutputMeshType::PointType tetPoint;
        tetPoint[0] = coords[1][0];
        tetPoint[1] = coords[1][1];
        tetPoint[2] = coords[1][2];
        input_mesh->SetPoint(pointIds[0], tetPoint);
        tetPoint[0] = coords[0][0];
        tetPoint[1] = coords[0][1];
        tetPoint[2] = coords[0][2];
        input_mesh->SetPoint(pointIds[1], tetPoint);
      }
    outCellsI++;
  }
  */
}

} /** end namespace itk. */

#endif

/*
template<class TInputImage, class TOutputMesh>
TYPE
TetrahedralMeshToSurfaceMeshFilter<TInputImage,TOutputMesh>
*/
