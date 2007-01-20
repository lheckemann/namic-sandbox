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
#include "itkAffineTransform.h"

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

#include <sstream>
#include <string>
#include <fstream>

#include "AddImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


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
      std::cout << "\tValue " <<optimizer->GetValue() << "   " << std::endl;
      /*
      if(optimizer->GetCurrentIteration() % 50 == 0)
      {
        std::ofstream output("output.txt");

        output << optimizer->GetCurrentPosition() << std::endl;
      }
      */
      
    }
};
// Function to read inputs
int getCommandLine(int argc, char *argv[], vector<string>& fileNames, string& inputFolder, string& outputFolder, string& optimizerType,                      int& multiLevel, string& transformType );

int main( int argc, char *argv[] )
{

  vector<string> fileNames;
  string inputFolder, outputFolder, optimizerType, transformType;
  int multiLevel;

  if( getCommandLine(argc,argv, fileNames, inputFolder, outputFolder, optimizerType, multiLevel, transformType  ) )
    return 1;

  const    unsigned int    Dimension = 2;
  typedef  float  PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef   float     InternalPixelType;
  //typedef itk::TranslationTransform< double, Dimension > TransformType;
  typedef itk::AffineTransform< double, Dimension > TransformType;
  //typedef itk::CenteredRigid2DTransform< double > TransformType;
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
  TransformArrayType      transformArray;
  typedef vector<InterpolatorType::Pointer>  InterpolatorArrayType;
  InterpolatorArrayType      interpolatorArray;
  transformArray.resize(N);
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


/** Connect the compenents together */
try
{
  for( int i=0; i< N; i++ ){
    transformArray[i]     = TransformType::New();
    interpolatorArray[i]  = InterpolatorType::New();
    registration->SetTransformArray(     transformArray[i] ,i    );
    registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );
    
    imageArrayReader[i] = FixedImageReaderType::New();
    imageArrayReader[i]->SetFileName( inputFileNames[i].c_str() );
    
    normalizedFilterArray[i] = FixedNormalizeFilterType::New();
    gaussianFilterArray[i] = GaussianFilterType::New();
    gaussianFilterArray[i]->SetVariance( 2.0 );
    normalizedFilterArray[i]->SetInput( imageArrayReader[i]->GetOutput() );
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



  normalizedFilterArray[0]->Update();
  ImageType::RegionType fixedImageRegion =
       normalizedFilterArray[0]->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedImageRegion );
  

  // Set initial parameters of the transform
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transformArray[0]->GetNumberOfParameters()*N);
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
    optimizerScales[i*numberOfParameters + 0] = 1.0; // scale for M11
    optimizerScales[i*numberOfParameters + 1] = 1.0; // scale for M12
    optimizerScales[i*numberOfParameters + 2] = 1.0; // scale for M21
    optimizerScales[i*numberOfParameters + 3] = 1.0; // scale for M22

    optimizerScales[i*numberOfParameters + 4] = 1.0 / 1000000.0; // scale for translation on X
    optimizerScales[i*numberOfParameters + 5] = 1.0 / 1000000.0; // scale for translation on Y
  }
  optimizer->SetScales( optimizerScales );

  
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  const unsigned int numberOfSamples = 
                        static_cast< unsigned int >( numberOfPixels * 0.01 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );


  optimizer->SetLearningRate( 5e-5 );
  optimizer->SetNumberOfIterations( 400 );
  optimizer->MaximizeOn();

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  

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

  ParametersType finalParameters = registration->GetLastTransformParameters();
  
  double TranslationAlongX = finalParameters[0];
  double TranslationAlongY = finalParameters[1];
  
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  
  double bestValue = optimizer->GetValue();



  std::cout << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " Translation X = " << TranslationAlongX  << std::endl;
  std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  std::cout << " Numb. Samples = " << numberOfSamples    << std::endl;




  typedef itk::ResampleImageFilter< 
                            ImageType, 
                            ImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();
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


  ParametersType currentParameters(numberOfParameters);
  


  for(int i=0; i<N; i++)
  {
  //copy current parameters
  for(int j=0; j<numberOfParameters; j++ )
    currentParameters[j] = finalParameters[numberOfParameters*i + j];

  finalTransform->SetParameters( currentParameters );
  resample->SetTransform( finalTransform );
  resample->SetInput( imageArrayReader[i]->GetOutput() );
  fixedImage = imageArrayReader[i]->GetOutput();
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetDefaultPixelValue( 100 );

  cout << "Writing " << outputFileNames[i] << std::endl;
  //cout << "   \tX= "<< finalParameters[2*i] << " Y= " <<finalParameters[2*i+1] <<std::endl;
  writer->SetFileName( outputFileNames[i].c_str() );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();
  }

  /** Compute Mean Image */
  ResampleFilterType::Pointer resample2 = ResampleFilterType::New();
  TransformType::Pointer finalTransform2 = TransformType::New();

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

  finalTransform->SetParameters( currentParameters );
  resample->SetTransform( finalTransform );
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
    currentParameters[j] = finalParameters[numberOfParameters*1 + j];
  finalTransform2->SetParameters( currentParameters );
  resample2->SetTransform( finalTransform2 );
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
      currentParameters[j] = finalParameters[numberOfParameters*i + j];
    finalTransform->SetParameters( currentParameters );
    resample->SetTransform( finalTransform );
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
  multiLevel = 1;
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

