/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration2.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/20 13:27:53 $
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

//user defined headers
#include "MultiImageRegistrationMethod.h"
#include "VarianceMultiImageMetric.h"

#include "itkTranslationTransform.h"
//#include "itkMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGradientDescentOptimizer.h"
#include "itkImage.h"

#include "itkNormalizeImageFilter.h"

#include "itkDiscreteGaussianImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"

#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSOptimizer.h"
//#include "itkLBFGSBOptimizer.h"
    
#include "AddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkBSplineResampleImageFunction.h"
#include "itkIdentityTransform.h"
#include "itkBSplineDecompositionImageFilter.h"
    
#include <sstream>
#include <string>

#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command
{
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    CommandIterationUpdate() {};
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
      if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
        return;
      }
      std::cout << "Step " << optimizer->GetCurrentIteration() << "   ";
      std::cout << "\tValue " <<optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition()[0] << std::endl;
    }
};

// Function to read inputs
int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType, int& multiLevel, string& transformType );

int main( int argc, char *argv[] )
{
  vector<string> fileNames;
  string inputFolder, outputFolder, optimizerType, transformType;
  int multiLevel;
  int numberOfResolutionLevel = 4;
  int bsplineGridSize = 2;
  int bsplineOrder = 3;

  if( getCommandLine(argc,argv, fileNames, inputFolder, outputFolder, optimizerType, multiLevel, transformType  ) )
    return 1;


  const    unsigned int    Dimension = 2;
  typedef  float  PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef   float     InternalPixelType;

  const unsigned int SpaceDimension = Dimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform< CoordinateRepType, SpaceDimension, SplineOrder >     TransformType;
  //typedef itk::LBFGSOptimizer       OptimizerType;
  typedef itk::GradientDescentOptimizer                  OptimizerType;
  typedef itk::LinearInterpolateImageFunction<
                                    ImageType,
                                    double >           InterpolatorType;

  typedef itk::MultiImageRegistrationMethod< ImageType >    RegistrationType;

  typedef itk::VarianceMultiImageMetric< ImageType>    MetricType;



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
  TransformArrayType      transformArrayLow;
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray;
  transformArrayLow.resize(N);
  interpolatorArray.resize(N);



  MetricType::Pointer         metric        = MetricType::New();
  registration->SetMetric( metric  );

  metric->SetFixedImageStandardDeviation(  0.4 );
  metric->SetMovingImageStandardDeviation( 0.4 );




  typedef itk::ImageFileReader< ImageType  > FixedImageReaderType;
  typedef vector< FixedImageReaderType::Pointer > ImageArrayReader;
  ImageArrayReader imageArrayReader;
  imageArrayReader.resize(N);


  typedef itk::NormalizeImageFilter< 
                                ImageType, 
                                ImageType 
                                        > FixedNormalizeFilterType;
  typedef FixedNormalizeFilterType::Pointer FixedNormalizeFilterTypePointer;
  typedef vector<FixedNormalizeFilterTypePointer> NormalizedFilterArrayType;
  NormalizedFilterArrayType normalizedFilterArray;
  normalizedFilterArray.resize(N);


    

  typedef itk::DiscreteGaussianImageFilter<
                                      ImageType, 
                                      ImageType
                                                    > GaussianFilterType;
  typedef vector< GaussianFilterType::Pointer > GaussianFilterArrayType;
  GaussianFilterArrayType gaussianFilterArray;
  gaussianFilterArray.resize(N);

  typedef TransformType::ParametersType     ParametersType;
  vector<ParametersType> parametersArray;
  parametersArray.resize(N);

  typedef TransformType::RegionType RegionType;
  typedef TransformType::SpacingType SpacingType;
  typedef TransformType::OriginType OriginType;


/** Connect the compenents together */
try
{
  for( int i=0; i< imageArrayReader.size(); i++ ){

    interpolatorArray[i]  = InterpolatorType::New();
    registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );

    imageArrayReader[i] = FixedImageReaderType::New();
    imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
    
    normalizedFilterArray[i] = FixedNormalizeFilterType::New();
    gaussianFilterArray[i] = GaussianFilterType::New();
    gaussianFilterArray[i]->SetVariance( 2.0 );
    normalizedFilterArray[i]->SetInput( imageArrayReader[i]->GetOutput() );
    gaussianFilterArray[i]->SetInput( normalizedFilterArray[i]->GetOutput() );
    registration->SetImageArrayPointer(    gaussianFilterArray[i]->GetOutput() , i   );
    imageArrayReader[i]->Update();

    transformArrayLow[i]     = TransformType::New();
    RegionType bsplineRegion;
    RegionType::SizeType   gridSizeOnImage;
    RegionType::SizeType   gridBorderSize;
    RegionType::SizeType   totalGridSize;

    gridSizeOnImage.Fill( bsplineGridSize );
    gridBorderSize.Fill( SplineOrder );    // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = gridSizeOnImage + gridBorderSize;

    bsplineRegion.SetSize( totalGridSize );

    SpacingType spacing = imageArrayReader[i]->GetOutput()->GetSpacing();

    OriginType origin = imageArrayReader[i]->GetOutput()->GetOrigin();

    ImageType::RegionType fixedRegion = imageArrayReader[i]->GetOutput()->GetBufferedRegion();
    ImageType::SizeType fixedImageSize = fixedRegion.GetSize();

    for(unsigned int r=0; r<Dimension; r++)
    {
      spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
          static_cast<double>(gridSizeOnImage[r] - 1) );
      origin[r]  -=  spacing[r];
    }

    transformArrayLow[i]->SetGridSpacing( spacing );
    transformArrayLow[i]->SetGridOrigin( origin );
    transformArrayLow[i]->SetGridRegion( bsplineRegion );


    const unsigned int numberOfParametersLow =
        transformArrayLow[i]->GetNumberOfParameters();

    parametersArray[i].SetSize( numberOfParametersLow );
    parametersArray[i].Fill( 0.0 );

    transformArrayLow[i]->SetParameters( parametersArray[i] );
    registration->SetTransformArray(     transformArrayLow[i] ,i    );
  }
}   
catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return -1;
    } 



  normalizedFilterArray[0]->Update();
  ImageType::RegionType fixedImageRegion =
       normalizedFilterArray[0]->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );
  

  typedef TransformType::ParametersType ParametersType;
  ParametersType initialParameters( transformArrayLow[0]->GetNumberOfParameters()*N );

  initialParameters.Fill(0.0);

  registration->SetInitialTransformParameters( initialParameters );
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();

  const unsigned int numberOfSamples = 
                        static_cast< unsigned int >( numberOfPixels * 0.01 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );

  // Set Optimizer Parameters
  /*
  optimizer->SetGradientConvergenceTolerance( 1e-5 );
  optimizer->SetLineSearchAccuracy( 0.9 );
  optimizer->SetDefaultStepLength( 1.5 );
  optimizer->TraceOn();
  optimizer->SetMaximumNumberOfFunctionEvaluations( 1000 );
  */


  optimizer->SetLearningRate( 2000 );
  optimizer->SetNumberOfIterations( 50 );
  optimizer->MaximizeOn();

  

  // Add a time probe
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  
  std::cout << std::endl << "Starting Registration" << std::endl;

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
  TransformArrayType transformArrayHigh(N);
  vector<ParametersType> parametersArrayHigh(N);

  
  for(int level=0; level < numberOfResolutionLevel; level++)
  {
    /** Copy the last parameters */
    ParametersType parametersLow = registration->GetLastTransformParameters();

    int numberOfParametersLow = transformArrayLow[0]->GetNumberOfParameters();
    ParametersType currentParametersLow(numberOfParametersLow);
  
    for(int i=0; i<N; i++)
    {
      //copy current parameters
      for(int j=0; j<numberOfParametersLow; j++ )
        currentParametersLow[j] = parametersLow[numberOfParametersLow*i + j];

      transformArrayLow[i]->SetParametersByValue( currentParametersLow );
    }


    bsplineGridSize *= 2;
    registration->SetTransformParametersLength( (int) pow( (double) (bsplineGridSize+SplineOrder), (int) Dimension)*Dimension*N);
  
    for( int i=0; i<N; i++)
    {

      transformArrayHigh[i] = TransformType::New();

      RegionType bsplineRegion;
      RegionType::SizeType   gridHighSizeOnImage;
      RegionType::SizeType   gridBorderSize;
      RegionType::SizeType   totalGridSize;

      
      gridBorderSize.Fill( SplineOrder );
      gridHighSizeOnImage.Fill( bsplineGridSize );
      totalGridSize = gridHighSizeOnImage + gridBorderSize;

      bsplineRegion.SetSize( totalGridSize );
    
      imageArrayReader[i]->Update();

      SpacingType spacingHigh = imageArrayReader[i]->GetOutput()->GetSpacing();
      OriginType  originHigh  = imageArrayReader[i]->GetOutput()->GetOrigin();

      ImageType::RegionType fixedRegion = imageArrayReader[i]->GetOutput()->GetBufferedRegion();
      ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
    
      for(unsigned int rh=0; rh<Dimension; rh++)
      {
        spacingHigh[rh] *= floor( static_cast<double>(fixedImageSize[rh] - 1)  /
            static_cast<double>(gridHighSizeOnImage[rh] - 1) );
        originHigh[rh]  -=  spacingHigh[rh];
      }

      transformArrayHigh[i]->SetGridSpacing( spacingHigh );
      transformArrayHigh[i]->SetGridOrigin( originHigh );
      transformArrayHigh[i]->SetGridRegion( bsplineRegion );

      parametersArrayHigh[i].SetSize( transformArrayHigh[i]->GetNumberOfParameters() );
      parametersArrayHigh[i].Fill( 0.0 );


      //  Now we need to initialize the BSpline coefficients of the higher resolution
      //  transform. 

      int counter = 0;

      for ( unsigned int k = 0; k < SpaceDimension; k++ )
      {
        typedef TransformType::ImageType ParametersImageType;
        typedef itk::ResampleImageFilter<ParametersImageType,ParametersImageType> ResamplerType;
        ResamplerType::Pointer upsampler = ResamplerType::New();

        typedef itk::BSplineResampleImageFunction<ParametersImageType,double> FunctionType;
        FunctionType::Pointer function = FunctionType::New();

        typedef itk::IdentityTransform<double,SpaceDimension> IdentityTransformType;
        IdentityTransformType::Pointer identity = IdentityTransformType::New();

        upsampler->SetInput( transformArrayLow[i]->GetCoefficientImage()[k] );
        upsampler->SetInterpolator( function );
        upsampler->SetTransform( identity );
        upsampler->SetSize( transformArrayHigh[i]->GetGridRegion().GetSize() );
        upsampler->SetOutputSpacing( transformArrayHigh[i]->GetGridSpacing() );
        upsampler->SetOutputOrigin( transformArrayHigh[i]->GetGridOrigin() );

        typedef itk::BSplineDecompositionImageFilter<ParametersImageType,ParametersImageType>
            DecompositionType;
            DecompositionType::Pointer decomposition = DecompositionType::New();

        decomposition->SetSplineOrder( SplineOrder );
        decomposition->SetInput( upsampler->GetOutput() );
        decomposition->Update();

        ParametersImageType::Pointer newCoefficients = decomposition->GetOutput();

        // copy the coefficients into the parameter array
        typedef itk::ImageRegionIterator<ParametersImageType> Iterator;
        Iterator it( newCoefficients, transformArrayHigh[i]->GetGridRegion() );
        while ( !it.IsAtEnd() )
        {
          parametersArrayHigh[i][ counter++ ] = it.Get();
          ++it;
        }

      }

      // parametersArrayHigh[i].Fill(0.0);
      transformArrayHigh[i]->SetParameters( parametersArrayHigh[i] );

      // Set parameters of the high transform to low transform for the next level
      transformArrayLow[i]->SetGridSpacing( spacingHigh );
      transformArrayLow[i]->SetGridOrigin( originHigh );
      transformArrayLow[i]->SetGridRegion( bsplineRegion );

      //  We now pass the parameters of the high resolution transform as the initial
      //  parameters to be used in a second stage of the registration process.

    

      // Software Guide : BeginCodeSnippet
      registration->SetInitialTransformParameters( parametersArrayHigh[i] , i );
      registration->SetTransformArray( transformArrayHigh[i], i );


      //  Typically, we will also want to tighten the optimizer parameters
      //  when we move from lower to higher resolution grid.

    }
  
    std::cout << "Starting Registration with high resolution transform: resolution level " << level << std::endl;
    //optimizer->SetLearningRate( 1e-3 );
    //optimizer->SetNumberOfIterations( 1 );
    optimizer->MaximizeOn();

    //Reset the optimizer scales
    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( transformArrayHigh[0]->GetNumberOfParameters()*N );
    optimizerScales.Fill( 1.0 );
    optimizer->SetScales( optimizerScales );
  
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
  // Software Guide : EndCodeSnippet



  ParametersType finalParameters = registration->GetLastTransformParameters();
  
  
  double bestValue = optimizer->GetValue();



  std::cout << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  std::cout << " Numb. Samples = " << numberOfSamples    << std::endl;
  std::cout << " Optimizervalue 1 " << finalParameters[0] << std::endl;



  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            ImageType >    ResampleFilterType;


  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  ImageType::Pointer fixedImage;
  typedef  unsigned char  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        ImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;  

  
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster = CastFilterType::New();

  int numberOfParameters = transformArrayHigh[0]->GetNumberOfParameters();
  ParametersType currentParameters(numberOfParameters);
  ParametersType currentParameters2(numberOfParameters);
  
  /** write output images */
  for(int i=0; i<N; i++)
  {
    //copy current parameters
    for(int j=0; j<numberOfParameters; j++ )
      currentParameters[j] = finalParameters[numberOfParameters*i + j];

    transformArrayHigh[i]->SetParameters( currentParameters );
    resample->SetTransform( transformArrayHigh[i] );
    resample->SetInput( imageArrayReader[i]->GetOutput() );
    resample->SetSize(    imageArrayReader[i]->GetOutput()->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(  imageArrayReader[i]->GetOutput()->GetOrigin() );
    resample->SetOutputSpacing( imageArrayReader[i]->GetOutput()->GetSpacing() );
    resample->SetDefaultPixelValue( 100 );

    cout << "Writing " << outputFileNames[i] << endl;
    writer->SetFileName( outputFileNames[i].c_str() );
    caster->SetInput( resample->GetOutput() );
    writer->SetInput( caster->GetOutput()   );
    writer->Update();
  }

  /** Compute Mean Image */
  ResampleFilterType::Pointer resample2 = ResampleFilterType::New();

  typedef itk::AddImageFilter <
      ImageType,
  ImageType,
  ImageType > AddFilterType;

  //Mean of the registered images
  AddFilterType::Pointer addition = AddFilterType::New();
  // Mean Image of original images
  AddFilterType::Pointer addition2 = AddFilterType::New();


  //Set the first image
  for(int j=0; j<numberOfParameters; j++ )
    currentParameters[j] = finalParameters[numberOfParameters*0 + j];

  transformArrayHigh[0]->SetParametersByValue( currentParameters );
  resample->SetTransform( transformArrayHigh[0] );
  resample->SetInput( imageArrayReader[0]->GetOutput() );
  fixedImage = imageArrayReader[0]->GetOutput();
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetDefaultPixelValue( 1 );
  addition->SetInput1( resample->GetOutput() );
  addition2->SetInput1(imageArrayReader[0]->GetOutput() );
  //Set the second image
  for(int j=0; j<numberOfParameters; j++ )
    currentParameters2[j] = finalParameters[numberOfParameters*1 + j];
  transformArrayHigh[1]->SetParametersByValue( currentParameters2 );
  resample2->SetTransform( transformArrayHigh[1] );
  resample2->SetInput( imageArrayReader[1]->GetOutput() );
  fixedImage = imageArrayReader[1]->GetOutput();
  resample2->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample2->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample2->SetOutputSpacing( fixedImage->GetSpacing() );
  resample2->SetDefaultPixelValue( 1 );
  addition->SetInput2( resample2->GetOutput() );
  addition2->SetInput2(imageArrayReader[1]->GetOutput() );
  addition->Update();
  addition2->Update();

  //Add other images
  for(int i=2; i<N; i++)
  {
    //copy current parameters
    for(int j=0; j<numberOfParameters; j++ )
    {
      currentParameters[j] = finalParameters[numberOfParameters*i + j];
    }
    transformArrayHigh[i]->SetParameters( currentParameters );
    resample->SetTransform( transformArrayHigh[i] );
    resample->SetInput( imageArrayReader[i]->GetOutput() );
    fixedImage = imageArrayReader[i]->GetOutput();
    resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(  fixedImage->GetOrigin() );
    resample->SetOutputSpacing( fixedImage->GetSpacing() );
    resample->SetDefaultPixelValue( 1 );

    addition->SetInput1( addition->GetOutput() );
    addition->SetInput2( resample->GetOutput() );
    addition->Update();

    addition2->SetInput1( addition2->GetOutput() );
    addition2->SetInput2( imageArrayReader[i]->GetOutput() );
    addition2->Update();

  }


  
  typedef itk::RescaleIntensityImageFilter<
      ImageType,
  ImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();
  
  intensityRescaler->SetInput( addition->GetOutput() );
  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  caster->SetInput( intensityRescaler->GetOutput() );
  writer->SetInput( caster->GetOutput()   );

  string meanImageFname = outputFolder + "MeanRegisteredImage.png";
  writer->SetFileName( meanImageFname.c_str() );
  writer->Update();

  intensityRescaler->SetInput( addition2->GetOutput() );

  caster->SetInput( intensityRescaler->GetOutput() );
  writer->SetInput( caster->GetOutput()   );

  string meanImageFname2 = outputFolder + "MeanOriginalImage.png";
  writer->SetFileName( meanImageFname2.c_str() );
  writer->Update();
  

  return 0;
}

int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType, int& multiLevel, string& transformType  )
{

  //initialize parameters
  inputFolder = "";
  outputFolder = "";
  multiLevel = 3;
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
      multiLevel = atoi(argv[++i]);
    else if (dummy == "-opt")
      optimizerType == argv[++i];
    else if (dummy == "-t")
      optimizerType == argv[++i];
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
