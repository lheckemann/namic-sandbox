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

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"


int main( int argc, char *argv[] )
{
  if( argc < 19 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage  outputImage";
    std::cerr << " Sigma SigmoidAlpha SigmoidBeta ";
    std::cerr << " InitialRadiusDistance";
    std::cerr << " seedX seedY seedZ";
    std::cerr << " seedX seedY seedZ";
    std::cerr << " seedX seedY seedZ";
    std::cerr << " seedX seedY seedZ";
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

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  InternalWriterType::Pointer internalWriter = InternalWriterType::New();

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  typedef   itk::CurvatureAnisotropicDiffusionImageFilter< 
                               InternalImageType, 
                               InternalImageType >  SmoothingFilterType;

  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();


  typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< 
                               InternalImageType, 
                               InternalImageType >  GradientFilterType;

  typedef   itk::SigmoidImageFilter<
                               InternalImageType, 
                               InternalImageType >  SigmoidFilterType;



  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();

  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );


  typedef  itk::FastMarchingImageFilter< InternalImageType, InternalImageType >
    FastMarchingFilterType;

  FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();


  typedef  itk::ShapeDetectionLevelSetImageFilter< InternalImageType, 
                              InternalImageType >    ShapeDetectionFilterType;

  ShapeDetectionFilterType::Pointer shapeDetection = ShapeDetectionFilterType::New();


  smoothing->SetInput( reader->GetOutput() );
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  

  shapeDetection->SetFeatureImage( sigmoid->GetOutput() );

  thresholder->SetInput( shapeDetection->GetOutput() );

  writer->SetInput( thresholder->GetOutput() );


  smoothing->SetTimeStep( 0.01 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );

  const double sigma = atof( argv[3] );

  gradientMagnitude->SetSigma(  sigma  );

  const double alpha =  atof( argv[4] );
  const double beta  =  atof( argv[5] );

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );
  

  typedef FastMarchingFilterType::NodeContainer           NodeContainer;
  typedef FastMarchingFilterType::NodeType                NodeType;
  NodeContainer::Pointer seeds = NodeContainer::New();
  

  const double initialDistance = atof( argv[6] );


  InternalImageType::IndexType  seedPosition;
  
  seedPosition[0] = atoi( argv[7] );
  seedPosition[1] = atoi( argv[8] );
  seedPosition[2] = atoi( argv[9] );


  NodeType node;
  const double seedValue = - initialDistance;
  
  seeds->Initialize();

  node.SetValue( seedValue );
  node.SetIndex( seedPosition );

  seeds->InsertElement( 0, node );

  seedPosition[0] = atoi( argv[10] );
  seedPosition[1] = atoi( argv[11] );
  seedPosition[2] = atoi( argv[12] );

  node.SetValue( seedValue );
  node.SetIndex( seedPosition );

  seeds->InsertElement( 1, node );

  seedPosition[0] = atoi( argv[13] );
  seedPosition[1] = atoi( argv[14] );
  seedPosition[2] = atoi( argv[15] );

  node.SetValue( seedValue );
  node.SetIndex( seedPosition );

  seeds->InsertElement( 2, node );

  seedPosition[0] = atoi( argv[16] );
  seedPosition[1] = atoi( argv[17] );
  seedPosition[2] = atoi( argv[18] );

  node.SetValue( seedValue );
  node.SetIndex( seedPosition );

  seeds->InsertElement( 3, node );


  fastMarching->SetInput( sigmoid->GetOutput() );
  fastMarching->SetTrialPoints(  seeds  );

  fastMarching->SetOutputSize( reader->GetOutput()->GetBufferedRegion().GetSize() );

  internalWriter->SetInput( fastMarching->GetOutput() );
  internalWriter->SetFileName("fastMarching.mhd");
  internalWriter->Update();

  typedef itk::BinaryThresholdImageFilter< 
    InternalImageType, InternalImageType > InternalThresholdingFilterType;

  InternalThresholdingFilterType::Pointer thresholder2 = InternalThresholdingFilterType::New();
                        
  thresholder2->SetLowerThreshold( -initialDistance * 2.0 );
  thresholder2->SetUpperThreshold( 0.0 );

  thresholder2->SetOutsideValue(  -4.0  );
  thresholder2->SetInsideValue(    4.0 );

  thresholder2->SetInput( fastMarching->GetOutput() );
  shapeDetection->SetInput( thresholder2->GetOutput() );

  const double curvatureScaling   = 1.0;
  const double propagationScaling = 1.0;

  shapeDetection->SetPropagationScaling(  propagationScaling );
  shapeDetection->SetCurvatureScaling( curvatureScaling ); 


  shapeDetection->SetMaximumRMSError( 0.01 );
  shapeDetection->SetNumberOfIterations( 300 );
  

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  // Print out some useful information 
  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << shapeDetection->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << shapeDetection->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elpased iterations: " << shapeDetection->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << shapeDetection->GetRMSChange() << std::endl;

  return EXIT_SUCCESS;
}
