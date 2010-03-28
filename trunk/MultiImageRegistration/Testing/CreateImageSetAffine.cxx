/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ResampleImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/14 12:12:52 $
  Version:   $Revision: 1.32 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#include "itkAffineTransform.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkTransformFileWriter.h"

#include "itkRescaleIntensityImageFilter.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <ctime>

#include <itksys/SystemTools.hxx>
#include "CreateImageSetAffineCLP.h"


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

/*
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  folderName numberOfImages <on/off (opt: for srand)>" << std::endl;
    return EXIT_FAILURE;
    }

  if(argc == 5)
  {
    srand(time(NULL));
  }
*/

  const     unsigned int   Dimension = 3;
  typedef   double  InputPixelType;
  typedef   unsigned short  OutputPixelType;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  // Read the input image
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFile );
  reader->Update();
  InputImageType::Pointer referenceImage=reader->GetOutput();

  // Rescale the input image to 0-255
  typedef itk::RescaleIntensityImageFilter<InputImageType, InputImageType >  RescaleFilterType;
  RescaleFilterType::Pointer    rescaleFilter    = RescaleFilterType::New();
  rescaleFilter->SetInput(    referenceImage );
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);
  rescaleFilter->Update();

  const int numberOfImages = atoi(NumberOfImages.c_str());

  // Create numbers with zero mean
  std::vector<double> randomOffsetNumbers(numberOfImages*12);
  for(long int i=0;i<12*numberOfImages; i++)
  {
    randomOffsetNumbers[i] = (rand()%100/100.0 - 0.5);
  }
  for(long int i=0;i<12; i++)
  {
    // Make the mean zero along images
    double mean = 0.0;
    for(int j=0; j<numberOfImages;j++)
    {
      mean += randomOffsetNumbers[12*j+i];
    }
    mean /= (double) numberOfImages;
    for(int j=0; j<numberOfImages;j++)
    {
      randomOffsetNumbers[12*j+i] -= mean;
    }
  }

  for(int i=0; i<numberOfImages ; i++)
  {

    WriterType::Pointer writer = WriterType::New();

    typedef itk::ResampleImageFilter<InputImageType,OutputImageType> ResampleFilterType;
    ResampleFilterType::Pointer resample = ResampleFilterType::New();

    typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
    typedef itk::LinearInterpolateImageFunction<
                             InputImageType, double >  InterpolatorType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resample->SetInterpolator( interpolator );


    // Set the parameters of the affine transform
    //Get the spacing
    const InputImageType::SpacingType spacing = referenceImage->GetSpacing();
    //Get the origin
    const InputImageType::PointType origin = referenceImage->GetOrigin();
    const InputImageType::SizeType size = referenceImage->GetLargestPossibleRegion().GetSize();

    AffineTransformType::Pointer affineTransform = AffineTransformType::New();
      { //Randomly assign parameters for the affineParameters
      AffineTransformType::InputPointType center;
      for(unsigned int j=0; j< Dimension; j++)
        {
        center[j] = origin[j] + spacing[j]*size[j] / 2.0;
        }
      affineTransform->SetIdentity();
      affineTransform->SetCenter(center);
      AffineTransformType::ParametersType affineParameters;
      affineParameters = affineTransform->GetParameters();

      affineParameters[0] = 1.0 + randomOffsetNumbers[12*i+0]*0.05;
      affineParameters[1] = randomOffsetNumbers[12*i+1]*0.3;
      affineParameters[2] = randomOffsetNumbers[12*i+2]*0.2;

      affineParameters[3] = randomOffsetNumbers[12*i+3]*0.3;
      affineParameters[4] = 1.0 + randomOffsetNumbers[12*i+4]*0.05;
      affineParameters[5] = randomOffsetNumbers[12*i+5]*0.3;

      affineParameters[6] = randomOffsetNumbers[12*i+6]*0.2;
      affineParameters[7] = randomOffsetNumbers[12*i+7]*0.3;
      affineParameters[8] = 1.0 + randomOffsetNumbers[12*i+8]*0.05;

      affineParameters[9] = randomOffsetNumbers[12*i+9]*15.0;
      affineParameters[10] = randomOffsetNumbers[12*i+10]*15.0;
      affineParameters[11] = randomOffsetNumbers[12*i+11]*15.0;

      affineTransform->SetParameters(affineParameters);

      resample->SetTransform( affineTransform );
      }
    // Initialize the resampler
    // Get the size of the image

    resample->SetOutputParametersFromImage(referenceImage);
    resample->SetDefaultPixelValue( 0 );
    resample->SetInput( rescaleFilter->GetOutput() );
    writer->SetInput( resample->GetOutput() );

    std::string fname;
    std::ostringstream fnameStream;
    fnameStream << i ;

      { //Write the transform files
      itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
      itksys::SystemTools::MakeDirectory( (fname + folderName + "/TransformFiles/").c_str() );

      std::string fileName = fname + folderName + "/TransformFiles/" + fnameStream.str() + ".mat";
      transformFileWriter->SetFileName(fileName.c_str());
      transformFileWriter->SetPrecision(12);
      transformFileWriter->SetInput(affineTransform);
      transformFileWriter->Update();
      }

    itksys::SystemTools::MakeDirectory( (fname+folderName+"/Images/").c_str() );

    fname = fname + folderName + "/Images/" + fnameStream.str();
    if(Dimension == 2)
    {
      fname += ".png";
    }
    else
    {
      fname += ".nii.gz";
    }

    writer->SetFileName( fname.c_str() );
    std::cout << "Writing " << fname.c_str() << std::endl;
    writer->Update();

  }
  return EXIT_SUCCESS;
}
