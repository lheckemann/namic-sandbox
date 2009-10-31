/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: PrepareImageForRegistration.cxx,v $
  Language:  C++
  Date:      $Date: 2009-06-09 23:45:57 $
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

#include "itkVotingBinaryHoleFillFloodingImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main( int argc, char * argv[] )
{

  if( argc < 6 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile outputImageFile radius majority maxiterations" << std::endl;  
    return EXIT_FAILURE;
    }

  typedef unsigned char PixelType;
  const unsigned int    Dimension = 3;

  typedef itk::Image< PixelType,  Dimension >   ImageType;

  typedef itk::ImageFileReader< ImageType >     ReaderType;
  typedef itk::ImageFileWriter< ImageType >     WriterType;

  typedef itk::VotingBinaryHoleFillFloodingImageFilter< ImageType, ImageType > FilterType;

  FilterType::Pointer holeFiller = FilterType::New();

  holeFiller->SetInput( reader->GetOutput() );
           
  ImageType::SizeType indexRadius;
  
  const unsigned int radius = atoi( argv[3] );

  indexRadius.Fill( radius ); // Manhattan radius Reconsider "2"

  holeFiller->SetRadius( indexRadius );
  holeFiller->SetBackgroundValue(   0 );
  holeFiller->SetForegroundValue( 255 );

  const unsigned int majority = atoi( argv[4] );

  holeFiller->SetMajorityThreshold( majority );

  const unsigned int numberOfIterations = atoi( argv[5] );

  holeFiller->SetMaximumNumberOfIterations( numberOfIterations );


  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );

  WriterType::Pointer writer = WriterType::New();

  writer->SetInput( holeFiller->GetOutput() );

  writer->SetFileName( argv[2] );
  writer->UseCompressionOn();

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
