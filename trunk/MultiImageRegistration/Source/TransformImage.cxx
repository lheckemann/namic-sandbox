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
#include "itkTransformFactory.h"

#include "itkLinearInterpolateImageFunction.h"

#include <string>
#include <sstream>
#include <fstream>

//Define the global types for image type
#define PixelType float
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"    
    

int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  <inputImageFile>  <outputImageFile> <transformParametersFile> ";
    std::cerr << "  <transformType:(affine,bspline)>" << std::endl;
    std::cerr << "   Usage: input.img output.img Tfile.txt affine " << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< PixelType,  Dimension >   ImageType;

  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  // typedef for transformation types
  typedef itk::Transform< double, Dimension >  TransformType;
  typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
  typedef itk::BSplineDeformableTransform< double,
                                           Dimension,
                                           3 >     BSplineTransformType;
                                           
  itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
  BSplineTransformType::Pointer  bsplineTransform = BSplineTransformType::New();
  f->RegisterTransform(bsplineTransform->GetTransformTypeAsString().c_str(),
                       bsplineTransform->GetTransformTypeAsString().c_str(),
                       bsplineTransform->GetTransformTypeAsString().c_str(),
                       1,
                       itk::CreateObjectFunction<BSplineTransformType>::New());
  
  AffineTransformType::Pointer  affineTransform = AffineTransformType::New();
  f->RegisterTransform(affineTransform->GetTransformTypeAsString().c_str(),
                       affineTransform->GetTransformTypeAsString().c_str(),
                       affineTransform->GetTransformTypeAsString().c_str(),
                       1,
                       itk::CreateObjectFunction<AffineTransformType>::New());
  
  // Typedef for reader
  typedef itk::TransformFileReader    TransformFileReader;

  TransformFileReader::Pointer      transformFileReader = TransformFileReader::New();
  transformFileReader->SetFileName(argv[3]);
  transformFileReader->Update();

  typedef TransformFileReader::TransformListType   TransformListType;
  TransformListType*   transformList = transformFileReader->GetTransformList();


  const std::string type(argv[4]);
  TransformType::Pointer   transform;
  for(TransformListType::iterator transformIter=transformList->begin(); transformIter !=transformList->end() ; transformIter++)
  {
    if(type=="bspline")
    {
      BSplineTransformType::Pointer  localbsplineTransform = BSplineTransformType::New();
      localbsplineTransform->SetFixedParameters((*transformIter)->GetFixedParameters());
      localbsplineTransform->SetParameters((*transformIter)->GetParameters());
      transform = localbsplineTransform;
    }
    else
    {
      AffineTransformType::Pointer  localaffineTransform = AffineTransformType::New();
      localaffineTransform->SetParameters((*transformIter)->GetParameters());
      localaffineTransform->SetFixedParameters((*transformIter)->GetFixedParameters());
      transform = localaffineTransform;
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
  //  Resample the image using the given transform
  //
  //
  //
  
  typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  typedef itk::LinearInterpolateImageFunction<
                             ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  resample->SetInterpolator( interpolator );
  resample->SetTransform( transform );

  ImageType::Pointer imagePointer = reader->GetOutput();
  ResampleFilterType::OriginPointType  origin = reader->GetOutput()->GetOrigin();
  origin[0] = 111.5625;
  origin[1] = 111.5625;
  origin[2] = -138.0;
  imagePointer->SetOrigin(origin);
  resample->SetInput( imagePointer );
  resample->SetSize(    imagePointer->GetLargestPossibleRegion().GetSize() );

  resample->SetOutputOrigin(  origin );
  resample->SetOutputSpacing( imagePointer->GetSpacing() );
  resample->SetOutputDirection( imagePointer->GetDirection());
  resample->SetDefaultPixelValue( 0 );


  // Write the tranformed image
  writer->SetImageIO(reader->GetImageIO());
  writer->SetFileName( argv[2] );
  writer->SetInput( resample->GetOutput()   );
  writer->Update();

  return EXIT_SUCCESS;
}

