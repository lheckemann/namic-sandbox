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
#include "itkMultiResolutionImageRegistrationMethod.h"
//#include "itkMutualInformationImageToImageMetric.h"
#include "MutualInformationImageToImageMetricBsplineRegularization.h"
#include "itkImageToImageMetric.h"

#include "AddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkNormalizeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

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

//header for creating mask
#include "itkImageMaskSpatialObject.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"

//Define the global types for image type
#define PixelType unsigned char
#define InternalPixelType double
#define Dimension 3

using namespace std;;
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
  
  typedef   itk::SingleValuedNonLinearOptimizer   OptimizerType;
  typedef   const OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer       GradientOptimizerType;
  typedef   const GradientOptimizerType *             GradientOptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;
  typedef   const LineSearchOptimizerType  *          LineSearchOptimizerPointer;


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

      // only print results every ten iterations
      if(m_CumulativeIterationIndex % 10 != 0 && false)
      {
        m_CumulativeIterationIndex++;
        return;
      }
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer =
            dynamic_cast< GradientOptimizerPointer >(object );
        std::cout << std::setiosflags(ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter " << std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << gradientPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << gradientPointer->GetValue() << "   " << std::endl;
        if(gradientPointer->GetCurrentIteration() % 10 == 0 )
        {
          //std::cout << std::setw(6) << "Position: " << gradientPointer->GetCurrentPosition() << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >( object );
        std::cout << std::setiosflags(ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << lineSearchOptimizerPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << lineSearchOptimizerPointer->GetValue() << "   " << std::endl;
        if(lineSearchOptimizerPointer->GetCurrentIteration() % 6 == 0 )
        {
          //std::cout << std::setw(6) << "Position: " << lineSearchOptimizerPointer->GetCurrentPosition() << std::endl;
        }
      }

      //Increase the cumulative index
      m_CumulativeIterationIndex++;

      
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
  typedef   itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;
  typedef   LineSearchOptimizerType  *          LineSearchOptimizerPointer;

  typedef   itk::Image< InternalPixelType, Dimension >   InternalImageType;
  typedef   itk::ImageToImageMetric< InternalImageType, InternalImageType>    MetricType;
  typedef   MetricType *                                 MetricPointer;
  typedef   itk::MutualInformationImageToImageMetricBsplineRegularization< InternalImageType, InternalImageType >    MutualInformationMetricType;
  typedef   MutualInformationMetricType *                                 MutualInformationMetricTypePointer;

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

    // Output message about registration
    std::cout << "message: Registration using " << registration->GetTransform()->GetNameOfClass() << std::endl;
    std::cout << "message: Multiresolution level : " << registration->GetCurrentLevel() << std::endl;
    std::cout << "message: Number of total parameters : " << registration->GetTransform()->GetNumberOfParameters() << std::endl;
    std::cout << "message: Optimizertype : " << optimizer->GetNameOfClass() << std::endl;

    if ( registration->GetCurrentLevel() == 0 )
    {
      // Set the number of spatial samples according to the current level
      if(  !strcmp(metric->GetNameOfClass(), "VarianceMultiImageMetric" ) )
      {
        MutualInformationMetricTypePointer  mutualInformationMetric = dynamic_cast< MutualInformationMetricTypePointer>
                                                              (registration->GetMetric());
        mutualInformationMetric->SetNumberOfSpatialSamples(
            (unsigned int) (mutualInformationMetric->GetNumberOfSpatialSamples() /
                        pow( pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease,
                             (double) (registration->GetNumberOfLevels() - 1.0) ) ) );
      }

      
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer = dynamic_cast< GradientOptimizerPointer >(
            registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations(
             (int)(gradientPointer->GetNumberOfIterations()*pow(m_MultiScaleMaximumIterationIncrease,
                   (double) (registration->GetNumberOfLevels() - 1.0) ) ));
        gradientPointer->SetLearningRate(
            gradientPointer->GetLearningRate()*pow(m_MultiScaleStepLengthIncrease,
                (double) (registration->GetNumberOfLevels() - 1.0) )  );
        //print messages
        std::cout << "message: Optimizer number of Iterations : " << gradientPointer->GetNumberOfIterations() << std::endl;
        std::cout << "message: Learning rate : " << gradientPointer->GetLearningRate() << std::endl;

      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >(
            registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration(
            (int)(lineSearchOptimizerPointer->GetMaximumIteration()*pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        lineSearchOptimizerPointer->SetStepLength(lineSearchOptimizerPointer->GetStepLength()*
            pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) );
        //print messages
        std::cout << "message: Optimizer number of Iterations : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() <<std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

      
    }
    else
    {
      // Set the number of spatial samples according to the current level
      if(  !strcmp(metric->GetNameOfClass(), "VarianceMultiImageMetric" ) )
      {
        MutualInformationMetricTypePointer  mutualInformationMetric = dynamic_cast< MutualInformationMetricTypePointer>
            (registration->GetMetric());
        mutualInformationMetric->SetNumberOfSpatialSamples(
            (unsigned int) (mutualInformationMetric->GetNumberOfSpatialSamples() *
                  pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease ) );
      }


      // Decrease the learning rate at each increasing multiresolution level
      // Increase the number of steps
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer =
            dynamic_cast< GradientOptimizerPointer >(
                                                                 registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations(
            (int) (gradientPointer->GetNumberOfIterations()/m_MultiScaleMaximumIterationIncrease ));
        gradientPointer->SetLearningRate(
            gradientPointer->GetLearningRate()/m_MultiScaleStepLengthIncrease  );
        //print messages
        std::cout << "message: Optimizer number of Iterations : " << gradientPointer->GetNumberOfIterations() << std::endl;
        std::cout << "message: Learning rate : " << gradientPointer->GetLearningRate() << std::endl;
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >(registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration(
            (int)(lineSearchOptimizerPointer->GetMaximumIteration()/m_MultiScaleMaximumIterationIncrease ));
        lineSearchOptimizerPointer->SetStepLength(
            lineSearchOptimizerPointer->GetStepLength()/m_MultiScaleStepLengthIncrease );
        
        std::cout << "message: Optimizer number of Iterations : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() << std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

    }


    std::cout << "message: Number of total pixels : " << metric->GetFixedImageRegion().GetNumberOfPixels() << std::endl;
    std::cout << "message: Number of used pixels : " << metric->GetNumberOfPixelsCounted() << std::endl;

  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
  
  protected:
    //Constructor initialize the variables
    RegistrationInterfaceCommand()
    {
      m_MultiScaleSamplePercentageIncrease = 8;
      m_MultiScaleMaximumIterationIncrease = 8;
      m_MultiScaleStepLengthIncrease = 8;
    };
  private:
    double m_MultiScaleSamplePercentageIncrease;
    double m_MultiScaleMaximumIterationIncrease;
    double m_MultiScaleStepLengthIncrease;
    

};

// Get the command line arguments
int getCommandLine(int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,
                   int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                   double& optTranslationLearningRate, double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                   int& optTranslationNumberOfIterations, int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                   double& numberOfSpatialSamplesTranslationPercentage, double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                   int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                   string& useBSplineRegularization, double& bsplineRegularizationFactor,
                   string& transformType, string& imageType,string& metricType, string& useBspline, string& useBsplineHigh,
                   double& translationScaleCoeffs,  double& gaussianFilterVariance,
                   int& maximumLineIteration, double& parzenWindowStandardDeviation,
                   double& translationMultiScaleSamplePercentageIncrease, double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,
                   double& translationMultiScaleMaximumIterationIncrease, double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,
                   double& translationMultiScaleStepLengthIncrease, double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease,
                   unsigned int& numberOfSpatialSamplesTranslation, unsigned int& numberOfSpatialSamplesAffine, unsigned int& numberOfSpatialSamplesBspline, unsigned int& numberOfSpatialSamplesBsplineHigh,
                   string &mask, string& maskType, unsigned int& threshold1, unsigned int threshold2,
                   string &writeOutputImages, string &writeDeformationFields );


int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder("");
  string outputFolder("");
  
  string optimizerType("lineSearch");
  string transformType("");
  string metricType("entropy");

  string metricPrint("off");
  
  int multiLevelAffine = 2;
  int multiLevelBspline = 1;
  int multiLevelBsplineHigh = 1;

  double optTranslationLearningRate = 1e5;
  double optAffineLearningRate = 1e-3;
  double optBsplineLearningRate = 1e5;
  double optBsplineHighLearningRate = 1e5;

  int optTranslationNumberOfIterations = 500;
  int optAffineNumberOfIterations = 500;
  int optBsplineNumberOfIterations = 500;
  int optBsplineHighNumberOfIterations = 500;

  double numberOfSpatialSamplesTranslationPercentage = 0;
  double numberOfSpatialSamplesAffinePercentage = 0;
  double numberOfSpatialSamplesBsplinePercentage = 0;
  double numberOfSpatialSamplesBsplineHighPercentage = 0;

  unsigned int numberOfSpatialSamplesTranslation = 2000;
  unsigned int numberOfSpatialSamplesAffine = 2000;
  unsigned int numberOfSpatialSamplesBspline = 4000;
  unsigned int numberOfSpatialSamplesBsplineHigh = 4000;

  double translationMultiScaleSamplePercentageIncrease = 4.0;
  double affineMultiScaleSamplePercentageIncrease = 4.0;
  double bsplineMultiScaleSamplePercentageIncrease = 4.0;
  
  double translationMultiScaleMaximumIterationIncrease = 1.0;
  double affineMultiScaleMaximumIterationIncrease = 1.0;
  double bsplineMultiScaleMaximumIterationIncrease = 1.0;

  
  double translationMultiScaleStepLengthIncrease = 1e-1;
  double affineMultiScaleStepLengthIncrease = 1e-1;
  double bsplineMultiScaleStepLengthIncrease = 1e-1;

  double translationScaleCoeffs = 1e-5;
  double gaussianFilterVariance = 2.0;
  int maximumLineIteration = 10;
  
  int bsplineInitialGridSize = 5;
  int numberOfBsplineLevel = 1;
  string useBSplineRegularization("on");
  double bsplineRegularizationFactor = 1e-1;
  double parzenWindowStandardDeviation = 0.4;

  string mask("all");
  string maskType("none");
  unsigned int threshold1 = 9;
  unsigned int threshold2 = 20;
      
  string imageType = "normal";

  string useBspline("off");
  string useBsplineHigh("off");

  string writeOutputImages("on");
  string writeDeformationFields("on");

  //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if( getCommandLine(argc,argv[i], fileNames, inputFolder, outputFolder, optimizerType,
        multiLevelAffine, multiLevelBspline, multiLevelBsplineHigh,
        optTranslationLearningRate, optAffineLearningRate,  optBsplineLearningRate, optBsplineHighLearningRate,
        optTranslationNumberOfIterations, optAffineNumberOfIterations, optBsplineNumberOfIterations, optBsplineHighNumberOfIterations,
        numberOfSpatialSamplesTranslationPercentage, numberOfSpatialSamplesAffinePercentage, numberOfSpatialSamplesBsplinePercentage, numberOfSpatialSamplesBsplineHighPercentage,
        bsplineInitialGridSize, numberOfBsplineLevel,
        useBSplineRegularization, bsplineRegularizationFactor,
        transformType, imageType, metricType, useBspline, useBsplineHigh,
        translationScaleCoeffs,gaussianFilterVariance, maximumLineIteration,  parzenWindowStandardDeviation,
        translationMultiScaleSamplePercentageIncrease, affineMultiScaleSamplePercentageIncrease, bsplineMultiScaleSamplePercentageIncrease, translationMultiScaleMaximumIterationIncrease, affineMultiScaleMaximumIterationIncrease,  bsplineMultiScaleMaximumIterationIncrease,
        translationMultiScaleStepLengthIncrease, affineMultiScaleStepLengthIncrease, bsplineMultiScaleStepLengthIncrease,
        numberOfSpatialSamplesTranslation, numberOfSpatialSamplesAffine, numberOfSpatialSamplesBspline, numberOfSpatialSamplesBsplineHigh,
        mask, maskType, threshold1, threshold2,
        writeOutputImages, writeDeformationFields) )
    {
      std:: cout << "Error reading parameter file " << std::endl;
      return 1;
    }
  }


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
  typedef itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;

  typedef itk::LinearInterpolateImageFunction<InternalImageType,ScalarType        > InterpolatorType;
                                    
  typedef itk::MutualInformationImageToImageMetricBsplineRegularization< InternalImageType, InternalImageType >    MutualInformationMetricType;


  typedef OptimizerType::ScalesType       OptimizerScalesType;

  typedef itk::MultiResolutionImageRegistrationMethod< InternalImageType, InternalImageType >    RegistrationType;

  //Mask related typedefs
  typedef itk::Image< unsigned char, Dimension > ImageMaskType;
  typedef itk::ConnectedThresholdImageFilter< ImageType,ImageMaskType >
                                        ConnectedThresholdImageFilterType;
  typedef itk::NeighborhoodConnectedImageFilter< ImageType,ImageMaskType >
                                        NeighborhoodConnectedImageFilterType;
  typedef itk::ImageMaskSpatialObject<Dimension> ImageMaskSpatialObject;


  RegistrationType::Pointer   registration  = RegistrationType::New();


  // N is the number of images in the registration
  const unsigned int N = fileNames.size();
  if( N != 2 )
  {
    std::cout << "number of files is not equal to two " << std::endl;
    return 1;
  }
  
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

  //Set the optimizerType
  OptimizerType::Pointer      optimizer;
  LineSearchOptimizerType::Pointer lineSearchOptimizer;
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer     = LineSearchOptimizerType::New();
    if(metricType == "MI")
    {
      lineSearchOptimizer->SetMaximize(false);
    }
    else
    {
      lineSearchOptimizer->SetMaximize(true);
    }
    registration->SetOptimizer(     lineSearchOptimizer     );
  } 
  else
  {
    optimizer     = OptimizerType::New();
    if(metricType == "MI")
    {
      optimizer->MaximizeOn();
    }
    else
    {
      optimizer->MaximizeOff();
    }
    registration->SetOptimizer(     optimizer     );
  }

  //Set the metric type
  MutualInformationMetricType::Pointer         mutualInformationMetric;
  if(metricType == "MI")
  {
    mutualInformationMetric        = MutualInformationMetricType::New();
    registration->SetMetric( mutualInformationMetric  );
    mutualInformationMetric->SetFixedImageStandardDeviation(  parzenWindowStandardDeviation );
    mutualInformationMetric->SetMovingImageStandardDeviation( parzenWindowStandardDeviation );
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
  ImageArrayReader   imageArrayReader(N);
  
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
  GaussianFilterArrayType gaussianFilterArray(N);
  // Begin the registration with the affine transform
  // Connect the compenents together
  //
  // for all input images
  // create a separate tranform, interpolater, image reader,
  // normalized filter, gaussian filter and connect those components
  //

  // Add probe to count the time used by the registration
  itk::TimeProbesCollectorBase collector;
  
  collector.Start( "1Image Read " );
  collector.Start( "6Total Time " );

  try
  {

    for( int i=0; i< N; i++ )
    {

      if(imageType == "DICOM")
      {
        imageIOTypeArray[i] = ImageIOType::New();
        namesGeneratorArray[i] = NamesGeneratorType::New();
        namesGeneratorArray[i]->SetInputDirectory( inputFileNames[i].c_str() );

        std::cout << "message: Reading first slice " << namesGeneratorArray[i]->GetInputFileNames()[0] << std:: endl;
        std::cout << "message: Reading " << namesGeneratorArray[i]->GetInputFileNames().size() << " DICOM slices" << std::endl;

        dicomArrayReader[i] = DICOMReaderType::New();
        dicomArrayReader[i]->SetImageIO( imageIOTypeArray[i] );
        dicomArrayReader[i]->SetFileNames( namesGeneratorArray[i]->GetInputFileNames() );

      }
      else
      {
        imageArrayReader[i] = ImageReaderType::New();
        imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
      }

      //Initialize mask filters
      ConnectedThresholdImageFilterType::Pointer connectedThreshold;
      NeighborhoodConnectedImageFilterType::Pointer neighborhoodConnected;

      //if mask is single only mask the first image
      //else mask all images
      if(maskType == "connectedThreshold" && ((mask == "single" && i==0 ) || mask != "single"))
      {
        connectedThreshold =ConnectedThresholdImageFilterType::New();
        if( imageType == "DICOM")
        {
          connectedThreshold->SetInput( dicomArrayReader[i]->GetOutput() );
        }
        else
        {
          connectedThreshold->SetInput( imageArrayReader[i]->GetOutput() );
        }
        ConnectedThresholdImageFilterType::IndexType seed;
        seed.Fill(0);
        connectedThreshold->AddSeed(seed);

        connectedThreshold->SetLower(0);
        connectedThreshold->SetUpper(9);
        
        connectedThreshold->ReleaseDataFlagOn();
        connectedThreshold->Update();
        
        ImageMaskSpatialObject::Pointer maskImage = ImageMaskSpatialObject::New();
        maskImage->SetImage(connectedThreshold->GetOutput());

        if(metricType == "MI")
        {
          if(i==0)
          {
            mutualInformationMetric->SetFixedImageMask( maskImage );
          }
          else
          {
            mutualInformationMetric->SetMovingImageMask( maskImage );
          }
        }

      }
      else if( maskType == "neighborhoodConnected" && ((mask == "single" && i==0 ) || mask != "single"))
      {
        neighborhoodConnected = NeighborhoodConnectedImageFilterType::New();
        if( imageType == "DICOM")
        {
          neighborhoodConnected->SetInput( dicomArrayReader[i]->GetOutput() );
        }
        else
        {
          neighborhoodConnected->SetInput( imageArrayReader[i]->GetOutput() );
        }
        NeighborhoodConnectedImageFilterType::IndexType seed;
  
        seed.Fill(0);
        neighborhoodConnected->SetSeed(seed);
  
        neighborhoodConnected->SetLower (0);
        neighborhoodConnected->SetUpper (20);
        
        typedef NeighborhoodConnectedImageFilterType::InputImageSizeType SizeType;
        SizeType radius;
        radius.Fill(1);
        neighborhoodConnected->SetRadius(radius);
        
        neighborhoodConnected->ReleaseDataFlagOn();
        neighborhoodConnected->Update();
        
        ImageMaskSpatialObject::Pointer maskImage = ImageMaskSpatialObject::New();
        maskImage->SetImage(neighborhoodConnected->GetOutput());

        if(metricType == "MI")
        {
          if(i==0)
          {
            mutualInformationMetric->SetFixedImageMask( maskImage );
          }
          else
          {
            mutualInformationMetric->SetMovingImageMask( maskImage );
          }
        }

      }

      NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
      if( imageType == "DICOM")
      {
        normalizeFilter->SetInput( dicomArrayReader[i]->GetOutput() );
      }
      else
      {
        normalizeFilter->SetInput( imageArrayReader[i]->GetOutput() );
      }

      gaussianFilterArray[i] = GaussianFilterType::New();
      gaussianFilterArray[i]->SetVariance( gaussianFilterVariance );
      gaussianFilterArray[i]->SetInput( normalizeFilter->GetOutput() );
      gaussianFilterArray[i]->Update();
      if(i==0)
      {
        registration->SetFixedImage(    gaussianFilterArray[i]->GetOutput()    );
      }
      else
      {
        registration->SetMovingImage(   gaussianFilterArray[i]->GetOutput()   );
      }
    }
    
    translationTransformArray[1] = TranslationTransformType::New();
    interpolatorArray[1]  = InterpolatorType::New();
    registration->SetTransform(     translationTransformArray[1]     );
    registration->SetInterpolator(     interpolatorArray[1]     );
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }


  // Set initial parameters of the transform
  ImageType::RegionType fixedImageRegion =
      gaussianFilterArray[0]->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );


  // Allocate the space for tranform parameters used by registration method
  // We use a large array to concatenate the parameter array of each tranform
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( translationTransformArray[1]->GetNumberOfParameters() );
  initialParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialParameters );


  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( translationTransformArray[1]->GetNumberOfParameters() );
  optimizerScales.Fill(1.0);


  // Get the number of pixels (voxels) in the images
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  unsigned int numberOfSamples = numberOfSpatialSamplesTranslation;
  if( numberOfSpatialSamplesTranslationPercentage > 0 )
  {
    numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesTranslationPercentage );
  }


  //Set the number of samples
  if(metricType == "MI")
  {
    mutualInformationMetric->SetNumberOfSpatialSamples( numberOfSamples );
  }


  // Create the Command observer and register it with the optimizer.
  // And set output file name
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  //observer->SetFilename("iterations.txt");
  
  // Set the optimizer parameters
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optTranslationLearningRate);
    lineSearchOptimizer->SetMaximumIteration( optTranslationNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( maximumLineIteration );
    lineSearchOptimizer->SetScales( optimizerScales );
    lineSearchOptimizer->AddObserver( itk::IterationEvent(), observer );
  } 
  else
  {
    optimizer->SetLearningRate( optTranslationLearningRate );
    optimizer->SetNumberOfIterations( optTranslationNumberOfIterations );
    optimizer->SetScales( optimizerScales );
    optimizer->AddObserver( itk::IterationEvent(), observer );
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

  std::cout << "message: Starting Registration " << std::endl;

  // Add probe to count the time used by the registration
  collector.Stop( "1Image Read " );



  // Start registration with translation transform
  try 
  {
    collector.Start( "2Translation Reg." );
    registration->StartRegistration();
    collector.Stop( "2Translation Reg." );
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return -1;
  }

  //
  //
  //
  //
  //
  //    AFFINE TRANSFORM REGISTRATION
  //
  //
  //
  //
  //

  
  // Continue with affine transform using the results of the translation transform
  //
  //Get the latest parameters from the registration
  ParametersType translationParameters = registration->GetLastTransformParameters();
  
  affineTransformArray[1]     = TransformType::New();
  registration->SetTransform(     affineTransformArray[1]    );


  //int numberOfParameters = affineTransformArray[1]->GetNumberOfParameters();

  // Initialize the affine transforms to identity transform
  // And use the results of the translation transform

  affineTransformArray[1]->SetIdentity();
  TransformType::InputPointType center;
  // Get spacing, origin and size of the images
  ImageType::SpacingType spacing = gaussianFilterArray[1]->GetOutput()->GetSpacing();
  itk::Point<double, Dimension> origin = gaussianFilterArray[1]->GetOutput()->GetOrigin();
  ImageType::SizeType size = gaussianFilterArray[1]->GetOutput()->GetLargestPossibleRegion().GetSize();

  // Place the center of rotation to the center of the image
  for(int j=0; j< Dimension; j++)
  {
    center[j] = origin[j] + spacing[j]*size[j] / 2.0;
  }
  affineTransformArray[1]->SetCenter(center);

  //Initialize the translation parameters using the results of the tranlation transform
  ParametersType affineParameters = affineTransformArray[1]->GetParameters();
  for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
  {
    affineParameters[j] = translationParameters[j-Dimension*Dimension]; // scale for translation on X,Y,Z
  }
  affineTransformArray[1]->SetParametersByValue(affineParameters);

  registration->SetInitialTransformParameters( affineTransformArray[1]->GetParameters() );


  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  OptimizerScalesType optimizerAffineScales( affineTransformArray[1]->GetNumberOfParameters());
  optimizerAffineScales.Fill(1.0);
  for( int j=0; j<Dimension*Dimension; j++ )
  {
    optimizerAffineScales[j] = 1.0; // scale for indices in 2x2 (3x3) Matrix
  }
  for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
  {
    optimizerAffineScales[j] = translationScaleCoeffs; // scale for translation on X,Y,Z
  }


  // Set the optimizer parameters
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optAffineLearningRate);
    lineSearchOptimizer->SetMaximumIteration( optAffineNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( 10 );
    lineSearchOptimizer->SetScales( optimizerAffineScales );
  }
  else
  {
    optimizer->SetLearningRate( optAffineLearningRate );
    optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
    optimizer->SetScales( optimizerAffineScales );
  }

  // Set the number of spatial samples for the metric
  numberOfSamples = numberOfSpatialSamplesAffine;
  if( numberOfSpatialSamplesAffinePercentage > 0 )
  {
    numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesAffinePercentage );
  }
  if(metricType == "MI")
  {
    mutualInformationMetric->SetNumberOfSpatialSamples( numberOfSamples );
  }

  
  //Set the parameters of the command observer
  command->SetMultiScaleSamplePercentageIncrease(affineMultiScaleSamplePercentageIncrease);
  command->SetMultiScaleMaximumIterationIncrease(affineMultiScaleMaximumIterationIncrease);
  command->SetMultiScaleStepLengthIncrease(affineMultiScaleStepLengthIncrease);
  
  // Start registration with Affine Transform
  try
  {
    collector.Start( "3Affine Reg." );
    registration->StartRegistration();
    collector.Stop( "3Affine Reg." );
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }

  //
  // BSpline Registration
  //
  // We supply the affine transform from the first part as initial parameters to the Bspline
  // registration. As the user might want to increase the resolution of the Bspline grid
  // we begin by declaring the Bspline parameters for the low resolution

  collector.Start( "4Bspline Reg." );

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
    affineTransformArray[1]->SetParametersByValue(registration->GetLastTransformParameters());


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
      bsplineTransformArrayLow[1] = BSplineTransformType::New();
      
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
        
      spacing = gaussianFilterArray[1]->GetOutput()->GetSpacing();
      origin = gaussianFilterArray[1]->GetOutput()->GetOrigin();
      fixedRegion = gaussianFilterArray[1]->GetOutput()->GetBufferedRegion();


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
      bsplineTransformArrayLow[1]->SetGridSpacing( spacing );
      bsplineTransformArrayLow[1]->SetGridOrigin( origin );
      bsplineTransformArrayLow[1]->SetGridRegion( bsplineRegion );


      unsigned int numberOfParametersLow =
                      bsplineTransformArrayLow[1]->GetNumberOfParameters();

      // Set the initial Bspline parameters to zero
      bsplineParametersArrayLow[1].SetSize( numberOfParametersLow );
      bsplineParametersArrayLow[1].Fill( 0.0 );

      // Set the affine tranform and initial paramters of Bsplines
      bsplineTransformArrayLow[1]->SetBulkTransform(affineTransformArray[1]);
      bsplineTransformArrayLow[1]->SetParameters( bsplineParametersArrayLow[1] );
      
      // register Bspline pointers with the registration method
      registration->SetInitialTransformParameters( bsplineTransformArrayLow[1]->GetParameters() );
      registration->SetTransform(     bsplineTransformArrayLow[1] );
      if(metricType == "MI")
      {
        if(useBSplineRegularization == "on")
        {
          mutualInformationMetric->SetBSplineTransform(     bsplineTransformArrayLow[1] );
          mutualInformationMetric->SetRegularization(true);
          mutualInformationMetric->SetRegularizationFactor(bsplineRegularizationFactor);
        }
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
    optimizerScales.SetSize( bsplineTransformArrayLow[1]->GetNumberOfParameters());
    optimizerScales.Fill( 1.0 );
    if(optimizerType == "lineSearch")
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
    numberOfSamples = numberOfSpatialSamplesBspline;
    if( numberOfSpatialSamplesBsplinePercentage > 0 )
    {
      numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesBsplinePercentage );
    }
    if(metricType == "MI")
    {
      mutualInformationMetric->SetNumberOfSpatialSamples( numberOfSamples );
    }


    registration->SetNumberOfLevels( multiLevelBspline );

    //Set the parameters of the command observer
    command->SetMultiScaleSamplePercentageIncrease(bsplineMultiScaleSamplePercentageIncrease);
    command->SetMultiScaleMaximumIterationIncrease(bsplineMultiScaleMaximumIterationIncrease);
    command->SetMultiScaleStepLengthIncrease(bsplineMultiScaleStepLengthIncrease);

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
    
        // Update the last transform parameters
        bsplineTransformArrayLow[1]->SetParametersByValue( registration->GetLastTransformParameters() );



        // Increase the grid size by a factor of two
        bsplineInitialGridSize = 2*(bsplineInitialGridSize+SplineOrder)-SplineOrder;


        // Set the parameters of the high resolution Bspline Transform
        bsplineTransformArrayHigh[1] = BSplineTransformType::New();

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

        spacingHigh = gaussianFilterArray[1]->GetOutput()->GetSpacing();
        originHigh  = gaussianFilterArray[1]->GetOutput()->GetOrigin();
        fixedRegion = gaussianFilterArray[1]->GetOutput()->GetBufferedRegion();

        ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
    
        for(unsigned int rh=0; rh<Dimension; rh++)
        {
          //There was a floor here, is it a BUG?
          spacingHigh[rh] *= floor( static_cast<double>(fixedImageSize[rh] - 1)  /
              static_cast<double>(gridHighSizeOnImage[rh] - 1) );
          originHigh[rh]  -=  spacingHigh[rh];
        }

        bsplineTransformArrayHigh[1]->SetGridSpacing( spacingHigh );
        bsplineTransformArrayHigh[1]->SetGridOrigin( originHigh );
        bsplineTransformArrayHigh[1]->SetGridRegion( bsplineRegion );

        bsplineParametersArrayHigh[1].SetSize( bsplineTransformArrayHigh[1]->GetNumberOfParameters() );
        bsplineParametersArrayHigh[1].Fill( 0.0 );


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

          upsampler->SetInput( bsplineTransformArrayLow[1]->GetCoefficientImage()[k] );
          upsampler->SetInterpolator( function );
          upsampler->SetTransform( identity );
          upsampler->SetSize( bsplineTransformArrayHigh[1]->GetGridRegion().GetSize() );
          upsampler->SetOutputSpacing( bsplineTransformArrayHigh[1]->GetGridSpacing() );
          upsampler->SetOutputOrigin( bsplineTransformArrayHigh[1]->GetGridOrigin() );

          typedef itk::BSplineDecompositionImageFilter<ParametersImageType,ParametersImageType>
              DecompositionType;
          DecompositionType::Pointer decomposition = DecompositionType::New();

          decomposition->SetSplineOrder( SplineOrder );
          decomposition->SetInput( upsampler->GetOutput() );
          decomposition->Update();

          ParametersImageType::Pointer newCoefficients = decomposition->GetOutput();

          // copy the coefficients into the parameter array
          typedef itk::ImageRegionIterator<ParametersImageType> Iterator;
          Iterator it( newCoefficients, bsplineTransformArrayHigh[1]->GetGridRegion() );
          while ( !it.IsAtEnd() )
          {
            bsplineParametersArrayHigh[1][ counter++ ] = it.Get();
            ++it;
          }

        }

        bsplineTransformArrayHigh[1]->SetParameters( bsplineParametersArrayHigh[1] );

        // Set parameters of the fine grid Bspline transform
        // to coarse grid Bspline transform for the next level
        bsplineTransformArrayLow[1]->SetGridSpacing( spacingHigh );
        bsplineTransformArrayLow[1]->SetGridOrigin( originHigh );
        bsplineTransformArrayLow[1]->SetGridRegion( bsplineRegion );


        // Set initial parameters of the registration
        registration->SetInitialTransformParameters( bsplineTransformArrayHigh[1]->GetParameters() );
        registration->SetTransform( bsplineTransformArrayHigh[1] );

        // Set the regularization term
        if(metricType == "MI")
        {
          if(useBSplineRegularization == "on")
          {
            mutualInformationMetric->SetBSplineTransform(     bsplineTransformArrayHigh[1] );
            mutualInformationMetric->SetRegularization(true);
            mutualInformationMetric->SetRegularizationFactor(bsplineRegularizationFactor);
          }
        }
        
        // Decrease the learning rate at each level
        // Reset the optimizer scales
        typedef OptimizerType::ScalesType       OptimizerScalesType;
        OptimizerScalesType optimizerScales( bsplineTransformArrayHigh[1]->GetNumberOfParameters() );
        optimizerScales.Fill( 1.0 );

        if(optimizerType == "lineSearch")
        {
          lineSearchOptimizer->SetScales( optimizerScales );
          lineSearchOptimizer->SetStepLength(optBsplineHighLearningRate);
          lineSearchOptimizer->SetMaximumIteration( optBsplineHighNumberOfIterations );
          lineSearchOptimizer->SetScales( optimizerScales );
        }
        else
        {
          optimizer->SetScales( optimizerScales );
          optimizer->SetLearningRate( optBsplineHighLearningRate );
          optimizer->SetNumberOfIterations( optBsplineHighNumberOfIterations );
        }


        // Set the number of samples to be used by the metric
        numberOfSamples = numberOfSpatialSamplesBsplineHigh;
        if( numberOfSpatialSamplesBsplineHighPercentage > 0 )
        {
          numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesBsplineHighPercentage );
        }
        if(metricType == "MI")
        {
          mutualInformationMetric->SetNumberOfSpatialSamples( numberOfSamples );
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
  collector.Stop( "4Bspline Reg." );

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
  if(optimizerType == "lineSearch")
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
  std::cout << "message: Registration Completed " << std::endl;

  // Get the time for the registration
  collector.Start( "5Image Write " );


  // typedefs for output images
  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            InternalImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  ImageType::Pointer fixedImage;
  
  typedef  PixelType  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        InternalImageType,
                        OutputImageType > CastFilterType;
  typedef itk::CastImageFilter< 
                        ImageType,
                        InternalImageType > ImageCastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;  

  //DICOM writer type definitions
  typedef itk::Image< OutputPixelType, 2 >    Image2DType;
  typedef itk::ImageSeriesWriter< ImageType, Image2DType >  SeriesWriterType;
  SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  // Extract slices writer type
  typedef itk::Image< unsigned char, 2 >    SliceImageType;
  typedef itk::ImageFileWriter< SliceImageType >  SliceWriterType;
  SliceWriterType::Pointer  sliceWriter = SliceWriterType::New();
  // Filter to extract a slice from an image
  typedef itk::ExtractImageFilter< ImageType, SliceImageType > SliceExtractFilterType;
  SliceExtractFilterType::Pointer sliceExtractFilter = SliceExtractFilterType::New();

  // typedefs for output images
  typedef itk::ResampleImageFilter< Image2DType, Image2DType >    SliceResampleFilterType;
  SliceResampleFilterType::Pointer sliceResample = SliceResampleFilterType::New();

  
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster = CastFilterType::New();

  ImageCastFilterType::Pointer  imageCaster = ImageCastFilterType::New();
  ImageCastFilterType::Pointer  imageCaster2 = ImageCastFilterType::New();

  // Update last Transform Parameters
  // and write the last tranform parameters to an output file
  string parametersFolder("RegistrationParameters/");
  parametersFolder = outputFolder + parametersFolder;
  itksys::SystemTools::MakeDirectory( parametersFolder.c_str() );

  std::cout << "message: Writing result parameters " << std::endl;

  //Print the resulting parameters
  string outputFname(fileNames[1]);
  outputFname[outputFname.size()-4] = '.';
  outputFname[outputFname.size()-3] = 't';
  outputFname[outputFname.size()-2] = 'x';
  outputFname[outputFname.size()-1] = 't';
  outputFname = parametersFolder + outputFname;
  ofstream outputFile(outputFname.c_str());

  if(useBsplineHigh == "on")
  {
    bsplineTransformArrayHigh[1]->SetBulkTransform( affineTransformArray[1] );
    bsplineTransformArrayHigh[1]->SetParametersByValue( finalParameters );

    outputFile << "Bspline: " << std::endl;
    outputFile << "GridSize: " << bsplineTransformArrayHigh[1]->GetGridRegion().GetSize() << std::endl;
    outputFile << "Parameters:" << std::endl << finalParameters << std::endl;
  }
  else if (useBspline == "on")
  {
    bsplineTransformArrayLow[1]->SetBulkTransform( affineTransformArray[1] );
    bsplineTransformArrayLow[1]->SetParametersByValue( finalParameters );

    outputFile << "Bspline: " << std::endl;
    outputFile << "GridSize: " << bsplineTransformArrayLow[1]->GetGridRegion().GetSize() << std::endl;
    outputFile << "Parameters:" << std::endl << finalParameters << std::endl;

  }
  else
  {
    affineTransformArray[1]->SetParametersByValue( finalParameters );
  }

  outputFile << "Affine: " << std::endl;
  outputFile << affineTransformArray[1]->GetParameters() << std::endl;
  outputFile.close();

  


  
  // Create a toy image for deformation Field visualization
  ImageType::Pointer deformationImage = ImageType::New();


  // Loop over images and write output images
  //Set the correct tranform
  if(useBsplineHigh == "on")
  {
    resample->SetTransform( bsplineTransformArrayHigh[1] );
  }
  else if (useBspline == "on")
  {
    resample->SetTransform( bsplineTransformArrayLow[1] );
  }
  else
  {
    resample->SetTransform( affineTransformArray[1] );
  }

    
  if( imageType == "DICOM")
  {
    resample->SetInput( dicomArrayReader[1]->GetOutput() );
    fixedImage = dicomArrayReader[1]->GetOutput();
  }
  else
  {
    resample->SetInput( imageArrayReader[1]->GetOutput() );
    fixedImage = imageArrayReader[1]->GetOutput();
  }
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection());
  resample->SetDefaultPixelValue( 0 );

  std::cout << "message: Writing " << outputFileNames[1] << std::endl;
  if( imageType == "DICOM")
  {
    itksys::SystemTools::MakeDirectory( outputFileNames[1].c_str() );
    caster->SetInput(resample->GetOutput());
    seriesWriter->SetInput( caster->GetOutput() );
    seriesWriter->SetImageIO( imageIOTypeArray[1] );
    namesGeneratorArray[1]->SetOutputDirectory( outputFileNames[1].c_str() );
    seriesWriter->SetFileNames( namesGeneratorArray[1]->GetOutputFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[1]->GetMetaDataDictionaryArray() );
      
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
    string registeredImagesFolder("RegisteredImages/");
    registeredImagesFolder = outputFolder + registeredImagesFolder;
    itksys::SystemTools::MakeDirectory( registeredImagesFolder.c_str() );
    outputFileNames[1] = registeredImagesFolder + fileNames[1];
      
    caster->SetInput( resample->GetOutput() );
    writer->SetImageIO(imageArrayReader[1]->GetImageIO());
    writer->SetFileName( outputFileNames[1].c_str() );
    writer->SetInput( caster->GetOutput()   );
    if(writeOutputImages == "on")
    {
      writer->Update();
    }

    //Extract slices for 3D Images
    if(Dimension == 3)
    {
      //Write the registered images
      string slices("RegisteredSlices/");
      slices = outputFolder + slices;
      string outputFilename(fileNames[1]);
      outputFilename[outputFilename.size()-4] = '.';
      outputFilename[outputFilename.size()-3] = 'p';
      outputFilename[outputFilename.size()-2] = 'n';
      outputFilename[outputFilename.size()-1] = 'g';
      outputFilename = slices + outputFilename;
      itksys::SystemTools::MakeDirectory( slices.c_str() );
      
      OutputImageType::SizeType size = fixedImage->GetLargestPossibleRegion().GetSize();
      OutputImageType::IndexType start = fixedImage->GetLargestPossibleRegion().GetIndex();
      start[2] = size[2]/2;
      size[2] = 0;

      OutputImageType::RegionType extractRegion;
      extractRegion.SetSize(  size  );
      extractRegion.SetIndex( start );
      sliceExtractFilter->SetExtractionRegion( extractRegion );

      caster->SetInput(resample->GetOutput());
      sliceExtractFilter->SetInput( caster->GetOutput() );
      sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
      sliceWriter->SetFileName( outputFilename.c_str() );
      sliceWriter->Update();

      for(int i=0; i<N; i++)
      {
        //Write the original images
        string slices2("OriginalSlices/");
        slices2 = outputFolder + slices2;
        string outputFilename2(fileNames[i]);
        outputFilename2[outputFilename2.size()-4] = '.';
        outputFilename2[outputFilename2.size()-3] = 'p';
        outputFilename2[outputFilename2.size()-2] = 'n';
        outputFilename2[outputFilename2.size()-1] = 'g';
        outputFilename2 = slices2 + outputFilename2;
        itksys::SystemTools::MakeDirectory( slices2.c_str() );

        sliceExtractFilter->SetInput( imageArrayReader[i]->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
        sliceWriter->SetFileName( outputFilename2.c_str() );
        sliceWriter->Update();
      }


      // Write the deformation fields for Bsplines
      string defName("DeformationFields3D/");
      defName = outputFolder + defName;
      itksys::SystemTools::MakeDirectory( defName.c_str() );
        
      //Create the toy image

      deformationImage->SetRegions( imageArrayReader[1]->GetOutput()->GetLargestPossibleRegion() );
      deformationImage->CopyInformation( imageArrayReader[1]->GetOutput() );
      deformationImage->Allocate();
      deformationImage->FillBuffer( 0 );
          
      // Create perpendicular planes in the deformationImage
      typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorWithIndexType;
      IteratorWithIndexType defIt( deformationImage, deformationImage->GetRequestedRegion() );
      ImageType::IndexType index;
      for ( defIt.GoToBegin(); !defIt.IsAtEnd(); ++defIt)
      {
        index = defIt.GetIndex();
        if(index[0]%8 == 0 || index[1]%8 ==0 || index[2]%8 ==4)
        {
          defIt.Set( 255 );
        }
      }


      resample->SetInput( deformationImage );

      // Generate the outputfilename and write the output
      string defFile(fileNames[1]);
      defFile = defName + defFile;
      writer->SetFileName( defFile.c_str() );
      if(writeDeformationFields =="on")
      {
        writer->Update();
      }

      // Extract the central slices of the the deformation field
      string defName2D("DeformationFields2D/");
      defName2D = outputFolder + defName2D;
      itksys::SystemTools::MakeDirectory( defName2D.c_str() );
      string defFile2D(fileNames[1]);
      defFile2D[defFile2D.size()-4] = '.';
      defFile2D[defFile2D.size()-3] = 'p';
      defFile2D[defFile2D.size()-2] = 'n';
      defFile2D[defFile2D.size()-1] = 'g';
      defFile2D = defName2D + defFile2D;
      sliceExtractFilter->SetInput( caster->GetOutput() );
      sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
      sliceWriter->SetFileName( defFile2D.c_str() );
      sliceWriter->Update();
      
    }
  }
  

  std::cout << "message: Computing mean images" << std::endl;

  // Compute Mean Images 
  typedef itk::AddImageFilter < InternalImageType, InternalImageType,
                                           InternalImageType > AddFilterType;

  //Mean of the registered images
  AddFilterType::Pointer addition = AddFilterType::New();
  // Mean Image of original images
  AddFilterType::Pointer addition2 = AddFilterType::New();


  //Set the first image
  if(useBsplineHigh == "on")
  {
    resample->SetTransform( bsplineTransformArrayHigh[1] );
  }
  else if (useBspline == "on")
  {
    resample->SetTransform( bsplineTransformArrayLow[1] );
  }
  else
  {
    resample->SetTransform( affineTransformArray[1] );
  }

  if( imageType == "DICOM")
  {
    resample->SetInput( dicomArrayReader[1]->GetOutput() );
    fixedImage = dicomArrayReader[1]->GetOutput();
  }
  else
  {
    resample->SetInput( imageArrayReader[1]->GetOutput() );
    fixedImage = imageArrayReader[1]->GetOutput();
  }
  
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection());
  resample->SetDefaultPixelValue( 0 );
  addition->SetInput2( resample->GetOutput() );

  //Set the second image
  if( imageType == "DICOM")
  {
    imageCaster->SetInput(dicomArrayReader[0]->GetOutput());
  }
  else
  {
    imageCaster->SetInput(imageArrayReader[0]->GetOutput());
  }

  addition->SetInput1( imageCaster->GetOutput() );
  addition2->SetInput1(imageCaster->GetOutput() );

  if( imageType == "DICOM")
  {
    imageCaster2->SetInput(dicomArrayReader[1]->GetOutput());
  }
  else
  {
    imageCaster2->SetInput(imageArrayReader[1]->GetOutput());
  }
  addition2->SetInput2(imageCaster2->GetOutput() );

  //addition->Update();
  //addition2->Update();




  //Write the mean image
  typedef itk::RescaleIntensityImageFilter< InternalImageType, InternalImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  RescalerType::Pointer intensityRescaler2 = RescalerType::New();
  WriterType::Pointer      writer2 =  WriterType::New();
  WriterType::Pointer      writer3 =  WriterType::New();

  intensityRescaler->SetInput( addition->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );
  
  intensityRescaler2->SetInput( addition2->GetOutput() );
  intensityRescaler2->SetOutputMinimum(   0 );
  intensityRescaler2->SetOutputMaximum( 255 );
  if( imageType == "DICOM")
  {
    string meanImageFname = outputFolder + "MeanRegisteredImage";
    itksys::SystemTools::MakeDirectory( meanImageFname.c_str() );
    caster->SetInput( intensityRescaler->GetOutput() );
    seriesWriter->SetInput( caster->GetOutput() );
    seriesWriter->SetImageIO( imageIOTypeArray[0] );
    namesGeneratorArray[0]->SetOutputDirectory( meanImageFname.c_str() );
    seriesWriter->SetFileNames( namesGeneratorArray[0]->GetOutputFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[0]->GetMetaDataDictionaryArray() );
    seriesWriter->Update();

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
      //Write the registered images
      // in the format of the input images
      string meanImages("MeanImages/");
      meanImageFname = outputFolder + meanImages + "MeanRegisteredImage.";
      meanImageFname = meanImageFname + inputFileNames[0][inputFileNames[0].size()-3] +
          inputFileNames[0][inputFileNames[0].size()-2] +inputFileNames[0][inputFileNames[0].size()-1];
      
      meanImages = outputFolder + meanImages;
      itksys::SystemTools::MakeDirectory( meanImages.c_str() );

      vector<string> outputFilenames(Dimension);
      outputFilenames[0] = "MeanRegisteredSlice1.png";
      outputFilenames[1] = "MeanRegisteredSlice2.png";
      outputFilenames[2] = "MeanRegisteredSlice3.png";

      for(int index=0; index<Dimension; index++)
      {
        outputFilenames[index] = meanImages + outputFilenames[index];

        OutputImageType::SizeType size = fixedImage->GetLargestPossibleRegion().GetSize();
        OutputImageType::IndexType start = fixedImage->GetLargestPossibleRegion().GetIndex();
        start[index] = size[index]/2;
        size[index] = 0;
        
        OutputImageType::RegionType extractRegion;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );

        caster->SetInput(intensityRescaler->GetOutput());
        sliceExtractFilter->SetInput( caster->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
        sliceWriter->SetFileName( outputFilenames[index].c_str() );
        sliceWriter->Update();
      }

    }
    writer2->SetImageIO(imageArrayReader[0]->GetImageIO());
    writer2->SetFileName( meanImageFname.c_str() );
    writer2->Update();
  }

  if( imageType == "DICOM")
  {
    string meanImageFname = outputFolder + "MeanOriginalImage";
    itksys::SystemTools::MakeDirectory( meanImageFname.c_str() );
    caster->SetInput(intensityRescaler2->GetOutput());
    seriesWriter->SetInput( caster->GetOutput() );
    seriesWriter->SetImageIO( imageIOTypeArray[0] );
    namesGeneratorArray[0]->SetOutputDirectory( meanImageFname.c_str() );
    seriesWriter->SetFileNames( namesGeneratorArray[0]->GetOutputFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( dicomArrayReader[0]->GetMetaDataDictionaryArray() );
    seriesWriter->Update();

  }
  else
  {
    caster->SetInput( intensityRescaler2->GetOutput() );
    writer3->SetInput( caster->GetOutput()   );

    string meanImageFname;
    if (Dimension == 2)
    {
      meanImageFname = outputFolder + "MeanOriginalImage.png";
    }
    else
    {
      //Write the original images
      // in the format of the given images
      string meanImages("MeanImages/");
      meanImageFname = outputFolder + meanImages + "MeanOriginalImage.";
      meanImageFname = meanImageFname + inputFileNames[0][inputFileNames[0].size()-3] +
          inputFileNames[0][inputFileNames[0].size()-2] +inputFileNames[0][inputFileNames[0].size()-1];

      meanImages = outputFolder + meanImages;

      vector<string> outputFilenames(Dimension);
      outputFilenames[0] = "MeanOriginalSlice1.png";
      outputFilenames[1] = "MeanOriginalSlice2.png";
      outputFilenames[2] = "MeanOriginalSlice3.png";

      for(int index=0; index<Dimension; index++)
      {
        outputFilenames[index] = meanImages + outputFilenames[index];

        OutputImageType::SizeType size = fixedImage->GetLargestPossibleRegion().GetSize();
        OutputImageType::IndexType start = fixedImage->GetLargestPossibleRegion().GetIndex();
        start[index] = size[index]/2;
        size[index] = 0;
        
        OutputImageType::RegionType extractRegion;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );

        caster->SetInput(intensityRescaler2->GetOutput());
        sliceExtractFilter->SetInput( caster->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
        sliceWriter->SetFileName( outputFilenames[index].c_str() );
        sliceWriter->Update();
      }
    }
    writer3->SetImageIO(imageArrayReader[1]->GetImageIO());
    writer3->SetFileName( meanImageFname.c_str() );
    writer3->Update();
  }

  std::cout << "message: Time Report " << std::endl;
  collector.Stop( "5Image Write " );
  collector.Stop( "6Total Time " );
  collector.Report();


  return 0;
}




int getCommandLine(       int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,
                          int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                          double& optTranslationLearningRate, double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                          int& optTranslationNumberOfIterations, int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                          double& numberOfSpatialSamplesTranslationPercentage, double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          
                          string& useBSplineRegularization, double& bsplineRegularizationFactor,

                          string& transformType, string& imageType,string& metricType,
                          string& useBspline, string& useBsplineHigh,
                          
                          double& translationScaleCoeffs,double& gaussianFilterVariance,
                          int& maximumLineIteration,   double& parzenWindowStandardDeviation,
                          
                          double& translationMultiScaleSamplePercentageIncrease, double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,

                          double& translationMultiScaleMaximumIterationIncrease, double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,

                          double& translationMultiScaleStepLengthIncrease, double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease,
                          unsigned int& numberOfSpatialSamplesTranslation, unsigned int& numberOfSpatialSamplesAffine, unsigned int& numberOfSpatialSamplesBspline, unsigned int& numberOfSpatialSamplesBsplineHigh,

                          string &mask, string& maskType, unsigned int& threshold1, unsigned int threshold2,
                          string &writeOutputImages, string &writeDeformationFields )
{


  ifstream initFile(initFname);
  if( initFile.fail() )
  {
    std::cout << "could not open file: " << initFname << std::endl;
    return 1;
  }

  while( !initFile.eof() )
  {
    
    string dummy;
    initFile >> dummy;

    if(dummy == "-i")
    {
      initFile >> dummy;
      inputFolder = dummy;
    }
    else if (dummy == "-o")
    {
      initFile >> dummy;
      outputFolder = dummy;
    }
    else if (dummy == "-opt")
    {
      initFile >> dummy;
      optimizerType == dummy;
    }
    else if (dummy == "-t")
    {
      initFile >> dummy;
      optimizerType == dummy;
    }
    
    else if (dummy == "-imageType")
    {
      initFile >> dummy;
      imageType = dummy;
    }
    else if (dummy == "-metricType")
    {
      initFile >> dummy;
      metricType = dummy;
    }
    else if (dummy == "-optimizerType")
    {
      initFile >> dummy;
      optimizerType = dummy;
    }
    
    else if (dummy == "-multiLevelAffine")
    {
      initFile >> dummy;
      multiLevelAffine = atoi(dummy.c_str());
    }
    else if (dummy == "-multiLevelBspline")
    {
      initFile >> dummy;
      multiLevelBspline = atoi(dummy.c_str());
    }
    else if (dummy == "-multiLevelBsplineHigh")
    {
      initFile >> dummy;
      multiLevelBsplineHigh = atoi(dummy.c_str());
    }
    
    else if (dummy == "-optTranslationLearningRate")
    {
      initFile >> dummy;
      optTranslationLearningRate = atof(dummy.c_str());
    }
    else if (dummy == "-optAffineLearningRate")
    {
      initFile >> dummy;
      optAffineLearningRate = atof(dummy.c_str());
    }
    else if (dummy == "-optBsplineLearningRate")
    {
      initFile >> dummy;
      optBsplineLearningRate = atof(dummy.c_str());
    }
    else if (dummy == "-optBsplineHighLearningrate")
    {
      initFile >> dummy;
      optBsplineHighLearningRate = atof(dummy.c_str());
    }

    else if (dummy == "-optTranslationNumberOfIterations")
    {
      initFile >> dummy;
      optTranslationNumberOfIterations = atoi(dummy.c_str());
    }
    else if (dummy == "-optAffineNumberOfIterations")
    {
      initFile >> dummy;
      optAffineNumberOfIterations = atoi(dummy.c_str());
    }
    else if (dummy == "-optBsplineNumberOfIterations")
    {
      initFile >> dummy;
      optBsplineNumberOfIterations = atoi(dummy.c_str());
    }
    else if (dummy == "-optBsplineHighNumberOfIterations")
    {
      initFile >> dummy;
      optBsplineHighNumberOfIterations = atoi(dummy.c_str());
    }

    else if (dummy == "-numberOfSpatialSamplesTranslationPercentage")
    {
      initFile >> dummy;
      numberOfSpatialSamplesTranslationPercentage = atof(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesAffinePercentage")
    {
      initFile >> dummy;
      numberOfSpatialSamplesAffinePercentage = atof(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesBsplinePercentage")
    {
      initFile >> dummy;
      numberOfSpatialSamplesBsplinePercentage = atof(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesBsplineHighPercentage")
    {
      initFile >> dummy;
      numberOfSpatialSamplesBsplineHighPercentage = atof(dummy.c_str());
    }


    else if (dummy == "-numberOfSpatialSamplesTranslation")
    {
      initFile >> dummy;
      numberOfSpatialSamplesTranslation = atoi(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesAffine")
    {
      initFile >> dummy;
      numberOfSpatialSamplesAffine = atoi(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesBspline")
    {
      initFile >> dummy;
      numberOfSpatialSamplesBspline = atoi(dummy.c_str());
    }
    else if (dummy == "-numberOfSpatialSamplesBsplineHigh")
    {
      initFile >> dummy;
      numberOfSpatialSamplesBsplineHigh = atoi(dummy.c_str());
    }
    
    else if (dummy == "-translationMultiScaleSamplePercentageIncrease")
    {
      initFile >> dummy;
      translationMultiScaleSamplePercentageIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-affineMultiScaleSamplePercentageIncrease")
    {
      initFile >> dummy;
      affineMultiScaleSamplePercentageIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-bsplineMultiScaleSamplePercentageIncrease")
    {
      initFile >> dummy;
      bsplineMultiScaleSamplePercentageIncrease = atof(dummy.c_str());
    }
    

    else if (dummy == "-translationMultiScaleMaximumIterationIncrease")
    {
      initFile >> dummy;
      translationMultiScaleMaximumIterationIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-affineMultiScaleMaximumIterationIncrease")
    {
      initFile >> dummy;
      affineMultiScaleMaximumIterationIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-bsplineMultiScaleMaximumIterationIncrease")
    {
      initFile >> dummy;
      bsplineMultiScaleMaximumIterationIncrease = atof(dummy.c_str());
    }

    else if (dummy == "-translationMultiScaleStepLengthIncrease")
    {
      initFile >> dummy;
      translationMultiScaleStepLengthIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-affineMultiScaleStepLengthIncrease")
    {
      initFile >> dummy;
      affineMultiScaleStepLengthIncrease = atof(dummy.c_str());
    }
    else if (dummy == "-bsplineMultiScaleStepLengthIncrease")
    {
      initFile >> dummy;
      bsplineMultiScaleStepLengthIncrease = atof(dummy.c_str());
    }
    
    else if (dummy == "-bsplineInitialGridSize")
    {
      initFile >> dummy;
      bsplineInitialGridSize = atoi(dummy.c_str());
    }
    else if (dummy == "-numberOfBsplineLevel")
    {
      initFile >> dummy;
      numberOfBsplineLevel = atoi(dummy.c_str());
    }
    else if (dummy == "-useBSplineRegularization")
    {
      initFile >> dummy;
      useBSplineRegularization = dummy;
    }
    else if (dummy == "-bsplineRegularizationFactor")
    {
      initFile >> dummy;
      bsplineRegularizationFactor = atof(dummy.c_str());
    }

    else if (dummy == "-translationScaleCoeffs")
    {
      initFile >> dummy;
      translationScaleCoeffs = atof(dummy.c_str());
    }
    else if (dummy == "-gaussianFilterVariance")
    {
      initFile >> dummy;
      gaussianFilterVariance = atof(dummy.c_str());
    }
    else if (dummy == "-maximumLineIteration")
    {
      initFile >> dummy;
      maximumLineIteration = atoi(dummy.c_str());
    }
    else if (dummy == "-parzenWindowStandardDeviation")
    {
      initFile >> dummy;
      parzenWindowStandardDeviation = atof(dummy.c_str());
    }

    else if (dummy == "-mask")
    {
      initFile >> dummy;
      mask = dummy;
    }
    else if (dummy == "-maskType")
    {
      initFile >> dummy;
      maskType = dummy;
    }
    else if (dummy == "-threshold1")
    {
      initFile >> dummy;
      threshold1 = atoi(dummy.c_str());
    }
    else if (dummy == "-threshold2")
    {
      initFile >> dummy;
      threshold2 = atoi(dummy.c_str());
    }
    
    else if (dummy == "-useBspline")
    {
      initFile >> dummy;
      useBspline = dummy;
    }
    else if (dummy == "-useBsplineHigh")
    {
      initFile >> dummy;
      useBsplineHigh = dummy;
    }
    else if (dummy == "-writeOutputImages")
    {
      initFile >> dummy;
      writeOutputImages = dummy;
    }
    else if (dummy == "-writeDeformationFields")
    {
      initFile >> dummy;
      writeDeformationFields = dummy;
    }
    
    else if (dummy == "-f")
    {
      initFile >> dummy;
      fileNames.push_back(dummy); // get file name
    }

  }

  initFile.close();
  return 0;
  // Ignore the output message for right now 
  if( false )
  {
    std::cerr << "Not enough input arguments " << std::endl;

    std::cerr << "\t  imageName [image names does not require any prefix parameters]" << std::endl;
    std::cerr << "\t -i Input folder for images" << std::endl;
    std::cerr << "\t -o Output folder for registered images" << std::endl;

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
    //std::cerr << std::endl << "Usage: " << std::endl << "\t" << argv[0];
    std::cerr << " -i folder1/ -f output/ -o gradient -m 4 -t Affine ImageFile1  ImageFile2 ImageFile3 ... " << std::endl << std::endl;
    return 1;
  }
  else
    return 0;

}

