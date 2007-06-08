/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToCooccurrenceListSampleFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:55:12 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImage.h"

#include "itkImageFileReader.h"
#include "itkScalarImageToCooccurrenceListSampleFilter.h"

int itkScalarImageToCooccurrenceListSampleFilterTest( int argc, char * argv [] )
{
//Data definitions 
  const unsigned int  IMGWIDTH         =  5;
  const unsigned int  IMGHEIGHT        =  5;
  const unsigned int  NDIMENSION       =  2;


  //------------------------------------------------------
  //Create a simple test images
  //------------------------------------------------------
  typedef itk::Image<unsigned int, NDIMENSION> InputImageType;

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
  //  4 5 6 7 8
  //  3 4 5 6 7
  //  2 3 4 5 6
  //  1 2 3 4 5
  //  0 1 2 3 4
  //--------------------------------------------------------------------------

  image->SetRegions( region );
  image->Allocate();

  // setup the iterator
  InputImageIterator imageIt( image, image->GetBufferedRegion() );

  for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++, ++imageIt)
      {
      imageIt.Set(i % 5 + j);
      }

  typedef itk::Statistics::ScalarImageToCooccurrenceListSampleFilter < 
                                  InputImageType > CooccurrenceListType;

  CooccurrenceListType::Pointer filter = CooccurrenceListType::New();

  filter->Print( std::cout );
  
  //Invoke update before adding an input. An exception should be 
  //thrown.
  try
    {
    filter->Update();
    std::cerr << "Failed to throw expected exception due to NULL input: " << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cout << "Expected exception caught: " << excp << std::endl;
    }    

  filter->ResetPipeline();

  if ( filter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should return NULL since the input is\
                  not set yet " << std::endl;
    return EXIT_FAILURE;
    }
 
  filter->SetInput(image);

  CooccurrenceListType::OffsetType offset = {{1,0}};

  filter->UseNeighbor(offset);

  filter->Update();

  const CooccurrenceListType::SampleType * sample = filter->GetOutput();

  typedef CooccurrenceListType::SampleType::ConstIterator ConstIteratorType;
  
  ConstIteratorType s_iter = sample->Begin();

  typedef CooccurrenceListType::SampleType::MeasurementVectorType MeasurementVectorType;

  while ( s_iter != sample->End() )
    {

    MeasurementVectorType  v = s_iter.GetMeasurementVector(); 

    std::cout << "[" << v[0] << "," << v[1] << "]" << std::endl;

    ++s_iter;
    } 

  return EXIT_SUCCESS;
  
}


