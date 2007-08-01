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
#include <ImageToObjectmap.h>
int main( int argc, char ** argv )
{
  int error_count = 0;
  if ( argc != 4 )
    {
    std::cerr << "USAGE: " << argv[0] << "<inputFileName> <outputFileName> <Nifti file>" << std::endl;
    }
  const char *InputObjectFileName = argv[1];
  const char *OuptputObjectFileName = argv[2];
  const char *NiftiFile = argv[3];
  typedef unsigned char       InputPixelType;
  typedef unsigned char       OutputPixelType;
  const   unsigned int        Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >    InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >    OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  typedef itk::ImageToObjectMap<InputImageType> ImageToObjectMapType;

  ImageToObjectMapType::Pointer ImageToObjectConvertor = ImageToObjectMapType::New();

  ReaderType::Pointer reader = ReaderType::New();
  ReaderType::Pointer readerTwo = ReaderType::New();
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

  itk::AnalyzeObjectMap::Pointer ObjectMap = ImageToObjectConvertor->TransformImage(reader->GetOutput());

  writer->SetFileName(OuptputObjectFileName);
  //writer->SetFileName("objectLabelTest2.obj");
  writer->SetInput(reader->GetOutput());
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

  std::ifstream ReferenceFile;
  ReferenceFile.open(InputObjectFileName, std::ios::binary | std::ios::in);

  std::ifstream WrittenFile;
  WrittenFile.open(OuptputObjectFileName, std::ios::binary | std::ios::in);

  char ReferenceBytes;
  char WrittenBytes;
  int count = 0;
  std::ofstream myfile;
  myfile.open("testing1.txt");
  while(!ReferenceFile.eof()  && !WrittenFile.eof())
  {
    count++;
    ReferenceFile.read(reinterpret_cast<char *> (&ReferenceBytes), sizeof(char));
    WrittenFile.read(reinterpret_cast<char *>(&WrittenBytes), sizeof(char));
    if(ReferenceBytes != WrittenBytes)
    {
        error_count++;
        myfile<<count<< ":" << (int)ReferenceBytes << ":" << (int)WrittenBytes << std::endl;
    }
  }

  if(!ReferenceFile.eof())
  {
      error_count++;
      std::cout<<"ReferenceFile is not at end of file"<<std::endl;
  }
  if(!WrittenFile.eof())
  {
      error_count++;
      std::cout<<"WrittenFile is not at end of file"<<std::endl;
  }

  ReferenceFile.close();
  WrittenFile.close();

  readerTwo->SetFileName(NiftiFile);
  try
    {
    readerTwo->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }
  itk::AnalyzeObjectMap::Pointer CreateObjectMap = itk::AnalyzeObjectMap::New();

  CreateObjectMap->AddObjectBasedOnImagePixel(reader->GetOutput(), 200, "Square");
  CreateObjectMap->AddObjectBasedOnImagePixel(reader->GetOutput(), 128, "Circle");
  CreateObjectMap->PlaceObjectMapEntriesIntoMetaData();

  writer->SetInput(CreateObjectMap);
  writer->SetFileName("CreatingObjectMap.obj");

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

  if( error_count )
  {
  return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
