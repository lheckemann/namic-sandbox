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
     154,205,50,
     240,255,255,
     };



int main( int argc, char *argv[] )
{
  if( argc < 3 ) //|| (argc < 4+atoi(argv[4])))
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage1 inputImage2 z_slice_number";
 /*   std::cerr << " NumberOfSeedPoints " ;
    std::cerr << " SeedsFile1";
    std::cerr << " SeedsFile2";
    std::cerr << " SeedsFile3";
    std::cerr << " SeedsFile4";
    std::cerr << " ... ";
    std::cerr << std::endl;
*/    return EXIT_FAILURE;
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

  reader1->SetFileName( argv[1] );
  reader2->SetFileName( argv[2] );

  reader1->Update();
  reader2->Update();
 
  ImageType::Pointer image1 = reader1->GetOutput();
  ImageType::Pointer image2 = reader2->GetOutput();

  unsigned int numberOfPixels=0;   
  RGBImageType::PixelType clusterPixelValues[MAX_SEED_FILES];

  /* Iterate through the images */
  IteratorType it1( image1, image1->GetRequestedRegion());
  IteratorType it2( image2, image2->GetRequestedRegion());
  int nx=512, ny=512;
  int z_slice = atoi(argv[3]);
  /* Get the scatter plot */
//  for (it1.GoToBegin(), it2.GoToBegin(); !it1.IsAtEnd() && !it2.IsAtEnd() ; ++it1,++it2)
  for (int x1=0; x1<nx ; ++x1)
  {
   /* Find x and y */
/*   float x, y;
   x = it1.Get();
   y = it2.Get();
*/
   for(int y1=0; y1<ny ; ++y1)
   {
    ImageType::IndexType pixelIndex;
    pixelIndex[0]=x1;
    pixelIndex[1]=y1;
    pixelIndex[2]=z_slice;

    std::cout << x1 << " " << y1 << " " << image1->GetPixel(pixelIndex) << " " << image2->GetPixel(pixelIndex) << std::endl;
    ++numberOfPixels;
   }
//          std::cout << " X = "<< x << " Y = "<< y << std::endl;
  }
  
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
