/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DicomSeriesReadImageWrite2.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 20:54:00 $
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


#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageFileWriter.h"

int main( int argc, char* argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " DicomDirectory  outputFileName  [seriesName]" << std::endl;
    return EXIT_FAILURE;
    }


  typedef itk::Image<signed short,3>                     ImageType;
  typedef itk::ImageSeriesReader< ImageType >     ReaderType;


  typedef itk::GDCMImageIO                        ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  // Get the DICOM filenames from the directory

  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetInputDirectory( argv[1] );
  

  try
    {
      
    typedef std::vector<std::string> fileNamesContainer;
    fileNamesContainer fileNames;
    fileNames = nameGenerator->GetInputFileNames();
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileNames( fileNames );

    reader->SetImageIO( dicomIO );

    try
      {
      reader->Update();
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
      }


    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();

    std::cout  << "Writing the image as " << std::endl << std::endl;
    std::cout  << argv[2] << std::endl << std::endl;

    writer->SetFileName( argv[2] );
    writer->SetInput( reader->GetOutput() );

    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
