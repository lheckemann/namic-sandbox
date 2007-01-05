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
      //std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};


int main( int argc, char *argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " ImageFile1  ImageFile2 ImageFile3 ... " << endl;;
    std::cerr << "outputs are placed into the folder ./output/ " << endl;
    return 1;
    }

  const    unsigned int    Dimension = 2;
  typedef  float  PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef   float     InternalPixelType;

  typedef itk::TranslationTransform< double, Dimension > TransformType;
  typedef itk::GradientDescentOptimizer                  OptimizerType;
  typedef itk::LinearInterpolateImageFunction< 
                                    ImageType,
                                    double >           InterpolatorType;

  typedef itk::MultiImageRegistrationMethod< ImageType >    RegistrationType;

  typedef itk::VarianceMultiImageMetric< ImageType>    MetricType;



  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  int N = argc-1;
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
  for( int i=0; i< imageArrayReader.size(); i++ ){
    transformArray[i]     = TransformType::New();
    interpolatorArray[i]  = InterpolatorType::New();
    registration->SetTransformArray(     transformArray[i] ,i    );
    registration->SetInterpolatorArray(     interpolatorArray[i] ,i    );

    imageArrayReader[i] = FixedImageReaderType::New();
    imageArrayReader[i]->SetFileName(  argv[i+1] );
    normalizedFilterArray[i] = FixedNormalizeFilterType::New();
    gaussianFilterArray[i] = GaussianFilterType::New();
    gaussianFilterArray[i]->SetVariance( 2.0 );
    normalizedFilterArray[i]->SetInput( imageArrayReader[i]->GetOutput() );
    gaussianFilterArray[i]->SetInput( normalizedFilterArray[i]->GetOutput() );
    registration->SetImage(    gaussianFilterArray[i]->GetOutput() , i   );
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
  

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transformArray[0]->GetNumberOfParameters()*transformArray.size() );

  initialParameters.Fill(0.0);

  registration->SetInitialTransformParameters( initialParameters );
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  
  const unsigned int numberOfSamples = 
                        static_cast< unsigned int >( numberOfPixels * 0.02 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );

  optimizer->SetLearningRate( 10000.0 );
  optimizer->SetNumberOfIterations( 100 );
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

  int numberOfParameters = transformArray[0]->GetNumberOfParameters();
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

  string fname;
  ostringstream fnameStream;
  fnameStream << "output/" << argv[i+1] << "_output.png";
  fname = fnameStream.str();
  cout << "Writing " << fname << "   \tX= "<< finalParameters[2*i] << " Y= " <<finalParameters[2*i+1] <<endl;
  writer->SetFileName( fname.c_str() );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();
  }


  return 0;
}

