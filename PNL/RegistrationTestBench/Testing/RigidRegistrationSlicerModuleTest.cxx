/*=========================================================================

  Program:   Registration stand-alone
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/RigidRegistration.cxx $
  Language:  C++
  Date:      $Date: 2009-05-16 12:00:18 -0400 (Sat, 16 May 2009) $
  Version:   $Revision: 9511 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.
=========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "itkPluginUtilities.h"

#include <iostream>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileWriter.h"

#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkQuaternionRigidTransform.h"

#include "itkCenteredTransformInitializer.h"

#include "itkTimeProbesCollectorBase.h"


namespace {


class ScheduleCommand : public itk::Command
{
 public:
  typedef ScheduleCommand Self;
  typedef itk::SmartPointer<Self>  Pointer;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ScheduleCommand,itk::Command);

  void SetLearningRates ( std::vector<double> &LearningRates )
    {
    m_LearningRates = LearningRates;
    }
  void SetNumberOfIterations ( std::vector<int> &NumberOfIterations )
    {
    m_NumberOfIterations = NumberOfIterations;
    this->m_NextChange = NumberOfIterations[0];
    }
  void SetSchedule ( std::vector<int> &NumberOfIterations,
                     std::vector<double> &LearningRates )
    {
    this->SetNumberOfIterations(NumberOfIterations);
    this->SetLearningRates(LearningRates);
    }
  void SetRegistration (itk::ProcessObject* reg)
    {
    m_Registration = reg; 
    }
  void DoExecute ( itk::GradientDescentOptimizer* optimizer ) 
    {
    if ( m_Schedule < m_NumberOfIterations.size()-1 )
      {
      if ( static_cast<int>(optimizer->GetCurrentIteration())
           >= this->m_NumberOfIterations[ m_Schedule ])
        {
        m_Schedule++;
        optimizer->SetLearningRate ( this->m_LearningRates[m_Schedule] );
        this->m_NextChange = optimizer->GetCurrentIteration()
          + this->m_NumberOfIterations[m_Schedule];
        std::cout << "Iteration: " << optimizer->GetCurrentIteration()
                  << " LearningRate: " << optimizer->GetLearningRate()
                  << std::endl;
        }
      }
    }
  void Execute ( itk::Object *caller, const itk::EventObject & event )
    {
    Execute( (const itk::Object *)caller, event);
    }
  void Execute ( const itk::Object *caller, const itk::EventObject & event )
    {
    itk::GradientDescentOptimizer* optimizer = (itk::GradientDescentOptimizer*)(const_cast<itk::Object *>(caller));

    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "  :  ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    if (m_Registration)
      {
      // for our purposes, an iteration even is a progress event
      m_Registration->UpdateProgress( static_cast<double>(optimizer->GetCurrentIteration()) /
                                      static_cast<double>(optimizer->GetNumberOfIterations()));
      }
                                        
    if ( optimizer->GetCurrentIteration() >= this->m_NextChange )
      {
      this->DoExecute ( optimizer );
      }
    }
  
 protected:
  std::vector<int> m_NumberOfIterations;
  std::vector<double> m_LearningRates;
  unsigned int m_Schedule;
  unsigned int m_NextChange;
  itk::ProcessObject::Pointer m_Registration;
  ScheduleCommand()
    {
    m_Schedule = 0;
    m_Registration = 0;
    }
  ~ScheduleCommand() {}
};
        

template<class T1, class T2> int DoIt2( int argc, char * argv[], const T1&, const T2& )
{
  const    unsigned int  ImageDimension = 3;
  typedef  T1  FixedPixelType; //##
  typedef itk::Image<FixedPixelType, ImageDimension> FixedImageType;//##

  typedef itk::ImageFileReader<FixedImageType> FixedFileReaderType;//##

  typedef  T2  MovingPixelType;//##
  typedef itk::Image<MovingPixelType, ImageDimension> MovingImageType;//##

  typedef itk::ImageFileReader<MovingImageType> MovingFileReaderType;//##
  
  typedef itk::MattesMutualInformationImageToImageMetric<FixedImageType, MovingImageType>    MetricType; //##

  typedef itk::QuaternionRigidTransformGradientDescentOptimizer  OptimizerType;    
  typedef itk::LinearInterpolateImageFunction<MovingImageType, double>  InterpolatorType;//##
  typedef itk::ImageRegistrationMethod<FixedImageType,MovingImageType>  RegistrationType;//##
  typedef itk::QuaternionRigidTransform<double> TransformType;
  typedef OptimizerType::ScalesType OptimizerScalesType;

  typedef itk::ImageFileWriter<MovingImageType> WriterType;//##
  typedef itk::ImageFileWriter<FixedImageType> FixedWriterType;//##
  typedef itk::ContinuousIndex<double, 3> ContinuousIndexType;


  typename FixedFileReaderType::Pointer fixedReader = FixedFileReaderType::New();
  fixedReader->SetFileName ( argv[1] );

  try
    {
    fixedReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Error Reading Fixed image: " << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  typename MovingFileReaderType::Pointer movingReader = MovingFileReaderType::New();
  movingReader->SetFileName ( argv[2] );

  try
    {
    movingReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Error Reading Moving image: " << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }


  ScheduleCommand::Pointer Schedule = ScheduleCommand::New();

  std::vector<int> Iterations;
  std::vector<double> LearningRate;

  Iterations.push_back( atoi( argv[5] ) );
  LearningRate.push_back( atof( argv[6] ) );

  Schedule->SetSchedule ( Iterations, LearningRate );

  int sum = 0;
  for (size_t i = 0; i < Iterations.size(); i++)
    {
    sum += Iterations[i];
    }

  typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
  optimizer->SetNumberOfIterations ( sum );
  optimizer->SetLearningRate ( LearningRate[0] );
  optimizer->AddObserver ( itk::IterationEvent(), Schedule );

  typename TransformType::Pointer transform = TransformType::New();
  typedef OptimizerType::ScalesType OptimizerScalesType;
  OptimizerScalesType scales( transform->GetNumberOfParameters() );

  const double TranslationScale = atof( argv[7] );

  scales.Fill ( 1.0 );
  for( unsigned j = 4; j < 7; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(TranslationScale);
    }
  optimizer->SetScales( scales );
  optimizer->SetMinimize ( true );


  // Initialize the transform
  //
  //
  typedef itk::CenteredTransformInitializer< TransformType, 
                                             FixedImageType, 
                                             MovingImageType 
                                                 >  TransformInitializerType;

  typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedReader->GetOutput() );
  initializer->SetMovingImage( movingReader->GetOutput() );
  initializer->GeometryOn();


  // Set up the metric
  //
  const unsigned int HistogramBins = atoi( argv[3] );
  const unsigned int SpatialSamples = atoi( argv[4] );

  typename MetricType::Pointer  metric        = MetricType::New();
  metric->SetNumberOfHistogramBins ( HistogramBins );
  metric->SetNumberOfSpatialSamples( SpatialSamples );
  metric->ReinitializeSeed(123);
  
  // Create the interpolator
  //
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

  // Set up the registration
  //
  typename RegistrationType::Pointer registration = RegistrationType::New();
  registration->SetTransform ( transform );
  registration->SetInitialTransformParameters ( transform->GetParameters() );
  registration->SetMetric ( metric );
  registration->SetOptimizer ( optimizer );
  registration->SetInterpolator ( interpolator );
  registration->SetFixedImage ( fixedReader->GetOutput() );
  registration->SetMovingImage ( movingReader->GetOutput() );
  registration->SetNumberOfThreads( atoi( argv[9] ) );

  // Force an iteration event to trigger a progress event
  Schedule->SetRegistration( registration );
  
  try
    {
    registration->Update();     
    } 
  catch( itk::ExceptionObject & err )
    {
    std::cout << err << std::endl;
    std::cerr << err << std::endl;
    return  EXIT_FAILURE ;
    } 
  catch ( ... )
    {
    return  EXIT_FAILURE ;
    }


  transform->SetParameters ( registration->GetLastTransformParameters() );

  std::string OutputTransform = argv[8];

  if (OutputTransform != "")
    {
    typedef itk::TransformFileWriter TransformWriterType;
    TransformWriterType::Pointer outputTransformWriter;

    outputTransformWriter= TransformWriterType::New();
    outputTransformWriter->SetFileName( OutputTransform );
    outputTransformWriter->SetInput( transform );
    try
      {
      outputTransformWriter->Update();
      }
    catch (itk::ExceptionObject &err)
      {
      std::cerr << err << std::endl;
      return EXIT_FAILURE ;
      }
    }

  return EXIT_SUCCESS ;
}
  

template<class T> int DoIt( int argc, char * argv[], const T& targ)
{
  
  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  std::string MovingImageFileName = argv[2];

  try
    {
    itk::GetImageType (MovingImageFileName, pixelType, componentType);
    return DoIt2( argc, argv, targ, static_cast<short>(0));
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_FAILURE;
}

} // end of anonymous namespace


int main( int argc, char * argv[] )
{
  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  std::string FixedImageFileName = argv[1];

  try
    {
    itk::GetImageType (FixedImageFileName, pixelType, componentType);

    return DoIt( argc, argv, static_cast<short>(0));
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
  