/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:18:41 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkImageToListSampleAdaptor.h"
#include "itkRandomImageSource.h"
#include "itkImageRegionIterator.h"
#include "itkScalarToArrayCastImageFilter.h"
#include "itkFixedArray.h"

int itkImageToListSampleAdaptorTest(int, char* [] ) 
{
  std::cout << "ImageToListSampleAdaptor Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;

  typedef itk::Image< float, 3 > FloatImage ;

  // Now generate a random image
  typedef itk::RandomImageSource<FloatImage> SourceType;
  SourceType::Pointer source = SourceType::New();
  unsigned long size[3] = {17, 8, 20} ;
  unsigned long totalSize = size[0] * size[1] * size[2] ;

  source->SetSize(size);
  float minValue = -100.0;
  float maxValue = 1000.0;

  source->SetMin( static_cast< FloatImage::PixelType >( minValue ) );
  source->SetMax( static_cast< FloatImage::PixelType >( maxValue ) );
  source->Update() ;

  // creat a new image with array pixel type from the source
  typedef itk::FixedArray< FloatImage::PixelType, 1 > ArrayPixelType ;
  typedef itk::Image< ArrayPixelType, 3 > ArrayPixelImageType ;
  typedef itk::ScalarToArrayCastImageFilter< FloatImage, ArrayPixelImageType >
    ImageCastFilterType ;
  ImageCastFilterType::Pointer castFilter = ImageCastFilterType::New() ;
  castFilter->SetInput(source->GetOutput()) ;
  castFilter->Update() ;

  // creates a sample
  typedef  itk::Statistics::ImageToListSampleAdaptor< ArrayPixelImageType>
    ImageToListSampleAdaptorType ;

  ImageToListSampleAdaptorType::Pointer sample = ImageToListSampleAdaptorType::New() ;

  //Test if the methods throw exceptions if invoked before setting the image
  try
    {
    unsigned long size = sample->Size();
    std::cerr << "Exception should have been thrown since the input image \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
  try
    {
    ImageToListSampleAdaptorType::FrequencyType totalFrequency = sample->GetTotalFrequency();
    std::cerr << "Exception should have been thrown since the input image \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
 
  try
    {
    ImageToListSampleAdaptorType::MeasurementVectorType m = sample->GetMeasurementVector( 0 );
    std::cerr << "Exception should have been thrown since the input image \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
 
  try
    {
    ImageToListSampleAdaptorType::ImageConstPointer image = sample->GetImage( );
    std::cerr << "Exception should have been thrown since the input image \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
 
 
  sample->SetImage(castFilter->GetOutput()) ;

  // tests begin

  //check size
  if (totalSize != sample->Size())
    {
    std::cerr << "Size() is not returning the correct size"<< std::endl;
    return EXIT_FAILURE;
    }

  //check frequency
  if (totalSize != sample->GetTotalFrequency())
    {
    std::cerr << "GetTotalFrequency() is not returning the correct frequency"<< std::endl;
    return EXIT_FAILURE;
    }


  sample->Print( std::cout );

  ArrayPixelImageType::IndexType index;
  ArrayPixelImageType::PixelType pixel;

  ImageToListSampleAdaptorType::InstanceIdentifier id;

  for ( unsigned int i=0 ; i < size[2] ; i++ )
    for ( unsigned int j=0; j < size[1]; j++ )
      for ( unsigned int k=0; k < size[0]; k++ )  
      {
      index[0]=k;
      index[1]=j;
      index[2]=i;

      pixel = sample->GetImage()->GetPixel( index );
      id = sample->GetImage()->ComputeOffset( index );
      for ( unsigned int m=0; m < sample->GetMeasurementVectorSize(); m++ )
        {
        if ( sample->GetMeasurementVector(id)[m] != pixel[m] )
          {
          std::cerr << "Error in pixel value accessed using the adaptor" << std::endl;  
          return EXIT_FAILURE;
          }
        }
      }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



