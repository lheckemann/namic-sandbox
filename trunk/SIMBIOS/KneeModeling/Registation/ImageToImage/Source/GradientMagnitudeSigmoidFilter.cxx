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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkTimeProbesCollectorBase.h"


int main( int argc, char *argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage  outputImage";
    std::cerr << " Sigma SigmoidAlpha SigmoidBeta ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 3;

  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


  typedef unsigned char     OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  itk::TimeProbesCollectorBase  chronometer;

  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter< InternalImageType > WriterType;

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

  smoothing->SetInput( reader->GetOutput() );
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );

  smoothing->SetTimeStep( 0.01 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );

  const double sigma = atof( argv[4] );

  gradientMagnitude->SetSigma(  sigma  );

  const double alpha =  atof( argv[5] );
  const double beta  =  atof( argv[6] );

  std::cout << "alpha = " << alpha << std::endl;
  std::cout << "beta = " << beta << std::endl;

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );
  

  try
    {
    chronometer.Start("reading");
    reader->Update();
    chronometer.Stop("reading");
    chronometer.Report( std::cout );

    chronometer.Start("smoothing");
    smoothing->Update();
    chronometer.Stop("smoothing");
    chronometer.Report( std::cout );

    chronometer.Start("gradient");
    gradientMagnitude->Update();
    chronometer.Stop("gradient");
    chronometer.Report( std::cout );

    chronometer.Start("sigmoid");
    sigmoid->Update();
    chronometer.Stop("sigmoid");
    chronometer.Report( std::cout );

    writer->SetInput( sigmoid->GetOutput() );
    writer->SetFileName( argv[3] );
    chronometer.Start("writer");
    writer->Update();
    chronometer.Stop("writer");
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  chronometer.Report( std::cout );

  return EXIT_SUCCESS;
}
