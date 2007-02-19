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
#include "itkLinearInterpolateImageFunction.h"
#include "itkGradientDescentOptimizer.h"
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
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
    
//Bspline optimizer and transform
#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSBOptimizer.h"


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
  typedef   itk::GradientDescentOptimizer     OptimizerType;
  typedef   const OptimizerType   *           OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      //caller->Print(std::cout, 3 );
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
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
protected:
  RegistrationInterfaceCommand() {};
public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;
  typedef   itk::GradientDescentOptimizer   OptimizerType;
  typedef   OptimizerType *                            OptimizerPointer;
  void Execute(itk::Object * object, const itk::EventObject & event)
  {
    if( !(itk::IterationEvent().CheckEvent( &event )) )
    {
      return;
    }
    RegistrationPointer registration =
                        dynamic_cast<RegistrationPointer>( object );
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( 
                       registration->GetOptimizer() );

    if ( registration->GetCurrentLevel() == 0 )
    {
        //optimizer->SetLearningRate( 2e-4 );
        optimizer->MaximizeOn();
    }
    else
    {
      // Decrease the learning rate at each increasing multiresolution level
      // optimizer->SetLearningRate( optimizer->GetLearningRate() / 5.0 );
      optimizer->MaximizeOn();
    }
  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
};

// Get the command line arguments
int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType, int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh, double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate, int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,  int& bsplineInitialGridSize,  int& numberOfBsplineLevel, string& transformType, string& imageType,string& metricType, string& useBspline, string& useBsplineHigh  );


int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder("");
  string outputFolder("");
  
  string optimizerType("");
  string transformType("");
  string metricType("entropy");

  string metricPrint("on");
  
  int multiLevelAffine = 1;
  int multiLevelBspline = 1;
  int multiLevelBsplineHigh = 1;
  
  double optAffineLearningRate = 2e-3;
  double optBsplineLearningRate = 500;
  double optBsplineHighLearningRate = 500;
  
  int optAffineNumberOfIterations = 10000;
  int optBsplineNumberOfIterations = 10000;
  int optBsplineHighNumberOfIterations = 10000;
  
  double numberOfSpatialSamplesAffinePercentage = 0.01;
  double numberOfSpatialSamplesBsplinePercentage = 0.01;
  double numberOfSpatialSamplesBsplineHighPercentage = 0.01;

  
  int bsplineInitialGridSize = 5;
  int numberOfBsplineLevel = 1;
  string imageType = "normal";

  string useBspline("off");
  string useBsplineHigh("off");

  //Get the command line arguments
  if( getCommandLine(argc,argv, fileNames, inputFolder, outputFolder, optimizerType, multiLevelAffine, multiLevelBspline, multiLevelBsplineHigh, optAffineLearningRate,  optBsplineLearningRate, optBsplineHighLearningRate, optAffineNumberOfIterations, optBsplineNumberOfIterations, optBsplineHighNumberOfIterations, numberOfSpatialSamplesAffinePercentage, numberOfSpatialSamplesBsplinePercentage, numberOfSpatialSamplesBsplineHighPercentage, bsplineInitialGridSize, numberOfBsplineLevel, transformType, imageType, metricType, useBspline, useBsplineHigh  ) )
    return 1;
  

  // Input Image type typedef
  const    unsigned int    Dimension = 3;
  typedef  unsigned short  PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;

  //Internal Image Type typedef
  typedef double InternalPixelType;
  typedef double ScalarType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;



  typedef itk::AffineTransform< ScalarType, Dimension > TransformType;


  typedef itk::GradientDescentOptimizer       OptimizerType;
  typedef itk::LinearInterpolateImageFunction< 
                                    InternalImageType,
                                    ScalarType        > InterpolatorType;
                                    
  typedef itk::VarianceMultiImageMetric< InternalImageType>    MetricType;
  typedef itk::ParzenWindowEntropyMultiImageMetric< InternalImageType>    EntropyMetricType;


  typedef OptimizerType::ScalesType       OptimizerScalesType;

  typedef itk::MultiResolutionMultiImageRegistrationMethod< InternalImageType >    RegistrationType;

  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType  >    ImagePyramidType;


  OptimizerType::Pointer      optimizer     = OptimizerType::New();
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
  registration->SetOptimizer(     optimizer     );

  //typedefs for affine transform array
  typedef vector<TransformType::Pointer> TransformArrayType;
  TransformArrayType      transformArray(N);

  //typedefs for intorpolater array
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray(N);

  // typedefs for image file readers
  typedef itk::ImageFileReader< ImageType  > ImageReaderType;
  typedef vector< ImageReaderType::Pointer > ImageArrayReader;
  ImageArrayReader imageArrayReader(N);

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
  NormalizedFilterArrayType normalizedFilterArray(N);

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
  try
  {
    for( int i=0; i< N; i++ )
    {
      transformArray[i]     = TransformType::New();
      interpolatorArray[i]  = InterpolatorType::New();
      registration->SetTransformArray(     transformArray[i] ,i    );
      registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );
      imagePyramidArray[i] =  ImagePyramidType::New();

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
        imageArrayReader[i] = ImageReaderType::New();
        imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
      }
    
      normalizedFilterArray[i] = NormalizeFilterType::New();
      gaussianFilterArray[i] = GaussianFilterType::New();
      gaussianFilterArray[i]->SetVariance( 2.0 );
      
      if( imageType == "DICOM")
      {
        normalizedFilterArray[i]->SetInput( dicomArrayReader[i]->GetOutput() );
      }
      else
      {
        normalizedFilterArray[i]->SetInput( imageArrayReader[i]->GetOutput() );
      }
      
      gaussianFilterArray[i]->SetInput( normalizedFilterArray[i]->GetOutput() );
      registration->SetImageArrayPointer(    gaussianFilterArray[i]->GetOutput() , i   );
    }
  }
  catch( itk::ExceptionObject & err )
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return -1;
  }


  // Set initial parameters of the transform
  normalizedFilterArray[0]->Update();
  ImageType::RegionType fixedImageRegion =
      normalizedFilterArray[0]->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );


  // Allocate the space for tranform parameters used by registration method
  // We use a large array to concatenate the parameter array of each tranform
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transformArray[0]->GetNumberOfParameters()*N );
  initialParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialParameters );

  //Initialize the affine transforms to identity transform
  for(int i=0; i<N; i++)
  {
    transformArray[i]->SetIdentity();
    TransformType::InputPointType center;
    // Get spacing, origin and size of the images
    ImageType::SpacingType spacing = normalizedFilterArray[0]->GetOutput()->GetSpacing();
    itk::Point<double, Dimension> origin = normalizedFilterArray[0]->GetOutput()->GetOrigin();
    ImageType::SizeType size = normalizedFilterArray[0]->GetOutput()->GetLargestPossibleRegion().GetSize();

    // Place the center of rotation to the center of the image
    for(int j=0; j< Dimension; j++)
    {
      center[j] = (origin[j] + spacing[j]*size[j]) / 2.0;
    }
    transformArray[i]->SetIdentity();
    transformArray[i]->SetCenter(center);
    registration->SetInitialTransformParameters( transformArray[i]->GetParameters(),i );
  }


  // Set the scales of the optimizer
  // We set a large scale for the parameters corresponding to translation
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transformArray[0]->GetNumberOfParameters()*N );
  int numberOfParameters = transformArray[0]->GetNumberOfParameters();
  for( int i=0; i<N; i++)
  {
    for( int j=0; j<Dimension*Dimension; j++ )
    {
      optimizerScales[i*numberOfParameters + j] = -1.0; // scale for indices in 2x2 (3x3) Matrix
    }
    for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
    {
      optimizerScales[i*numberOfParameters + j] = -1.0 / 1000.0; // scale for translation on X,Y,Z
    }
  }
  optimizer->SetScales( optimizerScales );


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
  optimizer->SetLearningRate( optAffineLearningRate );
  optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
  optimizer->MaximizeOn();

  // Create the Command observer and register it with the optimizer.
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );


  // Create the Command interface observer and register it with the optimizer.
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  registration->AddObserver( itk::IterationEvent(), command );

  // Set the number of multiresolution levels
  registration->SetNumberOfLevels( multiLevelAffine );

  std::cout << "Starting Registration with Affine Transform " << std::endl;

  // Add probe to count the time used by the registration
  itk::TimeProbesCollectorBase collector;


  // Start registration
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

  //Print out the metric values for translation parameters
  if( metricPrint == "on")
  {
      ofstream outputFile("metricOutput.txt");
      ParametersType parameters = registration->GetLastTransformParameters();

      for(double i=2.6; i<7.4; i+=0.2)
      {
        for(double j=2.6; j<7.4; j+=0.2)
        {
          parameters[10] = i/10.0;
          parameters[11] = j/10.0;

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
    ParametersType affineCurrentParameters(transformArray[0]->GetNumberOfParameters());
  
    // Update the affine transform parameters
    for( int i=0; i<N; i++)
    {
      for(int j=0; j<transformArray[0]->GetNumberOfParameters(); j++)
      {
        affineCurrentParameters[j]=affineParameters[i*transformArray[0]->GetNumberOfParameters()+j];
      }
      transformArray[i]->SetParametersByValue(affineCurrentParameters);
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
          spacing = imageArrayReader[i]->GetOutput()->GetSpacing();
          origin = imageArrayReader[i]->GetOutput()->GetOrigin();
          fixedRegion = imageArrayReader[i]->GetOutput()->GetBufferedRegion();
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
        bsplineTransformArrayLow[i]->SetBulkTransform(transformArray[i]);
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
    optimizer->SetScales( optimizerScales );

    // Set optimizer parameters for bspline registration
    optimizer->SetLearningRate( optBsplineLearningRate );
    optimizer->SetNumberOfIterations( optBsplineNumberOfIterations );
    optimizer->MaximizeOn();

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
            spacingHigh = imageArrayReader[i]->GetOutput()->GetSpacing();
            originHigh  = imageArrayReader[i]->GetOutput()->GetOrigin();
            fixedRegion = imageArrayReader[i]->GetOutput()->GetBufferedRegion();
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
        optimizer->SetLearningRate( optBsplineHighLearningRate );
        optimizer->SetNumberOfIterations( optBsplineHighNumberOfIterations );
        optimizer->MaximizeOn();

        //Reset the optimizer scales
        typedef OptimizerType::ScalesType       OptimizerScalesType;
        OptimizerScalesType optimizerScales( bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N );
        optimizerScales.Fill( 1.0 );
        optimizer->SetScales( optimizerScales );

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

  
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  
  double bestValue = optimizer->GetValue();

  
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
  CastFilterType::Pointer  caster = CastFilterType::New();


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
    numberOfParameters = transformArray[0]->GetNumberOfParameters();
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
      bsplineTransformArrayHigh[i]->SetBulkTransform( transformArray[i] );
      bsplineTransformArrayHigh[i]->SetParametersByValue( currentParameters );
    }
    else if (useBspline == "on")
    {
      bsplineTransformArrayLow[i]->SetBulkTransform( transformArray[i] );
      bsplineTransformArrayLow[i]->SetParametersByValue( currentParameters );
    }
    else
    {
      transformArray[i]->SetParametersByValue( currentParameters );
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
      resample->SetTransform( transformArray[i] );
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
    resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(  fixedImage->GetOrigin() );
    resample->SetOutputSpacing( fixedImage->GetSpacing() );
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
      writer->SetFileName( outputFileNames[i].c_str() );
      writer->SetInput( caster->GetOutput()   );
      writer->Update();
    }
  }

  std::cout << "Computing mean images" << std::endl;

  // Compute Mean Images 
  ResampleFilterType::Pointer resample2 = ResampleFilterType::New();

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
    resample->SetTransform( transformArray[0] );
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
    resample2->SetTransform( transformArray[1] );
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
      resample->SetTransform( transformArray[i] );
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
    writer->SetInput( caster->GetOutput()   );

    string meanImageFname;
    if (Dimension == 2)
    {
      meanImageFname = outputFolder + "MeanRegisteredImage.png";
    }
    else
    {
      meanImageFname = outputFolder + "MeanRegisteredImage.mhd";
    }
    writer->SetFileName( meanImageFname.c_str() );
    writer->Update();
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
    writer->SetInput( caster->GetOutput()   );

    string meanImageFname;
    if (Dimension == 2)
    {
      meanImageFname = outputFolder + "MeanOriginalImage.png";
    }
    else
    {
      meanImageFname = outputFolder + "MeanOriginalImage.mhd";
    }
    writer->SetFileName( meanImageFname.c_str() );
    writer->Update();
  }

  

  return 0;
}




int getCommandLine(       int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,
                          int& multiLevelAffine, int& multiLevelBspline, int& multiLevelBsplineHigh,
                          double& optAffineLearningRate, double& optBsplineLearningRate, double& optBsplineHighLearningRate,
                          int& optAffineNumberOfIterations, int& optBsplineNumberOfIterations, int& optBsplineHighNumberOfIterations,
                          double& numberOfSpatialSamplesAffinePercentage, double& numberOfSpatialSamplesBsplinePercentage, double& numberOfSpatialSamplesBsplineHighPercentage,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& transformType, string& imageType,string& metricType,
                          string& useBspline, string& useBsplineHigh  )
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
    
    else if (dummy == "-multiLevelAffine")
      multiLevelAffine = atoi(argv[++i]);
    else if (dummy == "-multiLevelBspline")
      multiLevelBspline = atoi(argv[++i]);
    else if (dummy == "-multiLevelBsplineHigh")
      multiLevelBsplineHigh = atoi(argv[++i]);

    else if (dummy == "-optAffineLearningRate")
      optAffineLearningRate = atof(argv[++i]);
    else if (dummy == "-optBsplineLearningRate")
      optBsplineLearningRate = atof(argv[++i]);
    else if (dummy == "-optBsplineHighLearningrate")
      optBsplineHighLearningRate = atof(argv[++i]);

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

