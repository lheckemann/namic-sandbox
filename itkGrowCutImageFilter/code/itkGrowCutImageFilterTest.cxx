/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: NeighborhoodIterators3.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:59:00 $
  Version:   $Revision: 1.16 $

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

#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


#include "itkLogImageFilter.h"
#include "itkExpImageFilter.h"

#include "itkBilateralImageFilter.h"
#include "itkGrowCutImageFilter.h"

int main( int argc, char ** argv )
{
  if ( argc < 4 )
    {
      std::cerr << "Missing parameters. " << std::endl;
      std::cerr << "Usage: " << std::endl;
      std::cerr << argv[0]
                << " inputImageFile initialLabelMapFile outputLabelMapFile"
                << std::endl;
      return -1;
    }

  //  typedef float PixelType;
  typedef unsigned char charPixelType;
  typedef unsigned char LabelType;
  typedef float floatPixelType;
  const unsigned int dim = 2;
  
  typedef itk::Image< charPixelType, dim >  charImageType;
  typedef itk::Image< floatPixelType, dim >  floatImageType;
  typedef itk::Image< LabelType, dim >  LabelMapType;

 
  //////////////////////////////////////////////////
  // read in image
  typedef itk::ImageFileReader< charImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
      reader->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return -1;
    }


  //////////////////////////////////////////////////
  // read in initial label map image
  typedef itk::ImageFileReader< LabelMapType > LabelMapReaderType;
  LabelMapReaderType::Pointer labelReader = LabelMapReaderType::New();
  labelReader->SetFileName( argv[2] );
  try
    {
      labelReader->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return -1;
    }


  ////////////////////////////////////////////////////////////////////////////////
  // cast uchar image to float image
  typedef itk::CastImageFilter< charImageType, floatImageType > charToFloatCastImageFilterType;
  charToFloatCastImageFilterType::Pointer castFilter = charToFloatCastImageFilterType::New();
  castFilter->SetInput( reader->GetOutput() );

  ////////////////////////////////////////////////////////////////////////////////
  // deal with the 0's in the input image by rescaling image to 1 to 255
  //  typedef itk::RescaleIntensityImageFilter< charImageType, charImageType > rescaleFilterType;
  typedef itk::RescaleIntensityImageFilter< floatImageType, floatImageType > rescaleFilterType;
  rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
  rescaleFilter->SetInput( castFilter->GetOutput() );
  rescaleFilter->SetOutputMinimum( 1 );
  rescaleFilter->SetOutputMaximum( 255 );

  //  printf("rescaled to 1 to 255\n");


  ////////////////////////////////////////////////////////////////////////////////
  // log the image
  //  typedef itk::LogImageFilter<charImageType, charImageType> logFilterType;
  typedef itk::LogImageFilter<floatImageType, floatImageType> logFilterType;
  logFilterType::Pointer logFilter = logFilterType::New();
  logFilter->SetInput( rescaleFilter->GetOutput() );

  // printf("log is done.\n");

  ////////////////////////////////////////////////////////////////////////////////
  // rescale
  rescaleFilterType::Pointer rescaleFilter1 = rescaleFilterType::New();
  rescaleFilter1->SetInput( logFilter->GetOutput() );
  rescaleFilter1->SetOutputMinimum( 1 );
  rescaleFilter1->SetOutputMaximum( 255 );

  // printf("rescaled to 1 to 255\n");

  ////////////////////////////////////////////////////////////////////////////////
  // bilateral filtering of log image
  typedef itk::BilateralImageFilter< floatImageType, floatImageType >  bilateralFilterType;
  //               charImageType, charImageType >  bilateralFilterType;
  bilateralFilterType::Pointer bilateralFilter = bilateralFilterType::New();
  bilateralFilter->SetInput( rescaleFilter1->GetOutput() );

  const unsigned int Dimension = floatImageType::ImageDimension;
  double domainSigmas[ Dimension ];
  for(unsigned int i=0; i<Dimension; i++)
    {
      domainSigmas[i] = 5; //atof( argv[3] );
    }
  const double rangeSigma = 80; //atof( argv[4] );
  bilateralFilter->SetDomainSigma( domainSigmas );
  bilateralFilter->SetRangeSigma(  rangeSigma   );


  // printf("done. Bilateral filtering\n");


  ////////////////////////////////////////////////////////////////////////////////
  // rescale to 0 to ln(255), for the following exp
  rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
  rescaleFilter2->SetInput( bilateralFilter->GetOutput() );
  rescaleFilter2->SetOutputMinimum( 0 );
  rescaleFilter2->SetOutputMaximum( 5.5413 );
  
  // printf("done. rescale to 0 to 255 for write\n");


  ////////////////////////////////////////////////////////////////////////////////
  // exp and 
  typedef itk::ExpImageFilter< floatImageType, floatImageType > expImageFilterType;
  expImageFilterType::Pointer expImageFilter = expImageFilterType::New();
  expImageFilter->SetInput( rescaleFilter2->GetOutput() );

  // printf("done. exp\n");


  ////////////////////////////////////////////////////////////////////////////////
  // cast back from float to uchar
  typedef itk::CastImageFilter< floatImageType, charImageType > floatToCharCastImageFilterType;
  floatToCharCastImageFilterType::Pointer floatToCharCastImageFilter = floatToCharCastImageFilterType::New();
  floatToCharCastImageFilter->SetInput( expImageFilter->GetOutput() );




  ////////////////////////////////////////////////////////////////////////////////  
  // grow cut!!!
  typedef itk::GrowCutImageFilter< charImageType, LabelMapType > gcType;

  gcType::Pointer gc = gcType::New();
  gc->SetInput( floatToCharCastImageFilter->GetOutput() );
  gc->SetInitLabelMap( labelReader->GetOutput() );


  //////////////////////////////////////////////////
  // write the result
  typedef itk::Image< charPixelType, dim > WriteImageType;
  //  typedef itk::ImageFileWriter< WriteImageType > WriterType;
  typedef itk::ImageFileWriter< charImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( gc->GetOutput() );
  //  writer->SetInput( floatToCharCastImageFilter->GetOutput() );
  try
    {
      writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl;
      std::cout << err << std::endl;
      return -1;
    }

  return 0;
}
