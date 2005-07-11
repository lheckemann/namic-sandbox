/* 
 * Really simple and basic writer for INRIA unstructured grids. Written
 * primarily so that the output mesh of the 3D mesher can be saved. Design
 * suggested by Luis Ibanez, implemented by Andriy Fedorov.
 */

#ifndef __itkINRIAUnstructuredGridMeshWriter_h
#define __itkINRIAUnstructuredGridMeshWriter_h

#include "itkLightProcessObject.h"
#include <stding>
#include <ofstream>

namespace itk{

template <class TInputMesh>
  class INRIAUnstructuredGridMeshWriter : public LightProcessObject 
                                        // TODO: (by Kitware :)) should inherit this from some generic MeshFileWriter
{
public:
  typedef INRIAUnstructuredGridMeshWriter Self;
  typedef SmartPointer<Self> Pointer;
  typedef InputMeshType TInputMesh;

  itkNewMacro(Self);
  
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);
 
  void SetInput(const InputMeshType * inputMesh);
  void Update();
  
protected:
  std::string m_FileName;
  INRIAUnstructuredGridMeshWriter();
  ~INRIAUnstructuredGridMeshWriter();

private:
  std::ofstream m_OutputFileStream;
  
}
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkINRIAUnstructuredGridWriter.txx"
#endif

#endif
