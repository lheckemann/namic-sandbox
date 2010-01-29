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
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineDeformableTransform.h"

#include "itkImageRegionIterator.h"
#include "itkNormalVariateGenerator.h"

#include "itkTransformFileWriter.h"

#include "itkRescaleIntensityImageFilter.h"

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <cstdlib>
#include <ctime>

#include "CreateImageSetBsplineCLP.h"
#include <itksys/SystemTools.hxx>

#define TOTNUM 100000

int main( int argc, char * argv[] )
{
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
  PARSE_ARGS;
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
  InputImageType::Pointer referenceImage = reader->GetOutput();

  // Rescale the input image to 0-255
  typedef itk::RescaleIntensityImageFilter<InputImageType, InputImageType >  RescaleFilterType;
  RescaleFilterType::Pointer    rescaleFilter    = RescaleFilterType::New();
  rescaleFilter->SetInput(    referenceImage );
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);
  rescaleFilter->Update();

  const int numberOfImages = atoi(NumberOfImages.c_str());

  std::vector<double> randomOffsetNumbers(numberOfImages*TOTNUM);
  for(long int i=0;i<TOTNUM*numberOfImages; i++)
  {
    randomOffsetNumbers[i] = (rand()%203/203.0 - 0.5)*23.0;
  }
  for(long int i=0;i<TOTNUM; i++)
  {
    // Make the mean zero along images
    double mean = 0.0;
    for(int j=0; j<numberOfImages;j++)
    {
      mean += randomOffsetNumbers[TOTNUM*j+i];
    }
    mean /= (double) numberOfImages;
    for(int j=0; j<numberOfImages;j++)
    {
      randomOffsetNumbers[TOTNUM*j+i] -= mean;
    }
  }


  for(int i=0; i<numberOfImages ; i++)
  {

    WriterType::Pointer writer = WriterType::New();

    typedef itk::ResampleImageFilter<InputImageType,OutputImageType> ResampleFilterType;
    ResampleFilterType::Pointer resample = ResampleFilterType::New();

    typedef itk::BSplineDeformableTransform< double,
                                           Dimension,
                                           3 >     BSplineTransformType;

    BSplineTransformType::Pointer bsplineTransform = BSplineTransformType::New();


    typedef BSplineTransformType::RegionType RegionType;
    RegionType bsplineRegion;
    RegionType::SizeType   gridSizeOnImage;
    RegionType::SizeType   gridBorderSize;
    RegionType::SizeType   totalGridSize;

    gridSizeOnImage.Fill( 8 );
    gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = gridSizeOnImage + gridBorderSize;

    bsplineRegion.SetSize( totalGridSize );

    typedef BSplineTransformType::SpacingType SpacingType;
    SpacingType bsplineSpacing = referenceImage->GetSpacing();

    typedef BSplineTransformType::OriginType OriginType;
    OriginType bsplineOrigin = referenceImage->GetOrigin();;

    InputImageType::SizeType ImageSize = referenceImage->GetLargestPossibleRegion().GetSize();

    for(unsigned int r=0; r<Dimension; r++)
    {
      bsplineSpacing[r] *= floor( static_cast<double>(ImageSize[r] )  /
          static_cast<double>(gridSizeOnImage[r] ) );
      bsplineOrigin[r]  -=  bsplineSpacing[r];
    }

    bsplineTransform->SetGridSpacing( bsplineSpacing );
    bsplineTransform->SetGridOrigin( bsplineOrigin );
    bsplineTransform->SetGridRegion( bsplineRegion );

    typedef BSplineTransformType::ParametersType     ParametersType;

    const unsigned int numberOfParameters =
                     bsplineTransform->GetNumberOfParameters();

    ParametersType bsplineParameters( numberOfParameters );

    bsplineParameters.Fill( 0.0 );

    //Randomly set the parameters
    int count = 0;
    for(unsigned int j=0; j<bsplineParameters.GetSize(); j++)
    {
      bsplineParameters[j] = randomOffsetNumbers[TOTNUM*i+count];
      count = (count+1) % TOTNUM;
    }


    bsplineTransform->SetParameters( bsplineParameters );

    resample->SetTransform( bsplineTransform );
    typedef itk::LinearInterpolateImageFunction<
                             InputImageType, double >  InterpolatorType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resample->SetInterpolator( interpolator );

    // Initialize the resampler
    // Get the size of the image
    const InputImageType::SizeType size = referenceImage->GetLargestPossibleRegion().GetSize();

    //Get the spacing
    const InputImageType::SpacingType spacing = referenceImage->GetSpacing();
    //Get the origin
    const InputImageType::PointType origin = referenceImage->GetOrigin();

    resample->SetOutputParametersFromImage(referenceImage);
    resample->SetDefaultPixelValue( 0 );
    resample->SetInput( rescaleFilter->GetOutput() );
    writer->SetInput( resample->GetOutput() );

    std::string fname;
    std::ostringstream fnameStream;
    fnameStream << i ;


      {
      //Write the transform files
      itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
      itksys::SystemTools::MakeDirectory( (fname + folderName + "/TransformFiles/").c_str() );

      std::string fileName = fname + folderName + "/TransformFiles/" + fnameStream.str() + ".mat";
      transformFileWriter->SetFileName(fileName.c_str());
      transformFileWriter->SetPrecision(12);
      transformFileWriter->SetInput(bsplineTransform);
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

