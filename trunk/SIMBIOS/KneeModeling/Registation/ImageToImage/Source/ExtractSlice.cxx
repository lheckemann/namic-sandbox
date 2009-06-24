/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageReadExtractWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-17 20:36:50 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImage.h"


int main( int argc, char ** argv )
{
  // Verify the number of parameters in the command line
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " input3DImageFile  output2DImageFile " << std::endl;
    std::cerr << " orientation(x=0, y=1, z=2)  sliceNumber " << std::endl;
    return EXIT_FAILURE;
    }


  typedef signed short  InputPixelType;
  typedef unsigned char OutputPixelType;

  typedef itk::Image< InputPixelType,  3 >    InputImageType;
  typedef itk::Image< InputPixelType,  2 >    SliceImageType;
  typedef itk::Image< OutputPixelType, 2 >    OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  const char * inputFilename  = argv[1];
  const char * outputFilename = argv[2];

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( inputFilename  );
  writer->SetFileName( outputFilename );

  typedef itk::ExtractImageFilter< InputImageType, SliceImageType > ExtractFilterType;
  ExtractFilterType::Pointer extractor = ExtractFilterType::New();

  typedef itk::RescaleIntensityImageFilter< SliceImageType, OutputImageType > RescaleFilterType;
  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  reader->Update();
  InputImageType::RegionType inputRegion = reader->GetOutput()->GetLargestPossibleRegion();

  unsigned int orientation = atoi( argv[3] );

  InputImageType::SizeType size = inputRegion.GetSize();
  size[orientation] = 0;

  InputImageType::IndexType start = inputRegion.GetIndex();
  const unsigned int sliceNumber = atoi( argv[4] );
  start[orientation] = sliceNumber;

  InputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );

  extractor->SetExtractionRegion( desiredRegion );

  extractor->SetInput( reader->GetOutput() );
  rescaler->SetInput( extractor->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );

  try 
    { 
    writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 

  return EXIT_SUCCESS;
}
