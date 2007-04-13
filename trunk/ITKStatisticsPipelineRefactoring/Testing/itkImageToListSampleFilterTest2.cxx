/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/01/15 18:38:35 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// The example tests the class itk::Statistics::ImageToListSampleFilter.
// The class is capable of generating an itk::ListSample from an image
// confined to a mask (if specified). This test exercises that.

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkImageToListSampleFilterTest2(int, char* [] ) 
{
  const unsigned int MeasurementVectorSize = 8;
  typedef unsigned long MeasurementComponentType;
  typedef itk::FixedArray< MeasurementComponentType, MeasurementVectorSize > PixelType;

  const unsigned int ImageDimension = 3;
  typedef itk::Image< PixelType, ImageDimension > ImageType;
  typedef itk::Image< unsigned char, ImageDimension > MaskImageType;

  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start = {0,0,0};
  ImageType::SizeType  size = {10,10,10};
  ImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorType;
  IteratorType it( image, region );
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    PixelType value;
    for( unsigned int i=0; i< MeasurementVectorSize; i++ )
      {
      value[i] = i + it.GetIndex()[0];
      }
    it.Set( value );
    ++it; 
    }

  MaskImageType::Pointer maskImage = MaskImageType::New();
  maskImage->SetRegions( region );
  maskImage->Allocate();
  maskImage->FillBuffer(0);
  MaskImageType::IndexType startMask = {2,3,5};
  MaskImageType::SizeType sizeMask = {7,3,9};
  MaskImageType::RegionType regionMask( startMask, sizeMask);
  typedef itk::ImageRegionIteratorWithIndex< MaskImageType > MaskIteratorType;
  MaskIteratorType mit( maskImage, regionMask );
  mit.GoToBegin();
  while (!mit.IsAtEnd())
    {
    mit.Set((unsigned char)255);    
    ++mit; 
    }

  // Generate a list sample from "image" confined to the mask, "maskImage".
  typedef itk::Statistics::ImageToListSampleFilter< 
    ImageType, MaskImageType > ImageToListSampleFilterType;
  ImageToListSampleFilterType::Pointer filter 
                              = ImageToListSampleFilterType::New();

  filter->SetInput( image );
  filter->SetMaskImage( maskImage );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Verify here the output content : FIXME
  //
  //
  //
 
  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

