/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/09 20:58:34 $
  Version:   $Revision: 1.3 $

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
#include "itkBayesianClassifierImageFilter.h"
#include "itkImageFileWriter.h"

//int itkBayesianClassifierImageFilterTest(int argc, char* argv[] )
int main(int argc, char* argv[] )
{

  if( argc < 3 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  // input parameters
  const char * membershipImageFileName  = argv[1];
  const char * labelMapImageFileName    = argv[2];

  // setup reader
  const unsigned int Dimension = 2;
  typedef float InputPixelType;
  typedef itk::VectorImage< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< 
                     InputImageType, 
                     itk::DefaultConvertPixelTraits<InputPixelType> >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( membershipImageFileName );

  typedef unsigned char  LabelType;
  typedef float          PriorType;
  typedef float          PosteriorType;


  typedef itk::BayesianClassifierImageFilter< 
                              InputImageType,LabelType,
                              PosteriorType,PriorType >   ClassifierFilterType;

  ClassifierFilterType::Pointer filter = ClassifierFilterType::New();


  filter->SetInput( reader->GetOutput() );



  // SET FILTER'S PRIOR PARAMETERS
  // do nothing here to default to uniform priors
  // otherwise set the priors to some user provided values



  // SET FILTER'S SMOOTHING PARAMETERS
  // do nothing here to use default smoothing parameters
  // otherwise set the smoothing parameters


  //
  // Setup writer
  //
  typedef ClassifierFilterType::OutputImageType      OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType >    WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( labelMapImageFileName );


  //
  // Write labelmap to file
  // 
  writer->SetInput( filter->GetOutput() );


  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception catched : " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  

  // Testing print
  filter->Print( std::cout );
  std::cout << "Test passed." << std::endl;

  return EXIT_SUCCESS;

}
