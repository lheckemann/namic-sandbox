#ifndef __itkTetrahedralMeshWriter_txx
#define __itkTetrahedralMeshWriter_txx

#include "itkTetrahedralMeshWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include <itksys/SystemTools.hxx>
#include "MeshUtil.h"

namespace itk{
  template<class TInputMesh>
    TetrahedralMeshWriter<TInputMesh>::
    TetrahedralMeshWriter(){
      m_FileName = "";
      this->SetNumberOfRequiredInputs(1);
    }
  
  template<class TInputMesh>
    TetrahedralMeshWriter<TInputMesh>::
    ~TetrahedralMeshWriter(){
    }
  
  template<class TInputMesh>
  void TetrahedralMeshWriter<TInputMesh>::
    PrintSelf(std::ostream& os, Indent indent) const{
      Superclass::PrintSelf(os, indent);
      os << indent << "m_FileName: " << m_FileName << "\n";
    }
  
  // this was copied from ImageFileWriter
  template<class TInputMesh>
  void TetrahedralMeshWriter<TInputMesh>::
    TestFileExistanceAndReadability(){
      // Test if the file exists.
      if( ! itksys::SystemTools::FileExists( m_FileName.c_str() ) )
        {
        itkExceptionMacro(<<"File does not exist");
        }

      // Test if the file can be open for reading access.
      std::ifstream readTester;
      readTester.open( m_FileName.c_str() );
      if( readTester.fail() )
        {
        readTester.close();
        itkExceptionMacro(<<"File cannot be open");
        return;
        }
      readTester.close();
    }

  

  template<class TInputMesh>
  void TetrahedralMeshWriter<TInputMesh>::
    GenerateData(){
      if(m_FileName == "")
        itkExceptionMacro(<<"Missing input file name");

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
      else
        itkExceptionMacro(<<"File type not supported");
    }

  template<class TInputMesh>
  void TetrahedralMeshWriter<TInputMesh>::
    WriteVTKMesh(){
      vtkUnstructuredGridWriter *vtk_output_writer;
      vtkUnstructuredGrid *vtk_input;

      vtk_input = MeshUtil<TInputMesh>::meshToUnstructuredGrid(m_InputMesh);
      vtk_output_writer = vtkUnstructuredGridWriter::New();
      vtk_input = MeshUtil<InputMeshType>::meshToUnstructuredGrid(m_InputMesh);
      vtk_output_writer->SetFileName(m_FileName.c_str());
      vtk_output_writer->SetInput(vtk_input);
      vtk_output_writer->Update();
    }

  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteINRIAMesh(){
      
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
        curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
        if(inCellsI.Value()->GetType() != InputMeshType::CellType::TETRAHEDRON_CELL)
          itkExceptionMacro(<<"Input mesh is not tetrahedral");
        ptI = curTet->PointIdsBegin();
        inria_mesh << *ptI++ << " " << *ptI++ << " " << *ptI++ << " " << *ptI++ << std::endl;
        inCellsI++;
      }
      inria_mesh.close();
    }
  
  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteGRUMMPMesh(){      
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
    }

  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    SetInput(const InputMeshType* itk_mesh){
      this->ProcessObject::SetNthInput(0,
        const_cast<InputMeshType*>(itk_mesh));
    }
}

#endif // __itkTetrahedralMeshWriter_txx
