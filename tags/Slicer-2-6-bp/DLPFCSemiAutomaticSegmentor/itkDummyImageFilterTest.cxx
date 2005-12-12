/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDummyImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/09 20:58:34 $
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

#include "itkDummyImageFilter.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"

//int itkDummyImageFilterTest(int argc, char* argv[] )
int main(int argc, char* argv[] )
{

  if( argc < 2 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  // INPUT PARAMETERS
  char * inputFileName = argv[1];
  char * outputFileName = argv[2];
  

  // SETUP READER
  const unsigned int Dimension = 2;
  typedef unsigned short InputPixelType;
  typedef itk::Image< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );


  // SETUP FILTER
  typedef unsigned short OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::DummyImageFilter< 
                                 InputImageType,
                                 OutputImageType > ClassifierFilterType;

  ClassifierFilterType::Pointer filter = ClassifierFilterType::New();
  filter->SetInput( reader->GetOutput() );


  // SETUP WRITER
  typedef itk::ImageFileWriter< OutputImageType >    WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );


  // WRITE LABELMAP TO FILE
  // need to write GetOutput method
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( outputFileName );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while writing image file : "
      << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // TESTING PRINT
  filter->Print( std::cout );
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
