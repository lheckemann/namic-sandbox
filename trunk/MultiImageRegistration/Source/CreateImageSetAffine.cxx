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

#include "itkImageRegionIterator.h"
#include "itkNormalVariateGenerator.h"
    
#include <string>
#include <sstream>

#include <itksys/SystemTools.hxx>

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
  const     double   Sigma = 2;
  typedef   unsigned short  InputPixelType;
  typedef   unsigned short  OutputPixelType;
  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  typedef itk::ImageRegionIterator< InputImageType> IteratorType;

  typedef itk::Statistics::NormalVariateGenerator GeneratorType;
  
  GeneratorType::Pointer generator = GeneratorType::New();
  generator->Initialize(981645);

  //Create the output folder
  itksys::SystemTools::MakeDirectory( argv[2] );
  
  for(int i=0; i<30 ; i++)
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
    
    // Add Gaussian Noise to the images
    IteratorType it( reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
    for ( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
      //it.Set( abs( it.Get() + static_cast<int>(generator->GetVariate()*Sigma) ) );
    }
    
    // Set the parameters of the affine transform
    //Get the spacing
    InputImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
    //Get the origin
    BSplineTransformType::OriginType origin;
    origin = reader->GetOutput()->GetOrigin();
    
    InputImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    AffineTransformType::InputPointType center;
    for(int j=0; j< Dimension; j++)
    {
      center[j] = origin[j] + spacing[j]*size[j] / 2.0;
    }
    affineTransform->SetIdentity();
    affineTransform->SetCenter(center);
    AffineTransformType::ParametersType affineParameters;
    affineParameters = affineTransform->GetParameters();

    /*
    if(i==1)
    {
      if(Dimension == 2)
      {
        affineParameters[0] = 1.03;
        affineParameters[1] = 0.15;
            
        affineParameters[2] = -0.15;
        affineParameters[3] = 1.03;

        affineParameters[4] = 5;
        affineParameters[5] = 5;
      }
      else
      {
        affineParameters[0] = 1.03;
        affineParameters[1] = 0.15;
        affineParameters[2] = 0.05;
      
        affineParameters[3] = -0.15;
        affineParameters[4] = 1.03;
        affineParameters[5] = 0.15;
      
        affineParameters[6] = -0.05;
        affineParameters[7] = -0.15;
        affineParameters[8] = 1.03;
      
        affineParameters[9] = 5;
        affineParameters[10] = 5;
        affineParameters[11] = 5;
      }
    }
    else if(i==2)
    {
      if(Dimension == 2)
      {
        affineParameters[0] = 0.97;
        affineParameters[1] = -0.15;
            
        affineParameters[2] = 0.15;
        affineParameters[3] = 0.97;

        affineParameters[4] = -5;
        affineParameters[5] = -5;
      }
      else
      {
        affineParameters[0] = 0.97;
        affineParameters[1] = -0.15;
        affineParameters[2] = -0.05;
      
        affineParameters[3] = 0.15;
        affineParameters[4] = 0.97;
        affineParameters[5] = -0.15;
      
        affineParameters[6] = 0.05;
        affineParameters[7] = 0.15;
        affineParameters[8] = 0.97;
      
        affineParameters[9] = -5;
        affineParameters[10] = -5;
        affineParameters[11] = -5;
      }
    }
    */
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
    // Increase the size by 10 pixels (voxels)
    for(int r=0; r<Dimension; r++)
    {
      size[r] += 6;
    }
      

    // Move the origin 5 spaces
    for(int r=0; r<Dimension; r++ )
    {
      origin[r] -= 3*spacing[r];
    }
    resample->SetSize(size);
    resample->SetOutputOrigin(origin);
    resample->SetOutputSpacing(spacing);
    resample->SetDefaultPixelValue( 100 );
    resample->SetOutputDirection( reader->GetOutput()->GetDirection());
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

