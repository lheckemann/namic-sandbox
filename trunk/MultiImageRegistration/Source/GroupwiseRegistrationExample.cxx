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
#include "MultiResolutionMultiImageRegistrationMethod.h"
#include "VarianceMultiImageMetric.h"
#include "UnivariateEntropyMultiImageMetric.h"
#include "FixedTemplateVarianceMultiImageMetric.h"



// Transform headers
#include "itkAffineTransform.h"
#include "itkBSplineDeformableTransform.h"

// Interpolator headers    
#include "itkLinearInterpolateImageFunction.h"

#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkImageFileReader.h"

#include "itkCastImageFilter.h"

// Header to collect the time to register the images
#include "itkTimeProbesCollectorBase.h"

#include "itkCommand.h"
#include "itkNormalizeImageFilter.h"

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
#include "itkResampleImageFilter.h"


//System Related headers
#include <itksys/SystemTools.hxx>

//transform file reader/writer
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFactory.h"

//header for creating mask
#include "itkImageMaskSpatialObject.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"

//Define the global types for image type
#define PixelType float
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
      if(m_CumulativeIterationIndex % 1 != 0 )
      {
        m_CumulativeIterationIndex++;
        return;
      }
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer =
            dynamic_cast< GradientOptimizerPointer >(object );
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
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
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
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
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
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
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
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
        std::cout << "ALLSamples: Iter" << m_CumulativeIterationIndex;
        std::cout << " Value " << m_MetricPointer->GetValue(optimizer->GetCurrentPosition());
        std::cout << " # of samples " << m_MetricPointer->GetFixedImageRegion().GetNumberOfPixels()/2 << std::endl;
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
  
  typedef   itk::SingleValuedNonLinearOptimizer        OptimizerType;
  typedef   OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer              GradientOptimizerType;
  typedef   GradientOptimizerType *                    GradientOptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer    LineSearchOptimizerType;
  typedef   LineSearchOptimizerType  *                 LineSearchOptimizerPointer;
  typedef   itk::SPSAOptimizer                         SPSAOptimizerType;
  typedef   SPSAOptimizerType  *                       SPSAOptimizerPointerType;
  
  typedef   itk::Image< InternalPixelType, Dimension >   ImageType;
  typedef   itk::MultiImageMetric< ImageType>    MetricType;
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
    std::cout << "message: metric type: " << metric->GetNameOfClass() << std::endl;
    std::cout << "message: Transform Type: " << registration->GetTransformArray(0)->GetNameOfClass();
    if( !strcmp(registration->GetTransformArray(0)->GetNameOfClass(), "BSplineDeformableTransform") )
    {
      std::cout << " (" << bsplineGridSize << "x" << bsplineGridSize << "x" << bsplineGridSize << ")";
    }    
    std::cout << std::endl;
    std::cout << "message: Image Resolution: " <<
        registration->GetImagePyramidArray(0)->
        GetOutput(registration->GetCurrentLevel())->GetLargestPossibleRegion().GetSize() << std::endl;
    std::cout << "message: Number of Images: " << metric->GetNumberOfImages() << std::endl;
    std::cout << "message: Number of total parameters: " << registration->GetTransformParametersLength() << std::endl;
    std::cout << "message: Optimizertype: " << optimizer->GetNameOfClass() << std::endl;

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

    std::cout << "message: Sample Percentage : " << 
        metric->GetNumberOfSpatialSamples() / (double)metric->GetFixedImageRegion().GetNumberOfPixels() << std::endl;




  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }

  void SetFileNames(    std::vector<std::string> fileNames2,
                        std::vector<std::string> itkNotUsed(inputFileNames2),
                        std::vector<std::string> outputFileNames2,
                        std::string              outputFolder2)
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
  int bsplineGridSize;
  
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

    std::vector<std::string> fileNames;
    std::vector<std::string> inputFileNames;
    std::vector<std::string> outputFileNames;
    std::string              outputFolder;
    std::vector< itk::Transform< double, Dimension,Dimension >* > transformArray;
};

std::string replaceExtension(const std::string oldname, const std::string extension)
{
  return oldname.substr(0, oldname.rfind(".")) + "." + extension;
}


// Get the command line arguments
int getCommandLine(int argc, char *initFname, vector<std::string>& fileNames, std::string& inputFolder, std::string& outputFolder, std::string& optimizerType,
                   int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                   double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                   int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                   double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                   int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                   std::string& BSplineRegularizationFlag, double& bsplineRegularizationFactor,
                   std::string& transformType,std::string& metricType, std::string& useBspline, std::string& useBsplineHigh,
                   double& translationScaleCoeffs,
                   int& maximumLineIteration, double& parzenWindowStandardDeviation,
                   double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,
                   double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,
                   double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease,
                   unsigned int& numberOfSpatialSamplesAffine, unsigned int& numberOfSpatialSamplesBspline, unsigned int& numberOfSpatialSamplesBsplineHigh,
                   std::string &mask, std::string& maskType, unsigned int& threshold1, unsigned int& threshold2,
                   std::string &writeOutputImages, std::string &writeDeformationFields,
                   std::string &writeMean3DImages, std::string& metricPrint, unsigned int& printInterval,
                   double& SPSAalpha , double& SPSAgamma, double& SPSAcRel, int&    SPSAnumberOfPerturbation,
                   unsigned int& StartLevel,
                   std::string& useNormalizeFilter, double& gaussianFilterKernelWidth,
                   unsigned int& fixedTemplateIndex);


int main( int argc, char *argv[] )
{

  vector<std::string> fileNames;
  std::string inputFolder("");
  std::string outputFolder("");
  
  std::string optimizerType("lineSearch");
  std::string metricType("entropy");
  std::string interpolatorType("linear");
  
  std::string metricPrint("off");
  std::string useNormalizeFilter("off");
  
  unsigned int printInterval = 500;
  unsigned int startLevel = 0;
  
  int multiLevelAffine = 2;
  int multiLevelBspline = 1;
  int multiLevelBsplineHigh = 1;

  double optAffineLearningRate = 1e-3;
  double optBsplineLearningRate = 1e5;
  double optBsplineHighLearningRate = 1e5;

  int optAffineNumberOfIterations = 500;
  int optBsplineNumberOfIterations = 500;
  int optBsplineHighNumberOfIterations = 500;

  double numberOfSpatialSamplesAffinePercentage = 0;
  double numberOfSpatialSamplesBsplinePercentage = 0;
  double numberOfSpatialSamplesBsplineHighPercentage = 0;

  unsigned int numberOfSpatialSamplesAffine = 0;
  unsigned int numberOfSpatialSamplesBspline = 0;
  unsigned int numberOfSpatialSamplesBsplineHigh = 0;

  double affineMultiScaleSamplePercentageIncrease = 4.0;
  double bsplineMultiScaleSamplePercentageIncrease = 4.0;
  
  double affineMultiScaleMaximumIterationIncrease = 1.0;
  double bsplineMultiScaleMaximumIterationIncrease = 1.0;

  double affineMultiScaleStepLengthIncrease = 1.0;
  double bsplineMultiScaleStepLengthIncrease = 1.0;

  double translationScaleCoeffs = 1e-4;
  int maximumLineIteration = 7;
  
  int bsplineInitialGridSize = 4;
  int numberOfBsplineLevel = 1;
  
  std::string BSplineRegularizationFlag("off");
  double bsplineRegularizationFactor = 1e-1;
  
  double parzenWindowStandardDeviation = 0.4;
  double gaussianFilterKernelWidth = 3.0;
  std::string mask("all");
  std::string maskType("none");
  unsigned int threshold1 = 9;
  unsigned int threshold2 = 20;
      
  std::string useBspline("off");
  std::string useBsplineHigh("off");

  std::string writeOutputImages("off");
  std::string writeDeformationFields("off");
  std::string writeMean3DImages("off");
  
  //SPSA related parameters
  double SPSAalpha = 0.602;
  double SPSAgamma = 0.101;
  double SPSAcRel = 0.0001;
  int    SPSAnumberOfPerturbation = 1;
  
  unsigned int fixedTemplateIndex = 0;


  //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if( getCommandLine(argc,argv[i], fileNames, inputFolder, outputFolder, optimizerType,
        multiLevelAffine, multiLevelBspline, multiLevelBsplineHigh,
        optAffineLearningRate,  optBsplineLearningRate, optBsplineHighLearningRate,
        optAffineNumberOfIterations, optBsplineNumberOfIterations, optBsplineHighNumberOfIterations,
        numberOfSpatialSamplesAffinePercentage, numberOfSpatialSamplesBsplinePercentage, numberOfSpatialSamplesBsplineHighPercentage,
        bsplineInitialGridSize, numberOfBsplineLevel,
        BSplineRegularizationFlag, bsplineRegularizationFactor,
        interpolatorType, metricType, useBspline, useBsplineHigh,
        translationScaleCoeffs, maximumLineIteration,  parzenWindowStandardDeviation,
        affineMultiScaleSamplePercentageIncrease, bsplineMultiScaleSamplePercentageIncrease,
        affineMultiScaleMaximumIterationIncrease,  bsplineMultiScaleMaximumIterationIncrease,
        affineMultiScaleStepLengthIncrease, bsplineMultiScaleStepLengthIncrease,
        numberOfSpatialSamplesAffine, numberOfSpatialSamplesBspline, numberOfSpatialSamplesBsplineHigh,
        mask, maskType, threshold1, threshold2,
        writeOutputImages, writeDeformationFields,
        writeMean3DImages, metricPrint, printInterval,
        SPSAalpha , SPSAgamma, SPSAcRel, SPSAnumberOfPerturbation,
        startLevel,
        useNormalizeFilter, gaussianFilterKernelWidth,
        fixedTemplateIndex ) )
    {
      std::cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Image typedef
  typedef double ScalarType;
  typedef itk::Image< InternalPixelType, Dimension > ImageType;



  typedef itk::AffineTransform< ScalarType, Dimension > AffineTransformType;

  // Optimizer types
  typedef itk::GradientDescentOptimizer                OptimizerType;
  typedef itk::GradientDescentLineSearchOptimizer      LineSearchOptimizerType;
  typedef itk::SPSAOptimizer                           SPSAOptimizerType;
  typedef itk::AmoebaOptimizer                         SimplexOptimizerType;


  // Interpolator typedef
  typedef itk::InterpolateImageFunction<ImageType,ScalarType        >       InterpolatorType;
  typedef itk::LinearInterpolateImageFunction<ImageType,ScalarType  >       LinearInterpolatorType;


  // Metric typedefs
  typedef itk::MultiImageMetric< ImageType>                        MetricType;
  typedef itk::VarianceMultiImageMetric< ImageType>                VarianceMetricType;
  typedef itk::FixedTemplateVarianceMultiImageMetric< ImageType>                
                                                                   FixedTemplateVarianceMetricType;  
  typedef itk::UnivariateEntropyMultiImageMetric< ImageType>       EntropyMetricType;



  typedef OptimizerType::ScalesType                                        OptimizerScalesType;

  typedef itk::MultiResolutionMultiImageRegistrationMethod< ImageType >    RegistrationType;

  typedef itk::RecursiveMultiResolutionPyramidImageFilter<
                                    ImageType,
                                    ImageType  >                           ImagePyramidType;


  // Mask related typedefs
  typedef itk::Image< unsigned char, Dimension >                         ImageMaskType;
  typedef itk::ConnectedThresholdImageFilter< ImageType,ImageMaskType >
                                                                         ConnectedThresholdImageFilterType;
  typedef itk::NeighborhoodConnectedImageFilter< ImageType,ImageMaskType >
                                                                         NeighborhoodConnectedImageFilterType;
  typedef itk::ImageMaskSpatialObject<Dimension>                         ImageMaskSpatialObject;


  RegistrationType::Pointer   registration  = RegistrationType::New();

  // N is the number of images in the registration
  const int N = fileNames.size();
  if( N < 2 )
  {
    std::cout << "Not enough filenames " << std::endl;
    return EXIT_FAILURE;
  }
  
  //generate filenames
  vector<std::string> inputFileNames(N);
  vector<std::string> outputFileNames(N);
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
  OptimizerType::Pointer               optimizer;
  LineSearchOptimizerType::Pointer     lineSearchOptimizer;
  SPSAOptimizerType::Pointer           SPSAOptimizer;
  SimplexOptimizerType::Pointer        simplexOptimizer;
  
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
  
  //typedefs for affine transform array
  typedef std::vector<AffineTransformType::Pointer> AffineTransformArrayType;
  AffineTransformArrayType                          affineTransformArray(N);

  //typedefs for intorpolater array
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType                      interpolatorArray(N);

  // typedefs for image file readers
  typedef itk::ImageFileReader< ImageType  > ImageReaderType;
  typedef vector< ImageReaderType::Pointer > ImageArrayReader;

  typedef  vector<ImagePyramidType::Pointer> ImagePyramidArray;
  ImagePyramidArray                          imagePyramidArray(N);

  // typedef for normalized image filters
  // the mean and the variance of the images normalized before registering
  typedef itk::NormalizeImageFilter< ImageType, ImageType >   NormalizeFilterType;
  typedef NormalizeFilterType::Pointer                        NormalizeFilterTypePointer;
  typedef vector<NormalizeFilterTypePointer>                  NormalizedFilterArrayType;
  
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
  collector.Start( "5Total Time " );


  //Set the metric type
  MetricType::Pointer                 metric;
  if(metricType == "variance")
  {
    VarianceMetricType::Pointer varianceMetric        = VarianceMetricType::New();
    varianceMetric->SetGaussianFilterKernelWidth(gaussianFilterKernelWidth);
    if( BSplineRegularizationFlag == "on" )
    {
      varianceMetric->SetBSplineRegularizationFlag(true);
    }
    metric = varianceMetric;
  }
  else if(metricType == "fixedTemplateVariance")
  {
    FixedTemplateVarianceMetricType::Pointer fixedTemplateMetric = FixedTemplateVarianceMetricType::New();
    fixedTemplateMetric->SetFixedTemplateIndex(fixedTemplateIndex);
    fixedTemplateMetric->SetGaussianFilterKernelWidth(gaussianFilterKernelWidth);
    if( BSplineRegularizationFlag == "on" )
    {
      fixedTemplateMetric->SetBSplineRegularizationFlag(true);
    }
    metric = fixedTemplateMetric;    
  }
  else
  {
    EntropyMetricType::Pointer entropyMetric        = EntropyMetricType::New();
    entropyMetric->SetImageStandardDeviation(parzenWindowStandardDeviation);
    entropyMetric->SetGaussianFilterKernelWidth(gaussianFilterKernelWidth);
    if( BSplineRegularizationFlag == "on" )
    {
      entropyMetric->SetBSplineRegularizationFlag(true);
    }
    metric = entropyMetric;
  }
  metric->SetNumberOfImages(N);

  
  try
  {
  
    for(int i=0; i< N; i++ )
    {
      ImageReaderType::Pointer imageReader;
      affineTransformArray[i]     = AffineTransformType::New();
      
      // Get the interpolator type
      // assume linear by default
      interpolatorArray[i]  = LinearInterpolatorType::New();

      registration->SetTransformArray(i, affineTransformArray[i] );
      registration->SetInterpolatorArray(i, interpolatorArray[i] );

      imageReader = ImageReaderType::New();
      imageReader->ReleaseDataFlagOn();
      imageReader->SetFileName( inputFileNames[i].c_str() );

      //Initialize mask filters
      ConnectedThresholdImageFilterType::Pointer connectedThreshold;
      NeighborhoodConnectedImageFilterType::Pointer neighborhoodConnected;

      // if mask is single only mask the first image
      // else mask all images
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
        registration->SetImageMaskArray(i, maskImage);

        std::cout << "message: Computing mask "  << std::endl;
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
        registration->SetImageMaskArray(i, maskImage);

        std::cout << "message: Computing mask " << std::endl;
      }

      NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
      normalizeFilter->ReleaseDataFlagOn();

      normalizeFilter->SetInput( imageReader->GetOutput() );
      
      //Set up the Image Pyramid
      imagePyramidArray[i] = ImagePyramidType::New();
      imagePyramidArray[i]->SetNumberOfLevels( multiLevelAffine );
      if(useNormalizeFilter == "on")
      {
        imagePyramidArray[i]->SetInput( normalizeFilter->GetOutput() );
      }
      else
      {
        imagePyramidArray[i]->SetInput( imageReader->GetOutput() );  
      }

      std::cout << "message: Reading Image: " << inputFileNames[i].c_str() << std::endl;
      imagePyramidArray[i]->Update();


      //Set the input into the registration method
      registration->SetImagePyramidArray(i, imagePyramidArray[i] );

    }
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }





  // Set the number of samples to be used by the metric
  registration->SetMetric( metric  );

  
  // Create the Command observer and register it with the optimizer.
  // And set output file name
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  observer->m_MetricPointer = metric;
  observer->m_PrintInterval = printInterval;
  if( metricPrint == "on")
  {
    observer->m_MetricPrint = true;
  }


  // Create the Command interface observer and register it with the optimizer.
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  command->bsplineGridSize = bsplineInitialGridSize;

  registration->AddObserver( itk::IterationEvent(), command );

  std::cout << "message: Starting Registration " << std::endl;

  // Add probe to count the time used by the registration
  collector.Stop( "1Image Read " );

  


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

  // Allocate the space for tranform parameters used by registration method
  // We use a large array to concatenate the parameter array of each tranform
  typedef RegistrationType::ParametersType ParametersType;

  ParametersType initialAffineParameters( affineTransformArray[0]->GetNumberOfParameters()*N );
  initialAffineParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialAffineParameters );
  
  int numberOfParameters = affineTransformArray[0]->GetNumberOfParameters();

  // Initialize the affine transforms to identity transform
  // And use the results of the translation transform
  for(int i=0; i<N; i++)
  {
    affineTransformArray[i]->SetIdentity();
    AffineTransformType::InputPointType center;
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

    registration->SetInitialTransformParameters( i, affineTransformArray[i]->GetParameters() );
  }

  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  
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
    lineSearchOptimizer->SetMaximumLineIteration( maximumLineIteration );
    lineSearchOptimizer->SetScales( optimizerAffineScales );
    lineSearchOptimizer->AddObserver( itk::IterationEvent(), observer );

  }
  else if(optimizerType == "simplex")
  {
    simplexOptimizer->SetMaximumNumberOfIterations( optAffineNumberOfIterations );
    simplexOptimizer->SetScales( optimizerAffineScales );
    simplexOptimizer->AddObserver( itk::IterationEvent(), observer );

  }
  else if(optimizerType == "SPSA")
  {
    SPSAOptimizer->Seta( optAffineLearningRate );
    SPSAOptimizer->SetMaximumNumberOfIterations(optAffineNumberOfIterations);
    SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
    SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );
    
    SPSAOptimizer->SetScales( optimizerAffineScales );
    SPSAOptimizer->AddObserver( itk::IterationEvent(), observer );
  }
  else
  {
    optimizer->SetLearningRate( optAffineLearningRate );
    optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
    optimizer->MaximizeOff();
    optimizer->SetScales( optimizerAffineScales );
    optimizer->AddObserver( itk::IterationEvent(), observer );
  }

  // Set initial parameters of the transform
  ImageType::RegionType fixedImageRegion =
      imagePyramidArray[0]->GetOutput(imagePyramidArray[0]->GetNumberOfLevels()-1)->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );
  

  // Get the number of pixels (voxels) in the images
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  // Set the number of spatial samples for the metric
  unsigned int numberOfSamples = numberOfSpatialSamplesAffine;
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
  std::vector< itk::Transform< double, Dimension,Dimension >* > transformArray(N);

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
      std::string fileName = outputFolder + "Affine/TransformFiles/" + fileNames[i];
      fileName = replaceExtension(fileName, "txt");
      transformFileWriter->SetFileName(fileName.c_str());
      transformFileWriter->SetPrecision(12);
      transformFileWriter->SetInput(affineTransformArray[i]);
      transformFileWriter->Update();
    }
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
                             itk::CreateObjectFunction<AffineTransformType>::New());
       
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        std::string fileName = outputFolder + "Affine/TransformFiles/" + fileNames[i];
        fileName = replaceExtension(fileName, "txt");
        std::cout << "message: Reading " << fileName << std::endl;        
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
        registration->SetInitialTransformParameters( i, bsplineTransformArrayLow[i]->GetParameters() );
        registration->SetTransformArray(i, bsplineTransformArrayLow[i] );
        metric->SetBSplineTransformArray(i, bsplineTransformArrayLow[i] );


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
    OptimizerScalesType optimizerScales( bsplineTransformArrayLow[0]->GetNumberOfParameters()*N );
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

    
    for(int i=0; i< N; i++)
    {
      transformArray[i] = bsplineTransformArrayLow[i];
    }
    std::ostringstream localbsplineFolderName;
    localbsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize;
    command->SetFileNames( fileNames, inputFileNames,
                           outputFileNames, outputFolder + localbsplineFolderName.str() + "_MultiScale_" );
    localbsplineFolderName << "/" ;


    if(startLevel != 2)
    {
      //Write the transform files
      itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
      itksys::SystemTools::MakeDirectory( (outputFolder + localbsplineFolderName.str() + "TransformFiles/").c_str() );
      for(int i=0; i<N;i++)
      {
        std::string fileName = outputFolder + localbsplineFolderName.str() + "TransformFiles/" + fileNames[i];
        fileName = replaceExtension(fileName, "txt");
        transformFileWriter->SetFileName(fileName.c_str());
        transformFileWriter->SetPrecision(12);
        transformFileWriter->SetInput(bsplineTransformArrayLow[i]);
        transformFileWriter->Update();
      }
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

            std::ostringstream tempbsplineFolderName;
            tempbsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize << "/";
            std::string fileName = outputFolder + tempbsplineFolderName.str() + "TransformFiles/" + fileNames[i];
            fileName = replaceExtension(fileName, "txt");
            transformFileReader->SetFileName(fileName.c_str());
            transformFileReader->Update();
            TransformListType*   transformList = transformFileReader->GetTransformList();
        
            bsplineTransformArrayLow[i]->SetFixedParameters(transformList->front()->GetFixedParameters());
        
            bsplineTransformArrayLow[i]->SetParametersByValue(transformList->front()->GetParameters());
          }
        }
        
        // Increase the grid size by a factor of two
        bsplineInitialGridSize = 2*bsplineInitialGridSize;
        command->bsplineGridSize = bsplineInitialGridSize;
        
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

          spacingHigh =
              imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetSpacing();
          originHigh  =
              imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetOrigin();
          fixedRegion =
              imagePyramidArray[i]->GetOutput(imagePyramidArray[i]->GetNumberOfLevels()-1)->GetBufferedRegion();
          
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
          registration->SetInitialTransformParameters( i, bsplineTransformArrayHigh[i]->GetParameters() );
          registration->SetTransformArray(i, bsplineTransformArrayHigh[i] );
          metric->SetBSplineTransformArray(i, bsplineTransformArrayHigh[i] );

        }


        // Decrease the learning rate at each level
        // Reset the optimizer scales
        typedef OptimizerType::ScalesType       OptimizerScalesType;
        OptimizerScalesType localOptimizerScales( bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N );
        localOptimizerScales.Fill( 1.0 );

        if(optimizerType == "lineSearch")
        {
          lineSearchOptimizer->SetScales( localOptimizerScales );
          lineSearchOptimizer->SetStepLength(optBsplineHighLearningRate);
          lineSearchOptimizer->SetMaximumIteration( optBsplineHighNumberOfIterations );
        }
        else if(optimizerType == "simplex")
        {
          simplexOptimizer->SetMaximumNumberOfIterations( optBsplineHighNumberOfIterations );
          simplexOptimizer->SetScales( localOptimizerScales );
        }
        else if(optimizerType == "SPSA")
        {
          SPSAOptimizer->Seta( optBsplineHighLearningRate );
          SPSAOptimizer->SetMaximumNumberOfIterations(optBsplineHighNumberOfIterations);
          SPSAOptimizer->SetA( SPSAOptimizer->GetMaximumNumberOfIterations()/10.0 );
          SPSAOptimizer->Setc( SPSAOptimizer->Geta()*SPSAcRel );
          
          SPSAOptimizer->SetScales( localOptimizerScales );
        }
        else
        {
          optimizer->SetScales( localOptimizerScales );
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

        for(int i=0; i< N; i++)
        {
          transformArray[i] = bsplineTransformArrayHigh[i];
        }
        std::ostringstream tempbsplineFolderName;
        tempbsplineFolderName << "Bspline_Grid_" << bsplineInitialGridSize;
        
        command->SetFileNames( fileNames, inputFileNames,
                               outputFileNames, outputFolder + tempbsplineFolderName.str() + "_MultiScale_" );
        tempbsplineFolderName << "/" ;


        // Write the transform files
        itk::TransformFileWriter::Pointer  transformFileWriter = itk::TransformFileWriter::New();
        itksys::SystemTools::MakeDirectory( (outputFolder + tempbsplineFolderName.str() + "TransformFiles/").c_str() );
        for(int i=0; i<N;i++)
        {
          std::string fileName = outputFolder + tempbsplineFolderName.str() + "TransformFiles/" + fileNames[i];
          fileName = replaceExtension(fileName, "txt");
          transformFileWriter->SetFileName(fileName.c_str());
          transformFileWriter->SetPrecision(12);
          transformFileWriter->SetInput(bsplineTransformArrayHigh[i]);
          transformFileWriter->Update();
        }
      }

    } // End of BsplineHigh registration

  } // End of Bspline Registration
  collector.Stop( "4Bspline Reg." );


  // Print out results
  //
  std::cout << "message: Registration Completed " << std::endl;
  std::cout << "message: Time Report " << std::endl;
  collector.Stop( "5Total Time " );
  collector.Report();


  return EXIT_SUCCESS;
}




int getCommandLine(       int /*argc*/, char *initFname, vector<std::string>& fileNames, std::string& inputFolder, std::string& outputFolder, std::string& optimizerType,
                          int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                          double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                          int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                          double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          
                          std::string& BSplineRegularizationFlag, double& bsplineRegularizationFactor,

                          std::string& interpolatorType, std::string& metricType,
                          std::string& useBspline, std::string& useBsplineHigh,
                          
                          double& translationScaleCoeffs,
                          int& maximumLineIteration,   double& parzenWindowStandardDeviation,
                          
                          double& affineMultiScaleSamplePercentageIncrease, double& bsplineMultiScaleSamplePercentageIncrease,

                          double& affineMultiScaleMaximumIterationIncrease, double& bsplineMultiScaleMaximumIterationIncrease,

                          double& affineMultiScaleStepLengthIncrease, double& bsplineMultiScaleStepLengthIncrease,
                          unsigned int& numberOfSpatialSamplesAffine, unsigned int& numberOfSpatialSamplesBspline, unsigned int& numberOfSpatialSamplesBsplineHigh,

                          std::string &mask, std::string& maskType, unsigned int& threshold1, unsigned int& threshold2,
                          std::string &writeOutputImages, std::string &writeDeformationFields,
                          
                          std::string &writeMean3DImages, std::string& metricPrint, unsigned int& printInterval,

                          double& SPSAalpha , double& SPSAgamma, double& SPSAcRel, int&    SPSAnumberOfPerturbation,
                          unsigned int& StartLevel,
                          std::string& useNormalizeFilter,
                          double& gaussianFilterKernelWidth,
                          unsigned int& fixedTemplateIndex)
{


  std::ifstream initFile(initFname);
  if( initFile.fail() )
  {
    std::cout << "could not open file: " << initFname << std::endl;
    return EXIT_FAILURE;
  }

  while( !initFile.eof() )
  {
    
    std::string dummy;
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
    else if (dummy == "-BSplineRegularizationFlag")
    {
      initFile >> dummy;
      BSplineRegularizationFlag = dummy;
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
    else if (dummy == "-gaussianFilterKernelWidth")
    {
      initFile >> dummy;
      gaussianFilterKernelWidth = atof(dummy.c_str());
    }
    else if (dummy == "-useNormalizeFilter")
    {
      initFile >> dummy;
      useNormalizeFilter = dummy;
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
    
    else if( dummy == "-fixedTemplateIndex" )
    {
      initFile >> dummy;
      fixedTemplateIndex = atoi(dummy.c_str());
    }

    else if (dummy == "-f")
    {
      initFile >> dummy;
      fileNames.push_back(dummy); // get file name
    }

  }

  initFile.close();
  return EXIT_SUCCESS;

}

