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

#include "itkJointDomainImageToListSampleAdaptor.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkVariableLengthVector.h"

int itkJointDomainImageToListSampleAdaptorTest(int, char* [] ) 
{
  const unsigned int MeasurementVectorSize = 8;
  typedef unsigned long MeasurementComponentType;
  typedef itk::FixedArray< MeasurementComponentType, MeasurementVectorSize > PixelType;

  const unsigned int ImageDimension = 3;
  typedef itk::Image< PixelType, ImageDimension > ImageType;

  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start = {0,0,0};
  ImageType::SizeType  size = {10,10,10};
  unsigned long totalSize = size[0] * size[1] * size[2] ;
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
  typedef itk::Statistics::JointDomainImageToListSampleAdaptor< 
    ImageType > JointDomainImageToListSampleAdaptorType;
  JointDomainImageToListSampleAdaptorType::Pointer adaptor 
                              = JointDomainImageToListSampleAdaptorType::New();

  adaptor->SetImage ( image );  

  //check size
  if (totalSize != adaptor->Size())
    {
    std::cerr << "Size() is not returning the correct size"<< std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

