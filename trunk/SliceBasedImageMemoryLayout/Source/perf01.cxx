/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: perf01.cxx,v $
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

#include "itkImage.h"
#include "itkSparseImage.h"
#include "itkSliceContiguousImage.h"
#include "itkSingleBitBinaryImage.h"
#include "itkImageRegionIterator.h"
#include "itkTimeProbe.h"

int main(int argc, char * argv [])
{
  // Typedefs
  const unsigned int Dimension = 3;
  const unsigned int SliceDimension = 2;
  typedef unsigned short PixelType;
  typedef itk::Image< PixelType, Dimension > ContiguousImageType;
  typedef itk::Image< PixelType, SliceDimension > SliceImageType;
  typedef itk::SparseImage< PixelType, Dimension > SparseImageType;
  typedef itk::SliceContiguousImage< PixelType > SliceContiguousImageType;
  typedef itk::SingleBitBinaryImage< Dimension > BinaryImageType;
  typedef itk::ImageRegionIterator< ContiguousImageType > ContiguousIteratorType;
  typedef itk::ImageRegionIterator< SparseImageType > SparseIteratorType;
  typedef itk::ImageRegionIterator< ContiguousImageType > ContiguousIteratorType;
  typedef itk::ImageRegionIterator< SliceContiguousImageType > SliceContiguousIteratorType;
  typedef itk::ImageRegionIterator< BinaryImageType > BinaryIteratorType;

  PixelType value = 123;
  unsigned int Iterations = 5;
  unsigned int count = 0;

  // Create probes
  itk::TimeProbe probeAllocate1;
  itk::TimeProbe probeFillBuffer1;
  itk::TimeProbe probeIterateSet1;
  itk::TimeProbe probeIterateGet1;
  itk::TimeProbe probeAllocate2;
  itk::TimeProbe probeFillBuffer2;
  itk::TimeProbe probeIterateSet2;
  itk::TimeProbe probeIterateGet2;
  itk::TimeProbe probeAllocate3;
  itk::TimeProbe probeFillBuffer3;
  itk::TimeProbe probeIterateSet3;
  itk::TimeProbe probeIterateGet3;
  itk::TimeProbe probeAllocate4;
  itk::TimeProbe probeFillBuffer4;
  itk::TimeProbe probeIterateSet4;
  itk::TimeProbe probeIterateGet4;

  // Specify index/size
  itk::Index< Dimension > start;
  start.Fill( 0 );
  itk::Size< Dimension > size;
  size.Fill( 350 );
  itk::ImageRegion< Dimension > region( start, size );

  // Time normal contiguous image
  for (count = 0; count < Iterations; count++)
    {
    ContiguousImageType::Pointer imageContiguous = ContiguousImageType::New();
    imageContiguous->SetRegions( region );
    probeAllocate1.Start();
    imageContiguous->Allocate( );
    probeAllocate1.Stop();
    probeFillBuffer1.Start();
    imageContiguous->FillBuffer( value );
    probeFillBuffer1.Stop();
    ContiguousIteratorType itContiguous( imageContiguous, region );
    itContiguous.GoToBegin();
    probeIterateSet1.Start();
    while( !itContiguous.IsAtEnd() )
      {
      itContiguous.Set( value );
      ++itContiguous;
      }
    probeIterateSet1.Stop();
    itContiguous.GoToBegin();
    probeIterateGet1.Start();
    while( !itContiguous.IsAtEnd() )
      {
      assert( itContiguous.Get() == value );
      ++itContiguous;
      }
    probeIterateGet1.Stop();
    }
  std::cout << "Contiguous Allocate: " << probeAllocate1.GetMeanTime() << std::endl;
  std::cout << "Contiguous FillBuffer: " << probeFillBuffer1.GetMeanTime() << std::endl;
  std::cout << "Contiguous IterateSet: " << probeIterateSet1.GetMeanTime() << std::endl;
  std::cout << "Contiguous IterateGet: " << probeIterateGet1.GetMeanTime() << std::endl;

  // Time slice contiguous image
  for (count = 0; count < Iterations; count++)
    {
    SliceContiguousImageType::Pointer imageSliceContiguous = SliceContiguousImageType::New();
    imageSliceContiguous->SetRegions( region );
    probeAllocate3.Start();
    std::vector< SliceImageType::Pointer > sliceImages;
    SliceContiguousImageType::PixelContainer::SliceArrayType slicePointers;
    for (unsigned int i=0; i<size[2]; i++)
      {
        SliceImageType::Pointer sliceImage = SliceImageType::New();
        itk::Index< SliceDimension > startSlice;
        startSlice.Fill( 0 );
        itk::Size< SliceDimension > sizeSlice;
        sizeSlice[0] = size[0]; sizeSlice[1] = size[1];
        itk::ImageRegion< SliceDimension > regionSlice( startSlice, sizeSlice );
        sliceImage->SetRegions( regionSlice );
        sliceImage->Allocate( );
        sliceImages.push_back( sliceImage );
        slicePointers.push_back( sliceImage->GetBufferPointer() );
      }
    SliceContiguousImageType::PixelContainerPointer container =
      SliceContiguousImageType::PixelContainer::New();
    container->SetImportPointersForSlices( slicePointers, size[0]*size[1], false );
    imageSliceContiguous->SetPixelContainer( container );
    probeAllocate3.Stop();
    probeFillBuffer3.Start();
    imageSliceContiguous->FillBuffer( value );
    probeFillBuffer3.Stop();
    SliceContiguousIteratorType itSliceContiguous( imageSliceContiguous, region );
    itSliceContiguous.GoToBegin();
    probeIterateSet3.Start();
    while( !itSliceContiguous.IsAtEnd() )
      {
      itSliceContiguous.Set( value );
      ++itSliceContiguous;
      }
    probeIterateSet3.Stop();
    itSliceContiguous.GoToBegin();
    probeIterateGet3.Start();
    while( !itSliceContiguous.IsAtEnd() )
      {
      assert( itSliceContiguous.Get() == value );
      ++itSliceContiguous;
      }
    probeIterateGet3.Stop();
    }
  std::cout << "SliceContiguous Allocate: " << probeAllocate3.GetMeanTime() << std::endl;
  std::cout << "SliceContiguous FillBuffer: " << probeFillBuffer3.GetMeanTime() << std::endl;
  std::cout << "SliceContiguous IterateSet: " << probeIterateSet3.GetMeanTime() << std::endl;
  std::cout << "SliceContiguous IterateGet: " << probeIterateGet3.GetMeanTime() << std::endl;

  // Time sparse image
  for (count = 0; count < Iterations; count++)
    {
    SparseImageType::Pointer imageSparse = SparseImageType::New();
    imageSparse->SetRegions( region );
    probeAllocate2.Start();
    imageSparse->Allocate( );
    probeAllocate2.Stop();
    probeFillBuffer2.Start();
    imageSparse->FillBuffer( value );
    probeFillBuffer2.Stop();
    SparseIteratorType itSparse( imageSparse, region );
    itSparse.GoToBegin();
    probeIterateSet2.Start();
    while( !itSparse.IsAtEnd() )
      {
      itSparse.Set( value );
      ++itSparse;
      }
    probeIterateSet2.Stop();
    itSparse.GoToBegin();
    probeIterateGet2.Start();
    while( !itSparse.IsAtEnd() )
      {
      assert( itSparse.Get() == value );
      ++itSparse;
      }
    probeIterateGet2.Stop();
    }
  std::cout << "Sparse Allocate: " << probeAllocate2.GetMeanTime() << std::endl;
  std::cout << "Sparse FillBuffer: " << probeFillBuffer2.GetMeanTime() << std::endl;
  std::cout << "Sparse IterateSet: " << probeIterateSet2.GetMeanTime() << std::endl;
  std::cout << "Sparse IterateGet: " << probeIterateGet2.GetMeanTime() << std::endl;

  // Time binay image
  for (count = 0; count < Iterations; count++)
    {
    BinaryImageType::Pointer imageBinary = BinaryImageType::New();
    imageBinary->SetRegions( region );
    probeAllocate4.Start();
    imageBinary->Allocate( );
    probeAllocate4.Stop();
    probeFillBuffer4.Start();
    imageBinary->FillBuffer( value );
    probeFillBuffer4.Stop();
    BinaryIteratorType itBinary( imageBinary, region );
    itBinary.GoToBegin();
    probeIterateSet4.Start();
    while( !itBinary.IsAtEnd() )
      {
      itBinary.Set( true );
      ++itBinary;
      }
    probeIterateSet4.Stop();
    itBinary.GoToBegin();
    probeIterateGet4.Start();
    while( !itBinary.IsAtEnd() )
      {
      assert( itBinary.Get() == true );
      ++itBinary;
      }
    probeIterateGet4.Stop();
    }
  std::cout << "Binary Allocate: " << probeAllocate4.GetMeanTime() << std::endl;
  std::cout << "Binary FillBuffer: " << probeFillBuffer4.GetMeanTime() << std::endl;
  std::cout << "Binary IterateSet: " << probeIterateSet4.GetMeanTime() << std::endl;
  std::cout << "Binary IterateGet: " << probeIterateGet4.GetMeanTime() << std::endl;

  return EXIT_SUCCESS;
}

