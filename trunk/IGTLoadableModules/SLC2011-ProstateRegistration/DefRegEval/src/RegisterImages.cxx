#include "DefRegEval.h"

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkCastImageFilter.h"
#include "itkLevelSetMotionRegistrationFilter.h"
#include "itkWarpImageFilter.h"
#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkBSplineDeformableTransform.h"

#include <fstream>


//////////////////////////

typedef itk::LevelSetMotionRegistrationFilter<InternalImageType,InternalImageType,DeformationFieldType> DeformableRegistrationFilterType;
typedef itk::VersorRigid3DTransformOptimizer OptimizerType;

//////////////////////////

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;
  itkNewMacro( CommandIterationUpdate );
protected:
  CommandIterationUpdate() {};

public:

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    const DeformableRegistrationFilterType * deformableReg = dynamic_cast< const DeformableRegistrationFilterType * >( object );
    const OptimizerType * optimizer = dynamic_cast< const OptimizerType * >( object );

    if( !(itk::IterationEvent().CheckEvent( &event )) )
    {
      return;
    }

    if (deformableReg!=NULL)
    {
      std::cout << deformableReg->GetMetric() << std::endl;
    }
    if (optimizer!=NULL)
    {
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    } 
  }

};



//////////////////////////

void RigidRegistration(DeformationFieldType::Pointer& deformationField, ExternalImageType::ConstPointer fixedImage, ExternalImageType::ConstPointer movingImage, int maxNumberOfIterations=200)
{

  typedef itk::VersorRigid3DTransform<double> TransformType;
  typedef itk::MeanSquaresImageToImageMetric<ExternalImageType,ExternalImageType> MetricType;
  typedef itk::LinearInterpolateImageFunction<ExternalImageType,double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<ExternalImageType,ExternalImageType> RigidRegistrationFilterType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RigidRegistrationFilterType::Pointer   registration  = RigidRegistrationFilterType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage(fixedImage);
  registration->SetMovingImage(movingImage);

  registration->SetFixedImageRegion(fixedImage->GetBufferedRegion() );

  typedef itk::CenteredTransformInitializer<TransformType, ExternalImageType, ExternalImageType>  TransformInitializerType;

  TransformInitializerType::Pointer initializer = TransformInitializerType::New();

  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImage );
  initializer->SetMovingImage( movingImage );

  initializer->MomentsOn();

  initializer->InitializeTransform();

  typedef TransformType::VersorType  VersorType;
  typedef VersorType::VectorType     VectorType;

  VersorType     rotation;
  VectorType     axis;
  
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  const double angle = 0;

  rotation.Set(  axis, angle  );

  transform->SetRotation( rotation );

  registration->SetInitialTransformParameters( transform->GetParameters() );

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;

  optimizer->SetScales( optimizerScales );

  optimizer->SetMaximumStepLength( 0.2000  ); 
  optimizer->SetMinimumStepLength( 0.0001 );

  optimizer->SetNumberOfIterations( maxNumberOfIterations );

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  try 
    { 
    registration->StartRegistration(); 
    std::cout << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    exit(EXIT_FAILURE);
    } 
  
  OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();

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

  std::cout << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << std::endl << offset << std::endl;

  // Generate the explicit deformation field resulting from 
  // the registration.

  typedef itk::Image< VectorPixelType, ImageDimension >  DeformationFieldType;

  deformationField = DeformationFieldType::New();
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
  
 }

void DeformableRegistration(DeformationFieldType::Pointer& deformationField, ExternalImageType::ConstPointer fixedImage, ExternalImageType::ConstPointer movingImage, int maxNumberOfIterations=30, double gradientSmoothingStandardDeviations=0)
{
  typedef itk::CastImageFilter<ExternalImageType, InternalImageType > FixedImageCasterType;
  typedef itk::CastImageFilter<ExternalImageType, InternalImageType > MovingImageCasterType;

  FixedImageCasterType::Pointer fixedImageCaster   = FixedImageCasterType::New();
  MovingImageCasterType::Pointer movingImageCaster = MovingImageCasterType::New();

  fixedImageCaster->SetInput(fixedImage);
  movingImageCaster->SetInput(movingImage); 

  DeformableRegistrationFilterType::Pointer registrationFilter = DeformableRegistrationFilterType::New();

  // Create the Command observer and register it with the registration filter.
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  registrationFilter->AddObserver( itk::IterationEvent(), observer );

  registrationFilter->SetFixedImage(fixedImageCaster->GetOutput());
  registrationFilter->SetMovingImage(movingImageCaster->GetOutput());

  registrationFilter->SetNumberOfIterations(maxNumberOfIterations);
  registrationFilter->SetGradientSmoothingStandardDeviations(gradientSmoothingStandardDeviations);

  if (gradientSmoothingStandardDeviations>0)
  {
    // To smooth the deformation field (otherwise deformation field is nonzero near the object edge only)
    registrationFilter->SmoothDeformationFieldOn();
    registrationFilter->SetStandardDeviations(2.0);
  }

  registrationFilter->Update();

  deformationField=registrationFilter->GetDeformationField();

}



int main( int argc, char * argv[] )
{
  std::string fixedImageFilename = "PreOpSegmentedImage.mha";
  std::string movingImageFilename = "IntraOpSegmentedImage.mha";
  std::string registrationMethod = "RIGID";
  std::string outputRegisteredImageFilename;
  std::string outputDeformationFieldFilename;
  std::string fixedPointsFilename;
  std::string movingPointsFilename;

  int numberOfIterations=5;
  double gradientSmoothingStandardDeviations=4.0;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--fixedImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fixedImageFilename, "Fixed image file name");
  args.AddArgument("--movingImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &movingImageFilename, "Moving image file name");
  args.AddArgument("--registrationMethod", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &registrationMethod, "Registration method: DEFORMABLE or RIGID"); 
  args.AddArgument("--outputRegisteredImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputRegisteredImageFilename, "Resulting deformed moving image (optional)");
  args.AddArgument("--outputDeformationField", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputDeformationFieldFilename, "Resulting deformation image file name (optional)");
  args.AddArgument("--inputFixedImagePoints", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fixedPointsFilename, "Position of points in the fixed image"); 
  args.AddArgument("--outputMovingImagePoints", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &movingPointsFilename, "Resulting point positions in the moving image"); 
  args.AddArgument("--numberOfIterations", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfIterations, "Number of registration iterations"); 
  args.AddArgument("--gradientSmoothingStandardDeviations", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &gradientSmoothingStandardDeviations, "For LEVELSET: gradient smoothing (in standard deviations)"); 

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }


  // Read images

  ExternalImageReaderType::Pointer fixedReader = ExternalImageReaderType::New();
  fixedReader->SetFileName( fixedImageFilename );
  try
  {
    fixedReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }
  ExternalImageType::ConstPointer fixedImage = fixedReader->GetOutput();

  ExternalImageReaderType::Pointer movingReader = ExternalImageReaderType::New();
  movingReader->SetFileName( movingImageFilename );
  try
  {
    movingReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }
  ExternalImageType::ConstPointer movingImage = movingReader->GetOutput();

  bool needDeformationField=false;
  if (!outputDeformationFieldFilename.empty())
  {
    needDeformationField=true;
  }

  DeformationFieldType::Pointer deformationField;

  if (registrationMethod.compare("DEFORMABLE")==0)
  {
    DeformableRegistration(deformationField, fixedImage, movingImage, numberOfIterations, gradientSmoothingStandardDeviations);
  }
  else if (registrationMethod.compare("RIGID")==0)
  {
    RigidRegistration(deformationField, fixedImage, movingImage, numberOfIterations);
  }

  if (deformationField.IsNotNull() && !outputDeformationFieldFilename.empty())
  {
    typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
    FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
    fieldWriter->SetInput( deformationField );
    fieldWriter->SetFileName( outputDeformationFieldFilename );
    try
    {
      fieldWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (deformationField.IsNotNull() && !outputRegisteredImageFilename.empty())
  {
    //create the filter that applies the deformation
    typedef itk::WarpImageFilter<ExternalImageType, ExternalImageType, DeformationFieldType>  WarpImageFilterType;
    WarpImageFilterType::Pointer warpFilter = WarpImageFilterType::New();

    //use linear interpolation method for float-valued outputs of the deformation
    typedef itk::LinearInterpolateImageFunction<ExternalImageType, double>  InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    warpFilter->SetInterpolator(interpolator);
    warpFilter->SetOutputSpacing(deformationField->GetSpacing());
    warpFilter->SetOutputOrigin(deformationField->GetOrigin());
    warpFilter->SetDeformationField(deformationField);

    warpFilter->SetInput(movingImage);

    try
    {
      warpFilter->Update();
    }
    catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

    typedef   itk::ImageFileWriter< ExternalImageType >  MovingWriterType;
    MovingWriterType::Pointer movingWriter = MovingWriterType::New();
    movingWriter->SetFileName( outputRegisteredImageFilename );
    movingWriter->SetInput(warpFilter->GetOutput());

    try
    {
      movingWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
