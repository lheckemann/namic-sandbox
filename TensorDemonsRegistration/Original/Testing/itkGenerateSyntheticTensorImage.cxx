/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/22 00:30:16 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"
#include "itkDiffusionTensor3D.h"
#include "itkImageFileWriter.h"


int main( int argc, char *argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " outputImageFile";
    return EXIT_FAILURE;
    }
  
  const unsigned int Dimension = 3;

  typedef itk::DiffusionTensor3D< double >  PixelType;

  typedef itk::Image< PixelType, Dimension >   ImageType;

  typedef itk::ImageFileWriter< ImageType >    WriterType;

  WriterType::Pointer writer = WriterType::New();

  ImageType::Pointer image = ImageType::New();

  ImageType::RegionType region;
  ImageType::SizeType size;
  ImageType::IndexType start;
  
  start.Fill( 0 );
  size.Fill( 10 );

  region.SetIndex( start );
  region.SetSize( size );

  image->SetRegions( region );
  image->Allocate();

  PixelType pixelValue;

  pixelValue[0] = 2.0;
  pixelValue[1] = 0.0;
  pixelValue[2] = 0.0;
  pixelValue[3] = 1.0;
  pixelValue[4] = 0.0;
  pixelValue[5] = 1.0;

  image->FillBuffer( pixelValue );

  writer->SetInput( image );
  writer->SetFileName( argv[1] );
  
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

