
#include "MultiThreadMIMetricTestImplementation.h"


#include "itkMultiThreadedMutualInformationImageToImageMetric.h"
#include "itkBSplineDeformableTransform.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkTimeProbesCollectorBase.h"

#include "itkMattesMutualInformationImageToImageMetric.h"

//#include "itkBSplineDeformableTransform2.h"

#include <fstream>
#include <string>
#include <sstream>

#include "itkExperimentTimeProbesCollector.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "MemoryUsage.h"
#include "itkExperimentMemoryProbesCollector.h"

template <unsigned int Dimension>
MultiThreadMIMetricTestImplementation<Dimension>
::MultiThreadMIMetricTestImplementation() : 
    m_OutputFileName("MultiThreadMIMetricTestImplementation.timing.txt" ),
    m_MemOutputFileName("MultiThreadMIMetricTestImplementation.memory.txt" )
{
  m_UseThreading = true;
  m_ImageSize.Fill( 32 );
  m_BSplineSize.Fill( 8 );
  m_NumberOfIterations = 1;
  m_NumberOfSamples = 1; // not reasonable
}

template <unsigned int Dimension>
bool 
MultiThreadMIMetricTestImplementation<Dimension>
::RunTest()
{
  const unsigned int                                        ImageDimension = 3;
  typedef float                                             PixelType;
  typedef itk::Image< PixelType, ImageDimension >           ImageType;
  typedef double                                            CoordinateRepType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >   MetricType;

  typedef itk::MultiThreadedMutualInformationImageToImageMetric< ImageType, ImageType > ThreadMetricType;
  typedef itk::MutualInformationImageToImageMetric< ImageType, ImageType >              DefaultMetricType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >                               CommonMetricType;
  typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType>         MattesMetricType;
 
  typedef ThreadMetricType::ParametersType ParametersType;

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;

  typedef itk::BSplineDeformableTransform<
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
  MattesMetricType::Pointer  mattesMetric = MattesMetricType::New();

  CommonMetricType::Pointer metric;
  std::string threadingString;
  int useThreading = m_UseThreading; //atoi(argv[5]);

  if ( 1 == useThreading )
    {
    threadingString = "Multithreaded";
    metric = threadMetric;
    }
  else if ( 2 == useThreading )
    {
    threadingString = "Mattes";
    metric = mattesMetric;
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

  size[0] = m_ImageSize[0];
  size[1] = m_ImageSize[1];
  size[2] = m_ImageSize[2];

  const unsigned int numberOfIterations = m_NumberOfIterations; 

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
  for (unsigned int i = 0; i < Dimension; i++)
    {
    gridSizeOnImage[i] = m_BSplineSize[i]; //32
    }

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
  //const unsigned int numberOfSamples = static_cast< unsigned int >( totalNumberOfPixels * 0.05 );
  const unsigned int numberOfSamples = m_NumberOfSamples;

  const unsigned int numberOfHistogramBins = 50;
  const unsigned int seed = 76926294;

  threadMetric->SetNumberOfSpatialSamples( numberOfSamples );
  threadMetric->ReinitializeSeed( seed );
  defaultMetric->SetNumberOfSpatialSamples( numberOfSamples );
  defaultMetric->ReinitializeSeed( seed );
  mattesMetric->SetNumberOfSpatialSamples( numberOfSamples );
  mattesMetric->SetNumberOfHistogramBins( 50 );

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
    return false;
    } 
  catch( std::bad_alloc )
    {
    std::cerr << "Problem allocating memory" << std::endl;
    return false;
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

  char derivIterFileName[256];

  for(unsigned int i=0; i<numberOfIterations; i++)
    {
    std::cout << "Iteration " << i << " of " << numberOfIterations << std::endl;
    
    threadMetric->ReinitializeSeed( seed );
    defaultMetric->ReinitializeSeed( seed );

    timeCollector.Start("GetValueAndDerivative");

    metric->GetValueAndDerivative( parameters, value, derivative );

    timeCollector.Stop("GetValueAndDerivative");

    std::cout << threadingString;
    std::cout << " value :  ";
    std::cout.width(20);
    std::cout.precision(17);
    std::cout << value << std::endl;
    // std::cout << "Derivative : ";
    // std::cout << derivative << std::endl;

    sprintf(derivIterFileName, "deriv.%d.out.txt", i);
    std::ofstream derivativeOutputFile2( derivIterFileName, std::ios_base::app );
    derivativeOutputFile2 << derivative << std::endl;
    std::cout << "Derivative written to : " << derivIterFileName << std::endl;

    }

  memoryCollector.Stop("MultiThreadMI");

  //char numberOfSamplesString[16];
  //sprintf(numberOfSamplesString, "%d", numberOfSamples);
  // std::stringstr
  //std::string experimentString;
  //experimentString = threadingString + "\t" + m_ImageSize[0] + "\t" + m_ImageSize[1] + "\t" + m_ImageSize[2] + "\t" + numberOfSamplesString;  

  // FIXME - Assumes 3D input image.
  std::stringstream experimentStringStream;
  experimentStringStream << threadingString << "\t" << m_ImageSize[0] << "\t" << m_ImageSize[1] << "\t" << m_ImageSize[2];
  experimentStringStream << "\t" << m_BSplineSize[0] << "\t" << m_BSplineSize[1] << "\t" << m_BSplineSize[2];
  experimentStringStream << "\t" << numberOfSamples << "\t" << m_BSplineSize[0] * m_BSplineSize[1] * m_BSplineSize[2];

  timeCollector.SetExperimentString( experimentStringStream.str() );
  memoryCollector.SetExperimentString( experimentStringStream.str() );
  std::string outputFileName;
  std::string memOutputFileName;

  /*
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
  */
  // Note: open append so multiple experiments can write to the same results
  // file.
  std::ofstream outputFile( m_OutputFileName.c_str(), std::ios_base::app );
  timeCollector.Report( outputFile );

  timeCollector.Report( std::cout );

  std::cout << "Timing report written to : " << m_OutputFileName << std::endl;

  memoryCollector.Report( std::cout );
  std::ofstream memOutputFile( m_MemOutputFileName.c_str(), std::ios_base::app );
  memoryCollector.Report( memOutputFile );
  std::cout << "Memory report written to : " << m_MemOutputFileName << std::endl;

  std::ofstream derivativeOutputFile( m_DerivativeOutputFileName.c_str(), std::ios_base::app );
  derivativeOutputFile << derivative << std::endl;
  std::cout << "Derivative written to : " << m_DerivativeOutputFileName << std::endl;

  return true;
}

template <unsigned int Dimension>
void
MultiThreadMIMetricTestImplementation<Dimension>
::SetNumberOfIterations( unsigned int iter )
{
  m_NumberOfIterations = iter;
}

template <unsigned int Dimension>
unsigned int
MultiThreadMIMetricTestImplementation<Dimension>
::GetNumberOfIterations()
{
  return m_NumberOfIterations;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetImageSize( SizeType& size )
{
  m_ImageSize = size;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetBSplineGridOnImageSize( SizeType& size )
{
  m_BSplineSize = size;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetNumberOfSamples( unsigned long samples )
{
  m_NumberOfSamples = samples;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetUseThreading( int threading )
{
  m_UseThreading = threading;
}


template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::CreateTimingOutputFileName()
{
  std::stringstream output;
  output << "MultiThreadedMIMetricTimingTestResult-" << (m_UseThreading ? "Threaded" : "Default");

  output << "-volumesize-";
  for (unsigned int i = 0; i < Dimension - 1; i++)
    {
    output << m_ImageSize[i] << "x";
    }
  output << m_ImageSize[Dimension-1];

  output << "-bsplinesize-";
  for (unsigned int i = 0; i < Dimension - 1; i++)
    {
    output << m_BSplineSize[i] << "x";
    }
  output << m_BSplineSize[Dimension-1];

  output << ".txt";

  m_OutputFileName = output.str();
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::CreateMemoryOutputFileName()
{
  std::stringstream output;
  output << "MultiThreadedMIMetricMemoryTestResult-" << (m_UseThreading ? "Threaded" : "Default");

  output << "-volumesize-";
  for (unsigned int i = 0; i < Dimension - 1; i++)
    {
    output << m_ImageSize[i] << "x";
    }
  output << m_ImageSize[Dimension-1];

  output << "-bsplinesize-";
  for (unsigned int i = 0; i < Dimension - 1; i++)
    {
    output << m_BSplineSize[i] << "x";
    }
  output << m_BSplineSize[Dimension-1];

  output << ".txt";

  m_MemOutputFileName = output.str();
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetTimingOutputFileName( const char* in )
{
  m_OutputFileName = in;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetMemoryOutputFileName( const char* in )
{
  m_MemOutputFileName = in;
}

template <unsigned int Dimension>
void 
MultiThreadMIMetricTestImplementation<Dimension>
::SetDerivativeOutputFileName( const char* in)
{
  m_DerivativeOutputFileName = in;
}


template <unsigned int Dimension>
bool 
MultiThreadMIMetricTestImplementation<Dimension>
::TestNumericsOfValueAndDerivative( const double tol ) 
{
  const unsigned int                                        ImageDimension = 3;
  typedef float                                             PixelType;
  typedef itk::Image< PixelType, ImageDimension >           ImageType;
  typedef double                                            CoordinateRepType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >   MetricType;

  typedef itk::MultiThreadedMutualInformationImageToImageMetric< ImageType, ImageType > ThreadMetricType;
  typedef itk::MutualInformationImageToImageMetric< ImageType, ImageType >              DefaultMetricType;
  typedef itk::ImageToImageMetric< ImageType, ImageType >                               CommonMetricType;
  typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType>         MattesMetricType;
 
  typedef ThreadMetricType::ParametersType ParametersType;

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;

  typedef itk::BSplineDeformableTransform<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    ImageType,
                                    double >    InterpolatorType;

  ThreadMetricType::Pointer  threadMetric = ThreadMetricType::New();
  DefaultMetricType::Pointer defaultMetric = DefaultMetricType::New();
  MattesMetricType::Pointer  mattesMetric = MattesMetricType::New();

  CommonMetricType::Pointer metric;
  std::string threadingString;
  int useThreading = m_UseThreading; //atoi(argv[5]);

  if ( 1 == useThreading )
    {
    threadingString = "Multithreaded";
    metric = threadMetric;
    }
  else if ( 2 == useThreading )
    {
    threadingString = "Mattes";
    metric = mattesMetric;
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

  size[0] = m_ImageSize[0];
  size[1] = m_ImageSize[1];
  size[2] = m_ImageSize[2];

  const unsigned int numberOfIterations = m_NumberOfIterations; 

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
  for (unsigned int i = 0; i < Dimension; i++)
    {
    gridSizeOnImage[i] = m_BSplineSize[i]; //32
    }

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
  //const unsigned int numberOfSamples = static_cast< unsigned int >( totalNumberOfPixels * 0.05 );
  const unsigned int numberOfSamples = m_NumberOfSamples;

  const unsigned int numberOfHistogramBins = 50;
  const unsigned int seed = 76926294;

  threadMetric->SetNumberOfSpatialSamples( numberOfSamples );
  threadMetric->ReinitializeSeed( seed );
  defaultMetric->SetNumberOfSpatialSamples( numberOfSamples );
  defaultMetric->ReinitializeSeed( seed );
  mattesMetric->SetNumberOfSpatialSamples( numberOfSamples );
  mattesMetric->SetNumberOfHistogramBins( 50 );

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

    defaultMetric->SetTransform( transform );
    defaultMetric->SetInterpolator( interpolator );
    defaultMetric->SetFixedImage(  image   );
    defaultMetric->SetMovingImage(  image );
    defaultMetric->SetFixedImageRegion( fixedRegion );
    defaultMetric->Initialize(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return false;
    } 
  catch( std::bad_alloc )
    {
    std::cerr << "Problem allocating memory" << std::endl;
    return false;
    }
   
  ThreadMetricType::MeasureType     value; 
  ThreadMetricType::DerivativeType  derivative;
  ThreadMetricType::MeasureType     defaultValue; 
  ThreadMetricType::DerivativeType  defaultDerivative;

  char derivIterFileName[256];
  bool testPassed = true;
  std::vector<unsigned int> derivativeDifferenceVector;

  for(unsigned int i=0; i<numberOfIterations; i++)
    {
    std::cout << "Iteration " << i << " of " << numberOfIterations << std::endl;
    
    threadMetric->ReinitializeSeed( seed );
    defaultMetric->ReinitializeSeed( seed );
    defaultMetric->GetValueAndDerivative( parameters, defaultValue, defaultDerivative );

    threadMetric->ReinitializeSeed( seed );
    defaultMetric->ReinitializeSeed( seed );
    metric->GetValueAndDerivative( parameters, value, derivative );

    if ( fabs( value - defaultValue ) > tol )
      {
      std::cerr << "Test value difference from default value is greater than tolerance!" << std::endl;
      std::cerr << "\t Test value : " << value << std::endl;
      std::cerr << "\t Default value : " << defaultValue << std::endl;
      std::cerr << "\t Tol : " << tol << std::endl;
      testPassed = false;
      }

    derivativeDifferenceVector.clear();
    double maxDerivativeDifference = 0.0;
    double mse = 0.0;
    double mae = 0.0;

    for ( unsigned int i = 0; i < parameters.size(); ++i )
      {
      double diff = defaultDerivative[i] - derivative[i];
      double adiff = fabs( diff );
      if ( adiff > tol )
        {
        derivativeDifferenceVector.push_back( i );
        }
      if ( adiff > maxDerivativeDifference )
        {
        maxDerivativeDifference = adiff;
        }
      mse += diff * diff;
      mae += adiff; 
      }
  
    mse /= static_cast<double>( derivative.size() );
    mae /= static_cast<double>( derivative.size() );

    if ( derivativeDifferenceVector.size() > 0 )
      {
      testPassed = false;
      std::cerr << "Test derivative difference from default derivative is greater than tolerance!" << std::endl;
      std::cerr << "\t Failed at " << derivativeDifferenceVector.size();
      std::cerr << " of " << derivative.size() << " components." << std::endl;
      std::cerr << "\t Maximum component error : " << maxDerivativeDifference << std::endl;
      std::cerr << "\t Mean squared error : " << mse << std::endl;
      std::cerr << "\t Mean absolute error : " << mae << std::endl;
      std::cerr << "\t Tol : " << tol << std::endl;
      }

/*
    sprintf(derivIterFileName, "deriv.%d.out.txt", i);
    std::ofstream derivativeOutputFile2( derivIterFileName, std::ios_base::app );
    derivativeOutputFile2 << derivative << std::endl;
    std::cout << "Derivative written to : " << derivIterFileName << std::endl;
*/
    }

/*
  std::ofstream derivativeOutputFile( m_DerivativeOutputFileName.c_str(), std::ios_base::app );
  derivativeOutputFile << derivative << std::endl;
  std::cout << "Derivative written to : " << m_DerivativeOutputFileName << std::endl;
*/

  return testPassed;
}
