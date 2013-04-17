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

#include "itkPluginUtilities.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
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

const unsigned int Dimension = 3;

//--------------------------------------------------------------------------------
//Calculate of Dice Similarity Coefficient (DSC) metric
//--------------------------------------------------------------------------------
/*
A decription of the DSC as used to evaluate prostate segmentation is given in the following
reference: A. Bharatha, M. Hirose, N. Hata, S. K. Warfield, M. Ferrant, K. H. Zou, E. Suarez-Santana,
J. Ruiz-Alzola, A. D'Amico, R. A. Cormack, R. Kikinis, F. A. Jolesz, and C. M. Tempany, "Evaluation of
three-dimensional finite element-based deformable registration of pre- and intraoperative prostate
imaging," Med Phys, vol. 28(12), pp. 2551-60, 2001.

Returns a value between 0 and 100 if successful. -1 if failed.
*/
template <class InputImageType>
double ComputeDscPercentage(typename InputImageType::Pointer input1, typename InputImageType::Pointer input2)
{
  //The DSC will be calculated as twice the number of pixels that have a value greater
  //than 0 in both images, divided by the sum of the number of pixels that have a value
  //greater than 0 in each image.
  float numPixels1 = 0;
  float numPixels2 = 0;
  int numIntersectPixels = 0;

  //Instantiate and define the iterators
  typedef itk::ImageRegionConstIterator< InputImageType > ConstIterCharType;
  ConstIterCharType eIter1( input1, input1->GetLargestPossibleRegion() );
  ConstIterCharType eIter2( input2, input2->GetLargestPossibleRegion() );

  //Calculate number of pixels from images 1 and 2
  for( eIter1.GoToBegin(), eIter2.GoToBegin(); !eIter1.IsAtEnd(); ++eIter1, ++eIter2)
  {
    if(eIter1.Get() > 0)
    {
      ++numPixels1;
    }
    if(eIter2.Get() > 0)
    {
      ++numPixels2;
    }
    if((eIter1.Get() > 0) && (eIter2.Get() > 0))
    {
      ++numIntersectPixels;
    }
  }

  //Calculate and print DSC

  std::cout << "Number of pixels in Image 1: " << numPixels1 << std::endl;
  std::cout << "Number of pixels in Image 2: " << numPixels2 << std::endl;
  std::cout << "Number of pixels intersecting Image 1 and 2: " << numIntersectPixels << std::endl;

  double dscPercentage = 100.0*double(2*numIntersectPixels)/double(numPixels1 + numPixels2);
  std::cout << "DSC percentage value [0-100]: " << dscPercentage << std::endl;

  return dscPercentage;
}

//--------------------------------------------------------------------------------
//Calculate 95% maximum Hausdorff distance metric
//--------------------------------------------------------------------------------
/*
Returns a value >=0 if successful. -1 if failed.
*/
template <class InputImageType>
double ComputeHd(typename InputImageType::Pointer input1, typename InputImageType::Pointer input2, 
  const std::string &edgeImage1OutputFilename, const std::string &edgeImage2OutputFilename, const std::string &distanceMapOutputFilename)
{
  //Canny Edge Detection
  //--------------------------------------------------------------------------------
  //To calculate the HD, we must extract the edges from the images.
  //To use the Canny Edge detector we must cast the image to float
  //Create templates for float pixel and image type

  typedef float    FloatPixelType;
  typedef itk::Image< FloatPixelType, Dimension > FloatImageType;

  //Cast the image to FloatImageType
  typedef itk::CastImageFilter< InputImageType, FloatImageType > CastToFloatFilterType;
  typename CastToFloatFilterType::Pointer castFilter1 = CastToFloatFilterType::New();
  typename CastToFloatFilterType::Pointer castFilter2 = CastToFloatFilterType::New();
  castFilter1->SetInput( input1 );
  castFilter2->SetInput( input2 );

  // Rescale the labelmap from 0 to 100 to make the results independent from the actual labelmap value
  typedef itk::RescaleIntensityImageFilter<FloatImageType,FloatImageType>    RescaleType;
  RescaleType::Pointer rescale1 = RescaleType::New();
  rescale1->SetOutputMinimum(0);
  rescale1->SetOutputMaximum(100);
  rescale1->SetInput(castFilter1->GetOutput());
  rescale1->UpdateLargestPossibleRegion();

  RescaleType::Pointer rescale2 = RescaleType::New();
  rescale2->SetOutputMinimum(0);
  rescale2->SetOutputMaximum(100);
  rescale2->SetInput(castFilter2->GetOutput());
  rescale2->UpdateLargestPossibleRegion();

  //Use the Canny Edge Detector to extract the edges of the images
  typedef itk::CannyEdgeDetectionImageFilter < FloatImageType, FloatImageType > CannyFilterType;
  CannyFilterType::Pointer cannyFilter1 = CannyFilterType::New();
  CannyFilterType::Pointer cannyFilter2 = CannyFilterType::New();
  float variance = 1.0;
  float uthresh = 0.5;
  float lthresh = 0.05;
  cannyFilter1->SetInput( rescale1->GetOutput() );
  cannyFilter2->SetInput( rescale2->GetOutput() );
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

  if (!edgeImage1OutputFilename.empty())
  {    
    typedef itk::ImageFileWriter< FloatImageType > FloatImageWriterType;
    FloatImageWriterType::Pointer writer1 = FloatImageWriterType::New();
    writer1->SetInput( cannyFilter1->GetOutput() );
    writer1->SetFileName( edgeImage1OutputFilename.c_str() );
    //Execute pipeline
    try
    {
      writer1->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Exception error caught when writing edge image 1" << std::endl;
      std::cerr << err << std::endl;
    }
  }

  if (!edgeImage2OutputFilename.empty())
  {    
    typedef itk::ImageFileWriter< FloatImageType > FloatImageWriterType;
    FloatImageWriterType::Pointer writer1 = FloatImageWriterType::New();
    writer1->SetInput( cannyFilter2->GetOutput() );
    writer1->SetFileName( edgeImage2OutputFilename.c_str() );
    //Execute pipeline
    try
    {
      writer1->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Exception error caught when writing edge image 2" << std::endl;
      std::cerr << err << std::endl;
    }
  }

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
    std::cerr << "Exception error caught when computing distance maps" << std::endl;
    std::cerr << err << std::endl;
    return -1;
  }

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
    return -1;
  }

  //Calculate 95% Husdorff Distance for each set
  //------------------------------------------------------------------------
  // The Hausdorff distance is not a symmetric value, and generally h(A,B) is not the same
  // as h(B,A). Therefore we will calculate the 95% Hausdorff distance for each of the images
  // and then choose the maximum

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
    {
      distances1.push_back(Iter3.Get());
    }
  }

  //Calculate HD2 from edges 2 to edges 1
  std::vector<float> distances2;
  for( Iter2.GoToBegin(), Iter3.GoToBegin(); !Iter2.IsAtEnd(); ++Iter2, ++Iter3)
  {
    if(Iter2.Get()>0)
    {
      distances2.push_back(Iter3.Get());
    }
  }

  //Get the maximum of the two 95% Hausdorff distances  
  float percentile = 0.95;
 
  //Sort the two vectors
  std::sort(distances1.begin(), distances1.end());
  std::sort(distances2.begin(), distances2.end());

  // Get the n-th percentile
  float hdmax = -1;
  float hd1 = -1;
  float hd2 = -1;
  float numMeas1 = distances1.size();
  float numMeas2 = distances2.size();
  if (numMeas1==0)
  {
    std::cerr << "No edge voxels could be extracted from input 1" << std::endl;
    return -1;
  }
  if (numMeas2==0)
  {
    std::cerr << "No edge voxels could be extracted from input 2" << std::endl;
    return -1;
  }

  int hd1Index=(int)(percentile*numMeas1) - 1;
  int hd2Index=(int)(percentile*numMeas2) - 1;
  if (hd1Index<0 || hd1Index>=numMeas1 || hd2Index<0 || hd2Index>=numMeas2)
  {
    std::cerr << "Failed to compute Hausdorff distance" << std::endl;
    return -1;
  }

  hd1 = distances1[hd1Index];
  hd2 = distances2[hd2Index];
  if(hd1 >= hd2)
  {
    hdmax = hd1;
  }
  else
  {
    hdmax = hd2;
  }

  //Print out maximum Hausdorff distance
  std::cout << "Hausdorff distance: " << hdmax << " mm" << std::endl;

  //Write out distance map image to file
  if (!distanceMapOutputFilename.empty())
  {    
    // Make and save output greyscale map
    // A greyscale map will be generated which will assign a pixel intensity equal
    // to the HD of the pixel

    //Define pixel and image type for output distance and color map
    FloatImageType::Pointer outputDistMap = FloatImageType::New();

    //Prepare output distance and color map image
    FloatImageType::RegionType region;
    if ( hdmax == hd1 )
    {
      region.SetIndex( input1->GetLargestPossibleRegion().GetIndex() );
      region.SetSize( input1->GetLargestPossibleRegion().GetSize() );
      outputDistMap->SetSpacing( input1->GetSpacing() );
      outputDistMap->SetOrigin( input1->GetOrigin() );
      outputDistMap->SetDirection( input1->GetDirection() );
    }
    else
    {
      region.SetIndex( input2->GetLargestPossibleRegion().GetIndex() );
      region.SetSize( input2->GetLargestPossibleRegion().GetSize() );
      outputDistMap->SetSpacing( input2->GetSpacing() );
      outputDistMap->SetOrigin( input2->GetOrigin() );
      outputDistMap->SetDirection( input2->GetDirection() );
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

    // Write distance map and colormap image to file
    typedef itk::ImageFileWriter< FloatImageType > FloatImageWriterType;
    FloatImageWriterType::Pointer writer1 = FloatImageWriterType::New();
    writer1->SetInput( outputDistMap );
    writer1->SetFileName( distanceMapOutputFilename.c_str() );
    //Execute pipeline
    try
    {
      writer1->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Exception error caught when writing colormap!" << std::endl;
      std::cerr << err << std::endl;
      // The HD is computed already, so don't return with -1 if we cannot save this additional distance map
    }   
  }

  return hdmax;
}

//--------------------------------------------------------------------------------
// Write out the return parameters in "name = value" form
//--------------------------------------------------------------------------------
void SaveResults(const std::string &returnParameterFile, double dscResult, double hdResult)
{ 
  std::ofstream rts;
  rts.open(returnParameterFile.c_str() );
  rts << "dscResult = " << std::fixed << std::setprecision(4) << dscResult << std::endl;
  rts << "hdResult = " << std::fixed << std::setprecision(4) << hdResult << std::endl;
  rts.close(); 
}


//--------------------------------------------------------------------------------
// main
//--------------------------------------------------------------------------------
template <class PixelType>
int DoIt( int argc, char * argv[])
{

  PARSE_ARGS;

  typedef itk::Image< PixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType > ReaderType;

  // Read in images from Slicer

  //Instantiate an imagefile reader and specify filename
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = ReaderType::New();
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
    std::cerr << "Exception error caught when reading the input files!" << std::endl;
    std::cerr << err << std::endl;
    SaveResults(returnParameterFile, -1, -1);
    return EXIT_FAILURE;
  }

  dscResult=-1;
  try
  {
    dscResult=ComputeDscPercentage<InputImageType>(reader1->GetOutput(), reader2->GetOutput());
  }
  catch (itk::ExceptionObject & err)
  {
    std::cerr << "Exception error caught in ComputeDsc" << std::endl;
    std::cerr << err << std::endl;
  }

  hdResult=-1;
  try
  {
    hdResult=ComputeHd<InputImageType>(reader1->GetOutput(), reader2->GetOutput(), EdgeImage1, EdgeImage2, DistanceMap);
  }
  catch (itk::ExceptionObject & err)
  {
    std::cerr << "Exception error caught in ComputeHd" << std::endl;
    std::cerr << err << std::endl;
  }

  SaveResults(returnParameterFile, dscResult, hdResult);
  return EXIT_SUCCESS; // otherwise output values are not loaded back into Slicer
}


int main( int argc, char* argv[] )
{
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType     pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType(imageFile1, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types

    switch( componentType )
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt<unsigned char>( argc, argv);
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt<char>( argc, argv);
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt<unsigned short>( argc, argv);
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt<short>( argc, argv);
        break;
      case itk::ImageIOBase::UINT:
        return DoIt<unsigned int>( argc, argv);
        break;
      case itk::ImageIOBase::INT:
        return DoIt<int>( argc, argv);
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt<unsigned long>( argc, argv);
        break;
      case itk::ImageIOBase::LONG:
        return DoIt<long>( argc, argv);
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt<float>( argc, argv);
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt<double>( argc, argv);
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
    
  return EXIT_SUCCESS;
}
