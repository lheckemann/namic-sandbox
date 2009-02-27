/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-10 19:46:31 $
  Version:   $Revision: 1.37 $

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
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"



#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"


#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"


//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
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
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};


int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " outputImagefile  [differenceBeforeRegistration] ";
    std::cerr << " [differenceAfterRegistration] ";
    std::cerr << " [sliceBeforeRegistration] ";
    std::cerr << " [sliceDifferenceBeforeRegistration] ";
    std::cerr << " [sliceDifferenceAfterRegistration] ";
    std::cerr << " [sliceAfterRegistration] " << std::endl;
    return EXIT_FAILURE;
    }
  
  const    unsigned int    Dimension = 3;
  typedef  float           PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;



  typedef itk::VersorRigid3DTransform< double > TransformType;



  typedef itk::VersorRigid3DTransformOptimizer           OptimizerType;

  typedef itk::MeanSquaresImageToImageMetric< 
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



  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );
  fixedImageReader->Update();

  registration->SetFixedImageRegion( 
     fixedImageReader->GetOutput()->GetBufferedRegion() );



  typedef itk::CenteredTransformInitializer< TransformType, 
                                             FixedImageType, 
                                             MovingImageType 
                                                 >  TransformInitializerType;

  TransformInitializerType::Pointer initializer = 
                                          TransformInitializerType::New();




  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );


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

  optimizer->SetNumberOfIterations( 200 );


  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );


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
  
  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();


  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
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






  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetCenter( transform->GetCenter() );

  finalTransform->SetParameters( finalParameters );

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform( finalTransform );
  resampler->SetInput( movingImageReader->GetOutput() );

  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  resampler->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetOutputDirection( fixedImage->GetDirection() );
  resampler->SetDefaultPixelValue( 100 );
  
  typedef  unsigned char  OutputPixelType;

  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();


  writer->SetFileName( argv[3] );


  caster->SetInput( resampler->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();


  typedef itk::SubtractImageFilter< 
                                  FixedImageType, 
                                  FixedImageType, 
                                  FixedImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();


  typedef itk::RescaleIntensityImageFilter< 
                                  FixedImageType, 
                                  OutputImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  
  intensityRescaler->SetInput( difference->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  difference->SetInput1( fixedImageReader->GetOutput() );
  difference->SetInput2( resampler->GetOutput() );

  resampler->SetDefaultPixelValue( 1 );

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( intensityRescaler->GetOutput() );  
  

  // Compute the difference image between the 
  // fixed and resampled moving image.
  if( argc > 5 )
    {
    writer2->SetFileName( argv[5] );
    writer2->Update();
    }


  typedef itk::IdentityTransform< double, Dimension > IdentityTransformType;
  IdentityTransformType::Pointer identity = IdentityTransformType::New();

  // Compute the difference image between the 
  // fixed and moving image before registration.
  if( argc > 4 )
    {
    resampler->SetTransform( identity );
    writer2->SetFileName( argv[4] );
    writer2->Update();
    }




//
//  Here we extract slices from the input volume, and the difference volumes
//  produced before and after the registration.  These slices are presented as
//  figures in the Software Guide.
//
//
  
  typedef itk::Image< OutputPixelType, 2 > OutputSliceType;

  typedef itk::ExtractImageFilter< 
                          OutputImageType, 
                          OutputSliceType > ExtractFilterType;

  ExtractFilterType::Pointer extractor = ExtractFilterType::New();


  FixedImageType::RegionType inputRegion =
                               fixedImage->GetLargestPossibleRegion();

  FixedImageType::SizeType  size  = inputRegion.GetSize();
  FixedImageType::IndexType start = inputRegion.GetIndex();

  // Select one slice as output
  size[2]  =  0;
  start[2] = 90;
  
  FixedImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );

  extractor->SetExtractionRegion( desiredRegion );

  typedef itk::ImageFileWriter< OutputSliceType > SliceWriterType;
  SliceWriterType::Pointer sliceWriter = SliceWriterType::New();

  sliceWriter->SetInput( extractor->GetOutput() );
   
  if( argc > 6 )
    {
    extractor->SetInput( caster->GetOutput() );
    resampler->SetTransform( identity );
    sliceWriter->SetFileName( argv[6] );  
    sliceWriter->Update();
    }
 
  if( argc > 7 )
    {
    extractor->SetInput( intensityRescaler->GetOutput() );
    resampler->SetTransform( identity );
    sliceWriter->SetFileName( argv[7] );  
    sliceWriter->Update();
    }

  if( argc > 8 )
    {
    resampler->SetTransform( finalTransform );
    sliceWriter->SetFileName( argv[8] );  
    sliceWriter->Update();
    }

  if( argc > 9 )
    {
    extractor->SetInput( caster->GetOutput() );
    resampler->SetTransform( finalTransform );
    sliceWriter->SetFileName( argv[9] );  
    sliceWriter->Update();
    }


  return EXIT_SUCCESS;
}

