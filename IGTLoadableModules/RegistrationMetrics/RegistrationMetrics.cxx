//********************************************************************************
//Date: May 2009
//Author: Haytham Elhawary
//Institution: Brigham and Women's Hospital, Surgical Planning Laboratory,
//75 Francis Street, Boston, MA, 02115
//Description: This code takes two label map input images and calculates the 95%
//Hausdorff distance and the Dice Similarity Coefficient between them. It also outputs
//an image which shows the contour of one of the input images with a color map representing
//the HD at each point of the contour. The code is part of a command line module for Slicer3
//********************************************************************************

#include "itkImageFileReader.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkLocalDistanceMapImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "RegistrationMetricsCLP.h"
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char** argv){

  PARSE_ARGS;

  //--------------------------------------------------------------------------------
  //Read in images from Slicer3
  //--------------------------------------------------------------------------------
  //Define pixel type, dimension of the input images, image type and reader type
  typedef unsigned char  PixelType;
  const unsigned int     Dimension = 3;
  typedef itk::Image< PixelType, Dimension > CharImageType;
  typedef itk::ImageFileReader< CharImageType > ReaderType;

  //Instantiate an imagefile reader and specify filename
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  reader1->SetFileName(imageFile1.c_str());
  reader2->SetFileName(imageFile2.c_str());

  //Execute pipeline
  try
    {
    reader1->Update();
    reader2->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "Exception error caught when readng files!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  //--------------------------------------------------------------------------------
  //Calculate of Dice Similarity Coefficient (DSC) metric
  //--------------------------------------------------------------------------------
  /*A decription of the DSC as used to evaluate prostate segmentation is given in the following
  reference: A. Bharatha, M. Hirose, N. Hata, S. K. Warfield, M. Ferrant, K. H. Zou, E. Suarez-Santana,
  J. Ruiz-Alzola, A. D'Amico, R. A. Cormack, R. Kikinis, F. A. Jolesz, and C. M. Tempany, "Evaluation of
  three-dimensional finite element-based deformable registration of pre- and intraoperative prostate
  imaging," Med Phys, vol. 28(12), pp. 2551-60, 2001. */

  //The DSC will be calculated as twice the number of pixels that have a value greater
  //than 0 in both images, divided by the sum of the number of pixels that have a value
  //greater than 0 in each image.
  float numPixels1 = 0, numPixels2 = 0, numIntersectPixels = 0;

  //Instantiate and define the iterators
  typedef itk::ImageRegionConstIterator< CharImageType > ConstIterCharType;
  ConstIterCharType eIter1( reader1->GetOutput(), reader1->GetOutput()->GetLargestPossibleRegion() );
  ConstIterCharType eIter2( reader2->GetOutput(), reader2->GetOutput()->GetLargestPossibleRegion() );

  //Calculate number of pixels from images 1 and 2
  for( eIter1.GoToBegin(), eIter2.GoToBegin(); !eIter1.IsAtEnd(); ++eIter1, ++eIter2)
    {
    if(eIter1.Get() > 0)
      numPixels1 = numPixels1 + 1;
    if(eIter2.Get() > 0)
      numPixels2 = numPixels2 + 1;
    if((eIter1.Get() > 0) && (eIter2.Get() > 0))
      numIntersectPixels = numIntersectPixels + 1;
    }

  //Calculate and print DSC
  float DSC;
  std::cerr << "Number of pixels in Image 1: " << numPixels1 << std::endl;
  std::cerr << "Number of pixels in Image 2: " << numPixels2 << std::endl;
  std::cerr << "Number of pixels intersecting Image 1 and 2: " << numIntersectPixels << std::endl;
  DSC = (2*numIntersectPixels)/(numPixels1 + numPixels2);
  std::cerr << "DSC value [0-1]: " << DSC << std::endl;

  //--------------------------------------------------------------------------------
  //Canny Edge Detection
  //--------------------------------------------------------------------------------
  //To calculate the HD, we must extract the edges from the images.
  //To use the Canny Edge detector we must cast the image to float
  //Create templates for float pixel and image type
  typedef float    FloatPixelType;
  typedef itk::Image< FloatPixelType, Dimension > FloatImageType;

  //Cast the image to FloatImageType
  typedef itk::CastImageFilter< CharImageType, FloatImageType > CastToFloatFilterType;
  CastToFloatFilterType::Pointer castFilter1 = CastToFloatFilterType::New();
  CastToFloatFilterType::Pointer castFilter2 = CastToFloatFilterType::New();
  castFilter1->SetInput( reader1->GetOutput() );
  castFilter2->SetInput( reader2->GetOutput() );

  //Use the Canny Edge Detector to extract the edges of the images
  typedef itk::CannyEdgeDetectionImageFilter < FloatImageType, FloatImageType > CannyFilterType;
  CannyFilterType::Pointer cannyFilter1 = CannyFilterType::New();
  CannyFilterType::Pointer cannyFilter2 = CannyFilterType::New();
  float variance = 1.0;
  float uthresh = 0.5;
  float lthresh = 0.05;
  cannyFilter1->SetInput( castFilter1->GetOutput() );
  cannyFilter2->SetInput( castFilter2->GetOutput() );
  cannyFilter1->SetVariance( variance );
  cannyFilter2->SetVariance( variance );
  cannyFilter1->SetLowerThreshold( lthresh );
  cannyFilter2->SetLowerThreshold( lthresh );
  cannyFilter1->SetUpperThreshold( uthresh );
  cannyFilter2->SetUpperThreshold( uthresh );

  //Execute pipeline
  try
    {
    cannyFilter1->Update();
    cannyFilter2->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "Exception error caught when extracting edges!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  //--------------------------------------------------------------------------
  //Distance Map filter
  //--------------------------------------------------------------------------
  //A distance map filter takes in a binary image and calculates the distance
  //from each pixel to the binary boundary. The inside of the boundary is
  //considered negative and the outside positive

  //Instantiate a SignedMaurerDistanceMapImageFilter
  typedef itk::SignedMaurerDistanceMapImageFilter< FloatImageType, FloatImageType > DistMapFilterType;
  DistMapFilterType::Pointer distMap1 = DistMapFilterType::New();
  DistMapFilterType::Pointer distMap2 = DistMapFilterType::New();

  //Turn off Euclidean squared distance and set filter input
  distMap1->SetInput( cannyFilter1->GetOutput() );
  distMap2->SetInput( cannyFilter2->GetOutput() );
  distMap1->SquaredDistanceOff();
  distMap1->SetUseImageSpacing(true);
  distMap2->SquaredDistanceOff();
  distMap2->SetUseImageSpacing(true);

  //Execute pipeline
  try
    {
    distMap1->Update();
    distMap2->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "Exception error caught when writing distance maps!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------------
  //Maximum Distance Map Image Filter
  //------------------------------------------------------------------------
  //We will now make an image which express the maximum distance map. This
  //is an image where each pixel represents the maximum distance from the edge
  //of its original binary image to the edge of the other binary image

  //The local distance map filter goes through the pixels of an edge binary image
  //and checks to see the distance expressed on the other image at that pixel. It
  //computes an image where a given pixel is the Hausdorff distance of the corresponding
  //pixels in two input edge images.

  //Instantiate the filter
  typedef itk::LocalDistanceMapImageFilter< FloatImageType, FloatImageType, FloatImageType > LocalDistanceMapType;
  LocalDistanceMapType::Pointer localDistMap = LocalDistanceMapType::New();
  localDistMap->SetInput1( distMap1->GetOutput() );
  localDistMap->SetInput2( distMap2->GetOutput() );

  //Execute pipeline
  try
    {
    localDistMap->Update();
    }
    catch (itk::ExceptionObject & err)
    {
    std::cerr << "Exception error caught when writing local distance maps!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------------
  //Calculate 95% Husdorff Distance for each set
  //------------------------------------------------------------------------
  //The Hausdorff distance is not a symmetric value, and generally h(A,B) is not the same
  //as h(B,A). Therefore we will calculate the 95% Hausdorff distance for each of the images
  //and then choose the maximum

  //Instantiate iterator
  typedef itk::ImageRegionConstIterator<FloatImageType> ConstIterFloatType;

  //Define Image and region for each iterator: edge image 1, edge image 2 and max distance map image
  ConstIterFloatType Iter1( cannyFilter1->GetOutput(), cannyFilter1->GetOutput()->GetLargestPossibleRegion() );
  ConstIterFloatType Iter2( cannyFilter2->GetOutput(), cannyFilter2->GetOutput()->GetLargestPossibleRegion() );
  ConstIterFloatType Iter3( localDistMap->GetOutput(), localDistMap->GetOutput()->GetLargestPossibleRegion() );

  //Calculate HD1 from edges 1 to edges 2
  std::vector<float> distances1;
  for( Iter1.GoToBegin(), Iter3.GoToBegin(); !Iter1.IsAtEnd(); ++Iter1, ++Iter3)
    {
    if(Iter1.Get()>0)
    distances1.push_back(Iter3.Get());
    }

  //Calculate HD2 from edges 2 to edges 1
  std::vector<float> distances2;
  for( Iter2.GoToBegin(), Iter3.GoToBegin(); !Iter2.IsAtEnd(); ++Iter2, ++Iter3)
    {
    if(Iter2.Get()>0)
    distances2.push_back(Iter3.Get());
    }

  //Get the maximum of the two 95% Hausdorff distances
  float hd1, hd2, hdmax;
  float percentile = 0.95;
  float numMeas1, numMeas2;
  numMeas1 = distances1.size();
  numMeas2 = distances2.size();
  //Sort the two vectors
  std::sort(distances1.begin(), distances1.end());
  std::sort(distances2.begin(), distances2.end());
  hd1 = distances1[(int)(percentile*numMeas1) - 1];
  hd2 = distances2[(int)(percentile*numMeas2) - 1];
  if(hd1 >= hd2)
    hdmax = hd1;
  else
    hdmax = hd2;

  //Print out maximum Hausdorff distance
  std::cerr << "Hausdorff distance: " << hdmax << " mm" << std::endl;

  //-------------------------------------------------------------------------
  // Make and save output greyscale map
  //-------------------------------------------------------------------------
  //A greyscale map will be generated which will assign a pixel intensity equal
  // to the HD of the pixel

  //Define pixel and image type for output distance and color map
  FloatImageType::Pointer outputDistMap = FloatImageType::New();

  //Prepare output distance and color map image
  FloatImageType::RegionType region;
  if ( hdmax == hd1 )
    {
    region.SetIndex( reader1->GetOutput()->GetLargestPossibleRegion().GetIndex() );
    region.SetSize( reader1->GetOutput()->GetLargestPossibleRegion().GetSize() );
    }
  else
    {
    region.SetIndex( reader2->GetOutput()->GetLargestPossibleRegion().GetIndex() );
    region.SetSize( reader2->GetOutput()->GetLargestPossibleRegion().GetSize() );
    }
  outputDistMap->SetRegions( region );
  outputDistMap->Allocate();


  //Instantiate and define colorMap iterator
  typedef itk::ImageRegionIterator<FloatImageType> IterFloatType;
  IterFloatType Iter5( outputDistMap, outputDistMap->GetLargestPossibleRegion() );

  if ( hdmax == hd1 )
    {
    //Iterate through the input image with the highest 95% HD, and generate color map
    for (Iter1.GoToBegin(), Iter3.GoToBegin(), Iter5.GoToBegin();
      !Iter1.IsAtEnd(); ++Iter1, ++Iter3, ++Iter5)
      {
      if (Iter1.Get() > 0)
        {
        //Access value of maximum distance map and assign pixel that value. If value is 0, make it
        //0.02 in order for it to have a different color than the background
        if (Iter3.Get() > 0)
          Iter5.Set( Iter3.Get() );
        else
          Iter5.Set( 0.02 );
        }
      else
        {
        //Make pixel black
        Iter5.Set( 0 );
        }
      }
    }
  else
    {
    //Iterate through the input image with the highest 95% HD, and generate color map
    for (Iter2.GoToBegin(), Iter3.GoToBegin(), Iter5.GoToBegin();
      !Iter2.IsAtEnd(); ++Iter2, ++Iter3, ++Iter5)
      {
      if (Iter2.Get() > 0)
        {
        //Access value of maximum distance map and assign pixel that value. If value is 0, make it
        //0.02 in order for it to have a different color than the background
        if (Iter3.Get() > 0)
          Iter5.Set( Iter3.Get() );
        else
          Iter5.Set( 0.02 );
        }
      else
        {
        //Make pixel black
        Iter5.Set( 0 );
        }
     }
  }

  //--------------------------------------------------------------------------------
  //Write out image to file
  //--------------------------------------------------------------------------------
  //Write distance map and colormap image to file
  typedef itk::ImageFileWriter< FloatImageType > FloatImageWriterType;
  FloatImageWriterType::Pointer writer1 = FloatImageWriterType::New();
  writer1->SetInput( outputDistMap );
  writer1->SetFileName( DistanceMap.c_str() );

  //Execute pipeline
  try
    {
    writer1->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "Exception error caught when writing colormap!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  return 0;
}//end main
