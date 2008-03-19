
#include "itkMultiThreadedMutualInformationImageToImageMetric.h"
#include "itkBSplineDeformableTransform.h"
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
#include "itkMutualInformationImageToImageMetric.h"
#include "MemoryUsage.h"
#include "itkExperimentMemoryProbesCollector.h"

#define USE_MULTITHREADED

int main( int argc, char* argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " imagesizeX imagesizeY imagesizeZ  numberOfIterations useThreading [timing_file] [memory_file]";
    return EXIT_FAILURE;
    }
  
  for (unsigned int i = 0; i < argc; i++)
    {
    std::cout << i << " : " << argv[i] << std::endl;
    }

  const unsigned int                                        ImageDimension = 3;
  typedef float                                             PixelType;
  typedef itk::Image< PixelType, ImageDimension >           ImageType;
  typedef double                                            CoordinateRepType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >   MetricType;

  typedef itk::MultiThreadedMutualInformationImageToImageMetric< ImageType, ImageType > ThreadMetricType;
  typedef itk::MutualInformationImageToImageMetric< ImageType, ImageType >              DefaultMetricType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >                               CommonMetricType;
  
  typedef ThreadMetricType::ParametersType ParametersType;

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;

  typedef itk::BSplineDeformableTransform2<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    ImageType,
                                    double >    InterpolatorType;

  itk::ExperimentMemoryProbesCollector      memoryCollector;
  memoryCollector.Start("MultiThreadMI");

  ThreadMetricType::Pointer  threadMetric = ThreadMetricType::New();
  DefaultMetricType::Pointer defaultMetric = DefaultMetricType::New();
  CommonMetricType::Pointer metric;
  std::string threadingString;
  int useThreading = atoi(argv[5]);

  if ( useThreading )
    {
    threadingString = "Multithreaded";
    metric = threadMetric;
    }
  else
    {
    threadingString = "Default";
    metric = defaultMetric;
    }


  TransformType::Pointer  transform = TransformType::New();

  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  
  ImageType::Pointer image = ImageType::New();
  
  ImageType::RegionType region;
  ImageType::SizeType   size;
  ImageType::IndexType  start;

  start.Fill( 0 );

  size[0] = atoi( argv[1] );
  size[1] = atoi( argv[2] );
  size[2] = atoi( argv[3] );

  const unsigned int numberOfIterations = atoi( argv[4] );

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

  // was 2 2 2
  gridSizeOnImage[0] = 32; //32
  gridSizeOnImage[1] = 32; //32
  gridSizeOnImage[2] = 16; //16

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

  // FIX ME
  const unsigned int numberOfSamples = static_cast< unsigned int >( totalNumberOfPixels * 0.05 );

  const unsigned int numberOfHistogramBins = 50;
  const unsigned int seed = 76926294;

  threadMetric->SetNumberOfSpatialSamples( numberOfSamples );
  threadMetric->ReinitializeSeed( seed );
  defaultMetric->SetNumberOfSpatialSamples( numberOfSamples );
  defaultMetric->ReinitializeSeed( seed );

  itk::ExperimentTimeProbesCollector        timeCollector;

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

    metric->Initialize(); 
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
   
  ThreadMetricType::MeasureType     value; 
  ThreadMetricType::DerivativeType  derivative;

#if 0
  for(unsigned int i=0; i<numberOfIterations; i++)
    {
    std::cout << "Iteration " << i << " of " << numberOfIterations << std::endl;

    timeCollector.Start("GetValue");

    value = metric->GetValue( parameters );

    timeCollector.Stop("GetValue");

    std::cout << threadingString;
    std::cout << " value :  ";
    std::cout.width(20);
    std::cout.precision(17);
    std::cout << value << std::endl;

    }
#endif

  for(unsigned int i=0; i<numberOfIterations; i++)
    {
    std::cout << "Iteration " << i << " of " << numberOfIterations << std::endl;

    timeCollector.Start("GetValueAndDerivative");

    metric->GetValueAndDerivative( parameters, value, derivative );

    timeCollector.Stop("GetValueAndDerivative");

    std::cout << threadingString;
    std::cout << " value :  ";
    std::cout.width(20);
    std::cout.precision(17);
    std::cout << value << std::endl;
    std::cout << "Derivative : ";
    std::cout << derivative << std::endl;
    }

  memoryCollector.Stop("MultiThreadMI");

  char numberOfSamplesString[16];
  sprintf(numberOfSamplesString, "%d", numberOfSamples);
  std::string experimentString;
  experimentString = threadingString + "\t" + argv[1] + "\t" + argv[2] + "\t" + argv[3] + "\t" + numberOfSamplesString;  
  timeCollector.SetExperimentString( experimentString );
  memoryCollector.SetExperimentString( experimentString );
  std::string outputFileName;
  std::string memOutputFileName;
  if ( argc > 7)
    {
    outputFileName = argv[6];
    memOutputFileName = argv[7];
    }
  else if ( argc > 6 )
    {
    outputFileName = argv[6];

    memOutputFileName = "MultiThreadedMIMetricMemoryTestResult-" + threadingString;
    memOutputFileName = memOutputFileName + "-volumesize-";
    memOutputFileName = memOutputFileName + argv[1];
    memOutputFileName = memOutputFileName + "x" + argv[2] + "x" + argv[3];
    memOutputFileName = memOutputFileName + ".txt";
    }
  else
    {
    outputFileName = "MultiThreadedMIMetricTimingTestResult-" + threadingString;
    outputFileName = outputFileName + "-volumesize-";
    outputFileName = outputFileName + argv[1];
    outputFileName = outputFileName + "x" + argv[2] + "x" + argv[3];
    outputFileName = outputFileName + ".txt";

    memOutputFileName = "MultiThreadedMIMetricMemoryTestResult-" + threadingString;
    memOutputFileName = memOutputFileName + "-volumesize-";
    memOutputFileName = memOutputFileName + argv[1];
    memOutputFileName = memOutputFileName + "x" + argv[2] + "x" + argv[3];
    memOutputFileName = memOutputFileName + ".txt";
    }
  // Note: open append so multiple experiments can write to the same results
  // file.
  std::ofstream outputFile( outputFileName.c_str(), std::ios_base::app );
  timeCollector.Report( outputFile );

  timeCollector.Report( std::cout );

  std::cout << "Timing report written to : " << outputFileName << std::endl;

  memoryCollector.Report( std::cout );
  std::ofstream memOutputFile( memOutputFileName.c_str(), std::ios_base::app );
  memoryCollector.Report( memOutputFile );
  std::cout << "Memory report written to : " << memOutputFileName << std::endl;

  return EXIT_SUCCESS;
}
