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
#include "itkTransformFileWriter.h"

#include <string>
#include <sstream>

#include <itksys/SystemTools.hxx>

using namespace std;

int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  folderName numberOfImages" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 3;
  typedef   unsigned short  InputPixelType;
  typedef   unsigned short  OutputPixelType;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


  int numberOfImages = atoi(argv[3]);

  for(int i=0; i<numberOfImages ; i++)
  {

    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();
   
    reader->SetFileName( argv[1] );
    reader->Update();

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
    //Get the spacing
    InputImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
    //Get the origin
    BSplineTransformType::OriginType origin;
    origin = reader->GetOutput()->GetOrigin();
    
    InputImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    AffineTransformType::InputPointType center;
    for(unsigned int j=0; j< Dimension; j++)
    {
      center[j] = origin[j] + spacing[j]*size[j] / 2.0;
    }
    affineTransform->SetIdentity();
    affineTransform->SetCenter(center);
    AffineTransformType::ParametersType affineParameters;
    affineParameters = affineTransform->GetParameters();

    affineParameters[0] = 1.0 + (rand()%100/100.0 - 0.5)*0.035;
    affineParameters[1] = (rand()%100/100.0 - 0.5)*0.2;
    affineParameters[2] = (rand()%100/100.0 - 0.5)*0.15;
      
    affineParameters[3] = (rand()%100/100.0 - 0.5)*0.2;
    affineParameters[4] = 1.0 + (rand()%100/100.0 - 0.5)*0.035;
    affineParameters[5] = (rand()%100/100.0 - 0.5)*0.2;
      
    affineParameters[6] = (rand()%100/100.0 - 0.5)*0.15;
    affineParameters[7] = (rand()%100/100.0 - 0.5)*0.2;
    affineParameters[8] = 1.0 + (rand()%100/100.0 - 0.5)*0.035;
      
    affineParameters[9] = (rand()%100/100.0 - 0.5)*10.0;
    affineParameters[10] = (rand()%100/100.0 - 0.5)*10.0;
    affineParameters[11] = (rand()%100/100.0 - 0.5)*10.0;
    
    affineTransform->SetParameters(affineParameters);
      
    // Initialize the resampler
    // Get the size of the image
    size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    resample->SetSize(size);
    resample->SetOutputOrigin(origin);
    resample->SetOutputSpacing(spacing);
    resample->SetDefaultPixelValue( 0 );
    resample->SetOutputDirection( reader->GetOutput()->GetDirection());
    resample->SetInput( reader->GetOutput() );
    writer->SetInput( resample->GetOutput() );
      

    string fname;
    ostringstream fnameStream;
    fnameStream << i ;


    //Write the transform files
    itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
    itksys::SystemTools::MakeDirectory( (fname + argv[2] + "/TransformFiles/").c_str() );

    string fileName = fname + argv[2] + "/TransformFiles/" + fnameStream.str() + ".txt";
    transformFileWriter->SetFileName(fileName.c_str());
    transformFileWriter->SetPrecision(12);
    transformFileWriter->SetInput(affineTransform);
    transformFileWriter->Update();

    itksys::SystemTools::MakeDirectory( (fname+argv[2]+"/Images/").c_str() );
    
    fname = fname + argv[2] + "/Images/" + fnameStream.str();
    if(Dimension == 2)
    {
      fname += ".png";
    }
    else
    {
      fname += ".mhd";
    }
    
    writer->SetFileName( fname.c_str() );
    std::cout << "Writing " << fname.c_str() << std::endl;
    writer->Update();

  }
  return EXIT_SUCCESS;
}

