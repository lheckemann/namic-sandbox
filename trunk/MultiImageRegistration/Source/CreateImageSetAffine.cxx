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
#include "itkBSplineDeformableTransform.h"

#include "itkLinearInterpolateImageFunction.h"

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

  const     unsigned int   Dimension = 3;
  typedef   unsigned short  InputPixelType;
  typedef   unsigned short  OutputPixelType;
  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  
  for(int i=-5; i<=5 ; i=i+3)
  {
    for(int j=-5; j<=5 ; j=j+3)
    {
      ReaderType::Pointer reader = ReaderType::New();
      WriterType::Pointer writer = WriterType::New();

      reader->SetFileName( argv[1] );

      typedef itk::ResampleImageFilter<InputImageType,OutputImageType> ResampleFilterType;
      ResampleFilterType::Pointer resample = ResampleFilterType::New();

      typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
      AffineTransformType::Pointer affineTransform = AffineTransformType::New();

      typedef itk::BSplineDeformableTransform< double,
                                           Dimension,
                                           3 >     BSplineTransformType;

      resample->SetTransform( affineTransform );
      typedef itk::LinearInterpolateImageFunction< 
                             InputImageType, double >  InterpolatorType;

      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      resample->SetInterpolator( interpolator );
      resample->SetDefaultPixelValue( 0 );

      // Set the parameters of the affine transform
      affineTransform->SetIdentity();
      AffineTransformType::ParametersType affineParameters;
      affineParameters = affineTransform->GetParameters();
      for( int r=0; r<affineParameters.GetSize(); r++ )
      {
        affineParameters[r] += i;
      }
      
      // Initialize the resampler
      // Get the size of the image
      InputImageType::SizeType   size;
      reader->Update();
      size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
      // Increase the size by 10 pixels (voxels)
      for(int r=0; r<Dimension; r++)
      {
        size[r] += 10;
      }
      
      //Get the spacing
      InputImageType::SpacingType spacing;
      spacing = reader->GetOutput()->GetSpacing();
      //Get the origin
      BSplineTransformType::OriginType origin;
      origin = reader->GetOutput()->GetOrigin();

      // Move the origin 5 spaces
      for(int r=0; r<Dimension; r++ )
      {
        origin[r] += 5*spacing[r];
      }
      resample->SetSize(size);
      resample->SetOutputOrigin(origin);
      resample->SetOutputSpacing(spacing);


      resample->SetInput( reader->GetOutput() );
      writer->SetInput( resample->GetOutput() );
      

      string fname;
      ostringstream fnameStream;
      fnameStream << argv[2] << "/" << argv[3] <<"_" << affineParameters.GetSize();
      for(int r=0; r<affineParameters.GetSize(); r++ )
      {
        fnameStream << "_" << static_cast<int>(affineParameters[r]);
      }
      if(Dimension == 2)
        fnameStream << ".png";
      else
        fnameStream << ".mhd";
      fname = fnameStream.str();
      writer->SetFileName( fname.c_str() );

      std::cout << "Result = " << fname.c_str() << std::endl;
      writer->Update();
    }
  }
  return EXIT_SUCCESS;
}

