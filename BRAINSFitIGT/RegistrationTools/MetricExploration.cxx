
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

#include <MetricExplorationCLP.h>
#include <iostream>

#include "ConvertToRigidAffine.h"
#include <sstream>

#define MI_BINS 32


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

// Software Guide : BeginLatex
//
// We define the dimension and pixel type of the images to be used in the
// evaluation of the Metric.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  typedef itk::OrientedImage< float, 3 >   ImageType;
  typedef itk::OrientedImage< unsigned char, 3 >   MaskImageType;
// Software Guide : EndCodeSnippet

  typedef itk::ImageFileReader< ImageType >  ReaderType;
  typedef itk::ImageFileReader< MaskImageType >  MaskReaderType;
  typedef itk::TransformFileReader TransformReaderType;
  typedef itk::TransformFileWriter TransformWriterType;

  typedef itk::AffineTransform<double> TransformType;
  typedef itk::VersorRigid3DTransform<double> VersorTransformType;
  typedef itk::MattesMutualInformationImageToImageMetric< 
                            ImageType, ImageType >  MetricType;

  typedef itk::ImageRegionConstIterator<MaskImageType> ConstIteratorType;

  ReaderType::Pointer fixedReader  = ReaderType::New();
  ReaderType::Pointer movingReader = ReaderType::New();
  TransformReaderType::Pointer tfmReader = TransformReaderType::New();

  fixedReader->SetFileName(  FixedVolumeName.c_str() );
  movingReader->SetFileName( MovingVolumeName.c_str() );
  tfmReader->SetFileName( InitialTransformName.c_str() );

  try 
    {
    fixedReader->Update();
    movingReader->Update();
    tfmReader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }

  // input images
  ImageType::ConstPointer fixedImage  = fixedReader->GetOutput();
  ImageType::ConstPointer movingImage = movingReader->GetOutput();

  // initialize the transform
  typedef itk::StatisticsLabelObject< unsigned char, 3 > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType >               LabelMapType;
  typedef itk::LabelImageToStatisticsLabelMapFilter< MaskImageType, MaskImageType >
    LabelStatisticsFilterType;
  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;

  TransformType::Pointer transform = TransformType::New();
  if(!dynamic_cast<VersorTransformType*>(tfmReader->GetTransformList()->front().GetPointer())){
    std::cerr << "Input transform is not VersorRigid3DTransform!" << std::endl;
    return -1;
  } else {
    const VersorTransformType::ConstPointer versorTransform = 
      dynamic_cast<VersorTransformType const *const>(tfmReader->GetTransformList()->front().GetPointer());
    VersorTransformType::VersorType versor = versorTransform->GetVersor();
    std::cout << "Angle: " << versor.GetAngle()*(180./vnl_math::pi) << std::endl;
    std::cout << "Axis: " << versor.GetAxis() << std::endl;
    AssignRigid::AssignConvertedTransform(transform, versorTransform);
    std::cout << "Center: " << transform->GetCenter() << std::endl;
  }

  // initialize the interpolator
  typedef itk::LinearInterpolateImageFunction< 
                                 ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  // initialize the metric
  MetricType::Pointer metric = MetricType::New();
  metric->SetTransform( transform );
  metric->SetInterpolator( interpolator );

  metric->SetFixedImage(  fixedImage  );
  metric->SetMovingImage( movingImage );

  metric->SetFixedImageRegion(  fixedImage->GetBufferedRegion()  );
  std::cout << "Original number of spatial samples: " << metric->GetNumberOfSpatialSamples() 
    << std::endl;

  // MI-specific parameters
  metric->SetNumberOfHistogramBins(MI_BINS);
  //metric->UseAllPixelsOn();

  if(FixedMaskName.size() && MovingMaskName.size()){
    
    MaskImageType::Pointer fixedMaskImage = NULL;
    MaskImageType::Pointer movingMaskImage = NULL;

    MaskReaderType::Pointer rm1 = MaskReaderType::New();
    rm1->SetFileName( FixedMaskName.c_str() );
    rm1->Update();
    fixedMaskImage = rm1->GetOutput();

    MaskReaderType::Pointer rm2 = MaskReaderType::New();
    rm2->SetFileName(MovingMaskName.c_str());
    rm2->Update();
    movingMaskImage = rm2->GetOutput();
 
//    metric->SetFixedImageMask(fixedMask);
//    metric->SetMovingImageMask(movingMask);
    std::cout << "Image masks have been initialized" << std::endl;
    
    ConstIteratorType it1(fixedMaskImage, fixedMaskImage->GetBufferedRegion());
    ConstIteratorType it2(movingMaskImage, movingMaskImage->GetBufferedRegion());

    float fixedSamplesNum = 0, movingSamplesNum = 0;

    for(it1.GoToBegin();!it1.IsAtEnd();++it1)
      if(it1.Get())
        fixedSamplesNum++;

    for(it2.GoToBegin();!it2.IsAtEnd();++it2)
      if(it2.Get())
        movingSamplesNum++;

    std::cout << "Fixed samples: " << fixedSamplesNum << ", Moving samples: " << movingSamplesNum << std::endl;

    typedef itk::ImageMaskSpatialObject<3> MaskObject;
    MaskObject::Pointer fixedMaskObject = MaskObject::New();
    MaskObject::Pointer movingMaskObject = MaskObject::New();
    fixedMaskObject->SetImage(fixedMaskImage);
    movingMaskObject->SetImage(movingMaskImage);
    fixedMaskObject->ComputeObjectToWorldTransform();
    movingMaskObject->ComputeObjectToWorldTransform();
    metric->SetFixedImageMask(dynamic_cast<MetricType::FixedImageMaskType*>(fixedMaskObject.GetPointer()));
    metric->SetMovingImageMask(dynamic_cast<MetricType::MovingImageMaskType*>(movingMaskObject.GetPointer()));

    metric->SetNumberOfSpatialSamples(fixedSamplesNum);

    /*
    MaskImageType::Pointer fixedMaskImage = rm1->GetOutput();
    MaskImageType::Pointer movingMaskImage = rm2->GetOutput();

    typedef itk::ImageMaskSpatialObject<3> MaskObject;
    MaskObject::Pointer fixedMaskObject = MaskObject::New();
    MaskObject::Pointer movingMaskObject = MaskObject::New();
    fixedMaskObject->SetImage(fixedMaskImage);
    movingMaskObject->SetImage(movingMaskImage);
    fixedMaskObject->ComputeObjectToWorldTransform();
    movingMaskObject->ComputeObjectToWorldTransform();
    fixedMask = dynamic_cast<MetricType::FixedImageMaskType*>(fixedMaskObject.GetPointer());
    movingMask = dynamic_cast<MetricType::MovingImageMaskType*>(movingMaskObject.GetPointer());
    ImageType::PointType p;
    p[0] = 41;
    p[1] = 25;
    p[2] = -18;
    std::cout << fixedMask->IsInside(p) << " , " << movingMask->IsInside(p) << std::endl;
    */

  }

//  metric->SetUseFixedImageSamplesIntensityThreshold(true);
//  metric->SetFixedImageSamplesIntensityThreshold(10);

  std::cout << "Before initialize metric" << std::endl;
  try 
    {
    metric->Initialize();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Done" << std::endl;

  MetricType::TransformParametersType parameters;
  parameters = transform->GetParameters();
  std::cout << "Translation: " << parameters[9] << ", " << parameters[10] <<
    ", " << parameters[11] << std::endl;
  std::cout << "Center: " << transform->GetCenter() << std::endl;

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
    
  return EXIT_SUCCESS;
}


