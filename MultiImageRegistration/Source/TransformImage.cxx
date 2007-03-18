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
#include <fstream>

using namespace std;

int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  outputImageFile transformParametersFile" << std::endl;
    std::cerr << "note: the transform parameters file should be in the same format as the output of the binary: Congeal" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 3;
  typedef   unsigned short  PixelType;
  typedef itk::Image< PixelType,  Dimension >   ImageType;


  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
  typedef itk::BSplineDeformableTransform< double,
                                           Dimension,
                                           3 >     BSplineTransformType;


  AffineTransformType::ParametersType      affineParameters;
  BSplineTransformType::ParametersType     bsplineParameters;

  typedef BSplineTransformType::RegionType RegionType;
  RegionType::SizeType   gridSize;
  
   //Read the parameters of the transform
  ifstream parametersFile(argv[3]);
  if (parametersFile.fail())
  {
    cout << "parameters file " << argv[3] << " could not be read " << endl;
    return 1;
  }

  // Read the parameters from the file
  string currentString;
  char currentChar;
  unsigned int index = 0;
  unsigned int bsplineParametersSize = 1;
  bool useBspline = false;
  parametersFile >> currentString;
  if ( currentString == "Bspline:")
  {
    useBspline = true;
    // Read the gridSize from the file
    parametersFile >> currentString;
    parametersFile >> currentChar;
    if( currentChar == '[' )
    {
      while (currentChar != ']')
      {
        parametersFile >> gridSize[index];
        bsplineParametersSize *= gridSize[index];
        index++;
        parametersFile >> currentChar;
      }
    }
    bsplineParameters.SetSize(bsplineParametersSize*Dimension);
    index = 0;
    parametersFile >> currentString;
    parametersFile >> currentChar;
    if( currentChar == '[' )
    {
      while (currentChar != ']')
      {
        parametersFile >> bsplineParameters[index++];
        parametersFile >> currentChar;
      }
    }
    parametersFile >> currentString;
  }

  // Read Affine parameters
  if ( currentString == "Affine:")
  {

    // Read the gridSize from the file
    affineParameters.SetSize(Dimension*(Dimension+1));
    index = 0;
    parametersFile >> currentChar;
    if( currentChar == '[' )
    {
      while (currentChar != ']')
      {
        parametersFile >> affineParameters[index++];
        parametersFile >> currentChar;
      }
    }
    
  }

  // Read The input file
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
   
  reader->SetFileName( argv[1] );
  reader->Update();


  //
  //
  //
  // Initialize the affine transform
  //
  //
  //
  AffineTransformType::Pointer affineTransform = AffineTransformType::New();

  affineTransform->SetIdentity();
  AffineTransformType::InputPointType center;
  // Get spacing, origin and size of the images
  ImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
  itk::Point<double, Dimension> origin = reader->GetOutput()->GetOrigin();
  ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

  // Place the center of rotation to the center of the image
  for(int j=0; j< Dimension; j++)
  {
    center[j] = origin[j] + spacing[j]*size[j] / 2.0;
  }
  affineTransform->SetCenter(center);

  affineTransform->SetParameters(affineParameters);



  //
  //
  //
  //  Initialize the Bspline Transform
  //
  //
  //
  BSplineTransformType::Pointer bsplineTransform = BSplineTransformType::New();
  if( useBspline == true)
  {

    // Set region
    RegionType bsplineRegion;
    bsplineRegion.SetSize( gridSize );

    ImageType::RegionType fixedRegion = reader->GetOutput()->GetBufferedRegion();
    ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
    RegionType::SizeType   gridSizeOnImage;
    RegionType::SizeType   gridBorderSize;
    gridBorderSize.Fill(3);
    gridSizeOnImage = gridSize - gridBorderSize;
    
    // Calculate the spacing for the Bspline grid
    for(unsigned int r=0; r<Dimension; r++)
    {
      
      spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
          static_cast<double>(gridSizeOnImage[r] - 1) );
      origin[r]  -=  spacing[r];
    }

    // Set the spacing origin and bsplineRegion
    bsplineTransform->SetGridSpacing( spacing );
    bsplineTransform->SetGridOrigin( origin );
    bsplineTransform->SetGridRegion( bsplineRegion );

    // Set the parameters and the affine transform
    bsplineTransform->SetBulkTransform(affineTransform);
    bsplineTransform->SetParameters( bsplineParameters );


  }

  //
  //
  //
  //  Resample the image using the given transform
  //
  //
  //
  
  typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  if (useBspline == true)
  {
    resample->SetTransform( bsplineTransform );
  }
  else
  {
    resample->SetTransform( affineTransform );
  }

  
  typedef itk::LinearInterpolateImageFunction<
                             ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  resample->SetInterpolator( interpolator );

  resample->SetInput( reader->GetOutput() );
  resample->SetSize(    reader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  reader->GetOutput()->GetOrigin() );
  resample->SetOutputSpacing( reader->GetOutput()->GetSpacing() );
  resample->SetOutputDirection( reader->GetOutput()->GetDirection());
  resample->SetDefaultPixelValue( 0 );


  // Write the tranformed image
  writer->SetImageIO(reader->GetImageIO());
  writer->SetFileName( argv[2] );
  writer->SetInput( resample->GetOutput()   );
  writer->Update();

  return EXIT_SUCCESS;
}

