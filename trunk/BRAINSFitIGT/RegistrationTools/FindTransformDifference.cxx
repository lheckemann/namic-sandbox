
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

typedef itk::OrientedImage< float, 3 >   ImageType;
typedef itk::OrientedImage< unsigned char, 3 >   MaskImageType;

typedef itk::TransformFileReader TransformReaderType;
typedef itk::TransformFileWriter TransformWriterType;

typedef itk::AffineTransform<double> AffineTransformType;
typedef itk::Euler3DTransform<double> EulerTransformType;
typedef itk::VersorRigid3DTransform<double> VersorTransformType;
typedef itk::MattesMutualInformationImageToImageMetric< 
                          ImageType, ImageType >  MetricType;

typedef itk::ImageRegionConstIterator<MaskImageType> ConstIteratorType;

void SaveTransform(VersorTransformType::Pointer, std::string);
void SaveTransform(EulerTransformType::Pointer, std::string);


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  unsigned i = 0;
  double meanL = 0, meanP = 0, meanS = 0;
  double meanLa = 0, meanPa = 0, meanSa = 0;
  double stdL = 0, stdP = 0, stdS = 0;
  std::ostringstream fLname, fPname, fSname;
  std::ostringstream fLAname, fPAname, fSAname;

  VersorTransformType::InputPointType center;

  std::ofstream outputLog((OutputPrefix+".log").c_str());
  outputLog << "dL dP dS dLA dPA dSA" << std::endl;

  // read both transforms
  // convert to euler3d transforms
  // compute difference in translation and rotation
  // report the difference 
  while(true){
    std::ostringstream fname;
    fname << InputPrefix << i++ << InputSuffix;
    TransformReaderType::Pointer tfmReader = TransformReaderType::New();
    tfmReader->SetFileName( fname.str().c_str() );

    try 
      {
      tfmReader->Update();
      }
    catch( itk::ExceptionObject & excep )
      {
      break;
      }

    if(!dynamic_cast<VersorTransformType*>(tfmReader->GetTransformList()->front().GetPointer())){
      std::cerr << "Input transform is not VersorRigid3DTransform!" << std::endl;
      return -1;
    }

    const VersorTransformType::ConstPointer inputTransform = 
      dynamic_cast<VersorTransformType const *const>(tfmReader->GetTransformList()->front().GetPointer());
    VersorTransformType::VersorType inputVersor = inputTransform->GetVersor();

    center = inputTransform->GetCenter();

    EulerTransformType::Pointer eulerTransform = EulerTransformType::New();
    EulerTransformType::OutputVectorType translation;
    VersorTransformType::ParametersType inputParameters = 
      inputTransform->GetParameters();
    translation[0] = inputParameters[3];
    translation[1] = inputParameters[4];
    translation[2] = inputParameters[5];
    eulerTransform->SetMatrix(inputTransform->GetMatrix());
    eulerTransform->SetTranslation(translation);
    eulerTransform->SetCenter(inputTransform->GetCenter());
  


    double rotation[3];
    rotation[0] = eulerTransform->GetAngleX();
    rotation[1] = eulerTransform->GetAngleY();
    rotation[2] = eulerTransform->GetAngleZ();

//    std::cout << "AngleX: " << eulerTransform->GetAngleX()*(180./vnl_math::pi)<< std::endl;
//    std::cout << "AngleY: " << eulerTransform->GetAngleY()*(180./vnl_math::pi) << std::endl;
//    std::cout << "AngleZ: " << eulerTransform->GetAngleZ()*(180./vnl_math::pi) << std::endl;

    meanL += translation[0];
    meanP += translation[1];
    meanS += translation[2];

    outputLog << i << " " << translation[0] << " " << translation[1] << " " << translation[2] << " ";
    outputLog << rotation[0]*(180./vnl_math::pi)<< " " 
      << rotation[1]*(180./vnl_math::pi) << " " 
      << rotation[2]*(180./vnl_math::pi) << std::endl;

    meanLa += rotation[0];
    meanPa += rotation[1];
    meanSa += rotation[2];
  }

  EulerTransformType::Pointer eulerTransform = EulerTransformType::New();
  EulerTransformType::OutputVectorType translation;
  translation[0] = meanL/(double)i;
  translation[1] = meanP/(double)i;
  translation[2] = meanS/(double)i;
  eulerTransform->SetRotation(meanLa/(double)i,
    meanPa/(double)i, meanSa/(double)i);
  eulerTransform->SetTranslation(translation);
  eulerTransform->SetCenter(center);
  std::ostringstream aveTfmName;
  aveTfmName << OutputPrefix << "_average.tfm";

  VersorTransformType::Pointer newVersorTfm = VersorTransformType::New();
  newVersorTfm->SetMatrix(eulerTransform->GetMatrix());
  newVersorTfm->SetTranslation(translation);
  newVersorTfm->SetCenter(center);

  SaveTransform(newVersorTfm, aveTfmName.str().c_str() );

  i--;
  std::cout << "Total of " << i << " transformations analyzed" << std::endl;
  std::cout << "Mean translation: " << meanL/(double)i << ", " << meanP/(double)i << ", " << meanS/(double)i << std::endl;
  std::cout << "Mean rotation: " << (meanLa*(180./vnl_math::pi))/(double)i << ", " << (meanPa*(180./vnl_math::pi))/(double)i << ", " << (meanSa*(180./vnl_math::pi))/(double)i << std::endl;

  return EXIT_SUCCESS;
}

void SaveTransform(VersorTransformType::Pointer tfm, std::string name){
  TransformWriterType::Pointer tfmw = TransformWriterType::New();
  tfmw->SetInput(tfm);
  tfmw->SetFileName(name);
  tfmw->Update();
}

void SaveTransform(EulerTransformType::Pointer tfm, std::string name){
  TransformWriterType::Pointer tfmw = TransformWriterType::New();
  tfmw->SetInput(tfm);
  tfmw->SetFileName(name);
  tfmw->Update();
}
