/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: GrayscaleFunctionDilateImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2009-07-29 16:03:23 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"

int main(int argc, char *argv[] ) 
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile numIterations  outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef  unsigned char  PixelType;

  typedef itk::Image< PixelType, Dimension>  ImageType;

  typedef itk::ImageFileReader< ImageType >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  int numIter = atoi(argv[2]);
  writer->SetFileName( argv[3] );


  typedef itk::BinaryBallStructuringElement< 
    PixelType, Dimension> KernelType;
  
  typedef itk::BinaryDilateImageFilter<
    ImageType, ImageType, KernelType> FilterType;

  ImageType::Pointer output;
  FilterType::Pointer filter = FilterType::New();

  KernelType ball;
  KernelType::SizeType ballSize;
  ballSize[0] = 1;
  ballSize[1] = 1;
  ballSize[2] = 1;
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
  
  filter->SetKernel( ball );

  filter->SetInput( reader->GetOutput() );
  filter->SetForegroundValue(1);
  filter->Update();
  output = filter->GetOutput();
  output->DisconnectPipeline();

  for(int i=0;i<numIter-1;i++){
    std::cout << "Iteration " << i << std::endl;
    FilterType::Pointer filter = FilterType::New();  
    filter->SetKernel( ball );   
    filter->SetInput( output );
    filter->SetForegroundValue(1);
    filter->Update();
    output = filter->GetOutput();
    output->DisconnectPipeline();
  }

  writer->SetInput( output );

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
