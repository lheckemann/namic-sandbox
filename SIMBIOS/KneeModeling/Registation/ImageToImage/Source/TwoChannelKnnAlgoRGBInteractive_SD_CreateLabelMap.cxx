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

unsigned int findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, float *xStandardDev, float *yStandardDev, int numberOfClusters, float *xnumberOfSds, float *ynumberOfSds);
typedef enum {red, white, green, ran, pink, grey, blue, brown, chocolate, darkkhaki, darkmagenta, darkorange, darkorchid, darkturquoise, deeppink, dimgray, gold, indigo, 
       navy, silver, teal, yellow, yellowgreen, azure, black}colors;

unsigned int greyScalePixels[] = {10, 80, 128, 200, 250}; //Recoloring background to 10 instead of 0

unsigned int colorPixels[MAX_SEED_FILES][3] = {
     255,0,0,
     255,255,255,
     0,255,0,
     204,128,51,
     255,192,203,
     128,128,128,
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
     154,205,50,
     240,255,255,
     0,0,0
     };



int main( int argc, char *argv[] )
{
  if( argc < 5 || (argc < 4+atoi(argv[4])))
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage1 inputImage2 outputLabelMap";
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
  typedef  itk::ImageFileWriter< ImageType > WriterType;

  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();

  reader1->SetFileName( argv[1] );
  reader2->SetFileName( argv[2] );

  reader1->Update();
  reader2->Update();
 
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  unsigned int totalSeeds = atoi(argv[4]);
  PointType point;
  float seedX;
  float seedY;
  float seedZ;
  unsigned int numberOfSeedPoints=0;
  float xClusterCenter[totalSeeds];
  float yClusterCenter[totalSeeds];
  ImageType::IndexType pixelIndex;
  ImageType::Pointer image1 = reader1->GetOutput();
  ImageType::Pointer image2 = reader2->GetOutput();

  /* Get the cluster centers */
  for(unsigned int i=5; i<5+totalSeeds; i++)
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

   numberOfSeedPoints=0;
//   std::cout << "Seed x = "<< seedX <<" Seed y = "<< seedY << " Seed z = "<< seedZ << std::endl;

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

   xClusterCenter[i-5] = xCluster/numberOfSeedPoints;
   yClusterCenter[i-5] = yCluster/numberOfSeedPoints;
  }

//  xClusterCenter[3] += 500;
  std::cout << "Cluster Centers are "<< std::endl;
  for(unsigned int i=0; i<totalSeeds; i++)
   std::cout << xClusterCenter[i] <<"  "<<yClusterCenter[i] << std::endl;

  float xStandardDev[totalSeeds], yStandardDev[totalSeeds];

  /* Find the standard deviation */
  for(unsigned int i=5; i<5+totalSeeds; i++)
  {
    std::ifstream inputSeedsFile;

    std::cout << "Opening seeds file " << argv[i] << std::endl; 
    inputSeedsFile.open( argv[i] );

   if( inputSeedsFile.fail() )
   {   
    std::cerr << "Error reading file " << argv[i] << std::endl;
    return EXIT_FAILURE;
   }   
          
   float xdev=0, ydev=0;
          
   inputSeedsFile >> seedX >> seedY >> seedZ;

   numberOfSeedPoints=0;
//   std::cout << "Seed x = "<< seedX <<" Seed y = "<< seedY << " Seed z = "<< seedZ << std::endl;

   while( ! inputSeedsFile.eof() )
   {   
    pixelIndex[0] = seedX;
    pixelIndex[1] = seedY;
    pixelIndex[2] = seedZ;

    ImageType::PixelType pixelValue1 = image1->GetPixel( pixelIndex );
    ImageType::PixelType pixelValue2 = image2->GetPixel( pixelIndex );

    xdev += (xClusterCenter[i-5]-pixelValue1)*(xClusterCenter[i-5]-pixelValue1);
    ydev += (yClusterCenter[i-5]-pixelValue2)*(yClusterCenter[i-5]-pixelValue2);

   std::cout << "Seed " << numberOfSeedPoints << " : " <<  seedX << " " << seedY << " " << seedZ << " = " << pixelValue1 << " " << pixelValue2 << std::endl;
    numberOfSeedPoints++;

    inputSeedsFile >> seedX >> seedY >> seedZ;
   }

   inputSeedsFile.close();
//   std::cout << " numberOfSeedPoints = "<< numberOfSeedPoints << " xdev = " << xdev << " ydev = "<< ydev << " xClusterCenter[i]= " << xClusterCenter[i] << " yClusterCenter[i] = " << yClusterCenter[i] << std::endl;
//   return 0;
   xStandardDev[i-5] = sqrt (xdev/numberOfSeedPoints);
   yStandardDev[i-5] = sqrt (ydev/numberOfSeedPoints);
  }

  std::cout << "Standard Deviation are "<< std::endl;
  for(unsigned int i=0; i<totalSeeds; i++)
   std::cout << xStandardDev[i] <<"  "<<yStandardDev[i] << std::endl;

  unsigned int numberOfPixels=0;   

  float xnumberOfSds[totalSeeds];
  float ynumberOfSds[totalSeeds];

  xnumberOfSds[0]=5; // background //0
  xnumberOfSds[1]=10; // Bones // 80 
  xnumberOfSds[2]=20; // Cartilage //128 
  xnumberOfSds[3]=0; // Fat // 200 
  // 225 Conflict
  // 255 Reject

  ynumberOfSds[0]=5; // background
  ynumberOfSds[1]=20; // Bones
  ynumberOfSds[2]=5; // Cartilage
  ynumberOfSds[3]=5; // Fat

/* *** THIS IS INCLUDING FAT CLUSTER
  xnumberOfSds[0]=5; // background //0
  xnumberOfSds[1]=5; // Bones // 80 
  xnumberOfSds[2]=20; // Cartilage //128 
  xnumberOfSds[3]=2; // Fat // 200 
  // 225 Conflict
  // 255 Reject

  ynumberOfSds[0]=5; // background
  ynumberOfSds[1]=20; // Bones
  ynumberOfSds[2]=5; // Cartilage
  ynumberOfSds[3]=5; // Fat
*/
  /* For gray scale image */
  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetRegions( image1->GetRequestedRegion() );
  outputImage->CopyInformation( image1 );
  outputImage->Allocate();



  /* Iterate through the images */
  IteratorType it1( image1, image1->GetRequestedRegion());
  IteratorType it2( image2, image2->GetRequestedRegion());
  
//  RGBIteratorType it3( rgbImage, rgbImage->GetRequestedRegion());
  IteratorType it3( outputImage, outputImage->GetRequestedRegion());

  /* Get the scatter plot */
  for (it1.GoToBegin(), it2.GoToBegin(), it3.GoToBegin(); !it1.IsAtEnd() && !it2.IsAtEnd() ; ++it1,++it2,++it3)
  {
   /* Find x and y */
   float x, y;
   x = it1.Get();
   y = it2.Get();

   ++numberOfPixels;
   /* Find the cluster center closest to x,y */
    unsigned int cIndex = findClosestClusterCenterIndex(x,y, xClusterCenter, yClusterCenter, xStandardDev, yStandardDev, totalSeeds, xnumberOfSds, ynumberOfSds);


    if(cIndex < 4)
    {
//     std::cout << "CIndex = " << cIndex << std::endl;
     it3.Set(greyScalePixels[cIndex]);
    }
    else if(cIndex == 21) // Point falling into more than cluster
     it3.Set(225);
    else //Point which doesnt fall into any cluster
      it3.Set(255);
  }
  
  outputImage->Update();
  writer->SetInput( outputImage );
  writer->Update();
  std::cout << " Number of pixels = "<< numberOfPixels << std::endl;
  return EXIT_SUCCESS;
}

unsigned int
findClosestClusterCenterIndex(float x, float y, float *xClusterCenter, float *yClusterCenter, float *xStandardDev, float *yStandardDev, int numberOfClusters, float *xnumberOfSds, float *ynumberOfSds)
{
 int i=0;
 int minIndex=0;
 int flag=0;

 for(i=0; i<numberOfClusters; i++)
 {
  if(i == 0) //Case for background
  {
   if(x>=0 && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=0 && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==0)
   {
    minIndex=i;
    flag=1;
   }
   else if(x>=0 && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=0 && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==1)
   {
    return 21;
   }
   continue;
  }

  else if(i == 1) // Case for Bones 
  {
   if(x>=(xClusterCenter[i-1]+xnumberOfSds[i-1]*xStandardDev[i-1]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==0)
   {
    minIndex=i;
    flag=1;
   }

   else if(x>=(xClusterCenter[i-1]+xnumberOfSds[i-1]*xStandardDev[i-1]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==1)
   {
    return 21;
   }
   continue;
  }

  else if(i == 2) // case for cartilage
  {
   if(x>=(xClusterCenter[i]-xnumberOfSds[i]*xStandardDev[i]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && flag==0)
   {
    minIndex=i;
    flag=1;
   }

   else if(x>=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && flag==1)
   {
    return 21;
   }
   continue;
  }

  if(x>=(xClusterCenter[i]-xnumberOfSds[i]*xStandardDev[i]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==0)
  {
   minIndex=i;
   flag=1;
  }

  else if(x>=(xClusterCenter[i]-xnumberOfSds[i]*xStandardDev[i]) && x<=(xClusterCenter[i]+xnumberOfSds[i]*xStandardDev[i]) && y>=(yClusterCenter[i]-ynumberOfSds[i]*yStandardDev[i]) && y<=(yClusterCenter[i]+ynumberOfSds[i]*yStandardDev[i]) && flag==1)
  {
   return 21;
  }
 }

 if(flag == 0) /* It doesnt fall into any cluster */
  return MAX_SEED_FILES-1;
 
 return minIndex;
}
