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
#include "itkImageClassifierFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkFixedArray.h"

int itkImageClassifierFilterTest(int, char* [] ) 
{
  const unsigned int MeasurementVectorSize = 1;
  typedef unsigned long MeasurementComponentType;
  typedef itk::FixedArray< MeasurementComponentType, MeasurementVectorSize > PixelType;

  const unsigned int ImageDimension = 2;
  typedef itk::Image< PixelType, ImageDimension > InputImageType;

  typedef unsigned long OutputPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  

  InputImageType::Pointer image = InputImageType::New();

  InputImageType::IndexType start;
  InputImageType::SizeType  size;

  start.Fill( 0 );
  size.Fill( 10 );

  InputImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  typedef itk::ImageRegionIteratorWithIndex< InputImageType > IteratorType;
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

  //define ImageClassifier filter type 
  typedef itk::Statistics::ListSample< PixelType > SampleType;

  typedef itk::Statistics::ImageClassifierFilter< SampleType, 
    InputImageType,OutputImageType > ImageClassifierFilterType;
  ImageClassifierFilterType::Pointer filter 
                              = ImageClassifierFilterType::New();

  //filter->SetInput( image );

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

