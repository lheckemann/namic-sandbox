/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SingleBitBinaryImageTest01.cxx,v $
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
#include "itkSingleBitBinaryImage.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkInvertIntensityImageFilter.h"

void RegisterTests()
{
  REGISTER_TEST(test01);
}

int test01(int argc, char * argv [])
{
  // Typedefs
  const unsigned int Dimension = 2;
  typedef bool PixelType;
  typedef itk::SingleBitBinaryImage< Dimension > BinaryImageType;
  typedef itk::Image< PixelType, Dimension > ContiguousImageType;
  typedef itk::ImageRegionIterator< ContiguousImageType > ContiguousIteratorType;
  typedef itk::ImageRegionIterator< BinaryImageType > BinaryIteratorType;
  typedef itk::NeighborhoodIterator< BinaryImageType > BinaryNeighborhoodIteratorType;
  typedef itk::InvertIntensityImageFilter< BinaryImageType > InvertFilterType;

  // Create image
  BinaryImageType::Pointer image = BinaryImageType::New();
  BinaryImageType::IndexType start;
  start.Fill( 0 );
  BinaryImageType::SizeType size;
  size.Fill( 65 );
  BinaryImageType::RegionType region(start, size);
  image->SetRegions( region );
  image->Allocate( );
  image->FillBuffer( true );

  // Test Get/SetPixel
  BinaryImageType::IndexType index0;
  index0.Fill( 2 );
  image->SetPixel( index0, false );
  PixelType pixel0 = image->GetPixel( index0 );
  assert( pixel0 == false );
  image->SetPixel( index0, true );
  pixel0 = image->GetPixel( index0 );
  assert( pixel0 == true );

  // Test const iterator
  image->SetPixel( index0, false );
  BinaryIteratorType it( image, region );
  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    BinaryImageType::IndexType index = it.GetIndex();
    if (index == index0)
      {
      assert( it.Get() == false );
      }
    else
      {
      assert( it.Get() == true );
      }
    ++it;
    }

  // Test using filter
  InvertFilterType::Pointer filter = InvertFilterType::New();
  filter->SetInput( image );
  filter->Update();

  // Iterate subregion of filter output
  BinaryIteratorType it1( filter->GetOutput(), region );
  it1.GoToBegin();
  it.GoToBegin();

  while( !it1.IsAtEnd() )
    {
    BinaryImageType::IndexType index = it1.GetIndex();
    assert( it1.Get() != it.Get() );
    ++it1;
    ++it;
    }

  // Test const iterator
  BinaryImageType::IndexType substart;
  substart.Fill( 2 );
  BinaryImageType::SizeType subsize;
  subsize.Fill( 5 );
  BinaryImageType::RegionType subregion( substart, subsize );
  BinaryIteratorType it2( image, subregion );
  it2.GoToBegin();

  while( !it2.IsAtEnd() )
    {
    BinaryImageType::IndexType index = it2.GetIndex();
    bool value = it2.Get();
    if (index[0] == 2 && index[1] == 2)
      {
      assert( value == false );
      }
    else
      {
      assert( value == true );
      }
    ++it2;
    }

  // Test const neighborhood iterator
  BinaryImageType::SizeType radius;
  radius.Fill( 1 );
  BinaryNeighborhoodIteratorType nit( radius, image, region );
  nit.GoToBegin();
  BinaryNeighborhoodIteratorType::OffsetType offset1 = {{1,1}};

  while( !nit.IsAtEnd() )
    {
    BinaryImageType::IndexType index = nit.GetIndex();
    PixelType p = nit.GetCenterPixel();
    PixelType p1 = nit.GetPixel( offset1 );
    if (p == p1)
      {
      // Do nothing, here to prevent warning about not using p and p1
      }
    ++nit;
    }

  // Test iterator
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    it.Set( !it.Get() );
    ++it;
    }

  // Test fill buffer
  image->FillBuffer( false );
  pixel0 = image->GetPixel( index0 );
  assert( pixel0 == false );

  return EXIT_SUCCESS;
}
