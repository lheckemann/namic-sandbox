/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MultiModalityRigidRegistrationCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/14 17:54:01 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// This program performs rigid registration of multi-modality images
// using a multi-resolution approach.

#include "MultiModalityRigidRegistration.h"
#include "CommandIterationUpdate.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSquaredDifferenceImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


int main( int argc, char *argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImage movingImage outputImage" << std::endl;
    std::cerr << " outputTransformile" << std::endl;
    std::cerr << " [resolutionLevels{0,1,2}]" << std::endl;
    std::cerr << " [qualityLevels{0,1}]" << std::endl;
    std::cerr << " [inputTransformFile] " << std::endl;
    std::cerr << " [fixedImageMask]" << std::endl;
    std::cerr << " [movingImageMask]";
    std::cerr << std::endl;
    return 1;
    }
  
  std::cout << "argc = " << argc << std::endl;

  std::cout << "\n\nhit return to continue... " << std::endl;
  char buffer[BUFSIZ];
  fgets (buffer, BUFSIZ, stdin);
  //std::string s; 
  //std::cin >> s;

  
   
  try {

  typedef itk::TimeProbesCollectorBase      TimerType;
  TimerType timer;


  typedef itk::MultiModalityRigidRegistration     RegistrationType;
  typedef RegistrationType::FixedImageType        FixedImageType;  
  typedef RegistrationType::MovingImageType       MovingImageType;  

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
  typedef itk::ImageFileWriter< FixedImageType  > FixedImageWriterType;
  //typedef itk::ImageFileWriter< MovingImageType > MovingImageWriterType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  FixedImageWriterType::Pointer  outputImageWriter = FixedImageWriterType::New();

  const char * fixedImageFilename  = argv[1];
  const char * movingImageFilename = argv[2];
  const char * outputImageFilename = argv[3];

  fixedImageReader->SetFileName(  fixedImageFilename );
  movingImageReader->SetFileName( movingImageFilename );
  outputImageWriter->SetFileName( outputImageFilename );

  try
    {
    timer.Start("Reading Fixed Image");
    fixedImageReader->Update();
    timer.Stop("Reading Fixed Image");
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem reading Fixed image = " << std::endl;
    std::cerr << fixedImageFilename << std::endl;
    std::cerr << excp << std::endl;
    return 1;
    }

  try
    {
    timer.Start("Reading Moving Image");
    movingImageReader->Update();
    timer.Stop("Reading Moving Image");
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem reading Moving image = " << std::endl;
    std::cerr << movingImageFilename << std::endl;
    std::cerr << excp << std::endl;
    return 2;
    }

  RegistrationType::Pointer  registration = RegistrationType::New();

  registration->SetFixedImage(    fixedImageReader->GetOutput()  );
  registration->SetMovingImage(   movingImageReader->GetOutput() );

  registration->ReportTimersOn();


  // Initialize the Transform

  typedef RegistrationType::ParametersType   ParametersType;
  typedef RegistrationType::TransformType    TransformType;

  TransformType::Pointer initialTransform = TransformType::New();
  initialTransform->SetIdentity();

  if( argc > 7 ) {
    const char * inputTransformFileName  = argv[7];

    std::ifstream inputParametersFile;
    inputParametersFile.open( inputTransformFileName );

    ParametersType inputParameters = initialTransform->GetParameters();

    for( unsigned int i=0; i<inputParameters.Size(); i++ )
      {
        inputParametersFile >> inputParameters[i];
      }

    typedef TransformType::InputPointType  CenterType;
    
    CenterType initialCenter;
    for(unsigned int j=0; j<3; j++)
      {
        inputParametersFile >> initialCenter[j];
      }

    if( inputParametersFile.fail() )
      {
        std::cerr << "Problems found while reading the inputParameters" << std::endl;
        std::cerr << "Filename = " << inputTransformFileName << std::endl; 
        return -1;
      }


    initialTransform->SetCenter( initialCenter );
    initialTransform->SetParameters( inputParameters );

    /* Debugging messages
       std::cout << "Tranform from the initialTransform file" << std::endl;
       std::cout << "Input parameters = " << inputParameters << std::endl;
       std::cout << "Input center     = " << initialCenter << std::endl;
       initialTransform->Print( std::cout );
    */
  }

  registration->SetInitialTransform( initialTransform );


  timer.Start("InitializeRegistration");
  registration->InitializeRegistration();
  timer.Stop("InitializeRegistration");

  unsigned int resolutionLevels = 2;

  if( argc > 5 )
    {
    resolutionLevels = atoi( argv[5] );
    }

 
  unsigned int qualityLevel = 1;

  if( argc > 6 )
    {
    qualityLevel = atoi( argv[6] );
    switch( qualityLevel )
      {
      case 0:
        registration->SetQualityLevel( RegistrationType::Low );
        break;
      case 1:
        registration->SetQualityLevel( RegistrationType::High );
        break;
      }
    }


  typedef itk::ImageFileReader< RegistrationType::MaskImageType > MaskReaderType;
  MaskReaderType::Pointer fixedMaskReader  = MaskReaderType::New();
  MaskReaderType::Pointer movingMaskReader = MaskReaderType::New();


  if( argc > 8 )
    {
    fixedMaskReader->SetFileName( argv[8] );
    try
      {
      timer.Start("Reading Fixed Mask");
      fixedMaskReader->Update();
      registration->SetFixedImageMask( fixedMaskReader->GetOutput() );
      timer.Stop("Reading Fixed Mask");
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Problem reading Fixed Mask from file = " << std::endl;
      std::cerr << argv[7] << std::endl;
      std::cerr << excp << std::endl;
      return 1;
      }
    }

 
  if( argc > 9 )
    {
    movingMaskReader->SetFileName( argv[9] );
    try
      {
      timer.Start("Reading Moving Mask");
      movingMaskReader->Update();
      registration->SetMovingImageMask( movingMaskReader->GetOutput() );
      timer.Stop("Reading Moving Mask");
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Problem reading Moving Mask from file = " << std::endl;
      std::cerr << argv[8] << std::endl;
      std::cerr << excp << std::endl;
      return 1;
      }
    }


  // Sequencially execute the registration for each resolution level
  
  if( resolutionLevels >= 0 )
    {
    // Register Level Zero : Quarter Resolution
    timer.Start("Register Level Zero");
    registration->RegisterCurrentResolutionLevel();
    timer.Stop("Register Level Zero");
    }

  if( resolutionLevels >= 1 )
    {
    // Register Level One  : Half Resolution
    timer.Start("Register Level One");
    registration->RegisterCurrentResolutionLevel();
    timer.Stop("Register Level One");
    }

  if( resolutionLevels >= 2 )
    {
    // Register Level Two  : Original Resolution
    timer.Start("Register Level Two");
    registration->RegisterCurrentResolutionLevel();
    timer.Stop("Register Level Two");
    }

 

  ParametersType parameters = registration->GetParameters();

  const char * outputTransformFileName = argv[4];

  std::ofstream parametersFile;
  parametersFile.open( outputTransformFileName );


  for( unsigned int i=0; i<parameters.Size(); i++ )
    {
    parametersFile << parameters[i] << "  ";  
    }

  typedef TransformType::InputPointType  CenterType;

  CenterType center = registration->GetTransform()->GetCenter();
  for(unsigned int j=0; j<3; j++)
    {
    parametersFile << center[j] << "  ";
    }

  parametersFile << std::endl;
  parametersFile.close();

  if( parametersFile.fail() )
    {
    std::cerr << "Problems found while saving the parameters" << std::endl;
    std::cerr << "Filename = " << outputTransformFileName << std::endl; 
    }

  //
  // Print out profiling information
  std::cout << std::endl << std::endl;
  timer.Report();


  //
  // Apply the transformation to the moving image
  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleMovingToFixedFilterType;

  ResampleMovingToFixedFilterType::Pointer resample = ResampleMovingToFixedFilterType::New();
  resample->SetInput( movingImageReader->GetOutput() );
  resample->SetTransform( registration->GetTransform() );

  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetDefaultPixelValue( 100 );

  
  //don't need a caster for fixed -> output
  //does resamplefilter handle that step for moving -> fixed?
  outputImageWriter->SetInput(resample->GetOutput());
  outputImageWriter->Update();

  } 
  catch ( itk::ExceptionObject & allexcep )
    {
    std::cerr << "Execute catched " << std::endl;
    std::cerr << allexcep << std::endl;
    }



  return 0;

}


