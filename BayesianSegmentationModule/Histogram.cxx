/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageHistogram3.cxx,v $
  Language:  C++
  Date:      $Date: 2005/05/07 20:12:37 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImageToHistogramGenerator.h"
#include "itkImage.h"
#include "itkJoinImageFilter.h"
#include "itkImageFileReader.h"



int main( int argc, char * argv [] )
{

  if( argc < 2 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage :  Histogram  inputImageFileName intputLabelImageFileName" << std::endl;
    return -1;
    }




  typedef unsigned short                        PixelType;

  const unsigned int                            Dimension = 2;

  typedef itk::Image< PixelType, Dimension >    InputImageType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();

  reader1->SetFileName( argv[1] );
  reader2->SetFileName( argv[2] );

  try
    {
    reader1->Update();
    reader2->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while reading image file : " << argv[1] << std::endl;
    std::cerr << excp << std::endl;
    return -1;
    }


  typedef itk::JoinImageFilter< InputImageType, InputImageType >  JoinFilterType;

  typedef JoinFilterType::OutputImageType                         JoinImageType;


  typedef itk::Statistics::ImageToHistogramGenerator< 
                            JoinImageType >   HistogramGeneratorType;

  HistogramGeneratorType::Pointer histogramGenerator = 
                                             HistogramGeneratorType::New();


  typedef HistogramGeneratorType::SizeType   SizeType;

  SizeType size;

  size[0] = 255;        // number of bins for the gray levels
  size[1] =   4;        // number of bins for the labels = number of labels

  histogramGenerator->SetNumberOfBins( size );
  histogramGenerator->SetMarginalScale( 10.0 );
  histogramGenerator->SetInput(  reader->GetOutput()  );
  histogramGenerator->Compute();


  typedef HistogramGeneratorType::HistogramType  HistogramType;
  const HistogramType * histogram = histogramGenerator->GetOutput();
  const unsigned int histogramSize = histogram->Size();

  std::cout << "Histogram size " << histogramSize << std::endl;

  channel = 1;  // a particular label

  std::cout << "Histogram of the green component" << std::endl;

  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    std::cout << "bin = " << bin << " frequency = ";
    std::cout << histogram->GetFrequency( bin, channel ) << std::endl;
    }

  return 0;
  
}


