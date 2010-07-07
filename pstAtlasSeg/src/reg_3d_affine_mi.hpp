#ifndef reg_3d_affine_mi_hpp_
#define reg_3d_affine_mi_hpp_
#if defined(_MSC_VER)
   #pragma warning ( disable : 4786 )
#endif


// itk, IO
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"


// itk reg-opt
#include "itkRegularStepGradientDescentOptimizer.h"
//#include "itkLBFGSBOptimizer.h"

#include "itkCenteredTransformInitializer.h"

#include "itkAffineTransform.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"


// std
#include <utility>


namespace newProstate
{

  template<typename fix_image_t, typename moving_image_t, typename output_image_t>
  std::pair<typename output_image_t::Pointer, typename output_image_t::Pointer>
  reg_3d_affine_mi(typename fix_image_t::Pointer fixImg,         \
                   typename moving_image_t::Pointer movingImg1,      \
                   typename moving_image_t::Pointer movingImg2,  \
                   typename moving_image_t::PixelType fillInVal, \
                   double& finalCost = 0)
  {
    typedef itk::AffineTransform< double, 3 > transform_t;

    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    //typedef itk::LBFGSBOptimizer       OptimizerType;

    typedef itk::MattesMutualInformationImageToImageMetric< fix_image_t, moving_image_t >    MetricType;
    typedef itk::LinearInterpolateImageFunction< moving_image_t, double >    InterpolatorType;
    typedef itk::ImageRegistrationMethod< fix_image_t, moving_image_t >    RegistrationType;

    typename MetricType::Pointer         metric        = MetricType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();

    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );

    typename transform_t::Pointer  transform = transform_t::New();
    registration->SetTransform( transform );

    registration->SetFixedImage(  fixImg    );
    registration->SetMovingImage( movingImg1 );

    registration->SetFixedImageRegion( fixImg->GetBufferedRegion() );


    typedef itk::CenteredTransformInitializer< transform_t, fix_image_t, moving_image_t >  TransformInitializerType;
    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
    initializer->SetTransform(   transform );
    initializer->SetFixedImage(  fixImg );
    initializer->SetMovingImage( movingImg1 );
    //initializer->MomentsOn();
    initializer->GeometryOn();
    initializer->InitializeTransform();

    registration->SetInitialTransformParameters( transform->GetParameters() );

    double translationScale = 1.0 / 100.0;

    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

    optimizerScales[0] =  1.0;
    optimizerScales[1] =  1.0;
    optimizerScales[2] =  1.0;
    optimizerScales[3] =  1.0;
    optimizerScales[4] =  1.0;
    optimizerScales[5] =  1.0;
    optimizerScales[6] =  1.0;
    optimizerScales[7] =  1.0;
    optimizerScales[8] =  1.0;
    optimizerScales[9] =  translationScale;
    optimizerScales[10] =  translationScale;
    optimizerScales[11] =  translationScale;

    optimizer->SetScales( optimizerScales );

    double steplength = 0.02;
    optimizer->SetMaximumStepLength( steplength ); 
    optimizer->SetMinimumStepLength( 0.0005 );

    unsigned int maxNumberOfIterations = 2000;
    optimizer->SetNumberOfIterations( maxNumberOfIterations );

    optimizer->MinimizeOn();


    metric->SetNumberOfHistogramBins( 50 );
  
    const unsigned int numberOfSamples = 
      static_cast<unsigned int>( fixImg->GetBufferedRegion().GetNumberOfPixels() / 20.0 );

    metric->SetNumberOfSpatialSamples( numberOfSamples );
    //metric->SetNumberOfSpatialSamples( 50000 );
    metric->ReinitializeSeed( 76926294 );
    metric->SetUseCachingOfBSplineWeights( true );


    try 
      { 
        registration->StartRegistration(); 
        //     std::cout << "Optimizer stop condition: "
        //               << registration->GetOptimizer()->GetStopConditionDescription()
        //               << std::endl;
      } 
    catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      } 

    finalCost = optimizer->GetValue();


    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

    typename transform_t::Pointer finalTransform = transform_t::New();
    finalTransform->SetParameters( finalParameters );
    finalTransform->SetFixedParameters( transform->GetFixedParameters() );

    typedef itk::ResampleImageFilter< moving_image_t, fix_image_t >    ResampleFilterType;

    // transform moving img1
    typename ResampleFilterType::Pointer resampler1 = ResampleFilterType::New();
    resampler1->SetTransform( finalTransform );
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
    resampler2->SetTransform( finalTransform );
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


} //namespace

#endif
