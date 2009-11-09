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

#include "itkFastMarchingImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkTimeProbesCollectorBase.h"



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
  if( argc < 12 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputSigmoidImage  outputImage";
    std::cerr << " InitialRadiusDistance";
    std::cerr << " CurvatureWeight PropagationWeight";
    std::cerr << " MaximumNumberOfLevelSetIterations";
    std::cerr << " InputSeedsFile";
    std::cerr << " outputEdgeMap";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


  typedef unsigned char     OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  itk::TimeProbesCollectorBase  chronometer;

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

  sigmoidReader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  typedef  itk::FastMarchingImageFilter< InternalImageType, InternalImageType >
    FastMarchingFilterType;

  FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();


  typedef  itk::ShapeDetectionLevelSetImageFilter< InternalImageType, 
                              InternalImageType >    ShapeDetectionFilterType;

  ShapeDetectionFilterType::Pointer shapeDetection = ShapeDetectionFilterType::New();

  shapeDetection->SetFeatureImage( sigmoidReader->GetOutput() );

  thresholder->SetInput( shapeDetection->GetOutput() );

  writer->SetInput( thresholder->GetOutput() );

  chronometer.Start("reading");
  sigmoidReader->Update();
  chronometer.Stop("reading");
  chronometer.Report( std::cout );


  typedef FastMarchingFilterType::NodeContainer           NodeContainer;
  typedef FastMarchingFilterType::NodeType                NodeType;
  NodeContainer::Pointer seeds = NodeContainer::New();
  

  const double initialDistance = atof( argv[7] );


  InternalImageType::IndexType  seedPosition;

  NodeType node;
  const double seedValue = - initialDistance;
  
  node.SetValue( seedValue );

  seeds->Initialize();

 
  unsigned int numberOfSeedPoints = 0;

  std::ifstream inputSeedsFile;

  std::cout << "Opening seeds file " << argv[11] << std::endl;
  inputSeedsFile.open( argv[11] );

  if( inputSeedsFile.fail() )
    {
    std::cerr << "Error reading file " << argv[11] << std::endl;
    return EXIT_FAILURE;
    }

  float seedX;
  float seedY;
  float seedZ;

  inputSeedsFile >> seedX >> seedY >> seedZ;

  while( ! inputSeedsFile.eof() )
    {
    seedPosition[0] = seedX;
    seedPosition[1] = seedY;
    seedPosition[2] = seedZ;

    node.SetIndex( seedPosition );

    seeds->InsertElement( numberOfSeedPoints, node );

    std::cout << "Seed " << numberOfSeedPoints << " : " <<  seedX << " " << seedY << " " << seedZ << " = " << seedPosition << std::endl;
    numberOfSeedPoints++;

    inputSeedsFile >> seedX >> seedY >> seedZ;
    }

  inputSeedsFile.close();

  fastMarching->SetInput( sigmoidReader->GetOutput() );
  fastMarching->SetTrialPoints(  seeds  );

  fastMarching->SetOutputSize( sigmoidReader->GetOutput()->GetBufferedRegion().GetSize() );

  fastMarching->SetStoppingValue( 300 ); // related to the size of the object to be segmented.

  chronometer.Start("fastMarching");
  fastMarching->Update();
  chronometer.Stop("fastMarching");
  chronometer.Report( std::cout );


  typedef itk::BinaryThresholdImageFilter< 
    InternalImageType, InternalImageType > InternalThresholdingFilterType;

  InternalThresholdingFilterType::Pointer thresholder2 = InternalThresholdingFilterType::New();
                        
  thresholder2->SetLowerThreshold( -initialDistance * 2.0 );
  thresholder2->SetUpperThreshold( 0.0 );

  thresholder2->SetOutsideValue(   4.0  );
  thresholder2->SetInsideValue(   -4.0 );

  thresholder2->SetInput( fastMarching->GetOutput() );
  shapeDetection->SetInput( thresholder2->GetOutput() );

  shapeDetection->UseImageSpacingOn();


  const double curvatureScaling   = atof( argv[8] );
  const double propagationScaling = atof( argv[9] );

  shapeDetection->SetPropagationScaling(  propagationScaling );
  shapeDetection->SetCurvatureScaling( curvatureScaling ); 

  const unsigned int numberOfIterations = atoi( argv[10] );

  std::cout << "Maximum number of iterations = " << numberOfIterations << std::endl;

  shapeDetection->SetMaximumRMSError( 0.0001 );
  shapeDetection->SetNumberOfIterations( numberOfIterations );
  

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

  shapeDetection->AddObserver( itk::IterationEvent(), observer );

  chronometer.Start("shapeDetection");
  shapeDetection->Update();
  chronometer.Stop("shapeDetection");
  chronometer.Report( std::cout );

  chronometer.Start("thresholding");
  thresholder->Update();
  chronometer.Stop("thresholding");
  chronometer.Report( std::cout );


  try
    {
    chronometer.Start("writer");
    writer->Update();
    chronometer.Stop("writer");
    chronometer.Report( std::cout );
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }


  // Print out some useful information 
  std::cout << std::endl;
  std::cout << "Maximum number of iterations: " << shapeDetection->GetNumberOfIterations() << std::endl;
  std::cout << "Maximum RMS error: " << shapeDetection->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "Number of elapsed iterations: " << shapeDetection->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << shapeDetection->GetRMSChange() << std::endl;

  chronometer.Report( std::cout );

  return EXIT_SUCCESS;
}
