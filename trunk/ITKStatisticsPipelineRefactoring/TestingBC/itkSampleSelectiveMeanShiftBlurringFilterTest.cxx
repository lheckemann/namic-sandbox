/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleSelectiveMeanShiftBlurringFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2006-09-02 12:56:09 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkJointDomainImageToListAdaptor.h"
#include "itkSampleSelectiveMeanShiftBlurringFilter.h"
#include "itkHypersphereKernelMeanShiftModeSeeker.h"

int itkSampleSelectiveMeanShiftBlurringFilterTest(int argc, char* argv[] ) 
{
  std::cout << "SampleSelectiveMeanShiftBlurringFilter Test \n \n"; 

  if (argc < 3)
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage  outputImage" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< unsigned char, 2 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ImageReaderType ;
  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

  imageReader->SetFileName( argv[1] ) ;
  imageReader->Update() ;
  ImageType::Pointer image = imageReader->GetOutput() ;
  
  typedef itk::Statistics::JointDomainImageToListAdaptor< ImageType >
    ListSampleType ;
  
  ListSampleType::Pointer listSample = ListSampleType::New() ;
  listSample->SetImage( image ) ;

  ListSampleType::NormalizationFactorsType factors ;
  factors[0] = 4 ;
  factors[1] = 4 ;
  factors[2] = 8 ;
  listSample->SetNormalizationFactors( factors ) ;

  typedef itk::Statistics::HypersphereKernelMeanShiftModeSeeker< 
    ListSampleType > ModeSeekerType ;
  ModeSeekerType::Pointer modeSeeker = ModeSeekerType::New() ;
  modeSeeker->SetInputSample( listSample ) ;
  modeSeeker->SetSearchRadius( 1.0 ) ;

  typedef itk::Statistics::SampleSelectiveMeanShiftBlurringFilter< 
    ListSampleType > FilterType ;
  FilterType::Pointer filter = FilterType::New() ;
  filter->SetInputSample( listSample ) ;
  filter->SetMeanShiftModeSeeker( modeSeeker ) ;

  std::cout << "Length of measurement vectors in the list sample: " 
                << listSample->GetMeasurementVectorSize() << std::endl;
  FilterType::ComponentSelectionsType componentSelections( 
      listSample->GetMeasurementVectorSize(), false) ;
  componentSelections[2] = true ;
  filter->SetComponentSelections( componentSelections ) ;
  try
    {
    filter->Update() ;
    }
  catch ( ... )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  typedef ImageType OutputImageType ;
  typedef itk::ImageRegionIterator< OutputImageType > ImageIteratorType ;
  typedef ImageType::PixelType PixelType ;
  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType ;

  OutputImageType::Pointer outputImage = OutputImageType::New() ;
  outputImage->SetRegions( image->GetLargestPossibleRegion() ) ;
  outputImage->Allocate() ;
    
  ImageIteratorType io_iter( outputImage,
                             outputImage->GetLargestPossibleRegion() ) ;
  io_iter.GoToBegin() ;
    
  FilterType::OutputType::Pointer output = filter->GetOutput() ;
  FilterType::OutputType::Iterator fo_iter = output->Begin() ;
  FilterType::OutputType::Iterator fo_end = output->End() ;
    
  while ( fo_iter != fo_end )
    {
    io_iter.Set
      ((PixelType) (factors[2] * fo_iter.GetMeasurementVector()[2])) ;
    ++fo_iter ;
    ++io_iter ;
    }
    
  ImageWriterType::Pointer writer = ImageWriterType::New() ;
  writer->SetFileName(argv[2]) ;
  writer->SetInput( outputImage ) ;
  writer->Update() ;
  
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



