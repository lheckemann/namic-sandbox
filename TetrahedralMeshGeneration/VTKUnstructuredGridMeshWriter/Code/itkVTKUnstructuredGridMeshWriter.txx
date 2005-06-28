#ifndef __itkVTKUnstructuredGridMeshWriter_txx
#define __itkVTKUnstructuredGridMeshWriter_txx

namespace itk
{
  template <class TInputMesh>
    VTKUnstructuredGridMeshWriter
    ::VTKUnstructuredMeshWriter():
    m_FileName(""){
    }

  template <class TImputMesh>
    VTKUnstructuredGridMeshWriter
    ::~VTKUnstructuredMeshWriter(){
    }

  template <class TInputMesh> void
    VTKUnstructuredGridMeshWriter
    ::Update(){
      m_OutputFileStream.open(m_FileName.c_str());
      assert(m_OutputFileStream.is_open());
      
      m_OutputFileStream.close();
    }

  template <class TInputMesh> void
    VTKUnstructuredGridMeshWriter
    ::SetInput(const InputMeshType *input){
      this->ProcessObject::SetNthInput(0,
        const_cast<TInputMesh *>(input));
    }

}

#endif
