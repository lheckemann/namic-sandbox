#ifndef __itkTriangularSurfaceMeshWriter_txx
#define __itkTriangularSurfaceMeshWriter_txx

#include "itkTriangularSurfaceMeshWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"
#include <itksys/SystemTools.hxx>
#include "MeshUtil.h"

namespace itk{
  template<class TInputMesh>
    TriangularSurfaceMeshWriter<TInputMesh>::
    TriangularSurfaceMeshWriter(){
      m_FileName = "";
      this->SetNumberOfRequiredInputs(1);
    }
  
  template<class TInputMesh>
    TriangularSurfaceMeshWriter<TInputMesh>::
    ~TriangularSurfaceMeshWriter(){
    }
  
  template<class TInputMesh>
  void TriangularSurfaceMeshWriter<TInputMesh>::
    PrintSelf(std::ostream& os, Indent indent) const{
      Superclass::PrintSelf(os, indent);
      os << indent << "m_FileName: " << m_FileName << "\n";
    }
  
  template<class TInputMesh>
  void TriangularSurfaceMeshWriter<TInputMesh>::
    GenerateData(){
      if(m_FileName == "")
        itkExceptionMacro(<<"Missing output file name");

      m_InputMesh = 
        static_cast<InputMeshType*>(this->ProcessObject::GetInput(0));

      // Find out the type of data saved
      std::string suffix;
      suffix = 
        m_FileName.substr(m_FileName.rfind(".")+1,m_FileName.size());

      m_InputMesh->SetBufferedRegion(m_InputMesh->GetRequestedRegion());
      
      if(suffix == "vtk")
        WriteVTKMesh();
      else if(suffix == "vmesh")
        WriteGRUMMPMesh();
      else if(suffix == "vol")
        WriteINRIAMesh();
      else if(suffix == "raw")
        WriteRawMesh();
      else
        itkExceptionMacro(<<"File type not supported");
    }

  template<class TInputMesh>
  void TriangularSurfaceMeshWriter<TInputMesh>::
    WriteVTKMesh(){
      vtkPolyDataWriter *vtk_output_writer;
      vtkPolyData *vtk_input;

      vtk_input = MeshUtil<TInputMesh>::meshToPolyData(m_InputMesh);
      vtk_output_writer = vtkPolyDataWriter::New();
      vtk_output_writer->SetFileName(m_FileName.c_str());
      vtk_output_writer->SetInput(vtk_input);
      vtk_output_writer->Update();
    }
  
  template<class TInputMesh>
    void TriangularSurfaceMeshWriter<TInputMesh>::
    WriteRawMesh(){
      std::ofstream raw_mesh(m_FileName.c_str());
      raw_mesh << m_InputMesh->GetPoints()->Size() << " "
        << m_InputMesh->GetCells()->Size() << std::endl;
      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        raw_mesh << curPoint[0] << " " << curPoint[1] << " " << curPoint[2] << std::endl;
        inPointsI++;
      }

      inCellsI = m_InputMesh->GetCells()->Begin();
      typedef itk::TriangleCell<typename InputMeshType::CellType> TriangleType;
      while(inCellsI != m_InputMesh->GetCells()->End()){
        TriangleType *curTri;
        typename TriangleType::PointIdIterator ptI;
        if(inCellsI.Value()->GetType() != InputMeshType::CellType::TRIANGLE_CELL)
          itkExceptionMacro(<<"Input surface mesh is not triangular");
        curTri = dynamic_cast<TriangleType*>(inCellsI.Value());
        ptI = curTri->PointIdsBegin();
        raw_mesh << *ptI++ << " " << *ptI++ << " " << *ptI++ << std::endl;
        inCellsI++;
      }
      raw_mesh.close();
  }

  template<class TInputMesh>
    void TriangularSurfaceMeshWriter<TInputMesh>::
    WriteINRIAMesh(){
      itkExceptionMacro(<<"Mesh format is not supported");

      /*
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      
      std::cout << "Saving the mesh in INRIA format..." << std::endl;
      std::ofstream inria_mesh(m_FileName.c_str());
      inria_mesh << "#VERSION 1.0" << std::endl << std::endl;
      inria_mesh << "#VERTEX " << m_InputMesh->GetPoints()->Size() << std::endl;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      i = 0;
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        inria_mesh << curPoint[0] << " " << curPoint[1] << " " << curPoint[2] << std::endl;
        inPointsI++;
      }
      inria_mesh << std::endl;

      typename InputMeshType::PointsContainer::ConstIterator curPointI;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      inria_mesh << "#TETRAHEDRON " << m_InputMesh->GetCells()->Size() << std::endl;
      inCellsI = m_InputMesh->GetCells()->Begin();
      typedef itk::TetrahedronCell<typename InputMeshType::CellType> TetrahedronType;
      std::vector<TetrahedronType*> mesh_out_tets;
      while(inCellsI != m_InputMesh->GetCells()->End()){
        TetrahedronType *curTet;
        typename TetrahedronType::PointIdIterator ptI;
        unsigned points_in = 0;
        curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
        ptI = curTet->PointIdsBegin();
        inria_mesh << *ptI++ << " " << *ptI++ << " " << *ptI++ << " " << *ptI++ << std::endl;
        inCellsI++;
      }
      inria_mesh.close();
      */
    }
  
  template<class TInputMesh>
    void TriangularSurfaceMeshWriter<TInputMesh>::
    WriteGRUMMPMesh(){      
      itkExceptionMacro(<<"Mesh format not supported");
      /*
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      
      std::cout << "Saving the mesh in GRUMMP format..." << std::endl;
      std::ofstream grummp_mesh(m_FileName.c_str());
      grummp_mesh << m_InputMesh->GetPoints()->Size() << " " << m_InputMesh->GetCells()->Size() << std::endl;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      i = 0;
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        grummp_mesh << curPoint[0] << " " << curPoint[1] << " " << curPoint[2] << std::endl;
        inPointsI++;
      }

      typename InputMeshType::PointsContainer::ConstIterator curPointI;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      inCellsI = m_InputMesh->GetCells()->Begin();
      typedef itk::TetrahedronCell<typename InputMeshType::CellType> TetrahedronType;
      std::vector<TetrahedronType*> mesh_out_tets;
      while(inCellsI != m_InputMesh->GetCells()->End()){
        TetrahedronType *curTet;
        typename TetrahedronType::PointIdIterator ptI;
        unsigned points_in = 0;
        curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
        ptI = curTet->PointIdsBegin();
        grummp_mesh << *ptI++ << " " << *ptI++ << " " << *ptI++ << " " << *ptI++ << std::endl;
        inCellsI++;
      }
      grummp_mesh.close();
      */
    }

  template<class TInputMesh>
    void TriangularSurfaceMeshWriter<TInputMesh>::
    SetInput(const InputMeshType* itk_mesh){
      this->ProcessObject::SetNthInput(0,
        const_cast<InputMeshType*>(itk_mesh));
    }
}

#endif // __itkTriangularSurfaceMeshWriter_txx
