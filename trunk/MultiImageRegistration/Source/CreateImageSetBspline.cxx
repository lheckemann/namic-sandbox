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
#include "itkBSplineDeformableTransform.h"

#include <string>
#include <sstream>
#include <fstream>

#include <cstdlib>
    
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
  
  for(int i=0; i<4 ; i++)
  {

    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();
   
    reader->SetFileName( argv[1] );
    reader->Update();

    typedef itk::ResampleImageFilter<InputImageType,OutputImageType> ResampleFilterType;
    ResampleFilterType::Pointer resample = ResampleFilterType::New();

    typedef itk::BSplineDeformableTransform< double,
                                           Dimension,
                                           3 >     BSplineTransformType;

    BSplineTransformType::Pointer bsplineTransform = BSplineTransformType::New();


    typedef BSplineTransformType::RegionType RegionType;
    RegionType bsplineRegion;
    RegionType::SizeType   gridSizeOnImage;
    RegionType::SizeType   gridBorderSize;
    RegionType::SizeType   totalGridSize;

    gridSizeOnImage.Fill( 5 );
    gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = gridSizeOnImage + gridBorderSize;

    bsplineRegion.SetSize( totalGridSize );

    typedef BSplineTransformType::SpacingType SpacingType;
    SpacingType bsplineSpacing = reader->GetOutput()->GetSpacing();

    typedef BSplineTransformType::OriginType OriginType;
    OriginType bsplineOrigin = reader->GetOutput()->GetOrigin();;

    InputImageType::SizeType ImageSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    for(unsigned int r=0; r<Dimension; r++)
    {
      bsplineSpacing[r] *= floor( static_cast<double>(ImageSize[r] - 1)  /
          static_cast<double>(gridSizeOnImage[r] - 1) );
      bsplineOrigin[r]  -=  bsplineSpacing[r];
    }

    bsplineTransform->SetGridSpacing( bsplineSpacing );
    bsplineTransform->SetGridOrigin( bsplineOrigin );
    bsplineTransform->SetGridRegion( bsplineRegion );
  

    typedef BSplineTransformType::ParametersType     ParametersType;

    const unsigned int numberOfParameters =
                     bsplineTransform->GetNumberOfParameters();
  
    ParametersType bsplineParameters( numberOfParameters );

    bsplineParameters.Fill( 0.0 );

    //Randomly set the parameters
    if(i==0)
    {
      for(int j=0; j<bsplineParameters.GetSize(); j++)
      {
        if ( j%5 == 0 )
          bsplineParameters[j] = 1.5;
        else if (j%5 == 1 )
          bsplineParameters[j] = -1.5;
        else if (j%5 == 2 )
          bsplineParameters[j] = 3;
        else if (j%5 == 3 )
          bsplineParameters[j] = -3;
      }
    }
    else if(i==1)
    {
      for(int j=0; j<bsplineParameters.GetSize(); j++)
      {
        if ( j%5 == 0 )
          bsplineParameters[j] = -1.5;
        else if (j%5 == 1 )
          bsplineParameters[j] = 3;
        else if (j%5 == 2 )
          bsplineParameters[j] = -3;
        else if (j%5 == 3 )
          bsplineParameters[j] = 1.5;
      }
    }
    else if(i==2)
    {
      for(int j=0; j<bsplineParameters.GetSize(); j++)
      {
        if ( j%5 == 0 )
          bsplineParameters[j] = 3;
        else if (j%5 == 1 )
          bsplineParameters[j] = -3;
        else if (j%5 == 2 )
          bsplineParameters[j] = 1.5;
        else if (j%5 == 3 )
          bsplineParameters[j] = -1.5;
      }
    }
    else if(i==3)
    {
      for(int j=0; j<bsplineParameters.GetSize(); j++)
      {
        if ( j%5 == 0 )
          bsplineParameters[j] = -3;
        else if (j%5 == 1 )
          bsplineParameters[j] = 1.5;
        else if (j%5 == 2 )
          bsplineParameters[j] = -1.5;
        else if (j%5 == 3 )
          bsplineParameters[j] = 3;
      }
    }

    bsplineTransform->SetParameters( bsplineParameters );

    resample->SetTransform( bsplineTransform );
    typedef itk::LinearInterpolateImageFunction<
                             InputImageType, double >  InterpolatorType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resample->SetInterpolator( interpolator );
    resample->SetDefaultPixelValue( 1 );

    // Initialize the resampler
    // Get the size of the image
    InputImageType::SizeType   size;
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
      origin[r] -= 5*spacing[r];
    }
    resample->SetSize(size);
    resample->SetOutputOrigin(origin);
    resample->SetOutputSpacing(spacing);

      
    resample->SetInput( reader->GetOutput() );
    writer->SetInput( resample->GetOutput() );
      

    string fname;
    ostringstream fnameStream;
    fnameStream << argv[2] << "/" << argv[3] <<"_" << bsplineParameters.GetSize();
    for(int r=0; r<4; r++ )
    {
      fnameStream << "_" << (bsplineParameters[r]);
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

