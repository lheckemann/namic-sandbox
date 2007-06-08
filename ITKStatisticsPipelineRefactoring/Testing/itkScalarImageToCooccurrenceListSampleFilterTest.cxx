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


  if( argc < 2 )
    {
    std::cerr << "Error: argument missing" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile " << std::endl;
    }

  typedef itk::Statistics::ScalarImageToCooccurrenceListSampleFilter < 
                                  InputImageType > CooccurrenceListType;

  CooccurrenceListType::Pointer list = CooccurrenceListType::New();
  list->SetInput(image);

  CooccurrenceListType::OffsetType offset0 = {{-1,0}};
  CooccurrenceListType::OffsetType offset1 = {{1,0}};
  CooccurrenceListType::OffsetType offset2 = {{0,-1}};
  CooccurrenceListType::OffsetType offset3 = {{0,1}};

  list->UseNeighbor(offset0);
  list->UseNeighbor(offset1);
  list->UseNeighbor(offset2);
  list->UseNeighbor(offset3);

  list->Update();
  std::cout << "ScalarImageToCooccurrenceListSampleFilterTest [PASSED]" << std::endl;

  return EXIT_SUCCESS;
  
}


