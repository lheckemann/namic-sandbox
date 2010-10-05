
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkMeanSquaresImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkTranslationTransform.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkAffineTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkEuler3DTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkLabelObject.h"
#include "itkStatisticsLabelObject.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImageRegionConstIterator.h"

#include <iostream>

#include "ConvertToRigidAffine.h"
#include <sstream>

#include "ComputeAverageTransformCLP.h"

#define MI_BINS 32

typedef itk::TransformFileReader TransformReaderType;

typedef itk::Euler3DTransform<double> EulerTransformType;
typedef itk::VersorRigid3DTransform<double> VersorTransformType;

int main( int argc, char * argv[] )
{
  VersorTransformType::InputPointType center;

  // read both transforms
  // convert to euler3d transforms
  // compute difference in translation and rotation
  // report the difference 
  for(int i=2;i<argc;i++){
  TransformReaderType::Pointer tfmReader1 = TransformReaderType::New();
  tfmReader1->SetFileName( argv[1] );
  TransformReaderType::Pointer tfmReader2 = TransformReaderType::New();
  tfmReader2->SetFileName( argv[i] );

  try 
    {
    tfmReader1->Update();
    tfmReader2->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    return -1;
    }

  if(!dynamic_cast<VersorTransformType*>(tfmReader1->GetTransformList()->front().GetPointer()) ||
    !dynamic_cast<VersorTransformType*>(tfmReader2->GetTransformList()->front().GetPointer())){
    std::cerr << "Input transform is not VersorRigid3DTransform!" << std::endl;
    return -1;
  }

  const VersorTransformType::ConstPointer inputTransform1 = 
    dynamic_cast<VersorTransformType const *const>(tfmReader1->GetTransformList()->front().GetPointer());
  const VersorTransformType::ConstPointer inputTransform2 = 
    dynamic_cast<VersorTransformType const *const>(tfmReader2->GetTransformList()->front().GetPointer());

  center = inputTransform1->GetCenter();
  EulerTransformType::OutputVectorType translation;

  EulerTransformType::Pointer eulerTransform1 = EulerTransformType::New();
  VersorTransformType::ParametersType inputParameters = 
    inputTransform1->GetParameters();
  translation[0] = inputParameters[3];
  translation[1] = inputParameters[4];
  translation[2] = inputParameters[5];
  eulerTransform1->SetMatrix(inputTransform1->GetMatrix());
  eulerTransform1->SetTranslation(translation);
  eulerTransform1->SetCenter(center);

  EulerTransformType::Pointer eulerTransform2 = EulerTransformType::New();
  inputParameters = inputTransform2->GetParameters();
  eulerTransform2->SetMatrix(inputTransform2->GetMatrix());
  eulerTransform2->SetTranslation(translation);
  eulerTransform2->SetCenter(center);

  std::cout << "Translation: " << translation[0]-inputParameters[3] << ", " <<      
    translation[1]-inputParameters[4] << ", " << 
    translation[2]-inputParameters[5] << ", " << 
    (eulerTransform1->GetAngleX()-eulerTransform2->GetAngleX())*(180./vnl_math::pi) << ", " << 
    (eulerTransform1->GetAngleY()-eulerTransform2->GetAngleY())*(180./vnl_math::pi) << ", " <<
    (eulerTransform1->GetAngleZ()-eulerTransform2->GetAngleZ())*(180./vnl_math::pi) << std::endl;
  }

  return EXIT_SUCCESS;
}
