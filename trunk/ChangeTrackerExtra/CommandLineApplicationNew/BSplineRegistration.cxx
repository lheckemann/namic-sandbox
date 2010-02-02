/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration15.cxx,v $
  Language:  C++
  Date:      $Date: 2008-05-06 20:27:58 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


//
// This file was based on
// Insight/Examples/Registration/DeformableRegistration15.cxx
//

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"

#include "itkTimeProbesCollectorBase.h"

#ifdef ITK_USE_REVIEW
#include "itkMemoryProbesCollectorBase.h"
#define itkProbesCreate()  \
  itk::TimeProbesCollectorBase chronometer; \
  itk::MemoryProbesCollectorBase memorymeter
#define itkProbesStart( text ) memorymeter.Start( text ); chronometer.Start( text )
#define itkProbesStop( text )  chronometer.Stop( text ); memorymeter.Stop( text  )
#define itkProbesReport( stream )  chronometer.Report( stream ); memorymeter.Report( stream  )
#else
#define itkProbesCreate()  \
  itk::TimeProbesCollectorBase chronometer
#define itkProbesStart( text ) chronometer.Start( text )
#define itkProbesStop( text )  chronometer.Stop( text )
#define itkProbesReport( stream )  chronometer.Report( stream )
#endif


#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransform.h"
#include "itkAffineTransform.h"
#include "itkBSplineDeformableTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkBSplineResampleImageFunction.h"
#include "itkBSplineDecompositionImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"

#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

//  The following section of code implements a Command observer
//  used to monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << std::endl;
    }
};


int main( int argc, char *argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile outputImagefile deformationField" << std::endl;
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  
  const    unsigned int    ImageDimension = 3;
  typedef  signed short    PixelType;

  typedef itk::Image< PixelType, ImageDimension >  FixedImageType;
  typedef itk::Image< PixelType, ImageDimension >  MovingImageType;

  const unsigned int SpaceDimension = ImageDimension;

  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::VersorRigid3DTransform< double > RigidTransformType;

  typedef itk::AffineTransform< double, SpaceDimension > AffineTransformType;

  typedef itk::BSplineDeformableTransform<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     DeformableTransformType;

  typedef itk::CenteredTransformInitializer< RigidTransformType, 
                                             FixedImageType, 
                                             MovingImageType 
                                                 >  TransformInitializerType;


  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;


  typedef itk::MattesMutualInformationImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  // Auxiliary identity transform.
  typedef itk::IdentityTransform<double,SpaceDimension> IdentityTransformType;
  IdentityTransformType::Pointer identityTransform = IdentityTransformType::New();


  //
  //   Read the Fixed and Moving images.
  // 
  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  try
    {
    fixedImageReader->Update();
    movingImageReader->Update();
    }
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 

  FixedImageType::ConstPointer fixedImage = fixedImageReader->GetOutput();

  registration->SetFixedImage(  fixedImage   );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );

  //
  // Add a time and memory probes collector for profiling the computation time
  // of every stage.
  //
  itkProbesCreate();


  // 
  // Setup the metric parameters
  //
  metric->SetNumberOfHistogramBins( 50 );
  
  FixedImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
  
  const unsigned int numberOfPixels = fixedRegion.GetNumberOfPixels();

  metric->ReinitializeSeed( 76926294 );

  //
  //  Perform Deformable Registration
  // 
  DeformableTransformType::Pointer  bsplineTransformCoarse = DeformableTransformType::New();

  unsigned int numberOfGridNodesInOneDimensionCoarse = 5;

  typedef DeformableTransformType::RegionType RegionType;
  RegionType bsplineRegion;
  RegionType::SizeType   gridSizeOnImage;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;

  gridSizeOnImage.Fill( numberOfGridNodesInOneDimensionCoarse );
  gridBorderSize.Fill( SplineOrder );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef DeformableTransformType::SpacingType SpacingType;
  SpacingType spacing = fixedImage->GetSpacing();

  typedef DeformableTransformType::OriginType OriginType;
  OriginType origin = fixedImage->GetOrigin();;

  FixedImageType::SizeType fixedImageSize = fixedRegion.GetSize();

  for(unsigned int r=0; r<ImageDimension; r++)
    {
    spacing[r] *= static_cast<double>(fixedImageSize[r] - 1)  / 
                  static_cast<double>(gridSizeOnImage[r] - 1);
    }

  FixedImageType::DirectionType gridDirection = fixedImage->GetDirection();
  SpacingType gridOriginOffset = gridDirection * spacing;

  OriginType gridOrigin = origin - gridOriginOffset; 

  bsplineTransformCoarse->SetGridSpacing( spacing );
  bsplineTransformCoarse->SetGridOrigin( gridOrigin );
  bsplineTransformCoarse->SetGridRegion( bsplineRegion );
  bsplineTransformCoarse->SetGridDirection( gridDirection );
  
  bsplineTransformCoarse->SetBulkTransform( identityTransform );

  typedef DeformableTransformType::ParametersType     ParametersType;

  unsigned int numberOfBSplineParameters = bsplineTransformCoarse->GetNumberOfParameters();

  typedef OptimizerType::ScalesType       OptimizerScalesType;

  unsigned long numberOfSamples = numberOfPixels / 100;
  OptimizerScalesType optimizerScales = OptimizerScalesType( numberOfBSplineParameters );
  optimizerScales.Fill( 1.0 );

  optimizer->SetScales( optimizerScales );


  ParametersType initialDeformableTransformParameters( numberOfBSplineParameters );
  initialDeformableTransformParameters.Fill( 0.0 );

  bsplineTransformCoarse->SetParameters( initialDeformableTransformParameters );

  registration->SetInitialTransformParameters( bsplineTransformCoarse->GetParameters() );
  registration->SetTransform( bsplineTransformCoarse );

  optimizer->SetMaximumStepLength( 10.0 );
  optimizer->SetMinimumStepLength(  0.01 );

  optimizer->SetRelaxationFactor( 0.7 );
  optimizer->SetNumberOfIterations( 50 );

  //
  // The BSpline transform has a large number of parameters, we use therefore a
  // much larger number of samples to run this stage.
  //
  // Regulating the number of samples in the Metric is equivalent to performing
  // multi-resolution registration because it is indeed a sub-sampling of the
  // image.
  if( numberOfSamples < numberOfBSplineParameters * 100 )
    {
    numberOfSamples = numberOfBSplineParameters * 100;
    }
  std::cout << "Number of Samples = " << numberOfSamples << std::endl;
  metric->SetNumberOfSpatialSamples( numberOfSamples );

  std::cout << std::endl << "Starting Deformable Registration Coarse Grid" << std::endl;

  try 
    { 
    itkProbesStart( "Deformable Registration Coarse" );
    registration->StartRegistration(); 
    itkProbesStop( "Deformable Registration Coarse" );
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  
  std::cout << "Deformable Registration Coarse Grid completed" << std::endl;
  std::cout << std::endl;

  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  bsplineTransformCoarse->SetParameters( finalParameters );


  // Report the time and memory taken by the registration
  itkProbesReport( std::cout );

  finalParameters = registration->GetLastTransformParameters();

  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  resample->SetTransform( bsplineTransformCoarse );
  resample->SetInput( movingImageReader->GetOutput() );

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );

  // This value is set to zero in order to make easier to perform
  // regression testing in this example. However, for didactic 
  // exercise it will be better to set it to a medium gray value
  // such as 100 or 128.
  resample->SetDefaultPixelValue( 0 );
  
  typedef  signed short  OutputPixelType;

  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();


  writer->SetFileName( argv[3] );


  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );

  std::cout << "Writing resampled moving image...";

  try
    {
    writer->UseCompressionOn();
    writer->Update();
    }
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
 
  std::cout << " Done!" << std::endl;


  typedef itk::SquaredDifferenceImageFilter< 
                                  FixedImageType, 
                                  FixedImageType, 
                                  OutputImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( difference->GetOutput() );  
  

  // Compute the difference image between the 
  // fixed and resampled moving image.
  if( 1 )
    {
    difference->SetInput1( fixedImageReader->GetOutput() );
    difference->SetInput2( resample->GetOutput() );
    writer2->SetFileName( "diff_after.nrrd" );

    std::cout << "Writing difference image after registration...";

    try
      {
      writer2->UseCompressionOn();
      writer2->Update();
      }
    catch( itk::ExceptionObject & err ) 
      { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      return EXIT_FAILURE;
      } 

    std::cout << " Done!" << std::endl;
    }


  // Compute the difference image between the 
  // fixed and moving image before registration.
  if( 1 )
    {
    writer2->SetFileName( "diff_before.nrrd" );
    difference->SetInput1( fixedImageReader->GetOutput() );
    resample->SetTransform( identityTransform );

    std::cout << "Writing difference image before registration...";

    try
      {
      writer2->UseCompressionOn();
      writer2->Update();
      }
    catch( itk::ExceptionObject & err ) 
      { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      return EXIT_FAILURE;
      } 

    std::cout << " Done!" << std::endl;
    }



  // Generate the explicit deformation field resulting from 
  // the registration.
  if( 1 )
    {

    typedef itk::Vector< float, ImageDimension >  VectorType;
    typedef itk::Image< VectorType, ImageDimension >  DeformationFieldType;

    DeformationFieldType::Pointer field = DeformationFieldType::New();
    field->SetRegions( fixedRegion );
    field->SetOrigin( fixedImage->GetOrigin() );
    field->SetSpacing( fixedImage->GetSpacing() );
    field->SetDirection( fixedImage->GetDirection() );
    field->Allocate();

    typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
    FieldIterator fi( field, fixedRegion );

    fi.GoToBegin();

    DeformableTransformType::InputPointType  fixedPoint;
    DeformableTransformType::OutputPointType movingPoint;
    DeformationFieldType::IndexType index;

    VectorType displacement;

    while( ! fi.IsAtEnd() )
      {
      index = fi.GetIndex();
      field->TransformIndexToPhysicalPoint( index, fixedPoint );
      movingPoint = bsplineTransformCoarse->TransformPoint( fixedPoint );
      displacement = movingPoint - fixedPoint;
      fi.Set( displacement );
      ++fi;
      }

    typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
    FieldWriterType::Pointer fieldWriter = FieldWriterType::New();

    fieldWriter->SetInput( field );

    fieldWriter->SetFileName( argv[4] );

    std::cout << "Writing deformation field ...";

    try
      {
      fieldWriter->UseCompressionOn();
      fieldWriter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }

    std::cout << " Done!" << std::endl;
    }
  
  return EXIT_SUCCESS;
}

#undef itkProbesCreate
#undef itkProbesStart
#undef itkProbesStop
#undef itkProbesReport

