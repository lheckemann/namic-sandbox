#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/utsname.h>

#include <itkDiffusionTensor3D.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkNiftiImageIO.h>

#include "itkPoistatsFilter.h"
#include "CommandUpdate.h"
#include "PoistatsCLICLP.h"

const std::string NORMAL_A = "normal_a:";
const std::string NORMAL_S = "normal_s:";

void 
WriteData( const std::string fileName, 
  const double *data, const int nData) {

  std::cerr << "writing: " << fileName << std::endl;
  
  std::ofstream output( fileName.c_str() );
  
  for( int cData=0; cData<nData; cData++ ) {
    output << data[ cData ] << std::endl;
  }
  
  output.close();
    
}

void 
WriteData( const std::string fileName, 
  double **dataArray, const int nRows, const int nCols ) {

  std::cerr << "writing: " << fileName << std::endl;
  
  std::ofstream output( fileName.c_str() );
  
  for( int cRow=0; cRow<nRows; cRow++ ) {
    for( int cCol=0; cCol<nCols; cCol++ ) {
      output << dataArray[ cRow ][ cCol ] << "   ";
    }    
    output << std::endl;
  }
  
  output.close();
    
}

std::string 
GetFieldAndParameter( std::string field, std::string parameter ) {
  std::ostringstream output;
  output << field << ": \n" << "  " << parameter << std::endl << std::endl;
  return output.str();
}

std::string 
GetVersion() {
  return "1.0 Beta";
}

std::string 
GetCurrentDirectory() {
  
  const int nChars = 2000;
  char cwd[ nChars ];
  
  getcwd( cwd, nChars );
  
  return std::string( cwd );
}

std::string 
GetCommandArguments( int argc, char * argv[] ) {  

  std::ostringstream output;

  for( int cArg=0; cArg<argc; cArg++ ) {
    output << argv[ cArg ] << " ";
  }
  output << std::endl;
  
  return output.str();
}

std::string 
GetOperatingSystem() {  
  utsname uts;
  uname( &uts );    
  std::string name( uts.sysname );
  return name;
}

std::string
GetHostName() {
  utsname uts;
  uname( &uts );
  std::string name( uts.nodename );
  return name;
}

std::string
GetMachine() {
  utsname uts;
  uname( &uts );
  std::string machine( uts.machine );
  return machine;
}

std::string 
GetFreeSurferHome() {  
  std::string fsHome( std::getenv( "FREESURFER_HOME" ) ); 
  return fsHome;
}

std::string 
GetSubjectsDirectory() {
  std::string subjectsDirectory( std::getenv( "SUBJECTS_DIR" ) ); 
  return subjectsDirectory;
}

double ReadField( std::string fileName, std::string fieldName ) {
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

std::string
GetFileExtension( std::string fileName ) {

  // take in part from itk  
  std::string::size_type ext = fileName.rfind( '.' );
  std::string exts = fileName.substr( ext );
  if(exts == ".gz") {
    std::string::size_type dotpos = fileName.rfind('.',ext-1);
    if(dotpos != std::string::npos) {
      exts = fileName.substr(dotpos);
    }
  }  
  
  return exts;
}

int main (int argc, char * argv[]) {

  PARSE_ARGS;
  
  CommandUpdate::Pointer observer = CommandUpdate::New();

  observer->SetOutputDirectory( outputDirectory );  
  observer->SetLogFileName( "poistats.log" );

  observer->PostMessage( "-- Poistats " + GetVersion() + " --\n\n" );
  
  observer->PostMessage( GetFieldAndParameter( "Command", 
    GetCommandArguments( argc, argv ) ) );

  observer->PostMessage( GetFieldAndParameter( "FreeSurfer Home", 
    GetFreeSurferHome() ) );

  observer->PostMessage( GetFieldAndParameter( "Subjects Directory", 
    GetSubjectsDirectory() ) );

  observer->PostMessage( GetFieldAndParameter( "Current Working Directory", 
    GetCurrentDirectory() ) );
  
  observer->PostMessage( GetFieldAndParameter( "Operating System", 
    GetOperatingSystem() ) );

  observer->PostMessage( GetFieldAndParameter( "Host name", GetHostName() ) );

  observer->PostMessage( GetFieldAndParameter( "Machine", GetMachine() ) );
  
  typedef itk::DiffusionTensor3D< float > TensorPixelType;
  typedef itk::Image< TensorPixelType, 3 > TensorImageType;

  TensorImageType::Pointer tensors;
  
  // if the data is stored as 9 components  
  if( !isSymmetricTensorData ){
    
    observer->PostMessage( "not stored symmetrically\n" );
    
    typedef itk::Image< float, 4 > FullTensorImageType;
    typedef itk::ImageFileReader< FullTensorImageType > FullTensorReaderType;
    FullTensorReaderType::Pointer tensorReader = FullTensorReaderType::New();
    tensorReader->SetFileName( diffusionTensorImage );
    observer->PostMessage( "reading tensors...\n" );
    try { 
      tensorReader->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::ostringstream output;
      output << "Error reading the series." << std::endl << excp << std::endl;
      observer->PostErrorMessage( output.str() );
      return EXIT_FAILURE;
    }
    
    FullTensorImageType::Pointer fullTensors = tensorReader->GetOutput();

    
    // TODO:
    std::cerr << fullTensors << std::endl;
    std::cerr << "\ndirection: " << fullTensors->GetDirection();

    
    // convert the full 9 component tensors to 6 component tensors
    //  - create and allocate a new DiffusionTensor3D image
    FullTensorImageType::RegionType dtiRegion = 
      fullTensors->GetLargestPossibleRegion();
    TensorImageType::SizeType size;
    double origin[ TensorImageType::RegionType::GetImageDimension() ];
    TensorImageType::IndexType start;
    TensorImageType::SpacingType spacing;
  
    for( int cDim=0; cDim<TensorImageType::RegionType::GetImageDimension(); cDim++ ) {    
      size[ cDim ] = dtiRegion.GetSize()[ cDim ];
      origin[ cDim ] = fullTensors->GetOrigin()[ cDim ];
      start[ cDim ] = 0;
      spacing[ cDim ] = fullTensors->GetSpacing()[ cDim ];
    }
        
    TensorImageType::RegionType region;
    region.SetSize( size );    
    region.SetIndex( start );
    
    tensors = TensorImageType::New();
    tensors->SetRegions( region );
    tensors->SetOrigin( origin );
    tensors->SetSpacing( spacing );      
    tensors->Allocate();  
    tensors->FillBuffer( 0.0 );

    //  - copy the data into the right areas
    const int nTensorRows = 3;
    const int nTensorCols = 3;

    observer->PostMessage( "converting tensors to symmetric format\n" );
    for( int cImageRow=0; cImageRow<size[ 0 ]; cImageRow++ ) {
      for( int cImageCol=0; cImageCol<size[ 1 ]; cImageCol++ ) {
        for( int cImageSlice=0; cImageSlice<size[ 2 ]; cImageSlice++ ) {
        
          TensorImageType::IndexType symmetricIndex;
          symmetricIndex[ 0 ] = cImageRow;
          symmetricIndex[ 1 ] = cImageCol;
          symmetricIndex[ 2 ] = cImageSlice;

          TensorPixelType symmetricTensor;

          FullTensorImageType::IndexType fullTensorIndex;
          fullTensorIndex[ 0 ] = cImageRow;
          fullTensorIndex[ 1 ] = cImageCol;
          fullTensorIndex[ 2 ] = cImageSlice;
        
          int cContinuousIndex = 0;
          
          // iterate through each component
          // TODO: actually we'll only need to iterate through some of them
          for( int cTensorRow = 0; cTensorRow<nTensorRows; cTensorRow++ ) {
            
            for( int cTensorCol = 0; cTensorCol<nTensorCols; cTensorCol++ ) {
              
              fullTensorIndex[ 3 ] = cContinuousIndex;
              
              symmetricTensor( cTensorRow, cTensorCol ) = 
                fullTensors->GetPixel( fullTensorIndex );

//              if( fullTensors->GetPixel( fullTensorIndex ) != 0.0 )
//                std::cerr << fullTensors->GetPixel( fullTensorIndex ) << "  ";
              
              cContinuousIndex++;
            }
            
          }
          
          tensors->SetPixel( symmetricIndex, symmetricTensor );

        }
      }
    }
    
    
  } else {
    // data stored in 6 components
    typedef itk::ImageFileReader< TensorImageType > TensorReaderType;
    TensorReaderType::Pointer tensorReader = TensorReaderType::New();
    tensorReader->SetFileName( diffusionTensorImage );
    observer->PostMessage( "reading symmetric tensors...\n" );
    try { 
      tensorReader->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::ostringstream output;
      output << "Error reading the series." << std::endl << excp << std::endl;
      observer->PostErrorMessage( output.str() );
      return EXIT_FAILURE;
    }
    tensors = tensorReader->GetOutput();
  }
  
  typedef itk::Image< float, 3 > OutputImageType;
  typedef itk::PoistatsFilter< TensorImageType, OutputImageType > 
    PoistatsFilterType;
  PoistatsFilterType::Pointer poistatsFilter = PoistatsFilterType::New();
  
  poistatsFilter->AddObserver( itk::AnyEvent(), observer );
  
  poistatsFilter->SetInput( tensors );
    
  double normalS = 1.0;
  double normalA = 0.0;
  observer->PostMessage( "*** NOTE: reading " + headerFile + " for normalS and normalA\n" );
  std::string headerFileName( headerFile );
  normalS = ReadField( headerFileName, NORMAL_S );
  std::cout << NORMAL_S << normalS << std::endl;

  normalA = ReadField( headerFileName, NORMAL_A );
  std::cout << NORMAL_A << normalA << std::endl;    
  
  const double sliceUpData[] = { normalS, normalA, 0 };
  PoistatsFilterType::ArrayType sliceUp( sliceUpData, 3 );
  poistatsFilter->SetSliceUp( sliceUp );

  observer->PostMessage( "reading seed volume...\n" );
  // read seed volume
  typedef itk::ImageFileReader< PoistatsFilterType::SeedVolumeType > SeedReaderType;
  SeedReaderType::Pointer seedReader = SeedReaderType::New();
  seedReader->SetFileName( seedRegions );
  try { 
    seedReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::ostringstream output;
    output << "Error reading the series." << std::endl << excp << std::endl;
    observer->PostErrorMessage( output.str() );
    return EXIT_FAILURE;
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

  poistatsFilter->SetNumberOfControlPoints( numberOfControlPoints );
  poistatsFilter->SetInitialSigma( sigma );

  observer->PostMessage( "reading sampling volume...\n" );

  // read sampling volume
  typedef itk::ImageFileReader< PoistatsFilterType::SamplingVolumeType > SamplingReaderType;
  SamplingReaderType::Pointer samplingReader = SamplingReaderType::New();
  samplingReader->SetFileName( sample );
  try { 
    samplingReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::ostringstream output;
    output << "Error reading the series." << std::endl << excp << std::endl;
    observer->PostErrorMessage( output.str() );
    return EXIT_FAILURE;
  }
  poistatsFilter->SetSamplingVolume( samplingReader->GetOutput() );
  
  // set number of sample points
  poistatsFilter->SetNumberOfSamplePoints( numberOfSamplePoints );

  // read mask volume if it exists
  if( mask.size() != 0 ) {
    observer->PostMessage( "reading mask...\n" );
    typedef itk::ImageFileReader< PoistatsFilterType::MaskVolumeType > MaskReaderType;
    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName( mask );
    try { 
      maskReader->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::ostringstream output;
      output << "Error reading the series." << std::endl << excp << std::endl;
      observer->PostErrorMessage( output.str() );
      return EXIT_FAILURE;
    }
    poistatsFilter->SetMaskVolume( maskReader->GetOutput() );
  }

  // seeds the seeds to be used if they exist  
  for( std::vector< int >::iterator seedIt = seedRegionsToUse.begin();
    seedIt != seedRegionsToUse.end(); seedIt++ ) {
  
    int seedValueToUse = ( *seedIt );
    poistatsFilter->SetNextSeedValueToUse( seedValueToUse );      
  }
  
  if( exchangeProbability > 0 ) {
    poistatsFilter->SetReplicaExchangeProbability( exchangeProbability );
  }

  if( timeConstant > 0 ) {
    poistatsFilter->SetSigmaTimeConstant( timeConstant );
  }
  
  if( gamma > 0 ) {
    poistatsFilter->SetPointsToImageGamma( gamma );
  }

  if( numberOfReplicas > 0 ) {
    poistatsFilter->SetNumberOfReplicas( numberOfReplicas );
  }

  // compute the poi
  try { 
    poistatsFilter->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::ostringstream output;
    output << "Error thrown in poistats filter." << std::endl << excp << std::endl;
    observer->PostErrorMessage( output.str() );
    return EXIT_FAILURE;
  }
  
  typedef itk::ImageFileWriter< PoistatsFilterType::OutputImageType > WriterType;  
  WriterType::Pointer writer = WriterType::New();

  // save the image volumes out as the same file type as input  
  std::string imageFileExtension = GetFileExtension( diffusionTensorImage );

  // write aggregate densities
  std::string densityFileName = (std::string)outputDirectory + 
    (std::string)"/PathDensity" + imageFileExtension;
  OutputImageType::Pointer pathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::PATH_DENSITY_OUTPUT );
  writer->SetInput( pathDensity );
  writer->SetFileName( densityFileName.c_str() );
  
  observer->PostMessage( "writing: " + densityFileName + "\n" );  
  writer->Update();  

  std::string optimalDensityFileName = (std::string)outputDirectory + 
    (std::string)"/OptimalPathDensity" + imageFileExtension;
  OutputImageType::Pointer optimalPathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::OPTIMAL_PATH_DENSITY_OUTPUT );
  writer->SetInput( optimalPathDensity );
  writer->SetFileName( optimalDensityFileName.c_str() );

  observer->PostMessage( "writing: " + optimalDensityFileName );  
  writer->Update();
  
  PoistatsFilterType::MatrixType finalPath = poistatsFilter->GetFinalPath();
  const std::string finalPathFileName( (std::string)outputDirectory + 
    (std::string)"/OptimalPath.txt" );
  WriteData( finalPathFileName, finalPath.data_array(), 
    finalPath.rows(), finalPath.cols() );

  PoistatsFilterType::ArrayType fa = poistatsFilter->GetSamples();  

  const std::string faFileName( (std::string)outputDirectory + 
    (std::string)"/OptimalPathSamples.txt" );
  WriteData( faFileName, fa.data_block(), fa.size() );
  
  PoistatsFilterType::ArrayType finalPathProbabilities = 
    poistatsFilter->GetFinalPathProbabilities();
  const std::string pathProbabilitiesFileName( (std::string)outputDirectory + 
    (std::string)"/OptimalPathProbabilities.txt" );
  WriteData( pathProbabilitiesFileName, 
    finalPathProbabilities.data_block(), finalPathProbabilities.size() );
    
//  PoistatsFilterType::MatrixType replicasProbabilities = 
//    poistatsFilter->GetBestPathsProbabilities();
//  const std::string bestPathsProbabilitiesFileName( (std::string)outputDirectory + 
//    (std::string)"/ReplicasProbabilities.txt" );
//  WriteData( bestPathsProbabilitiesFileName, 
//    replicasProbabilities.data_array(), replicasProbabilities.rows(), 
//    replicasProbabilities.cols() );
//
//  typedef itk::ImageFileWriter< PoistatsFilterType::SegmentationVolumeType > IntWriterType;  
//  IntWriterType::Pointer intWriter = IntWriterType::New();
//
//  std::string optimalSegmentationFileName = (std::string)outputDirectory + 
//    (std::string)"/OptimalPathSegmentation.nii";
//  PoistatsFilterType::SegmentationVolumePointer optimalPathSegmenation = 
//    poistatsFilter->GetOptimalSegmentation();
//  intWriter->SetInput( optimalPathSegmenation );
//  intWriter->SetFileName( optimalSegmentationFileName.c_str() );
//
//  observer->PostMessage( "writing: " + optimalSegmentationFileName + "\n" );  
//  intWriter->Update();    

  return EXIT_SUCCESS;

}
