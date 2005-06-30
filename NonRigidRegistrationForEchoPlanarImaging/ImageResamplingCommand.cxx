/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageResamplingCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 01:32:05 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkVersorRigid3DTransform.h"
#include "itkResampleImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"

#include "itkTimeProbesCollectorBase.h"


int main( int argc, char *argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFilename  movingImageFilename "; 
    std::cerr << " transformFile  interpolationType defaultPixelValue outputImageFilename";
    std::cerr << std::endl;
    std::cerr << "interpolationType = [0=NearestNeigbor,1=Linear,2=BSpline,3=Sinc]" << std::endl;
    return 1;
    }
  
  try {

  typedef itk::TimeProbesCollectorBase      TimerType;
  TimerType timer;


  typedef float                         PixelType;  // to leave the CT as a PET.

  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > FixedImageReaderType;
  typedef itk::ImageFileReader< ImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  const char * fixedImageFilename  = argv[1];
  const char * movingImageFilename = argv[2];

  fixedImageReader->SetFileName( fixedImageFilename );
  movingImageReader->SetFileName( movingImageFilename );

  try
    {
    timer.Start("Reading Fixed Image");
    fixedImageReader->Update();
    std::cout << "Fixed Image: " << std::endl;
    std::cout << "Origin  = " << fixedImageReader->GetOutput()->GetOrigin()  << std::endl;
    std::cout << "Spacing = " << fixedImageReader->GetOutput()->GetSpacing() << std::endl;
    timer.Stop("Reading Fixed Image");
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem reading Fixed image from directory = " << std::endl;
    std::cerr << fixedImageFilename << std::endl;
    std::cerr << excp << std::endl;
    return 1;
    }

  try
    {
    timer.Start("Reading Moving Image");
    movingImageReader->Update();
    std::cout << "Moving Image: " << std::endl;
    std::cout << "Origin  = " << movingImageReader->GetOutput()->GetOrigin()  << std::endl;
    std::cout << "Spacing = " << movingImageReader->GetOutput()->GetSpacing() << std::endl;

    timer.Stop("Reading Moving Image");
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem reading Moving image from directory = " << std::endl;
    std::cerr << movingImageFilename << std::endl;
    std::cerr << excp << std::endl;
    return 2;
    }

  
  //
  //   Load transform parameters from file
  // 
  const char * parametersFileName = argv[3];


  timer.Start("ResampleMovingImage");

  std::ifstream parametersFile;
  parametersFile.open( parametersFileName );

  typedef itk::VersorRigid3DTransform< double  > TransformType;
  typedef TransformType::ParametersType          ParametersType;

  TransformType::Pointer transform = TransformType::New();

  ParametersType parameters = transform->GetParameters();

  for( unsigned int i=0; i< parameters.Size(); i++)
     {
     parametersFile >> parameters[i];
     if( parametersFile.fail() )
       {
       std::cerr << "Error reading parameters from file = " << parametersFileName << std::endl;
       return 1;
       }
     }


  TransformType::InputPointType  center;
  for(unsigned int k=0; k<3; k++)
    {
    parametersFile >> center[k];  
    if( parametersFile.fail() )
      {
      std::cerr << "Error reading parameters from file = " << parametersFileName << std::endl;
      return 1;
      }
    }

  transform->SetCenter( center );        // the order here is important.

  transform->SetParameters( parameters );


  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            ImageType >    ResampleFilterType;


  ResampleFilterType::Pointer resampler = ResampleFilterType::New();


  resampler->SetTransform( transform );

  resampler->SetInput( movingImageReader->GetOutput() );



  const ImageType * fixedImage = fixedImageReader->GetOutput();

  ImageType::RegionType region = fixedImage->GetLargestPossibleRegion();

  resampler->SetSize( region.GetSize() ); 

  resampler->SetOutputSpacing(  fixedImage->GetSpacing() );
  resampler->SetOutputOrigin(   fixedImage->GetOrigin()  );
  resampler->SetOutputStartIndex( region.GetIndex() );

  
  const PixelType backgroundGrayLevel = atof( argv[5] );
  
  resampler->SetDefaultPixelValue( backgroundGrayLevel );


  const unsigned int interpolatorType = atoi( argv[4] );

  switch( interpolatorType )
  {
    case 0:  // Nearest Neighbor
      {
      typedef itk:: NearestNeighborInterpolateImageFunction< 
                         ImageType, double  >    InterpolatorType;

      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
       
      resampler->SetInterpolator( interpolator );
      break;
      }
    case 1:  // Linear
      {
      typedef itk:: LinearInterpolateImageFunction< 
                         ImageType, double  >    InterpolatorType;

      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
       
      resampler->SetInterpolator( interpolator );
      break;
      }
    case 2:  // BSpline
      {
      typedef itk::BSplineInterpolateImageFunction< 
                         ImageType, double  >    InterpolatorType;

      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
       
      resampler->SetInterpolator( interpolator );
      break;
      }
    case 3:  // Windowed Sinc (Sinus Cardinal)
      {

      typedef itk::ConstantBoundaryCondition< ImageType >  BoundaryConditionType;

      const unsigned int WindowRadius = 2;

      typedef itk::Function::HammingWindowFunction< WindowRadius >  WindowFunctionType;

      typedef itk::WindowedSincInterpolateImageFunction< 
                                              ImageType, 
                                              WindowRadius,
                                              WindowFunctionType, 
                                              BoundaryConditionType, 
                                              double  >    InterpolatorType;

      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
       
      resampler->SetInterpolator( interpolator );
      break;
      }
    default:
      {
      std::cerr << "The Interpolator you selected is an Unknown type. Please change your selection" << std::endl;
      return -1;
      }
  }


  
  resampler->Update();



  timer.Stop("ResampleMovingImage");

  typedef itk::Image< PixelType, Dimension > OutputImageType;

  typedef itk::ImageFileWriter< OutputImageType >  ImageWriterType;

  ImageWriterType::Pointer imageWriter = ImageWriterType::New();

  imageWriter->SetInput( resampler->GetOutput() );

  const char * outputImageFilename = argv[6];
  imageWriter->SetFileName( outputImageFilename );

  try
    {
    timer.Start("Writing Output Image");
    imageWriter->Update();
    timer.Stop("Writing Output Image");
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem writing resampled Moving image " << std::endl;
    std::cerr << excp << std::endl;
    return 3;
    }


  //
  // Print out profiling information
  std::cout << std::endl << std::endl;
  timer.Report();

  } 
  catch ( itk::ExceptionObject & allexcep )
    {
    std::cerr << "Execute catched " << std::endl;
    std::cerr << allexcep << std::endl;
    }


  return 0;

}


