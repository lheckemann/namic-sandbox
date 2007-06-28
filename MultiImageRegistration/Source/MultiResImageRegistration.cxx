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
#include "itkNaryAddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

// Transform headers    
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"
#include "UserBSplineDeformableTransform.h"

// Interpolator headers    
#include "itkLinearInterpolateImageFunction.h"

#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkImage.h"
#include "itkNormalizeImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"

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
#include "itkGradientDescentOptimizer.h"
#include "GradientDescentLineSearchOptimizer.h"
#include "itkSPSAOptimizer.h"
#include "itkAmoebaOptimizer.h"


//BSpline related headers
#include "itkBSplineResampleImageFunction.h"
#include "itkIdentityTransform.h"
#include "itkBSplineDecompositionImageFilter.h"


//System Related headers
#include <itksys/SystemTools.hxx>

//transform file reader/writer
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFactory.h"

#include "itkImageRegionIterator.h"

//header for creating mask
#include "itkImageMaskSpatialObject.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"

//Define the global types for image type
#define PixelType unsigned short
#define InternalPixelType float
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
  CommandIterationUpdate(): m_CumulativeIterationIndex(0)
  {
    m_MetricPrint = false;
    m_PrintInterval = 500;
  };
public:
  
  typedef   itk::SingleValuedNonLinearOptimizer   OptimizerType;
  typedef   const OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer       GradientOptimizerType;
  typedef   const GradientOptimizerType *             GradientOptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;
  typedef   const LineSearchOptimizerType  *          LineSearchOptimizerPointer;
  typedef   itk::SPSAOptimizer                        SPSAOptimizerType;
  typedef   const SPSAOptimizerType  *                SPSAOptimizerPointer;
  typedef   itk::AmoebaOptimizer                        SimplexOptimizerType;
  typedef   const SimplexOptimizerType  *                SimplexOptimizerTypePointer;
  typedef   itk::MultiImageMetric< itk::Image<InternalPixelType, Dimension > >    MetricType;


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
      if(m_CumulativeIterationIndex % 20 != 0 )
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
        if(gradientPointer->GetCurrentIteration() % 50 == 0)
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
        if(m_CumulativeIterationIndex % 50 == 0 )
        { 
          //std::cout << std::setw(6) << "Position: " << lineSearchOptimizerPointer->GetCurrentPosition() << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointer spsaOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointer >( object );
        std::cout << std::setiosflags(ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << spsaOptimizerPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << spsaOptimizerPointer->GetValue() << "   " << std::endl;
        if(m_CumulativeIterationIndex % 100 == 0 )
        {
          typedef SPSAOptimizerType::ParametersType ParametersType;
          ParametersType parameters = spsaOptimizerPointer->GetCurrentPosition();
          double max = -1e308;
          for(unsigned int i=0; i< parameters.Size(); i++)
          {
            if(parameters[i]> max)
            {
               max = parameters[i];
            }
          }
          std::cout << std::setw(6) << "Max Parameter: " << max << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "AmoebaOptimizer") )
      {
        SimplexOptimizerTypePointer simplexOptimizer =
            dynamic_cast< SimplexOptimizerTypePointer >( object );
        std::cout << std::setiosflags(ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(6) << simplexOptimizer->GetCachedValue() << "   " << std::endl;
        if(m_CumulativeIterationIndex % 100 == 0 )
        {
          typedef SimplexOptimizerType::ParametersType ParametersType;
          ParametersType parameters = simplexOptimizer->GetCurrentPosition();
          double max = -1e308;
          for(unsigned int i=0; i< parameters.Size(); i++)
          {
            if(parameters[i]> max)
            {
              max = parameters[i];
            }
          }
          std::cout << std::setw(6) << "Max Parameter: " << max << std::endl;
        }
      }

      // only print results every ten iterations
      if(m_MetricPrint && m_CumulativeIterationIndex % m_PrintInterval == 0 )
      {
         // Print the metric Value
        unsigned long int numberOfSamples = m_MetricPointer->GetNumberOfSpatialSamples();
        m_MetricPointer->Finalize();
        m_MetricPointer->SetNumberOfSpatialSamples( 1000000 );
        m_MetricPointer->Initialize();
        cout << "ALLSamples: Iter" << m_CumulativeIterationIndex;
        cout << " Value " << m_MetricPointer->GetValue(optimizer->GetCurrentPosition());
        cout << " # of samples " << m_MetricPointer->GetFixedImageRegion().GetNumberOfPixels()/2 << endl;
        m_MetricPointer->Finalize();
        m_MetricPointer->SetNumberOfSpatialSamples(numberOfSamples);
        m_MetricPointer->Initialize();

      }
      
      //Increase the cumulative index
      m_CumulativeIterationIndex++;
      //std::cout << std::setw(6) << optimizer->GetCurrentPosition()[i] << "   ";

      
    }

    MetricType::Pointer m_MetricPointer;
    bool m_MetricPrint;
    unsigned int m_PrintInterval;
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
  typedef   itk::SPSAOptimizer                  SPSAOptimizerType;
  typedef   SPSAOptimizerType  *                SPSAOptimizerPointerType;
  
  typedef   itk::Image< InternalPixelType, Dimension >   InternalImageType;
  typedef   itk::MultiImageMetric< InternalImageType>    MetricType;
  typedef   MetricType *                                 MetricPointer;

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
    std::cout << "message: Registration using " << metric->GetNameOfClass() << std::endl;
    std::cout << "message: Transform Type " << registration->GetTransformArray(0)->GetNameOfClass() << std::endl;
    std::cout << "message: Multiresolution level : " << registration->GetCurrentLevel() << std::endl;
    std::cout << "message: Number of total parameters : " << registration->GetTransformParametersLength() << std::endl;
    std::cout << "message: Optimizertype : " << optimizer->GetNameOfClass() << std::endl;

    if ( registration->GetCurrentLevel() == 0 )
    {
      // Set the number of spatial samples according to the current level
      metric->SetNumberOfSpatialSamples(
                        (unsigned int) (metric->GetNumberOfSpatialSamples() /
                        pow( pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease,
                             (double) (registration->GetNumberOfLevels() - 1.0) ) ) );

      
      
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
        std::cout << "message: Optimizer # of Iter. to go : " << gradientPointer->GetNumberOfIterations() << std::endl;
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
        std::cout << "message: Optimizer # of Iter. to go : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() <<std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointerType SPSAOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointerType >(
            registration->GetOptimizer() );
        SPSAOptimizerPointer->SetMaximumNumberOfIterations(
            (int)(SPSAOptimizerPointer->GetMaximumNumberOfIterations()*pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        SPSAOptimizerPointer->Seta(SPSAOptimizerPointer->Geta()*
            pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) );
        SPSAOptimizerPointer->SetA( SPSAOptimizerPointer->GetMaximumNumberOfIterations()/10.0 );
        SPSAOptimizerPointer->Setc( SPSAOptimizerPointer->Getc()*
                                    pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ));
        //print messages
        std::cout << "message: Optimizer # of Iter. to go : " <<
            SPSAOptimizerPointer->GetMaximumNumberOfIterations() <<std::endl;
        std::cout << "message: Optimizer a : " <<
            SPSAOptimizerPointer->Geta() << std::endl;
        std::cout << "message: Optimizer c : " <<
            SPSAOptimizerPointer->Getc() << std::endl;
      }

      
    }
    else
    {
      // Set the number of spatial samples according to the current level
      metric->SetNumberOfSpatialSamples(
            (unsigned int) (metric->GetNumberOfSpatialSamples() *
                  pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease ) );

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
        std::cout << "message: Optimizer # of Iter. to go : " << gradientPointer->GetNumberOfIterations() << std::endl;
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
        
        std::cout << "message: Optimizer # of Iter. to go : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() << std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

      //SPSA
      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointerType SPSAOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointerType >(registration->GetOptimizer() );
        SPSAOptimizerPointer->SetMaximumNumberOfIterations(
            (int)(SPSAOptimizerPointer->GetMaximumNumberOfIterations()/m_MultiScaleMaximumIterationIncrease ));
        SPSAOptimizerPointer->Seta(
            SPSAOptimizerPointer->Geta()/m_MultiScaleStepLengthIncrease );
        SPSAOptimizerPointer->SetA( SPSAOptimizerPointer->GetMaximumNumberOfIterations()/10.0 );
        SPSAOptimizerPointer->Setc( SPSAOptimizerPointer->Getc()/m_MultiScaleStepLengthIncrease );

        
        std::cout << "message: Optimizer # of Iter. to go : " <<
            SPSAOptimizerPointer->GetMaximumNumberOfIterations() << std::endl;
        std::cout << "message: Optimizer a : " <<
            SPSAOptimizerPointer->Geta() << std::endl;
        std::cout << "message: Optimizer c : " <<
            SPSAOptimizerPointer->Getc() << std::endl;
      }

    }

    std::cout << "message: Number of total pixels : " << metric->GetFixedImageRegion().GetNumberOfPixels() << std::endl;
    std::cout << "message: Number of used pixels : " << metric->GetNumberOfSpatialSamples() << std::endl;




  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }

  void SetFileNames(    std::vector<string> fileNames2,
                        std::vector<string> inputFileNames2,
                        std::vector<string> outputFileNames2,
                        string              outputFolder2)
  {
    for(unsigned int i=0; i<fileNames.size(); i++ )
    {
      fileNames[i] = fileNames2[i];
      inputFileNames[i] = inputFileNames[i];
      outputFileNames[i] =  outputFileNames2[i];
    }
    outputFolder =  outputFolder2;
    transformArray.resize(fileNames.size());

  }
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

    std::vector<string> fileNames;
    std::vector<string> inputFileNames;
    std::vector<string> outputFileNames;
    string              outputFolder;
    std::vector< itk::Transform< double, Dimension,Dimension >* > transformArray;
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
                   string &mask, string& maskType, unsigned int& threshold1, unsigned int& threshold2,
                   string &writeOutputImages, string &writeDeformationFields,
                   unsigned int &NumberOfFixedImages,
                   unsigned int &numberOfNearestNeigbors, double &errorBound,
                   string &writeMean3DImages, string& metricPrint, unsigned int& printInterval,
                   double& SPSAalpha , double& SPSAgamma, double& SPSAcRel, int&    SPSAnumberOfPerturbation,
                   double& HistogramSamplesPerBin,
                   unsigned int& StartLevel, string& OutputIntermediateResults );


int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder("");
  string outputFolder("");
  
  string optimizerType("lineSearch");
  string metricType("entropy");
  string interpolatorType("linear");
  
  string metricPrint("off");
  unsigned int printInterval = 500;
  unsigned int startLevel = 0;
  
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
  double HistogramSamplesPerBin = 50.0;
  string mask("all");
  string maskType("none");
  unsigned int threshold1 = 9;
  unsigned int threshold2 = 20;
      
  string imageType = "normal";

  string useBspline("off");
  string useBsplineHigh("off");

  string writeOutputImages("off");
  string writeDeformationFields("off");
  string writeMean3DImages("off");
  string outputIntermediateResults("on");
  
  unsigned int NumberOfFixedImages = 0;

  //KNN related parameters
  unsigned int numberOfNearestNeigbors = 100;
  double errorBound = 0.1;

  //SPSA related parameters
  double SPSAalpha = 0.602;
  double SPSAgamma = 0.101;
  double SPSAcRel = 0.0001;
  int    SPSAnumberOfPerturbation = 1;


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
        interpolatorType, imageType, metricType, useBspline, useBsplineHigh,
        translationScaleCoeffs,gaussianFilterVariance, maximumLineIteration,  parzenWindowStandardDeviation,
        translationMultiScaleSamplePercentageIncrease, affineMultiScaleSamplePercentageIncrease, bsplineMultiScaleSamplePercentageIncrease, translationMultiScaleMaximumIterationIncrease, affineMultiScaleMaximumIterationIncrease,  bsplineMultiScaleMaximumIterationIncrease,
        translationMultiScaleStepLengthIncrease, affineMultiScaleStepLengthIncrease, bsplineMultiScaleStepLengthIncrease,
        numberOfSpatialSamplesTranslation, numberOfSpatialSamplesAffine, numberOfSpatialSamplesBspline, numberOfSpatialSamplesBsplineHigh,
        mask, maskType, threshold1, threshold2,
        writeOutputImages, writeDeformationFields,
        NumberOfFixedImages,
        numberOfNearestNeigbors, errorBound,
        writeMean3DImages, metricPrint, printInterval,
        SPSAalpha , SPSAgamma, SPSAcRel, SPSAnumberOfPerturbation,
        HistogramSamplesPerBin,
        startLevel, outputIntermediateResults ) )
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
  typedef itk::SPSAOptimizer                     SPSAOptimizerType;
  typedef itk::AmoebaOptimizer                     SimplexOptimizerType;


  // Interpolator typedef
  typedef itk::InterpolateImageFunction<InternalImageType,ScalarType        >  InterpolatorType;
  typedef itk::LinearInterpolateImageFunction<InternalImageType,ScalarType        > LinearInterpolatorType;


  
  typedef itk::MultiImageMetric< InternalImageType>    MetricType;
  typedef itk::VarianceMultiImageMetric< InternalImageType>    VarianceMetricType;
  typedef itk::ParzenWindowEntropyMultiImageMetric< InternalImageType>    EntropyMetricType;



  typedef OptimizerType::ScalesType       OptimizerScalesType;

  typedef itk::MultiResolutionMultiImageRegistrationMethod< InternalImageType >    RegistrationType;

  typedef itk::RecursiveMultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType  >    ImagePyramidType;


  //Mask related typedefs
  typedef itk::Image< unsigned char, Dimension > ImageMaskType;
  typedef itk::ConnectedThresholdImageFilter< ImageType,ImageMaskType >
                                        ConnectedThresholdImageFilterType;
  typedef itk::NeighborhoodConnectedImageFilter< ImageType,ImageMaskType >
                                        NeighborhoodConnectedImageFilterType;
  typedef itk::ImageMaskSpatialObject<Dimension> ImageMaskSpatialObject;


  RegistrationType::Pointer   registration  = RegistrationType::New();


  // N is the number of images in the registration
  const int N = fileNames.size();
  if( N < 2 )
  {
    std::cout << "Not enough filenames " << std::endl;
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

  registration->SetNumberOfLevels( multiLevelAffine );
  registration->SetNumberOfImages(N);
  registration->ReleaseDataFlagOn();

  //Set the optimizerType
  OptimizerType::Pointer      optimizer;
  LineSearchOptimizerType::Pointer lineSearchOptimizer;
  SPSAOptimizerType::Pointer SPSAOptimizer;
  SimplexOptimizerType::Pointer simplexOptimizer;
  
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer     = LineSearchOptimizerType::New();
    registration->SetOptimizer(     lineSearchOptimizer     );
  }
  else if(optimizerType == "simplex")
  {
    simplexOptimizer     = SimplexOptimizerType::New();
    simplexOptimizer->SetParametersConvergenceTolerance( 1e-100 );
    simplexOptimizer->SetFunctionConvergenceTolerance( 1e-100 );
    simplexOptimizer->SetAutomaticInitialSimplex(false);
    
    registration->SetOptimizer(     simplexOptimizer     );
  }
  else if(optimizerType == "SPSA")
  {
    SPSAOptimizer     = SPSAOptimizerType::New();
    
    SPSAOptimizer->MinimizeOn();
    SPSAOptimizer->SetMaximumNumberOfIterations(100);
    SPSAOptimizer->Seta( 1.0 );
    SPSAOptimizer->SetA( 20.0 );
    SPSAOptimizer->SetAlpha( SPSAalpha );
    SPSAOptimizer->SetGamma( SPSAgamma );
    SPSAOptimizer->SetTolerance(1e-50);
    SPSAOptimizer->SetStateOfConvergenceDecayRate(0.5);
    SPSAOptimizer->SetMinimumNumberOfIterations(100);
    SPSAOptimizer->SetNumberOfPerturbations(SPSAnumberOfPerturbation);

    registration->SetOptimizer(     SPSAOptimizer     );

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
  typedef std::vector<TransformType::Pointer> TransformArrayType;
  TransformArrayType      affineTransformArray(N);

  //typedefs for intorpolater array
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray(N);

  // typedefs for image file readers
  typedef itk::ImageFileReader< ImageType  > ImageReaderType;
  typedef vector< ImageReaderType::Pointer > ImageArrayReader;

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

  // Add probe to count the time used by the registration
  itk::TimeProbesCollectorBase collector;
  
  collector.Start( "1Image Read " );
  collector.Start( "6Total Time " );


  //Set the metric type
  MetricType::Pointer                 metric;
  if(metricType == "variance")
  {
    metric        = VarianceMetricType::New();
    // Set the number of samples to be used by the metric
  }
  else
  {
    EntropyMetricType::Pointer entropyMetric        = EntropyMetricType::New();
    entropyMetric->SetImageStandardDeviation(parzenWindowStandardDeviation);
    metric = entropyMetric;
  }
  metric->SetRegularizationFactor(bsplineRegularizationFactor);
  metric->SetNumberOfImages(N);

  
  try
  {
  
    for(int i=0; i< N; i++ )
    {
      ImageReaderType::Pointer imageReader;
      translationTransformArray[i] = TranslationTransformType::New();

      // Get the interpolator type
      // assume linear by default
      interpolatorArray[i]  = LinearInterpolatorType::New();

      registration->SetTransformArray(     translationTransformArray[i] ,i    );
      registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );

      imageReader = ImageReaderType::New();
      //imageReader->ReleaseDataFlagOn();
      imageReader->SetFileName( inputFileNames[i].c_str() );

      //Initialize mask filters
      ConnectedThresholdImageFilterType::Pointer connectedThreshold;
      NeighborhoodConnectedImageFilterType::Pointer neighborhoodConnected;

      //if mask is single only mask the first image
      //else mask all images
      if(maskType == "connectedThreshold" && ((mask == "single" && i==0 ) || mask == "all"))
      {
        connectedThreshold =ConnectedThresholdImageFilterType::New();
        connectedThreshold->SetInput( imageReader->GetOutput() );
        
        ConnectedThresholdImageFilterType::IndexType seed;
        seed.Fill(0);
        connectedThreshold->AddSeed(seed);

        connectedThreshold->SetLower(threshold1);
        connectedThreshold->SetUpper(threshold2);
        
        connectedThreshold->ReleaseDataFlagOn();
        connectedThreshold->Update();
        
        ImageMaskSpatialObject::Pointer maskImage = ImageMaskSpatialObject::New();
        maskImage->SetImage(connectedThreshold->GetOutput());
        registration->SetImageMaskArray(maskImage, i);

        cout << "message: Computing mask "  << endl;
      }
      else if( maskType == "neighborhoodConnected" && ((mask == "single" && i==0 ) || mask == "all"))
      {
        neighborhoodConnected = NeighborhoodConnectedImageFilterType::New();
        neighborhoodConnected->SetInput( imageReader->GetOutput() );
        
        NeighborhoodConnectedImageFilterType::IndexType seed;
  
        seed.Fill(0);
        neighborhoodConnected->SetSeed(seed);
  
        neighborhoodConnected->SetLower (threshold1);
        neighborhoodConnected->SetUpper (threshold2);
        
        typedef NeighborhoodConnectedImageFilterType::InputImageSizeType SizeType;
        SizeType radius;
        radius.Fill(1);
        neighborhoodConnected->SetRadius(radius);
        
        neighborhoodConnected->ReleaseDataFlagOn();
        neighborhoodConnected->Update();
        
        ImageMaskSpatialObject::Pointer maskImage = ImageMaskSpatialObject::New();
        maskImage->SetImage(neighborhoodConnected->GetOutput());
        registration->SetImageMaskArray(maskImage, i);

        cout << "message: Computing mask " << endl;
      }

      NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
      normalizeFilter->ReleaseDataFlagOn();

      normalizeFilter->SetInput( imageReader->GetOutput() );

      //GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
      //gaussianFilter->ReleaseDataFlagOn();
      //gaussianFilter->SetVariance( gaussianFilterVariance );
      //gaussianFilter->SetInput( normalizeFilter->GetOutput() );

      //Set up the Image Pyramid
      imagePyramidArray[i] = ImagePyramidType::New();
      //imagePyramidArray[i]->ReleaseDataFlagOn();
      imagePyramidArray[i]->SetNumberOfLevels( multiLevelAffine );
      imagePyramidArray[i]->SetInput( normalizeFilter->GetOutput() );

      std::cout << "message: Reading Image: " << inputFileNames[i].c_str() << std::endl;
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
      imagePyramidArray[0]->GetOutput(imagePyramidArray[0]->GetNumberOfLevels()-1)->GetBufferedRegion();
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
  
  unsigned int numberOfSamples = numberOfSpatialSamplesTranslation;
  if( numberOfSpatialSamplesTranslationPercentage > 0 )
  {
    numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesTranslationPercentage );
  }

  // Set the number of samples to be used by the metric
  registration->SetMetric( metric  );
  metric->SetNumberOfSpatialSamples( numberOfSamples );

  
  // Create the Command observer and register it with the optimizer.
  // And set output file name
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  observer->m_MetricPointer = metric;
  observer->m_PrintInterval = printInterval;
  if( metricPrint == "on")
  {
    observer->m_MetricPrint = true;
  }
  //observer->SetFilename("iterations.txt");


  // Set the optimizer parameters
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optTranslationLearningRate);
    lineSearchOptimizer->SetMaximize(false);
    lineSearchOptimizer->SetMaximumIteration( optTranslationNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( maximumLineIteration );
    lineSearchOptimizer->SetScales( optimizerScales );
    lineSearchOptimizer->AddObserver( itk::IterationEvent(), observer );
  }
  else if(optimizerType == "simplex")
  {
    simplexOptimizer->SetMaximumNumberOfIterations( optTranslationNumberOfIterations );
    simplexOptimizer->SetScales( optimizerScales );

    simplexOptimizer->AddObserver( itk::IterationEvent(), observer );
  }
  else if(optimizerType == "SPSA")
  {
    SPSAOptimizer->Seta( optTranslationLearningRate );
    SPSAOptimizer->SetMaximumNumberOfIterations(optTranslationNumberOfIterations);
    SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
    SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );

    
    SPSAOptimizer->SetScales( optimizerScales );
    SPSAOptimizer->AddObserver( itk::IterationEvent(), observer );
  }
  else
  {
    optimizer->SetLearningRate( optTranslationLearningRate );
    optimizer->SetNumberOfIterations( optTranslationNumberOfIterations );
    optimizer->MaximizeOff();
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

  std::cout << "message: Starting Registration " << std::endl;

  // Add probe to count the time used by the registration
  collector.Stop( "1Image Read " );



  // Start registration with translation transform
  try 
  {
    collector.Start( "2Translation Reg." );
    if( startLevel == 0 )
    {
      registration->StartRegistration();
    }
    collector.Stop( "2Translation Reg." );
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return -1;
  }

  // Write the output images after translation transform
  collector.Start( "5Image Write " );

  
  std::vector< itk::Transform< double, Dimension,Dimension >* > transformArray(N);
  for(int i=0; i< N; i++)
  {
    transformArray[i] = translationTransformArray[i];
  }

  command->SetFileNames( fileNames, inputFileNames,
                         outputFileNames, outputFolder + "Translation_MultiScale_");



  
  collector.Stop( "5Image Write " );

  //Print out the metric values for translation parameters
  if( 0 && metricPrint == "on")
  {
    cout << "message: Metric Probe " << endl;
    ofstream outputFile("metricOutput.txt");
    ParametersType parameters = registration->GetLastTransformParameters();
    cout << parameters << endl;
    parameters.Fill(0.0);
    
    for(double i=-10.0; i<0.0; i+=0.1)
    {
      parameters[0] = i;
      outputFile << metric->GetValue(parameters) << " ";
      outputFile << std::endl;
    }
    outputFile.close();
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

  // Initialize the affine transforms to identity transform
  // And use the results of the translation transform
  for(int i=0; i<N; i++)
  {
    affineTransformArray[i]->SetIdentity();
    TransformType::InputPointType center;
    // Get spacing, origin and size of the images
    ImageType::SpacingType spacing = imagePyramidArray[i]->GetOutput(imagePyramidArray[0]->GetNumberOfLevels()-1)->GetSpacing();
    itk::Point<double, Dimension> origin = imagePyramidArray[i]->GetOutput(imagePyramidArray[0]->GetNumberOfLevels()-1)->GetOrigin();
    ImageType::SizeType size = imagePyramidArray[i]->GetOutput(imagePyramidArray[0]->GetNumberOfLevels()-1)->GetLargestPossibleRegion().GetSize();

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
      optimizerAffineScales[i*numberOfParameters + j] = translationScaleCoeffs; // scale for translation on X,Y,Z
    }
  }

  // Set the optimizer parameters
  if(optimizerType == "lineSearch")
  {
    lineSearchOptimizer->SetStepLength(optAffineLearningRate);
    lineSearchOptimizer->SetMaximize(false);
    lineSearchOptimizer->SetMaximumIteration( optAffineNumberOfIterations );
    lineSearchOptimizer->SetMaximumLineIteration( 10 );
    lineSearchOptimizer->SetScales( optimizerAffineScales );
  }
  else if(optimizerType == "simplex")
  {
    simplexOptimizer->SetMaximumNumberOfIterations( optAffineNumberOfIterations );
    simplexOptimizer->SetScales( optimizerAffineScales );
  }
  else if(optimizerType == "SPSA")
  {
    SPSAOptimizer->Seta( optAffineLearningRate );
    SPSAOptimizer->SetMaximumNumberOfIterations(optAffineNumberOfIterations);
    SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
    SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );
    
    SPSAOptimizer->SetScales( optimizerAffineScales );
  }
  else
  {
    optimizer->SetLearningRate( optAffineLearningRate );
    optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
    optimizer->MaximizeOff();
    optimizer->SetScales( optimizerAffineScales );
    //optimizer->AddObserver( itk::IterationEvent(), observer );
  }

  // Set the number of spatial samples for the metric
  numberOfSamples = numberOfSpatialSamplesAffine;
  if( numberOfSpatialSamplesAffinePercentage > 0 )
  {
    numberOfSamples = static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesAffinePercentage );
  }
  metric->SetNumberOfSpatialSamples( numberOfSamples );

  
  //Set the parameters of the command observer
  command->SetMultiScaleSamplePercentageIncrease(affineMultiScaleSamplePercentageIncrease);
  command->SetMultiScaleMaximumIterationIncrease(affineMultiScaleMaximumIterationIncrease);
  command->SetMultiScaleStepLengthIncrease(affineMultiScaleStepLengthIncrease);
  
  // Start registration with Affine Transform
  try
  {
    collector.Start( "3Affine Reg." );
    if( startLevel == 0 )
    {
      registration->StartRegistration();
    }
    collector.Stop( "3Affine Reg." );
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }

  // Write the output images after affine transform
  collector.Start( "5Image Write " );


  for(int i=0; i< N; i++)
  {
    transformArray[i] = affineTransformArray[i];
  }
  
  command->SetFileNames( fileNames, inputFileNames,
                         outputFileNames, outputFolder + "Affine_MultiScale_");


  if( startLevel == 0 )
  {
    // Write the transform files
    itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
    itksys::SystemTools::MakeDirectory( (outputFolder + "Affine/TransformFiles/").c_str() );
    for(int i=0; i<N;i++)
    {
      string fileName = outputFolder + "Affine/TransformFiles/" + fileNames[i];
      fileName.replace(fileName.size()-3, 3, "txt" );
      transformFileWriter->SetFileName(fileName.c_str());
      transformFileWriter->SetPrecision(12);
      transformFileWriter->SetInput(affineTransformArray[i]);
      transformFileWriter->Update();
    }
  }


  collector.Stop( "5Image Write " );

  

  //
  // BSpline Registration
  //
  // We supply the affine transform from the first part as initial parameters to the Bspline
  // registration. As the user might want to increase the resolution of the Bspline grid
  // we begin by declaring the Bspline parameters for the low resolution

  collector.Start( "4Bspline Reg." );

  const unsigned int SplineOrder = 3;
  typedef ScalarType CoordinateRepType;

  typedef itk::UserBSplineDeformableTransform< CoordinateRepType,
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
    if(useBSplineRegularization == "on")
    {
      metric->SetRegularization(true);
    }
    
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
      for(unsigned int j=0; j<affineTransformArray[0]->GetNumberOfParameters(); j++)
      {
        affineCurrentParameters[j]=affineParameters[i*affineTransformArray[0]->GetNumberOfParameters()+j];
      }
      affineTransformArray[i]->SetParametersByValue(affineCurrentParameters);
    }

    // Initialize the size of the parameters array
    registration->SetTransformParametersLength( static_cast<int>( pow( static_cast<double>(bsplineInitialGridSize+SplineOrder),
                                                 static_cast<int>(Dimension))*Dimension*N ));


    if( startLevel != 0)
    {
      // Read the transform files
      typedef itk::TransformFileReader    TransformFileReader;
      typedef TransformFileReader::TransformListType   TransformListType;

      // Create reader factories
      for(int i=0; i<N; i++)
      {
        itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
        f->RegisterTransform(affineTransformArray[i]->GetTransformTypeAsString().c_str(),
                             affineTransformArray[i]->GetTransformTypeAsString().c_str(),
                             affineTransformArray[i]->GetTransformTypeAsString().c_str(),
                             1,
                             itk::CreateObjectFunction<TransformType>::New());
       
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        string fileName = outputFolder + "Affine/TransformFiles/" + fileNames[i];
        std::cout << "message: Reading " << fileName << endl;
        fileName.replace(fileName.size()-3, 3, "txt" );
        transformFileReader->SetFileName(fileName.c_str());
        
        // Create the transforms
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        affineTransformArray[i]->SetFixedParameters(transformList->front()->GetFixedParameters());
        affineTransformArray[i]->SetParameters(transformList->front()->GetParameters());
      }

    }
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
      for( int i=0; i< N; i++ )
      {

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
        SpacingType spacing = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetSpacing();
        OriginType origin   = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetOrigin();
        ImageType::RegionType fixedRegion = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetBufferedRegion();

        ImageType::SizeType fixedImageSize = fixedRegion.GetSize();

        // Calculate the spacing for the Bspline grid
        for(unsigned int r=0; r<Dimension; r++)
        {
          // There was a floor here, is it a bug? The floor causes a division by zero error
          // if the gridSizeOnImage is larger than fixedImageSize
          spacing[r] *= ( static_cast<double>(fixedImageSize[r] )  /
              static_cast<double>(gridSizeOnImage[r] ) );
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
        metric->SetBSplineTransformArray(     bsplineTransformArrayLow[i] ,i    );


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
    if(optimizerType == "lineSearch")
    {
      lineSearchOptimizer->SetScales( optimizerScales );
      lineSearchOptimizer->SetStepLength(optBsplineLearningRate);
      lineSearchOptimizer->SetMaximumIteration( optBsplineNumberOfIterations );
    }
    else if(optimizerType == "simplex")
    {
      simplexOptimizer->SetMaximumNumberOfIterations( optBsplineNumberOfIterations );
      simplexOptimizer->SetScales( optimizerScales );
    }
    else if(optimizerType == "SPSA")
    {
      SPSAOptimizer->Seta( optBsplineLearningRate );
      SPSAOptimizer->SetMaximumNumberOfIterations(optBsplineNumberOfIterations);
      SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
      SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );
      
      SPSAOptimizer->SetScales( optimizerScales );
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
    metric->SetNumberOfSpatialSamples( numberOfSamples );


    registration->SetNumberOfLevels( multiLevelBspline );

    //Set the parameters of the command observer
    command->SetMultiScaleSamplePercentageIncrease(bsplineMultiScaleSamplePercentageIncrease);
    command->SetMultiScaleMaximumIterationIncrease(bsplineMultiScaleMaximumIterationIncrease);
    command->SetMultiScaleStepLengthIncrease(bsplineMultiScaleStepLengthIncrease);

    try
    {
      if(startLevel != 2)
      {
         registration->StartRegistration();
      }
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return -1;
    }

    // Write the output images after bspline transform
    collector.Start( "5Image Write " );


    
    for(int i=0; i< N; i++)
    {
      transformArray[i] = bsplineTransformArrayLow[i];
    }
    ostringstream bsplineFolderName;
    bsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize;
    command->SetFileNames( fileNames, inputFileNames,
                           outputFileNames, outputFolder + bsplineFolderName.str() + "_MultiScale_" );
    bsplineFolderName << "/" ;


    if(startLevel != 2)
    {
      //Write the transform files
      itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
      itksys::SystemTools::MakeDirectory( (outputFolder + bsplineFolderName.str() + "TransformFiles/").c_str() );
      for(int i=0; i<N;i++)
      {
        string fileName = outputFolder + bsplineFolderName.str() + "TransformFiles/" + fileNames[i];
        fileName.replace(fileName.size()-3, 3, "txt" );
        transformFileWriter->SetFileName(fileName.c_str());
        transformFileWriter->SetPrecision(12);
        transformFileWriter->SetInput(bsplineTransformArrayLow[i]);
        transformFileWriter->Update();
      }
    }
    collector.Stop( "5Image Write " );

    
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
        

          // Read tranforms from file
          if(startLevel == 2)
          {
            typedef itk::TransformFileReader    TransformFileReader;
            typedef TransformFileReader::TransformListType   TransformListType;

            itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
          
            f->RegisterTransform(bsplineTransformArrayLow[i]->GetTransformTypeAsString().c_str(),
                                 bsplineTransformArrayLow[i]->GetTransformTypeAsString().c_str(),
                                 bsplineTransformArrayLow[i]->GetTransformTypeAsString().c_str(),
                                 1,
                                 itk::CreateObjectFunction<BSplineTransformType>::New());

            TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();

            ostringstream bsplineFolderName;
            bsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize << "/";
            string fileName = outputFolder + bsplineFolderName.str() + "TransformFiles/" + fileNames[i];
            fileName.replace(fileName.size()-3, 3, "txt" );
            transformFileReader->SetFileName(fileName.c_str());
            transformFileReader->Update();
            TransformListType*   transformList = transformFileReader->GetTransformList();
        
            bsplineTransformArrayLow[i]->SetFixedParameters(transformList->front()->GetFixedParameters());
        
            bsplineTransformArrayLow[i]->SetParametersByValue(transformList->front()->GetParameters());
          }
        }
        
        // Increase the grid size by a factor of two
        bsplineInitialGridSize = 2*bsplineInitialGridSize;
        
        registration->SetTransformParametersLength( static_cast<int>(
            pow( static_cast<double>(bsplineInitialGridSize+SplineOrder),
                                                    static_cast<int>(Dimension))
                                                      *Dimension*N ));

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

          spacingHigh = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetSpacing();
          originHigh  = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetOrigin();
          fixedRegion = imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetBufferedRegion();
          
          ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
    
          for(unsigned int rh=0; rh<Dimension; rh++)
          {
            //There was a floor here, is it a BUG?
            spacingHigh[rh] *= ( static_cast<double>(fixedImageSize[rh] )  /
                static_cast<double>(gridHighSizeOnImage[rh] ) );
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

          bsplineTransformArrayHigh[i]->SetBulkTransform( affineTransformArray[i] );
          bsplineTransformArrayHigh[i]->SetParameters( bsplineParametersArrayHigh[i] );

          // Set parameters of the fine grid Bspline transform
          // to coarse grid Bspline transform for the next level
          bsplineTransformArrayLow[i]->SetGridSpacing( spacingHigh );
          bsplineTransformArrayLow[i]->SetGridOrigin( originHigh );
          bsplineTransformArrayLow[i]->SetGridRegion( bsplineRegion );


          // Set initial parameters of the registration
          registration->SetInitialTransformParameters( bsplineTransformArrayHigh[i]->GetParameters() , i );
          registration->SetTransformArray( bsplineTransformArrayHigh[i], i );
          metric->SetBSplineTransformArray(     bsplineTransformArrayHigh[i] ,i    );

        }


        // Decrease the learning rate at each level
        // Reset the optimizer scales
        typedef OptimizerType::ScalesType       OptimizerScalesType;
        OptimizerScalesType optimizerScales( bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N );
        optimizerScales.Fill( 1.0 );

        if(optimizerType == "lineSearch")
        {
          lineSearchOptimizer->SetScales( optimizerScales );
          lineSearchOptimizer->SetStepLength(optBsplineHighLearningRate);
          lineSearchOptimizer->SetMaximumIteration( optBsplineHighNumberOfIterations );
        }
        else if(optimizerType == "simplex")
        {
          simplexOptimizer->SetMaximumNumberOfIterations( optBsplineHighNumberOfIterations );
          simplexOptimizer->SetScales( optimizerScales );
        }
        else if(optimizerType == "SPSA")
        {
          SPSAOptimizer->Seta( optBsplineHighLearningRate );
          SPSAOptimizer->SetMaximumNumberOfIterations(optBsplineHighNumberOfIterations);
          SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
          SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );
          
          SPSAOptimizer->SetScales( optimizerScales );
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
        metric->SetNumberOfSpatialSamples( numberOfSamples );


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
        
        // Write the output images after bspline transform
        collector.Start( "5Image Write " );


        for(int i=0; i< N; i++)
        {
          transformArray[i] = bsplineTransformArrayHigh[i];
        }
        ostringstream bsplineFolderName;
        bsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize;
        
        command->SetFileNames( fileNames, inputFileNames,
                               outputFileNames, outputFolder + bsplineFolderName.str() + "_MultiScale_" );
        bsplineFolderName << "/" ;


        // Write the transform files
        itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
        itksys::SystemTools::MakeDirectory( (outputFolder + bsplineFolderName.str() + "TransformFiles/").c_str() );
        for(int i=0; i<N;i++)
        {
          string fileName = outputFolder + bsplineFolderName.str() + "TransformFiles/" + fileNames[i];
          fileName.replace(fileName.size()-3, 3, "txt" );
          transformFileWriter->SetFileName(fileName.c_str());
          transformFileWriter->SetPrecision(12);
          transformFileWriter->SetInput(bsplineTransformArrayHigh[i]);
          transformFileWriter->Update();
        }
        collector.Stop( "5Image Write " );

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

  // Print out results
  //
  std::cout << "message: Registration Completed " << std::endl;

  // Get the time for the registration
  collector.Start( "5Image Write " );


  // typedefs for output images
  ImageArrayReader imageArrayReader(N);

  typedef itk::ImageFileWriter< ImageType >  WriterType;

  // Extract slices writer type
  typedef itk::Image< unsigned char, 2 >    SliceImageType;
  typedef itk::ImageFileWriter< SliceImageType >  SliceWriterType;
  SliceWriterType::Pointer  sliceWriter = SliceWriterType::New();
  // Filter to extract a slice from an image
  typedef itk::ExtractImageFilter< ImageType, SliceImageType > SliceExtractFilterType;
  SliceExtractFilterType::Pointer sliceExtractFilter = SliceExtractFilterType::New();


  WriterType::Pointer      writer =  WriterType::New();
  writer->ReleaseDataFlagOn();


  // Loop over images and write output images
  for(int i=0; i<N; i++)
  {

    //Read the images again for memory efficiency
    imageArrayReader[i] = ImageReaderType::New();
    //imageArrayReader[i]->ReleaseDataFlagOn();
    imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
    imageArrayReader[i]->Update();

    //Extract slices for 3D Images
    if(Dimension == 3)
    {

      sliceExtractFilter->SetInput( imageArrayReader[i]->GetOutput() );
      
      ImageType::SizeType size = imageArrayReader[i]->GetOutput()->GetLargestPossibleRegion().GetSize();
      ImageType::IndexType start = imageArrayReader[i]->GetOutput()->GetLargestPossibleRegion().GetIndex();
      start[2] = size[2]/2;
      size[2] = 0;

        
      ImageType::RegionType extractRegion;
      extractRegion.SetSize(  size  );
      extractRegion.SetIndex( start );
      sliceExtractFilter->SetExtractionRegion( extractRegion );

      //Write the original images
      string slices2("InputImage/Slices/");
      slices2 = outputFolder + slices2;
      string outputFilename2(fileNames[i]);
      outputFilename2[outputFilename2.size()-4] = '.';
      outputFilename2[outputFilename2.size()-3] = 'j';
      outputFilename2[outputFilename2.size()-2] = 'p';
      outputFilename2[outputFilename2.size()-1] = 'g';
      outputFilename2 = slices2 + outputFilename2;
      itksys::SystemTools::MakeDirectory( slices2.c_str() );

      sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
      sliceWriter->SetFileName( outputFilename2.c_str() );
      sliceWriter->Update();

    }
  }

  std::cout << "message: Computing mean images" << std::endl;
      
  // Compute Mean Images
  typedef itk::NaryAddImageFilter < InternalImageType,
                                           InternalImageType > NaryAddFilterType;
  NaryAddFilterType::Pointer NaryAddfilter = NaryAddFilterType::New();

  typedef itk::CastImageFilter<ImageType, InternalImageType > InternalImageCasterType;
  typedef std::vector< InternalImageCasterType::Pointer >    InternalImageCasterArray;
  InternalImageCasterArray  internalImageCasterArray(N);
  
  for(int i=0; i<N; i++)
  {
    internalImageCasterArray[i] = InternalImageCasterType::New();
    internalImageCasterArray[i]->ReleaseDataFlagOn();
    internalImageCasterArray[i]->SetInput(imageArrayReader[i]->GetOutput());
    NaryAddfilter->SetInput(i,internalImageCasterArray[i]->GetOutput());
  }

  
  
  //Write the mean image
  typedef itk::RescaleIntensityImageFilter< InternalImageType, InternalImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  WriterType::Pointer      writer3 =  WriterType::New();

  intensityRescaler->SetInput( NaryAddfilter->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );


  string meanImageFname;

  typedef itk::CastImageFilter<InternalImageType, ImageType > OutputImageCasterType;
  OutputImageCasterType::Pointer  outputImageCaster = OutputImageCasterType::New();

  outputImageCaster->SetInput(intensityRescaler->GetOutput());
  writer3->SetInput( outputImageCaster->GetOutput()   );

  if (Dimension == 2)
  {
    meanImageFname = outputFolder + "InputImage/MeanImages/MeanOriginalImage.jpg";
  }
  else
  {
    //Write the original images
    // in the format of the given images
    string meanImages("InputImage/MeanImages/");
    meanImageFname = outputFolder + meanImages + "MeanOriginalImage.";
    meanImageFname = meanImageFname + inputFileNames[0][inputFileNames[0].size()-3] +
        inputFileNames[0][inputFileNames[0].size()-2] +inputFileNames[0][inputFileNames[0].size()-1];

    meanImages = outputFolder + meanImages;
    itksys::SystemTools::MakeDirectory( meanImages.c_str() );

    vector<string> outputFilenames(Dimension);
    outputFilenames[0] = "MeanSlice1.jpg";
    outputFilenames[1] = "MeanSlice2.jpg";
    outputFilenames[2] = "MeanSlice3.jpg";

    for(int index=0; index<Dimension; index++)
    {
      outputFilenames[index] = meanImages + outputFilenames[index];

      ImageType::SizeType size = imageArrayReader[0]->GetOutput()->GetLargestPossibleRegion().GetSize();
      ImageType::IndexType start = imageArrayReader[0]->GetOutput()->GetLargestPossibleRegion().GetIndex();
      start[index] = size[index]/2;
      size[index] = 0;
        
      ImageType::RegionType extractRegion;
      extractRegion.SetSize(  size  );
      extractRegion.SetIndex( start );
      sliceExtractFilter->SetExtractionRegion( extractRegion );

      sliceExtractFilter->SetInput( outputImageCaster->GetOutput() );
      sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
      sliceWriter->SetFileName( outputFilenames[index].c_str() );
      sliceWriter->Update();
    }
  }
  writer3->SetImageIO(imageArrayReader[1]->GetImageIO());
  writer3->SetFileName( meanImageFname.c_str() );
  if(writeMean3DImages == "on")
  {
    writer3->Update();
  }
  collector.Stop( "5Image Write " );

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

                          string& interpolatorType, string& imageType,string& metricType,
                          string& useBspline, string& useBsplineHigh,
                          
                          double& translationScaleCoeffs,double& gaussianFilterVariance,
                          int& maximumLineIteration,   double& parzenWindowStandardDeviation,
                          
                          double& translationMultiScaleSamplePercentageIncrease, double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,

                          double& translationMultiScaleMaximumIterationIncrease, double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,

                          double& translationMultiScaleStepLengthIncrease, double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease,
                          unsigned int& numberOfSpatialSamplesTranslation, unsigned int& numberOfSpatialSamplesAffine, unsigned int& numberOfSpatialSamplesBspline, unsigned int& numberOfSpatialSamplesBsplineHigh,

                          string &mask, string& maskType, unsigned int& threshold1, unsigned int& threshold2,
                          string &writeOutputImages, string &writeDeformationFields,
                          unsigned int& NumberOfFixedImages,
                          
                          unsigned int &numberOfNearestNeigbors, double &errorBound,

                          string &writeMean3DImages, string& metricPrint, unsigned int& printInterval,

                          double& SPSAalpha , double& SPSAgamma, double& SPSAcRel, int&    SPSAnumberOfPerturbation,
                          double& HistogramSamplesPerBin,
                          unsigned int& StartLevel, string& outputIntermediateResults )
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
    else if (dummy == "-interpolatorType")
    {
      initFile >> dummy;
      interpolatorType = dummy;
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
    else if (dummy == "-optBsplineHighLearningRate")
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
    else if (dummy == "-HistogramSamplesPerBin")
    {
      initFile >> dummy;
      HistogramSamplesPerBin = atof(dummy.c_str());
    }
    else if (dummy == "-OutputIntermediateResults")
    {
      initFile >> dummy;
      outputIntermediateResults = dummy;
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

    
    else if (dummy == "-alpha")
    {
      initFile >> dummy;
      SPSAalpha = atof(dummy.c_str());
    }
    else if (dummy == "-gamma")
    {
      initFile >> dummy;
      SPSAgamma = atof(dummy.c_str());
    }
    else if (dummy == "-cRel")
    {
      initFile >> dummy;
      SPSAcRel = atof(dummy.c_str());
    }
    else if (dummy == "-numberOfPerturbation")
    {
      initFile >> dummy;
      SPSAnumberOfPerturbation = atoi(dummy.c_str());
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
    else if (dummy == "-writeMean3DImages")
    {
      initFile >> dummy;
      writeMean3DImages = dummy;
    }
    
    else if (dummy == "-NumberOfFixedImages")
    {
      initFile >> dummy;
      NumberOfFixedImages = atoi(dummy.c_str());
    }

    else if (dummy == "-NumberOfNearestNeigbors")
    {
      initFile >> dummy;
      numberOfNearestNeigbors = atoi(dummy.c_str());
    }
    else if (dummy == "-errorBound")
    {
      initFile >> dummy;
      errorBound = atof(dummy.c_str());
    }

    else if (dummy == "-metricPrint")
    {
      initFile >> dummy;
      metricPrint = dummy;
    }
    else if( dummy == "-printInterval" )
    {
      initFile >> dummy;
      printInterval = atoi(dummy.c_str());
    }
    else if( dummy == "-StartLevel" )
    {
      initFile >> dummy;
      StartLevel = atoi(dummy.c_str());
    }

    else if (dummy == "-f")
    {
      initFile >> dummy;
      fileNames.push_back(dummy); // get file name
    }

  }

  initFile.close();
  return 0;

}

