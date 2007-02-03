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
  
  for(int i=0; i<3 ; i++)
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
    

    // Set the parameters of the affine transform
    affineTransform->SetIdentity();
    AffineTransformType::ParametersType affineParameters;
    affineParameters = affineTransform->GetParameters();

    if(i==1)
    {
      affineParameters[0] = 1.01;
      affineParameters[1] = 0.02;
      affineParameters[2] = 0.01;
      
      affineParameters[3] = 0.02;
      affineParameters[4] = 1.01;
      affineParameters[5] = 0.02;
      
      affineParameters[6] = 0.01;
      affineParameters[7] = 0.02;
      affineParameters[8] = 1.01;
      
      affineParameters[9] = 1;
      affineParameters[10] = 1;
      affineParameters[11] = 1;
    }
    else if(i==2)
    {
      affineParameters[0] = 0.99;
      affineParameters[1] = -0.02;
      affineParameters[2] = -0.01;
      
      affineParameters[3] = -0.02;
      affineParameters[4] = 0.99;
      affineParameters[5] = -0.02;
      
      affineParameters[6] = -0.01;
      affineParameters[7] = -0.02;
      affineParameters[8] = 0.99;
      
      affineParameters[9] = -1;
      affineParameters[10] = -1;
      affineParameters[11] = -1;
    }
    affineTransform->SetParameters(affineParameters);
      
    // Initialize the resampler
    // Get the size of the image
    InputImageType::SizeType   size;
    reader->Update();
    size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    // Increase the size by 10 pixels (voxels)
    for(int r=0; r<Dimension; r++)
    {
      //size[r] += 10;
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
      //origin[r] -= 5*spacing[r];
    }
    resample->SetSize(size);
    resample->SetOutputOrigin(origin);
    resample->SetOutputSpacing(spacing);
    resample->SetDefaultPixelValue( 0 );
      
    resample->SetInput( reader->GetOutput() );
    writer->SetInput( resample->GetOutput() );
      

    string fname;
    ostringstream fnameStream;
    fnameStream << argv[2] << "/" << argv[3] <<"_" << affineParameters.GetSize();
    for(int r=0; r<affineParameters.GetSize(); r++ )
    {
      fnameStream << "_" << (affineParameters[r]);
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
  return EXIT_SUCCESS;
}

