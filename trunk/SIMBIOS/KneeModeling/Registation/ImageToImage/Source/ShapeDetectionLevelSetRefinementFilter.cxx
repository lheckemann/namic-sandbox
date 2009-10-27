/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ShapeDetectionLevelSetFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-17 21:44:43 $
  Version:   $Revision: 1.41 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"



class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;

  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:

  typedef   float           InputPixelType;
  typedef   float           InternalPixelType;

  typedef itk::Image< InputPixelType, 3 >     InputImageType;
  typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

  typedef itk::ShapeDetectionLevelSetImageFilter< 
    InternalImageType, InputImageType > ThresholdSegmentationLevelSetImageFilterType;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    const ThresholdSegmentationLevelSetImageFilterType * filter = 
      dynamic_cast< const ThresholdSegmentationLevelSetImageFilterType * >( object );

    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << filter->GetElapsedIterations() << "  ";
    std::cout << filter->GetRMSChange() << std::endl;
  }
   
};



int main( int argc, char *argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputEdgeMap  inputLevelSet outputLevelSet";
    std::cerr << " CurvatureWeight PropagationWeight";
    std::cerr << " MaximumNumberOfLevelSetIterations";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


  typedef unsigned char     OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


  typedef itk::BinaryThresholdImageFilter< 
    InternalImageType, OutputImageType > ThresholdingFilterType;

  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
                        
  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );

  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  255 );


  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;
  typedef  itk::ImageFileWriter< InternalImageType > InternalWriterType;

  ReaderType::Pointer sigmoidReader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  ReaderType::Pointer levelSetReader = ReaderType::New();

  InternalWriterType::Pointer internalWriter = InternalWriterType::New();

  sigmoidReader->SetFileName( argv[1] );
  levelSetReader->SetFileName( argv[2] );
  writer->SetFileName( argv[3] );

  typedef   itk::CurvatureAnisotropicDiffusionImageFilter< 
                               InternalImageType, 
                               InternalImageType >  SmoothingFilterType;


  typedef  itk::ShapeDetectionLevelSetImageFilter< InternalImageType, 
                              InternalImageType >    ShapeDetectionFilterType;

  ShapeDetectionFilterType::Pointer shapeDetection = ShapeDetectionFilterType::New();



  shapeDetection->SetFeatureImage( sigmoidReader->GetOutput() );

  thresholder->SetInput( shapeDetection->GetOutput() );

  writer->SetInput( thresholder->GetOutput() );

  typedef itk::BinaryThresholdImageFilter< 
    InternalImageType, InternalImageType > InternalThresholdingFilterType;

  InternalThresholdingFilterType::Pointer thresholder2 = InternalThresholdingFilterType::New();
                        
  thresholder2->SetLowerThreshold( 0.0 );
  thresholder2->SetUpperThreshold( 127.5 );

  thresholder2->SetOutsideValue(   4.0  );
  thresholder2->SetInsideValue(   -4.0 );

  thresholder2->SetInput( levelSetReader->GetOutput() );
  shapeDetection->SetInput( thresholder2->GetOutput() );

  shapeDetection->UseImageSpacingOn();

  internalWriter->SetInput( thresholder2->GetOutput() );
  internalWriter->SetFileName( "ShapeDetectionInput.mhd" );
  internalWriter->Update();


  const double curvatureScaling   = atof( argv[4] );
  const double propagationScaling = atof( argv[5] );

  shapeDetection->SetPropagationScaling(  propagationScaling );
  shapeDetection->SetCurvatureScaling( curvatureScaling ); 

  const unsigned int numberOfIterations = atoi( argv[6] );

  std::cout << "Maximum number of iterations = " << numberOfIterations << std::endl;

  shapeDetection->SetMaximumRMSError( 0.0001 );
  shapeDetection->SetNumberOfIterations( numberOfIterations );
  

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

  shapeDetection->AddObserver( itk::IterationEvent(), observer );


  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  internalWriter->SetInput( shapeDetection->GetOutput() );
  internalWriter->SetFileName( "ShapeDetection.mhd" );
  internalWriter->Update();

  // Print out some useful information 
  std::cout << std::endl;
  std::cout << "Maximum number of iterations: " << shapeDetection->GetNumberOfIterations() << std::endl;
  std::cout << "Maximum RMS error: " << shapeDetection->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "Number of elapsed iterations: " << shapeDetection->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << shapeDetection->GetRMSChange() << std::endl;

  return EXIT_SUCCESS;
}
