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

#include "itkBinaryBallStructuringElement.h" 
#include "itkBinaryDilateImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkMaskImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main( int argc, char * argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile outputImageFile " << std::endl;  
    return EXIT_FAILURE;
    }

  typedef signed short    InputPixelType;
  typedef unsigned char   MaskPixelType;
  typedef signed short    OutputPixelType;

  const unsigned int Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< MaskPixelType,   Dimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >   ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  typedef itk::ImageFileReader< MaskImageType >    MaskReaderType;

  typedef itk::BinaryBallStructuringElement< 
                      MaskPixelType,
                      Dimension  >             StructuringElementType;

  typedef itk::BinaryDilateImageFilter<
                            MaskImageType, 
                            MaskImageType, 
                            StructuringElementType >  DilateFilterType;

  typedef itk::MaskImageFilter<
    InputImageType, MaskImageType, OutputImageType  >   MaskFilterType;
            
  ReaderType::Pointer reader = ReaderType::New();
  MaskReaderType::Pointer maskReader = MaskReaderType::New();

  reader->SetFileName( argv[1] );
  maskReader->SetFileName( argv[2] );

  StructuringElementType  structuringElement;

  structuringElement.SetRadius( 2 );  // 5 x 5 x 5 structuring element
  structuringElement.CreateStructuringElement();

  DilateFilterType::Pointer dilater = DilateFilterType::New();

  dilater->SetInput( maskReader->GetOutput() );

  dilater->SetDilateValue( 255 );
  dilater->SetKernel( structuringElement );

  try
    {
    dilater->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Run a second dilation
  //
  MaskImageType::Pointer mask = dilater->GetOutput();
  mask->DisconnectPipeline();

  dilater->SetInput( mask );
  dilater->Update();

  MaskFilterType::Pointer masker  = MaskFilterType::New();

  masker->SetInput1( reader->GetOutput() );
  masker->SetInput2( dilater->GetOutput() );

  const OutputPixelType outsideValue = -5;  // Purposely outside of the range of MRI

  masker->SetOutsideValue( outsideValue );

  WriterType::Pointer writer = WriterType::New();

  writer->SetInput( masker->GetOutput() );

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
