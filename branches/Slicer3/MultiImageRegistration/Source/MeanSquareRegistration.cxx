/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration1.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/20 13:27:53 $
  Version:   $Revision: 1.51 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


//user defined headers
#include "CongealingRegistration.h"

#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkImage.h"



#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"



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
  typedef const OptimizerType                         *OptimizerPointer;

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

    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
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
    std::cerr << "outputImagefile [differenceImageAfter]";
    std::cerr << "[differenceImageBefore]" << std::endl;
    return 1;
    }
  

  const    unsigned int    Dimension = 2;
  typedef  float           PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;

  typedef itk::TranslationTransform< double, Dimension > TransformType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  typedef itk::CongealingMultiImageMetric< 
                                    ImageType, 
                                    ImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    ImageType,
                                    double          >    InterpolatorType;

  typedef itk::CongealingRegistrationMethod< 
                                    ImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );


  typedef itk::ImageFileReader< ImageType  > FixedImageReaderType;
  typedef vector< FixedImageReaderType::Pointer > ImageArrayReader;
  //typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
  ImageArrayReader imageArrayReader;
  imageArrayReader.resize(2);
  // Register the files with the registration method
  for( int i=0; i<imageArrayReader.size(); i++)
  {
    imageArrayReader[i] = FixedImageReaderType::New();
    imageArrayReader[i]->SetFileName(  argv[i+1] );
    registration->SetImage(    imageArrayReader[i]->GetOutput() , i   );
  }
  //FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  //fixedImageReader->SetFileName(  argv[1] );
//   registration->SetFixedImage(    fixedImageReader->GetOutput() , 0   );

  // Write the points to be selected
  imageArrayReader[0]->Update();
  registration->SetFixedImageRegion( 
                    imageArrayReader[0]->GetOutput()->GetBufferedRegion() );


  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );

  initialParameters[0] = 0.0;  // Initial offset in mm along X
  initialParameters[1] = 0.0;  // Initial offset in mm along Y
  
  registration->SetInitialTransformParameters( initialParameters );

  optimizer->SetMaximumStepLength( 4.00 );  
  optimizer->SetMinimumStepLength( 0.01 );

  optimizer->SetNumberOfIterations( 200 );

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );



  try 
    { 
    registration->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 

  ParametersType finalParameters = registration->GetLastTransformParameters();

  const double TranslationAlongX = finalParameters[0];
  const double TranslationAlongY = finalParameters[1];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();

  std::cout << "Result = " << std::endl;
  std::cout << " Translation X = " << TranslationAlongX  << std::endl;
  std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;


  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            ImageType >    ResampleFilterType;

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  resampler->SetInput( imageArrayReader[1]->GetOutput() );


  resampler->SetTransform( registration->GetOutput()->Get() );

  ImageType::Pointer fixedImage = imageArrayReader[0]->GetOutput();
  resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetDefaultPixelValue( 100 );

  typedef unsigned char OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter< 
                        ImageType,
                        OutputImageType > CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();


  writer->SetFileName( argv[3] );



  caster->SetInput( resampler->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();

  typedef itk::SubtractImageFilter< 
                                  ImageType, 
                                  ImageType, 
                                  ImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  difference->SetInput1( imageArrayReader[0]->GetOutput() );
  difference->SetInput2( resampler->GetOutput() );

  typedef itk::RescaleIntensityImageFilter< 
                                  ImageType, 
                                  OutputImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  
  intensityRescaler->SetInput( difference->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  resampler->SetDefaultPixelValue( 1 );

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( intensityRescaler->GetOutput() );  



  if( argc > 4 )
    {
    writer2->SetFileName( argv[4] );
    writer2->Update();
    }




  TransformType::Pointer identityTransform = TransformType::New();
  identityTransform->SetIdentity();
  resampler->SetTransform( identityTransform );


  if( argc > 5 )
    {
    writer2->SetFileName( argv[5] );
    writer2->Update();
    }

   int *integers;
   integers = new int[3];
   integers[0] = 5;
   std::cout << integers[0] << std::endl;

  return 0;
}

