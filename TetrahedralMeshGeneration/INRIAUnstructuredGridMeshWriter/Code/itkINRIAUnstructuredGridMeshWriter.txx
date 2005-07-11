#ifndef __itkINRIAUnstructuredGridMeshWriter_txx
#define __itkINRIAUnstructuredGridMeshWriter_txx

namespace itk
{
  template <class TInputMesh>
    INRIAUnstructuredGridMeshWriter
    ::INRIAUnstructuredMeshWriter():
    m_FileName(""){
    }

  template <class TImputMesh>
    INRIAUnstructuredGridMeshWriter
    ::~INRIAUnstructuredMeshWriter(){
    }

  template <class TInputMesh> void
    INRIAUnstructuredGridMeshWriter
    ::Update(){
      m_OutputFileStream.open(m_FileName.c_str());
      assert(m_OutputFileStream.is_open());
      
      m_OutputFileStream.close();
    }

  template <class TInputMesh> void
    INRIAUnstructuredGridMeshWriter
    ::SetInput(const InputMeshType *input){
      this->ProcessObject::SetNthInput(0,
        const_cast<TInputMesh *>(input));
    }

}

#endif
