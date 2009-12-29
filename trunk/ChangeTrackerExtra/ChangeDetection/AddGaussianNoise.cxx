/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianImageSourceTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-07-12 10:53:00 $
  Version:   $Revision: 1.11 $

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
#include "itkGaussianImageSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAddImageFilter.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkGaussianDistribution.h"
#include "itkImageRegionIterator.h"

int main(int argc, char* argv[] )
{
  if(argc!=4){
    std::cerr << "Usage: " << argv[0] << " input_image noise_level output_image" << std::endl;
    return -1;
  }

  const unsigned int dim = 3;

  typedef itk::Image< unsigned char, dim > NoiseImageType;
  typedef itk::Image< float, dim > ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typedef itk::ImageFileWriter<NoiseImageType> NoiseWriterType;
  typedef itk::ImageRegionIterator<ImageType> IterType;
  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator GeneratorType;
  typedef itk::Statistics::GaussianDistribution GaussianType;

  double noiseLevel = atof(argv[2]);

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  ImageType::Pointer input = reader->GetOutput();

  GeneratorType::Pointer generator = GeneratorType::New();
  GaussianType::Pointer gaussian = GaussianType::New();
  IterType it(input,input->GetBufferedRegion());
  for(it.GoToBegin();!it.IsAtEnd();++it){
    double r1 = generator->GetVariate();
    double r2 = gaussian->InverseCDF(r1);
    it.Set(it.Get()+r2*noiseLevel);
  }

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(input);
  writer->SetFileName(argv[3]);
  writer->Update();

  return EXIT_SUCCESS;
}


