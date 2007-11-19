/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageReadImageSeriesWrite.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/20 13:27:52 $
  Version:   $Revision: 1.14 $

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

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesWriter.h"
#include "itkNumericSeriesFileNames.h"

int main( int argc, char *argv[] )
{
  if (argc < 4 )
    {
    std::cerr << "Usage: ImageReadImageSeriesWrite inputFile outputPrefix outputExtension" << std::endl;
    return EXIT_FAILURE;
    }


  typedef itk::Image< unsigned short, 3 >     ImageType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );


  typedef itk::Image< unsigned short, 2 >     Image2DType;

  typedef itk::ImageSeriesWriter< ImageType, Image2DType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( reader->GetOutput() );


  typedef itk::NumericSeriesFileNames    NameGeneratorType;
  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

  std::string format = argv[2];
  format += "%03d.";
  format += argv[3];   // filename extension

  nameGenerator->SetSeriesFormat( format.c_str() );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while reading the image" << std::endl;
    std::cerr << excp << std::endl;
    }

 
  ImageType::ConstPointer inputImage = reader->GetOutput();
  ImageType::RegionType   region     = inputImage->GetLargestPossibleRegion();
  ImageType::IndexType    start      = region.GetIndex(); 
  ImageType::SizeType     size       = region.GetSize(); 

  const unsigned int firstSlice = start[2];
  const unsigned int lastSlice  = start[2] + size[2] - 1;

  nameGenerator->SetStartIndex( firstSlice );
  nameGenerator->SetEndIndex( lastSlice );
  nameGenerator->SetIncrementIndex( 1 );

  writer->SetFileNames( nameGenerator->GetFileNames() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while reading the image" << std::endl;
    std::cerr << excp << std::endl;
    }

  return EXIT_SUCCESS;
}


