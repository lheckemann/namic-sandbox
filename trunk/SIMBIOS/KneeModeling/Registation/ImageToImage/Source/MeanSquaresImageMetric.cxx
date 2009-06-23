/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MeanSquaresImageMetric1.cxx,v $
  Language:  C++
  Date:      $Date: 2006-11-12 22:08:29 $
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
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkIdentityTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  fixedImage  movingImage identifier outputFile" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 3;
  typedef   signed short   PixelType;

  typedef itk::Image< PixelType, Dimension >   ImageType;


  typedef itk::ImageFileReader< ImageType >  ReaderType;

  ReaderType::Pointer fixedReader  = ReaderType::New();
  ReaderType::Pointer movingReader = ReaderType::New();

  fixedReader->SetFileName(  argv[ 1 ] );
  movingReader->SetFileName( argv[ 2 ] );

  try 
    {
    fixedReader->Update();
    movingReader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }


  typedef itk::MeanSquaresImageToImageMetric< 
                            ImageType, ImageType >  MetricType;

  MetricType::Pointer metric = MetricType::New();


  typedef itk::IdentityTransform< double, Dimension >  TransformType;

  TransformType::Pointer transform = TransformType::New();


  typedef itk::NearestNeighborInterpolateImageFunction< 
                                 ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();


  ImageType::ConstPointer fixedImage  = fixedReader->GetOutput();
  ImageType::ConstPointer movingImage = movingReader->GetOutput();

  metric->SetTransform( transform );
  metric->SetInterpolator( interpolator );

  metric->SetFixedImage(  fixedImage  );
  metric->SetMovingImage( movingImage );

  metric->SetFixedImageRegion(  fixedImage->GetBufferedRegion()  );

  try 
    {
    metric->Initialize();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  MetricType::TransformParametersType displacement( Dimension );

  displacement.Fill(0.0);

  const double value = metric->GetValue( displacement );


  std::string identifier = argv[3];

  std::cout << "MeanSquaresMetric " << identifier << " = " << value << std::endl;

  std::ofstream outputFile;
  outputFile.open( argv[4] );

  outputFile << identifier << " = " << value << std::endl;

  outputFile.close();

  return EXIT_SUCCESS;
}


