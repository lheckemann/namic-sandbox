/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: BSplineWarping2.cxx,v $
  Language:  C++
  Date:      $Date: 2008-04-21 17:40:10 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageFileReader.h" 
#include "itkImageFileWriter.h" 
#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkAffineTransform.h"
#include "itkBSplineDeformableTransform.h"
#include "itkTransformFileReader.h"


#include "itkCommand.h"
class CommandProgressUpdate : public itk::Command 
{
public:
  typedef  CommandProgressUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandProgressUpdate() {};
public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      const itk::ProcessObject * filter = 
        dynamic_cast< const itk::ProcessObject * >( object );
      if( ! itk::ProgressEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << filter->GetProgress() << std::endl;
    }
};


int main( int argc, char * argv[] )
{

  if( argc < 5 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << "fixedImage movingImage ";
    std::cerr << "affineTransformFile bsplineTransformFile ";
    std::cerr << "deformedMovingImage" << std::endl;
    std::cerr << "[deformationField]" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   ImageDimension = 3;

  typedef   signed short PixelType;

  typedef   itk::Image< PixelType, ImageDimension >  FixedImageType;
  typedef   itk::Image< PixelType, ImageDimension >  MovingImageType;

  typedef   itk::ImageFileReader< FixedImageType  >  FixedReaderType;
  typedef   itk::ImageFileReader< MovingImageType >  MovingReaderType;

  typedef   itk::ImageFileWriter< MovingImageType >  MovingWriterType;


  FixedReaderType::Pointer fixedReader = FixedReaderType::New();
  fixedReader->SetFileName( argv[1] );

  try
    {
    fixedReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  MovingReaderType::Pointer movingReader = MovingReaderType::New();
  MovingWriterType::Pointer movingWriter = MovingWriterType::New();

  movingReader->SetFileName( argv[2] );
  movingWriter->SetFileName( argv[5] );


  FixedImageType::ConstPointer fixedImage = fixedReader->GetOutput();


  typedef itk::ResampleImageFilter< MovingImageType, 
                                    FixedImageType  >  FilterType;

  FilterType::Pointer resampler = FilterType::New();

  typedef itk::LinearInterpolateImageFunction< 
                       MovingImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator( interpolator );

  FixedImageType::SpacingType   fixedSpacing    = fixedImage->GetSpacing();
  FixedImageType::PointType     fixedOrigin     = fixedImage->GetOrigin();
  FixedImageType::DirectionType fixedDirection  = fixedImage->GetDirection();

  resampler->SetOutputSpacing( fixedSpacing );
  resampler->SetOutputOrigin(  fixedOrigin  );
  resampler->SetOutputDirection(  fixedDirection  );

  
  FixedImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
  FixedImageType::SizeType   fixedSize =  fixedRegion.GetSize();
  resampler->SetSize( fixedSize );
  resampler->SetOutputStartIndex(  fixedRegion.GetIndex() );


  resampler->SetInput( movingReader->GetOutput() );
  
  movingWriter->SetInput( resampler->GetOutput() );

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::AffineTransform< double, SpaceDimension > AffineTransformType;

  AffineTransformType::Pointer  affineTransform = AffineTransformType::New();

  typedef itk::BSplineDeformableTransform<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;
  
  TransformType::Pointer bsplineTransform = TransformType::New();

  typedef itk::TransformFileReader     TransformReaderType;

  {
  TransformReaderType::Pointer transformReader = TransformReaderType::New();

  std::cout << "Reading Affine transform from " << argv[3] << std::endl;

  transformReader->SetFileName( argv[3] );
  transformReader->Update();

  typedef TransformReaderType::TransformListType * TransformListType;

  TransformListType transforms = transformReader->GetTransformList();

  TransformReaderType::TransformListType::const_iterator titr = transforms->begin();

  if( !strcmp((*titr)->GetNameOfClass(),"AffineTransform") )
    {
    affineTransform = dynamic_cast< AffineTransformType * >( titr->GetPointer() );
  
    if( !affineTransform )
      {
      std::cerr << "Error reading Affine Transform" << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    std::cerr << "Input file does not contain an Affine Transform" << std::endl;
    std::cerr << "Transform type is: " << (*titr)->GetNameOfClass() << std::endl;
    return EXIT_FAILURE;
    }
  }


  {
  TransformReaderType::Pointer transformReader = TransformReaderType::New();

  std::cout << "Reading BSpline transform from " << argv[4] << std::endl;

  transformReader->SetFileName( argv[4] );
  transformReader->Update();

  typedef TransformReaderType::TransformListType * TransformListType;

  TransformListType transforms = transformReader->GetTransformList();

  TransformReaderType::TransformListType::const_iterator titr = transforms->begin();

  if( !strcmp((*titr)->GetNameOfClass(),"BSplineDeformableTransform") )
    {
    bsplineTransform = dynamic_cast< TransformType * >( titr->GetPointer() );
  
    if( !bsplineTransform )
      {
      std::cerr << "Error reading BSpline Transform" << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    std::cerr << "Input file does not contain a BSpline Transform" << std::endl;
    std::cerr << "Transform type is: " << (*titr)->GetNameOfClass() << std::endl;
    return EXIT_FAILURE;
    }
  }

  bsplineTransform->SetBulkTransform( affineTransform );

  CommandProgressUpdate::Pointer observer = CommandProgressUpdate::New();

  resampler->AddObserver( itk::ProgressEvent(), observer );
  

  resampler->SetTransform( bsplineTransform );
  
  try
    {
    movingWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  typedef itk::Point<  float, ImageDimension >  PointType;
  typedef itk::Vector< float, ImageDimension >  VectorType;
  typedef itk::Image< VectorType, ImageDimension >  DeformationFieldType;

  DeformationFieldType::Pointer field = DeformationFieldType::New();
  field->SetRegions( fixedRegion );
  field->SetOrigin( fixedOrigin );
  field->SetSpacing( fixedSpacing );
  field->SetDirection( fixedDirection );
  field->Allocate();

  typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
  FieldIterator fi( field, fixedRegion );

  fi.GoToBegin();

  TransformType::InputPointType  fixedPoint;
  TransformType::OutputPointType movingPoint;
  DeformationFieldType::IndexType index;

  VectorType displacement;

  while( ! fi.IsAtEnd() )
    {
    index = fi.GetIndex();
    field->TransformIndexToPhysicalPoint( index, fixedPoint );
    movingPoint = bsplineTransform->TransformPoint( fixedPoint );
    displacement = movingPoint - fixedPoint;
    fi.Set( displacement );
    ++fi;
    }


  typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
  FieldWriterType::Pointer fieldWriter = FieldWriterType::New();

  fieldWriter->SetInput( field );

  if( argc >= 7 )
    {
    fieldWriter->SetFileName( argv[6] );
    try
      {
      fieldWriter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}
