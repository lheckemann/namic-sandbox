/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ScalarImageKmeansClassifier.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 19:34:53 $
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
#include "itkImageKmeansImageFilter.h"

int main( int argc, char * argv [] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0];
    std::cerr << " inputImage outputLabeledImage numberOfClasses useNonContiguousLabels";
    return EXIT_FAILURE;
    }

  const char * inputImageFileName  = argv[1];
  const char * outputImageFileName = argv[2];

  const unsigned int numberOfClasses = atoi( argv[3] );

  const unsigned int useNonContiguousLabels = atoi( argv[4] );

  typedef unsigned char                     PixelValueType;
  const unsigned int                        NumberOfComponents = 3;

  typedef itk::Vector< PixelValueType, NumberOfComponents >  PixelType;

  const unsigned int                        Dimension = 2;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );


  typedef itk::ImageKmeansImageFilter< ImageType > KMeansFilterType;

  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();

  kmeansFilter->SetInput( reader->GetOutput() );

  kmeansFilter->SetUseNonContiguousLabels( useNonContiguousLabels );


  KMeansFilterType::RealPixelType initialMean;
  initialMean.Fill(0.0);

  for( unsigned int cc = 0; cc < numberOfClasses; ++cc )
    {
    kmeansFilter->AddClassWithInitialMean( initialMean );
    }


  typedef KMeansFilterType::OutputImageType  OutputImageType;

  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();
  
  writer->SetInput( kmeansFilter->GetOutput() );

  writer->SetFileName( outputImageFileName );


  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encountered while writing ";
    std::cerr << " image file : " << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  
  KMeansFilterType::ParametersType estimatedMeans = 
                                            kmeansFilter->GetFinalMeans();


  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << "  estimated mean : ";
    for ( unsigned int j = 0 ; j < NumberOfComponents ; ++j )
      {
      std::cout << "    " << estimatedMeans[i*NumberOfComponents +j];
      }
    std::cout << std::endl;
    }

  return EXIT_SUCCESS;
  
}


