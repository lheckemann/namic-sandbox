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
#include "itkImageToAnalyzeObjectmap.h"
int main( int argc, char ** argv )
{
  int error_count = 0;
  if ( argc != 6 )
    {
    std::cerr << "USAGE: " << argv[0] << "<inputFileName> <outputFileName> <Nifti file> <NewObjectMapFileName> <oneObjectEntryFileName>" << std::endl;
    }
  const char *InputObjectFileName = argv[1];
  const char *OuptputObjectFileName = argv[2];
  const char *NiftiFile = argv[3];
  const char *CreatingObject = argv[4];
  const char *oneObjectEntryFileName = argv[5];
  typedef unsigned char       InputPixelType;
  typedef unsigned char       OutputPixelType;
  const   unsigned int        Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >    InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >    OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  typedef itk::ImageToAnalyzeObjectMap<InputImageType> ImageToObjectMapType;

  ImageToObjectMapType::Pointer ImageToObjectConvertor = ImageToObjectMapType::New();

  ReaderType::Pointer reader = ReaderType::New();
  ReaderType::Pointer readerTwo = ReaderType::New();
  ReaderType::Pointer readerThree = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( InputObjectFileName);
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

  //Now that we have an itk image now we need to make the image an object map
  itk::AnalyzeObjectMap::Pointer ObjectMap = ImageToObjectConvertor->TransformImage(reader->GetOutput());

  //Now we can change the object map into an itk RGB image, we then can send this image to the itk-vtk
  //converter and show the image if we wanted to.
  itk::Image<itk::RGBPixel<unsigned char>, 3>::Pointer RGBImage = ObjectMap->ObjectMapToRGBImage();

  writer->SetFileName(OuptputObjectFileName);
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

  //Check the original file against the file that was written out to see if they are the exact same files.
  std::ifstream ReferenceFile;
  ReferenceFile.open(InputObjectFileName, std::ios::binary | std::ios::in);

  std::ifstream WrittenFile;
  WrittenFile.open(OuptputObjectFileName, std::ios::binary | std::ios::in);

  char ReferenceBytes;
  char WrittenBytes;
  int count = 0;
  while(!ReferenceFile.eof()  && !WrittenFile.eof())
  {
    count++;
    ReferenceFile.read(reinterpret_cast<char *> (&ReferenceBytes), sizeof(char));
    WrittenFile.read(reinterpret_cast<char *>(&WrittenBytes), sizeof(char));
    if(ReferenceBytes != WrittenBytes)
    {
        error_count++;
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

  //End of checking the original versus what was written

  //Now we bring in a nifti file that Hans and Jeffrey created, the image is two squares and a circle of different intensity values.
  //See the paper in the Insight Journal named "AnalyzeObjectLabelMap" for a picutre of the nifti file.
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

  CreateObjectMap->AddObjectEntry("You Can Delete Me");
  CreateObjectMap->AddObjectEntryBasedOnImagePixel(readerTwo->GetOutput(), 200, "Square", 250, 0, 0);
  CreateObjectMap->AddObjectEntryBasedOnImagePixel(readerTwo->GetOutput(), 128, "Circle", 0, 250,0);
  CreateObjectMap->AddObjectEntry("Nothing In Here");
  CreateObjectMap->DeleteObjectEntry("Nothing In Here");
  CreateObjectMap->PlaceObjectMapEntriesIntoMetaData();

  writer->SetInput(CreateObjectMap);
  writer->SetFileName(CreatingObject);

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

  readerThree->SetFileName(CreatingObject);
  try
    {
    readerThree->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }
  itk::AnalyzeObjectMap::Pointer ObjectMapTwo = ImageToObjectConvertor->TransformImage(readerThree->GetOutput());

  itk::Image<itk::RGBPixel<unsigned char>, 3>::Pointer RGBImageTwo = ObjectMapTwo->ObjectMapToRGBImage();

  itk::AnalyzeObjectMap::Pointer circleObjectMap = ObjectMapTwo->PickOneEntry(3);

  circleObjectMap->PlaceObjectMapEntriesIntoMetaData();

  writer->SetInput(circleObjectMap);
  writer->SetFileName(oneObjectEntryFileName);

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

  readerThree->SetFileName(oneObjectEntryFileName);
  try
    {
    readerThree->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  itk::AnalyzeObjectMap::Pointer ObjectMapThree = ImageToObjectConvertor->TransformImage(readerThree->GetOutput());

  itk::Image<itk::RGBPixel<unsigned char>, 3>::Pointer RGBImageThree = ObjectMapThree->ObjectMapToRGBImage();


  if( error_count )
  {
  return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
