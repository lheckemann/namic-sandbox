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
#include "itkAnalyzeObjectMap.h"
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
  typedef itk::Image< InputPixelType, 4 > FourDimensionImageType;
  typedef itk::Image<itk::RGBPixel<InputPixelType>, Dimension> RGBImageType;
  typedef itk::Image< double, 3 > DoubleImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  typedef itk::ImageFileWriter< FourDimensionImageType > FourDimensionWriterType;
  typedef itk::ImageFileReader< FourDimensionImageType > FourDimensionReaderType;
  typedef itk::ImageFileWriter< DoubleImageType > DoubleWriterType;

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
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }

  //Now that we have an itk image now we need to make the image an object map
  itk::AnalyzeObjectMap<InputImageType, RGBImageType>::Pointer ObjectMap = itk::AnalyzeObjectMap<InputImageType, RGBImageType>::New();

  ObjectMap->TransformImage(reader->GetOutput());

  //Now we can change the object map into an itk RGB image, we then can send this image to the itk-vtk
  //converter and show the image if we wanted to.
  RGBImageType::Pointer RGBImage = ObjectMap->ObjectMapToRGBImage();

  writer->SetFileName(OuptputObjectFileName);
  writer->SetInput(reader->GetOutput());
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
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
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }
  itk::AnalyzeObjectMap<InputImageType>::Pointer CreateObjectMap = itk::AnalyzeObjectMap<InputImageType>::New();

  CreateObjectMap->AddObjectEntry("You Can Delete Me");
  CreateObjectMap->AddObjectEntryBasedOnImagePixel(readerTwo->GetOutput(), 200, "Square", 250, 0, 0);
  CreateObjectMap->AddObjectEntryBasedOnImagePixel(readerTwo->GetOutput(), 128, "Circle", 0, 250,0);
  CreateObjectMap->AddObjectEntry("Nothing In Here");
  CreateObjectMap->GetObjectEntry(4)->Copy(CreateObjectMap->GetObjectEntry(1));
  CreateObjectMap->DeleteObjectEntry("Nothing In Here");

  writer->SetInput(CreateObjectMap);
  writer->SetFileName(CreatingObject);

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }

  readerThree->SetFileName(CreatingObject);
  try
    {
    readerThree->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }
  itk::AnalyzeObjectMap<InputImageType, RGBImageType>::Pointer ObjectMapTwo = itk::AnalyzeObjectMap<InputImageType, RGBImageType>::New();

  ObjectMapTwo->TransformImage(readerThree->GetOutput());
  RGBImageType::Pointer RGBImageTwo = ObjectMapTwo->ObjectMapToRGBImage();

  writer->SetInput(ObjectMapTwo->PickOneEntry(3));
  writer->SetFileName(oneObjectEntryFileName);

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }

  readerThree->SetFileName(oneObjectEntryFileName);
  try
    {
    readerThree->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }

  itk::AnalyzeObjectMap<InputImageType, RGBImageType>::Pointer ObjectMapThree = itk::AnalyzeObjectMap<InputImageType, RGBImageType>::New();//ImageToObjectConvertor->TransformImage(readerThree->GetOutput());

  ObjectMapThree->TransformImage(readerThree->GetOutput());
  RGBImageType::Pointer RGBImageThree = ObjectMapThree->ObjectMapToRGBImage();


  FourDimensionImageType::Pointer BlankImage = FourDimensionImageType::New();
  const FourDimensionImageType::SizeType size = {{20,20,20,2}};
  const FourDimensionImageType::IndexType orgin = {{0,0,0,0}};

  FourDimensionImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(orgin);
  BlankImage->SetRegions(region);
  BlankImage->Allocate();
  BlankImage->FillBuffer(50);
  FourDimensionWriterType::Pointer FourDimensionWriter = FourDimensionWriterType::New();
  FourDimensionWriter->SetInput(BlankImage);
  FourDimensionWriter->SetFileName("blankImage.obj");

  try
    {
    FourDimensionWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }

  FourDimensionReaderType::Pointer FourDimensionReader = FourDimensionReaderType::New();
  FourDimensionReader->SetFileName("blankImage.obj");
  try
    {
    FourDimensionReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl
    << err << std::endl;
    return EXIT_FAILURE;
    }


  if( error_count )
  {
  return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
