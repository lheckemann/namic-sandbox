/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOuterProductImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/05/07 17:22:40 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif




#include <itkImage.h>
#include <itkFixedArray.h>
#include <itkVector.h>
#include <itkSymmetricSecondRankTensor.h>
#include <itkOuterProductImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>

//int itkOuterProductImageFilterTest(int, char* [] )
int main( int argc, char ** argv)
{

  // Define the dimension of the images
  const unsigned int myDimension = 3;
  const unsigned int myVectorLength = 2;

  // Declare the types of the images
  typedef itk::Image<itk::FixedArray<float,myVectorLength>, myDimension>           myImageType;

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
  size[0] = 3;
  size[1] = 3;
  size[2] = 2;

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
  float count = -10.0;
  while( !it.IsAtEnd() )
    {
    for (unsigned int k=0; k<myVectorLength; k++)
      {
      it.Value()[k] = count++;
      }
    ++it;
    }



  // Declare the type for the
  typedef itk::OuterProductImageFilter< myImageType, myVectorLength >  myFilterType;

  typedef itk::Image<itk::SymmetricSecondRankTensor<float,myVectorLength>, myDimension>           myTensorImageType;


  // Create a  Filter
  myFilterType::Pointer filter = myFilterType::New();


  // Connect the input images
  filter->SetInput( inputImage );


  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  // It is important to do it AFTER the filter is Updated
  // Because the object connected to the output may be changed
  // by another during GenerateData() call
  myTensorImageType::Pointer outputImage = filter->GetOutput();

  // Declare Iterator type for the output image
  typedef itk::ImageRegionIteratorWithIndex<
                                 myTensorImageType>  myOutputIteratorType;

  // Create an iterator for going through the output image
  myOutputIteratorType itg( outputImage,
                            outputImage->GetRequestedRegion() );

  //  Print the content of the result image
  std::cout << " Result " << std::endl;
  itg.GoToBegin();
  it.GoToBegin();
  while( !itg.IsAtEnd() | count < 10)
    {
    std::cout << " the vecotor: " << it.Get() << "  the Outer Product is    " << itg.Get() << std::endl;
    ++itg;
    ++it;
    }




  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;

}




