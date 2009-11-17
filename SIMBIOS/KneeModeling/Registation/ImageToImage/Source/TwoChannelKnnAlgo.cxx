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

  typedef unsigned char     OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  itk::TimeProbesCollectorBase  chronometer;

  typedef  itk::ImageFileReader< ImageType > ReaderType;
  typedef  itk::ImageFileWriter< ImageType > WriterType;

  typedef itk::ImageRegionIterator< ImageType > IteratorType;

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
  unsigned int pixelValueArray[512][512];
  unsigned int xDimensions=512, yDimensions=512;
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

  /* Iterate through the images */
  IteratorType it1( image1, image1->GetRequestedRegion());
  IteratorType it2( image2, image2->GetRequestedRegion());

   
  /* Get the scatter plot */

  for ( it1.GoToBegin(), it2.GoToBegin(); !it1.IsAtEnd() && !it2.IsAtEnd();
    ++it1, ++it2)
  {
   /* Find x and y */
   float x,y;
   x = it1.Get();
   y = it2.Get();

   /* Find the cluster center closest to x,y */
//          std::cout << " X = "<< x << " Y = "<< y << std::endl;
//   findClosestClusterCenterIndex(x, y, xClusterCenter, yClusterCenter);
  }

  return EXIT_SUCCESS;
}
