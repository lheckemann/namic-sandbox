/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SparseImageTest01.cxx,v $
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
#include "itkSparseImage.h"
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
  const unsigned int Dimension = 2;
  typedef unsigned short PixelType;
  typedef itk::SparseImage< PixelType, Dimension > SparseImageType;
  typedef itk::Image< PixelType, Dimension > ContiguousImageType;
  typedef itk::ImageRegionIterator< SparseImageType > SparseIteratorType;
  typedef itk::ImageRegionIterator< ContiguousImageType > ContiguousIteratorType;
  typedef itk::NeighborhoodIterator< SparseImageType > SparseNeighborhoodIteratorType;

  // Create image
  PixelType value = 123;
  SparseImageType::Pointer image = SparseImageType::New();
  SparseImageType::IndexType start;
  start.Fill( 0 );
  SparseImageType::SizeType size;
  size.Fill( 1000000 ); // try this with a "normal" image!
  SparseImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate( );
  image->FillBuffer( value );

  // Create a sub region for operations
  SparseImageType::IndexType substart;
  substart.Fill( 100 );
  SparseImageType::SizeType subsize;
  subsize.Fill( 100 );
  SparseImageType::RegionType subregion;
  subregion.SetSize( subsize );
  subregion.SetIndex( substart );

  // Test Get/SetPixel
  SparseImageType::IndexType index0;
  index0.Fill( 100 );
  image->SetPixel( index0, 5 );
  PixelType pixel0 = image->GetPixel( index0 );
  assert( pixel0 == 5 );
  image->SetPixel( index0, value );
  pixel0 = image->GetPixel( index0 );
  assert( pixel0 == value );

  SparseImageType::IndexType index1;
  index1.Fill( 15 );
  PixelType pixel1 = image->GetPixel( index1 );
  assert( pixel1 == value );

  // Test using filter
  typedef itk::ShiftScaleImageFilter<SparseImageType, SparseImageType>
    ShiftScaleFilterType;
  ShiftScaleFilterType::Pointer filter = ShiftScaleFilterType::New();
  filter->SetInput( image );
  filter->SetShift( 10 );
  SparseImageType::Pointer output = filter->GetOutput();
  output->SetRequestedRegion( subregion );
  output->FillBuffer( 0 );
  filter->SetNumberOfThreads( 1 ); // IMPORTANT!!!
  filter->Update();

  // Iterate subregion of filter output
  SparseIteratorType it1( output, subregion );
  it1.GoToBegin();

  while( !it1.IsAtEnd() )
    {
    SparseImageType::IndexType index = it1.GetIndex();
    assert( it1.Get() == (value + 10) );
    ++it1;
    }

  // Test const iterator
  SparseImageType::IndexType substart1;
  substart1.Fill( 50 );
  subregion.SetIndex( substart1 );
  SparseIteratorType it( output, subregion );
  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    SparseImageType::IndexType index = it.GetIndex();
    if ( index[0] >= 100 && index[1] >= 100 )
      {
      assert( it.Get() == (value + 10) );
      }
    else
      {
      assert( it.Get() == 0 );
      }
    ++it;
    }

  SparseImageType::SizeType radius;
  radius.Fill( 1 );
  SparseNeighborhoodIteratorType nit( radius, output, subregion );
  nit.GoToBegin();
  SparseNeighborhoodIteratorType::OffsetType offset1 = {{1,1}};

  while( !nit.IsAtEnd() )
    {
    SparseImageType::IndexType index = nit.GetIndex();
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
    it.Set( (it.Get()+1) * 10 );
    ++it;
    }

  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    SparseImageType::IndexType index = it.GetIndex();
    if ( index[0] >= 100 && index[1] >= 100 )
      {
      assert( it.Get() == ((value+11) * 10) );
      }
    else
      {
      assert( it.Get() == 10 );
      }
    ++it;
    }

  // Test fill buffer
  image->FillBuffer( 100 );
  SparseImageType::IndexType indexA = {100, 100};
  PixelType pixelA = image->GetPixel( indexA );
  assert( pixelA == 100 );
  SparseImageType::IndexType indexB = {10000, 10000};
  image->SetPixel( indexB, 5 );
  PixelType pixelB = image->GetPixel( indexB );
  assert( pixelB == 5 );

  return EXIT_SUCCESS;
}

