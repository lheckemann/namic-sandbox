/*=========================================================================

  Program:   Registration stand-alone
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include <itkCommand.h>
#include <itkImage.h>
#include <itkOrientedImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include "XMLHelp.h"

#define USE_TCLAP 0
#define USE_CLI 1
#define USE_KWSYS 0
#define USE_METACOMMAND 0


#include "cli/CLI.h"
#include "tclap/CmdLine.h"
using namespace TCLAP;
#include <metaCommand.h>

#if USE_KWSYS
#include <itksys/CommandLineArguments.hxx>
#endif

// ITK Stuff
// Registration
#include <itkCenteredTransformInitializer.h>
#include <itkGradientDescentOptimizer.h>
#include <itkImageFileWriter.h>
#include <itkImageRegistrationMethod.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkQuaternionRigidTransform.h>
#include <itkQuaternionRigidTransformGradientDescentOptimizer.h>
#include <itkResampleImageFilter.h>
#include <itkStdStreamLogOutput.h>
#include <itkLogger.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

class ScheduleCommand : public itk::Command
{
 public:
  typedef ScheduleCommand Self;
  typedef itk::SmartPointer<Self>  Pointer;
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ScheduleCommand,itk::Command);

  void SetLearningRates ( itk::Array<double> LearningRates ) {
    this->m_LearningRates = LearningRates;
  }
  void SetNumberOfIterations ( itk::Array<unsigned int> NumberOfIterations ) {
    this->m_NumberOfIterations = NumberOfIterations;
    this->m_NextChange = NumberOfIterations.GetElement ( 0 );
  }
  void SetSchedule ( itk::Array<unsigned int> NumberOfIterations, itk::Array<double> LearningRates )
  {
    this->m_NumberOfIterations = NumberOfIterations;
    this->m_LearningRates = LearningRates;
  }
  void DoExecute ( itk::GradientDescentOptimizer* optimizer ) 
  {
    if ( m_Schedule < m_NumberOfIterations.GetNumberOfElements()-1 ) {
      if ( optimizer->GetCurrentIteration() >= this->m_NumberOfIterations.GetElement ( m_Schedule ) ) {
        m_Schedule++;
        optimizer->SetLearningRate ( this->m_LearningRates.GetElement ( m_Schedule ) );
        this->m_NextChange = this->m_NumberOfIterations.GetElement ( m_Schedule );
        // std::cout << "Iteration: " << optimizer->GetCurrentIteration() << " LearningRate: " << optimizer->GetLearningRate() << std::endl;
      }
    }
  }
  void Execute ( itk::Object *caller, const itk::EventObject & event )
  {
    itk::GradientDescentOptimizer* obj = dynamic_cast<itk::GradientDescentOptimizer*>(caller);
    if ( obj->GetCurrentIteration() >= this->m_NextChange ) {
      this->DoExecute ( obj );
    }
  }
  void Execute ( const itk::Object *caller, const itk::EventObject & event )
  {
    itk::GradientDescentOptimizer* obj = (itk::GradientDescentOptimizer*)(caller);
    if ( obj->GetCurrentIteration() >= this->m_NextChange ) {
      this->DoExecute ( obj );
    }
  }
 protected:
  itk::Array<unsigned int> m_NumberOfIterations;
  itk::Array<double> m_LearningRates;
  unsigned int m_Schedule;
  unsigned int m_NextChange;
  ScheduleCommand()
  {
    m_Schedule = 0;
  }
  ~ScheduleCommand()
  {
  }
};
        
void Split(const char* str, const char* delim, std::vector<std::string>& lines)
{
  std::string data(str);
  std::string::size_type lpos = 0;
  while(lpos < data.length())
    {
    std::string::size_type rpos = data.find_first_of(delim, lpos);
    if(rpos == std::string::npos)
      {
      // Line ends at end of string without a newline.
      lines.push_back(data.substr(lpos));
      return;
      }
    // Line ends in a "\n", remove the character.
    lines.push_back(data.substr(lpos, rpos-lpos));
    lpos = rpos+1;
    }
  return;
}

template<typename T> itk::Array<T> Parse ( std::string s )
{
  itk::Array<T> values;
  std::vector<std::string> l;
  Split ( s.c_str(), ",", l );
  values.SetSize ( l.size() );
  for ( int i = 0; i < l.size(); i++ )
    {
    values[i] = (T) atof ( l[i].c_str() );
    }
  return values;
}

typedef itk::OrientedImage<signed short, 3> Volume;

int main ( int argc, const char* argv[] ) 
{  
  itk::OStringStream msg;
  itk::StdStreamLogOutput::Pointer coutput = itk::StdStreamLogOutput::New();
  coutput->SetStream(std::cout);
  itk::Logger::Pointer logger = itk::Logger::New();
  logger->SetName ( "CLRegistration" );
  logger->AddLogOutput ( coutput );

  bool DoInitializeTransform = true;
  int HistogramBins = 30;
  int RandomSeed = 1234567;
  int SpatialSamples = 10000;
  float TranslationScale = 100.0;
  itk::Array<unsigned int> Iterations(1);
  Iterations[0] = 200;
  itk::Array<double> LearningRate(1);
  LearningRate[0] = 0.05;
  double GradientMagnitudeTolerance = 1e-5;
  string fixedImageFileName;
  string movingImageFileName;
  string resampledImageFileName;


#if USE_TCLAP
  // This is the TCLAP version
  try
    {
    CmdLine cl ( "CLRegistration",
                 "CLRegistration registers 2 signed short volumes",
                 "$Revision: $" );
    
    msg.str ( "" ); msg << "Number of histogram bins (default: " << HistogramBins << ")";
    ValueArg<int> HistogramBinsArg ( "b", "histogrambins", msg.str(), false, HistogramBins, "integer", cl );
    
    msg.str ( "" ); msg << "Random seed (default: " << RandomSeed << ")";
    ValueArg<int> RandomSeedArg ( "d", "randomseed", msg.str(), false, RandomSeed, "int", cl );

    msg.str ( "" ); msg << "GradientMagnitudeTolerance (default: " << GradientMagnitudeTolerance << ")";
    ValueArg<double> GradToleranceArg ( "g", "gradtolerance", msg.str(), false, GradientMagnitudeTolerance, "double", cl );

    msg.str ( "" ); msg << "Number of iterations seperated by commas, e.g. 100,200,300 (default: " << Iterations[0] << ")";
    ValueArg<string> IterationsArg ( "i", "iterations", msg.str(), false, "100,100", "int[,int]", cl );

    msg.str ( "" ); msg << "Learning rate (default: " << LearningRate[0] << ")";
    ValueArg<string> LearningRateArg ( "l", "learningrate", msg.str(), false, "0.05,0.005", "double[,double]", cl );

    msg.str ( "" ); msg << "Number of spatial samples (default: " << SpatialSamples << ")";
    ValueArg<int> SpatialSamplesArg ( "s", "spatialsamples", msg.str(), false, SpatialSamples, "int", cl );

    msg.str ( "" ); msg << "Translation scaling for optimizer (default: " << TranslationScale << ")";
    ValueArg<float> TranslationScaleArg ( "t", "translationscale", msg.str(), false, TranslationScale, "float", cl );

    msg.str ( "" ); msg << "Calculate initial transform (default: " << DoInitializeTransform << ")";   
    SwitchArg DoInitializeTransformArg ( "u", "noinitializetransform", msg.str(), DoInitializeTransform, cl );
    
    UnlabeledValueArg<string> FixedImageArg ( "fixed", "Fixed image filename", true, "", "filename", cl );
    UnlabeledValueArg<string> MovingImageArg ( "moving", "Moving image filename", true, "", "filename", cl );
    UnlabeledValueArg<string> ResampledImageArg ( "resampled", "Resampled image filename", true, "", "filename", cl );
    cl.parse ( argc, (char**) argv );
    
    HistogramBins = HistogramBinsArg.getValue();
    DoInitializeTransform = DoInitializeTransformArg.getValue();
    Iterations = Parse<unsigned int> ( IterationsArg.getValue() );
    LearningRate = Parse<double> ( LearningRateArg.getValue() );
    RandomSeed = RandomSeedArg.getValue();
    SpatialSamples = SpatialSamplesArg.getValue();
    TranslationScale = TranslationScaleArg.getValue();
    GradientMagnitudeTolerance = GradToleranceArg.getValue();

    fixedImageFileName = FixedImageArg.getValue();
    movingImageFileName = MovingImageArg.getValue();
    resampledImageFileName = ResampledImageArg.getValue();
    
  } catch ( ArgException e ) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    exit ( EXIT_FAILURE );
  }
#endif

#if USE_CLI
  // Now parse using CLI4Cxx
  try
    {
    CLI::Options options;
    msg.str ( "" ); msg << "Number of histogram bins (default: " << HistogramBins << ")";
    options.addOption("b", "histogrambins", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Random seed (default: " << RandomSeed << ")";
    options.addOption ( "d", "randomseed", true, msg.str().c_str() );

    msg.str ( "" ); msg << "GradientMagnitudeTolerance (default: " << GradientMagnitudeTolerance << ")";
    options.addOption ( "g", "gradtolerance", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Number of iterations seperated by commas, e.g. 100,200,300 (default: " << Iterations[0] << ")";
    options.addOption ( "i", "iterations", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Learning rate (default: " << LearningRate[0] << ")";
    options.addOption ( "l", "learningrate", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Number of spatial samples (default: " << SpatialSamples << ")";
    options.addOption ( "s", "spatialsamples", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Translation scaling for optimizer (default: " << TranslationScale << ")";
    options.addOption ( "t", "translationscale", true, msg.str().c_str() );

    msg.str ( "" ); msg << "Calculate initial transform (default: " << DoInitializeTransform << ")";   
    options.addOption ( "u", "noinitializetransform", false, msg.str().c_str()  );

    msg.str ( "" ); msg << "Display help message and exit";
    options.addOption("h", "help", false, msg.str().c_str() );

    msg.str ( "" ); msg << "Format JSON description of the parameters";
    options.addOption("j", "json", false, msg.str().c_str() );

    msg.str ( "" ); msg << "Format XML description of the parameters";
    options.addOption("x", "xml", false, msg.str().c_str() );

    CLI::BasicParser parser;
    CLI::CommandLine cl( parser.parse(options, argc, argv) );

    // print help
    if ( cl.hasOption('x') )
      {
      std::cout << XMLHelp << std::endl;
      exit ( EXIT_SUCCESS );
      }
    if ( cl.hasOption('j') )
      {
      CLI::JSONHelpFormatter formatter;
      formatter.printJSONHelp ( "registration", "CLRegistration", "Register two volumes", "CLRegistration", options);
      exit ( EXIT_SUCCESS );
      }
    if ( cl.hasOption('h') )
      {
      CLI::HelpFormatter formatter;
      formatter.printHelp ( "Register two volumes", options);
      exit ( EXIT_SUCCESS );
      }

    HistogramBins = atoi ( cl.getOptionValue ( "b", "30" ).c_str() );
    RandomSeed = atoi ( cl.getOptionValue ( "d", "1234567" ).c_str() );
    GradientMagnitudeTolerance = atof ( cl.getOptionValue ( "g", "0.0001" ).c_str() );
    Iterations = Parse<unsigned int> ( cl.getOptionValue ( "i", "200" ) );
    LearningRate = Parse<double> ( cl.getOptionValue ( "l", "0.05" ) );
    SpatialSamples = atoi ( cl.getOptionValue ( "s", "10000" ).c_str() );
    TranslationScale = atof ( cl.getOptionValue ( "t", "100.0" ).c_str() );
    DoInitializeTransform = cl.hasOption('u');

    std::vector<std::string> args = cl.getArgs();
    if ( args.size() != 3 )
      {
      CLI::HelpFormatter formatter;
      formatter.printHelp("CLRegistration requires <fixed> <moving> <output> filenames", options);
      exit ( EXIT_FAILURE );
      }
    fixedImageFileName = args[0];
    movingImageFileName = args[1];
    resampledImageFileName = args[2];
    }
  catch (CLI::ParseException& e)
    {
    std::cerr << "error: " << e.what() << std::endl;
    exit ( EXIT_FAILURE );
    }
#endif


#if USE_KWSYS
  std::string IterationsString, LearningRateString;
  bool ShowHelp = false;
  itksys::CommandLineArguments arg;
  arg.Initialize(argc, argv);
  typedef itksys::CommandLineArguments argT;

  msg.str ( "" ); msg << "Number of histogram bins (default: " << HistogramBins << ")";
  arg.AddArgument( "--histogrambins", argT::SPACE_ARGUMENT, &HistogramBins, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Random seed (default: " << RandomSeed << ")";
  arg.AddArgument (  "--randomseed", argT::SPACE_ARGUMENT, &RandomSeed, msg.str().c_str() );

  msg.str ( "" ); msg << "GradientMagnitudeTolerance (default: " << GradientMagnitudeTolerance << ")";
  arg.AddArgument (  "--gradtolerance", argT::SPACE_ARGUMENT, &GradientMagnitudeTolerance, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Number of iterations seperated by commas, e.g. 100,200,300 (default: " << Iterations[0] << ")";
  arg.AddArgument (  "--iterations", argT::SPACE_ARGUMENT, &IterationsString, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Learning rate (default: " << LearningRate[0] << ")";
  arg.AddArgument (  "--learningrate", argT::SPACE_ARGUMENT, &LearningRateString, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Number of spatial samples (default: " << SpatialSamples << ")";
  arg.AddArgument (  "--spatialsamples", argT::SPACE_ARGUMENT, &SpatialSamples, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Translation scaling for optimizer (default: " << TranslationScale << ")";
  arg.AddArgument (  "--translationscale", argT::SPACE_ARGUMENT, &TranslationScale, msg.str().c_str() );
  
  msg.str ( "" ); msg << "Calculate initial transform (default: " << DoInitializeTransform << ")";   
  arg.AddArgument (  "--noinitializetransform", argT::NO_ARGUMENT, &DoInitializeTransform, msg.str().c_str()  );
  
  msg.str ( "" ); msg << "Display help message and exit";
  arg.AddArgument( "--help", argT::NO_ARGUMENT, &ShowHelp, msg.str().c_str() );
  if ( !arg.Parse() )
    {
    std::cerr << "Problem parsing arguments" << std::endl;
    exit ( EXIT_FAILURE );
    }

  if ( ShowHelp )
    {
    std::cout << arg.GetHelp() << std::endl;
    exit ( EXIT_SUCCESS );
    }
  Iterations = Parse<unsigned int> ( IterationsString );
  LearningRate = Parse<double> ( LearningRateString );

  char** LastArgs;
  int LastArgCount;
  arg.GetRemainingArguments ( &LastArgCount, &LastArgs );
  if ( LastArgCount != 3 )
    {
    std::cerr << "CLRegistration requires <fixed> <moving> <output> filenames" << std::endl;
    std::cerr << arg.GetHelp() << std::endl;
    exit ( EXIT_FAILURE );
    }
  fixedImageFileName = std::string ( LastArgs[0] );
  movingImageFileName = std::string ( LastArgs[1] );
  resampledImageFileName = std::string ( LastArgs[2] );
#endif  

#if USE_METACOMMAND
  MetaCommand command;
  msg.str ( "" ); msg << "Number of histogram bins (default: " << HistogramBins << ")";
  command.SetOption( "histogrambins", "b", false, msg.str().c_str() );
  command.AddOptionField ( "histogrambins", "Value", MetaCommand::INT, true, "30" );

  msg.str ( "" ); msg << "Random seed (default: " << RandomSeed << ")";
  command.SetOption (  "randomseed", "d", false, msg.str().c_str() );
  command.AddOptionField ( "randomseed", "Value", MetaCommand::INT, true, "1234567" );

  msg.str ( "" ); msg << "GradientMagnitudeTolerance (default: " << GradientMagnitudeTolerance << ")";
  command.SetOption (  "gradtolerance", "g", false, msg.str().c_str() );
  command.AddOptionField ( "gradtolerance", "Value", MetaCommand::FLOAT, true, "0.005" );
  
  msg.str ( "" ); msg << "Number of iterations seperated by commas, e.g. 100,200,300 (default: " << Iterations[0] << ")";
  command.SetOption (  "iterations", "i", false, msg.str().c_str() );
  command.AddOptionField ( "iterations", "Value", MetaCommand::STRING, true, "100,100" );

  msg.str ( "" ); msg << "Learning rate (default: " << LearningRate[0] << ")";
  command.SetOption (  "learningrate", "l", false, msg.str().c_str() );
  command.AddOptionField ( "learningrate", "Value", MetaCommand::STRING, true, "0.005,0.0005" );

  msg.str ( "" ); msg << "Number of spatial samples (default: " << SpatialSamples << ")";
  command.SetOption (  "spatialsamples", "s", false, msg.str().c_str() );
  command.AddOptionField ( "spatialsamples", "Value", MetaCommand::INT, true, "10000" );

  msg.str ( "" ); msg << "Translation scaling for optimizer (default: " << TranslationScale << ")";
  command.SetOption (  "translationscale", "t", false, msg.str().c_str() );
  command.AddOptionField ( "translationscale", "Value", MetaCommand::INT, true, "100" );

  msg.str ( "" ); msg << "Calculate initial transform (default: " << DoInitializeTransform << ")";   
  command.SetOption (  "noinitializetransform", "u", false, msg.str().c_str()  );
  command.AddOptionField ( "noinitializetransform", "Value", MetaCommand::FLAG, false, "1" );

  msg.str ( "" ); msg << "Display help message and exit";
  command.SetOption( "help", "h", false, msg.str().c_str() );
  command.AddOptionField ( "help", "Value", MetaCommand::FLAG, false, "1" );

  command.AddField ( "fixed", "Fixed Image Filename", MetaCommand::STRING, true );
  command.AddField ( "moving", "Moving Image Filename", MetaCommand::STRING, true );
  command.AddField ( "resampled", "Resampled Image Filename", MetaCommand::STRING, true );

  if ( !command.Parse ( argc, (char**)argv ) )
    {
    std::cerr << "error: " << std::endl;
    exit ( EXIT_FAILURE );
    }
  if ( command.GetOptionWasSet ( "help" ) )
    {
    command.ListOptionsSimplified();
    command.ListOptions();
    command.ListOptionsXML();
    exit ( EXIT_SUCCESS );
    }
    
  HistogramBins = command.GetValueAsInt ( "histogrambins", "Value" );
  RandomSeed = command.GetValueAsInt ( "randomseed", "Value" );
  GradientMagnitudeTolerance = command.GetValueAsFloat ( "gradientmagnitudetolerance", "Value" );
  Iterations = Parse<unsigned int> ( command.GetValueAsString ( "iterations", "Value" ) );
  LearningRate = Parse<double> ( command.GetValueAsString ( "learningrate", "Value" ) );
  SpatialSamples = command.GetValueAsInt ( "spatialsamples", "Value" );
  TranslationScale = command.GetValueAsFloat ( "translationscale", "Value" );
  DoInitializeTransform = command.GetOptionWasSet ( "doinitializetransform" );

  fixedImageFileName = command.GetValueAsString ( "fixed" );
  movingImageFileName = command.GetValueAsString ( "moving" );
  resampledImageFileName = command.GetValueAsString ( "resampled" );
    
#endif
  

  std::cout << "Parsed arguments" << std::endl
            << "HistogramBins: " << HistogramBins << std::endl
            << "RandomSeed: " << RandomSeed << std::endl
            << "GradientMagnitudeTolerance: " << GradientMagnitudeTolerance << std::endl
            << "Iterations: " << Iterations << std::endl
            << "LearningRate: " << LearningRate << std::endl
            << "SpatialSamples: " << SpatialSamples << std::endl
            << "TranslationScale: " << TranslationScale << std::endl
            << "DoInitializeTransform: " << DoInitializeTransform << std::endl
            << "fixedImageFileName: " << fixedImageFileName << std::endl
            << "movingImageFileName: " << movingImageFileName << std::endl
            << "resampledImageFileName: " << resampledImageFileName << std::endl
            << std::endl;
  typedef itk::ImageFileWriter<Volume> WriterType;
  WriterType::Pointer resampled = WriterType::New();
  resampled->SetFileName ( resampledImageFileName.c_str() );

  Volume::Pointer fixed, moving;
  typedef itk::ImageFileReader<Volume> FileReaderType;
  FileReaderType::Pointer FixedReader = FileReaderType::New();
  FileReaderType::Pointer MovingReader = FileReaderType::New();
  FixedReader->SetFileName ( fixedImageFileName.c_str() );
  MovingReader->SetFileName ( movingImageFileName.c_str() );

  try
    {
    FixedReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    msg.str(""); msg << "Error Reading Fixed image: " << err;
    logger->Write ( itk::LoggerBase::CRITICAL, msg.str() );
    return EXIT_FAILURE;
    }

  // read in the moving image and do nothing
  try
    {
    MovingReader->Update();
    moving = MovingReader->GetOutput();
    }
  catch( itk::ExceptionObject & err )
    {
    msg.str(""); msg << "Error Reading Moving image: " << err;
    logger->Write ( itk::LoggerBase::CRITICAL, msg.str() );
    return EXIT_FAILURE;
    }

  fixed = FixedReader->GetOutput();
  moving = MovingReader->GetOutput();
  
  typedef itk::QuaternionRigidTransform< double > TransformType;
  typedef itk::QuaternionRigidTransformGradientDescentOptimizer OptimizerType;
  typedef itk::MattesMutualInformationImageToImageMetric<Volume,Volume> MetricType;
  typedef itk::LinearInterpolateImageFunction<Volume,double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<Volume,Volume> RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  TransformType::Pointer  transform = TransformType::New();

  registration->SetMetric ( metric );
  registration->SetOptimizer ( optimizer );
  registration->SetInterpolator ( interpolator );
  registration->SetTransform ( transform );
  registration->SetFixedImage ( fixed );
  registration->SetMovingImage ( moving );

  ScheduleCommand::Pointer Schedule = ScheduleCommand::New();
  Schedule->SetSchedule ( Iterations, LearningRate );
  optimizer->SetNumberOfIterations ( Iterations.sum() );
  optimizer->SetLearningRate ( LearningRate[0] );
  optimizer->AddObserver ( itk::IterationEvent(), Schedule );
  typedef OptimizerType::ScalesType OptimizerScalesType;
  OptimizerScalesType scales( transform->GetNumberOfParameters() );
  scales.Fill ( 1.0 );
  for( unsigned j = 4; j < 7; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(TranslationScale);
    }
  optimizer->SetScales( scales );
  optimizer->SetMinimize ( true );

  // Initialize the transform
  TransformType::InputPointType center;
  Volume::RegionType::SizeType s = fixed->GetLargestPossibleRegion().GetSize();
  // Find the center
  Volume::IndexType centerIndex;
  for ( unsigned j = 0; j < 3; j++ )
    {
    centerIndex[j] = (long) ( (s[j]-1) / 2.0 );
    }
  fixed->TransformIndexToPhysicalPoint ( centerIndex, center );
  msg.str(""); msg << "Index: " << centerIndex << " to point: " << center;
  logger->Write ( itk::LoggerBase::INFO, msg.str() );
  transform->SetCenter ( center );
  transform->SetIdentity();
  registration->SetInitialTransformParameters ( transform->GetParameters() );


  
  metric->SetNumberOfHistogramBins ( HistogramBins );
  metric->SetNumberOfSpatialSamples( SpatialSamples );

  try
    {
    registration->StartRegistration();     
    logger->Write ( itk::LoggerBase::INFO, "Registration finished" );
    } 
  catch( itk::ExceptionObject & err )
    {
    msg.str(""); msg << err;
    logger->Write ( itk::LoggerBase::CRITICAL, msg.str() );

    exit ( EXIT_FAILURE );
    } 
  catch ( ... )
    {
    logger->Write ( itk::LoggerBase::FATAL, "Something bad!!!" );
    exit ( EXIT_FAILURE );
    }

  typedef itk::ResampleImageFilter<Volume,Volume> ResampleType;
  ResampleType::Pointer Resample = ResampleType::New();

  typedef itk::LinearInterpolateImageFunction<Volume, double> ResampleInterpolatorType;
  ResampleInterpolatorType::Pointer Interpolator = ResampleInterpolatorType::New();

  Resample->SetInput ( moving ); 
  transform->SetParameters ( registration->GetLastTransformParameters() );

  fixed->Print ( std::cout );
  moving->Print ( std::cout );
  transform->Print ( std::cout );
  
  Resample->SetTransform ( transform );
  Resample->SetInterpolator ( Interpolator );
  Resample->SetOutputParametersFromImage ( fixed );
  
  Resample->Update();
  resampled->SetInput ( Resample->GetOutput() );
  try {
    resampled->Write();
  } catch( itk::ExceptionObject & err ) { 
    cerr << err << endl;
    exit ( EXIT_FAILURE );
  }
  
  exit ( EXIT_SUCCESS );
}
  
