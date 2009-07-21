/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ConsolidationLabelImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2009-07-08 02:34:52 $
  Version:   $Revision: 1.9 $

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
#include "itkConsolidationLabelImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main( int argc, char *argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputLabelImage outputLabelImage";
    std::cerr << " affinityThreshold ";
    std::cerr << " maximumNumberOfIterations ";
    std::cerr << std::endl;
    return 1;
    }

  typedef   unsigned char   LabelPixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< LabelPixelType, Dimension >      LabelImageType;

  typedef itk::ConsolidationLabelImageFilter< 
    LabelImageType, LabelImageType >    ImageFilterType;


  typedef  itk::ImageFileReader< LabelImageType >  ReaderType;

  typedef ImageFilterType::OutputImageType        OutputImageType;
                        
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer  reader  = ReaderType::New();

  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );

  ImageFilterType::Pointer floodFilter = ImageFilterType::New();

  const double affinityThreshold = atof( argv[3] );
  const unsigned int maximumNumberOfIterations = atoi( argv[4] );

  floodFilter->SetMaximumNumberOfIterations( maximumNumberOfIterations );
  floodFilter->SetMajorityThreshold( 1 );
  floodFilter->SetAffinityThreshold( affinityThreshold );
  floodFilter->InPlaceOn();

  LabelImageType::SizeType  ballManhattanRadius;
  ballManhattanRadius.Fill( 2 );
  floodFilter->SetRadius( ballManhattanRadius );

  floodFilter->SetInput( reader->GetOutput() );

  writer->SetInput( floodFilter->GetOutput() );
 
  writer->UseCompressionOn();

  writer->SetFileName( argv[2] );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  std::cout << "Number of iterations used = ";
  std::cout << floodFilter->GetCurrentIterationNumber() << std::endl;

  std::cout << "Number of pixels changed = ";
  std::cout << floodFilter->GetTotalNumberOfPixelsChanged() << std::endl;

  return EXIT_SUCCESS;
}
