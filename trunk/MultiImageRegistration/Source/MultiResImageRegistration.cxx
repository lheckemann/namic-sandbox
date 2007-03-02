/*=========================================================================

  Program:   Group-wise registration
  Module:    MultiResImageRegistration.cxx
  Language:  C++
  Date:      
  Version:   

  We implement a multiresolution based approach to the group-wise registration problem.
  We first register the input images using Affine Transform. Then the output of the
  affine transform is supplied to the B-spline transform. We successively increase the
  resolution of the Bspline grid. All the transforms are implemented in a multiresolution
  fashion.
    
=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

//    INPUTS:  A series of images, either 2D or 3D
//    OUTPUTS: registered version of each input image
//    OUTPUTS: The arithmetic mean of the input images and the mean of the registered images

// This code will use a entropy based metric that is implemented in a multithreaded way.
// The metric uses stoachastic subsampling. The sampled pixels(voxels) are stored in a
// std::vector. Each element in the vector contains pixels(voxels) that correspond to
// each other along the images. The metric then computes the summation of the entropies
// of each element in the vector.

// Headers for the registration method and the metric
#include "MultiResolutionImageRegistrationMethod.h"
#include "VarianceMultiImageMetric.h"
#include "ParzenWindowEntropyMultiImageMetric.h"

    
    
#include "AddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkImage.h"
#include "itkNormalizeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"

// Header to collect the time to register the images
#include "itkTimeProbesCollectorBase.h"

#include "itkCommand.h"

#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
    
//Bspline optimizer and transform
#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSBOptimizer.h"
#include "itkFRPROptimizer.h"
#include "itkGradientDescentOptimizer.h"
#include "GradientDescentLineSearchOptimizer.h"


//BSpline related headers
#include "itkBSplineResampleImageFunction.h"
#include "itkIdentityTransform.h"
#include "itkBSplineDecompositionImageFilter.h"


//DICOM related headers
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

//System Related headers
#include <itksys/SystemTools.hxx>

#include "itkImageRegionIterator.h"

//Define the global types for image type
#define PixelType unsigned short
#define InternalPixelType double
#define Dimension 3

//  The following section of code implements an observer
//  that will monitor the evolution of the registration process.

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate(): m_CumulativeIterationIndex(0) {};
public:
  //typedef   itk::GradientDescentOptimizer     OptimizerType;
  typedef   itk::FRPROptimizer                  OptimizerType;
  typedef   const OptimizerType   *           OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
        return;
      }

      std::cout << std::setiosflags(ios::fixed) << std::showpoint << std::setfill('0');
      std::cout << std::setw(3) << m_CumulativeIterationIndex++ << "   ";
      std::cout << std::setw(3) << optimizer->GetCurrentIteration() << "   ";
      std::cout << std::setw(6) << optimizer->GetValue() << "   ";
      for(int i=0; i<5; i++)
      {
        std::cout << std::setw(6) << optimizer->GetCurrentPosition()[i] << "   ";
      }
      std::cout << std::endl;// std::setw(6) << optimizer->GetCurrentPosition()[12] <<std::endl;
      
    }
private:
  unsigned int m_CumulativeIterationIndex;
};


//  The following section of code implements a Command observer
//  that will control the modification of optimizer parameters
//  at every change of resolution level.
//
template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command 
{
public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command                   Superclass;
  typedef  itk::SmartPointer<Self>        Pointer;
  itkNewMacro( Self );

  //Set-Get methods to change between the scales
  itkSetMacro(MultiScaleSamplePercentageIncrease, double);
  itkSetMacro(MultiScaleMaximumIterationIncrease, double);
  itkSetMacro(MultiScaleStepLengthIncrease, double);


public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;
  
  typedef   itk::SingleValuedNonLinearOptimizer   OptimizerType;
  typedef   OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer       GradientOptimizerType;
  typedef   GradientOptimizerType *             GradientOptimizerPointer;
  typedef   itk::FRPROptimizer                  FRPROptimizerType;
  typedef   FRPROptimizerType *                 FRPROptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;
  typedef   LineSearchOptimizerType  *          LineSearchOptimizerPointer;

  typedef   itk::Image< InternalPixelType, Dimension >   InternalImageType;
  typedef   itk::MultiImageMetric< InternalImageType>    MetricType;
  typedef   MetricType *                                 MetricPointer;
  typedef   itk::VarianceMultiImageMetric< InternalImageType>    VarianceMetricType;
  typedef   VarianceMetricType *                                 VarianceMetricPointer;
  typedef   itk::ParzenWindowEntropyMultiImageMetric< InternalImageType>    EntropyMetricType;
  typedef   EntropyMetricType *                                             EntropyMetricPointer;

  void Execute(itk::Object * object, const itk::EventObject & event)
  {
    if( !(itk::IterationEvent().CheckEvent( &event )) )
    {
      return;
    }
    RegistrationPointer registration =  dynamic_cast<RegistrationPointer>( object );
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( 
                       registration->GetOptimizer() );
    MetricPointer       metric = dynamic_cast< MetricPointer>
                                         (registration->GetMetric());

    if ( registration->GetCurrentLevel() == 0 )
    {
      // Set the number of spatial samples according to the current level
      if(  !strcmp(metric->GetNameOfClass(), "VarianceMultiImageMetric" ) )
      {
        VarianceMetricPointer  varianceMetric = dynamic_cast< VarianceMetricPointer>
                                                              (registration->GetMetric());
        varianceMetric->SetNumberOfSpatialSamples((unsigned int) (varianceMetric->GetNumberOfSpatialSamples() /
                        pow( pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease, (double) (registration->GetNumberOfLevels() - 1.0) ) ) );
      }
      else if(!strcmp(metric->GetNameOfClass(), "ParzenWindowEntropyMultiImageMetric") )
      {
        EntropyMetricPointer  entropyMetric = dynamic_cast< EntropyMetricPointer>
                                                            (registration->GetMetric());
        entropyMetric->SetNumberOfSpatialSamples((unsigned int) (entropyMetric->GetNumberOfSpatialSamples() /
                                  pow( pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease, (double) (registration->GetNumberOfLevels() - 1.0) ) ) );
      }
      
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer = dynamic_cast< GradientOptimizerPointer >(
            registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations( (int)( gradientPointer->GetNumberOfIterations()*pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        gradientPointer->SetLearningRate( gradientPointer->GetLearningRate()*pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) )  );
      }
      else if(!strcmp(optimizer->GetNameOfClass(), "FRPROptimizer") )
      {
        FRPROptimizerPointer FRPRPointer = dynamic_cast< FRPROptimizerPointer >(
            registration->GetOptimizer() );
        FRPRPointer->SetMaximumIteration( (int)(FRPRPointer->GetMaximumIteration()*pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        FRPRPointer->SetStepLength( FRPRPointer->GetStepLength()*pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ));
      }
      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer = dynamic_cast< LineSearchOptimizerPointer >(
            registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration((int)(lineSearchOptimizerPointer->GetMaximumIteration()*pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        lineSearchOptimizerPointer->SetStepLength(lineSearchOptimizerPointer->GetStepLength()*pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) );
      }

      
    }
    else
    {
      // Set the number of spatial samples according to the current level
      if(  !strcmp(metric->GetNameOfClass(), "VarianceMultiImageMetric" ) )
      {
        VarianceMetricPointer  varianceMetric = dynamic_cast< VarianceMetricPointer>
            (registration->GetMetric());
        varianceMetric->SetNumberOfSpatialSamples((unsigned int) (varianceMetric->GetNumberOfSpatialSamples() *
                                                        pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease ) );
      }
      else if(!strcmp(metric->GetNameOfClass(), "ParzenWindowEntropyMultiImageMetric") )
      {
        EntropyMetricPointer  entropyMetric = dynamic_cast< EntropyMetricPointer>
            (registration->GetMetric());
        entropyMetric->SetNumberOfSpatialSamples((unsigned int) (entropyMetric->GetNumberOfSpatialSamples() *
                                                        pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease ) );
      }

      // Decrease the learning rate at each increasing multiresolution level
      // Increase the number of steps
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer = dynamic_cast< GradientOptimizerPointer >(
                                                                 registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations((int) (gradientPointer->GetNumberOfIterations()/m_MultiScaleMaximumIterationIncrease ));
        gradientPointer->SetLearningRate( gradientPointer->GetLearningRate()/m_MultiScaleStepLengthIncrease  );
      }
      else if(!strcmp(optimizer->GetNameOfClass(), "FRPROptimizer") )
      {
        FRPROptimizerPointer FRPRPointer = dynamic_cast< FRPROptimizerPointer >(
                                                         registration->GetOptimizer() );
        FRPRPointer->SetMaximumIteration( (int)(FRPRPointer->GetMaximumIteration()/m_MultiScaleMaximumIterationIncrease ));
        FRPRPointer->SetStepLength( FRPRPointer->GetStepLength()/m_MultiScaleStepLengthIncrease);
      }
      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer = dynamic_cast< LineSearchOptimizerPointer >(
            registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration((int)(lineSearchOptimizerPointer->GetMaximumIteration()/m_MultiScaleMaximumIterationIncrease ));
        lineSearchOptimizerPointer->SetStepLength(lineSearchOptimizerPointer->GetStepLength()/m_MultiScaleStepLengthIncrease );
      }

    }
  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
  
  protected:
    //Constructor initialize the variables
    RegistrationInterfaceCommand()
    {
      m_MultiScaleSamplePercentageIncrease = 1;
      m_MultiScaleMaximumIterationIncrease = 1;
      m_MultiScaleStepLengthIncrease = 1;
    };
  private:
    double m_MultiScaleSamplePercentageIncrease;
    double m_MultiScaleMaximumIterationIncrease;
    double m_MultiScaleStepLengthIncrease;
    

};

// Get the command line arguments
int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType, int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh, double& optTranslationLearningRate, double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate, int& optTranslationNumberOfIterations, int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,  int& bsplineInitialGridSize,  int& numberOfBsplineLevel, string& transformType, string& imageType,string& metricType, string& useBspline, string& useBsplineHigh,
                   double& translationMultiScaleSamplePercentageIncrease, double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,
                   double& translationMultiScaleMaximumIterationIncrease, double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,
                   double& translationMultiScaleStepLengthIncrease, double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease  );


int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder("");
  string outputFolder("");
  
  string optimizerType("gradient");
  string transformType("");
  string metricType("entropy");

  string metricPrint("off");
  
  int multiLevelAffine = 1;
  int multiLevelBspline = 1;
  int multiLevelBsplineHigh = 1;

  double optTranslationLearningRate = 1000;
  double optAffineLearningRate = 2e-3;
  double optBsplineLearningRate = 500;
  double optBsplineHighLearningRate = 500;

  int optTranslationNumberOfIterations = 500;
  int optAffineNumberOfIterations = 10000;
  int optBsplineNumberOfIterations = 10000;
  int optBsplineHighNumberOfIterations = 10000;
  
  double numberOfSpatialSamplesAffinePercentage = 0.01;
  double numberOfSpatialSamplesBsplinePercentage = 0.01;
  double numberOfSpatialSamplesBsplineHighPercentage = 0.01;

  double translationMultiScaleSamplePercentageIncrease = 4.0;
  double affineMultiScaleSamplePercentageIncrease = 4.0;
  double bsplineMultiScaleSamplePercentageIncrease = 4.0;
  
  double translationMultiScaleMaximumIterationIncrease = 1.0;
  double affineMultiScaleMaximumIterationIncrease = 1.0;
  double bsplineMultiScaleMaximumIterationIncrease = 1.0;

  
  double translationMultiScaleStepLengthIncrease = 1.0;
  double affineMultiScaleStepLengthIncrease = 1.0;
  double bsplineMultiScaleStepLengthIncrease = 1.0;

  
  int bsplineInitialGridSize = 5;
  int numberOfBsplineLevel = 1;
  string imageType = "normal";

  string useBspline("off");
  string useBsplineHigh("off");

  //Get the command line arguments
  if( getCommandLine(argc,argv, fileNames, inputFolder, outputFolder, optimizerType, multiLevelAffine, multiLevelBspline, multiLevelBsplineHigh, optTranslationLearningRate, optAffineLearningRate,  optBsplineLearningRate, optBsplineHighLearningRate, optTranslationNumberOfIterations, optAffineNumberOfIterations, optBsplineNumberOfIterations, optBsplineHighNumberOfIterations, numberOfSpatialSamplesAffinePercentage, numberOfSpatialSamplesBsplinePercentage, numberOfSpatialSamplesBsplineHighPercentage, bsplineInitialGridSize, numberOfBsplineLevel, transformType, imageType, metricType, useBspline, useBsplineHigh, translationMultiScaleSamplePercentageIncrease, affineMultiScaleSamplePercentageIncrease, bsplineMultiScaleSamplePercentageIncrease, translationMultiScaleMaximumIterationIncrease, affineMultiScaleMaximumIterationIncrease,  bsplineMultiScaleMaximumIterationIncrease, translationMultiScaleStepLengthIncrease, affineMultiScaleStepLengthIncrease, bsplineMultiScaleStepLengthIncrease   ) )
    return 1;
  

  // Input Image type typedef
  // const    unsigned int    Dimension = 3;
  // typedef  unsigned short  PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;

  //Internal Image Type typedef
  //typedef double InternalPixelType;
  typedef double ScalarType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;



  typedef itk::TranslationTransform< ScalarType, Dimension > TranslationTransformType;
  typedef itk::AffineTransform< ScalarType, Dimension > TransformType;


  typedef itk::GradientDescentOptimizer       OptimizerType;
  typedef itk::FRPROptimizer                  FRPROptimizerType;
  typedef itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;

  typedef itk::LinearInterpolateImageFunction<InternalImageType,ScalarType        > InterpolatorType;
                                    
  typedef itk::VarianceMultiImageMetric< InternalImageType>    MetricType;
  typedef itk::ParzenWindowEntropyMultiImageMetric< InternalImageType>    EntropyMetricType;


  typedef OptimizerType::ScalesType       OptimizerScalesType;

  typedef itk::MultiResolutionMultiImageRegistrationMethod< InternalImageType >    RegistrationType;

  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType  >    ImagePyramidType;


  RegistrationType::Pointer   registration  = RegistrationType::New();


  // N is the number of images in the registration
  int N = fileNames.size();
  
  //generate filenames
  vector<string> inputFileNames(N);
  vector<string> outputFileNames(N);
  for(int i=0; i<N; i++)
  {
    //write output file names
    outputFileNames[i] = outputFolder + fileNames[i];

    // write input file names
    inputFileNames[i] = inputFolder + fileNames[i];
  }


  registration->SetNumberOfImages(N);
  registration->ReleaseDataFlagOn();

  //Set the optimizerType
  OptimizerType::Pointer      optimizer;
  FRPROptimizerType::Pointer  FRPRoptimizer;
  LineSearchOptimizerType::Pointer lineSearchOptimizer;
  if(optimizerType == "FRPR")
  {
    FRPRoptimizer = FRPROptimizerType::New();
    registration->SetOptimizer(     FRPRoptimizer     );
  }
  else if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer     = LineSearchOptimizerType::New();
    registration->SetOptimizer(     lineSearchOptimizer     );
  } 
  else
  {
    optimizer     = OptimizerType::New();
    registration->SetOptimizer(     optimizer     );
  }
  
  //typedefs for translation transform array
  typedef vector<TranslationTransformType::Pointer> TranslationTransformArrayType;
  TranslationTransformArrayType      translationTransformArray(N);

  //typedefs for affine transform array
  typedef vector<TransformType::Pointer> TransformArrayType;
  TransformArrayType      affineTransformArray(N);

  //typedefs for intorpolater array
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray(N);

  // typedefs for image file readers
  typedef itk::ImageFileReader< ImageType  > ImageReaderType;
  typedef vector< ImageReaderType::Pointer > ImageArrayReader;

  //Type definitions to be used in reading DICOM images
  typedef itk::ImageSeriesReader< ImageType >     DICOMReaderType;
  typedef vector< DICOMReaderType::Pointer > DICOMReaderArray;
  DICOMReaderArray dicomArrayReader(N);
  typedef itk::GDCMImageIO                        ImageIOType;
  typedef vector<ImageIOType::Pointer>                     ImageIOTypeArray;
  ImageIOTypeArray imageIOTypeArray(N);
  typedef itk::GDCMSeriesFileNames                NamesGeneratorType;
  typedef vector<NamesGeneratorType::Pointer>              NamesGeneratorTypeArray;
  NamesGeneratorTypeArray namesGeneratorArray(N);


  typedef  vector<ImagePyramidType::Pointer>                ImagePyramidArray;
  ImagePyramidArray imagePyramidArray(N);


  // typedef for normalized image filters
  // the mean and the variance of the images normalized before registering
  typedef itk::NormalizeImageFilter< ImageType, InternalImageType > NormalizeFilterType;
  typedef NormalizeFilterType::Pointer NormalizeFilterTypePointer;
  typedef vector<NormalizeFilterTypePointer> NormalizedFilterArrayType;


  // typedefs for Gaussian filters
  // The normalized images are passed through a Gaussian filter for smoothing
  typedef itk::DiscreteGaussianImageFilter<
                                      InternalImageType, 
                                      InternalImageType
                                                    > GaussianFilterType;
  typedef vector< GaussianFilterType::Pointer > GaussianFilterArrayType;

  // Begin the registration with the affine transform
  // Connect the compenents together
  //
  // for all input images
  // create a separate tranform, interpolater, image reader,
  // normalized filter, gaussian filter and connect those components
  //
  try
  {
  
    for( int i=0; i< N; i++ )
    {
      ImageReaderType::Pointer imageReader;
      translationTransformArray[i] = TranslationTransformType::New();
      interpolatorArray[i]  = InterpolatorType::New();
      registration->SetTransformArray(     translationTransformArray[i] ,i    );
      registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );

      if(imageType == "DICOM")
      {
        imageIOTypeArray[i] = ImageIOType::New();
        namesGeneratorArray[i] = NamesGeneratorType::New();
        namesGeneratorArray[i]->SetInputDirectory( inputFileNames[i].c_str() );

        std::cout << "Reading first slice " << namesGeneratorArray[i]->GetInputFileNames()[0] << std:: endl;
        std::cout << "Reading " << namesGeneratorArray[i]->GetInputFileNames().size() << " DICOM slices" << std::endl;

        dicomArrayReader[i] = DICOMReaderType::New();
        dicomArrayReader[i]->SetImageIO( imageIOTypeArray[i] );
        dicomArrayReader[i]->SetFileNames( namesGeneratorArray[i]->GetInputFileNames() );
        
      }
      else
      {
        imageReader = ImageReaderType::New();
        //imageReader->ReleaseDataFlagOn();
        imageReader->SetFileName( inputFileNames[i].c_str() );
      }
        
    
      NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
      normalizeFilter->ReleaseDataFlagOn();
      if( imageType == "DICOM")
      {
        normalizeFilter->SetInput( dicomArrayReader[i]->GetOutput() );
      }
      else
      {
        normalizeFilter->SetInput( imageReader->GetOutput() );
      }

      GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
      gaussianFilter->ReleaseDataFlagOn();
      gaussianFilter->SetVariance( 2.0 );
      gaussianFilter->SetInput( normalizeFilter->GetOutput() );

      //Set up the Image Pyramid
      imagePyramidArray[i] = ImagePyramidType::New();
      imagePyramidArray[i]->ReleaseDataFlagOn();
      imagePyramidArray[i]->SetNumberOfLevels( multiLevelAffine );
      imagePyramidArray[i]->SetInput( gaussianFilter->GetOutput() );

      std::cout << "Reading Image: " << inputFileNames[i].c_str() << std::endl;
      imagePyramidArray[i]->Update();

      //Set the input into the registration method
      registration->SetImagePyramidArray(imagePyramidArray[i],i);

    }
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }


  // Set initial parameters of the transform
  ImageType::RegionType fixedImageRegion =
      imagePyramidArray[0]->GetOutput(0)->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );


  // Allocate the space for tranform parameters used by registration method
  // We use a large array to concatenate the parameter array of each tranform
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( translationTransformArray[0]->GetNumberOfParameters()*N );
  initialParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialParameters );


  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( translationTransformArray[0]->GetNumberOfParameters()*N );
  optimizerScales.Fill(1.0);


  // Get the number of pixels (voxels) in the images
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  const unsigned int numberOfSamples =
      static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesAffinePercentage );

  //Set the metric type
  MetricType::Pointer         varianceMetric;
  EntropyMetricType::Pointer         entropyMetric;
  if(metricType == "variance")
  {
    varianceMetric        = MetricType::New();
    registration->SetMetric( varianceMetric  );
    // Set the number of samples to be used by the metric
    varianceMetric->SetNumberOfSpatialSamples( numberOfSamples );
  }
  else
  {
    entropyMetric        = EntropyMetricType::New();
    registration->SetMetric( entropyMetric  );
    // Set the number of samples to be used by the metric
    entropyMetric->SetNumberOfSpatialSamples( numberOfSamples );
  }


  // Set the optimizer parameters
  if(optimizerType == "FRPR")
  {
    FRPRoptimizer->SetStepLength(optTranslationLearningRate);
    FRPRoptimizer->SetMaximize(false);
    FRPRoptimizer->SetMaximumIteration( optTranslationNumberOfIterations );
    FRPRoptimizer->SetMaximumLineIteration( 10 );
    FRPRoptimizer->SetScales( optimizerScales );
    FRPRoptimizer->SetToPolakRibiere();
    //FRPRoptimizer->SetToFletchReeves();
  }
  else if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optTranslationLearningRate);
    lineSearchOptimizer->SetMaximize(false);
    lineSearchOptimizer->SetMaximumIteration( optTranslationNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( 10 );
    lineSearchOptimizer->SetScales( optimizerScales );
  } 
  else
  {
    optimizer->SetLearningRate( optTranslationLearningRate );
    optimizer->SetNumberOfIterations( optTranslationNumberOfIterations );
    optimizer->MaximizeOff();
    optimizer->SetScales( optimizerScales );
    // Create the Command observer and register it with the optimizer.
    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    //optimizer->AddObserver( itk::IterationEvent(), observer );
  }



  // Create the Command interface observer and register it with the optimizer.
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  //Set the parameters of the command observer
  command->SetMultiScaleSamplePercentageIncrease(translationMultiScaleSamplePercentageIncrease);
  command->SetMultiScaleMaximumIterationIncrease(translationMultiScaleMaximumIterationIncrease);
  command->SetMultiScaleStepLengthIncrease(translationMultiScaleStepLengthIncrease);
  
  registration->AddObserver( itk::IterationEvent(), command );

  // Set the number of multiresolution levels
  registration->SetNumberOfLevels( multiLevelAffine );

  std::cout << "Starting Registration with Translation Transform " << std::endl;

  // Add probe to count the time used by the registration
  itk::TimeProbesCollectorBase collector;


  // Start registration with translation transform
  try 
  {
    collector.Start( "Registration" );
    registration->StartRegistration();
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return -1;
  }


  
  // Continue with affine transform using the results of the translation transform
  //
  //Get the latest parameters from the registration
  ParametersType translationParameters = registration->GetLastTransformParameters();
  
  std::cout << "Starting Registration with Affine Transform " << std::endl;
  for(int i=0; i<N; i++)
  {
    affineTransformArray[i]     = TransformType::New();
    registration->SetTransformArray(     affineTransformArray[i] ,i    );
  }

  // Allocate the space for tranform parameters used by registration method
  // We use a large array to concatenate the parameter array of each tranform
  ParametersType initialAffineParameters( affineTransformArray[0]->GetNumberOfParameters()*N );
  initialAffineParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialAffineParameters );
  
  int numberOfParameters = affineTransformArray[0]->GetNumberOfParameters();

  //Initialize the affine transforms to identity transform
  // And use the results of the translation transform
  for(int i=0; i<N; i++)
  {
    affineTransformArray[i]->SetIdentity();
    TransformType::InputPointType center;
    // Get spacing, origin and size of the images
    ImageType::SpacingType spacing = imagePyramidArray[i]->GetOutput(0)->GetSpacing();
    itk::Point<double, Dimension> origin = imagePyramidArray[i]->GetOutput(0)->GetOrigin();
    ImageType::SizeType size = imagePyramidArray[i]->GetOutput(0)->GetLargestPossibleRegion().GetSize();

    // Place the center of rotation to the center of the image
    for(int j=0; j< Dimension; j++)
    {
      center[j] = origin[j] + spacing[j]*size[j] / 2.0;
    }
    affineTransformArray[i]->SetCenter(center);

    //Initialize the translation parameters using the results of the tranlation transform
    ParametersType affineParameters = affineTransformArray[i]->GetParameters();
    for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
    {
      affineParameters[j] = translationParameters[i*Dimension + j-Dimension*Dimension]; // scale for translation on X,Y,Z
    }
    affineTransformArray[i]->SetParametersByValue(affineParameters);

    registration->SetInitialTransformParameters( affineTransformArray[i]->GetParameters(),i );
  }

  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  OptimizerScalesType optimizerAffineScales( affineTransformArray[0]->GetNumberOfParameters()*N );
  optimizerAffineScales.Fill(1.0);
  for( int i=0; i<N; i++)
  {
    for( int j=0; j<Dimension*Dimension; j++ )
    {
      optimizerAffineScales[i*numberOfParameters + j] = 1.0; // scale for indices in 2x2 (3x3) Matrix
    }
    for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
    {
      optimizerAffineScales[i*numberOfParameters + j] = 1.0 ; // scale for translation on X,Y,Z
    }
  }

  // Set the optimizer parameters
  if(optimizerType == "FRPR")
  {
    FRPRoptimizer->SetStepLength(optAffineLearningRate);
    FRPRoptimizer->SetMaximize(false);
    FRPRoptimizer->SetMaximumIteration( optAffineNumberOfIterations );
    FRPRoptimizer->SetMaximumLineIteration( 10 );
    FRPRoptimizer->SetScales( optimizerAffineScales );
    FRPRoptimizer->SetToPolakRibiere();
    //FRPRoptimizer->SetToFletchReeves();
  }
  else if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optAffineLearningRate);
    lineSearchOptimizer->SetMaximize(false);
    lineSearchOptimizer->SetMaximumIteration( optAffineNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( 10 );
    lineSearchOptimizer->SetScales( optimizerAffineScales );
  }
  else
  {
    optimizer->SetLearningRate( optAffineLearningRate );
    optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
    optimizer->MaximizeOff();
    optimizer->SetScales( optimizerAffineScales );
    // Create the Command observer and register it with the optimizer.
    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    //optimizer->AddObserver( itk::IterationEvent(), observer );
  }

  //Set the parameters of the command observer
  command->SetMultiScaleSamplePercentageIncrease(affineMultiScaleSamplePercentageIncrease);
  command->SetMultiScaleMaximumIterationIncrease(affineMultiScaleMaximumIterationIncrease);
  command->SetMultiScaleStepLengthIncrease(affineMultiScaleStepLengthIncrease);
  
  // Start registration with Affine Transform
  try
  {
    registration->StartRegistration();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }

  
  //Print out the metric values for translation parameters
  if( metricPrint == "on")
  {
    cout << "Metric Probe " << endl;
    ofstream outputFile("metricOutput.txt");
    ParametersType parameters = registration->GetLastTransformParameters();

    for(double i=-7.5; i<7.5; i+=0.5)
    {
      for(double j=-7.5; j<7.5; j+=0.5)
      {
        parameters[10] = i;
        parameters[11] = j;

        if(metricType =="variance")
        {
          outputFile << varianceMetric->GetValue(parameters) << " ";
        }
        else
        {
          outputFile << entropyMetric->GetValue(parameters) << " ";
        }
      }
      outputFile << std::endl;
    }
    outputFile.close();
  }

  //
  // BSpline Registration
  //
  // We supply the affine transform from the first part as initial parameters to the Bspline
  // registration. As the user might want to increase the resolution of the Bspline grid
  // we begin by declaring the Bspline parameters for the low resolution

  const unsigned int SplineOrder = 3;
  typedef ScalarType CoordinateRepType;

  typedef itk::BSplineDeformableTransform< CoordinateRepType,
  Dimension,
  SplineOrder >     BSplineTransformType;

    // Allocate bspline tranform array for low grid size
  typedef vector<BSplineTransformType::Pointer> BSplineTransformArrayType;
  BSplineTransformArrayType      bsplineTransformArrayLow(N);

  BSplineTransformArrayType bsplineTransformArrayHigh(N);

  // Allocate the vector holding Bspline transform parameters for low resolution
  typedef BSplineTransformType::ParametersType     BSplineParametersType;
  vector<BSplineParametersType> bsplineParametersArrayLow(N);
  vector<BSplineParametersType> bsplineParametersArrayHigh(N);

  
  if( useBspline == "on" )
  {
    
    // typdefs for region, spacing and origin of the Bspline coefficient images
    typedef BSplineTransformType::RegionType RegionType;
    typedef BSplineTransformType::SpacingType SpacingType;
    typedef BSplineTransformType::OriginType OriginType;


    // Get the latest transform parameters of affine transfom
    ParametersType affineParameters = registration->GetLastTransformParameters();
    ParametersType affineCurrentParameters(affineTransformArray[0]->GetNumberOfParameters());
  
    // Update the affine transform parameters
    for( int i=0; i<N; i++)
    {
      for(int j=0; j<affineTransformArray[0]->GetNumberOfParameters(); j++)
      {
        affineCurrentParameters[j]=affineParameters[i*affineTransformArray[0]->GetNumberOfParameters()+j];
      }
      affineTransformArray[i]->SetParametersByValue(affineCurrentParameters);
    }

    // Initialize the size of the parameters array
    registration->SetTransformParametersLength( static_cast<int>( pow( static_cast<double>(bsplineInitialGridSize+SplineOrder),
                                                 static_cast<int>(Dimension))*Dimension*N ));

  
    //
    // As in the affine registration each image has its own pointers.
    // As we dont change image readers, normalized fileters, Gaussian filters and
    // the interpolaters. We dont re-register them with the registration method.
    // We only need to reinitilize Bspline tranforms for each input image.
    //
    // Connect the compenents together
    //
    try
    {
      for( int i=0; i< N; i++ ){

        bsplineTransformArrayLow[i] = BSplineTransformType::New();
      
        RegionType bsplineRegion;
        RegionType::SizeType   gridSizeOnImage;
        RegionType::SizeType   gridBorderSize;
        RegionType::SizeType   totalGridSize;

        gridSizeOnImage.Fill( bsplineInitialGridSize ); // We actually should initilize gridSizeOnImage taking into account image dimensions
        gridBorderSize.Fill( SplineOrder );    // Border for spline order = 3 ( 1 lower, 2 upper )
        totalGridSize = gridSizeOnImage + gridBorderSize;

        bsplineRegion.SetSize( totalGridSize );

        // Get the spacing, origin and imagesize form the image readers
        SpacingType spacing;
        OriginType origin;
        ImageType::RegionType fixedRegion;
        if( imageType == "DICOM")
        {
          spacing = dicomArrayReader[i]->GetOutput()->GetSpacing();
          origin = dicomArrayReader[i]->GetOutput()->GetOrigin();
          fixedRegion = dicomArrayReader[i]->GetOutput()->GetBufferedRegion();
        }
        else
        {
          spacing = imagePyramidArray[i]->GetOutput(0)->GetSpacing();
          origin = imagePyramidArray[i]->GetOutput(0)->GetOrigin();
          fixedRegion = imagePyramidArray[i]->GetOutput(0)->GetBufferedRegion();
        }

        ImageType::SizeType fixedImageSize = fixedRegion.GetSize();

        // Calculate the spacing for the Bspline grid
        for(unsigned int r=0; r<Dimension; r++)
        {
          // There was a floor here, is it a bug? The floor causes a division by zero error
          // if the gridSizeOnImage is larger than fixedImageSize
          spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
              static_cast<double>(gridSizeOnImage[r] - 1) );
          origin[r]  -=  spacing[r];
        }

        // Set the spacing origin and bsplineRegion
        bsplineTransformArrayLow[i]->SetGridSpacing( spacing );
        bsplineTransformArrayLow[i]->SetGridOrigin( origin );
        bsplineTransformArrayLow[i]->SetGridRegion( bsplineRegion );


        unsigned int numberOfParametersLow =
                        bsplineTransformArrayLow[i]->GetNumberOfParameters();

        // Set the initial Bspline parameters to zero
        bsplineParametersArrayLow[i].SetSize( numberOfParametersLow );
        bsplineParametersArrayLow[i].Fill( 0.0 );

        // Set the affine tranform and initial paramters of Bsplines
        bsplineTransformArrayLow[i]->SetBulkTransform(affineTransformArray[i]);
        bsplineTransformArrayLow[i]->SetParameters( bsplineParametersArrayLow[i] );

        // register Bspline pointers with the registration method
        registration->SetInitialTransformParameters( bsplineTransformArrayLow[i]->GetParameters(), i);
        registration->SetTransformArray(     bsplineTransformArrayLow[i] ,i    );
      }
    }
    catch( itk::ExceptionObject & err )
    {
      std::cout << "ExceptionObject caught !" << std::endl;
      std::cout << err << std::endl;
      return -1;
    }


    // Reset the optimizer scales
    // All parameters are set to be equal
    optimizerScales.SetSize( bsplineTransformArrayLow[0]->GetNumberOfParameters()*N);
    optimizerScales.Fill( 1.0 );
    if(optimizerType == "FRPR")
    {
      FRPRoptimizer->SetScales( optimizerScales );
      FRPRoptimizer->SetStepLength(optBsplineLearningRate);
      FRPRoptimizer->SetMaximumIteration( optBsplineNumberOfIterations );
      FRPRoptimizer->SetScales( optimizerScales );
    }
    else if(optimizerType == "lineSearch")
    {
      lineSearchOptimizer->SetScales( optimizerScales );
      lineSearchOptimizer->SetStepLength(optBsplineLearningRate);
      lineSearchOptimizer->SetMaximumIteration( optBsplineNumberOfIterations );
      lineSearchOptimizer->SetScales( optimizerScales );
    }
    else
    {
      optimizer->SetScales( optimizerScales );
      optimizer->SetLearningRate( optBsplineLearningRate );
      optimizer->SetNumberOfIterations( optBsplineNumberOfIterations );
    }



    // Set the number of samples to be used by the metric
    if(metricType == "variance")
    {
      varianceMetric->SetNumberOfSpatialSamples( static_cast<int>(numberOfPixels * numberOfSpatialSamplesBsplinePercentage) );
    }
    else
    {
      entropyMetric->SetNumberOfSpatialSamples( static_cast<int>(numberOfPixels * numberOfSpatialSamplesBsplinePercentage) );
    }

    registration->SetNumberOfLevels( multiLevelBspline );

    //Set the parameters of the command observer
    command->SetMultiScaleSamplePercentageIncrease(bsplineMultiScaleSamplePercentageIncrease);
    command->SetMultiScaleMaximumIterationIncrease(bsplineMultiScaleMaximumIterationIncrease);
    command->SetMultiScaleStepLengthIncrease(bsplineMultiScaleStepLengthIncrease);

    std::cout << "Starting BSpline Registration with low resolution transform: " << std::endl;
    std::cout << "Resolution level " << 0;
    std::cout << " Number Of parameters: " << bsplineTransformArrayLow[0]->GetNumberOfParameters()*N <<std::endl;
    try
    {
      registration->StartRegistration();
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return -1;
    }


    // Using the result of the low grid Bspline registration
    // begin a registration on a finer grid.
    // Calculate the initial parameters for the finer grid using
    // itkBsplineDecomposition filter
    //
    // We need to register higher resolution Bsplines with the registration
    // method.
    //
    
    if(useBsplineHigh =="on")
    {

      // For each level in the Bspline increase the resolution grid by
      // a factor of two
      for(int level=0; level < numberOfBsplineLevel; level++)
      {
        // Copy the last parameters of coarse grid Bspline
        BSplineParametersType parametersLow = registration->GetLastTransformParameters();
    
        int numberOfParametersLow = bsplineTransformArrayLow[0]->GetNumberOfParameters();
        BSplineParametersType currentParametersLow(numberOfParametersLow);

        // Update the last transform parameters
        for(int i=0; i<N; i++)
        {
          //copy current parameters
          for(int j=0; j<numberOfParametersLow; j++ )
            currentParametersLow[j] = parametersLow[numberOfParametersLow*i + j];

          bsplineTransformArrayLow[i]->SetParametersByValue( currentParametersLow );
        }


        // Increase the grid size by a factor of two
        bsplineInitialGridSize *= 2;
        registration->SetTransformParametersLength( static_cast<int>( pow( static_cast<double>(bsplineInitialGridSize+SplineOrder),
                                                    static_cast<int>(Dimension))*Dimension*N ));

        // Set the parameters of the high resolution Bspline Transform
        for( int i=0; i<N; i++)
        {

          bsplineTransformArrayHigh[i] = BSplineTransformType::New();

          RegionType bsplineRegion;
          RegionType::SizeType   gridHighSizeOnImage;
          RegionType::SizeType   gridBorderSize;
          RegionType::SizeType   totalGridSize;
      
          gridBorderSize.Fill( SplineOrder );
          gridHighSizeOnImage.Fill( bsplineInitialGridSize );
          totalGridSize = gridHighSizeOnImage + gridBorderSize;

          bsplineRegion.SetSize( totalGridSize );

          SpacingType spacingHigh;
          OriginType  originHigh;
          ImageType::RegionType fixedRegion;

          if(imageType =="DICOM")
          {
            spacingHigh = dicomArrayReader[i]->GetOutput()->GetSpacing();
            originHigh  = dicomArrayReader[i]->GetOutput()->GetOrigin();
            fixedRegion = dicomArrayReader[i]->GetOutput()->GetBufferedRegion();
          }
          else
          {
            spacingHigh = imagePyramidArray[i]->GetOutput(0)->GetSpacing();
            originHigh  = imagePyramidArray[i]->GetOutput(0)->GetOrigin();
            fixedRegion = imagePyramidArray[i]->GetOutput(0)->GetBufferedRegion();
          }
          ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
    
          for(unsigned int rh=0; rh<Dimension; rh++)
          {
            //There was a floor here, is it a BUG?
            spacingHigh[rh] *= floor( static_cast<double>(fixedImageSize[rh] - 1)  /
                static_cast<double>(gridHighSizeOnImage[rh] - 1) );
            originHigh[rh]  -=  spacingHigh[rh];
          }

          bsplineTransformArrayHigh[i]->SetGridSpacing( spacingHigh );
          bsplineTransformArrayHigh[i]->SetGridOrigin( originHigh );
          bsplineTransformArrayHigh[i]->SetGridRegion( bsplineRegion );

          bsplineParametersArrayHigh[i].SetSize( bsplineTransformArrayHigh[i]->GetNumberOfParameters() );
          bsplineParametersArrayHigh[i].Fill( 0.0 );


          //  Now we need to initialize the BSpline coefficients of the higher resolution
          //  transform. We take the coefficient image of the low resolution Bspline.
          // using this image we initiliaze the coefficients of the finer grid Bspline
          //
          int counter = 0;

          for ( unsigned int k = 0; k < Dimension; k++ )
          {
            typedef BSplineTransformType::ImageType ParametersImageType;
            typedef itk::ResampleImageFilter<ParametersImageType,ParametersImageType> ResamplerType;
            ResamplerType::Pointer upsampler = ResamplerType::New();

            typedef itk::BSplineResampleImageFunction<ParametersImageType,double> FunctionType;
            FunctionType::Pointer function = FunctionType::New();

            typedef itk::IdentityTransform<double,Dimension> IdentityTransformType;
            IdentityTransformType::Pointer identity = IdentityTransformType::New();

            upsampler->SetInput( bsplineTransformArrayLow[i]->GetCoefficientImage()[k] );
            upsampler->SetInterpolator( function );
            upsampler->SetTransform( identity );
            upsampler->SetSize( bsplineTransformArrayHigh[i]->GetGridRegion().GetSize() );
            upsampler->SetOutputSpacing( bsplineTransformArrayHigh[i]->GetGridSpacing() );
            upsampler->SetOutputOrigin( bsplineTransformArrayHigh[i]->GetGridOrigin() );

            typedef itk::BSplineDecompositionImageFilter<ParametersImageType,ParametersImageType>
                DecompositionType;
            DecompositionType::Pointer decomposition = DecompositionType::New();

            decomposition->SetSplineOrder( SplineOrder );
            decomposition->SetInput( upsampler->GetOutput() );
            decomposition->Update();

            ParametersImageType::Pointer newCoefficients = decomposition->GetOutput();

            // copy the coefficients into the parameter array
            typedef itk::ImageRegionIterator<ParametersImageType> Iterator;
            Iterator it( newCoefficients, bsplineTransformArrayHigh[i]->GetGridRegion() );
            while ( !it.IsAtEnd() )
            {
              bsplineParametersArrayHigh[i][ counter++ ] = it.Get();
              ++it;
            }

          }

          bsplineTransformArrayHigh[i]->SetParameters( bsplineParametersArrayHigh[i] );

          // Set parameters of the fine grid Bspline transform
          // to coarse grid Bspline transform for the next level
          bsplineTransformArrayLow[i]->SetGridSpacing( spacingHigh );
          bsplineTransformArrayLow[i]->SetGridOrigin( originHigh );
          bsplineTransformArrayLow[i]->SetGridRegion( bsplineRegion );


          // Set initial parameters of the registration
          registration->SetInitialTransformParameters( bsplineTransformArrayHigh[i]->GetParameters() , i );
          registration->SetTransformArray( bsplineTransformArrayHigh[i], i );

        }

        std::cout << "Starting Registration with high resolution transform: " << std::endl;
        std::cout << "Resolution level " << level;
        std::cout << " Number Of parameters: " << bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N <<std::endl;

        // Decrease the learning rate at each level
        // Reset the optimizer scales
        typedef OptimizerType::ScalesType       OptimizerScalesType;
        OptimizerScalesType optimizerScales( bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N );
        optimizerScales.Fill( 1.0 );

        if(optimizerType == "FRPR")
        {
          FRPRoptimizer->SetScales( optimizerScales );
          FRPRoptimizer->SetStepLength(optBsplineHighLearningRate);
          FRPRoptimizer->SetMaximumIteration( optBsplineHighNumberOfIterations );
          //FRPRoptimizer->SetMaximumLineIteration( 1 );
          FRPRoptimizer->SetScales( optimizerScales );
        }
        else if(optimizerType == "lineSearch")
        {
          lineSearchOptimizer->SetScales( optimizerScales );
          lineSearchOptimizer->SetStepLength(optBsplineHighLearningRate);
          lineSearchOptimizer->SetMaximumIteration( optBsplineHighNumberOfIterations );
          //lineSearchOptimizer->SetMaximumLineIteration( 1 );
          lineSearchOptimizer->SetScales( optimizerScales );
        }
        else
        {
          optimizer->SetScales( optimizerScales );
          optimizer->SetLearningRate( optBsplineHighLearningRate );
          optimizer->SetNumberOfIterations( optBsplineHighNumberOfIterations );
        }


        // Set the number of samples to be used by the metric
        if(metricType == "variance")
        {
          varianceMetric->SetNumberOfSpatialSamples( static_cast<int>(numberOfPixels * numberOfSpatialSamplesBsplineHighPercentage));
        }
        else
        {
          entropyMetric->SetNumberOfSpatialSamples( static_cast<int>(numberOfPixels * numberOfSpatialSamplesBsplineHighPercentage) );
        }

        registration->SetNumberOfLevels( multiLevelBsplineHigh );

        try
       {
          registration->StartRegistration();
       }
       catch( itk::ExceptionObject & err )
       {
          std::cerr << "ExceptionObject caught !" << std::endl;
          std::cerr << err << std::endl;
          return -1;
       }

      }

    } // End of BsplineHigh registration

  } // End of Bspline Registration
  //
  //
  //
  //
  // End of registration
  // The following code gets the last tranform parameters and writes the output images
  //
  //
  //
  ParametersType finalParameters = registration->GetLastTransformParameters();

  unsigned int numberOfIterations;
  double bestValue;
  if(optimizerType == "FRPR")
  {
    numberOfIterations = FRPRoptimizer->GetCurrentIteration();
    bestValue = FRPRoptimizer->GetValue();
  }
  else if(optimizerType == "lineSearch")
  {
    numberOfIterations = lineSearchOptimizer->GetCurrentIteration();
    bestValue = lineSearchOptimizer->GetValue();
  }
  else
  {
    numberOfIterations = optimizer->GetCurrentIteration();
    bestValue = optimizer->GetValue();
  }


  
  // Print out results
  //
  std::cout << "Result = " << std::endl;
  std::cout << " final parameters 0 = " << finalParameters[0]  << std::endl;
  std::cout << " final parameters 13 = " << finalParameters[13]  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  // Get the time for the registration
  collector.Stop( "Registration" );
  collector.Report();  
  

  // typedefs for output images
  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            ImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->ReleaseDataFlagOn();
  
  ImageArrayReader imageArrayReader(N);

  ImageType::Pointer fixedImage;
  
  typedef  unsigned short  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        ImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;  

  //DICOM writer type definitions
  typedef itk::Image< OutputPixelType, 2 >    Image2DType;
  typedef itk::ImageSeriesWriter< ImageType, Image2DType >  SeriesWriterType;
  SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
  
  WriterType::Pointer      writer =  WriterType::New();
  writer->ReleaseDataFlagOn();
  CastFilterType::Pointer  caster = CastFilterType::New();
  caster->ReleaseDataFlagOn();


  // Get the correct number of paramaters
  if(useBsplineHigh == "on")
  {
    numberOfParameters = bsplineTransformArrayHigh[0]->GetNumberOfParameters();
  }
  else if (useBspline == "on")
  {
    numberOfParameters = bsplineTransformArrayLow[0]->GetNumberOfParameters();
  }
  else
  {
    numberOfParameters = affineTransformArray[0]->GetNumberOfParameters();
  }
  ParametersType currentParameters(numberOfParameters);
  ParametersType currentParameters2(numberOfParameters);


  // Update last Transform Parameters
  for(int i=0; i<N; i++)
  {
    //copy current parameters
    for(int j=0; j<numberOfParameters; j++ )
    {
      currentParameters[j] = finalParameters[numberOfParameters*i + j];
    }

    if(useBsplineHigh == "on")
    {
      bsplineTransformArrayHigh[i]->SetBulkTransform( affineTransformArray[i] );
      bsplineTransformArrayHigh[i]->SetParametersByValue( currentParameters );
    }
    else if (useBspline == "on")
    {
      bsplineTransformArrayLow[i]->SetBulkTransform( affineTransformArray[i] );
      bsplineTransformArrayLow[i]->SetParametersByValue( currentParameters );
    }
    else
    {
      affineTransformArray[i]->SetParametersByValue( currentParameters );
    }

  }

  // Loop over images and write output images
  for(int i=0; i<N; i++)
  {

    //Set the correct tranform
    if(useBsplineHigh == "on")
    {
      resample->SetTransform( bsplineTransformArrayHigh[i] );
    }
    else if (useBspline == "on")
    {
      resample->SetTransform( bsplineTransformArrayLow[i] );
    }
    else
    {
      resample->SetTransform( affineTransformArray[i] );
    }

    
    if( imageType == "DICOM")
    {
      resample->SetInput( dicomArrayReader[i]->GetOutput() );
      fixedImage = dicomArrayReader[i]->GetOutput();
    }
    else
    {
      //Read the images again for memory efficiency
      imageArrayReader[i] = ImageReaderType::New();
      imageArrayReader[i]->ReleaseDataFlagOn();
      imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
      imageArrayReader[i]->Update();
      resample->SetInput( imageArrayReader[i]->GetOutput() );
      fixedImage = imageArrayReader[i]->GetOutput();
    }
    resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(  fixedImage->GetOrigin() );
    resample->SetOutputSpacing( fixedImage->GetSpacing() );
    resample->SetOutputDirection( fixedImage->GetDirection());
    resample->SetDefaultPixelValue( 100 );

    std::cout << "Writing " << outputFileNames[i] << std::endl;
    if( imageType == "DICOM")
    {
      itksys::SystemTools::MakeDirectory( outputFileNames[i].c_str() );
      seriesWriter->SetInput( resample->GetOutput() );
      seriesWriter->SetImageIO( imageIOTypeArray[i] );
      namesGeneratorArray[i]->SetOutputDirectory( outputFileNames[i].c_str() );
      seriesWriter->SetFileNames( namesGeneratorArray[i]->GetOutputFileNames() );
      seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[i]->GetMetaDataDictionaryArray() );
      
      try
      {
        seriesWriter->Update();
      }
      catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Exception thrown while writing the series " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
      }
    }
    else
    {
      itksys::SystemTools::MakeDirectory( outputFolder.c_str() );
      caster->SetInput( resample->GetOutput() );
      writer->SetImageIO(imageArrayReader[0]->GetImageIO());
      writer->SetFileName( outputFileNames[i].c_str() );
      writer->SetInput( caster->GetOutput()   );
      writer->Update();
    }
  }

  std::cout << "Computing mean images" << std::endl;

  // Compute Mean Images 
  ResampleFilterType::Pointer resample2 = ResampleFilterType::New();
  resample2->ReleaseDataFlagOn();

  typedef itk::AddImageFilter < ImageType, ImageType,
                                           ImageType > AddFilterType;

  //Mean of the registered images
  AddFilterType::Pointer addition = AddFilterType::New();
  // Mean Image of original images
  AddFilterType::Pointer addition2 = AddFilterType::New();


  //Set the first image
  if(useBsplineHigh == "on")
  {
    resample->SetTransform( bsplineTransformArrayHigh[0] );
  }
  else if (useBspline == "on")
  {
    resample->SetTransform( bsplineTransformArrayLow[0] );
  }
  else
  {
    resample->SetTransform( affineTransformArray[0] );
  }

  if( imageType == "DICOM")
  {
    resample->SetInput( dicomArrayReader[0]->GetOutput() );
    fixedImage = dicomArrayReader[0]->GetOutput();
  }
  else
  {
    resample->SetInput( imageArrayReader[0]->GetOutput() );
    fixedImage = imageArrayReader[0]->GetOutput();
  }
  
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection());
  resample->SetDefaultPixelValue( 1 );
  addition->SetInput1( resample->GetOutput() );
  
  if( imageType == "DICOM")
  {
    addition2->SetInput1(dicomArrayReader[0]->GetOutput() );
  }
  else
  {
    addition2->SetInput1(imageArrayReader[0]->GetOutput() );
  }
  
  //Set the second image
  if(useBsplineHigh == "on")
  {
    resample2->SetTransform( bsplineTransformArrayHigh[1] );
  }
  else if (useBspline == "on")
  {
    resample2->SetTransform( bsplineTransformArrayLow[1] );
  }
  else
  {
    resample2->SetTransform( affineTransformArray[1] );
  }

  if( imageType == "DICOM")
  {
    resample2->SetInput( dicomArrayReader[1]->GetOutput() );
    fixedImage = dicomArrayReader[1]->GetOutput();
  }
  else
  {
    resample2->SetInput( imageArrayReader[1]->GetOutput() );
    fixedImage = imageArrayReader[1]->GetOutput();
  }
  resample2->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample2->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample2->SetOutputSpacing( fixedImage->GetSpacing() );
  resample2->SetOutputDirection( fixedImage->GetDirection());
  resample2->SetDefaultPixelValue( 1 );
  addition->SetInput2( resample2->GetOutput() );
  if( imageType == "DICOM")
  {
    addition2->SetInput2(dicomArrayReader[1]->GetOutput() );
  }
  else
  {
    addition2->SetInput2(imageArrayReader[1]->GetOutput() );
  }
  addition->Update();
  addition2->Update();

  //Add other images
  resample->SetDefaultPixelValue( 1 );
  for(int i=2; i<N; i++)
  {

    if(useBsplineHigh == "on")
    {
      resample->SetTransform( bsplineTransformArrayHigh[i] );
    }
    else if (useBspline == "on")
    {
      resample->SetTransform( bsplineTransformArrayLow[i] );
    }
    else
    {
      resample->SetTransform( affineTransformArray[i] );
    }

    if( imageType == "DICOM")
    {
      resample->SetInput( dicomArrayReader[i]->GetOutput() );
      fixedImage = dicomArrayReader[i]->GetOutput();
    }
    else
    {
      resample->SetInput( imageArrayReader[i]->GetOutput() );
      fixedImage = imageArrayReader[i]->GetOutput();
    }

    addition->SetInput1( addition->GetOutput() );
    addition->SetInput2( resample->GetOutput() );

    addition->Update();

    addition2->SetInput1( addition2->GetOutput() );
    if( imageType == "DICOM")
    {
      addition2->SetInput2(dicomArrayReader[i]->GetOutput() );
    }
    else
    {
      addition2->SetInput2(imageArrayReader[i]->GetOutput() );
    }
    addition2->Update();

  }


  //Write the mean image
  typedef itk::RescaleIntensityImageFilter< ImageType, ImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  WriterType::Pointer      writer2 =  WriterType::New();

  intensityRescaler->SetInput( addition->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  if( imageType == "DICOM")
  {
    string meanImageFname = outputFolder + "MeanRegisteredImage";
    itksys::SystemTools::MakeDirectory( meanImageFname.c_str() );
    seriesWriter->SetInput( intensityRescaler->GetOutput() );
    seriesWriter->SetImageIO( imageIOTypeArray[0] );
    namesGeneratorArray[0]->SetOutputDirectory( meanImageFname.c_str() );
    seriesWriter->SetFileNames( namesGeneratorArray[0]->GetOutputFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[0]->GetMetaDataDictionaryArray() );
    
    try
    {
      seriesWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown while writing the series " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    caster->SetInput( intensityRescaler->GetOutput() );
    writer2->SetInput( caster->GetOutput()   );

    string meanImageFname;
    if (Dimension == 2)
    {
      meanImageFname = outputFolder + "MeanRegisteredImage.png";
    }
    else
    {
      meanImageFname = outputFolder + "MeanRegisteredImage.mhd";
    }
    writer2->SetFileName( meanImageFname.c_str() );
    writer2->Update();
  }

  intensityRescaler->SetInput( addition2->GetOutput() );

  if( imageType == "DICOM")
  {
    string meanImageFname = outputFolder + "MeanOriginalImage";
    itksys::SystemTools::MakeDirectory( meanImageFname.c_str() );
    seriesWriter->SetInput( intensityRescaler->GetOutput() );
    seriesWriter->SetImageIO( imageIOTypeArray[0] );
    namesGeneratorArray[0]->SetOutputDirectory( meanImageFname.c_str() );
    seriesWriter->SetFileNames( namesGeneratorArray[0]->GetOutputFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[0]->GetMetaDataDictionaryArray() );
    
    try
    {
      seriesWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown while writing the series " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    caster->SetInput( intensityRescaler->GetOutput() );
    writer2->SetInput( caster->GetOutput()   );

    string meanImageFname;
    if (Dimension == 2)
    {
      meanImageFname = outputFolder + "MeanOriginalImage.png";
    }
    else
    {
      meanImageFname = outputFolder + "MeanOriginalImage.mhd";
    }
    writer2->SetFileName( meanImageFname.c_str() );
    writer2->Update();
  }

  

  return 0;
}




int getCommandLine(       int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,
                          int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                          double& optTranslationLearningRate, double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                          int& optTranslationNumberOfIterations, int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                          double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& transformType, string& imageType,string& metricType,
                          string& useBspline, string& useBsplineHigh,
                          double& translationMultiScaleSamplePercentageIncrease, double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,

                          double& translationMultiScaleMaximumIterationIncrease, double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,

                          double& translationMultiScaleStepLengthIncrease, double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease    )
{


  //read parameters
  for(int i=1; i<argc; i++)
  {
    string dummy(argv[i]);
    if(dummy == "-i")
      inputFolder = argv[++i];
    else if (dummy == "-o")
      outputFolder = argv[++i];
    else if (dummy == "-opt")
      optimizerType == argv[++i];
    else if (dummy == "-t")
      optimizerType == argv[++i];
    
    else if (dummy == "-imageType")
      imageType = argv[++i];
    else if (dummy == "-metricType")
      metricType = argv[++i];
    else if (dummy == "-optimizerType")
      optimizerType = argv[++i];
    
    else if (dummy == "-multiLevelAffine")
      multiLevelAffine = atoi(argv[++i]);
    else if (dummy == "-multiLevelBspline")
      multiLevelBspline = atoi(argv[++i]);
    else if (dummy == "-multiLevelBsplineHigh")
      multiLevelBsplineHigh = atoi(argv[++i]);
    
    else if (dummy == "-optTranslationLearningRate")
      optTranslationLearningRate = atof(argv[++i]);
    else if (dummy == "-optAffineLearningRate")
      optAffineLearningRate = atof(argv[++i]);
    else if (dummy == "-optBsplineLearningRate")
      optBsplineLearningRate = atof(argv[++i]);
    else if (dummy == "-optBsplineHighLearningrate")
      optBsplineHighLearningRate = atof(argv[++i]);

    else if (dummy == "-optTranslationNumberOfIterations")
      optTranslationNumberOfIterations = atoi(argv[++i]);
    else if (dummy == "-optAffineNumberOfIterations")
      optAffineNumberOfIterations = atoi(argv[++i]);
    else if (dummy == "-optBsplineNumberOfIterations")
      optBsplineNumberOfIterations = atoi(argv[++i]);
    else if (dummy == "-optBsplineHighNumberOfIterations")
      optBsplineHighNumberOfIterations = atoi(argv[++i]);

    else if (dummy == "-numberOfSpatialSamplesAffinePercentage")
      numberOfSpatialSamplesAffinePercentage = atof(argv[++i]);
    else if (dummy == "-numberOfSpatialSamplesBsplinePercentage")
      numberOfSpatialSamplesBsplinePercentage = atof(argv[++i]);
    else if (dummy == "-numberOfSpatialSamplesBsplineHighPercentage")
      numberOfSpatialSamplesBsplineHighPercentage = atof(argv[++i]);

    else if (dummy == "-translationMultiScaleSamplePercentageIncrease")
      translationMultiScaleSamplePercentageIncrease = atof(argv[++i]);
    else if (dummy == "-affineMultiScaleSamplePercentageIncrease")
      affineMultiScaleSamplePercentageIncrease = atof(argv[++i]);
    else if (dummy == "-bsplineMultiScaleSamplePercentageIncrease")
      bsplineMultiScaleSamplePercentageIncrease = atof(argv[++i]);
    

    else if (dummy == "-translationMultiScaleMaximumIterationIncrease")
      translationMultiScaleMaximumIterationIncrease = atof(argv[++i]);
    else if (dummy == "-affineMultiScaleMaximumIterationIncrease")
      affineMultiScaleMaximumIterationIncrease = atof(argv[++i]);
    else if (dummy == "-bsplineMultiScaleMaximumIterationIncrease")
      bsplineMultiScaleMaximumIterationIncrease = atof(argv[++i]);

    else if (dummy == "-translationMultiScaleStepLengthIncrease")
      translationMultiScaleStepLengthIncrease = atof(argv[++i]);
    else if (dummy == "-affineMultiScaleStepLengthIncrease")
      affineMultiScaleStepLengthIncrease = atof(argv[++i]);
    else if (dummy == "-bsplineMultiScaleStepLengthIncrease")
      bsplineMultiScaleStepLengthIncrease = atof(argv[++i]);
    
    else if (dummy == "-bsplineInitialGridSize")
      bsplineInitialGridSize = atoi(argv[++i]);
    else if (dummy == "-numberOfBsplineLevel")
      numberOfBsplineLevel = atoi(argv[++i]);
    
    else if (dummy == "-useBspline")
      useBspline = argv[++i];
    else if (dummy == "-useBsplineHigh")
      useBsplineHigh = argv[++i];
    
    else
      fileNames.push_back(dummy); // get file name
  }


  if( fileNames.size() < 2)
  {
    std::cerr << "Not enough input arguments " << std::endl;

    std::cerr << "\t  imageName [image names does not require any prefix parameters]" << std::endl;
    std::cerr << "\t -i Input folder for images" << std::endl;
    std::cerr << "\t -f Output folder for registered images" << std::endl;

    std::cerr << "\t -imageType imageType. 'DICOM' for DICOM images 'normal' otherwise. [default=DICOM]" << std::endl;
    
    std::cerr << "\t -multiLevelAffine int. Number of multiresolution levels for affine transform. [default=3]" << std::endl;
    std::cerr << "\t -multiLevelBspline int. Number of multiresolution levels for Bspline transform. [default=1]" << std::endl;
    std::cerr << "\t -multiLevelBsplineHigh int. Number of multiresolution levels for Bspline transform (finer grids). [default=1]" << std::endl;

    std::cerr << "\t -optAffineLearningRate double. Learning rate for affine optimizer. [default = 1e-4] " << std::endl;
    std::cerr << "\t -optBsplineLearningRate double. Learning rate for Bspline optimizer. [default = 1] " << std::endl;
    std::cerr << "\t -optBsplineHighLearningrate double. Learning rate for Bspline(finer grid) optimizer. [default = 1] " << std::endl;
    
    std::cerr << "\t -optAffineNumberOfIterations int. Number of iterations used by affine optimizer. [default = 300]" << std::endl;
    std::cerr << "\t -optBsplineNumberOfIterations int. Number of iterations used by Bspline optimizer. [default = 300]" << std::endl;
    std::cerr << "\t -optBsplineHighNumberOfIterations int. Number of iterations used by Bspline(fine grid) optimizer. [default = 300]" << std::endl;

    std::cerr << "\t -numberOfSpatialSamplesAffinePercentage double. Percentage of points used by metric.[default=0.05]" << std::endl;
    std::cerr << "\t -numberOfSpatialSamplesBsplinePercentage double. Percentage of points used by metric.[default=0.05]" << std::endl;
    std::cerr << "\t -numberOfSpatialSamplesBsplineHighPercentage double. Percentage of points used by metric.[default=0.05]" << std::endl;

    std::cerr << "\t -bsplineInitialGridSize int. Size of the initial bspline grid size. [default=5]" << std::endl;
    
    std::cerr << "\t -numberOfBsplineLevel int. Number of bspline grid refinements. [default = 1]" << std::endl;
    
    std::cerr << "\t -useBspline ('on' or 'off). Perform Bspline registration after affine. [default=on]" << std::endl;
    std::cerr << "\t -useBsplineHigh ('on' or 'off'). Perform Bspline refinement. [default=on]" << std::endl;
    
    std::cerr << "\t -opt Optimizer Type" << std::endl;
    std::cerr << "\t -t Transform Type: Bspline Affine Translation" << std::endl;
    std::cerr << std::endl << "Usage: " << std::endl << "\t" << argv[0];
    std::cerr << " -i folder1/ -f output/ -o gradient -m 4 -t Affine ImageFile1  ImageFile2 ImageFile3 ... " << std::endl << std::endl;
    return 1;
  }
  else
    return 0;

}

