#ifndef reg_3d_similarity_mse_hpp_
#define reg_3d_similarity_mse_hpp_



#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"

#include "itkSimilarity3DTransform.h"

#include "itkCenteredTransformInitializer.h"

#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"


namespace newProstate
{
  template<typename image_t>
  double getVol(typename image_t::Pointer img, typename image_t::PixelType thld = 0)
  {
    typedef itk::ImageRegionConstIterator<image_t> ImageRegionConstIterator_t;
    ImageRegionConstIterator_t it(img, img->GetLargestPossibleRegion() );

    double cell = (img->GetSpacing()[0])*(img->GetSpacing()[1])*(img->GetSpacing()[2]);

    double v = 0.0;

    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
        typename image_t::PixelType f = it.Get();

        v += (f>thld?cell:0.0);
      }

    return v;
  }


  template<typename fix_image_t, typename moving_image_t, typename output_image_t>
  typename output_image_t::Pointer
  reg_3d_similarity_mse(typename fix_image_t::Pointer fixImg, \
                        typename moving_image_t::Pointer movingImg, \
                        typename moving_image_t::PixelType fillInVal)
  {
    typedef itk::Similarity3DTransform< double > TransformType;
    typedef TransformType::ParametersType ParametersType;

    typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;


    typedef itk::MeanSquaresImageToImageMetric< fix_image_t, moving_image_t >    MetricType;

    typedef itk:: LinearInterpolateImageFunction< moving_image_t, double >    InterpolatorType;

    typedef itk::ImageRegistrationMethod< fix_image_t, moving_image_t >    RegistrationType;

    typename MetricType::Pointer         metric        = MetricType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
  

    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );


    TransformType::Pointer  transform = TransformType::New();
    registration->SetTransform( transform );


    registration->SetFixedImage(    fixImg    );
    registration->SetMovingImage(   movingImg   );

    registration->SetFixedImageRegion( fixImg->GetBufferedRegion() );


    typedef itk::CenteredTransformInitializer< TransformType, fix_image_t, moving_image_t >  TransformInitializerType;

    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    initializer->SetTransform(   transform );
    initializer->SetFixedImage(  fixImg );
    initializer->SetMovingImage( movingImg );


    initializer->MomentsOn();
    initializer->InitializeTransform();

    //tst
    //   double vf = getVol<fix_image_t>(fixImg);
    //   double vm = getVol<moving_image_t>(movingImg);
    //   std::cout<<"vols = "<<vf<<"\t"<<vm<<std::endl;
    //   //double r = pow(vm/vf, 1/3.0);
    //   double r = 0.5;

    //   OptimizerType::ParametersType initParameters = transform->GetParameters();
    //   initParameters[6] = r;
    //   transform->SetParameters(initParameters);

    //std::cout<<transform->GetParameters()<<std::endl;
    //tst//

    registration->SetInitialTransformParameters( transform->GetParameters() );


    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

    assert(transform->GetNumberOfParameters() == 7);


    const double translationScale = 1.0 / 1000.0;

    optimizerScales[0] = 1.0; // versor.x
    optimizerScales[1] = 1.0; // versor.y
    optimizerScales[2] = 1.0; // versor.z
    optimizerScales[3] = translationScale; // translation.x
    optimizerScales[4] = translationScale; // translation.y
    optimizerScales[5] = translationScale; // translation.z
    optimizerScales[6] = 100; // scale

    optimizer->SetScales( optimizerScales );

    optimizer->SetMaximumStepLength( 0.0200  ); 
    optimizer->SetMinimumStepLength( 0.0001 );

    optimizer->SetNumberOfIterations( 1000 );



    try 
      { 
        registration->StartRegistration(); 
        std::cout << "Optimizer stop condition: "                         \
                  << registration->GetOptimizer()->GetStopConditionDescription() \
                  << std::endl;
      } 
    catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      } 
  
    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();


    //   const double versorX              = finalParameters[0];
    //   const double versorY              = finalParameters[1];
    //   const double versorZ              = finalParameters[2];
    //   const double finalTranslationX    = finalParameters[3];
    //   const double finalTranslationY    = finalParameters[4];
    //   const double finalTranslationZ    = finalParameters[5];

    //   const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

    //   const double bestValue = optimizer->GetValue();


    //   // Print out results
    //   //
    //   std::cout << std::endl << std::endl;
    //   std::cout << "Result = " << std::endl;
    //   std::cout << " versor X      = " << versorX  << std::endl;
    //   std::cout << " versor Y      = " << versorY  << std::endl;
    //   std::cout << " versor Z      = " << versorZ  << std::endl;
    //   std::cout << " Translation X = " << finalTranslationX  << std::endl;
    //   std::cout << " Translation Y = " << finalTranslationY  << std::endl;
    //   std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
    //   std::cout << " Iterations    = " << numberOfIterations << std::endl;
    //   std::cout << " Metric value  = " << bestValue          << std::endl;



    //   transform->SetParameters( finalParameters );

    //   TransformType::MatrixType matrix = transform->GetRotationMatrix();
    //   TransformType::OffsetType offset = transform->GetOffset();

    //   std::cout << "Matrix = " << std::endl << matrix << std::endl;
    //   std::cout << "Offset = " << std::endl << offset << std::endl;


    typedef itk::ResampleImageFilter< moving_image_t, fix_image_t >    ResampleFilterType;

    TransformType::Pointer finalTransform = TransformType::New();

    finalTransform->SetCenter( transform->GetCenter() );

    finalTransform->SetParameters( finalParameters );
    finalTransform->SetFixedParameters( transform->GetFixedParameters() );

    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetTransform( finalTransform );
    resampler->SetInput( movingImg );

    resampler->SetSize(    fixImg->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  fixImg->GetOrigin() );
    resampler->SetOutputSpacing( fixImg->GetSpacing() );
    resampler->SetOutputDirection( fixImg->GetDirection() );
    resampler->SetDefaultPixelValue( fillInVal );
  

    typedef itk::CastImageFilter< fix_image_t, output_image_t > CastFilterType;

    typename CastFilterType::Pointer  caster =  CastFilterType::New();

    caster->SetInput( resampler->GetOutput() );
    caster->Update();
  
    return caster->GetOutput();

  }

}

#endif


