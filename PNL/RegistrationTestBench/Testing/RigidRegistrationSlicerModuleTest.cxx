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

#include "itkOrientedImage.h"
#include "itkOrientImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

#include "itkGradientDescentOptimizer.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkQuaternionRigidTransform.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"
#include "itkBinomialBlurImageFilter.h"

#include "itkTimeProbesCollectorBase.h"


// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
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
    std::cout << optimizer->GetValue() << std::endl;
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
  //
  // Command line processing
  //

  const    unsigned int  ImageDimension = 3;
  typedef  T1  FixedPixelType; //##
  typedef itk::OrientedImage<FixedPixelType, ImageDimension> FixedImageType;//##

  typedef itk::ImageFileReader<FixedImageType> FixedFileReaderType;//##
  typedef itk::OrientImageFilter<FixedImageType,FixedImageType> FixedOrientFilterType;//##

  typedef  T2  MovingPixelType;//##
  typedef itk::OrientedImage<MovingPixelType, ImageDimension> MovingImageType;//##

  typedef itk::ImageFileReader<MovingImageType> MovingFileReaderType;//##
  typedef itk::OrientImageFilter<MovingImageType,MovingImageType> MovingOrientFilterType;//##
  
  typedef itk::MattesMutualInformationImageToImageMetric<FixedImageType, MovingImageType>    MetricType; //##
  typedef itk::QuaternionRigidTransformGradientDescentOptimizer  OptimizerType;    
  typedef itk::LinearInterpolateImageFunction<MovingImageType, double>  InterpolatorType;//##
  typedef itk::ImageRegistrationMethod<FixedImageType,MovingImageType>  RegistrationType;//##
  typedef itk::QuaternionRigidTransform<double> TransformType;
  typedef OptimizerType::ScalesType OptimizerScalesType;
  typedef itk::ResampleImageFilter<MovingImageType,MovingImageType> ResampleType;//##
  typedef itk::LinearInterpolateImageFunction<MovingImageType, double> ResampleInterpolatorType;//##
  typedef itk::ImageFileWriter<MovingImageType> WriterType;//##
  typedef itk::ImageFileWriter<FixedImageType> FixedWriterType;//##
  typedef itk::ContinuousIndex<double, 3> ContinuousIndexType;


  // Add a time probe
  itk::TimeProbesCollectorBase collector;

  // Read the fixed and moving volumes
  //
  //
  typename FixedFileReaderType::Pointer fixedReader = FixedFileReaderType::New();
  fixedReader->SetFileName ( argv[1] );

  try
    {
    collector.Start( "Read fixed volume" );
    fixedReader->Update();
    collector.Stop( "Read fixed volume" );
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
    collector.Start( "Read moving volume" );
    movingReader->Update();
    collector.Stop( "Read moving volume" );
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Error Reading Moving image: " << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }


  // if an initial transform was specified, read it
  typedef itk::TransformFileReader TransformReaderType;
  TransformReaderType::Pointer initialTransform;

  // Set up the optimizer
  //
  //
  ScheduleCommand::Pointer Schedule = ScheduleCommand::New();

  std::vector<int> Iterations;
  std::vector<double> LearningRate;

  Iterations.push_back( atoi( argv[ 7] ) );
  Iterations.push_back( atoi( argv[ 8] ) );
  Iterations.push_back( atoi( argv[ 9] ) );
  Iterations.push_back( atoi( argv[10] ) );

  LearningRate.push_back( atof( argv[11] ) );
  LearningRate.push_back( atof( argv[12] ) );
  LearningRate.push_back( atof( argv[13] ) );
  LearningRate.push_back( atof( argv[14] ) );


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

  const double TranslationScale = atof( argv[15] );

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
  typename TransformType::InputPointType centerFixed;
  typename FixedImageType::RegionType::SizeType sizeFixed = fixedReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  // Find the center
  ContinuousIndexType indexFixed;
  for ( unsigned j = 0; j < 3; j++ )
    {
    indexFixed[j] = (sizeFixed[j]-1) / 2.0;
    }
  fixedReader->GetOutput()->TransformContinuousIndexToPhysicalPoint ( indexFixed, centerFixed );

  typename TransformType::InputPointType centerMoving;
  typename MovingImageType::RegionType::SizeType sizeMoving = movingReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  // Find the center
  ContinuousIndexType indexMoving;
  for ( unsigned j = 0; j < 3; j++ )
    {
    indexMoving[j] = (sizeMoving[j]-1) / 2.0;
    }
  movingReader->GetOutput()->TransformContinuousIndexToPhysicalPoint ( indexMoving, centerMoving );

  transform->SetCenter( centerFixed );
  transform->Translate(centerMoving-centerFixed);
  std::cout << "Centering transform: "; transform->Print( std::cout );

  // Set up the metric
  //
  const unsigned int HistogramBins = atoi( argv[5] );
  const unsigned int SpatialSamples = atoi( argv[6] );

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
  registration->SetNumberOfThreads( atoi( argv[17] ) );

  // Force an iteration event to trigger a progress event
  Schedule->SetRegistration( registration );
  
  try
    {
    collector.Start( "Register" );
    registration->Update();     
    collector.Stop( "Register" );
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

  std::string OutputTransform = argv[16];

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

  // Report the time taken by the registration
  collector.Report();

 
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

    // This filter handles all types
    
    switch (componentType)
      {
//     case itk::ImageIOBase::CHAR:
//     case itk::ImageIOBase::UCHAR:
      case itk::ImageIOBase::SHORT:
        return DoIt2( argc, argv, targ, static_cast<short>(0));
        break;
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::INT:
        return DoIt2( argc, argv, targ, static_cast<int>(0));
        break;
 //    case itk::ImageIOBase::UINT:
 //    case itk::ImageIOBase::ULONG:
 //      return DoIt2( argc, argv, targ, static_cast<unsigned long>(0));
 //      break;
 //    case itk::ImageIOBase::LONG:
 //      return DoIt2( argc, argv, targ, static_cast<long>(0));
 //      break;
 //    case itk::ImageIOBase::FLOAT:
 //      return DoIt2( argc, argv, targ, static_cast<float>(0));
 //      break;
 //    case itk::ImageIOBase::DOUBLE:
 //      return DoIt2( argc, argv, targ, static_cast<float>(0));
 //      break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_FAILURE;
}

} // end of anonymous namespace


int main( int argc, char * argv[] )
{
  // this line is here to be able to see the full output on the dashboard even
  // when the test succeeds (to see the reproducibility error measure)
  // std::cout << std::endl << "ctest needs: CTEST_FULL_OUTPUT" << std::endl;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  std::string FixedImageFileName = argv[1];

  try
    {
    itk::GetImageType (FixedImageFileName, pixelType, componentType);

    // This filter handles all types
    
    switch (componentType)
      {
 //    case itk::ImageIOBase::CHAR:
 //    case itk::ImageIOBase::UCHAR:
       case itk::ImageIOBase::SHORT:
         return DoIt( argc, argv, static_cast<short>(0));
         break;
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0));
        break;
 //   case itk::ImageIOBase::UINT:
 //   case itk::ImageIOBase::ULONG:
 //     return DoIt( argc, argv, static_cast<unsigned long>(0));
 //     break;
  //   case itk::ImageIOBase::LONG:
  //     return DoIt( argc, argv, static_cast<long>(0));
  //     break;
  //   case itk::ImageIOBase::FLOAT:
  //     return DoIt( argc, argv, static_cast<float>(0));
  //     break;
  //   case itk::ImageIOBase::DOUBLE:
  //     return DoIt( argc, argv, static_cast<float>(0));
  //     break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
  
