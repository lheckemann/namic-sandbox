#ifndef __itkTetrahedralMeshReader_txx
#define __itkTetrahedralMeshReader_txx

#include "itkTetrahedralMeshReader.h"
#include "vtkUnstructuredGridReader.h"
#include <itksys/SystemTools.hxx>
#include "MeshUtil.h"

namespace itk{
  template<class TOutputMesh>
    TetrahedralMeshReader<TOutputMesh>::
    TetrahedralMeshReader(){
      m_FileName = "";
    }
  
  template<class TOutputMesh>
    TetrahedralMeshReader<TOutputMesh>::
    ~TetrahedralMeshReader(){
    }
  
  template<class TOutputMesh>
  void TetrahedralMeshReader<TOutputMesh>::
    PrintSelf(std::ostream& os, Indent indent) const{
      Superclass::PrintSelf(os, indent);
      os << indent << "m_FileName: " << m_FileName << "\n";
    }
  
  // this was copied from ImageFileReader
  template<class TOutputMesh>
  void TetrahedralMeshReader<TOutputMesh>::
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

  

  template<class TOutputMesh>
  void TetrahedralMeshReader<TOutputMesh>::
    GenerateData(){

      typename TOutputMesh::Pointer output = this->GetOutput();
      output->SetBufferedRegion(output->GetRequestedRegion());
      
      if(m_FileName == "")
        itkExceptionMacro(<<"Missing input file name");
      TestFileExistanceAndReadability();

      // Find out the type of data saved
      std::string suffix;
      suffix = 
        m_FileName.substr(m_FileName.rfind(".")+1,m_FileName.size());

      this->m_OutputMesh = this->GetOutput();
      m_OutputMesh->SetBufferedRegion(m_OutputMesh->GetRequestedRegion());
      
      if(suffix == "vtk")
        ReadVTKMesh();
      else if(suffix == "vmesh" or suffix == "out")
        ReadGRUMMPMesh();
      else if(suffix == "vol")
        ReadINRIAMesh();
      else
        itkExceptionMacro(<<"File type not supported");
    }

  template<class TOutputMesh>
  void TetrahedralMeshReader<TOutputMesh>::
    ReadVTKMesh(){
      vtkUnstructuredGridReader *vtk_input_reader;
      vtkUnstructuredGrid *vtk_input;

      vtk_input_reader = vtkUnstructuredGridReader::New();
      std::cout << "Reading vtk mesh: " << m_FileName.c_str() << std::endl;
      vtk_input_reader->SetFileName(m_FileName.c_str());
      vtk_input_reader->Update();
      vtk_input = vtk_input_reader->GetOutput();

      MeshUtil<TOutputMesh>::meshFromUnstructuredGrid(vtk_input, m_OutputMesh);
    }

  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadINRIAMesh(){
      unsigned num_points, num_tetras;
      unsigned i;
      std::ifstream f_in(m_FileName.c_str());
      std::string str_junk;
      char char_junk[255];

      f_in.getline(char_junk, 255);
      f_in.getline(char_junk, 255);
      f_in >> str_junk >> num_points;
      OutputPointsContainerPointer outPoints = 
        m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in >> newPoint[0] >> newPoint[1] >> newPoint[2];
        this->GetOutput()->SetPoint(i, newPoint);
      }
      std::cout << "ok" << std::endl;
      f_in >> str_junk >> num_tetras;
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4];
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }
      std::cout << "ok" << std::endl;
    }
  
  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadGRUMMPMesh(){
      unsigned num_points, num_tetras;
      unsigned i;
      std::ifstream f_in(m_FileName.c_str());
      std::string str_junk;
      f_in >> num_points >> num_tetras;
      OutputPointsContainerPointer outPoints = 
        m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in >> newPoint[0] >> newPoint[1] >> newPoint[2];
        this->GetOutput()->SetPoint(i, newPoint);
      }
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4];
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }

      this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
    }
}

#endif // __itkTetrahedralMeshReader_txx
