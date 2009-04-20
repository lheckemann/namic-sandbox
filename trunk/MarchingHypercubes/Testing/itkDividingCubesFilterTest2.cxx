/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDividingCubesFilterTest2.cxx,v $
  Language:  C++
  Date:      $Date: 2008-11-21 20:00:48 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageFileReader.h" 
#include "itkDividingCubesFilter.h"
#include "itkImage.h"
#include "itkPointSet.h"


int main( int argc, char * argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage isoSurfaceValue ";
    return EXIT_FAILURE;
    }

  const     unsigned int   ImageDimension = 3;
  typedef   unsigned char  PixelType;

  typedef   itk::Image< PixelType, ImageDimension >   ImageType;

  typedef   itk::CovariantVector< float, ImageDimension >  NormalType;

  typedef   itk::PointSet< NormalType, ImageDimension >    PointSetType;

  typedef   itk::DividingCubesFilter< ImageType, PointSetType > FilterType;

  typedef   itk::ImageFileReader< ImageType  >        ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  FilterType::Pointer dividingCubesFilter = FilterType::New();

  dividingCubesFilter->SetInput( reader->GetOutput() );

  dividingCubesFilter->SetSurfaceValue( atof( argv[2] ) );

  try
    {
    dividingCubesFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  PointSetType::ConstPointer pointSet = dividingCubesFilter->GetOutput();
  
  return EXIT_SUCCESS;
}

