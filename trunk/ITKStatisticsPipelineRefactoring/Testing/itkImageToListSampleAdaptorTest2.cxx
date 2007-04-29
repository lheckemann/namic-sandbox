/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/01/15 18:38:35 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// The example tests the class itk::Statistics::ImageToListSampleAdaptor.

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleAdaptor.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkImageToListSampleAdaptorTest2(int, char* [] ) 
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

  //define an adaptor type 
  typedef itk::Statistics::ImageToListSampleAdaptor< 
    ImageType > ImageToListSampleAdaptorType;
  ImageToListSampleAdaptorType::Pointer adaptor 
                              = ImageToListSampleAdaptorType::New();

  adaptor->SetImage( image );

  ImageType::IndexType index;
  ImageType::PixelType pixel;

  ImageToListSampleAdaptorType::InstanceIdentifier id;

  for ( unsigned int i=0 ; i < size[2] ; i++ )
    for ( unsigned int j=0; j < size[1]; j++ )
      for ( unsigned int k=0; k < size[0]; k++ )  
      {
      index[0]=k;
      index[1]=j;
      index[2]=i;

      pixel = image->GetPixel( index );
      id = image->ComputeOffset( index );
      for ( unsigned int m=0; m < adaptor->GetMeasurementVectorSize(); m++ )
        {
        if ( adaptor->GetMeasurementVector(id)[m] != pixel[m] )
          {
          std::cerr << "Error in pixel value accessed using the adaptor" << std::endl;  
          return EXIT_FAILURE;
          }
        }
      }
  

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

