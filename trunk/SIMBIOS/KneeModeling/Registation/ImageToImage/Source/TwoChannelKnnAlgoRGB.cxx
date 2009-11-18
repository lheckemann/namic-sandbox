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
#include <math.h>

unsigned int findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, int numberOfClusters);

int main( int argc, char *argv[] )
{
  if( argc < 7 )
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage1 inputImage2  outputLabelMap";
    std::cerr << " SeedsFile1";
    std::cerr << " SeedsFile2";
    std::cerr << " SeedsFile3";
    std::cerr << " SeedsFile4";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  typedef   float           PixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::Point< double, ImageType::ImageDimension > PointType;


  itk::TimeProbesCollectorBase  chronometer;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageRegionIterator< ImageType > IteratorType;

  typedef itk::RGBPixel <unsigned char> RGBPixelType;
  typedef itk::Image < RGBPixelType, Dimension > RGBImageType;
  typedef itk::ImageRegionIterator< RGBImageType > RGBIteratorType;
  typedef itk::ScalarToRGBColormapImageFilter < ImageType, RGBImageType > RGBFilterType;

  typedef  itk::ImageFileWriter< RGBImageType > WriterType;
  
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader1->SetFileName( argv[1] );
  reader2->SetFileName( argv[2] );
  writer->SetFileName( argv[3] );

  reader1->Update();
  reader2->Update();

  ImageType::Pointer image1 = reader1->GetOutput();
  ImageType::Pointer image2 = reader2->GetOutput();
  
  PointType point;
  float seedX;
  float seedY;
  float seedZ;
  unsigned int numberOfSeedPoints=0;
  float xClusterCenter[4];
  float yClusterCenter[4];
  ImageType::IndexType pixelIndex;

  /* Get the cluster centers */
  for(int i=4; i<8; i++)
  {
    std::ifstream inputSeedsFile;

    std::cout << "Opening seeds file " << argv[i] << std::endl; 
    inputSeedsFile.open( argv[i] );

   if( inputSeedsFile.fail() )
   {   
    std::cerr << "Error reading file " << argv[i] << std::endl;
    return EXIT_FAILURE;
   }   
          
   float xCluster=0, yCluster=0;
          
   inputSeedsFile >> seedX >> seedY >> seedZ;
   std::cout << " Causing failure here "<< std::endl;
   while( ! inputSeedsFile.eof() )
   {   
    pixelIndex[0] = seedX;
    pixelIndex[1] = seedY;
    pixelIndex[2] = seedZ;

    ImageType::PixelType pixelValue1 = image1->GetPixel( pixelIndex );
    ImageType::PixelType pixelValue2 = image2->GetPixel( pixelIndex );

    xCluster += pixelValue1;
    yCluster += pixelValue2;

//    std::cout << "Seed " << numberOfSeedPoints << " : " <<  seedX << " " << seedY << " " << seedZ << " = " << pixelValue1 << " " << pixelValue2 << std::endl;
    numberOfSeedPoints++;

    inputSeedsFile >> seedX >> seedY >> seedZ;
   }

   inputSeedsFile.close();

   xClusterCenter[i-4] = xCluster/numberOfSeedPoints;
   yClusterCenter[i-4] = yCluster/numberOfSeedPoints;
  }

  std::cout << "Cluster Centers are "<< std::endl;
  for(int i=0; i<4; i++)
   std::cout << xClusterCenter[i] <<"  "<<yClusterCenter[i] << std::endl;
  
  /* Give four different colors */
//  unsigned int colors[4];

//  colors[0] = 

  
  unsigned int numberOfPixels=0;   
  

/*  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetRegions( image1->GetRequestedRegion() );
  outputImage->CopyInformation( image1 );
  outputImage->Allocate();
*/
  /* For RGB Pixels */
  RGBImageType::Pointer rgbImage = RGBImageType::New();
  rgbImage->SetRegions( image1->GetRequestedRegion() );
  rgbImage->CopyInformation( image1 );
  rgbImage->Allocate();
  

  /* Iterate through the images */
  IteratorType it1( image1, image1->GetRequestedRegion());
  IteratorType it2( image2, image2->GetRequestedRegion());
//  IteratorType it3( outputImage, outputImage->GetRequestedRegion());
  RGBIteratorType it3( rgbImage, rgbImage->GetRequestedRegion());
  
  /* Get the scatter plot */
  for ( it1.GoToBegin(), it2.GoToBegin(), it3.GoToBegin(); !it1.IsAtEnd() && !it2.IsAtEnd();
    ++it1, ++it2, ++it3)
  {
   /* Find x and y */
   float x,y;
   x = it1.Get();
   y = it2.Get();
   ++numberOfPixels;
   /* Find the cluster center closest to x,y */
//          std::cout << " X = "<< x << " Y = "<< y << std::endl;
      it3.Set(findClosestClusterCenterIndex(x, y, xClusterCenter, yClusterCenter, 4));
  }
  
  rgbImage->Update();
  writer->SetInput( rgbImage );
  writer->Update();
  std::cout << " Number of pixels = "<< numberOfPixels << std::endl;
  return EXIT_SUCCESS;
}

unsigned int
findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, int numberOfClusters)
{
 int i=0;
        float minDistance=0.0, distance=0.0;
 int minIndex=0;
 int flag=0;

 for(i=0; i<numberOfClusters; i++)
 {
  distance = (x - xClusterCenter[i])*(x - xClusterCenter[i]) + (y - yClusterCenter[i])*(y - yClusterCenter[i]);

  if(flag == 0)
  {
   minDistance = distance;
   minIndex = i;
   flag=1;
   continue;
  }

  if(minDistance > distance)
  {
   minDistance = distance;
   minIndex = i;
  }
 }

/* if ( minIndex == 0)
  return 0;
 else if(minIndex == 1)
  return 80;
 else if(minIndex == 2)
  return 128;
 else if(minIndex == 3)
  return 255;
 return minIndex;
*/
 if ( minIndex == 0)
  return 0;
 else if(minIndex == 1)
  return 80;
 else if(minIndex == 2)
  return 128;
 else if(minIndex == 3)
  return 255;
 else
 {
  return 0;
 }

}
