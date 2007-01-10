/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ResampleImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/14 12:12:52 $
  Version:   $Revision: 1.32 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include <string>
#include <sstream>
using namespace std;

int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  folderName outputImageFile";
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 2;
  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;
  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  
  ofstream filenames("GeneratedAffineFileNames.txt");
  
   for(int i=-20; i<=20 ; i=i+3)
      {
      for(int j=-20; j<=20 ; j=j+3)
        {
        ReaderType::Pointer reader = ReaderType::New();
        WriterType::Pointer writer = WriterType::New();

        reader->SetFileName( argv[1] );
        writer->SetFileName( argv[1] );

        typedef itk::ResampleImageFilter<InputImageType,OutputImageType> FilterType;
        FilterType::Pointer filter = FilterType::New();

        typedef itk::AffineTransform< double, Dimension >  TransformType;
        TransformType::Pointer transform = TransformType::New();
        filter->SetTransform( transform );
        typedef itk::NearestNeighborInterpolateImageFunction< 
                             InputImageType, double >  InterpolatorType;

        InterpolatorType::Pointer interpolator = InterpolatorType::New();
        filter->SetInterpolator( interpolator );
        filter->SetDefaultPixelValue( 0 );
         
         InputImageType::SizeType   size;
       
         size[0] = 221;  // number of pixels along X
         size[1] = 257;  // number of pixels along Y
       
         filter->SetSize( size );

        filter->SetInput( reader->GetOutput() );
        writer->SetInput( filter->GetOutput() );
        writer->Update();
        TransformType::OutputVectorType translation;



        translation[0] = i;  // X translation in millimeters
        translation[1] = j;  // Y translation in millimeters
        transform->Translate( translation );

        string fname;
        ostringstream fnameStream;
        fnameStream << argv[2] << "/" << argv[3] <<"_X_" << i << "_Y_" << j << ".png" ;
        filenames << argv[3] <<"_X_" << i << "_Y_" << j << ".png ";
        fname = fnameStream.str();
        writer->SetFileName( fname.c_str() );
        

        filter->SetDefaultPixelValue( 0 );

        std::cout << "Result = " << fname.c_str() << std::endl;
        std::cout << " Translation X = " << translation[0];
        std::cout << " Translation Y = " << translation[1]  << std::endl;
        writer->Update();
        }
      }

  return EXIT_SUCCESS;
}

