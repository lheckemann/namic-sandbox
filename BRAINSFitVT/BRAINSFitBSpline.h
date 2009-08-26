#ifndef __BRAINSFitBSpline_H_
#define __BRAINSFitBSpline_H_
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkBSplineDeformableTransform.h>
#include "itkBSplineDeformableTransformInitializer.h"
#include <itkLBFGSBOptimizer.h>
#include <itkTimeProbesCollectorBase.h>
#include <itkImageRegistrationMethod.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

/**
 * This class is the BSpline component of the BRAINSFit program developed at the University of Iowa.
 * The master version of this file is always located from the nitric site.
 * http://www.nitrc.org/projects/multimodereg/
 */

template < class RegisterImageType, class ImageMaskType, class BSplineTransformType >
typename BSplineTransformType::Pointer
  DoBSpline( typename BSplineTransformType::Pointer InitializerBsplineTransform,
  typename RegisterImageType::Pointer m_FixedVolume, typename RegisterImageType::Pointer m_MovingVolume,
  typename ImageMaskType::Pointer m_FixedMask,           typename ImageMaskType::Pointer m_MovingMask,
  const int m_NumberOfSamples,
  const bool m_UseCachingOfBSplineWeights, const bool m_UseExplicitPDFDerivatives)
{
  /*
   *  Begin straightline BSpline optimization, after GTRACT/Common/itkAnatomicalBSplineFilter.
   */

  typedef typename RegisterImageType::Pointer         RegisterImagePointer;
  typedef typename RegisterImageType::ConstPointer    RegisterImageConstPointer;
  typedef typename RegisterImageType::RegionType      RegisterImageRegionType;
  typedef typename RegisterImageType::SizeType        RegisterImageSizeType;
  typedef typename RegisterImageType::SpacingType     RegisterImageSpacingType;
  typedef typename RegisterImageType::PointType       RegisterImagePointType;
  typedef typename RegisterImageType::PixelType       RegisterImagePixelType;
  typedef typename RegisterImageType::DirectionType   RegisterImageDirectionType;
  typedef typename RegisterImageType::IndexType       RegisterImageIndexType;

  typedef typename BSplineTransformType::RegionType                TransformRegionType;
  typedef typename TransformRegionType::SizeType                   TransformSizeType;
  typedef typename BSplineTransformType::SpacingType               TransformSpacingType;
  typedef typename BSplineTransformType::OriginType                TransformOriginType;
  typedef typename BSplineTransformType::DirectionType             TransformDirectionType;
  typedef typename BSplineTransformType::ParametersType            TransformParametersType;

  typedef typename itk::LBFGSBOptimizer OptimizerType;

  typedef typename itk::MattesMutualInformationImageToImageMetric<
    RegisterImageType,
    RegisterImageType >          MetricType;

  typedef typename itk:: LinearInterpolateImageFunction<
    RegisterImageType,
    double          >          InterpolatorType;

  typedef typename itk::ImageRegistrationMethod<
    RegisterImageType,
    RegisterImageType >          RegistrationType;

  typedef typename BSplineTransformType::Pointer         TransformTypePointer;
  typedef typename MetricType::Pointer                   MetricTypePointer;
  typedef typename OptimizerType::Pointer                OptimizerTypePointer;
  typedef typename OptimizerType::ParametersType         OptimizerParameterType;
  typedef typename OptimizerType::ScalesType             OptimizerScalesType;
  typedef typename OptimizerType::BoundSelectionType     OptimizerBoundSelectionType;
  typedef typename OptimizerType::BoundValueType         OptimizerBoundValueType;

  typedef typename InterpolatorType::Pointer             InterpolatorTypePointer;
  typedef typename RegistrationType::Pointer             RegistrationTypePointer;

  typedef typename itk::ResampleImageFilter<
    RegisterImageType,
    RegisterImageType>     ResampleFilterType;




  //  int m_SpatialSampleScale = 100;
  int m_MaximumNumberOfIterations = 500;
  int m_MaximumNumberOfEvaluations = 500;
  int m_MaximumNumberOfCorrections = 12;
  int m_BSplineHistogramBins = 50;
  float m_CostFunctionConvergenceFactor = 1e+7;
  float m_ProjectedGradientTolerance = 1e-4;

  MetricTypePointer         metric        = MetricType::New();
  OptimizerTypePointer      optimizer     = OptimizerType::New();
  InterpolatorTypePointer   interpolator  = InterpolatorType::New();
  RegistrationTypePointer   registration  = RegistrationType::New();

  // set the masks on the metric
  if ( m_FixedMask.IsNotNull() )
    {
    metric->SetFixedImageMask(m_FixedMask);
    }

  if ( m_MovingMask.IsNotNull() )
    {
    metric->SetMovingImageMask(m_MovingMask);
    }

  metric->SetUseCachingOfBSplineWeights( m_UseCachingOfBSplineWeights );
  metric->SetUseExplicitPDFDerivatives( m_UseExplicitPDFDerivatives );

  typename BSplineTransformType::Pointer m_OutputBSplineTransform=BSplineTransformType::New();
  m_OutputBSplineTransform->SetIdentity();
  m_OutputBSplineTransform->SetBulkTransform( InitializerBsplineTransform->GetBulkTransform() );
  m_OutputBSplineTransform->SetFixedParameters( InitializerBsplineTransform->GetFixedParameters() );
  m_OutputBSplineTransform->SetParametersByValue( InitializerBsplineTransform->GetParameters() );

  /*** Set up the Registration ***/
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );
  registration->SetTransform( m_OutputBSplineTransform );

  /*** Setup the Registration ***/
  registration->SetFixedImage(    m_FixedVolume   );
  registration->SetMovingImage(   m_MovingVolume   );


  RegisterImageRegionType fixedImageRegion = m_FixedVolume->GetBufferedRegion();

  registration->SetFixedImageRegion( fixedImageRegion );

  const unsigned int numberOfParameters =
    m_OutputBSplineTransform->GetNumberOfParameters();

  TransformParametersType parameters( numberOfParameters );

  parameters.Fill( 0.0 );

  m_OutputBSplineTransform->SetParameters( parameters );

  registration->SetInitialTransformParameters( m_OutputBSplineTransform->GetParameters() );


  OptimizerBoundSelectionType boundSelect( m_OutputBSplineTransform->GetNumberOfParameters() );
  OptimizerBoundValueType     upperBound( m_OutputBSplineTransform->GetNumberOfParameters() );
  OptimizerBoundValueType     lowerBound( m_OutputBSplineTransform->GetNumberOfParameters() );

  boundSelect.Fill( 0 );
  upperBound.Fill( 0.0 );
  lowerBound.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( m_CostFunctionConvergenceFactor );
  optimizer->SetProjectedGradientTolerance( m_ProjectedGradientTolerance );
  optimizer->SetMaximumNumberOfIterations( m_MaximumNumberOfIterations );
  optimizer->SetMaximumNumberOfEvaluations( m_MaximumNumberOfEvaluations );
  optimizer->SetMaximumNumberOfCorrections( m_MaximumNumberOfCorrections );

  metric->SetNumberOfHistogramBins( m_BSplineHistogramBins );

  metric->SetNumberOfSpatialSamples( m_NumberOfSamples );
  metric->ReinitializeSeed( 76926294 );


  /********************** Now start the execute function ************************/
  // Add a time probe
  itk::TimeProbesCollectorBase collector;

  std::cout << std::endl << "Starting Registration" << std::endl;

  try
    {
    collector.Start( "Registration" );
    registration->StartRegistration();
    collector.Stop( "Registration" );
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return NULL;
    }

  OptimizerType::ParametersType finalParameters =
    registration->GetLastTransformParameters();

//  std::cerr << "************* !!!!!!!!!! ***********" << 
//    "Stop condition: " << optimizer->GetStopCondition() << std::endl;
  collector.Report();

  /* This call is required to copy the parameters */
  m_OutputBSplineTransform->SetParametersByValue( finalParameters );
  return m_OutputBSplineTransform;
}

#endif //__BRAINSFitBSpline_H_
