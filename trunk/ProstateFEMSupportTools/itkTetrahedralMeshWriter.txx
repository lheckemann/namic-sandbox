#ifndef __itkTetrahedralMeshWriter_txx
#define __itkTetrahedralMeshWriter_txx

#include "itkTetrahedralMeshWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkCellData.h"
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
      
      std::cout << "File format requested on save is " << suffix << std::endl;

      if(suffix == "vtk")
        WriteVTKMesh();
      else if(suffix == "vmesh")
        WriteGRUMMPMesh();
      else if(suffix == "vol")
        WriteINRIAMesh();
      else if(suffix == "ugrid")
        WriteSMugridMesh();
      else if(suffix == "1" || suffix == "node")
        WriteTGMesh();
      else if(suffix == "msh")
        WriteMSHMesh();
      else if(suffix == "inp")
        WriteAbaqus();
      else
        itkExceptionMacro(<<"File type not supported");
    }

  template<class TInputMesh>
  void TetrahedralMeshWriter<TInputMesh>::
    WriteVTKMesh(){
      vtkUnstructuredGridWriter *vtk_output_writer;
      vtkUnstructuredGrid *vtk_input;
      vtkFloatArray* scalars;

      typename TInputMesh::PointsContainer::ConstIterator ptI;
      typename TInputMesh::PixelType ptData;
      unsigned pointId = 0;
      
      scalars = vtkFloatArray::New();
      ptI = m_InputMesh->GetPoints()->Begin();
      while(ptI!=m_InputMesh->GetPoints()->End()){
        m_InputMesh->GetPointData(pointId, &ptData);
        scalars->InsertTuple1(pointId, ptData);
        ++pointId;
        ++ptI;
      }

      vtk_input = MeshUtil<TInputMesh>::meshToUnstructuredGrid(m_InputMesh);
      vtk_output_writer = vtkUnstructuredGridWriter::New();
      vtk_input = MeshUtil<InputMeshType>::meshToUnstructuredGrid(m_InputMesh);
      vtk_input->GetPointData()->SetScalars(scalars);
      vtk_output_writer->SetFileName(m_FileName.c_str());
      vtk_output_writer->SetInput(vtk_input);
      vtk_output_writer->Update();
    }

  
  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteMSHMesh(){
      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      
//      std::cout << "Saving the mesh in INRIA format..." << std::endl;
      std::ofstream msh_mesh(m_FileName.c_str());

      msh_mesh << "$MeshFormat" << std::endl;
      msh_mesh << "2.0 0 " << sizeof(double) << std::endl;
      msh_mesh << "$EndMeshFormat" << std::endl;

      msh_mesh << "$Nodes" << std::endl;
      msh_mesh << m_InputMesh->GetPoints()->Size() << std::endl;
      i = 0;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        msh_mesh << i << " " << curPoint[0] << " " << curPoint[1] << 
          " " << curPoint[2] << std::endl;
        inPointsI++;
        i++;
      }
      msh_mesh << "$EndNodes" << std::endl;

      msh_mesh << "$Elements" << std::endl;

      typename InputMeshType::PointsContainer::ConstIterator curPointI;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      msh_mesh << m_InputMesh->GetCells()->Size() << std::endl;
      inCellsI = m_InputMesh->GetCells()->Begin();
      typedef itk::TetrahedronCell<typename InputMeshType::CellType> TetrahedronType;
      std::vector<TetrahedronType*> mesh_out_tets;
      i = 0;
      while(inCellsI != m_InputMesh->GetCells()->End()){
        TetrahedronType *curTet;
        typename TetrahedronType::PointIdIterator ptI;
        curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
        if(inCellsI.Value()->GetType() != InputMeshType::CellType::TETRAHEDRON_CELL)
          itkExceptionMacro(<<"Input mesh is not tetrahedral");
        ptI = curTet->PointIdsBegin();
        vtkIdType ids[4];
        ids[0] = (vtkIdType) (*ptI);
        ids[1] = (vtkIdType) (*(ptI+1));
        ids[2] = (vtkIdType) (*(ptI+2));
        ids[3] = (vtkIdType) (*(ptI+3));
        msh_mesh << i << " 4 0 " << ids[0] << " " << ids[1] << " " 
          << ids[2] << " " << ids[3] << std::endl;
        inCellsI++;
        i++;
      }
      msh_mesh << "$EndElements" << std::endl;
    }

  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteINRIAMesh(){
      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      
//      std::cout << "Saving the mesh in INRIA format..." << std::endl;
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
        vtkIdType ids[4];
        ids[0] = (vtkIdType) (*ptI);
        ids[1] = (vtkIdType) (*(ptI+1));
        ids[2] = (vtkIdType) (*(ptI+2));
        ids[3] = (vtkIdType) (*(ptI+3));
        inria_mesh << ids[0] << " " << ids[1] << " " << ids[2] << " " << ids[3] << std::endl;
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
      
//      std::cout << "Saving the mesh in GRUMMP format..." << std::endl;
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
        grummp_mesh << (int) *ptI++ << " " << (int) *ptI++ << " " << (int) *ptI++ << " " << (int) *ptI++ << std::endl;
        inCellsI++;
      }
      grummp_mesh.close();
    }
  
  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteSMugridMesh(){      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      
//      std::cout << "Saving the mesh in SolidMesh ugrid format..." << std::endl;
      std::ofstream sm_mesh(m_FileName.c_str());
      sm_mesh << m_InputMesh->GetPoints()->Size() << " 0 0 " << m_InputMesh->GetCells()->Size() << 
        " 0 0 0 " << std::endl;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      i = 0;
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        sm_mesh << curPoint[0] << " " << curPoint[1] << " " << curPoint[2] << std::endl;
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
        sm_mesh << (*ptI++)+1 << " " << (*ptI++)+1 << " " << (*ptI++)+1 << " " << (*ptI++)+1 << std::endl;
        inCellsI++;
      }
      sm_mesh.close();
    }

  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteAbaqus(){      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      int i;
      vtkUnstructuredGrid *vtk_input;
      vtk_input = MeshUtil<TInputMesh>::meshToUnstructuredGrid(m_InputMesh);
      
      std::cout << "Saving the mesh in Abaqus inp format..." << std::endl;
      std::ofstream sm_mesh(m_FileName.c_str());

      sm_mesh << "*NODE" << std::endl;
//      sm_mesh << "I10, F16.8, F16.8, F16.8" << std::endl;
/*
      vtkIntArray *nodeNumber = vtkIntArray::New();
      nodeNumber->SetNumberOfComponents(1);
      nodeNumber->SetNumberOfTuples(m_InputMesh->GetPoints()->Size());
      nodeNumber->SetName("Node_Numbers");
      vtk_input->GetPointData()->AddArray(nodeNumber);
      nodeNumber->Delete();

      vtkIntArray *elNumber = vtkIntArray::New();
      elNumber->SetNumberOfComponents(1);
      elNumber->SetNumberOfTuples(m_InputMesh->GetCells()->Size());
      elNumber->SetName("Element_Numbers");
      vtk_input->GetCellData()->AddArray(elNumber);
      elNumber->Delete();
*/
//      sm_mesh << m_InputMesh->GetPoints()->Size() << " 0 0 " << m_InputMesh->GetCells()->Size() <<  " 0 0 0 " << std::endl;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      i = 1;
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        sm_mesh << "   " << i << ", " << curPoint[0] << ", " << curPoint[1] << ", " << curPoint[2] << std::endl;
        vtkIdType id = i-1;
//        nodeNumber->InsertTuple1(id,i);
        inPointsI++;
        i++;
      }

      sm_mesh << "* ELEMENT, TYPE=C3D4ANP, ELSET=Prostate_uniform" << std::endl;
//      sm_mesh << "I10, I10, I10, I10, I10" << std::endl;

      typename InputMeshType::PointsContainer::ConstIterator curPointI;
      inPointsI = m_InputMesh->GetPoints()->Begin();
      inCellsI = m_InputMesh->GetCells()->Begin();
      typedef itk::TetrahedronCell<typename InputMeshType::CellType> TetrahedronType;
      std::vector<TetrahedronType*> mesh_out_tets;
      i = 1;
      while(inCellsI != m_InputMesh->GetCells()->End()){
        TetrahedronType *curTet;
        typename TetrahedronType::PointIdIterator ptI;
        unsigned points_in = 0;
        curTet = dynamic_cast<TetrahedronType*>(inCellsI.Value());
        ptI = curTet->PointIdsBegin();
        vtkIdType tetIds[4];
        for(int j=0;j<4;j++)
          tetIds[j] = (*ptI++)+1;
        sm_mesh << "   " << i << ", " << tetIds[0] << ", " << tetIds[1] << ", " << tetIds[2] << ", " << tetIds[3] << std::endl;
        // (*ptI++)+1 << ", " << (*ptI++)+1 << ", " << (*ptI++)+1 << ", " << (*ptI++)+1 << std::endl;
        inCellsI++;
        i++;        
      }
      sm_mesh.close();

/*
      vtkUnstructuredGridWriter *vtk_output_writer = 
        vtkUnstructuredGridWriter::New();
      vtk_output_writer->SetFileName("with_fields.vtk");
      vtk_output_writer->SetInput(vtk_input);
      vtk_output_writer->Update();
*/
      
    }

  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    WriteTGMesh(){      
      typename InputMeshType::PointsContainer::ConstIterator inPointsI;
      typename InputMeshType::CellsContainer::ConstIterator inCellsI;
      typename InputMeshType::PixelType pointData;
      int i;
      bool pointDataAvailable = false;
      
      // Find out the type of data saved
      std::string suffix, prefix;
      suffix = 
        m_FileName.substr(m_FileName.rfind(".")+1,m_FileName.size());
      prefix =
        m_FileName.substr(0, m_FileName.rfind("."));

//      std::cout << "Saving the mesh in SolidMesh ugrid format..." << std::endl;
      
      std::ofstream tg_mesh_node, tg_mesh_ele;
      if(suffix == "node"){
        tg_mesh_node.open(m_FileName.c_str());
        tg_mesh_ele.open((prefix+".ele").c_str());
      } else {
        tg_mesh_node.open((m_FileName+".node").c_str());
        tg_mesh_ele.open((m_FileName+".ele").c_str());
      }
      
      if(m_InputMesh->GetPointData(0, &pointData)){
        // If point data is available, for TetGen it should be saved in a
        // separate .mtr file
        std::ofstream tg_mesh_mtr;
        if(suffix == "node")
          tg_mesh_mtr.open((prefix+".mtr").c_str());
        else
          tg_mesh_mtr.open((m_FileName+".mtr").c_str());
        
        tg_mesh_mtr << m_InputMesh->GetPoints()->Size() << " 1" << std::endl;
        inPointsI = m_InputMesh->GetPoints()->Begin();
        i = 0;
        while(inPointsI != m_InputMesh->GetPoints()->End()){
          m_InputMesh->GetPointData(i, &pointData);
          tg_mesh_mtr << pointData << std::endl;
          i++;
          inPointsI++;
        }
        tg_mesh_mtr.close();
      }

      tg_mesh_node << m_InputMesh->GetPoints()->Size() << " 3 0 0" << std::endl;

      
      // TODO: handle mesh cell data!
      tg_mesh_ele << m_InputMesh->GetCells()->Size() << " 4 0 " << std::endl;
      
      inPointsI = m_InputMesh->GetPoints()->Begin();
      i = 0;
      while(inPointsI != m_InputMesh->GetPoints()->End()){
        typename InputMeshType::PointType curPoint;
        curPoint = inPointsI.Value();
        tg_mesh_node << i << " " << curPoint[0] << " " << curPoint[1] << " " << curPoint[2];
        if(pointDataAvailable){
          m_InputMesh->GetPointData(i-1, &pointData);
          tg_mesh_node << " " << pointData;
        }
        tg_mesh_node << std::endl;
        inPointsI++;
        i++;
      }

      i = 0;
      
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
        tg_mesh_ele << i << " " << (*ptI++) << " " << (*ptI++) << " " << (*ptI++) << " " << (*ptI++) << std::endl;
        inCellsI++;
        i++;
      }
      tg_mesh_node.close();
      tg_mesh_ele.close();
    }


  template<class TInputMesh>
    void TetrahedralMeshWriter<TInputMesh>::
    SetInput(const InputMeshType* itk_mesh){
      this->ProcessObject::SetNthInput(0,
        const_cast<InputMeshType*>(itk_mesh));
    }
}

#endif // __itkTetrahedralMeshWriter_txx
