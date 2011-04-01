/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2009-06-24 12:09:25 $
  Version:   $Revision: 1.39 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// This code is based on ITK Examples/Registration/ImageRegistration8.cxx

#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkTransformFileWriter.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageMaskSpatialObject.h"

#include "SliceRegistrationCLP.h"

#include "itkTimeProbesCollectorBase.h"
#include "itkPluginFilterWatcher.h"

#include "itkLBFGSBOptimizer.h"

#include "ConvertToRigidAffine.h"

/////////////////////////

const unsigned int    ImageDimension = 3;
typedef float           PixelType;
typedef itk::Vector<PixelType, ImageDimension> VectorPixelType;

typedef itk::Image< PixelType, ImageDimension >  FixedImageType;
typedef itk::Image< PixelType, ImageDimension >  MovingImageType;
typedef itk::Image< unsigned char, ImageDimension >  MaskImageType;

typedef itk::ResampleImageFilter<MovingImageType, FixedImageType>    ResampleFilterType;
typedef itk::VersorRigid3DTransform< double > TransformType;

////////////////////////

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::VersorRigid3DTransformOptimizer OptimizerType;
  typedef   const OptimizerType *              OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer = 
      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

class DefCommandIterationUpdate : public itk::Command 
{
public:
  typedef  DefCommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
  typedef itk::SingleValuedCostFunction CostFunctionType;
  void SetRegistration (itk::ProcessObject * obj)
    {
    m_Registration = obj; 
    }
  void SetCostFunction(CostFunctionType* fn)
    {
      m_CostFunction = fn;
    }
protected:
  DefCommandIterationUpdate() {};
  itk::ProcessObject::Pointer m_Registration;
  CostFunctionType::Pointer m_CostFunction;
public:
  typedef itk::LBFGSBOptimizer  OptimizerType;
  typedef OptimizerType   *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( const_cast<itk::Object *>(object) );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
        return;
        }
      
      if (m_Registration)
        {
        m_Registration->UpdateProgress( 
          static_cast<double>(optimizer->GetCurrentIteration()) /
          static_cast<double>(optimizer->GetMaximumNumberOfIterations()));
        }
    }
};

// Resample to the original coordinate frame (not the reoriented
// axial coordinate frame) of the fixed image
// Generate the explicit deformation field resulting from the registration.
template <typename transformType>
void WriteResampledImage(const char *resampledImageFilename, FixedImageType::Pointer fixedImage, MovingImageType::Pointer movingImage, transformType transform, ModuleProcessInformation *CLPProcessInformation)
{  
  ResampleFilterType::Pointer resample = ResampleFilterType::New();        

  const double DefaultPixelValue=0;
  resample->SetTransform        ( transform );
  
  resample->SetInput            ( movingImage );    
  resample->SetDefaultPixelValue( DefaultPixelValue );
  resample->SetOutputParametersFromImage ( fixedImage );

  itk::PluginFilterWatcher watcher(resample,"Resample",CLPProcessInformation,1.0/3.0, 2.0/3.0);
  resample->Update();

  typedef itk::ImageFileWriter< FixedImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( resampledImageFilename );
  writer->SetInput( resample->GetOutput()   );
  writer->SetUseCompression(0); // to allow loading into ParaView

  try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    exit(EXIT_FAILURE);
  }
}

// Generate the explicit deformation field resulting from the registration.
template <typename transformType>
void WriteFieldImage(const char *fieldImageFilename, FixedImageType::Pointer fixedImage, transformType transform)
{  
  typedef itk::Image<VectorPixelType,  ImageDimension> DeformationFieldType; 

  DeformationFieldType::Pointer deformationField = DeformationFieldType::New();
  deformationField->SetRegions( fixedImage->GetBufferedRegion() );
  deformationField->SetOrigin( fixedImage->GetOrigin() );
  deformationField->SetSpacing( fixedImage->GetSpacing() );
  deformationField->SetDirection( fixedImage->GetDirection() );
  deformationField->Allocate();

  typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
  FieldIterator fi( deformationField, fixedImage->GetBufferedRegion() );

  fi.GoToBegin();

  TransformType::InputPointType  fixedPoint;
  TransformType::OutputPointType movingPoint;
  DeformationFieldType::IndexType index;

  VectorPixelType displacement;

  while( ! fi.IsAtEnd() )
  {
    index = fi.GetIndex();
    deformationField->TransformIndexToPhysicalPoint( index, fixedPoint );
    movingPoint = transform->TransformPoint( fixedPoint );
    displacement = movingPoint - fixedPoint;
    fi.Set( displacement );
    ++fi;
  }

  // Write to file
  typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
  FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
  fieldWriter->SetInput( deformationField );
  fieldWriter->SetFileName( fieldImageFilename );
  try
  {
    fieldWriter->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    exit(EXIT_FAILURE);
  }
}



// Generate the explicit deformation field resulting from the registration.
enum MetricTestEnum
{
  TRANSLATION,
  ROTATION
};
template <typename metricType>
void WriteMetricTestResult(const char *outputFilename, MetricTestEnum testType, metricType metric, TransformType::Pointer inputTransform, std::vector<double> &rangeExtent, std::vector<int> &rangeSize)
{
  const unsigned int MetricOutputImageDimension = 3;
  typedef float MetricOutputImagePixelType;
  typedef itk::Image<MetricOutputImagePixelType, MetricOutputImageDimension> MetricOutputImageType;

  typedef itk::AffineTransform<double> MetricTestTransformType;
  MetricTestTransformType::Pointer transform = MetricTestTransformType::New();
  const TransformType::ConstPointer constInputTransform = dynamic_cast<TransformType const *const>(inputTransform.GetPointer());
  AssignRigid::AssignConvertedTransform(transform, constInputTransform);
  std::cout << "Center: " << transform->GetCenter() << std::endl;

  // initialize the interpolator
  metric->SetTransform( transform );
  std::cout << "Before initialize metric" << std::endl;
  try 
  {
    metric->Initialize();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    exit (EXIT_FAILURE);
  }
  std::cout << "Done" << std::endl;

  MetricTestTransformType::ParametersType parameters;
  parameters = transform->GetParameters();
  std::cout << "Translation: " << parameters[9] << ", " << parameters[10] << ", " << parameters[11] << std::endl;
  std::cout << "Center: " << transform->GetCenter() << std::endl;

/*  double transformRange[2]={-maxRange, maxRange};
  double transformIncrement=(transformRange[1]-transformRange[0])/(numberOfSamples-1);

  if (testType==TRANSLATION)
  {
    std::cout << "Translation range: " << transformRange[0] << " to " << transformRange[1] << " mm, increment " << transformIncrement << " mm" << std::endl;
  } 
  else
  {
    std::cout << "Rotation range: " << transformRange[0] << " to " << transformRange[1] << " deg, increment " << transformIncrement << " deg" << std::endl;
  }
  std::cout << "Number of samples along each dimension: " << numberOfSamples << std::endl;
*/

  // store the output of the metric test in 3D volumes (one for translation, one for rotation)
  MetricOutputImageType::Pointer outputImage = MetricOutputImageType::New();  
  MetricOutputImageType::RegionType outputImageRegion;
  outputImageRegion.SetIndex(0,0);
  outputImageRegion.SetIndex(1,0);
  outputImageRegion.SetIndex(2,0);
  MetricOutputImageType::SizeType outputImageSize;
  outputImageSize[0]=rangeSize[0];
  outputImageSize[1]=rangeSize[1];
  outputImageSize[2]=rangeSize[2];
  outputImageRegion.SetSize(outputImageSize);
  outputImage->SetLargestPossibleRegion(outputImageRegion);
  outputImage->SetBufferedRegion(outputImageRegion);
  MetricOutputImageType::PointType outputImageOrigin;
  outputImageOrigin[0]=rangeExtent[0];
  outputImageOrigin[1]=rangeExtent[2];
  outputImageOrigin[2]=rangeExtent[4];  
  outputImage->SetOrigin(outputImageOrigin);
  MetricOutputImageType::SpacingType outputImageSpacing;
  outputImageSpacing[0]=1;
  if (rangeSize[0]>1)
  {
    outputImageSpacing[0]=(rangeExtent[1]-rangeExtent[0])/(rangeSize[0]-1);
  }
  outputImageSpacing[1]=1;
  if (rangeSize[1]>1)
  {
    outputImageSpacing[1]=(rangeExtent[3]-rangeExtent[2])/(rangeSize[1]-1);
  }
  outputImageSpacing[2]=1;
  if (rangeSize[2]>1)
  {
    outputImageSpacing[2]=(rangeExtent[5]-rangeExtent[4])/(rangeSize[2]-1);
  }
  outputImage->SetSpacing(outputImageSpacing);
  outputImage->Allocate();

  MetricTestTransformType::Pointer tmpTransform = MetricTestTransformType::New();
  MetricTestTransformType::ParametersType tmpParameters = tmpTransform->GetParameters();       
  double d1=0;
  double d2=0;
  double d3=0;

  typedef itk::ImageRegionIterator< MetricOutputImageType > MetricOutputImageIterator;

  MetricOutputImageIterator outputImageIt( outputImage, outputImageRegion );
  outputImageIt.GoToBegin();
  MetricOutputImageType::IndexType tindex;
  MetricOutputImageType::PointType tpoint;  
  while( ! outputImageIt.IsAtEnd() )
  {
    tindex = outputImageIt.GetIndex();
    outputImage->TransformIndexToPhysicalPoint( tindex, tpoint );

    d1=tpoint[0];
    d2=tpoint[1];
    d3=tpoint[2];

    for(int i=0;i<12;i++)
    {
      tmpParameters[i] = parameters[i];
    }
    tmpTransform->SetIdentity();
    tmpTransform->SetCenter(transform->GetCenter());      
    tmpTransform->SetParameters(tmpParameters); // initially the same parameters

    if (testType==TRANSLATION)
    {
      tmpParameters[9] = parameters[9]+d1; 
      tmpParameters[10] = parameters[10]+d2; 
      tmpParameters[11] = parameters[11]+d3;  
      tmpTransform->SetParameters(tmpParameters);
    }
    else
    {
      float a1 = d1*vnl_math::pi/180.0;
      float a2 = d2*vnl_math::pi/180.0;
      float a3 = d3*vnl_math::pi/180.0;
      tmpTransform->Rotate(1, 2, a1);
      tmpTransform->Rotate(0, 2, a2);
      tmpTransform->Rotate(0, 1, a3);
    }

    tmpParameters = tmpTransform->GetParameters();       
    double metricValue = 0;
    try
    {      
      metricValue = -metric->GetValue( tmpParameters ); // invert it to make the optimum appear as a peak (it looks better when visualized as surface)
    } 
    catch(itk::ExceptionObject& e)
    {
      std::cout << "Exception caught while computing the metric: " << e << std::endl;
      exit(EXIT_FAILURE);
    }
    outputImageIt.Set(metricValue);
    
    if (testType==TRANSLATION)
    {
      std::cout << "translation " << d1 << " " << d2 << " " << d3 << " " << metricValue << std::endl;
    }
    else
    {
      std::cout << "rotation " << d1 << " " << d2 << " " << d3 << " " << metricValue << std::endl;
    }

    ++outputImageIt;
  }  

  typedef itk::ImageFileWriter< MetricOutputImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( outputFilename );
  writer->SetInput( outputImage );
  writer->SetUseCompression(0); // to allow loading into ParaView

  try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    exit(EXIT_FAILURE);
  }

}






///////////////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
  PARSE_ARGS;

  // Add a time probe
  itk::TimeProbesCollectorBase collector;




  typedef itk::VersorRigid3DTransformOptimizer           OptimizerType;

  typedef itk::ImageRegionConstIteratorWithIndex<FixedImageType> IterType;

//  typedef itk::MeanSquaresImageToImageMetric< 
//                                    FixedImageType, 
//                                    MovingImageType >    MetricType;

//  typedef itk::NormalizedCorrelationImageToImageMetric<
//                                    FixedImageType,
//                                    MovingImageType > MetricType;

  typedef itk::MattesMutualInformationImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType > MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;
  typedef itk:: NearestNeighborInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    NNInterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  typedef itk::DiscreteGaussianImageFilter<
                                    MovingImageType, MovingImageType > BlurType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  
  metric->SetNumberOfHistogramBins(32);

  std::cout << "Number of spatial samples: " << metric->GetNumberOfSpatialSamples() << std::endl;

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );


  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  MaskReaderType::Pointer maskImageReader = MaskReaderType::New();

  collector.Start( "Read input images" );

  fixedImageReader->SetFileName( SliceImageFilename.c_str() );
  try
  {
    fixedImageReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  } 

  movingImageReader->SetFileName( VolumeImageFilename.c_str() );
  try
  {
    movingImageReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  } 

  if (!VolumeMaskImageFilename.empty())
  {
    std::cout << "Mask is used" << std::endl;
    maskImageReader->SetFileName( VolumeMaskImageFilename.c_str() );
    try
    {
      maskImageReader->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
    typedef itk::ImageMaskSpatialObject<3> MaskObjectType;
    MaskObjectType::Pointer maskObject = MaskObjectType::New();
    maskObject->SetImage(maskImageReader->GetOutput());
    metric->SetFixedImageMask(maskObject);
  }
  else
  {
    std::cout << "Mask is not used" << std::endl;
  }
  
  collector.Stop( "Read input images" );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );

  const double blurVariance=0.0; // it worked robustly with 2.0

  std::cout << "Blur variance: " << blurVariance << std::endl;

  BlurType::Pointer blur = BlurType::New();
  blur->SetInput(movingImageReader->GetOutput());
  blur->SetVariance(blurVariance);
  blur->Update();

  MovingImageType::Pointer movingImage = blur->GetOutput();
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  registration->SetMovingImage(   blur->GetOutput()   );

  registration->SetFixedImageRegion( fixedImageReader->GetOutput()->GetBufferedRegion() );
 
  transform->SetIdentity();
  registration->SetInitialTransformParameters( transform->GetParameters() );

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  double translationScale[3];// = 1.0 / 1000.0;
  translationScale[0] = 1. / 2000.;//(10.*fixedImage->GetSpacing()[0]*fixedImage->GetBufferedRegion().GetSize()[0]);
  translationScale[1] = 1. / 2000.;//(10.*fixedImage->GetSpacing()[1]*fixedImage->GetBufferedRegion().GetSize()[1]);
  translationScale[2] = 1. / 2000.;//(10.*fixedImage->GetSpacing()[2]*fixedImage->GetBufferedRegion().GetSize()[2]);


  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale[0];
//    1./(10.*fixedImage->GetSpacing()[0]*fixedImage->GetBufferedRegion().GetSize()[0]);
  optimizerScales[4] = translationScale[1];
//    1./(10.*fixedImage->GetSpacing()[1]*fixedImage->GetBufferedRegion().GetSize()[1]);
  optimizerScales[5] = translationScale[2];
//    1./(.1*fixedImage->GetSpacing()[2]*fixedImage->GetBufferedRegion().GetSize()[2]);
//
  std::cout << "Optimizer scales: " << optimizerScales << std::endl;    
  std::cout << "Relaxation factor: " << optimizer->GetRelaxationFactor() << std::endl;

  optimizer->SetScales( optimizerScales );

  optimizer->SetMaximumStepLength( 0.2000  ); 
  optimizer->SetMinimumStepLength( 0.0001 );

  optimizer->SetNumberOfIterations( 400 );


  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );


  try 
    { 
    collector.Start( "Rigid registration" );
    registration->Update(); 
    collector.Stop( "Rigid registration" );
    
    std::cout << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  
  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();


  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  std::cout << std::endl << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " versor X      = " << versorX  << std::endl;
  std::cout << " versor Y      = " << versorY  << std::endl;
  std::cout << " versor Z      = " << versorZ  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  transform->SetParameters( finalParameters );

  TransformType::MatrixType matrix = transform->GetRotationMatrix();
  TransformType::OffsetType offset = transform->GetOffset();

  if (RigidTransformFilename != "")
  {
    typedef itk::TransformFileWriter TransformWriter;
    TransformWriter::Pointer tfmWriter = TransformWriter::New();
    tfmWriter->SetInput(transform);
    tfmWriter->SetFileName( RigidTransformFilename.c_str() );
    try
    {
      tfmWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Write deformed image
  if (ResampledRigidMovingImageFilename != "")
  {
    // movingImage it was blurred, so read it again to get the oriignal
    collector.Start( "Resample deformed moving image" );
    WriteResampledImage(ResampledRigidMovingImageFilename.c_str(), fixedImage, movingImageReader->GetOutput(), transform, CLPProcessInformation);
    collector.Stop( "Resample deformed moving image" );
  }
  // Write deformation field
  if (RigidFieldImageFilename != "")
  {
    collector.Start( "Write deformation field" );    
    WriteFieldImage<TransformType::Pointer>(RigidFieldImageFilename.c_str(), fixedImage, transform);
    collector.Stop( "Write deformation field" );
  }


  if (EnableMetricTesting)
  {
    WriteMetricTestResult<MetricType::Pointer>(MetricTestOutputTranslationFilename.c_str(), TRANSLATION, metric, transform, MetricTestTranslationRange, MetricTestTranslationResolution);
    WriteMetricTestResult<MetricType::Pointer>(MetricTestOutputRotationFilename.c_str(), ROTATION, metric, transform, MetricTestRotationRange, MetricTestRotationResolution);
  }


#if 0 // calculation of the error over the mask region
  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetCenter( transform->GetCenter() );

  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform( finalTransform );
  resampler->SetInput( movingImageReader->GetOutput() );

  fixedImage = fixedImageReader->GetOutput();

  resampler->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetOutputDirection( fixedImage->GetDirection() );
  resampler->SetDefaultPixelValue( 100 );
  
  typedef  float  OutputPixelType;

  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


  CastFilterType::Pointer  caster =  CastFilterType::New();

  NNInterpolatorType::Pointer nnInterp = NNInterpolatorType::New();
  resampler->SetInput(maskImageReader->GetOutput());
  resampler->SetInterpolator(nnInterp);
  resampler->SetDefaultPixelValue( 0 );
  resampler->Update();

  double avDist = 0, maxDist = 0, cnt = 0;
  FixedImageType::Pointer resampledMask = resampler->GetOutput();
  IterType it(resampledMask, resampledMask->GetBufferedRegion());
  for(it.GoToBegin();!it.IsAtEnd();++it){
    FixedImageType::PointType pt0, pt1;
    FixedImageType::IndexType idx;
    idx = it.GetIndex();
    if(it.Get()){
      resampledMask->TransformIndexToPhysicalPoint(idx, pt0);
      pt1 = finalTransform->TransformPoint(pt0);
      double dist = sqrt( (pt0[0]-pt1[0])*(pt0[0]-pt1[0])+
                          (pt0[1]-pt1[1])*(pt0[1]-pt1[1])+
                          (pt0[2]-pt1[2])*(pt0[2]-pt1[2]) );
      avDist += dist;
      if(dist>maxDist)
        maxDist = dist;
      cnt++;
    }
  }

  std::cout << "Average magnitude: " << avDist/cnt << std::endl;
  std::cout << "Max magnitude: " << maxDist << std::endl;
#endif


  if (!EnableDeformable)
  {
    std::cout << "Deformable registration was not enabled." << std::endl;
    return EXIT_SUCCESS;
  }


  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;
  typedef itk::BSplineDeformableTransform< CoordinateRepType, ImageDimension, SplineOrder >     DefTransformType;
  typedef itk::ContinuousIndex<CoordinateRepType, ImageDimension> ContinuousIndexType;  
  typedef itk::LBFGSBOptimizer       DefOptimizerType;  

  DefOptimizerType::Pointer      defOptimizer     = DefOptimizerType::New();
  DefTransformType::Pointer      defTransform     = DefTransformType::New();
  RegistrationType::Pointer   defRegistration  = RegistrationType::New();

  typedef DefTransformType::RegionType RegionType;
  typedef DefTransformType::SpacingType SpacingType;
  typedef DefTransformType::OriginType OriginType;
  typedef DefTransformType::ParametersType     ParametersType;  

  // Setup BSpline deformation
  //
  //  Note that the B-spline computation requires a finite support
  //  region ( 1 grid node at the lower borders and 2 grid nodes at
  //  upper borders).
  RegionType bsplineRegion;
  RegionType::SizeType   gridSizeOnImage;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;

  gridSizeOnImage.SetElement(0, fixedImage->GetBufferedRegion().GetSize()[0]*fixedImage->GetSpacing()[0]/GridSpacing);
  gridSizeOnImage.SetElement(1, fixedImage->GetBufferedRegion().GetSize()[1]*fixedImage->GetSpacing()[1]/GridSpacing);
  gridSizeOnImage.SetElement(2, fixedImage->GetBufferedRegion().GetSize()[2]*fixedImage->GetSpacing()[2]/GridSpacing);

  std::cout << "BSpline grid size: " << gridSizeOnImage << std::endl;

  //gridSizeOnImage.Fill( GridSize );
  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  SpacingType spacing = fixedImage->GetSpacing();
  OriginType origin = fixedImage->GetOrigin();;
  FixedImageType::DirectionType direction = fixedImage->GetDirection(); // :TODO: check if it is the fixed image and not the moving

  FixedImageType::RegionType fixedRegion = fixedImage->GetLargestPossibleRegion();
  FixedImageType::SizeType fixedImageSize = fixedRegion.GetSize();

  for(unsigned int r=0; r<ImageDimension; r++)
    {
    double spacingMultiplier=floor( static_cast<double>(fixedImageSize[r] - 1)  / 
                static_cast<double>(gridSizeOnImage[r] - 1) );
    if (spacingMultiplier<1)
      {
      std::cout << "Image size along dimension " << r << " is smaller than requested grid size " << GridSpacing << std::endl;
      spacingMultiplier=1; // grid spacing should not be smaller than the pixel spacing (it would prevent deformation of first and last slices)
      }
    spacing[r] *= spacingMultiplier;
    for(unsigned int s=0; s<ImageDimension; s++)
      {
      origin[s]  -=  spacing[r]*direction[s][r]; // or [s][r]
      } 
    }

  defTransform->SetGridSpacing ( spacing );
  defTransform->SetGridOrigin  ( origin );
  defTransform->SetGridRegion  ( bsplineRegion );
  defTransform->SetGridDirection ( direction );

  const unsigned int numberOfParameters =
               defTransform->GetNumberOfParameters();
  
  ParametersType parameters( numberOfParameters );
  parameters.Fill( 0.0 );

  defTransform->SetParameters  ( parameters );

  defTransform->SetBulkTransform( transform );
  
  // Setup optimizer
  //
  //
  DefOptimizerType::BoundSelectionType boundSelect( defTransform->GetNumberOfParameters() );
  DefOptimizerType::BoundValueType upperBound( defTransform->GetNumberOfParameters() );
  DefOptimizerType::BoundValueType lowerBound( defTransform->GetNumberOfParameters() );  
  bool ConstrainDeformation=true;
  if (ConstrainDeformation)
    {
    boundSelect.Fill( 2 );
    upperBound.Fill(  MaxDeformation );
    lowerBound.Fill( -MaxDeformation );
    }
  else
    {
    boundSelect.Fill( 0 );
    upperBound.Fill( 0.0 );
    lowerBound.Fill( 0.0 );
    }

  defOptimizer->SetBoundSelection( boundSelect );
  defOptimizer->SetUpperBound    ( upperBound );
  defOptimizer->SetLowerBound    ( lowerBound );

  const int Iterations=300;
  defOptimizer->SetCostFunctionConvergenceFactor ( 1e+1 );
  defOptimizer->SetProjectedGradientTolerance    ( 1e-7 );
  defOptimizer->SetMaximumNumberOfIterations     ( Iterations );
  defOptimizer->SetMaximumNumberOfEvaluations    ( 500 );
  defOptimizer->SetMaximumNumberOfCorrections    ( 12 );

  
  // Create the Command observer and register it with the optimizer.
  //
  DefCommandIterationUpdate::Pointer defObserver = DefCommandIterationUpdate::New();
  defObserver->SetRegistration ( defRegistration );
  defObserver->SetCostFunction( metric );

  defOptimizer->AddObserver( itk::IterationEvent(), defObserver );

  std::cout << std::endl << "Starting Registration" << std::endl;

  // Registration
  //
  //
  defRegistration->SetFixedImage  ( fixedImage  );
  defRegistration->SetMovingImage ( movingImage );
  defRegistration->SetMetric      ( metric       );
  defRegistration->SetOptimizer   ( defOptimizer    );
  defRegistration->SetInterpolator( interpolator );
  defRegistration->SetTransform   ( defTransform    );
  defRegistration->SetInitialTransformParameters( defTransform->GetParameters() );

  try 
    { 
    itk::PluginFilterWatcher watchRegistration(defRegistration,"Deformable registration",CLPProcessInformation,1.0/3.0, 2.0/3.0); 
    collector.Start( "Deformable registration" );
    defRegistration->Update();
    collector.Stop( "Deformable registration" );
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  
  DefOptimizerType::ParametersType defFinalParameters = 
    defRegistration->GetLastTransformParameters();
  std::cout << "Final parameters: " << defFinalParameters[50] << std::endl;
  defTransform->SetParameters      ( defFinalParameters );

  if (DefTransformFilename != "")
    {
    typedef itk::TransformFileWriter TransformWriterType;
    TransformWriterType::Pointer outputTransformWriter;

    outputTransformWriter= TransformWriterType::New();
    outputTransformWriter->SetFileName( DefTransformFilename );
    outputTransformWriter->SetInput( defTransform );
    outputTransformWriter->AddTransform( defTransform->GetBulkTransform() );
    try
      {
      outputTransformWriter->Update();
      }
    catch (itk::ExceptionObject &err)
      {
      std::cerr << err << std::endl;
      return EXIT_FAILURE ;
      }
    }

  // Write deformed image
  if (ResampledDefMovingImageFilename != "")
  {
    // movingImage it was blurred, so read it again to get the oriignal
    collector.Start( "Resample deformed moving image" );
    WriteResampledImage(ResampledDefMovingImageFilename.c_str(), fixedImage, movingImageReader->GetOutput(), defTransform, CLPProcessInformation);
    collector.Stop( "Resample deformed moving image" );
  }
  // Write deformation field
  if (DefFieldImageFilename != "")
  {
    collector.Start( "Write deformation field" );    
    WriteFieldImage<DefTransformType::Pointer>(DefFieldImageFilename.c_str(), fixedImage, defTransform);
    collector.Stop( "Write deformation field" );
  }

  // Report the time taken by the registration
  collector.Report();

  return EXIT_SUCCESS;
}
