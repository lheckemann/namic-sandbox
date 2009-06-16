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
#include "itkAffineTransform.h"

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFileReader.h"

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
  if( argc < 9 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile inputInitialTransformFile ";
    std::cerr << " outputImagefile differenceAfterRegistration screenshotsDirectory";
    std::cerr << " outputInitialTransformFile outputFinalTransformFile";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  
  const    unsigned int    Dimension = 3;
  typedef  signed short    PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::VersorRigid3DTransform< double >       RigidTransformType;
  typedef itk::AffineTransform< double, Dimension  >  AffineTransformType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer           metric        = MetricType::New();
  OptimizerType::Pointer        optimizer     = OptimizerType::New();
  InterpolatorType::Pointer     interpolator  = InterpolatorType::New();
  RegistrationType::Pointer     registration  = RegistrationType::New();
  AffineTransformType::Pointer  affineTransform = AffineTransformType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );
  registration->SetTransform( affineTransform );

 
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

  affineTransform->SetIdentity();

  typedef itk::TransformFileReader     TransformReaderType;

  TransformReaderType::Pointer transformReader = TransformReaderType::New();

  transformReader->SetFileName( argv[3] );
  transformReader->Update();

  typedef TransformReaderType::TransformListType * TransformListType;

  TransformListType transforms = transformReader->GetTransformList();

  TransformReaderType::TransformListType::const_iterator titr = transforms->begin();

  if( !strcmp((*titr)->GetNameOfClass(),"VersorRigid3DTransform") )
    {
    RigidTransformType::Pointer rigidTransform = RigidTransformType::New();
    rigidTransform = dynamic_cast< RigidTransformType * >( titr->GetPointer() );
  
    if( !rigidTransform )
      {
      std::cerr << "Error reading Rigid Transform" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      affineTransform->SetCenter( rigidTransform->GetCenter() );
      affineTransform->SetMatrix( rigidTransform->GetMatrix() );
      affineTransform->SetTranslation( rigidTransform->GetTranslation() );
      }
    } 
  else
    {
    std::cerr << "Input file does not contain a Rigid Transform" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Initial Transform = " << std::endl;
  affineTransform->Print( std::cout );
  
  registration->SetInitialTransformParameters( affineTransform->GetParameters() );


  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( affineTransform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;

  optimizerScales[3] = 1.0;
  optimizerScales[4] = 1.0;
  optimizerScales[5] = 1.0;

  optimizerScales[6] = 1.0;
  optimizerScales[7] = 1.0;
  optimizerScales[8] = 1.0;

  optimizerScales[ 9] = translationScale;
  optimizerScales[10] = translationScale;
  optimizerScales[11] = translationScale;

  optimizer->SetScales( optimizerScales );

  optimizer->MinimizeOn();

  optimizer->SetMaximumStepLength( 0.1000  ); 
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetRelaxationFactor( 0.9 );

  optimizer->SetNumberOfIterations( 100 );

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

  std::string screenshotDirectory = argv[6];
  itksys::SystemTools::MakeDirectory( screenshotDirectory.c_str() );

  std::cout <<  screenshotDirectory << std::endl; 

  monitor.SetScreenshotOutputDirectory( screenshotDirectory.c_str() );
  monitor.SetScreenshotBaseName( "registrationScreenshot" );

  monitor.Observe( optimizer, affineTransform );

  typedef itk::TransformFileWriter     TransformWriterType;

  TransformWriterType::Pointer transformWriter = TransformWriterType::New();

  transformWriter->SetFileName( argv[7] );
  transformWriter->SetInput( affineTransform );
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
  std::cout << metric->GetNumberOfPixelsCounter() << std::endl;

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

  affineTransform->SetParameters( finalParameters );



  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  AffineTransformType::Pointer finalTransform = AffineTransformType::New();

  finalTransform->SetFixedParameters( affineTransform->GetFixedParameters() );

  finalTransform->SetParameters( finalParameters );

  transformWriter->SetFileName( argv[8] );
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

  writer->SetFileName( argv[4] );
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
  writer2->SetFileName( argv[5] );
  writer2->Update();

  return EXIT_SUCCESS;
}

