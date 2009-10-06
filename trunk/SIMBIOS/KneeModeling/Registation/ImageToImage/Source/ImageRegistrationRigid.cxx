/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ShapeDetectionLevelSetFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-17 21:44:43 $
  Version:   $Revision: 1.41 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransform.h"

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkCenteredTransformInitializer.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkTransformFileWriter.h"

#include "itkResampleImageFilter.h"

#include "vtkRegistrationMonitor.h"
#include "vtkKWImage.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"

#include "itkCommandIterationUpdate.h"

#include <itksys/SystemTools.hxx>


int main( int argc, char *argv[] )
{
  if( argc < 8 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " outputImagefile differenceAfterRegistration screenshotsDirectory";
    std::cerr << " outputInitialTransformFile outputFinalTransformFile";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  
  const    unsigned int    Dimension = 3;
  typedef  signed short    PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::VersorRigid3DTransform< double >  RigidTransformType;
  typedef itk::VersorRigid3DTransformOptimizer   OptimizerType;

  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer           metric          = MetricType::New();
  OptimizerType::Pointer        optimizer       = OptimizerType::New();
  InterpolatorType::Pointer     interpolator    = InterpolatorType::New();
  RegistrationType::Pointer     registration    = RegistrationType::New();
  RigidTransformType::Pointer   rigidTransform  = RigidTransformType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );
  registration->SetTransform( rigidTransform );


 
  const PixelType   maskThreshold = 0;
  metric->SetFixedImageSamplesIntensityThreshold( maskThreshold );


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
 
  const FixedImageType *  fixedImage = fixedImageReader->GetOutput();
  const MovingImageType * movingImage = movingImageReader->GetOutput();

  registration->SetFixedImage(    fixedImage  );
  registration->SetMovingImage(   movingImage );

  rigidTransform->SetIdentity();

  typedef itk::CenteredTransformInitializer< 
                                    RigidTransformType, 
                                    FixedImageType, 
                                    MovingImageType >  TransformInitializerType;

  TransformInitializerType::Pointer initializer = TransformInitializerType::New();

  FixedImageType::RegionType fixedImageRegion = fixedImage->GetBufferedRegion();

  registration->SetFixedImageRegion( fixedImageRegion );

  initializer->SetTransform(   rigidTransform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );
  initializer->GeometryOn();

  std::cout << "Initializing Transform" << std::endl;
  initializer->InitializeTransform();

  std::cout << "Initialized Rigid Transform" << std::endl;
  rigidTransform->Print( std::cout );
  
  registration->SetInitialTransformParameters( rigidTransform->GetParameters() );


  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( rigidTransform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 10000.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;

  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;

  optimizer->SetScales( optimizerScales );

  optimizer->MinimizeOn();

  optimizer->SetMaximumStepLength( 1.0000  ); 
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetRelaxationFactor( 0.9 );

  optimizer->SetNumberOfIterations( 30 );

  typedef itk::CommandIterationUpdate<  OptimizerType >    CommandIterationType;

  CommandIterationType::Pointer observer = CommandIterationType::New();

  observer->SetOptimizer( optimizer );

  //------------------------------------------------------------
  //  Configure VTK visualization
  //------------------------------------------------------------
  std::cout << "Configuring Visualization" << std::endl;

  vtkRegistrationMonitor monitor;

  vtkSmartPointer< vtkKWImage > fixedKWImage  = vtkSmartPointer< vtkKWImage >::New();
  vtkSmartPointer< vtkKWImage > movingKWImage   = vtkSmartPointer< vtkKWImage >::New();

  fixedKWImage->SetITKImageBase( const_cast<FixedImageType *>( fixedImage ) );
  movingKWImage->SetITKImageBase( const_cast<MovingImageType *>( movingImage ) );

  vtkSmartPointer< vtkContourFilter > fixedContour  = vtkSmartPointer< vtkContourFilter >::New();
  vtkSmartPointer< vtkContourFilter > movingContour = vtkSmartPointer< vtkContourFilter >::New();

  fixedContour->SetInput( fixedKWImage->GetVTKImage() );
  movingContour->SetInput( movingKWImage->GetVTKImage() );

  fixedContour->SetValue(  0, 50.0 ); // level for iso-contour
  movingContour->SetValue( 0, 50.0 ); // level for iso-contour

  monitor.SetFixedSurface( fixedContour->GetOutput() );
  monitor.SetMovingSurface( movingContour->GetOutput() );

  monitor.SetNumberOfIterationPerUpdate( 1 );

  std::string screenshotDirectory = argv[5];
  itksys::SystemTools::MakeDirectory( screenshotDirectory.c_str() );

  std::cout <<  screenshotDirectory << std::endl; 

  monitor.SetScreenshotOutputDirectory( screenshotDirectory.c_str() );
  monitor.SetScreenshotBaseName( "registrationScreenshot" );

  monitor.Observe( optimizer, rigidTransform );

  typedef itk::TransformFileWriter     TransformWriterType;

  TransformWriterType::Pointer transformWriter = TransformWriterType::New();

  transformWriter->SetFileName( argv[6] );
  transformWriter->SetInput( rigidTransform );
  transformWriter->Update();

  std::cout << "Starting Registration " << std::endl;

  try 
    { 
    registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  

  std::cout << "Number of Pixels Used = ";
  std::cout << metric->GetNumberOfPixelsCounted() << std::endl;

  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();


  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  std::cout << std::endl << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " Final Parameters = " << finalParameters  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  rigidTransform->SetParameters( finalParameters );



  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  RigidTransformType::Pointer finalTransform = RigidTransformType::New();

  finalTransform->SetFixedParameters( rigidTransform->GetFixedParameters() );

  finalTransform->SetParameters( finalParameters );

  transformWriter->SetFileName( argv[7] );
  transformWriter->SetInput( finalTransform );
  transformWriter->Update();

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform( finalTransform );
  resampler->SetInput( movingImage );

  resampler->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetOutputDirection( fixedImage->GetDirection() );
  resampler->SetDefaultPixelValue( 0 ); // low MRI value
  

  typedef itk::ImageFileWriter< FixedImageType >  WriterType;

  WriterType::Pointer      writer =  WriterType::New();

  writer->SetFileName( argv[3] );
  writer->SetInput( resampler->GetOutput()   );
  writer->Update();


  typedef itk::SubtractImageFilter< 
                                  FixedImageType, 
                                  FixedImageType, 
                                  FixedImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  difference->SetInput1( fixedImage );
  difference->SetInput2( resampler->GetOutput() );

  resampler->SetDefaultPixelValue( 0 );

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( difference->GetOutput() );  
  

  // Compute the difference image between the 
  // fixed and resampled moving image.
  writer2->SetFileName( argv[4] );
  writer2->Update();

  return EXIT_SUCCESS;
}

