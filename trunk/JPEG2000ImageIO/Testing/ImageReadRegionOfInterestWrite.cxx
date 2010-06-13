/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageReadRegionOfInterestWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/27 01:46:11 $
  Version:   $Revision: 1.12 $

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

#include "itkJPEG2000ImageIOFactory.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkRegionOfInterestImageFilter.h"
#include "itkImage.h"

int main( int argc, char ** argv )
{
  // Verify the number of parameters in the command line
  if( argc < 7 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile  outputImageFile " << std::endl;
    std::cerr << " startX startY sizeX sizeY " << std::endl;
    return EXIT_FAILURE;
    }

  //  Register the factory
  itk::JPEG2000ImageIOFactory::RegisterOneFactory();

  //  Image types are defined below.
  typedef unsigned char       InputPixelType;
  typedef unsigned char       OutputPixelType;
  const   unsigned int        Dimension = 2;

  typedef itk::Image< InputPixelType,  Dimension >    InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >    OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  //  The RegionOfInterestImageFilter type is instantiated using
  //  the input and output image types. A filter object is created with the
  //  New() method and assigned to a \doxygen{SmartPointer}.
  typedef itk::RegionOfInterestImageFilter< InputImageType,
                                            OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();

  //  The RegionOfInterestImageFilter requires a region to be
  //  defined by the user. The region is specified by an \doxygen{Index}
  //  indicating the pixel where the region starts and an \doxygen{Size}
  //  indicating how many pixels the region has along each dimension. In this
  //  example, the specification of the region is taken from the command line
  //  arguments (this example assumes that a 2D image is being processed).
  OutputImageType::IndexType start;
  start[0] = atoi( argv[3] );
  start[1] = atoi( argv[4] );

  OutputImageType::SizeType size;
  size[0] = atoi( argv[5] );
  size[1] = atoi( argv[6] );

  OutputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  //  Then the region is passed to the filter using the
  //  SetRegionOfInterest() method.

  filter->SetRegionOfInterest( desiredRegion );

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  const std::string inputFilename  = argv[1];
  const std::string outputFilename = argv[2];

  reader->SetFileName( inputFilename  );
  writer->SetFileName( outputFilename );

  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );

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



