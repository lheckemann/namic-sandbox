#ifndef reg_3d_bspline_mi_hpp_
#define reg_3d_bspline_mi_hpp_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"

#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSBOptimizer.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
//#include "itkSquaredDifferenceImageFilter.h"


#include <csignal>

template<typename fix_image_t, typename moving_image_t, typename output_image_t>
typename output_image_t::Pointer
reg_3d_bspline_mi(typename fix_image_t::Pointer fixImg,         \
                   typename moving_image_t::Pointer movingImg,   \
                   typename moving_image_t::PixelType fillInVal, \
                   int gridNum)
{
  const unsigned int ImageDimension = 3;
  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform< CoordinateRepType, SpaceDimension, SplineOrder >     TransformType;

  /**
   * set up the optimizer
   */
  typedef itk::LBFGSBOptimizer       OptimizerType;


  /**
   * MI metric
   */
  typedef itk::MattesMutualInformationImageToImageMetric< fix_image_t, moving_image_t >    MetricType;

  /**
   * Linear interpolator
   */
  typedef itk::LinearInterpolateImageFunction< moving_image_t, double >    InterpolatorType;


  /**
   * Registratio type
   */
  typedef itk::ImageRegistrationMethod< fix_image_t, moving_image_t >    RegistrationType;

  typename MetricType::Pointer         metric        = MetricType::New();
  typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
  typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer   registration  = RegistrationType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  typename TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage(  fixImg   );
  registration->SetMovingImage(   movingImg   );

  typename fix_image_t::RegionType fixedRegion = fixImg->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedRegion );


  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;
  typename RegionType::SizeType   gridSizeOnImage;
  typename RegionType::SizeType   gridBorderSize;
  typename RegionType::SizeType   totalGridSize;

  gridSizeOnImage.Fill( gridNum );
  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef typename TransformType::SpacingType SpacingType;
  SpacingType spacing = fixImg->GetSpacing();

  typedef typename TransformType::OriginType OriginType;
  OriginType origin = fixImg->GetOrigin();

  typename fix_image_t::SizeType fixedImageSize = fixedRegion.GetSize();

  for(unsigned int r=0; r<ImageDimension; r++)
    {
      spacing[r] *= static_cast<double>(fixedImageSize[r] - 1)  / 
        static_cast<double>(gridSizeOnImage[r] - 1);
    }

  typename fix_image_t::DirectionType gridDirection = fixImg->GetDirection();
  SpacingType gridOriginOffset = gridDirection * spacing;

  OriginType gridOrigin = origin - gridOriginOffset; 

  transform->SetGridSpacing( spacing );
  transform->SetGridOrigin( gridOrigin );
  transform->SetGridRegion( bsplineRegion );
  transform->SetGridDirection( gridDirection );
  

  typedef typename TransformType::ParametersType     ParametersType;

  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  
  ParametersType parameters( numberOfParameters );

  parameters.Fill( 0.0 );

  transform->SetParameters( parameters );
  registration->SetInitialTransformParameters( transform->GetParameters() );


//   std::cout << "Intial Parameters = " << std::endl;
//   std::cout << transform->GetParameters() << std::endl;

  typename OptimizerType::BoundSelectionType boundSelect( transform->GetNumberOfParameters() );
  typename OptimizerType::BoundValueType upperBound( transform->GetNumberOfParameters() );
  typename OptimizerType::BoundValueType lowerBound( transform->GetNumberOfParameters() );

  boundSelect.Fill( 0 );
  upperBound.Fill( 0.0 );
  lowerBound.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( 1e+1 );
  optimizer->SetProjectedGradientTolerance( 1e-7 );
  optimizer->SetMaximumNumberOfIterations( 20 );
  optimizer->SetMaximumNumberOfEvaluations( 500 );
  optimizer->SetMaximumNumberOfCorrections( 12 );

  //optimizer->MaximizeOn(); // MI needs to be maximized to align images
  optimizer->MinimizeOn();


  metric->SetNumberOfHistogramBins( 50 );
  const unsigned int numberOfSamples = 
    static_cast<unsigned int>( fixedRegion.GetNumberOfPixels() / 20.0 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );
  //metric->SetNumberOfSpatialSamples( 50000 );
  metric->ReinitializeSeed( 76926294 );
  metric->SetUseCachingOfBSplineWeights( true );


  //std::cout << std::endl << "Starting Registration" << std::endl;

  try 
    { 
      registration->StartRegistration(); 
      //       std::cout << "Optimizer stop condition = "
      //                 << registration->GetOptimizer()->GetStopConditionDescription()
      //                 << std::endl;
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      raise(SIGABRT);
    } 
  
  typename OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

  transform->SetParameters( finalParameters );

  typedef itk::ResampleImageFilter< moving_image_t, fix_image_t >    ResampleFilterType;
  typename ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform( transform );
  resample->SetInput( movingImg );
  resample->SetSize(    fixImg->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixImg->GetOrigin() );
  resample->SetOutputSpacing( fixImg->GetSpacing() );
  resample->SetOutputDirection( fixImg->GetDirection() );
  resample->SetDefaultPixelValue( 0 );
  resample->Update();


  return resample->GetOutput();
}



template<typename fix_image_t, typename moving_image_t, typename output_image_t>
std::pair<typename output_image_t::Pointer, typename output_image_t::Pointer>
reg_3d_bspline_mi(typename fix_image_t::Pointer fixImg,         \
                  typename moving_image_t::Pointer movingImg1,  \
                  typename moving_image_t::Pointer movingImg2,  \
                  typename moving_image_t::PixelType fillInVal, \
                  int gridNum)
{
  const unsigned int ImageDimension = 3;
  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform< CoordinateRepType, SpaceDimension, SplineOrder >     TransformType;

  /**
   * set up the optimizer
   */
  typedef itk::LBFGSBOptimizer       OptimizerType;


  /**
   * MI metric
   */
  typedef itk::MattesMutualInformationImageToImageMetric< fix_image_t, moving_image_t >    MetricType;

  /**
   * Linear interpolator
   */
  typedef itk::LinearInterpolateImageFunction< moving_image_t, double >    InterpolatorType;


  /**
   * Registratio type
   */
  typedef itk::ImageRegistrationMethod< fix_image_t, moving_image_t >    RegistrationType;

  typename MetricType::Pointer         metric        = MetricType::New();
  typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
  typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer   registration  = RegistrationType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  typename TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage(  fixImg   );
  registration->SetMovingImage(   movingImg1   );

  typename fix_image_t::RegionType fixedRegion = fixImg->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedRegion );


  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;
  typename RegionType::SizeType   gridSizeOnImage;
  typename RegionType::SizeType   gridBorderSize;
  typename RegionType::SizeType   totalGridSize;

  gridSizeOnImage.Fill( gridNum );
  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef typename TransformType::SpacingType SpacingType;
  SpacingType spacing = fixImg->GetSpacing();

  typedef typename TransformType::OriginType OriginType;
  OriginType origin = fixImg->GetOrigin();

  typename fix_image_t::SizeType fixedImageSize = fixedRegion.GetSize();

  for(unsigned int r=0; r<ImageDimension; r++)
    {
      spacing[r] *= static_cast<double>(fixedImageSize[r] - 1)  / 
        static_cast<double>(gridSizeOnImage[r] - 1);
    }

  typename fix_image_t::DirectionType gridDirection = fixImg->GetDirection();
  SpacingType gridOriginOffset = gridDirection * spacing;

  OriginType gridOrigin = origin - gridOriginOffset; 

  transform->SetGridSpacing( spacing );
  transform->SetGridOrigin( gridOrigin );
  transform->SetGridRegion( bsplineRegion );
  transform->SetGridDirection( gridDirection );
  

  typedef typename TransformType::ParametersType     ParametersType;

  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  
  ParametersType parameters( numberOfParameters );

  parameters.Fill( 0.0 );

  transform->SetParameters( parameters );
  registration->SetInitialTransformParameters( transform->GetParameters() );


//   std::cout << "Intial Parameters = " << std::endl;
//   std::cout << transform->GetParameters() << std::endl;

  typename OptimizerType::BoundSelectionType boundSelect( transform->GetNumberOfParameters() );
  typename OptimizerType::BoundValueType upperBound( transform->GetNumberOfParameters() );
  typename OptimizerType::BoundValueType lowerBound( transform->GetNumberOfParameters() );

  boundSelect.Fill( 0 );
  upperBound.Fill( 0.0 );
  lowerBound.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( 1e+1 );
  optimizer->SetProjectedGradientTolerance( 1e-7 );
  optimizer->SetMaximumNumberOfIterations( 20 );
  optimizer->SetMaximumNumberOfEvaluations( 500 );
  optimizer->SetMaximumNumberOfCorrections( 12 );

  //optimizer->MaximizeOn(); // MI needs to be maximized to align images
  optimizer->MinimizeOn();


  metric->SetNumberOfHistogramBins( 50 );
  const unsigned int numberOfSamples = 
    static_cast<unsigned int>( fixedRegion.GetNumberOfPixels() / 20.0 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );
  //metric->SetNumberOfSpatialSamples( 50000 );
  metric->ReinitializeSeed( 76926294 );
  metric->SetUseCachingOfBSplineWeights( true );


  //std::cout << std::endl << "Starting Registration" << std::endl;

  try 
    { 
      registration->StartRegistration(); 
      //       std::cout << "Optimizer stop condition = "
      //                 << registration->GetOptimizer()->GetStopConditionDescription()
      //                 << std::endl;
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      raise(SIGABRT);
    } 
  
  typename OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

  transform->SetParameters( finalParameters );

  typedef itk::ResampleImageFilter< moving_image_t, fix_image_t >    ResampleFilterType;


  // transform moving img1
  typename ResampleFilterType::Pointer resampler1 = ResampleFilterType::New();
  resampler1->SetTransform( transform );
  resampler1->SetInput( movingImg1 );
  resampler1->SetSize( fixImg->GetLargestPossibleRegion().GetSize() );
  resampler1->SetOutputOrigin(  fixImg->GetOrigin() );
  resampler1->SetOutputSpacing( fixImg->GetSpacing() );
  resampler1->SetOutputDirection( fixImg->GetDirection() );
  resampler1->SetDefaultPixelValue( fillInVal );
  
  typedef itk::CastImageFilter< fix_image_t, output_image_t > CastFilterType;
  typename CastFilterType::Pointer  caster1 =  CastFilterType::New();

  caster1->SetInput( resampler1->GetOutput() );
  caster1->Update();
  typename output_image_t::Pointer regMovingImg1 = caster1->GetOutput();


  // transform moving img2
  typename ResampleFilterType::Pointer resampler2 = ResampleFilterType::New();
  resampler2->SetTransform( transform );
  resampler2->SetInput( movingImg2 );
  resampler2->SetSize( fixImg->GetLargestPossibleRegion().GetSize() );
  resampler2->SetOutputOrigin(  fixImg->GetOrigin() );
  resampler2->SetOutputSpacing( fixImg->GetSpacing() );
  resampler2->SetOutputDirection( fixImg->GetDirection() );
  resampler2->SetDefaultPixelValue( fillInVal );
  
  typedef itk::CastImageFilter< fix_image_t, output_image_t > CastFilterType;
  typename CastFilterType::Pointer  caster2 =  CastFilterType::New();

  caster2->SetInput( resampler2->GetOutput() );
  caster2->Update();
  typename output_image_t::Pointer regMovingImg2 = caster2->GetOutput();
  
  return std::make_pair(regMovingImg1, regMovingImg2);
}


#endif
