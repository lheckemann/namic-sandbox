/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SpatialObjectToImage1.cxx,v $
  Language:  C++
  Date:      $Date: 2009-04-01 12:31:37 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkSpatialObjectToImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkCylinderSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "itkImageFileWriter.h"


int main( int argc, char *argv[] )
{
  if( argc != 2 )
    {
    std::cerr << "Usage: " << argv[0] << " outputimagefile " << std::endl;
    return EXIT_FAILURE;
    }



  typedef signed short  PixelType;
  const unsigned int    Dimension = 3;

  typedef itk::Image< PixelType, Dimension >       ImageType;

  typedef itk::EllipseSpatialObject< Dimension >   EllipseType;
  typedef itk::GroupSpatialObject< Dimension >     GroupType;


  typedef itk::SpatialObjectToImageFilter< 
    GroupType, ImageType >   SpatialObjectToImageFilterType;

  SpatialObjectToImageFilterType::Pointer imageFilter =
    SpatialObjectToImageFilterType::New();


  ImageType::SizeType size;
  size[ 0 ] =  50;
  size[ 1 ] =  50;
  size[ 2 ] =  25;

  imageFilter->SetSize( size );

  ImageType::SpacingType spacing;
  spacing[0] =  100.0 / size[0];
  spacing[1] =  100.0 / size[1];
  spacing[2] =  100.0 / size[2];

  imageFilter->SetSpacing( spacing );



  EllipseType::Pointer ellipse    = EllipseType::New();

  ellipse->SetRadius(  size[0] * 0.45 * spacing[0] );

  typedef GroupType::TransformType    TransformType;

  TransformType::Pointer transform1 = TransformType::New();

  transform1->SetIdentity();

  TransformType::OutputVectorType  translation;
  TransformType::CenterType        center;

  translation[ 0 ] =  size[0] * spacing[0] / 2.0;
  translation[ 1 ] =  size[1] * spacing[1] / 2.0;
  translation[ 2 ] =  size[2] * spacing[2] / 2.0;

  transform1->Translate( translation, false );

  ellipse->SetObjectToParentTransform( transform1 );

  GroupType::Pointer group = GroupType::New();
  group->AddSpatialObject( ellipse );

  imageFilter->SetInput(  group  );



  const PixelType airHounsfieldUnits  = -1000;
  const PixelType boneHounsfieldUnits =   800;

  ellipse->SetDefaultInsideValue(   boneHounsfieldUnits );
  ellipse->SetDefaultOutsideValue(   airHounsfieldUnits );

  imageFilter->SetUseObjectValue( true );
  imageFilter->SetOutsideValue( airHounsfieldUnits );

  typedef itk::ImageFileWriter< ImageType >     WriterType;
  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( argv[1] );
  writer->SetInput( imageFilter->GetOutput() );

  try
    {
    imageFilter->Update();
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
