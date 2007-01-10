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
    std::cerr << argv[0] << "  inputImageFile  outputFolderName outputImageFile";
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 2;
  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;
  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;


  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ofstream filenames("GeneratedBsplineFileNames.txt");

    for(int i=-20; i<=20 ; i=i+20)
      {
      for(int j=0; j<=0 ; j=j+20)
        {
        for(int k=0; k<=0 ; k=k+20)
          {
          for(int l=0; l<=0 ; l=l+20)
            {
            for(int m=0; m<=0 ; m=m+20)
              {
              ReaderType::Pointer reader = ReaderType::New();
              WriterType::Pointer writer = WriterType::New();

              reader->SetFileName( argv[1] );
              writer->SetFileName( argv[1] );
                
              typedef itk::ResampleImageFilter<InputImageType,OutputImageType> FilterType;
              FilterType::Pointer filter = FilterType::New();

              
              const unsigned int SpaceDimension = Dimension;
              const unsigned int SplineOrder = 3;
              typedef double CoordinateRepType;

              typedef itk::BSplineDeformableTransform<
                  CoordinateRepType,
              SpaceDimension,
              SplineOrder >     TransformType;


            //  typedef itk::AffineTransform< double, Dimension >  TransformType;
              TransformType::Pointer transform = TransformType::New();

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

              typedef TransformType::RegionType RegionType;
              RegionType bsplineRegion;
              RegionType::SizeType   gridSizeOnImage;
              RegionType::SizeType   gridBorderSize;
              RegionType::SizeType   totalGridSize;

              gridSizeOnImage.Fill( 2 );
              gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
              totalGridSize = gridSizeOnImage + gridBorderSize;

              bsplineRegion.SetSize( totalGridSize );

              typedef TransformType::SpacingType SpacingType;
              SpacingType spacing = reader->GetOutput()->GetSpacing();

              typedef TransformType::OriginType OriginType;
              OriginType origin = reader->GetOutput()->GetOrigin();;

              InputImageType::RegionType fixedRegion = reader->GetOutput()->GetBufferedRegion();
              InputImageType::SizeType fixedImageSize = fixedRegion.GetSize();

              for(unsigned int r=0; r<Dimension; r++)
              {
                spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
                    static_cast<double>(gridSizeOnImage[r] - 1) );
                origin[r]  -=  spacing[r];
              }

              transform->SetGridSpacing( spacing );
              transform->SetGridOrigin( origin );
              transform->SetGridRegion( bsplineRegion );
              

              typedef TransformType::ParametersType     ParametersType;

              const unsigned int numberOfParameters =
                  transform->GetNumberOfParameters();
              
              ParametersType parameters( numberOfParameters );

              parameters.Fill( 0.0 );
              /*
              * Set the transformation parameters
              */
              for( int n=0; n<numberOfParameters; n++)
              {
                if (n%5 == 0)
                  parameters[n] = i;
                else if(n%5 == 1)
                  parameters(n) = j;
                else if(n%5 == 2)
                  parameters(n) = k;
                else if(n%5 == 3)
                  parameters(n) = l;
                else if(n%5 == 4)
                  parameters(n) = m;
                //parameters[n] = 10 - rand() % 20;
                parameters[n] = i;
              }

              transform->SetParameters( parameters );

              //  std::cout << "Intial Parameters = " << std::endl;
              //  std::cout << parameters << std::endl;
              
              filter->SetTransform( transform );
              
              string fname;
              ostringstream fnameStream;
              fnameStream << argv[2] << "/" << argv[3] <<"_" << i << "_" << j << "_" << k << "_" << l << "_" << m <<".png" ;
              fname = fnameStream.str();
              filenames << argv[3] <<"_" << i << "_" << j << "_" << k << "_" << l << "_" << m <<".png" << " ";
              writer->SetFileName( fname.c_str() );
              

              filter->SetDefaultPixelValue( 0 );

              std::cout << "Result = " << fname.c_str() << std::endl;
              //  std::cout << " Translation X = " << parameters;
              //  std::cout << " Translation Y = " << parameters[1]  << std::endl;
              writer->Update();
              }
            }
          }
        }
      }

  return EXIT_SUCCESS;
}

