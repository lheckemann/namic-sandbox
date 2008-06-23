/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/22 00:30:16 $
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
#include "itkLevelSetBasedCellSegmentation.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " featureImage foregroundMask outputLabelImage";
    return EXIT_FAILURE;
    }
  
  const unsigned int Dimension = 3;
  
  typedef float              FeaturePixelType;
  typedef unsigned char      MaskPixelType;
  typedef unsigned long      LabelPixelType;

  typedef itk::Image< FeaturePixelType, Dimension >   FeatureImageType;
  typedef itk::Image< MaskPixelType,    Dimension >   MaskImageType;
  typedef itk::Image< LabelPixelType,   Dimension >   LabelImageType;

  typedef itk::LevelSetBasedCellSegmentation<
      FeatureImageType, MaskImageType, LabelImageType >   FilterType;

  typedef itk::ImageFileReader< FeatureImageType > FeatureReaderType;
  typedef itk::ImageFileReader< MaskImageType >    MaskReaderType;
  typedef itk::ImageFileWriter< LabelImageType >   LabelReaderType;

  FilterType::Pointer   segmenter = FilterType::New();

  FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
  MaskReaderType::Pointer    maskReader    = MaskReaderType::New();
  LabelWriterType::Pointer   labelWriter   = LabelWriterType::New();

  segmenter->SetFeatureImage( featureReader->GetOutput() );
  segmenter->SetMaskImage( maskReader->GetOutput() );
  labelWriter->SetInput( segmenter->GetOutput() );

  featureReader->SetFileName( argv[1] );
  maskReader->SetFileName( argv[2] );
  labelWriter->SetFileName( argv[3] );

  try
    {
    labelWriter->Update();
    }
  catch( itk::Exception & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

