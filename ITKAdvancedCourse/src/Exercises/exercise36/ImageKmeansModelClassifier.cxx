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
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0];
    std::cerr << " inputImage outputLabeledImage contiguousLabels";
    std::cerr << " mean1 mean2... mean3 " << std::endl;
    return EXIT_FAILURE;
    }

  const char * inputImageFileName = argv[1];



  
  typedef unsigned char                     PixelValueType;
  const unsigned int                        NumberOfComponents = 3;

  typedef itk::Vector< PixelValueType, NumberOfComponents >  PixelType;

  const unsigned int                        Dimension = 2;

  typedef itk::Image<PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );





  typedef itk::ImageKmeansImageFilter< ImageType > KMeansFilterType;

  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();

  kmeansFilter->SetInput( reader->GetOutput() );

  const unsigned int numberOfInitialClasses = atoi( argv[4] );





  const unsigned int useNonContiguousLabels = atoi( argv[3] );

  kmeansFilter->SetUseNonContiguousLabels( useNonContiguousLabels );


  const unsigned int argoffset = 5;

  if( static_cast<unsigned int>(argc) <
      numberOfInitialClasses + argoffset )
    {
    std::cerr << "Error: " << std::endl;
    std::cerr << numberOfInitialClasses << " classes has been specified ";
    std::cerr << "but no enough means have been provided in the command ";
    std::cerr << "line arguments " << std::endl;
    return EXIT_FAILURE;
    }




  for( unsigned k=0; k < numberOfInitialClasses; k++ )
    {
    const double userProvidedInitialMean = atof( argv[k+argoffset] );
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }


  const char * outputImageFileName = argv[2];



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

  const unsigned int numberOfClasses = estimatedMeans.Size();

  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
    }



  return EXIT_SUCCESS;
  
}


