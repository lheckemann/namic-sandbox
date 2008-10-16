/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: VotingBinaryIterativeHoleFillingImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005-11-20 13:27:52 $
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


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkVotingBinaryHoleFillFloodingImageFilter.h"

int main( int argc, char * argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile outputImageFile radiusX radiusY majority maxNumberOfIterations" << std::endl;
    return EXIT_FAILURE;
    }


  typedef   unsigned char  PixelType;

  typedef itk::Image< PixelType, 2 >   ImageType;


  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  typedef itk::VotingBinaryHoleFillFloodingImageFilter< ImageType, ImageType >  FilterType;

  FilterType::Pointer filter = FilterType::New();

  const unsigned int radiusX = atoi( argv[3] );
  const unsigned int radiusY = atoi( argv[4] );

  ImageType::SizeType indexRadius;
  
  indexRadius[0] = radiusX; // radius along x
  indexRadius[1] = radiusY; // radius along y

  filter->SetRadius( indexRadius );


  filter->SetBackgroundValue(   0 );
  filter->SetForegroundValue( 255 );

  const unsigned int majorityThreshold = atoi( argv[5] );

  filter->SetMajorityThreshold( majorityThreshold  );

  const unsigned int maximumNumberOfIterations = atoi( argv[6] );

  filter->SetMaximumNumberOfIterations( maximumNumberOfIterations  );


  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  std::cout << "Iteration used = " << filter->GetCurrentIterationNumber()     << std::endl;
  std::cout << "Pixels changes = " << filter->GetTotalNumberOfPixelsChanged() << std::endl;

  return EXIT_SUCCESS;
}

