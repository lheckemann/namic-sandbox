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

#define MAX_SEED_FILES 20

unsigned int findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, int numberOfClusters);

typedef enum {red, green, blue, brown, chocolate, darkkhaki, darkmagenta, darkorange, darkorchid, darkturquoise, deeppink, dimgray, gold, indigo, 
       navy, silver, teal, yellow, yellowgreen, azure}colors;

unsigned int colorPixels[MAX_SEED_FILES][3] = {
     255,0,0,
     0,255,0,
     0,0,255,
     165,42,42,
     210,105,30,
     189,183,107,
     139,0,139,
     255,140,0,
     153,50,204,
     0,206,209,
     255,20,147,
     105,105,105,
     255,215,0,
     75,0,130,
     0,0,128,
     192,192,192,
     0,128,128,
     255,255,0,
     154,205,50
     240,255,255,
     };



int main( int argc, char *argv[] )
{
  if( argc < 6 || (argc < 5+atoi(argv[4])+atoi(argv[1])))
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " No of image vectors " ;
    std::cerr << " inputImage1 inputImage2 ... inputImageN outputLabelMap";
    std::cerr << " NumberOfSeedPoints " ;
    std::cerr << " SeedsFile1";
    std::cerr << " SeedsFile2";
    std::cerr << " SeedsFile3";
    std::cerr << " SeedsFile4";
    std::cerr << " ... ";
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
 
  int totalReaderFiles=atoi(argv[1]);

  ReaderType::Pointer reader[totalReaderFiles];
  ImageType::Pointer image[totalReaderFiles];
  for(int i=0; i<totalReaderFiles; i++)
  {
   reader[i] = ReaderType::New();
   reader[i]->SetFileName( argv[i+1] );
   reader[i]->Update();
     image[i] = reader[i]->GetOutput();
  }
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[1+totalReaderFiles] );
  unsigned int totalSeeds = atoi(argv[2+totalReaderFiles]);
  PointType point;
  float seedX;
  float seedY;
  float seedZ;
  unsigned int numberOfSeedPoints=0;
  float xClusterCenter[totalSeeds];
  float yClusterCenter[totalSeeds];
  ImageType::IndexType pixelIndex;


  /* Get the cluster centers */
  for(unsigned int i=2+totalReaderFiles; i<totalSeeds+2+totalReaderFiles; i++)
  {
    std::ifstream inputSeedsFile;

    std::cout << "Opening seeds file " << argv[i] << std::endl; 
    inputSeedsFile.open( argv[i] );

   if( inputSeedsFile.fail() )
   {   
    std::cerr << "Error reading file " << argv[i] << std::endl;
    return EXIT_FAILURE;
   }   
          
   float xCluster[totalReaderFiles];
          
   inputSeedsFile >> seedX >> seedY >> seedZ;
   while( ! inputSeedsFile.eof() )
   {   
    pixelIndex[0] = seedX;
    pixelIndex[1] = seedY;
    pixelIndex[2] = seedZ;

    
    ImageType::PixelType pixelValue[totalReaderFiles];
    
    for(int j=0; j<totalReaderFiles; j++)
    {
     pixelValue[j] = image[j]->GetPixel( pixelIndex );
     xCluster[j] += pixelValue1;
    }
//    std::cout << "Seed " << numberOfSeedPoints << " : " <<  seedX << " " << seedY << " " << seedZ << " = " << pixelValue1 << " " << pixelValue2 << std::endl;
    numberOfSeedPoints++;

    inputSeedsFile >> seedX >> seedY >> seedZ;
   }

   inputSeedsFile.close();

   xClusterCenter[i-5] = xCluster[i]/numberOfSeedPoints;
  }

  std::cout << "Cluster Centers are "<< std::endl;
  for(unsigned int i=0; i<totalSeeds; i++)
   std::cout << xClusterCenter[i] <<"  "<<yClusterCenter[i] << std::endl;
  
  unsigned int numberOfPixels=0;   
  RGBImageType::PixelType clusterPixelValues[MAX_SEED_FILES];

  for(unsigned int i=0; i<MAX_SEED_FILES; i++)
  {
   clusterPixelValues[i][0] = colorPixels[i][0];
   clusterPixelValues[i][1] = colorPixels[i][1];
   clusterPixelValues[i][2] = colorPixels[i][2];

   printf("Color of %d cluster is r=%d g=%d b=%d \n", i, clusterPixelValues[i][0], clusterPixelValues[i][1], clusterPixelValues[i][2]);
  }

  /* For RGB Pixels */
  RGBImageType::Pointer rgbImage = RGBImageType::New();
  rgbImage->SetRegions( image[0]->GetRequestedRegion() );
  rgbImage->CopyInformation( image[0] );
  rgbImage->Allocate();
  

  /* Iterate through the images */
//  IteratorType it1( image1, image->GetRequestedRegion());
//  IteratorType it2( image2, image2->GetRequestedRegion());
   IteratorType it[totalReaderFiles];
  
  RGBIteratorType it3( rgbImage, rgbImage->GetRequestedRegion());
  for(int i=0; i<totalReaderFiles; i++)
  {
 it[i].GoToBegin();
  }

  /* Get the scatter plot */
  for ( it3.GoToBegin(); !it[0].IsAtEnd(); ++it3)
  {
   /* Find x and y */
   float x[totalReaderFiles];
   for(int i=0; i<totalReaderFiles; i++)
    x = it[i].Get();
   ++numberOfPixels;
   /* Find the cluster center closest to x,y */
//          std::cout << " X = "<< x << " Y = "<< y << std::endl;
    unsigned int cIndex = findClosestClusterCenterIndex(x, totalReaderFiles, xClusterCenter, yClusterCenter, totalSeeds);
    
    it3.Set(clusterPixelValues[cIndex]);
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

 return minIndex;
}
