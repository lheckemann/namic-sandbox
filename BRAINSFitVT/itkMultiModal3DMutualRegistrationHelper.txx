/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date: 2007-08-02 14:58:12 -0500 (Thu, 02 Aug 2007) $
Version:   $Revision: 10282 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMultiModal3DMutualRegistrationHelper_txx
#define _itkMultiModal3DMutualRegistrationHelper_txx

#include "itkMultiModal3DMutualRegistrationHelper.h"

#include "itkVersor.h"
#include "itkMatrix.h"
#include "ConvertToRigidAffine.h"

#include <itkWindowedSincInterpolateImageFunction.h>

namespace itk
{
/*
 * Constructor
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::MultiModal3DMutualRegistrationHelper() :
  m_FixedImage(                       0 ),         // has to be provided by the
                                                   // user.
  m_MovingImage(                      0 ),         // has to be provided by the
                                                   // user.
  m_InitialTransform(                 0 ),         // has to be provided by the
                                                   // user.
  m_Transform(                        0 ),         // has to be provided by
                                                   // this->Initialize().
  m_Registration(                     0 ),         // has to be provided by
                                                   // this->Initialize().
  m_PermissionToVaryGuide(            1 ),
  m_NumberOfSpatialSamples(      100000 ),
  m_NumberOfHistogramBins(          200 ),
  m_NumberOfIterations(               0 ),
  m_RelaxationFactor(               0.5 ),
  m_MaximumStepLength(           0.2000 ),
  m_MinimumStepLength(           0.0001 ),
  m_TranslationScale(            1000.0 ),
  m_ReproportionScale(             25.0 ),
  m_SkewScale(                     25.0 ),
  m_InitialTransformPassThruFlag( false ),
  m_UseExplicitPDFDerivativesFlag( true ),
  m_BackgroundFillValue(            0.0 )
  {
  this->SetNumberOfRequiredOutputs( 1 );  // for the Transform

  TransformOutputPointer transformDecorator
    = static_cast<TransformOutputType *>(
    this->MakeOutput(0).GetPointer() );

  this->ProcessObject::SetNthOutput( 0, transformDecorator.GetPointer() );
  }

/*
 * Initialize by setting the interconnects between components.
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::Initialize(const int NumParam) throw (ExceptionObject)
  {
  if ( !m_FixedImage )
    {
    itkExceptionMacro(<< "FixedImage is not present");
    }

  if ( !m_MovingImage )
    {
    itkExceptionMacro(<< "MovingImage is not present");
    }

  m_NumberOfOptimizationParameters = NumParam;

  this->SetTransform( TransformType::New() );
  m_Transform->SetIdentity();
  
  //
  // Connect the transform to the Decorator.
  //
  TransformOutputType *transformOutput
    = static_cast<TransformOutputType *>( this->ProcessObject::GetOutput(0) );

  transformOutput->Set( m_Transform.GetPointer() );

  typename MetricType::Pointer       metric         = MetricType::New();
  typename OptimizerType::Pointer    optimizer      = OptimizerType::New();
  typename InterpolatorType::Pointer interpolator   = InterpolatorType::New();

  optimizer->SetMaximize( false );       // Mutual Information metrics are to be
                                         // minimized.

  metric->SetNumberOfHistogramBins( m_NumberOfHistogramBins );
  metric->SetNumberOfSpatialSamples( m_NumberOfSpatialSamples );
  // metric->SetUseAllPixels(true);  //DEBUG -- This was way too slow.
  // metric->SetUseExplicitPDFDerivatives(true);  //DEBUG -- Just test set to
  // false saves memory at expense of runtime

  //
  // set the masks on the metric
  if ( this->m_FixedBinaryVolume.IsNotNull() )
    {
    metric->SetFixedImageMask(this->m_FixedBinaryVolume);
    }

  if ( this->m_MovingBinaryVolume.IsNotNull() )
    {
    metric->SetMovingImageMask(this->m_MovingBinaryVolume);
    }

  if ( m_UseExplicitPDFDerivativesFlag == false )
    {
    metric->SetUseExplicitPDFDerivatives( false ); // an approximation only
    }
  else
    {
    metric->SetUseExplicitPDFDerivatives( true ); // the default
    }


  m_Registration = RegistrationType::New();

  m_Registration->SetMetric(       metric       );
  m_Registration->SetOptimizer(    optimizer    );
  m_Registration->SetInterpolator( interpolator );

  m_Registration->SetTransform(   m_Transform   );
  m_Registration->SetFixedImage(  m_FixedImage  );
  m_Registration->SetMovingImage( m_MovingImage );

  m_Registration->SetFixedImageRegion( m_FixedImage->GetLargestPossibleRegion() );

  // Decode permissionToVary from its initial segment, PermissionToVaryGuide.
  std::vector<int> permissionToVary( NumParam );
  for ( int i = 0; i < NumParam; i++ )
    {
    permissionToVary[i] = 1; // default Yes, this may vary.
    }
  for ( int i = 0; i < NumParam  &&  i < static_cast<int>(m_PermissionToVaryGuide.size()); i++ )
    {
    if (m_PermissionToVaryGuide[i] >= 0)
      {
      permissionToVary[i] = m_PermissionToVaryGuide[i];
      }
    }

  if ( m_InitialTransform )
    {
    // TODO: There should be no need to convert here, just assign m_Transform
    // from m_InitialTransform.
    //      They should be the same type!
    AssignRigid::AssignConvertedTransform(m_Transform, m_InitialTransform);
    
    // No need to step on parameters that may not vary; they will remain identical with 
    // values from InitialTransform which defaults correctly to SetIdentity().
    }
  else      // Won't happen under BRAINSFitPrimary.
    {
    std::cout
     << "FAILURE:  InitialTransform must be set in MultiModal3DMutualRegistrationHelper before Initialize is called."
     << std::endl;
    exit(-1);
    //  m_Transform would be SetIdentity() if this case continued.
    }

  //  We now pass the parameters of the current transform as the initial
  //  parameters to be used when the registration process starts.
  m_Registration->SetInitialTransformParameters( m_Transform->GetParameters() );

  const double translationScale  = 1.0 / m_TranslationScale;
  const double reproportionScale = 1.0 / m_ReproportionScale;
  const double skewScale         = 1.0 / m_SkewScale;

  OptimizerScalesType optimizerScales( NumParam );


  if ( NumParam == 15 )    //  ScaleSkew
    {
    for ( int i = 0; i < NumParam; i++ )
      {
      optimizerScales[i] = 1.0;
      }
    for ( int i = 3; i < 6; i++ )
      {
      optimizerScales[i] = translationScale;
      }
    for ( int i = 6; i < 9; i++ )
      {
      optimizerScales[i] = reproportionScale;
      }
    for ( int i = 9; i < 15; i++ )
      {
      optimizerScales[i] = skewScale;
      }
    }
  else if ( NumParam == 12 )    //  Affine
    {
    for ( int i = 0; i < 9; i++ )
      {
      optimizerScales[i] = 1.0;
      }
    for ( int i = 9; i < 12; i++ )
      {
      optimizerScales[i] = translationScale;
      }
    }
  else if ( NumParam == 9 )    //  ScaleVersorRigid3D
    {
    for ( int i = 0; i < 3; i++ )
      {
      optimizerScales[i] = 1.0;
      }
    for ( int i = 3; i < 6; i++ )
      {
      optimizerScales[i] = translationScale;
      }
    for ( int i = 6; i < 9; i++ )
      {
      optimizerScales[i] = reproportionScale;
      }
    }
  else if ( NumParam == 6 )    //  VersorRigid3D
    {
    for ( int i = 0; i < 3; i++ )
      {
      optimizerScales[i] = 1.0;
      }
    for ( int i = 3; i < 6; i++ )
      {
      optimizerScales[i] = translationScale;
      }
    }
  else     // most likely (NumParam == 3): uniform parameter scaling, whether
           // just rotating OR just translating.
    {
    for ( int i = 0; i < NumParam; i++ )
      {
      optimizerScales[i] = 1.0;
      }
    }

  // Step on parameters that may not vary; they also must be identical with SetIdentity().
  for ( int i = 0; i < NumParam; i++ )
    {
    if (permissionToVary[i] == 0)
      {
      optimizerScales[i] = vcl_numeric_limits<float>::max();
      }
    }

  std::cout << "Initializer, optimizerScales: " << optimizerScales << "."
            << std::endl;
  optimizer->SetScales( optimizerScales );

  optimizer->SetRelaxationFactor(   m_RelaxationFactor   );
  optimizer->SetMaximumStepLength(  m_MaximumStepLength  );
  optimizer->SetMinimumStepLength(  m_MinimumStepLength  );
  optimizer->SetNumberOfIterations( m_NumberOfIterations );

  // if (globalVerbose)
  if ( 0 )
    {
    std::cout << "Initializer, RelaxationFactor: " << m_RelaxationFactor
              << "." << std::endl;
    std::cout << "Initializer, MaximumStepLength: " << m_MaximumStepLength
              << "." << std::endl;
    std::cout << "Initializer, MinimumStepLength: " << m_MinimumStepLength
              << "." << std::endl;
    std::cout << "Initializer, NumberOfIterations: " << m_NumberOfIterations
              << "." << std::endl;
    std::cout << "Registration, Transform : " << m_Transform << "."
              << std::endl;
    std::cout << "Registration, FixedImage : " << m_FixedImage << "."
              << std::endl;
    std::cout << "Registration, MovingImage : " << m_MovingImage << "."
              << std::endl;
    }

  // Create the Command observer and register it with the optimizer.
  // TODO:  make this output optional.
  //
  typedef CommandIterationUpdate<TOptimizer> CommandIterationUpdateType;
  typename CommandIterationUpdateType::Pointer observer
    = CommandIterationUpdateType::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  }

/*
 * Starts the Registration Process
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::StartRegistration( void )
{
  if ( !m_InitialTransformPassThruFlag )
    {
    bool     successful = false;
    unsigned diff = this->m_NumberOfSpatialSamples / 10;
    while ( !successful )
      {
      try
        {
        m_Registration->StartRegistration();
        successful = true;
        }
      catch( itk::ExceptionObject & err )
        {
        // std::cerr << "ExceptionObject caught !" << std::endl;
        // std::cerr << err << std::endl;
        // Pass exception to caller
        //        throw err;
        //
        // lower the number of samples you request
        if ( diff > this->m_NumberOfSpatialSamples )
          {
          // we are done...
          throw err;
          }
        this->m_NumberOfSpatialSamples -= diff;
        typename MetricType::Pointer metric = dynamic_cast<MetricType *>(
          this->m_Registration->GetMetric() );
        if ( metric.IsNull() )
          {
          std::cout << "ERROR::" << __FILE__ << " " << __LINE__ << std::endl;
          exit(-1);
          }
        metric->SetNumberOfSpatialSamples(this->m_NumberOfSpatialSamples);
        }
      }

    typedef typename OptimizerType::ParametersType OptimizerParametersType;
    OptimizerParametersType finalParameters(m_NumberOfOptimizationParameters);

    finalParameters = m_Registration->GetLastTransformParameters();

    OptimizerPointer optimizer
      = dynamic_cast<OptimizerPointer>( m_Registration->GetOptimizer() );
    std::cerr << " *************** !!!!!! ********** " <<
      "Stop condition: " << optimizer->GetStopCondition() << std::endl;
    m_ActualNumberOfIterations = optimizer->GetCurrentIteration();
    m_Transform->SetParameters( finalParameters );
    }

  typename TransformType::MatrixType matrix = m_Transform->GetMatrix();
  typename TransformType::OffsetType offset = m_Transform->GetOffset();

  std::cout << std::endl << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << offset << std::endl << std::endl;
}

/**
 *
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
unsigned long
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::GetMTime() const
{
  unsigned long mtime = Superclass::GetMTime();
  unsigned long m;

  // Some of the following should be removed once ivars are put in the
  // input and output lists

  if ( m_Transform )
    {
    m = m_Transform->GetMTime();
    mtime = ( m > mtime ? m : mtime );
    }

  if ( m_Registration )
    {
    m = m_Registration->GetMTime();
    mtime = ( m > mtime ? m : mtime );
    }

  if ( m_InitialTransform )
    {
    m = m_InitialTransform->GetMTime();
    mtime = ( m > mtime ? m : mtime );
    }

  if ( m_FixedImage )
    {
    m = m_FixedImage->GetMTime();
    mtime = ( m > mtime ? m : mtime );
    }

  if ( m_MovingImage )
    {
    m = m_MovingImage->GetMTime();
    mtime = ( m > mtime ? m : mtime );
    }

  return mtime;
}

/*
 * Generate Data
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::GenerateData()
{
  this->StartRegistration();
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::SetFixedImage( FixedImagePointer fixedImage )
{
  itkDebugMacro("setting Fixed Image to " << fixedImage );

  if ( this->m_FixedImage.GetPointer() != fixedImage )
    {
    this->m_FixedImage = fixedImage;
    this->ProcessObject::SetNthInput(0, this->m_FixedImage);
    this->Modified();
    }
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::SetMovingImage( MovingImagePointer movingImage )
{
  itkDebugMacro("setting Moving Image to " << movingImage );

  if ( this->m_MovingImage.GetPointer() != movingImage )
    {
    this->m_MovingImage = movingImage;
    this->ProcessObject::SetNthInput(1, this->m_MovingImage);
    this->Modified();
    }
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::SetInitialTransform( typename TransformType::Pointer initialTransform )
{
  itkDebugMacro("setting Initial Transform to " << initialTransform );
  if ( this->m_InitialTransform.GetPointer() != initialTransform )
    {
    this->m_InitialTransform = initialTransform;
    this->Modified();
    }
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
typename MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer,
  TFixedImage, TMovingImage>::FixedImageType::Pointer
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::GetResampledImage(InterpolationType newInterp) const
{
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform( m_Transform );
  resampler->SetInput( m_MovingImage );         // Remember:  the Data is
                                                // Moving's, the shape is
                                                // Fixed's.
  resampler->SetOutputParametersFromImage(m_FixedImage);
  resampler->SetDefaultPixelValue( m_BackgroundFillValue );
  if ( newInterp == WINDOWSINC_INTERP )
    {
    typedef itk::ConstantBoundaryCondition<MovingImageType>
    BoundaryConditionType;
    static const unsigned int WindowedSincHammingWindowRadius = 5;
    typedef itk::Function::HammingWindowFunction<
      WindowedSincHammingWindowRadius, double, double> WindowFunctionType;
    typedef itk::WindowedSincInterpolateImageFunction<
      MovingImageType,
      WindowedSincHammingWindowRadius,
      WindowFunctionType,
      BoundaryConditionType,
      double>    WindowedSincInterpolatorType;

    typename WindowedSincInterpolatorType::Pointer interpolator
      = WindowedSincInterpolatorType::New();
    resampler->SetInterpolator( interpolator );
    }
  else     // Default to LINEAR_INTERP
    {
    // NOTE: Linear is the default.  resampler->SetInterpolator( interpolator );
    }
  resampler->Update();       //  Explicit Update() required here.
  typename TFixedImage::Pointer ResampledImage = resampler->GetOutput();

  return ResampledImage;
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
typename MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer,
  TFixedImage, TMovingImage>::TransformType::Pointer
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::GetTransform( void )
{
  this->Update();
  return m_Transform;
}

/*
 *  Get Output
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
const typename MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer,
  TFixedImage, TMovingImage>::TransformOutputType *
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::GetOutput() const
{
  return static_cast<const TransformOutputType *>(
           this->ProcessObject::GetOutput(0) );
}

template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
DataObject::Pointer
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::MakeOutput(unsigned int output)
{
  switch ( output )
    {
    case 0:
      return static_cast<DataObject *>( TransformOutputType::New().GetPointer() );
      break;
    default:
      itkExceptionMacro(
      "MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
    }
}

/*
 * PrintSelf
 */
template<typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
void
MultiModal3DMutualRegistrationHelper<TTransformType, TOptimizer, TFixedImage,
  TMovingImage>
  ::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Transform: " << m_Transform.GetPointer() << std::endl;
  os << indent << "Fixed Image: " << m_FixedImage.GetPointer() << std::endl;
  os << indent << "Moving Image: " << m_MovingImage.GetPointer() << std::endl;
}
} // end namespace itk

#endif
