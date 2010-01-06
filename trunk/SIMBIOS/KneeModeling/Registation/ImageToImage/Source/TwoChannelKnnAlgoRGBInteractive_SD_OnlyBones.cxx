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

#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWeightedAddImageFilter.h"
#include "itkImage.h"
#include "itkTimeProbesCollectorBase.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkRGBPixel.h"
#include <math.h>

#define MAX_SEED_FILES 25

unsigned int findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, float *xStandardDev, float *yStandardDev, int numberOfClusters, int *numberOfSds);
typedef enum {red, white, green, ran, pink, grey, blue, brown, chocolate, darkkhaki, darkmagenta, darkorange, darkorchid, darkturquoise, deeppink, dimgray, gold, indigo, 
       navy, silver, teal, yellow, yellowgreen, azure, black}colors;


int main( int argc, char *argv[] )
{
  if( argc < 4 )
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputLabelMap outputLabelMap ROIintensity";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }


  typedef   float           PixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::Point< double, ImageType::ImageDimension > PointType;



  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageRegionIterator< ImageType > IteratorType;

  typedef  itk::ImageFileWriter< ImageType > WriterType;

  ReaderType::Pointer reader1 = ReaderType::New();

  reader1->SetFileName( argv[1] );

  reader1->Update();
 
  ImageType::Pointer image1 = reader1->GetOutput();
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );
  float intensity = atof(argv[3]);

  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetRegions( image1->GetRequestedRegion() );
  outputImage->CopyInformation( image1 );
  outputImage->Allocate();


  /* Iterate through the images */
  IteratorType it1( image1, image1->GetRequestedRegion());
  
  IteratorType it3( outputImage, outputImage->GetRequestedRegion());

  unsigned int numberOfPixels=0;

  /* Get the scatter plot */
  for (it1.GoToBegin(), it3.GoToBegin(); !it1.IsAtEnd() && !it3.IsAtEnd() ; ++it1,++it3)
  {
   /* Find x and y */
   float x;
   x = it1.Get();

   if(x != intensity)
   {
    it3.Set(0);
   }
   else
   {
    it3.Set(x);
   }
   ++numberOfPixels;
  }
  
  outputImage->Update();
  writer->SetInput( outputImage );
  writer->Update();
  
  std::cout << "Number of pixels = " << numberOfPixels << std::endl; 
  return EXIT_SUCCESS;
}

