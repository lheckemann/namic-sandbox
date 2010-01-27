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
        itkExceptionMacro(<<"File " << m_FileName << " does not exist");
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
      
      // Find out the type of data saved
      std::string suffix;
      suffix = 
        m_FileName.substr(m_FileName.rfind(".")+1,m_FileName.size());
      
      if(m_FileName == "")
        itkExceptionMacro(<<"Missing input file name");

      if(suffix != "1")
        TestFileExistanceAndReadability();

      this->m_OutputMesh = this->GetOutput();
      m_OutputMesh->SetBufferedRegion(m_OutputMesh->GetRequestedRegion());
      
      if(suffix == "vtk")
        ReadVTKMesh();
      else if(suffix == "vmesh" or suffix == "out")
        ReadGRUMMPMesh();
      else if(suffix == "vol")
        ReadINRIAMesh();
      else if(suffix == "tet3D" || suffix == "atet3D")
        ReadTet3DMesh();
      else if(suffix == "ugrid")
        ReadSMugridMesh();
      else if(suffix == "noboite")
        ReadGHS3DMesh();
      else if(suffix == "neu")
        ReadNGMesh();
      else if(suffix == "1" || suffix == "node")  // TODO: input filename handling should be more robust!
        ReadTGMesh();
      else
        itkExceptionMacro(<<"File type not supported");
    }

  template<class TOutputMesh>
  void TetrahedralMeshReader<TOutputMesh>::
    ReadVTKMesh(){
      vtkUnstructuredGridReader *vtk_input_reader;
      vtkUnstructuredGrid *vtk_input;
      vtkDataArray* scalars;
      unsigned i;

      vtk_input_reader = vtkUnstructuredGridReader::New();
      vtk_input_reader->SetFileName(m_FileName.c_str());
      vtk_input_reader->Update();
      vtk_input = vtk_input_reader->GetOutput();
      scalars = vtk_input->GetPointData()->GetScalars();

      MeshUtil<TOutputMesh>::meshFromUnstructuredGrid(vtk_input, m_OutputMesh);

      // TODO: handle better? (there's not necessarily one single data item 
      // assigned to each point/cell...)
      //for(i=0;i<scalars->GetSize();i++){
      //  m_OutputMesh->SetPointData(i, (typename TOutputMesh::PixelType)scalars->GetTuple1(i));
      // }
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
    }
  
   template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadGHS3DMesh(){
      // This is derived from 
      // Biofem/StdPkgs/MeshPkg/Tetrahedrisation/Tetra3D.C 
      unsigned num_points, num_tetras;
      unsigned i, j;
      std::ifstream f_in(m_FileName.c_str());
      std::string str_junk;
      int int_junk;
      char char_junk[255];

      f_in >> num_tetras >> num_points;
      for(i=0;i<15;i++)
        f_in >> int_junk;
      // read tetra
      unsigned long *tetra_table = new unsigned long[4*num_tetras];
      for(i=0;i<num_tetras;i++){
        for(j=0;j<4;j++){
          f_in >> tetra_table[i*4+j];
          tetra_table[i*4+j]--;
        }
      }
      
      // read vertices
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
        newTet->SetPointIds(&tetra_table[i*4]);
        for(j=0;j<4;j++)
          pointIDs[j] = tetra_table[i*4+j];
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }

      delete [] tetra_table;
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
  
  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadTet3DMesh(){
      unsigned num_points, num_tetras;
      unsigned i;
      std::ifstream f_in(m_FileName.c_str());
      char str_junk[200];
      for(i=0;i<9;i++)
        f_in.getline(str_junk, 200);
      
      f_in >> num_points;
      
      OutputPointsContainerPointer outPoints = 
        m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points-1);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in >> newPoint[0] >> newPoint[1] >> newPoint[2];
        // AF: don't know why, but atet3D format contains (0,0,0)
        // vertex as the last vertex in the list. If saved in .vol format
        // with that unused vertex, it breaks the biofemir code
        // I skip it
        if(i==num_points-1)
          continue;
        this->GetOutput()->SetPoint(i, newPoint);
      }
      
      // Skip edges
      int num_edges;
      int junk;
      f_in >> num_edges;
      for(i=0;i<num_edges;i++)
        f_in >> junk >> junk;
      

      // Skip triangles
      int num_triangles;
      f_in >> num_triangles;
      for(i=0;i<num_triangles;i++)
        f_in >> junk >> junk >> junk;

      // Tetras
      f_in >> num_tetras;
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4];
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        if(pointIDs[0] == num_points-1){
          num_tetras = i;
          break;
        }
        // read neighbors
        int nei;
        f_in >> nei >> nei >> nei >> nei;
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }
      this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
    }
  
  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadSMugridMesh(){
      unsigned num_points, num_tetras, num_faces;
      unsigned i, junk;
      std::ifstream f_in(m_FileName.c_str());
      char str_junk[200];
      
      f_in >> num_points >> num_faces >> i >> num_tetras >> i >> i >> i;
      OutputPointsContainerPointer outPoints = 
      m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in >> newPoint[0] >> newPoint[1] >> newPoint[2];
        this->GetOutput()->SetPoint(i, newPoint);
      }
      for(i=0;i<num_faces;i++)
        f_in >> junk >> junk >> junk;
      for(i=0;i<num_faces;i++)
        f_in >> junk;
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4];
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        pointIDs[0]--;
        pointIDs[1]--;
        pointIDs[2]--;
        pointIDs[3]--;
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }
      this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
    }
  
  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadTGMesh(){
      unsigned num_points, num_tetras, num_faces;
      unsigned i, junk, pointDataCnt;
      typename TOutputMesh::PixelType pointData;
      
      std::string suffix, prefix;
      suffix = 
        m_FileName.substr(m_FileName.rfind(".")+1,m_FileName.size());
      prefix =
        m_FileName.substr(0, m_FileName.rfind("."));
      
      std::ifstream f_in_nodes, f_in_elems;

      if(suffix == "node"){
        f_in_nodes.open(m_FileName.c_str());
        f_in_elems.open((prefix+".ele").c_str());
      } else {
        f_in_nodes.open((m_FileName+".node").c_str());
        f_in_elems.open((m_FileName+".ele").c_str());
      }
      
      if(!f_in_nodes.is_open() || !f_in_elems.is_open())
        itkExceptionMacro(<<"Failed to find input files");

      char str_junk[200];
      
      // TODO: add handling of point/cell attributes
      f_in_nodes >> num_points >>  i >> pointDataCnt >> junk;
      if(i!=3)
        itkExceptionMacro(<<".poly data should be 3D");
     
      // Nodes
      OutputPointsContainerPointer outPoints = 
      m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in_nodes >> junk >> newPoint[0] >> newPoint[1] >> newPoint[2];
        this->GetOutput()->SetPoint(i, newPoint);
        if(pointDataCnt){
          f_in_nodes >> pointData;
          this->GetOutput()->SetPointData(i, pointData);
        }
      }

      // Elements
      f_in_elems >> num_tetras >> i >> junk;
      if(i!=4)
        itkExceptionMacro(<<"Expected to read tetrahedra as .poly elements");
      
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4];
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in_elems >> junk >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }
      this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
    }

  template<class TOutputMesh>
    void TetrahedralMeshReader<TOutputMesh>::
    ReadNGMesh(){
      unsigned num_points, num_tetras;
      unsigned i;

      std::ifstream f_in(m_FileName.c_str());
      std::string str_junk;
      f_in >> num_points;
      OutputPointsContainerPointer outPoints = 
        m_OutputMesh->GetPoints();
      outPoints->Reserve(num_points);
      outPoints->Squeeze();
      for(i=0;i<num_points;i++){
        typename OutputMeshType::PointType newPoint;
        f_in >> newPoint[0] >> newPoint[1] >> newPoint[2];
        this->GetOutput()->SetPoint(i, newPoint);
      }
      f_in >> num_tetras;
      for(i=0;i<num_tetras;i++){
        unsigned long pointIDs[4], junk;
        TetCellAutoPointer newTet;
        newTet.TakeOwnership(new TetCell);
        f_in >> junk >> pointIDs[0] >> pointIDs[1] >> pointIDs[2] >> pointIDs[3];
        pointIDs[0]--;
        pointIDs[1]--;
        pointIDs[2]--;
        pointIDs[3]--;
        newTet->SetPointIds(pointIDs);
        this->GetOutput()->SetCell(i, newTet);
        this->GetOutput()->SetCellData(i, 
          (typename OMeshTraits::CellPixelType) 0.0);
      }

      this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
    }

}

#endif // __itkTetrahedralMeshReader_txx
