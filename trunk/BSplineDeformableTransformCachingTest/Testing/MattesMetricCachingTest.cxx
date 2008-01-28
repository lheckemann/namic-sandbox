/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/22 00:30:16 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkTimeProbesCollectorBase.h"

#include "itkMattesMutualInformationImageToImageMetric2.h"
#include "itkMattesNoCachingMutualInformationImageToImageMetric.h"

#include "itkBSplineDeformableTransform2.h"

#include <fstream>
#include <string>

#include "itkExperimentTimeProbesCollector.h"

int main( int argc, char *argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " imagesizeX imagesizeY imagesizeZ  numberOfIterations useCaching [timing_file]";
    return EXIT_FAILURE;
    }
  
  for (unsigned int i = 0; i < argc; i++)
    {
    std::cout << i << " : " << argv[i] << std::endl;
    }

  const    unsigned int    ImageDimension = 3;
  typedef  float           PixelType;

  typedef itk::Image< PixelType, ImageDimension >  ImageType;


  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform2<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;

  typedef itk::ImageToImageMetric<  ImageType, 
                                    ImageType >    MetricType;

  typedef itk::MattesMutualInformationImageToImageMetric< 
                                    ImageType, 
                                    ImageType >    CachingMetricType;

  typedef itk::MattesNoCachingMutualInformationImageToImageMetric< 
                                    ImageType, 
                                    ImageType >    NoCachingMetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    ImageType,
                                    double >    InterpolatorType;

  MetricType::Pointer  metric;
  CachingMetricType::Pointer   cachingMetric   = CachingMetricType::New();
  NoCachingMetricType::Pointer noCachingMetric = NoCachingMetricType::New();

  TransformType::Pointer  transform = TransformType::New();

  std::string cachingString;

  if( atoi( argv[5] ) )
    {
    std::cout << "Using Caching Metric" << std::endl;
    metric = cachingMetric;
    cachingString = "Caching";
    }
  else
    {
    std::cout << "Using No Caching Metric" << std::endl;
    metric = noCachingMetric;
    cachingString = "NoCaching";
    }

  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  
  ImageType::Pointer image = ImageType::New();
  
  ImageType::RegionType region;
  ImageType::SizeType   size;
  ImageType::IndexType  start;

  start.Fill( 0 );

  size[0] = atoi( argv[1] );
  size[1] = atoi( argv[2] );
  size[2] = atoi( argv[3] );

  region.SetSize( size );
  region.SetIndex( start );

  image->SetRegions( region );
  image->Allocate();


  // Initialize the content of the image.
  typedef itk::ImageRegionIterator< ImageType >  IteratorType;

  IteratorType itr( image, region );

  itr.GoToBegin();

  while( !itr.IsAtEnd() )
    {
    // put the x index value as pixel value
    itr.Set( itr.GetIndex()[0] ); 
    ++itr;
    }

  ImageType::RegionType fixedRegion = image->GetBufferedRegion();
  

  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;
  RegionType::SizeType   gridSizeOnImage;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;

  gridSizeOnImage[0] = 32;
  gridSizeOnImage[1] = 32;
  gridSizeOnImage[2] = 16;

  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  typedef TransformType::SpacingType SpacingType;
  SpacingType spacing = image->GetSpacing();

  typedef TransformType::OriginType OriginType;
  OriginType origin = image->GetOrigin();;

  ImageType::SizeType fixedImageSize = fixedRegion.GetSize();

  for(unsigned int r=0; r<ImageDimension; r++)
    {
    spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  / 
                  static_cast<double>(gridSizeOnImage[r] - 1) );
    origin[r]  -=  spacing[r]; 
    }

 

  typedef TransformType::ParametersType     ParametersType;

  
  const unsigned int totalNumberOfPixels = fixedRegion.GetNumberOfPixels();

  const unsigned int numberOfSamples = 
    static_cast< unsigned int >( totalNumberOfPixels * 0.20 );

  const unsigned int numberOfHistogramBins = 50;
  const unsigned int seed = 76926294;

  cachingMetric->SetNumberOfHistogramBins( numberOfHistogramBins );
  cachingMetric->SetNumberOfSpatialSamples( numberOfSamples );
  cachingMetric->ReinitializeSeed( seed );

  noCachingMetric->SetNumberOfHistogramBins( numberOfHistogramBins );
  noCachingMetric->SetNumberOfSpatialSamples( numberOfSamples );
  noCachingMetric->ReinitializeSeed( seed );

  itk::ExperimentTimeProbesCollector collector;

  std::cout << std::endl << "Starting Benchmark" << std::endl;
  std::cout << "Number of Samples " << numberOfSamples << std::endl;

  TransformType::ParametersType parameters;

  try 
    { 

    transform->SetGridSpacing( spacing );
    transform->SetGridOrigin( origin );
    transform->SetGridRegion( bsplineRegion );
    const unsigned int numberOfParameters = transform->GetNumberOfParameters();
    std::cout << "numberOfParameters " << numberOfParameters << std::endl;
    parameters.SetSize( numberOfParameters );
    parameters.Fill( 0.0 );
    transform->SetParameters( parameters );
    metric->SetTransform( transform );

    metric->SetInterpolator( interpolator );
    metric->SetFixedImage(  image   );
    metric->SetMovingImage(  image );
    metric->SetFixedImageRegion( fixedRegion );


    //collector.Start("Initialize");
    metric->Initialize(); 
    //collector.Stop("Initialize");
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 
  catch( std::bad_alloc )
    {
    std::cerr << "Problem allocating memory" << std::endl;
    return EXIT_FAILURE;
    }
  
  const unsigned int numberOfIterations = atoi( argv[4] );
 
  MetricType::MeasureType     value; 
  MetricType::DerivativeType  derivative;

  for(unsigned int i=0; i<numberOfIterations; i++)
    {
    collector.Start("GetValueAndDerivative");
    std::cout << "Iteration " << i << " of " << numberOfIterations << std::endl;
    metric->GetValueAndDerivative( parameters, value, derivative );
    collector.Stop("GetValueAndDerivative");
    }

    std::cout << "Value :  " << value << std::endl;
    // std::cout << "Derivative :  " << derivative << std::endl;

  //collector.Report( std::cout );
  char numberOfSamplesString[16];
  sprintf(numberOfSamplesString, "%d", numberOfSamples);
  std::string experimentString;
  experimentString = cachingString + "\t" + argv[1] + "\t" + argv[2] + "\t" + argv[3] + "\t" + numberOfSamplesString;  
  collector.SetExperimentString( experimentString );
  std::string outputFileName;
  if ( argc > 6)
    {
    outputFileName = argv[6];
    }
  else
    {
    outputFileName = "MattesMetricCachingTestResult-" + cachingString;
    outputFileName = outputFileName + "-volumesize-";
    outputFileName = outputFileName + argv[1];
    outputFileName = outputFileName + "x" + argv[2] + "x" + argv[3];
    outputFileName = outputFileName + ".txt";
    }
  // Note: open append
  std::ofstream outputFile( outputFileName.c_str(), std::ios_base::app );
  collector.Report( outputFile );

  return EXIT_SUCCESS;
}

