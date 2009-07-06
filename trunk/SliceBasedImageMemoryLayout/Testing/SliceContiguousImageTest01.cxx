/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SliceContiguousImageTest01.cxx,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include <sstream>

#include "itkTestMain.h"

#include "itkImage.h"
#include "itkSliceContiguousImage.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkShiftScaleImageFilter.h"

void RegisterTests()
{
  REGISTER_TEST(test01);
}

int test01(int argc, char * argv [])
{
  // Typedefs
  const unsigned int SliceDimension = 2;
  const unsigned int Dimension = 3;
  typedef float PixelType;
  typedef itk::Image< PixelType, SliceDimension > SliceImageType;
  typedef itk::SliceContiguousImage< PixelType > SliceContiguousImageType;
  typedef itk::Image< PixelType, Dimension > ContiguousImageType;
  typedef itk::ImageRegionIterator< ContiguousImageType > ContiguousIteratorType;
  typedef itk::ImageRegionIterator< SliceContiguousImageType > SliceContiguousIteratorType;
  typedef itk::ImageRegionIterator< SliceContiguousImageType > IteratorType;
  typedef itk::NeighborhoodIterator< SliceContiguousImageType > NeighborhoodIteratorType;

  // Create some slices
  SliceImageType::IndexType startSlice;
  startSlice[0] = 0; startSlice[1] = 0;
  SliceImageType::SizeType  sizeSlice;
  sizeSlice[0] = 256; sizeSlice[1] = 256;
  SliceImageType::RegionType regionSlice;
  regionSlice.SetSize( sizeSlice );
  regionSlice.SetIndex( startSlice );
  SliceImageType::Pointer slice1 = SliceImageType::New();
  slice1->SetRegions( regionSlice );
  slice1->Allocate();
  slice1->FillBuffer( 1 );
  void* temp1 = malloc(1000); // Allocate some memory to ensure slices are non-contiguous
  SliceImageType::Pointer slice2 = SliceImageType::New();
  slice2->SetRegions( regionSlice );
  slice2->Allocate();
  slice2->FillBuffer( 2 );
  void* temp2 = malloc(1000); // Allocate some memory to ensure slices are non-contiguous
  SliceImageType::Pointer slice3 = SliceImageType::New();
  slice3->SetRegions( regionSlice );
  slice3->Allocate();
  slice3->FillBuffer( 3 );
  void* temp3 = malloc(1000); // Allocate some memory to ensure slices are non-contiguous
  SliceImageType::Pointer slice4 = SliceImageType::New();
  slice4->SetRegions( regionSlice );
  slice4->Allocate();
  slice4->FillBuffer( 4 );

  // Create image
  SliceContiguousImageType::Pointer image = SliceContiguousImageType::New();
  SliceContiguousImageType::IndexType start;
  SliceContiguousImageType::SizeType size;
  start.Fill( 0 );
  size[0] = sizeSlice[0]; size[1] = sizeSlice[1]; size[2] = 4;
  SliceContiguousImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate( );

  // Set slice pointers
  SliceContiguousImageType::PixelContainer::SliceArrayType slices;
  slices.push_back( slice2->GetBufferPointer() );
  slices.push_back( slice4->GetBufferPointer() );
  slices.push_back( slice3->GetBufferPointer() );
  slices.push_back( slice1->GetBufferPointer() );
  SliceContiguousImageType::PixelContainerPointer container =
    SliceContiguousImageType::PixelContainer::New();  
  container->SetImportPointersForSlices( slices, size[0]*size[1], false );
  image->SetPixelContainer( container );

  // Create a mapping between slice index and value
  float values[4] = { 2.0, 4.0, 3.0, 1.0 };

  // Test Get/SetPixel
  SliceContiguousImageType::IndexType index0;
  index0[0] = 10; index0[1] = 10; index0[2] = 0;
  image->SetPixel( index0, 10.0 );
  PixelType pixel0 = image->GetPixel( index0 );
  assert( pixel0 == 10.0 );
  image->SetPixel( index0, values[0] );
  pixel0 = image->GetPixel( index0 );
  assert( pixel0 == values[0] );

  SliceContiguousImageType::IndexType index1;
  index1[0] = 10; index1[1] = 10; index1[2] = 1;
  image->SetPixel( index1, 10.0 );
  PixelType pixel1 = image->GetPixel( index1 );
  assert( pixel1 == 10.0 );
  image->SetPixel( index1, values[1] );
  pixel1 = image->GetPixel( index1 );
  assert( pixel1 == values[1] );

  SliceContiguousImageType::IndexType index2;
  index2[0] = 10; index2[1] = 10; index2[2] = 2;
  image->SetPixel( index2, 10.0 );
  PixelType pixel2 = image->GetPixel( index2 );
  assert( pixel2 == 10.0 );
  image->SetPixel( index2, values[2] );
  pixel2 = image->GetPixel( index2 );
  assert( pixel2 == values[2] );

  SliceContiguousImageType::IndexType index3;
  index3[0] = 10; index3[1] = 10; index3[2] = 3;
  image->SetPixel( index3, 10.0 );
  PixelType pixel3 = image->GetPixel( index3 );
  assert( pixel3 == 10.0 );
  image->SetPixel( index3, values[3] );
  pixel3 = image->GetPixel( index3 );
  assert( pixel3 == values[3] );

  // Use the image with a filter
  typedef itk::ShiftScaleImageFilter<SliceContiguousImageType,
    SliceContiguousImageType> ShiftScaleFilterType;
  ShiftScaleFilterType::Pointer filter = ShiftScaleFilterType::New();
  filter->SetInput( image );
  filter->SetShift( 10 );
  filter->Update( );

  // Iterate filter output
  SliceContiguousImageType::RegionType region1;
  region1.SetSize( size );
  region1.SetIndex( start );
  SliceContiguousIteratorType it1( filter->GetOutput(), region1 );
  it1.GoToBegin();

  while( !it1.IsAtEnd() )
    {
    ContiguousImageType::IndexType index = it1.GetIndex();
    assert( it1.Get() == (values[index[2]] + 10.0) );
    ++it1;
    }

  // Test const iterator
  IteratorType it( image, region );
  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index = it.GetIndex();
    assert( it.Get() == values[index[2]] );
    ++it;
    }

  // Test const neighborhood iterator
  SliceContiguousImageType::SizeType radius;
  radius.Fill( 1 );
  NeighborhoodIteratorType nit( radius, image, region );
  nit.GoToBegin();
  NeighborhoodIteratorType::OffsetType offset1 = {{0,0,-1}};
  NeighborhoodIteratorType::OffsetType offset2 = {{0,0,+1}};

  while( !nit.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index = nit.GetIndex();
    PixelType p = nit.GetCenterPixel();
    PixelType p1 = nit.GetPixel( offset1 );
    PixelType p2 = nit.GetPixel( offset2 );
    assert( p == values[index[2]] );
    if (index[2] == 0) assert( p1 == values[0] );
    else assert( p1 == values[index[2]-1] );
    if (index[2] == 3) assert( p2 == values[3] );
    else assert( p2 == values[index[2]+1] );
    ++nit;
    }

  // Test iterator
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    it.Set( it.Get() * 10 );
    ++it;
    }

  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index = it.GetIndex();
    assert( it.Get() == (values[index[2]]*10.0) );
    ++it;
    }

  // Test fill buffer
  image->FillBuffer( 100.0 );
  pixel0 = image->GetPixel( index0 );
  assert( pixel0 == 100.0 );
  pixel1 = image->GetPixel( index1 );
  assert( pixel1 == 100.0 );
  pixel2 = image->GetPixel( index2 );
  assert( pixel2 == 100.0 );
  pixel3 = image->GetPixel( index3 );
  assert( pixel3 == 100.0 );

  // Clean up
  free(temp1);
  free(temp2);
  free(temp3);

  return EXIT_SUCCESS;
}

