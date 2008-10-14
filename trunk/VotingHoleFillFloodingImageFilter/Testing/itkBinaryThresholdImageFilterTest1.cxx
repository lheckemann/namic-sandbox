// Please do NOT edit this file.
// It has been automatically generated
// by a perl script from the original cxx sources
// in the Insight/Examples directory

// Any changes should be made in the file
// BinaryThresholdImageFilter.cxx


/*=========================================================================
  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: BinaryThresholdImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006-05-21 13:05:15 $
  Version:   $Revision: 1.35 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif


// Software Guide : BeginCommandLineArgs
// INPUTS:  {BrainProtonDensitySlice.png}
// OUTPUTS: {BinaryThresholdImageFilterOutput.png}
// 150 180 0 255
// Software Guide : EndCommandLineArgs


#include "itkBinaryThresholdImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main( int argc, char * argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile outputImageFile ";  
    std::cerr << " lowerThreshold upperThreshold ";  
    std::cerr << " outsideValue insideValue   "  << std::endl;  
    return EXIT_FAILURE;
    }
  

  typedef  unsigned char  InputPixelType;
  typedef  unsigned char  OutputPixelType;



  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;



  typedef itk::BinaryThresholdImageFilter<
               InputImageType, OutputImageType >  FilterType;



  typedef itk::ImageFileReader< InputImageType >  ReaderType;




  typedef itk::ImageFileWriter< OutputImageType >  WriterType;



  ReaderType::Pointer reader = ReaderType::New();
  FilterType::Pointer filter = FilterType::New();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  reader->SetFileName( argv[1] );



  filter->SetInput( reader->GetOutput() );



  const OutputPixelType outsideValue = atoi( argv[5] );
  const OutputPixelType insideValue  = atoi( argv[6] );

  filter->SetOutsideValue( outsideValue );
  filter->SetInsideValue(  insideValue  );



  const InputPixelType lowerThreshold = atoi( argv[3] );
  const InputPixelType upperThreshold = atoi( argv[4] );

  filter->SetLowerThreshold( lowerThreshold );
  filter->SetUpperThreshold( upperThreshold );



  filter->Update();


  writer->SetFileName( argv[2] );
  writer->Update();

  return EXIT_SUCCESS;
}

