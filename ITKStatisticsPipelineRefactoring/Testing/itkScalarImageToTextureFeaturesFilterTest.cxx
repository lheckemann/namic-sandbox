/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkScalarImageToTextureFeaturesFilterTest.cxx,v $
Language:  C++
Date:      $Date: 2004/08/02 06:59:23 $
Version:   $Revision: 1.6 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
// Insight classes
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "vnl/vnl_math.h"

#include "itkScalarImageToTextureFeaturesFilter.h"

int itkScalarImageToTextureFeaturesFilterTest(int, char* [] )
{

  //Data definitions 
  const unsigned int  IMGWIDTH         =  5;
  const unsigned int  IMGHEIGHT        =  5;
  const unsigned int  NDIMENSION       =  2;


  //------------------------------------------------------
  //Create a simple test images
  //------------------------------------------------------
  typedef itk::Image<unsigned char, NDIMENSION> InputImageType;

  typedef itk::ImageRegionIterator< InputImageType > InputImageIterator;

   
  InputImageType::Pointer image = InputImageType::New();
  
  InputImageType::SizeType inputImageSize = {{ IMGWIDTH, IMGHEIGHT }};

  InputImageType::IndexType index;
  index.Fill(0);
  InputImageType::RegionType region;

  region.SetSize( inputImageSize );
  region.SetIndex( index );

  //--------------------------------------------------------------------------
  // Set up the image first. It looks like:
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //  1 2 1 2 1
  //--------------------------------------------------------------------------

  image->SetRegions( region );
  image->Allocate();

  // setup the iterator
  InputImageIterator imageIt( image, image->GetBufferedRegion() );

  for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++, ++imageIt)
      {
      imageIt.Set(j % 2 + 1);
      }

  
  //--------------------------------------------------------------------------
  // Test the texFilter
  //--------------------------------------------------------------------------
  bool passed = true;
  
  try {
  
  typedef itk::Statistics::ScalarImageToTextureFeaturesFilter< 
    InputImageType> TextureFilterType;
  
  // First test: just use the defaults.
  TextureFilterType::Pointer texFilter = TextureFilterType::New();

  //Invoke update before adding an input. An exception should be 
  //thrown.
  try
    {
    texFilter->Update();
    passed = false;
    std::cerr << "Failed to throw expected exception due to NULL input: " << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cout << "Expected exception caught: " << excp << std::endl;
    }    

  texFilter->ResetPipeline();

  if ( texFilter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should return NULL since the input is\
                  not set yet " << std::endl;
    passed = false;
    }
 
  if ( texFilter->GetMaskImage() != NULL )
    {
    std::cerr << "GetMaskImage() should return NULL since the mask image is\
                  not set yet " << std::endl;
    passed = false;
    }

  //Invoke update with a NULL input. An exception should be 
  //thrown.
  texFilter->SetInput( NULL );
  try
    {
    texFilter->Update();
    passed = false;
    std::cerr << "Failed to throw expected exception due to NULL input: " << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cout << "Expected exception caught: " << excp << std::endl;
    }    

  texFilter->ResetPipeline();

  if ( texFilter->GetInput() != NULL )
    {
    passed = false;
    }
  
  texFilter->SetInput(image);
  texFilter->Update();
  TextureFilterType::FeatureValueVectorPointer means, stds;
  means = texFilter->GetFeatureMeans();
  stds = texFilter->GetFeatureStandardDeviations();
  
  double expectedMeans[6] = {0.505, 0.992738, 0.625, 0.75, 0.0959999, 0.2688};
  double expectedDeviations[6] = {0.00866027, 0.0125788, 0.216506351, 0.433012702, 
    0.166277, 0.465575};
  
  TextureFilterType::FeatureValueVector::ConstIterator mIt, sIt;
  int counter;
  for (counter = 0, mIt = means->Begin(); mIt != means->End(); ++mIt, counter++)
    {
    if ( vnl_math_abs(expectedMeans[counter] - mIt.Value()) > 0.0001 ) 
      {
      std::cout << "Error. Mean for feature " << counter << " is " << mIt.Value() <<
      ", expected " << expectedMeans[counter] << "." << std::endl;
      passed = false;
      }
    }

  for (counter = 0, sIt = stds->Begin(); sIt != stds->End(); ++sIt, counter ++)
    {
    if ( vnl_math_abs(expectedDeviations[counter] - sIt.Value()) > 0.0001 )
      {
      std::cout << "Error. Deiviation for feature " << counter << " is " << sIt.Value() <<
      ", expected " << expectedDeviations[counter] << "." << std::endl;
      passed = false;
      }
    }
  
  if (!passed)
    {
    std::cerr << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cerr << "Test succeeded" << std::endl;
    return EXIT_SUCCESS;
    }
  
  } catch( itk::ExceptionObject & err ) { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    std::cerr << "Test failed" << std::endl;
    return EXIT_FAILURE;
  }
}

