/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkReadITKImage3DSegmentShowVTK.cxx,v $
  Language:  C++
  Date:      $Date: 2006/07/23 17:39:54 $
  Version:   $Revision: 1.9 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkVectorImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"

int main(int argc, char*argv[])
{

  if (argc < 3)
    {
    std::cerr << "Usage" << std::endl;
    std::cerr << 
      "  ConvertImagesToVectorImage image-1.mhd image-2.mhd image-3.mhd ..... " 
     <<  "image-N.mhd outputVectorImage.mhd " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Make sure the images are of pixel type unsigned char and same dimensions " << std::endl;

  typedef unsigned char PixelType;
  const unsigned int    Dimension = 2;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageToVectorImageFilter< ImageType > FilterType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< FilterType::OutputImageType > WriterType;

  FilterType::Pointer filter = FilterType::New();

  for (unsigned int k = 1; k<argc-1; k++)
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[k] );
    reader->Update();
    filter->SetNthInput( k-1, reader->GetOutput() );
    }

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[argc-1] );
  writer->UseCompressionOff();

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

