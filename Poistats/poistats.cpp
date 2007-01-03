#include <iostream>
#include <string>

#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>

#include <itkBSplineInterpolateImageFunction.h>

#include <itkGDCMImageIO.h>
#include <itkNiftiImageIO.h>
#include "itkPoistatsFilter.h"

#include "FreeDiffusionExecutable.h"
#include "CommandParser.h"

/**
 * C++ replacement of the matlab Poistats.
 */
class Poistats : public FreeDiffusionExecutable {

private:
  // required arguments
  const char *m_InputStem;
  const char *m_OutputDir;
  const char *m_Seeds;
  const char *m_SampleStem;  
  int m_nControlPoints;
  int m_Sigma;
  
  // optional arguments
  char *m_MaskStem;
  int m_nSamples;
  std::vector< int > m_SeedValues;
  int m_nReplicas;

  char *m_HeaderFileName;
  
  double m_ReplicaExchangeProbability;

  double m_SigmaTimeConstant;  
  
  double m_PointsToImageGamma;
  
public:

  // required
  static const std::string FLAG_INPUT_STEM;
  static const std::string FLAG_OUTPUT_DIRECTORY;
  static const std::string FLAG_SEEDS;
  static const std::string FLAG_SAMPLE_STEM;
  static const std::string FLAG_NUM_CONTROL_POINTS;
  static const std::string FLAG_SIGMA;

  // optional
  static const std::string FLAG_MASK_STEM;
  static const std::string FLAG_NUM_SAMPLES;
  static const std::string FLAG_SEED_VALUES;
  
  static const std::string FLAG_NUM_REPLICAS;
    
  static const int DEFAULT_NUM_SEEDS = 100;

  static const std::string FLAG_HEADER_FILE;
  static const std::string NORMAL_A;
  static const std::string NORMAL_S;

  static const std::string FLAG_REPLICA_EXCHANGE_PROBABILITY;
  static const std::string FLAG_SIGMA_TIME_CONSTANT;
  
  static const std::string FLAG_POINTS_TO_IMAGE_GAMMA;
  
  Poistats( int inArgs, char ** iaArgs );
  ~Poistats();
    
  /**
   * Fills in the arguments and returns true of the arguments can be filled.
   */
  bool FillArguments();    
  void Run();
  
  double ReadField( std::string fileName, std::string field );
      
};

// required flags
const std::string Poistats::FLAG_INPUT_STEM = "-i";
const std::string Poistats::FLAG_OUTPUT_DIRECTORY = "-o";
const std::string Poistats::FLAG_SEEDS = "-seeds";
const std::string Poistats::FLAG_SAMPLE_STEM = "-sample";
const std::string Poistats::FLAG_NUM_CONTROL_POINTS = "-nc";
const std::string Poistats::FLAG_SIGMA = "-sigma";

// optional flags
const std::string Poistats::FLAG_MASK_STEM = "-m";
const std::string Poistats::FLAG_NUM_SAMPLES = "-ns";
const std::string Poistats::FLAG_SEED_VALUES = "-seednums";
const std::string Poistats::FLAG_NUM_REPLICAS = "-nreplicas";

// header fields
const std::string Poistats::FLAG_HEADER_FILE = "-header";
const std::string Poistats::NORMAL_A = "normal_a:";
const std::string Poistats::NORMAL_S = "normal_s:";

// these are new additions for playing with the parameter space
const std::string Poistats::FLAG_REPLICA_EXCHANGE_PROBABILITY = "-exchangeprob";
const std::string Poistats::FLAG_SIGMA_TIME_CONSTANT = "-timeconst";
const std::string Poistats::FLAG_POINTS_TO_IMAGE_GAMMA = "-gamma";

Poistats::Poistats( int inArgs, char ** iaArgs ) : 
  FreeDiffusionExecutable( inArgs, iaArgs ) {
    
  SetName( "poistats", "find optimal path in tensor volume" );  

  SetNextRequiredArgument( FLAG_INPUT_STEM, "dtensorinstem", 
    "dtensor input stem", "dtensor", "must specify a dtensor input filename" );
  SetNextRequiredArgument( FLAG_OUTPUT_DIRECTORY, "outdir", "", "poistats",
    "must specify an output directory" );
  SetNextRequiredArgument( FLAG_SEEDS, "seedstem", 
    "Volume containing numerical labels to use as seed regions.", "seedvol",
    "must specify a seed volume" );
  SetNextRequiredArgument( FLAG_SAMPLE_STEM, "samplestem", 
    "Instem for volume to sample. For example: fa, trace", "fa", 
    "must specify a sampling volume" );
  SetNextRequiredArgument( FLAG_NUM_CONTROL_POINTS, "ncontrolpoints", 
    "Number of control points used to describe path. Number should be approximately the number of 'turns' in the path. Almost always 1 or 2.", 
    "2", "must specify number of control points" );
  SetNextRequiredArgument( FLAG_SIGMA, "sigmasize", 
    "Search distance for path control points in units voxels. The search distance should be approximately the distance between the midpoint of the seed path and the target location", 
    "10", "must specify search distance" );

  SetNextOptionalArgument( FLAG_MASK_STEM, "maskstem", 
    "Instem for mask. The path will not be allowed to contact the mask. The mask can be used to specify invalid regions, e.g., CSF" );
  SetNextOptionalArgument( FLAG_NUM_SAMPLES, "nsamplepoints", 
    "Number of points to sample along path from sample volume. For example, -ns 100 will sample 100 values along the path. Default: 100" );
  SetNextOptionalArgument( FLAG_SEED_VALUES, "seednumvalue", 
    "Use <seednumvalue> to define seed region. Eg, -seednums \"1 2\"" );
  SetNextOptionalArgument( FLAG_NUM_REPLICAS, "nreplicas", 
    "Use <nreplicas> to specify the number of replicas.  For example, -nreplicas 100 will spawn 100 replicas. Default: 100" );
  SetNextOptionalArgument( FLAG_HEADER_FILE, "header", "Read in bheader file.  This is a hack to let me read in the normal_a and normal_s values." );

  SetNextOptionalArgument( FLAG_REPLICA_EXCHANGE_PROBABILITY, "exchangeprob", 
    "Replica exchange probability.  Default: 0.05" );
  SetNextOptionalArgument( FLAG_SIGMA_TIME_CONSTANT, "timeconst", 
    "Sigma time constant.  Default: 200." );
  SetNextOptionalArgument( FLAG_POINTS_TO_IMAGE_GAMMA, "gamma", 
    "Points to image gamma.  Default: 0.5." );

  std::string output = "";
  output = output + 
    "PathDensity.nii - path probability density" + "\n\t" +
    "OptimalPathDensity.nii - probability density of optimal path" + "\n\n\t" +

    "OptimalPathSamples.txt - values of sample volume along optimal path" + "\n\t" +
    "OptimalPathProbabilities.txt - probability values along optimal path" + "\n\t" +
    "OptimalPath.txt - coordinates of optimal path";

  SetOutput( output );
  
  SetChildren( "matlab: poistats" );
  SetVersion( "Beta 1.5" );
  SetAuthor( "David Tuch" );
  SetBugEmail( "martinos-tech@yahoogroups.com" );
}

Poistats::~Poistats() {
  
}

bool
Poistats::FillArguments() {
  bool isFilled = false;
  
  std::string *requiredArguments = GetRequiredArguments();
  
  try {
    m_InputStem = requiredArguments[0].c_str();
    m_OutputDir = requiredArguments[1].c_str();
    m_Seeds = requiredArguments[2].c_str();
    m_SampleStem = requiredArguments[3].c_str();
    
    m_nControlPoints = atoi( requiredArguments[4].c_str() );
    m_Sigma = atoi( requiredArguments[5].c_str() );
    
    isFilled = true;    
  } catch(...) {
    isFilled = false;
  }    
  
  if( isFilled ) {
    // optional parameters
    m_MaskStem = m_Parser->GetArgument( FLAG_MASK_STEM.c_str() );
  
    m_nSamples = m_Parser->GetArgumentInt( FLAG_NUM_SAMPLES.c_str() );
    if( m_nSamples <= 0) {
      m_nSamples == DEFAULT_NUM_SEEDS;
      std::cout << "INFO: setting nsamplepoints=" << m_nSamples << std::endl;
    }
    
    m_SeedValues = m_Parser->GetArgumentIntVector( FLAG_SEED_VALUES.c_str() );

    m_HeaderFileName = m_Parser->GetArgument( FLAG_HEADER_FILE.c_str() );
    
    m_nReplicas = m_Parser->GetArgumentInt( FLAG_NUM_REPLICAS.c_str() );
    
    m_ReplicaExchangeProbability = m_Parser->GetArgumentDouble( 
      FLAG_REPLICA_EXCHANGE_PROBABILITY.c_str() );

    m_SigmaTimeConstant = m_Parser->GetArgumentDouble( 
      FLAG_SIGMA_TIME_CONSTANT.c_str() );

    m_PointsToImageGamma = m_Parser->GetArgumentDouble( 
      FLAG_POINTS_TO_IMAGE_GAMMA.c_str() );
    
  }
      
  // TODO: delete requiredArguments  
  
  return isFilled;
}

double 
Poistats::ReadField( std::string fileName, std::string fieldName ) {
  std::string s;
  std::ifstream headerFile( fileName.c_str() );
  
  double field = 0.0;
  while( headerFile >> s ) {
    
    if( s == fieldName ) {
      
      headerFile >> s;
      
      std::stringstream stream;
      stream << s;
      stream >> field;
    }
    
  }
  
  return field;
}


void
Poistats::Run() {
//  // run the poistats itk algorithm here
  std::cerr << "C++ Poistats -- Version " << GetVersion() << std::endl;

  typedef itk::Image< float, 4 > DtImageType;
  typedef itk::Image< float, 3 > OutputImageType;
  typedef itk::PoistatsFilter< DtImageType, OutputImageType > 
    PoistatsFilterType;
  PoistatsFilterType::Pointer poistatsFilter = PoistatsFilterType::New();

  // load the tensor volume
  typedef itk::ImageFileReader< PoistatsFilterType::InputImageType > DtiReaderType;  
  DtiReaderType::Pointer dtiReader = DtiReaderType::New();
  dtiReader->SetFileName( m_InputStem );
  
  std::cerr << "reading dti..." << std::endl;
  try { 
    dtiReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error reading the series." << std::endl;
    std::cerr << excp << std::endl;
  }
  
  poistatsFilter->SetInput( dtiReader->GetOutput() );
  
  DtImageType* tmpImage = dtiReader->GetOutput();
  std::cerr << tmpImage->GetSpacing() ;
  
  // TODO: read in the normals:
//  typedef itk::MetaDataDictionary DictionaryType;
//  
//  std::cerr << "getting dictionary: " << std::endl;
//  const DictionaryType &dictionary = dtiReader->GetMetaDataDictionary();
//  typedef itk::MetaDataObject< std::string > MetaDataStringType;  

//////  
//  DictionaryType::ConstIterator itr = dictionary.Begin();
//  DictionaryType::ConstIterator end = dictionary.End();
//  
//   while( itr != end )
//    {
//    itk::MetaDataObjectBase::Pointer  entry = itr->second;
//
//    MetaDataStringType::Pointer entryvalue =
//      dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
//      
//    std::cerr << "\n  entry -- " << itr->first << std::endl;
//    std::cerr << "    second -- " << itr->second << std::endl;
//    std::cerr << "    value -- " << entryvalue << std::endl;
//
//    std::string labelId;
//    itk::GDCMImageIO::GetLabelFromTag( itr->first, labelId );
//    
//    std::cerr << "    label -- " << labelId << std::endl;
//    std::cerr << "    object value -- " << entryvalue->GetMetaDataObjectValue() << std::endl;
//    ++itr;
// 
//    } 
//////

//////
//    std::cerr << "image reader: " << dtiReader << std::endl;
//
//    DtImageType::Pointer dtImage = dtiReader->GetOutput();
//    std::cerr << "image: " << dtImage << std::endl;
//
//    std::vector< std::string > keys = dictionary.GetKeys();
//    for( int i=0; i< keys.size(); i++ ) {
//      std::cerr << "key: " << keys[ i ] << std::endl; 
//    }
//
//    std::string entryId = "z_a";
//    DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );
//    MetaDataStringType::ConstPointer entryvalue =
//    dynamic_cast<const MetaDataStringType *>(
//                                   tagItr->second.GetPointer() );
//    std::string tagvalue = entryvalue->GetMetaDataObjectValue();
//    std::cout << "Patient's Name (" << entryId <<  ") ";
//    std::cout << " is: " << tagvalue.c_str() << std::endl;
////

/////    
//  itk::MetaDataObjectBase::Pointer entry = dictionary.Find( NORMAL_A )->second;
//  std::cerr << "entry: " << entry << std::endl;
//  
//  MetaDataStringType::Pointer entryValue =
//    dynamic_cast< MetaDataStringType * >( entry.GetPointer() ) ;
//  
//  std::cerr << "normal a: " << entryValue << std::endl;
//  std::cerr << "normal s: " << dictionary[ NORMAL_S ] << std::endl;
  
//  const double normalA = static_cast< double >( dictionary[ "normal_a" ] );
//  const double normalS = dictionary[ "normal_s" ];
//////

  
  // TODO:

  double normalS = 1.0;
  double normalA = 0.0;
  if( m_HeaderFileName != NULL ) {
    std::cout << "*** NOTE: reading "<< m_HeaderFileName << " for normalS and normalA" <<std::endl;
    std::string headerFileName( m_HeaderFileName );
    normalS = ReadField( headerFileName, NORMAL_S );
    std::cout << NORMAL_S << normalS << std::endl;
  
    normalA = ReadField( headerFileName, NORMAL_A );
    std::cout << NORMAL_A << normalA << std::endl;
    
//    std::cerr << "directions: " << dtiReader->GetOutput()->GetDirection() << std::endl;
//    std::cerr << "origin: " << dtiReader->GetOutput()->GetOrigin() << std::endl;
//        
//    mat44 rval;
//    nifti_image *img = nifti_image_read( m_InputStem, false);
//
//    rval = img->qto_xyz; // or img->sto_xyz -- you need to check the transform type
//                                     // to ensure getting the 'one true transform' for this file
//                                     
////    rval = img->sto_xyz;
//                                        
//    nifti_image_free(img);
//    
//    for( int i=0; i<4; i++ ) {
//      for( int j=0; j<4; j++ ) {        
//        std::cerr << rval.m[ i ][ j ] << std::endl;
//      }
//    }
  } 
  const double sliceUpData[] = { normalS, normalA, 0 };
  PoistatsFilterType::ArrayType sliceUp( sliceUpData, 3 );
  poistatsFilter->SetSliceUp( sliceUp );

  std::cout << "reading seed volume..." << std::endl;
  // read seed volume
  typedef itk::ImageFileReader< PoistatsFilterType::SeedVolumeType > SeedReaderType;
  SeedReaderType::Pointer seedReader = SeedReaderType::New();
  seedReader->SetFileName( m_Seeds );
  try { 
    seedReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error reading the series." << std::endl;
    std::cerr << excp << std::endl;
  }
  poistatsFilter->SetSeedVolume( seedReader->GetOutput() );

  // set seed for random number generator
  std::srand( ( unsigned ) time( 0 ) ); 
  poistatsFilter->SetRandomSeed( std::rand() );

  // set polarity
  PoistatsFilterType::MatrixType polarity( 3, 3 );
  polarity[ 0 ][ 0 ] = 1;
  polarity[ 0 ][ 1 ] = 1;
  polarity[ 0 ][ 2 ] = -1;

  polarity[ 1 ][ 0 ] = 1;
  polarity[ 1 ][ 1 ] = 1;
  polarity[ 1 ][ 2 ] = -1;

  polarity[ 2 ][ 0 ] = -1;
  polarity[ 2 ][ 1 ] = -1;
  polarity[ 2 ][ 2 ] = 1;
  poistatsFilter->SetPolarity( polarity );

  poistatsFilter->SetNumberOfControlPoints( m_nControlPoints );
  poistatsFilter->SetInitialSigma( m_Sigma );

  std::cout << "reading sampling volume..." << std::endl;

  // read sampling volume
  typedef itk::ImageFileReader< PoistatsFilterType::SamplingVolumeType > SamplingReaderType;
  SamplingReaderType::Pointer samplingReader = SamplingReaderType::New();
  samplingReader->SetFileName( m_SampleStem );
  try { 
    samplingReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error reading the series." << std::endl;
    std::cerr << excp << std::endl;
  }
  poistatsFilter->SetSamplingVolume( samplingReader->GetOutput() );

  // read mask volume if it exists
  if( m_MaskStem != NULL ) {  
    typedef itk::ImageFileReader< PoistatsFilterType::MaskVolumeType > MaskReaderType;
    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName( m_MaskStem );
    try { 
      maskReader->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::cerr << "Error reading the series." << std::endl;
      std::cerr << excp << std::endl;
    }
    poistatsFilter->SetMaskVolume( maskReader->GetOutput() );
  }

  // seeds the seeds to be used if they exist  
  for( std::vector< int >::iterator seedIt = m_SeedValues.begin();
    seedIt != m_SeedValues.end(); seedIt++ ) {
  
    int seedValueToUse = ( *seedIt );
    poistatsFilter->SetNextSeedValueToUse( seedValueToUse );      
  }
  
  if( m_ReplicaExchangeProbability > 0 ) {
    poistatsFilter->SetReplicaExchangeProbability( 
      m_ReplicaExchangeProbability );
  }

  if( m_SigmaTimeConstant > 0 ) {
    poistatsFilter->SetSigmaTimeConstant( m_SigmaTimeConstant );
  }
  
  if( m_PointsToImageGamma > 0 ) {
    poistatsFilter->SetPointsToImageGamma( m_PointsToImageGamma );
  }

  if( m_nReplicas > 0 ) {
    poistatsFilter->SetNumberOfReplicas( m_nReplicas );
  }
  
  // TODO: normally, we wouldn't set the max time so low, but I'm just testing...
//  poistatsFilter->SetMaxTime( 5 );
  // TODO: remove this
//  poistatsFilter->SetNumberOfReplicas( 5 );
  // TODO: remove this
//  poistatsFilter->SetMaxLull( 5 );
  
  // compute the poi
  poistatsFilter->Update();
  
  typedef itk::ImageFileWriter< PoistatsFilterType::OutputImageType > WriterType;  
  WriterType::Pointer writer = WriterType::New();

  // write aggregate densities
  std::string densityFileName = (std::string)m_OutputDir + 
    (std::string)"/PathDensity.nii";
  OutputImageType::Pointer pathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::PATH_DENSITY_OUTPUT );
  writer->SetInput( pathDensity );
  writer->SetFileName( densityFileName.c_str() );
  
  std::cout << "writing: " << densityFileName << std::endl;
  writer->Update();  

  std::string optimalDensityFileName = (std::string)m_OutputDir + 
    (std::string)"/OptimalPathDensity.nii";
  OutputImageType::Pointer optimalPathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::OPTIMAL_PATH_DENSITY_OUTPUT );
  writer->SetInput( optimalPathDensity );
  writer->SetFileName( optimalDensityFileName.c_str() );

  std::cout << "writing: " << optimalDensityFileName << std::endl;
  writer->Update();    
  
  PoistatsFilterType::MatrixType finalPath = poistatsFilter->GetFinalPath();
  const std::string finalPathFileName( (std::string)m_OutputDir + 
    (std::string)"/OptimalPath.txt" );
  this->WriteData( finalPathFileName, finalPath.data_array(), 
    finalPath.rows(), finalPath.cols() );

  PoistatsFilterType::ArrayType fa = poistatsFilter->GetSamples();  

  const std::string faFileName( (std::string)m_OutputDir + 
    (std::string)"/OptimalPathSamples.txt" );
  this->WriteData( faFileName, fa.data_block(), fa.size() );
  
  PoistatsFilterType::ArrayType finalPathProbabilities = 
    poistatsFilter->GetFinalPathProbabilities();
  const std::string pathProbabilitiesFileName( (std::string)m_OutputDir + 
    (std::string)"/OptimalPathProbabilities.txt" );
  this->WriteData( pathProbabilitiesFileName, 
    finalPathProbabilities.data_block(), finalPathProbabilities.size() );
    
  PoistatsFilterType::MatrixType replicasProbabilities = 
    poistatsFilter->GetBestPathsProbabilities();
  const std::string bestPathsProbabilitiesFileName( (std::string)m_OutputDir + 
    (std::string)"/ReplicasProbabilities.txt" );
  this->WriteData( bestPathsProbabilitiesFileName, 
    replicasProbabilities.data_array(), replicasProbabilities.rows(), 
    replicasProbabilities.cols() );

  typedef itk::ImageFileWriter< PoistatsFilterType::SegmentationVolumeType > IntWriterType;  
  IntWriterType::Pointer intWriter = IntWriterType::New();

  std::string optimalSegmentationFileName = (std::string)m_OutputDir + 
    (std::string)"/OptimalPathSegmentation.nii";
  PoistatsFilterType::SegmentationVolumePointer optimalPathSegmenation = 
    poistatsFilter->GetOptimalSegmentation();
  intWriter->SetInput( optimalPathSegmenation );
  intWriter->SetFileName( optimalSegmentationFileName.c_str() );

  std::cout << "writing: " << optimalSegmentationFileName << std::endl;
  intWriter->Update();    
  
}

int main( int argc, char ** argv ) {
  
  FreeDiffusionExecutable *exe = new Poistats( argc, argv );

// TODO: ./poistats -i test/dtensor.nii -o test/MyPath -seeds test/FinalSeeds.nii -sample test/fa.nii -nc 2 -sigma 10
  
  if( argc == 1 ) {
    exe->PrintHelp();
  } else {
    bool isFilled = exe->FillArguments();
    if( isFilled ) {
      exe->Run();
    }
  }
  
  return 0;
}
