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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkRegionOfInterestImageFilter.h"
#include "itkImage.h"

#include "itkAnalyzeObjectLabelMapImageIO.h"
#include "ImageToObjectmap.h"
int main( int argc, char ** argv )
{
  if ( argc != 3 )
    {
    std::cerr << "USAGE: " << argv[0] << "<inputFileName> <outputFileName>" << std::endl;
    }
  const std::string InputObjectFileName(argv[1]);
  const std::string OuptputObjectFileName(argv[2]);
  typedef unsigned char       InputPixelType;
  typedef unsigned char       OutputPixelType;
  const   unsigned int        Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >    InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >    OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  typedef itk::ImageToObjectMap<InputImageType> ConvertObjectType;

  ConvertObjectType::Pointer ImageConvertor = ConvertObjectType::New();;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( InputObjectFileName);
  //reader->SetFileName("C:/Documents and Settings/woofton/Desktop/imageIOStreaming/Testing/Data/Input/test.obj");
   try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  InputImageType::Pointer IsOurDataCorrect = reader->GetOutput();
  //ImageConvertor->TransformImage(reader->GetOutput());
  //itk::AnalyzeObjectMap::Pointer = ItkImageToAnaylzeObjectMap(reader->GetOutput());

  //writer->SetFileName(OuptputObjectFileName);
  //writer->SetFileName("objectLabelTest2.obj");
  //writer->SetInput(reader->GetOutput());
  //try
  //  {
  //  writer->Update();
  //  }
  //catch( itk::ExceptionObject & err )
  //  {
  //  std::cerr << "ExceptionObject caught !" << std::endl;
  //  std::cerr << err << std::endl;
  //  return EXIT_FAILURE;
  //  }

  //reader->SetFileName("objectLabelTest2.obj");
  //reader->Update();
  //return EXIT_SUCCESS;
}
