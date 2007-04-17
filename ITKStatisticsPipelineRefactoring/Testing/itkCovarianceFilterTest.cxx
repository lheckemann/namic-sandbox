/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:18:41 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleFilter.h"
#include "itkCovarianceFilter.h"
#include "itkImageRegionIterator.h"
#include "itkFixedArray.h"
#include "itkVector.h"

int itkCovarianceFilterTest(int, char* [] ) 
{
  std::cout << "CovarianceFilter Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;

  // Now generate an image
  enum { MeasurementVectorSize = 3 } ;
  typedef float MeasurementType ;
  typedef itk::FixedArray< MeasurementType, MeasurementVectorSize > 
    MeasurementVectorType ;
  typedef itk::Image< MeasurementVectorType, 3 > ImageType ;
  ImageType::Pointer image = ImageType::New() ;
  ImageType::RegionType region ;
  ImageType::SizeType size ;
  ImageType::IndexType index ;
  index.Fill(0) ;
  size.Fill(5) ;
  region.SetIndex(index) ;
  region.SetSize(size) ;
  
  
  image->SetBufferedRegion(region) ;
  image->Allocate() ;

  typedef itk::ImageRegionIterator< ImageType > ImageIterator ;
  ImageIterator iter(image, region) ;

  unsigned int count = 0 ;
  MeasurementVectorType temp ;
  // fill the image
  while (!iter.IsAtEnd())
    {
    temp[0] = count ;
    iter.Set(temp) ;
    ++iter ;
    ++count ;
    }

  // creates an ImageToListAdaptor object
  typedef  itk::Statistics::ImageToListSampleFilter< ImageType, ImageType >
                                     ImageToListSampleFilterType ;

  ImageToListSampleFilterType::Pointer sampleGeneratingFilter
                            = ImageToListSampleFilterType::New() ;

  sampleGeneratingFilter->SetInput( image ) ;

  try
    {
    sampleGeneratingFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr<< excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef ImageToListSampleFilterType::ListSampleType                 ListSampleType;

  typedef itk::Statistics::CovarianceFilter< ListSampleType >         CovarianceFilterType;

  CovarianceFilterType::Pointer covarianceFilter = CovarianceFilterType::New() ;

  std::cout << covarianceFilter->GetNameOfClass() << std::endl;
  
  //Invoke update before adding an input. An exception should be 
  try
    {
    covarianceFilter->Update();
    std::cerr << "Exception should have been thrown since \
                 Update() is invoked without setting an input " << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }    

  covarianceFilter->ResetPipeline();

  if ( covarianceFilter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should return NULL if the input \
                     has not been set" << std::endl;
    return EXIT_FAILURE;
    }

  covarianceFilter->SetInput( sampleGeneratingFilter->GetOutput() ) ;
  try
    {
    covarianceFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr<< excp << std::endl;
    return EXIT_FAILURE;
    }

  covarianceFilter->Print( std::cout );

  // CHECK THE RESULTS
  if( !pass )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



