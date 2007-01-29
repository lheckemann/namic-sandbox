/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MultiResImageRegistration2.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/14 12:16:23 $
  Version:   $Revision: 1.43 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

//  Software Guide : BeginCommandLineArgs
//    INPUTS:  {BrainT1SliceBorder20.png}
//    INPUTS:  {BrainProtonDensitySliceShifted13x17y.png}
//    OUTPUTS: {MultiResImageRegistration2Output.png}
//    100
//    OUTPUTS: {MultiResImageRegistration2CheckerboardBefore.png}
//    OUTPUTS: {MultiResImageRegistration2CheckerboardAfter.png}
//  Software Guide : EndCommandLineArgs

// Software Guide : BeginLatex
//
//  This example illustrates the use of more complex components of the
//  registration framework. In particular, it introduces the use of the
//  \doxygen{AffineTransform} and the importance of fine-tuning the scale
//  parameters of the optimizer.
//
// \index{itk::ImageRegistrationMethod!AffineTransform}
// \index{itk::ImageRegistrationMethod!Scaling parameter space}
// \index{itk::AffineTransform!Image Registration}
//
// The AffineTransform is a linear transformation that maps lines into
// lines. It can be used to represent translations, rotations, anisotropic
// scaling, shearing or any combination of them. Details about the affine
// transform can be seen in Section~\ref{sec:AffineTransform}.
//
// In order to use the AffineTransform class, the following header
// must be included.
//
// \index{itk::AffineTransform!Header}
//
// Software Guide : EndLatex 


// Software Guide : BeginCodeSnippet
// Software Guide : EndCodeSnippet
//user defined headers
#include "MultiResolutionImageRegistrationMethod.h"
#include "VarianceMultiImageMetric.h"
#include "AddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "MultiResolutionImageRegistrationMethod.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGradientDescentOptimizer.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkImage.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"

#include "itkTimeProbesCollectorBase.h"

//  The following section of code implements an observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
    
//Bspline optimizer and transform
#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSBOptimizer.h"

#include "itkNormalizeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

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
      //std::cout << std::setw(6) << optimizer->GetCurrentPosition()[0] << "   ";
      std::cout << std::setw(6) << optimizer->GetCurrentPosition() <<std::endl;
      
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
      optimizer->SetLearningRate( optimizer->GetLearningRate() / 5.0 );
      optimizer->MaximizeOn();

      }
  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
};

// Get the command line arguments
int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,  int& multiLevel, string& transformType, string& imageType );

int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder;
  string outputFolder;
  string optimizerType;
  string transformType;
  int multiLevelAffine = 3;
  int multiLevelBspline = 1;
  int multiLevelBsplineHigh = 1;
  double optAffineLearningRate = 2e-4;
  double optBsplineLearningRate = 50;
  int optAffineNumberOfIterations = 5;
  int optBsplineNumberOfIterations = 8;
  double numberOfSpatialSamplesAffinePercentage = 0.001;
  double numberOfSpatialSamplesBsplinePercentage = 0.001;
  int bsplineGridSize = 5;
  int numberOfResolutionLevel = 2;
  string imageType = "normal";

  if( getCommandLine(argc,argv, fileNames, inputFolder, outputFolder, optimizerType, multiLevelAffine, transformType, imageType  ) )
    return 1;

  // Input Image type typedef
  const    unsigned int    Dimension = 3;
  typedef  unsigned int  PixelType;
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


  typedef OptimizerType::ScalesType       OptimizerScalesType;

  typedef itk::MultiResolutionMultiImageRegistrationMethod< InternalImageType >    RegistrationType;

  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType  >    ImagePyramidType;


  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();


  int N = fileNames.size();
  //create filenames
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

  
  typedef vector<TransformType::Pointer> TransformArrayType;
  TransformArrayType      transformArray;
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray;
  transformArray.resize(N);
  interpolatorArray.resize(N);

  MetricType::Pointer         metric        = MetricType::New();
  registration->SetMetric( metric  );


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


  
  typedef itk::NormalizeImageFilter< ImageType, InternalImageType > NormalizeFilterType;
  typedef NormalizeFilterType::Pointer NormalizeFilterTypePointer;
  typedef vector<NormalizeFilterTypePointer> NormalizedFilterArrayType;
  NormalizedFilterArrayType normalizedFilterArray(N);

  typedef itk::DiscreteGaussianImageFilter<
                                      InternalImageType, 
                                      InternalImageType
                                                    > GaussianFilterType;
  typedef vector< GaussianFilterType::Pointer > GaussianFilterArrayType;
  GaussianFilterArrayType gaussianFilterArray(N);



  /* Connect the compenents together */
  try
  {
    for( int i=0; i< N; i++ ){
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


  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transformArray[0]->GetNumberOfParameters()*N );
  initialParameters.Fill(0.0);
  registration->SetInitialTransformParameters( initialParameters );
  for(int i=0; i<N; i++)
  {
    transformArray[i]->SetIdentity();
    registration->SetInitialTransformParameters( transformArray[i]->GetParameters(),i );
  }


  
  // Set the scales of the optimizer
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transformArray[0]->GetNumberOfParameters()*N );
  int numberOfParameters = transformArray[0]->GetNumberOfParameters();
  for( int i=0; i<N; i++)
  {
    for( int j=0; j<Dimension*Dimension; j++ )
    {
      optimizerScales[i*numberOfParameters + j] = 1.0; // scale for indices in 2x2 (3x3) Matrix
    }
    for(int j=Dimension*Dimension; j<Dimension+Dimension*Dimension; j++)
    {
      optimizerScales[i*numberOfParameters + j] = 1.0 / 1000000.0; // scale for translation on X,Y,Z
    }
  }
  optimizer->SetScales( optimizerScales );

  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  const unsigned int numberOfSamples =
      static_cast< unsigned int >( numberOfPixels * numberOfSpatialSamplesAffinePercentage );

  metric->SetNumberOfSpatialSamples( numberOfSamples );


  optimizer->SetLearningRate( optAffineLearningRate );
  optimizer->SetNumberOfIterations( optAffineNumberOfIterations );
  optimizer->MaximizeOn();

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );


  // Create the Command interface observer and register it with the optimizer.
  //
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  registration->AddObserver( itk::IterationEvent(), command );
  registration->SetNumberOfLevels( multiLevelAffine );

  std::cout << "Starting Registration with Affine Transform " << std::endl;

  //Add probe
  itk::TimeProbesCollectorBase collector;
  
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


  /** BSpline Registration */

  const unsigned int SplineOrder = 3;
  typedef ScalarType CoordinateRepType;

  typedef itk::BSplineDeformableTransform< CoordinateRepType,
                                           Dimension,
                                           SplineOrder >     BSplineTransformType;

  // Allocate bspline tranform array for low grid size
  typedef vector<BSplineTransformType::Pointer> BSplineTransformArrayType;
  BSplineTransformArrayType      bsplineTransformArrayLow(N);
  
  typedef BSplineTransformType::RegionType RegionType;
  typedef BSplineTransformType::SpacingType SpacingType;
  typedef BSplineTransformType::OriginType OriginType;




  // Get the latest transform parameters of affine transfom
  ParametersType affineParameters = registration->GetLastTransformParameters();
  ParametersType affineCurrentParameters(transformArray[0]->GetNumberOfParameters());
  // Update the transform parameters
  for( int i=0; i<N; i++)
  {
    for(int j=0; j<transformArray[0]->GetNumberOfParameters(); j++)
    {
      affineCurrentParameters[j]=affineParameters[i*transformArray[0]->GetNumberOfParameters()+j];
    }
    transformArray[i]->SetParametersByValue(affineCurrentParameters);
  }

  // Initialize the size of the parameters array
  registration->SetTransformParametersLength( (int) pow( (double) (bsplineGridSize+SplineOrder), (int) Dimension)*Dimension*N);

  // Allocate the vector holding the transform parameters
  typedef BSplineTransformType::ParametersType     BSplineParametersType;
  vector<BSplineParametersType> bsplineParametersArrayLow(N);

  
  /** Connect the compenents together */
  try
  {
    for( int i=0; i< N; i++ ){


      bsplineTransformArrayLow[i] = BSplineTransformType::New();
      RegionType bsplineRegion;
      RegionType::SizeType   gridSizeOnImage;
      RegionType::SizeType   gridBorderSize;
      RegionType::SizeType   totalGridSize;

      gridSizeOnImage.Fill( bsplineGridSize );
      gridBorderSize.Fill( SplineOrder );    // Border for spline order = 3 ( 1 lower, 2 upper )
      totalGridSize = gridSizeOnImage + gridBorderSize;

      bsplineRegion.SetSize( totalGridSize );

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

      for(unsigned int r=0; r<Dimension; r++)
      {
        //There was a floor here, is it a bug?
        spacing[r] *= ( static_cast<double>(fixedImageSize[r] - 1)  /
            static_cast<double>(gridSizeOnImage[r] - 1) );
        origin[r]  -=  spacing[r];
      }

      bsplineTransformArrayLow[i]->SetGridSpacing( spacing );
      bsplineTransformArrayLow[i]->SetGridOrigin( origin );
      bsplineTransformArrayLow[i]->SetGridRegion( bsplineRegion );


      const unsigned int numberOfParametersLow =
          bsplineTransformArrayLow[i]->GetNumberOfParameters();

      bsplineParametersArrayLow[i].SetSize( numberOfParametersLow );
      bsplineParametersArrayLow[i].Fill( 0.0 );

      bsplineTransformArrayLow[i]->SetBulkTransform(transformArray[i]);
      bsplineTransformArrayLow[i]->SetParameters( bsplineParametersArrayLow[i] );
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
  optimizerScales.SetSize( bsplineTransformArrayLow[0]->GetNumberOfParameters()*N);
  optimizerScales.Fill( 1.0 );
  optimizer->SetScales( optimizerScales );

  // Set optimizer parameters for bspline registration
  optimizer->SetLearningRate( optBsplineLearningRate );
  optimizer->SetNumberOfIterations( optBsplineNumberOfIterations );
  optimizer->MaximizeOn();


  metric->SetNumberOfSpatialSamples( (int) (numberOfPixels * numberOfSpatialSamplesBsplinePercentage) );

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


  /** Increase the resolution of the grid */
  BSplineTransformArrayType bsplineTransformArrayHigh(N);
  vector<BSplineParametersType> bsplineParametersArrayHigh(N);

  
  for(int level=1; level < numberOfResolutionLevel; level++)
  {
    /** Copy the last parameters */
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


    bsplineGridSize *= 2;
    registration->SetTransformParametersLength( (int) pow( (double) (bsplineGridSize+SplineOrder), (int) Dimension)*Dimension*N);

    // Set the parameters of the high resolution Bspline Transform
    for( int i=0; i<N; i++)
    {

      bsplineTransformArrayHigh[i] = BSplineTransformType::New();

      RegionType bsplineRegion;
      RegionType::SizeType   gridHighSizeOnImage;
      RegionType::SizeType   gridBorderSize;
      RegionType::SizeType   totalGridSize;

      
      gridBorderSize.Fill( SplineOrder );
      gridHighSizeOnImage.Fill( bsplineGridSize );
      totalGridSize = gridHighSizeOnImage + gridBorderSize;

      bsplineRegion.SetSize( totalGridSize );
    
      //imageArrayReader[i]->Update();

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
        spacingHigh[rh] *= ( static_cast<double>(fixedImageSize[rh] - 1)  /
            static_cast<double>(gridHighSizeOnImage[rh] - 1) );
        originHigh[rh]  -=  spacingHigh[rh];
      }

      bsplineTransformArrayHigh[i]->SetGridSpacing( spacingHigh );
      bsplineTransformArrayHigh[i]->SetGridOrigin( originHigh );
      bsplineTransformArrayHigh[i]->SetGridRegion( bsplineRegion );

      bsplineParametersArrayHigh[i].SetSize( bsplineTransformArrayHigh[i]->GetNumberOfParameters() );
      bsplineParametersArrayHigh[i].Fill( 0.0 );


      //  Now we need to initialize the BSpline coefficients of the higher resolution
      //  transform. 

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

      // parametersArrayHigh[i].Fill(0.0);
      bsplineTransformArrayHigh[i]->SetParameters( bsplineParametersArrayHigh[i] );

      // Set parameters of the high transform to low transform for the next level
      bsplineTransformArrayLow[i]->SetGridSpacing( spacingHigh );
      bsplineTransformArrayLow[i]->SetGridOrigin( originHigh );
      bsplineTransformArrayLow[i]->SetGridRegion( bsplineRegion );

      //  We now pass the parameters of the high resolution transform as the initial
      //  parameters to be used in a second stage of the registration process.

    

      // Software Guide : BeginCodeSnippet
      registration->SetInitialTransformParameters( bsplineTransformArrayHigh[i]->GetParameters() , i );
      registration->SetTransformArray( bsplineTransformArrayHigh[i], i );


    }

    std::cout << "Starting Registration with high resolution transform: " << std::endl;
    std::cout << "Resolution level " << level;
    std::cout << " Number Of parameters: " << bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N <<std::endl;

    optBsplineLearningRate = optBsplineLearningRate / 10 ;
    optimizer->SetLearningRate( optBsplineLearningRate );
    optimizer->SetNumberOfIterations( optBsplineNumberOfIterations );
    optimizer->MaximizeOn();

    //Reset the optimizer scales
    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( bsplineTransformArrayHigh[0]->GetNumberOfParameters()*N );
    optimizerScales.Fill( 1.0 );
    optimizer->SetScales( optimizerScales );

    registration->SetNumberOfLevels( multiLevelBsplineHigh );

    try
    {
      registration->StartRegistration();
      collector.Stop( "Registration" );
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return -1;
    }

  }



  BSplineParametersType finalParameters = registration->GetLastTransformParameters();
 
  
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  
  double bestValue = optimizer->GetValue();


  // Print out results
  //
  std::cout << "Result = " << std::endl;
  std::cout << " final parameters 0 = " << finalParameters[0]  << std::endl;
  std::cout << " final parameters 10 = " << finalParameters[10]  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  collector.Report();






  // Write the output images
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


  numberOfParameters = bsplineTransformArrayHigh[0]->GetNumberOfParameters();
  BSplineParametersType currentParameters(numberOfParameters);
  BSplineParametersType currentParameters2(numberOfParameters);


  //Update Transform Parameters
  for(int i=0; i<N; i++)
  {
    //copy current parameters
    for(int j=0; j<numberOfParameters; j++ )
    {
      currentParameters[j] = finalParameters[numberOfParameters*i + j];
    }

    bsplineTransformArrayHigh[i]->SetBulkTransform( transformArray[i] );
    bsplineTransformArrayHigh[i]->SetParametersByValue( currentParameters );
  }

  // Loop over images and write output images
  for(int i=0; i<N; i++)
  {

    resample->SetTransform( bsplineTransformArrayHigh[i] );
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

  /** Compute Mean Image */
  ResampleFilterType::Pointer resample2 = ResampleFilterType::New();

  typedef itk::AddImageFilter < ImageType, ImageType,
                                           ImageType > AddFilterType;

  //Mean of the registered images
  AddFilterType::Pointer addition = AddFilterType::New();
  // Mean Image of original images
  AddFilterType::Pointer addition2 = AddFilterType::New();


  //Set the first image
  resample->SetTransform( bsplineTransformArrayHigh[0] );
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
  resample2->SetTransform( bsplineTransformArrayHigh[1] );
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
  for(int i=2; i<N; i++)
  {

    resample->SetTransform( bsplineTransformArrayHigh[i] );
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
    resample->SetDefaultPixelValue( 1 );

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
      meanImageFname = outputFolder + "MeanRegisteredImage.png";
    }
    else
    {
      meanImageFname = outputFolder + "MeanRegisteredImage.mhd";
    }
    writer->SetFileName( meanImageFname.c_str() );
    writer->Update();
  }


  return 0;
}




int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType, int& multiLevelAffine, string& transformType, string& imageType  )
{

  //initialize parameters
  inputFolder = "";
  outputFolder = "";
  multiLevelAffine = 3;
  optimizerType = "GradientDescent";
  transformType = "Translation";
  //read parameters
  for(int i=1; i<argc; i++)
  {
    string dummy(argv[i]);
    if(dummy == "-i")
      inputFolder = argv[++i];
    else if (dummy == "-o")
      outputFolder = argv[++i];
    else if (dummy == "-m")
      multiLevelAffine = atoi(argv[++i]);
    else if (dummy == "-opt")
      optimizerType == argv[++i];
    else if (dummy == "-t")
      optimizerType == argv[++i];
    else if (dummy == "-imageType")
      imageType = argv[++i];
    else
      fileNames.push_back(dummy); // get file name
  }


  if( fileNames.size() < 2)
  {
    std::cerr << "Missing Image Names " << std::endl;
    std::cerr << "\t -i Input folder for images" << std::endl;
    std::cerr << "\t -f Output folder for registered images" << std::endl;
    std::cerr << "\t -m Level of Multiresolution" << std::endl;
    std::cerr << "\t -opt Optimizer Type" << std::endl;
    std::cerr << "\t -t Transform Type: Bspline Affine Translation" << std::endl;
    std::cerr << std::endl << "Usage: " << std::endl << "\t" << argv[0];
    std::cerr << " -i folder1/ -f output/ -o gradient -m 4 -t Affine ImageFile1  ImageFile2 ImageFile3 ... " << std::endl << std::endl;
    return 1;
  }
  else
    return 0;
}

