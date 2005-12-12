/*=========================================================================

 author: Bryn Lloyd, Computational Radiology Laboratory, Brigham and Womans Hospital
   date: 06/30/2005

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif




#include <itkImage.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>


//#include "itkHarrisTensorToImageFilter.h"
#include "itkDeterminantImageFilter.h"
#include "itkDeterminantOverTraceNImageFilter.h"
#include "itkDeterminantOverTraceNminusOneImageFilter.h"


int main( int argc, char *argv[] )
//int itkDetAndDetOverTraceNminusOneTensorFilterTest(int, char* [] )
{

  // Define the dimension of the images
  const unsigned int myDimension = 3;
  typedef float PixelType;

  // Declare the types of the images
  typedef itk::Image<PixelType, myDimension>           myImageType;

  // Declare the type of the index to access images
  typedef itk::Index<myDimension>             myIndexType;

  // Declare the type of the size
  typedef itk::Size<myDimension>              mySizeType;

  // Declare the type of the Region
  typedef itk::ImageRegion<myDimension>        myRegionType;

  // Create the image
  myImageType::Pointer inputImage  = myImageType::New();


  // Define their size, and start index
  mySizeType size;
  size[0] = 8;
  size[1] = 8;
  size[2] = 8;

  myIndexType start;
  start.Fill(0);

  myRegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  // Initialize Image A
  inputImage->SetLargestPossibleRegion( region );
  inputImage->SetBufferedRegion( region );
  inputImage->SetRequestedRegion( region );
  inputImage->Allocate();

  // Declare Iterator type for the input image
  typedef itk::ImageRegionIteratorWithIndex<myImageType>  myIteratorType;

  // Create one iterator for the Input Image A (this is a light object)
  myIteratorType it( inputImage, inputImage->GetRequestedRegion() );

  // Initialize the content of Image A
  while( !it.IsAtEnd() )
    {
    it.Set( 0.0 );
    ++it;
    }

  size[0] = 4;
  size[1] = 4;
  size[2] = 4;

  start[0] = 2;
  start[1] = 2;
  start[2] = 2;

  // Create one iterator for an internal region
  region.SetSize( size );
  region.SetIndex( start );
  myIteratorType itb( inputImage, region );

  // Initialize the content the internal region
  while( !itb.IsAtEnd() )
    {
    itb.Set( 100.0 );
    ++itb;
    }

  // Declare the type for the
  typedef itk::HessianRecursiveGaussianImageFilter<
                                            myImageType >  myFilterType;

  typedef myFilterType::OutputImageType myDiadicProductOfGradientImageType;


  // Create a  Filter
  myFilterType::Pointer filter = myFilterType::New();


  // Connect the input images
  filter->SetInput( inputImage );

  // Select the value of Sigma
  filter->SetSigma( 2.5 );


  // Execute the filter
  filter->Update();


  //Run the various SymmetricSecondRankTensor type filters on the calculated image

  typedef itk::DeterminantImageFilter<myDiadicProductOfGradientImageType,myImageType>                    myDetImageFilterType;

  typedef itk::DeterminantOverTraceNImageFilter<myDiadicProductOfGradientImageType,myImageType>          myDetOverTraceImageFilterType;

  typedef itk::DeterminantOverTraceNminusOneImageFilter<myDiadicProductOfGradientImageType,myImageType>  myDetOverTraceNminusOneImageFilterType;


  myDetImageFilterType::Pointer                    myfilter1 = myDetImageFilterType::New();

  myDetOverTraceImageFilterType::Pointer           myfilter2 = myDetOverTraceImageFilterType::New();

  myDetOverTraceNminusOneImageFilterType::Pointer  myfilter3 = myDetOverTraceNminusOneImageFilterType::New();



  // Get the Smart Pointer to the Filter Output
  // It is important to do it AFTER the filter is Updated
  // Because the object connected to the output may be changed
  // by another during GenerateData() call
  myImageType::Pointer outputImage1 = myfilter1->GetOutput();
  myImageType::Pointer outputImage2 = myfilter2->GetOutput();
  myImageType::Pointer outputImage3 = myfilter3->GetOutput();


  // Declare Iterator type for the output image
  typedef itk::ImageRegionIteratorWithIndex<
                                 myImageType>  myOutputIteratorType;

  // Create an iterator for going through the output image
  myOutputIteratorType itg1( outputImage1,
                            outputImage1->GetRequestedRegion() );
  myOutputIteratorType itg2( outputImage2,
                           outputImage2->GetRequestedRegion() );
  myOutputIteratorType itg3( outputImage3,
                            outputImage3->GetRequestedRegion() );

  //  Print the content of the result image
  std::cout << " Result " << std::endl;
  itg1.GoToBegin();
  itg2.GoToBegin();
  itg3.GoToBegin();
  while( !itg1.IsAtEnd() )
    {
    std::cout << itg1.Get() << "  " << itg2.Get() << "  " << itg3.Get()  << std::endl;
    ++itg1;
    ++itg2;
    ++itg3;
    }



  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;

}




