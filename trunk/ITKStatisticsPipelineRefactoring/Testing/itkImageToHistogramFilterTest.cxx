/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToHistogramGeneratorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:18:41 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImageToHistogramFilter.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageRegionIteratorWithIndex.h"

int itkImageToHistogramFilterTest( int , char * [] )
{


  typedef unsigned char                         PixelComponentType;

  typedef itk::RGBPixel< PixelComponentType >   RGBPixelType;

  const unsigned int                            Dimension = 3;

  typedef itk::Image< RGBPixelType, Dimension > RGBImageType;

  const unsigned int MeasurementVectorSize = 3; // RGB

  RGBImageType::Pointer image = RGBImageType::New();

  RGBImageType::RegionType region;
  RGBImageType::SizeType   size;
  RGBImageType::IndexType  start;

  size[0] = 127;
  size[1] = 127;
  size[2] = 127;
  
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  region.SetIndex( start );
  region.SetSize( size );

  image->SetRegions( region );
  image->Allocate();
  
  

  //  Now fill up the image will all the combinations of RGB 
  //  values from 0-255 on each channel.
  itk::ImageRegionIteratorWithIndex< RGBImageType > it( image, region );
  it.GoToBegin();

  RGBPixelType pixel;
  RGBImageType::IndexType index;

  while( !it.IsAtEnd() )
    {
    index = it.GetIndex();
    pixel.SetRed(   index[0] );
    pixel.SetGreen( index[1] );
    pixel.SetBlue(  index[2] );
    it.Set( pixel );
    ++it;
    }

  typedef itk::Statistics::ImageToHistogramFilter< RGBImageType >   HistogramFilterType;

  HistogramFilterType::Pointer filter = HistogramFilterType::New();


  typedef HistogramFilterType::HistogramMeasurementVectorType             HistogramMeasurementVectorType;
  typedef HistogramFilterType::InputHistogramMeasurementVectorObjectType  InputHistogramMeasurementVectorObjectType;

  // Testing the settings of the BinMaximum and BinMinimum methods.
  HistogramMeasurementVectorType histogramBinMinimum1( MeasurementVectorSize );
  histogramBinMinimum1[0] = 0;
  histogramBinMinimum1[1] = 0;
  histogramBinMinimum1[2] = 0;

  HistogramMeasurementVectorType histogramBinMinimum2( MeasurementVectorSize );
  histogramBinMinimum2[0] = 17;
  histogramBinMinimum2[1] = 17;
  histogramBinMinimum2[2] = 17;

  filter->SetHistogramBinMinimum( histogramBinMinimum1 );

  const InputHistogramMeasurementVectorObjectType * returnedHistogramBinMinimumObject =
    filter->GetHistogramBinMinimumInput();

  if( returnedHistogramBinMinimumObject == NULL )
    {
    std::cerr << "SetHistogramSize() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  HistogramMeasurementVectorType returnedHistogramBinMinimum =
    returnedHistogramBinMinimumObject->Get();

  for( unsigned int k1 = 0; k1 < MeasurementVectorSize; k1++ )
    {
    if( returnedHistogramBinMinimum[k1] != histogramBinMinimum1[k1] )
      {
      std::cerr << "Get/Set HistogramBinMinimum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetHistogramBinMinimum( histogramBinMinimum2 );

  returnedHistogramBinMinimumObject = filter->GetHistogramBinMinimumInput();

  returnedHistogramBinMinimum = returnedHistogramBinMinimumObject->Get();

  for( unsigned int k2 = 0; k2 < MeasurementVectorSize; k2++ )
    {
    if( returnedHistogramBinMinimum[k2] != histogramBinMinimum2[k2] )
      {
      std::cerr << "Get/Set HistogramSize() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  InputHistogramMeasurementVectorObjectType::Pointer histogramBinMinimumObject =
    InputHistogramMeasurementVectorObjectType::New();

  histogramBinMinimumObject->Set( histogramBinMinimum1 );

  filter->SetHistogramBinMinimumInput( histogramBinMinimumObject );

  returnedHistogramBinMinimumObject = filter->GetHistogramBinMinimumInput();

  if( returnedHistogramBinMinimumObject != histogramBinMinimumObject )
    {
    std::cerr << "Get/Set HistogramBinMinimum() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramBinMinimum = returnedHistogramBinMinimumObject->Get();

  for( unsigned int k3 = 0; k3 < MeasurementVectorSize; k3++ )
    {
    if( returnedHistogramBinMinimum[k3] != histogramBinMinimum1[k3] )
      {
      std::cerr << "Get/Set HistogramBinMinimum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }

  histogramBinMinimumObject->Set( histogramBinMinimum2 );

  filter->SetInput3( histogramBinMinimumObject );

  returnedHistogramBinMinimumObject = filter->GetInput3();

  if( returnedHistogramBinMinimumObject != histogramBinMinimumObject )
    {
    std::cerr << "Get/Set HistogramBinMinimum() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramBinMinimum = returnedHistogramBinMinimumObject->Get();

  for( unsigned int k4 = 0; k4 < MeasurementVectorSize; k4++ )
    {
    if( returnedHistogramBinMinimum[k4] != histogramBinMinimum2[k4] )
      {
      std::cerr << "Get/Set HistogramBinMinimum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetHistogramBinMinimum( histogramBinMinimum1 );

  HistogramMeasurementVectorType histogramBinMaximum1( MeasurementVectorSize );
  histogramBinMaximum1[0] = 0;
  histogramBinMaximum1[1] = 0;
  histogramBinMaximum1[2] = 0;

  HistogramMeasurementVectorType histogramBinMaximum2( MeasurementVectorSize );
  histogramBinMaximum2[0] = 17;
  histogramBinMaximum2[1] = 17;
  histogramBinMaximum2[2] = 17;


  filter->SetHistogramBinMaximum( histogramBinMaximum1 );

  const InputHistogramMeasurementVectorObjectType * returnedHistogramBinMaximumObject =
    filter->GetHistogramBinMaximumInput();

  if( returnedHistogramBinMaximumObject == NULL )
    {
    std::cerr << "SetHistogramSize() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  HistogramMeasurementVectorType returnedHistogramBinMaximum =
    returnedHistogramBinMaximumObject->Get();

  for( unsigned int k1 = 0; k1 < MeasurementVectorSize; k1++ )
    {
    if( returnedHistogramBinMaximum[k1] != histogramBinMaximum1[k1] )
      {
      std::cerr << "Get/Set HistogramBinMaximum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetHistogramBinMaximum( histogramBinMaximum2 );

  returnedHistogramBinMaximumObject = filter->GetHistogramBinMaximumInput();

  returnedHistogramBinMaximum = returnedHistogramBinMaximumObject->Get();

  for( unsigned int k2 = 0; k2 < MeasurementVectorSize; k2++ )
    {
    if( returnedHistogramBinMaximum[k2] != histogramBinMaximum2[k2] )
      {
      std::cerr << "Get/Set HistogramSize() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  InputHistogramMeasurementVectorObjectType::Pointer histogramBinMaximumObject =
    InputHistogramMeasurementVectorObjectType::New();

  histogramBinMaximumObject->Set( histogramBinMaximum1 );

  filter->SetHistogramBinMaximumInput( histogramBinMaximumObject );

  returnedHistogramBinMaximumObject = filter->GetHistogramBinMaximumInput();

  if( returnedHistogramBinMaximumObject != histogramBinMaximumObject )
    {
    std::cerr << "Get/Set HistogramBinMaximum() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramBinMaximum = returnedHistogramBinMaximumObject->Get();

  for( unsigned int k3 = 0; k3 < MeasurementVectorSize; k3++ )
    {
    if( returnedHistogramBinMaximum[k3] != histogramBinMaximum1[k3] )
      {
      std::cerr << "Get/Set HistogramBinMaximum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }

  histogramBinMaximumObject->Set( histogramBinMaximum2 );

  filter->SetInput3( histogramBinMaximumObject );

  returnedHistogramBinMaximumObject = filter->GetInput3();

  if( returnedHistogramBinMaximumObject != histogramBinMaximumObject )
    {
    std::cerr << "Get/Set HistogramBinMaximum() failed pointer consistency test" << std::endl;
    return EXIT_FAILURE;
    }

  returnedHistogramBinMaximum = returnedHistogramBinMaximumObject->Get();

  for( unsigned int k4 = 0; k4 < MeasurementVectorSize; k4++ )
    {
    if( returnedHistogramBinMaximum[k4] != histogramBinMaximum2[k4] )
      {
      std::cerr << "Get/Set HistogramBinMaximum() failed value consistency test" << std::endl;
      return EXIT_FAILURE;
      }
    }


  filter->SetInput(  image  );


  filter->SetHistogramBinMaximum( histogramBinMaximum1 );
  
  unsigned long modifiedTime = filter->GetMTime();
  filter->SetHistogramBinMaximum( histogramBinMaximum1 );

  if( filter->GetMTime() != modifiedTime )
    {
    std::cerr << "SetHistogramBinMaximum() failed modified Test 1" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetHistogramBinMaximum( histogramBinMaximum2 );

  if( filter->GetMTime() == modifiedTime )
    {
    std::cerr << "SetHistogramBinMaximum() failed modified Test 2" << std::endl;
    return EXIT_FAILURE;
    }
  typedef HistogramFilterType::HistogramSizeType   HistogramSizeType;

  HistogramSizeType hsize( MeasurementVectorSize );

  hsize[0] = 127;  // number of bins for the Red   channel
  hsize[1] =   1;  // number of bins for the Green channel
  hsize[2] =   1;  // number of bins for the Blue  channel



  filter->SetHistogramSize( hsize );
  filter->SetMarginalScale( 10.0 );
  filter->Update();

  typedef HistogramFilterType::HistogramType  HistogramType;

  const HistogramType * histogram = filter->GetOutput();

  const unsigned int histogramSize = histogram->Size();

  std::cout << "Histogram size " << histogramSize << std::endl;

  unsigned int channel = 0;  // red channel

  std::cout << "Histogram of the red component" << std::endl;

  const unsigned int expectedFrequency = 127 * 127;

  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    if( histogram->GetFrequency( bin, channel ) != expectedFrequency ) 
       {
       std::cerr << "Error in bin= " << bin << " channel = " << channel << std::endl;
       std::cerr << "Frequency was= " <<  histogram->GetFrequency( bin, channel ) << " Instead of the expected " << expectedFrequency << std::endl;
       return EXIT_FAILURE;
       }
    }


  // Now compute the histogram for the Green component
  hsize[0] =   1;  // number of bins for the Red   channel
  hsize[1] = 127;  // number of bins for the Green channel
  hsize[2] =   1;  // number of bins for the Blue  channel

  filter->SetHistogramSize( hsize );
  filter->SetMarginalScale( 10.0 );
  filter->Update();

  channel = 1;  // green channel

  std::cout << "Histogram of the green component" << std::endl;

  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    std::cout << "bin = " << bin << " frequency = ";
    std::cout << histogram->GetFrequency( bin, channel ) << std::endl;
    }


  // Now compute the histogram for the Blue component
  hsize[0] =   1;  // number of bins for the Red   channel
  hsize[1] =   1;  // number of bins for the Green channel
  hsize[2] = 127;  // number of bins for the Blue  channel

  filter->SetHistogramSize( hsize );
  filter->SetMarginalScale( 10.0 );
  filter->Update();

  channel = 2;  // blue channel

  std::cout << "Histogram of the blue component" << std::endl;

  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    std::cout << "bin = " << bin << " frequency = ";
    std::cout << histogram->GetFrequency( bin, channel ) << std::endl;
    }


  // Now compute the joint histogram for the three components
  hsize[0] = 127;  // number of bins for the Red   channel
  hsize[1] = 127;  // number of bins for the Green channel
  hsize[2] = 127;  // number of bins for the Blue  channel

  filter->SetHistogramSize( hsize );
  filter->SetMarginalScale( 10.0 );
  filter->Update();

  return EXIT_SUCCESS;
  
}


