
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
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkLabelObject.h"
#include "itkStatisticsLabelObject.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImageRegionConstIterator.h"

#include <GeneratePerturbedTransformsCLP.h>
#include <iostream>

#include "ConvertToRigidAffine.h"
#include <sstream>

#define MI_BINS 32

typedef itk::OrientedImage< float, 3 >   ImageType;
typedef itk::OrientedImage< unsigned char, 3 >   MaskImageType;

typedef itk::TransformFileReader TransformReaderType;
typedef itk::TransformFileWriter TransformWriterType;

typedef itk::AffineTransform<double> AffineTransformType;
typedef itk::VersorRigid3DTransform<double> VersorTransformType;
typedef itk::MattesMutualInformationImageToImageMetric< 
                          ImageType, ImageType >  MetricType;

typedef itk::ImageRegionConstIterator<MaskImageType> ConstIteratorType;

void SaveTransform(VersorTransformType::Pointer, std::string);


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  TransformReaderType::Pointer tfmReader = TransformReaderType::New();

  tfmReader->SetFileName( InitialTransformName.c_str() );

  try 
    {
    tfmReader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }

  // initialize the transform
//  typedef itk::StatisticsLabelObject< unsigned char, 3 > LabelObjectType;
//  typedef itk::LabelMap< LabelObjectType >               LabelMapType;
//  typedef itk::LabelImageToStatisticsLabelMapFilter< MaskImageType, MaskImageType >
//    LabelStatisticsFilterType;
//  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;

  if(!dynamic_cast<VersorTransformType*>(tfmReader->GetTransformList()->front().GetPointer())){
    std::cerr << "Input transform is not VersorRigid3DTransform!" << std::endl;
    return -1;
  }

  const VersorTransformType::ConstPointer inputTransform = 
    dynamic_cast<VersorTransformType const *const>(tfmReader->GetTransformList()->front().GetPointer());
  VersorTransformType::VersorType inputVersor = inputTransform->GetVersor();
  std::cout << "Input versor: " << inputVersor << std::endl;

  itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(123);  
  double X, Y, Z, S; // versor axis sampling
  double Tx, Ty, Tz; // translation variation

  for(int i=0;i<100;i++){
    VersorTransformType::VersorType newVersor;
    VersorTransformType::Pointer newVersorTransform = VersorTransformType::New();
//    newVersorTransform->SetParameters(inputTransform->GetParameters());

    X = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-1.,1.);
    Y = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-1.,1.);

    S = X*X+Y*Y;

    if(S>1.){
        i--;
        continue;
    }

    Z = 2.*S-1.;
    S = sqrt((1.-Z*Z)/S);
    X = X*S;
    Y = Y*S;
    VersorTransformType::VersorType::VectorType newAxis;
    newAxis[0] = X;
    newAxis[1] = Y;
    newAxis[2] = Z;

//    std::cout << "axis: " << newAxis << std::endl;
    if(newAxis*inputVersor.GetAxis() < cos(vnl_math::pi/90.)){
        i--;
        continue;
    }
    
    Tx = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-3.,3.);
    Ty = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-3.,3.);
    Tz = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-3.,3.);

    // get the translation component
    VersorTransformType::ParametersType inputParameters = inputTransform->GetParameters();
    VersorTransformType::ParametersType newParameters(6);

    // versor->GetX() is NOT versor->GetAxis()[0] !!!
    newVersor.Set(newAxis, inputVersor.GetAngle());
    newParameters[0] = newVersor.GetX();
    newParameters[1] = newVersor.GetY();
    newParameters[2] = newVersor.GetZ();
    newParameters[3] = inputParameters[3]+Tx;
    newParameters[4] = inputParameters[4]+Ty;
    newParameters[5] = inputParameters[5]+Tz;

    newVersorTransform->SetParameters(newParameters);
    newVersorTransform->SetCenter(inputTransform->GetCenter());
  
    std::ostringstream sstr;
    sstr << "RandomTransform" << i << ".tfm";
    SaveTransform(newVersorTransform, sstr.str().c_str());
  }

  /*
  double initTranslation[3];
  initTranslation[0] = parameters[9];
  initTranslation[1] = parameters[10];
  initTranslation[2] = parameters[11];

  std::ofstream resultStream(ResultFilename.c_str());

  std::cout << "Range to be explored: " << ExplorationRange1[0] << " to " << ExplorationRange1[1] << std::endl;
  std::cout << "Range to be explored: " << ExplorationRange2[0] << " to " << ExplorationRange2[1] << std::endl;
  std::cout << "Increments: " << ExplorationIncrement1 << ", " << ExplorationIncrement2 << std::endl;
  std::cout << "Parameter: " << Parameter << ", Fixed DOF: " << FixedDOF << std::endl;

  TransformType::Pointer tmpTransform = TransformType::New();
  MetricType::TransformParametersType tmpParameters = tmpTransform->GetParameters();      
 
  for( float d1 = ExplorationRange1[0]; d1 <= ExplorationRange1[1]; d1 += ExplorationIncrement1 ) {
    for( float d2 = ExplorationRange2[0]; d2 <= ExplorationRange2[1]; d2 += ExplorationIncrement2 ) {

      for(int i=0;i<12;i++)
        tmpParameters[i] = parameters[i];
      tmpTransform->SetIdentity();
      tmpTransform->SetCenter(transform->GetCenter());      
      tmpTransform->SetParameters(tmpParameters); // initially the same parameters

      if(Parameter == "translation"){
        
        switch(FixedDOF){
          case 1: tmpParameters[10] = parameters[10]+d1; 
                  tmpParameters[11] = parameters[11]+d2; 
                  break;
          case 2: tmpParameters[9] = parameters[9]+d1; 
                  tmpParameters[11] = parameters[11]+d2; 
                  break;
          case 3: tmpParameters[9] = parameters[9]+d1; 
                  tmpParameters[10] = parameters[10]+d2; 
                  break;
          default: return -1;
        }
        tmpTransform->SetParameters(tmpParameters);
      } else if(Parameter == "rotation"){
        float a1 = d1*vnl_math::pi/180.;
        float a2 = d2*vnl_math::pi/180.;

        //a1 = 0; a2 = 0;
        switch(FixedDOF){
          case 1: if(a1) tmpTransform->Rotate(1, 2, a1); break;
          case 2: if(a1) tmpTransform->Rotate(0, 2, a1); break;
          case 3: if(a1) tmpTransform->Rotate(0, 1, a1); break;
          default: return -1;
        }
        std::cout << "Rotated by " << d1 << " / " << d2 << " degrees, " << a1 << " / " << a2 << " radians" << std::endl;
      } else {
        return -1;
      }
      tmpParameters = tmpTransform->GetParameters();
      double metricValue = 0;
      try{
        std::ostringstream names;
        names << "transform_" << d1 << "_" << d2 << ".tfm";
        if(OutputTransformName != ""){
          itk::TransformFileWriter::Pointer tfmWriter = itk::TransformFileWriter::New();
          tfmWriter->SetInput(tmpTransform);
          tfmWriter->SetFileName(names.str().c_str());
          tfmWriter->Update();
        }

        metricValue = fabs(metric->GetValue( tmpParameters ));
      } catch(itk::ExceptionObject& e){
        std::cout << "Exception caught while computing the metric: " << e << std::endl;
        if(OutputTransformName != ""){
          itk::TransformFileWriter::Pointer tfmWriter = itk::TransformFileWriter::New();
          tfmWriter->SetInput(tmpTransform);
          tfmWriter->SetFileName(OutputTransformName.c_str());
          tfmWriter->Update();
        }
        return -1;
      }
      resultStream << d1 << " " << d2 << " " << metricValue << std::endl;
      std::cout << d1 << " " << d2 << " " << metricValue << std::endl;
    }
  }
  */
  return EXIT_SUCCESS;
}

void SaveTransform(VersorTransformType::Pointer tfm, std::string name){
  TransformWriterType::Pointer tfmw = TransformWriterType::New();
  tfmw->SetInput(tfm);
  tfmw->SetFileName(name);
  tfmw->Update();
}
