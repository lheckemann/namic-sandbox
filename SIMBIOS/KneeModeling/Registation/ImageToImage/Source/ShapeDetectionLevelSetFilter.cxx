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
  if( argc < 11 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage  outputImage";
    std::cerr << " seedX seedY InitialDistance";
    std::cerr << " Sigma SigmoidAlpha SigmoidBeta ";
    std::cerr << " curvatureScaling propagationScaling" << std::endl;
    return 1;
    }


  typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 3;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;



  typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType >
    ThresholdingFilterType;
  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
                        


  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );

  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  255 );


  // We instantiate reader and writer types in the following lines.
  //
  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

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
  ShapeDetectionFilterType::Pointer 
    shapeDetection = ShapeDetectionFilterType::New();



  smoothing->SetInput( reader->GetOutput() );
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  
  shapeDetection->SetInput( fastMarching->GetOutput() );
  shapeDetection->SetFeatureImage( sigmoid->GetOutput() );

  thresholder->SetInput( shapeDetection->GetOutput() );

  writer->SetInput( thresholder->GetOutput() );



  smoothing->SetTimeStep( 0.125 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );



  const double sigma = atof( argv[6] );

  gradientMagnitude->SetSigma(  sigma  );



  const double alpha =  atof( argv[7] );
  const double beta  =  atof( argv[8] );

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );

  

  typedef FastMarchingFilterType::NodeContainer           NodeContainer;
  typedef FastMarchingFilterType::NodeType                NodeType;
  NodeContainer::Pointer seeds = NodeContainer::New();
  

  InternalImageType::IndexType  seedPosition;
  
  seedPosition[0] = atoi( argv[3] );
  seedPosition[1] = atoi( argv[4] );



  const double initialDistance = atof( argv[5] );

  NodeType node;
  const double seedValue = - initialDistance;
  
  node.SetValue( seedValue );
  node.SetIndex( seedPosition );



  seeds->Initialize();
  seeds->InsertElement( 0, node );



  fastMarching->SetTrialPoints(  seeds  );



  fastMarching->SetSpeedConstant( 1.0 );


  fastMarching->SetOutputSize( reader->GetOutput()->GetBufferedRegion().GetSize() );



  const double curvatureScaling   = atof( argv[  9 ] ); 
  const double propagationScaling = atof( argv[ 10 ] ); 

  shapeDetection->SetPropagationScaling(  propagationScaling );
  shapeDetection->SetCurvatureScaling( curvatureScaling ); 



  shapeDetection->SetMaximumRMSError( 0.02 );
  shapeDetection->SetNumberOfIterations( 800 );

  

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
