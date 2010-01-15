#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkVersorRigid3DTransform.h"
#include "itkMatrixOffsetTransformBase.h"

int main(int argc, char **argv){
  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " input.tfm output.tfm" << std::endl;
    return -1;
  }

  typedef itk::TransformFileReader TfmReaderType;
  typedef itk::TransformFileWriter TfmWriterType;

  TfmReaderType::Pointer tfmR = TfmReaderType::New();
  tfmR->SetFileName(argv[1]);
  TfmWriterType::Pointer tfmW = TfmWriterType::New();
  tfmW->SetFileName(argv[2]);

  tfmR->Update();
  
  TfmReaderType::TransformType::Pointer in =
    *(tfmR->GetTransformList()->begin());  

  typedef itk::MatrixOffsetTransformBase<double,3,3> DoubleMatrixOffsetType;  
  DoubleMatrixOffsetType::Pointer da
      = dynamic_cast<DoubleMatrixOffsetType*>(in.GetPointer());

  vnl_svd<double> svd(da->GetMatrix().GetVnlMatrix());

  typedef itk::VersorRigid3DTransform<double> TfmType;
  TfmType::Pointer out = TfmType::New();
  out->SetMatrix( svd.U() * vnl_transpose(svd.V()) );
  out->SetOffset( da->GetOffset() );

  tfmW->SetInput(out);
  tfmW->Update();

  return 0;  
}
