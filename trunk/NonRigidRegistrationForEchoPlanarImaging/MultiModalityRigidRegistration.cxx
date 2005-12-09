

#include "MultiModalityRigidRegistration.h"

namespace itk
{

MultiModalityRigidRegistration
::MultiModalityRigidRegistration()
{
  m_FixedNormalizer     = FixedNormalizeFilterType::New();
  m_MovingNormalizer    = MovingNormalizeFilterType::New();

  m_FixedWriter         = ResampledNormalizedWriter::New();
  m_MovingWriter        = ResampledNormalizedWriter::New();
  m_MovingNativeWriter  = ResampledWriter::New();

  m_Metric              = MetricType::New();
  m_Transform           = TransformType::New();
  m_Optimizer           = OptimizerType::New();
  m_LinearInterpolator  = LinearInterpolatorType::New();
  m_NearestInterpolator = NearestInterpolatorType::New();
  m_NativeLinearInterpolator = NativeLinearInterpolatorType::New();

  m_RegistrationMethod  = RegistrationType::New();

  m_RegistrationMethod->SetOptimizer(     m_Optimizer     );
  m_RegistrationMethod->SetInterpolator(  m_LinearInterpolator  );
  m_RegistrationMethod->SetMetric(        m_Metric        );
  m_RegistrationMethod->SetTransform(     m_Transform     );

  m_Metric->SetNumberOfHistogramBins( 256 );
  m_Metric->SetNumberOfSpatialSamples( 100000 );

  m_BackgroundLevel = itk::NumericTraits< MovingPixelType >::Zero;

  m_IterationObserver            = IterationObserverType::New();
  m_StopObserver                 = StopObserverType::New();

  m_Optimizer->AddObserver( itk::IterationEvent(), m_IterationObserver );
  m_Optimizer->AddObserver( itk::EndEvent(),       m_StopObserver );

  m_Optimizer->MinimizeOn();

  // Filters for sub-resolultion levels
  m_FixedResampler  = ResampleFilterType::New();
  m_MovingResampler = ResampleFilterType::New();
  m_MovingNativeResampler = ResampleNativeFilterType::New();

  m_FixedMask = SpatialObjectMaskType::New();
  m_MovingMask = SpatialObjectMaskType::New();

  m_FixedNormalizer->SetInput( m_FixedImage );
  m_MovingNormalizer->SetInput( m_MovingImage );
  
  m_FixedWriter->SetInput(  m_FixedResampler->GetOutput()  );
  m_MovingWriter->SetInput( m_MovingResampler->GetOutput() );
  m_MovingNativeWriter->SetInput( m_MovingNativeResampler->GetOutput() );

  // Insert scale factor for multiresolution.
  m_LevelFactor.push_back( 4 );
  m_LevelFactor.push_back( 2 );
  m_LevelFactor.push_back( 1 );

  m_Level = 0;
  m_ReportTimers = true;

  m_QualityLevel = Low;  // Fast registration.
}





MultiModalityRigidRegistration
::~MultiModalityRigidRegistration()
{
  if( m_ReportTimers )
    {
    std::cout << std::endl << std::endl;
    m_Timer.Report();
    }
}





void
MultiModalityRigidRegistration
::SetFixedImage( const FixedImageType * image )
{
  m_FixedImage = image;
  m_FixedNormalizer->SetInput( m_FixedImage );
}



void
MultiModalityRigidRegistration
::SetFixedImageMask( const MaskImageType * imageMask )
{
  if( !imageMask )
    {
    return;
    }
  m_FixedMask->SetImage( imageMask );
  m_Metric->SetFixedImageMask( m_FixedMask );
}



void
MultiModalityRigidRegistration
::SetMovingImageMask( const MaskImageType * imageMask )
{
  if( !imageMask )
    {
    return;
    }
  m_MovingMask->SetImage( imageMask );
  m_Metric->SetMovingImageMask( m_MovingMask );
}






void
MultiModalityRigidRegistration
::SetMovingImage( const MovingImageType * image )
{
  m_MovingImage = image;
  m_MovingNormalizer->SetInput( m_MovingImage );
  m_MovingNativeResampler->SetInput( m_MovingImage );
}


void
MultiModalityRigidRegistration
::SetQualityLevel( QualityLevelType level )
{
  m_QualityLevel = level;
}


void
MultiModalityRigidRegistration
::PrepareLevel( unsigned int factor )
{
  m_Timer.Start("PrepareLevel");

  InternalImageType::SpacingType spacing;
  InternalImageType::RegionType  region;
  InternalImageType::SizeType    size;

  // Sub-sample the Fixed image
  m_FixedResampler->SetInput( m_FixedNormalizer->GetOutput() );

  spacing = m_FixedImage->GetSpacing();
  region  = m_FixedImage->GetLargestPossibleRegion();
  size    = region.GetSize();

  for(unsigned int i=0; i<Dimension; i++)
    {
    spacing[i] *= factor;
    size[i]    /= factor;
    }

  m_FixedResampler->SetOutputSpacing( spacing );
  m_FixedResampler->SetOutputOrigin( m_FixedImage->GetOrigin() );
  m_FixedResampler->SetSize( size );
  m_FixedResampler->SetOutputStartIndex( region.GetIndex() );
  m_FixedResampler->SetTransform( itk::IdentityTransform<double>::New() );

  m_FixedResampler->UpdateLargestPossibleRegion();

  ::itk::OStringStream fixedFileName;
  fixedFileName << "fixed" << factor << ".nrrd";
  m_FixedWriter->SetFileName( fixedFileName.str().c_str() );
  m_FixedWriter->Update();

  m_RegistrationMethod->SetFixedImage( m_FixedResampler->GetOutput() );

  // Sub-sample the Moving image
  m_MovingResampler->SetInput( m_MovingNormalizer->GetOutput() );

  spacing = m_MovingImage->GetSpacing();
  region  = m_MovingImage->GetLargestPossibleRegion();
  size    = region.GetSize();

  for(unsigned int i=0; i<Dimension; i++)
    {
    spacing[i] *= factor;
    size[i]    /= factor;
    }

  m_MovingResampler->SetOutputSpacing( spacing );
  m_MovingResampler->SetOutputOrigin( m_MovingImage->GetOrigin() );
  m_MovingResampler->SetSize( size );
  m_MovingResampler->SetOutputStartIndex( region.GetIndex() );
  m_MovingResampler->SetTransform( itk::IdentityTransform<double>::New() );

  m_MovingResampler->UpdateLargestPossibleRegion();

  ::itk::OStringStream movingFileName;
  movingFileName << "moving" << factor << ".nrrd";
  m_MovingWriter->SetFileName( movingFileName.str().c_str() );
  m_MovingWriter->Update();

  m_RegistrationMethod->SetMovingImage( m_MovingResampler->GetOutput() );

  m_MovingNativeResampler->SetOutputSpacing( spacing );
  m_MovingNativeResampler->SetOutputOrigin( m_MovingImage->GetOrigin() );
  m_MovingNativeResampler->SetSize( size );
  m_MovingNativeResampler->SetOutputStartIndex( region.GetIndex() );
  m_MovingNativeResampler->SetTransform( itk::IdentityTransform<double>::New() );

  m_MovingNativeResampler->UpdateLargestPossibleRegion();

  // TODO - test
  //m_MovingNativeWriter->SetInput( m_MovingImage );


  ::itk::OStringStream movingNativeFileName;
  movingNativeFileName << "movingNative" << factor << ".nrrd";
  m_MovingNativeWriter->SetFileName( movingNativeFileName.str().c_str() );
  m_MovingNativeWriter->Update();


  region = m_FixedResampler->GetOutput()->GetBufferedRegion();

  const unsigned int numberOfSamples = 
    static_cast< unsigned int >( region.GetNumberOfPixels() * 0.60 );

  std::cout << "FIXED IMAGE = " << std::endl;
  m_FixedResampler->GetOutput()->Print( std::cout );

  std::cout << "MOVING IMAGE = " << std::endl;
  m_MovingResampler->GetOutput()->Print( std::cout );

  std::cout << "NumberOfSpatialSamples = " << numberOfSamples << std::endl;
  m_Metric->SetNumberOfSpatialSamples( numberOfSamples );
    
  m_RegistrationMethod->SetFixedImageRegion( region );

  m_Timer.Stop("PrepareLevel");
}




void
MultiModalityRigidRegistration
::RegisterCurrentResolutionLevel()
{
  const unsigned int numberOfQualityOptions   = 2;
  const unsigned int numberOfResolutionLevels = 3;

  int    IterationsList[  numberOfResolutionLevels ][ numberOfQualityOptions ];
  double MaximumStepList[ numberOfResolutionLevels ][ numberOfQualityOptions ];
  double MinimumStepList[ numberOfResolutionLevels ][ numberOfQualityOptions ];
  double GradientTolList[ numberOfResolutionLevels ][ numberOfQualityOptions ];


  // Quality level Low = Fastest run time, Lowest Quality
  MaximumStepList[ 0 ][ Low ] = 1.00; 
  MaximumStepList[ 1 ][ Low ] = 0.10; 
  MaximumStepList[ 2 ][ Low ] = 0.01; 

  MinimumStepList[ 0 ][ Low ] = 0.01; 
  MinimumStepList[ 1 ][ Low ] = 0.001; 
  MinimumStepList[ 2 ][ Low ] = 0.0001; 

  IterationsList[ 0 ][ Low ] = 100;
  IterationsList[ 1 ][ Low ] = 100;
  IterationsList[ 2 ][ Low ] = 100;

  GradientTolList[ 0 ][ Low ] = 0.1;
  GradientTolList[ 1 ][ Low ] = 0.1;
  GradientTolList[ 2 ][ Low ] = 0.1;
  
 
  // Quality level High = Slowest run time, Highest Quality
  MaximumStepList[ 0 ][ High ] = 0.500; 
  MaximumStepList[ 1 ][ High ] = 0.050; 
  MaximumStepList[ 2 ][ High ] = 0.005; 

  MinimumStepList[ 0 ][ High ] = 0.001; 
  MinimumStepList[ 1 ][ High ] = 0.0001; 
  MinimumStepList[ 2 ][ High ] = 0.00001; 

  IterationsList[ 0 ][ High ] = 500;
  IterationsList[ 1 ][ High ] = 500;
  IterationsList[ 2 ][ High ] = 500;

  GradientTolList[ 0 ][ High ] = 0.01;
  GradientTolList[ 1 ][ High ] = 0.01;
  GradientTolList[ 2 ][ High ] = 0.01;
 

  m_Timer.Start("RegisteringOneLevel");

  std::cout << "RegisterCurrentResolutionLevel = Level = " << m_Level << std::endl;

  this->PrepareLevel( m_LevelFactor[ m_Level ] );
  
  m_Optimizer->SetMaximumStepLength( MaximumStepList[ m_Level ][ m_QualityLevel ] );
  m_Optimizer->SetMinimumStepLength( MinimumStepList[ m_Level ][ m_QualityLevel ] );
  m_Optimizer->SetNumberOfIterations( IterationsList[ m_Level ][ m_QualityLevel ] );
  m_Optimizer->SetGradientMagnitudeTolerance( GradientTolList[ m_Level ][ m_QualityLevel ] );

  m_RegistrationMethod->SetInitialTransformParameters( 
                                  m_Transform->GetParameters() ); 

  m_RegistrationMethod->StartRegistration(); 

  std::cout << "Stopping condition = " << m_Optimizer->GetStopCondition() << std::endl;
  m_Level++;

  m_Timer.Stop("RegisteringOneLevel");
}



void
MultiModalityRigidRegistration
::UpdateInputs()
{
  m_Timer.Start("UpdateInputs");

  if( m_FixedImage->GetSource() )
    {
    m_FixedImage->GetSource()->Update();
    }

  if( m_MovingImage->GetSource() )
    {
    m_MovingImage->GetSource()->Update();
    }
  
  m_Timer.Stop("UpdateInputs");
}



void
MultiModalityRigidRegistration
::SetInitialTransform(const TransformType * transform)
{
  m_Timer.Start("SetInitialTransform");
  m_Transform->SetCenter( transform->GetCenter() );
  m_Transform->SetParameters( transform->GetParameters() );
  m_Timer.Stop("SetInitialTransform");
}




void
MultiModalityRigidRegistration
::InitializeRegistration()
{

  m_Timer.Start("InitializeRegistration");

  this->UpdateInputs();

  InternalImageType::SizeType    fixedImageSize     = m_FixedImage->GetBufferedRegion().GetSize();
  InternalImageType::SpacingType fixedImageSpacing  = m_FixedImage->GetSpacing();

  
  typedef OptimizerType::ScalesType       OptimizerScalesType;

  const unsigned int numberOfParameters = m_Transform->GetNumberOfParameters();

  OptimizerScalesType optimizerScales( numberOfParameters );

  optimizerScales[0] = 1.0; // scale for rotation
  optimizerScales[1] = 1.0; // scale for rotation
  optimizerScales[2] = 1.0; // scale for rotation

  optimizerScales[3] = 1.0 / ( 10.0 * fixedImageSize[0] * fixedImageSpacing[0] );
  optimizerScales[4] = 1.0 / ( 10.0 * fixedImageSize[1] * fixedImageSpacing[1] );
  optimizerScales[5] = 1.0 / ( 10.0 * fixedImageSize[2] * fixedImageSpacing[2] );


  std::cout << "Scales = " << optimizerScales << std::endl;

  m_Optimizer->SetScales( optimizerScales );

  m_Optimizer->SetRelaxationFactor( 0.9 );

  m_RegistrationMethod->SetFixedImage(  m_FixedResampler->GetOutput()  );
  m_RegistrationMethod->SetMovingImage( m_MovingResampler->GetOutput() );

  m_Timer.Stop("InitializeRegistration");
}





void
MultiModalityRigidRegistration
::ReportTimersOff()
{
  m_ReportTimers = false;
}


void
MultiModalityRigidRegistration
::ReportTimersOn()
{
  m_ReportTimers = true;
}



const MultiModalityRigidRegistration::ParametersType &
MultiModalityRigidRegistration
::GetParameters() const
{
  return m_RegistrationMethod->GetLastTransformParameters();
}




const MultiModalityRigidRegistration::TransformType *
MultiModalityRigidRegistration
::GetTransform() const
{
  m_Transform->SetParameters( m_RegistrationMethod->GetLastTransformParameters() );
  return m_Transform;
}




} // end of namespace itk


