/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SliceContiguousImageTest01.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:51:52 $
  Version:   $Revision: 1.19 $

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

#include "itkImage.h"
#include "itkSliceContiguousImage.h"
#include "itkImageRegionIterator.h"
#include "itkShiftScaleImageFilter.h"

int main(int argc, char * argv [])
{
  // Typedefs
  const unsigned int SliceDimension = 2;
  const unsigned int Dimension = 3;
  typedef float PixelType;
  typedef itk::Image< PixelType, SliceDimension > SliceImageType;
  typedef itk::SliceContiguousImage< PixelType, Dimension > SliceContiguousImageType;
  typedef itk::Image< PixelType, Dimension > ContiguousImageType;
  //typedef itk::ImageRegionIterator< ContiguousImageType > IteratorType;
  typedef itk::ImageRegionIterator< SliceContiguousImageType > IteratorType;

  // Create some slices
  SliceImageType::IndexType startSlice;
  startSlice[0]  = 0;  startSlice[1]  = 0;
  SliceImageType::SizeType  sizeSlice; 
  sizeSlice[0] = 200; sizeSlice[1] = 200;
  SliceImageType::RegionType regionSlice;
  regionSlice.SetSize( sizeSlice );
  regionSlice.SetIndex( startSlice );
  SliceImageType::Pointer slice1 = SliceImageType::New();
  slice1->SetRegions( regionSlice );
  slice1->Allocate();
  slice1->FillBuffer( 1 );
  SliceImageType::Pointer slice2 = SliceImageType::New();
  slice2->SetRegions( regionSlice );
  slice2->Allocate();
  slice2->FillBuffer( 2 );
  SliceImageType::Pointer slice3 = SliceImageType::New();
  slice3->SetRegions( regionSlice );
  slice3->Allocate();
  slice3->FillBuffer( 3 );
  SliceImageType::Pointer slice4 = SliceImageType::New();
  slice4->SetRegions( regionSlice );
  slice4->Allocate();
  slice4->FillBuffer( 4 );

  // Create image
  SliceContiguousImageType::Pointer image = SliceContiguousImageType::New();
  SliceContiguousImageType::IndexType start;
  SliceContiguousImageType::SizeType  size; 
  size[0]  = 20;   // size along X
  size[1]  = 20;   // size along Y
  size[2]  = 3;    // size along Z 
  start[0] =   0;  // first index on X
  start[1] =   0;  // first index on Y
  start[2] =   0;  // first index on Z 
  SliceContiguousImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  image->SetRegions( region );
  image->Allocate( );
  
  // Set slice pointers
  SliceContiguousImageType::PixelContainer::SliceArrayType slices;
  slices.push_back( slice2->GetBufferPointer() );
  slices.push_back( slice3->GetBufferPointer() );
  slices.push_back( slice1->GetBufferPointer() );
  SliceContiguousImageType::PixelContainerPointer container = SliceContiguousImageType::PixelContainer::New();  
  container->SetImportPointersForSlices( slices, size[0]*size[1], 3, false );
  image->SetPixelContainer( container );

  typedef itk::ShiftScaleImageFilter<SliceContiguousImageType, ContiguousImageType> ShiftScaleFilterType;
  ShiftScaleFilterType::Pointer filter = ShiftScaleFilterType::New();
  filter->SetInput( image );
  filter->SetShift( 10 );
  filter->Update();  

  // Iterate image
  SliceContiguousImageType::RegionType region1;
  region1.SetSize( size );
  region1.SetIndex( start );
  //IteratorType it( filter->GetOutput(), region1 );
  IteratorType it( image, region );
  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index = it.GetIndex();
    if (index[0] == 10 && index[1] == 10)
      {
      PixelType p = it.Get();
      std::stringstream ss;
      ss << p;
      std::cout << index << ": " <<  ss.str() << std::endl;
      }
    ++it;
    }

  unsigned int i = 0;
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index = it.GetIndex();
    it.Set( i );
    i += 1;
    ++it;
    }

  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    SliceContiguousImageType::IndexType index1 = it.GetIndex();
    if (index1[0] == 10 && index1[1] == 10)
      {
      PixelType p1 = it.Get();
      std::stringstream ss;
      ss << p1;
      std::cout << index1 << ": " <<  ss.str() << std::endl;
      }
    ++it;
    }

  return EXIT_SUCCESS;
}

