#include <iostream>
#include <string>

#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>

#include <itkBSplineInterpolateImageFunction.h>

#include <itkNiftiImageIO.h>
#include "itkPoistatsFilter.h"

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

int main (int argc, char * argv[]) {

  PARSE_ARGS;

  typedef itk::Image< float, 4 > DtImageType;
  typedef itk::Image< float, 3 > OutputImageType;
  typedef itk::PoistatsFilter< DtImageType, OutputImageType > 
    PoistatsFilterType;
  PoistatsFilterType::Pointer poistatsFilter = PoistatsFilterType::New();

  // load the tensor volume
  typedef itk::ImageFileReader< PoistatsFilterType::InputImageType > DtiReaderType;  
  DtiReaderType::Pointer dtiReader = DtiReaderType::New();
  dtiReader->SetFileName( diffusionTensorImage );
  
  std::cerr << "reading dti..." << std::endl;
  try { 
    dtiReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error reading the series." << std::endl;
    std::cerr << excp << std::endl;
  }

// TODO: I'm testing in reading nrrd files generated from slicer:
//  ./PoistatsCLI -i ~/odin/data/reliability/subj001.1/dtensor/dtensor.nii
//  ./PoistatsCLI -i ~/heraclitus/data/dwi-dicom/namic01-dwi.nhdr
//
//  const int vectorSize = 6;
//  typedef itk::Vector< float, vectorSize > TensorPixelType;
//  typedef itk::Image< TensorPixelType, 3 > TensorImageType;
//  typedef itk::ImageFileReader< TensorImageType > TensorReaderType;
//  TensorReaderType::Pointer tensorReader = TensorReaderType::New();
//  tensorReader->SetFileName( diffusionTensorImage );
//  
//  std::cerr << "reading tensors..." << std::endl;
//  try { 
//    tensorReader->Update();
//  } catch( itk::ExceptionObject & excp ) {
//    std::cerr << "Error reading the series." << std::endl;
//    std::cerr << excp << std::endl;
//  }
//    
//  TensorImageType::Pointer tensors = tensorReader->GetOutput();
//  std::cerr << "tensors: " << tensors << std::endl;
//  
//  TensorImageType::SizeType imageSize = tensors->GetLargestPossibleRegion().GetSize();
//
//  itk::ImageRegionIterator< TensorImageType > it(
//    tensors, tensors->GetLargestPossibleRegion() );
//
//  // get the unique values
//  for ( it = it.Begin(); !it.IsAtEnd(); ++it ) {    
//    TensorPixelType pixelValue = it.Value();
//    for( int cElement=0; cElement<vectorSize; cElement++ ) {
//      if( pixelValue[ cElement ] != 0 ) {
//        std::cerr << pixelValue << std::endl;
//      }
//    }
//  }
  
  PoistatsFilterType::InputImageType::Pointer dti = dtiReader->GetOutput();
  std::cerr << "direction: " << dti->GetDirection();
  
  poistatsFilter->SetInput( dtiReader->GetOutput() );

  double normalS = 1.0;
  double normalA = 0.0;
  std::cout << "*** NOTE: reading "<< headerFile << " for normalS and normalA" <<std::endl;
  std::string headerFileName( headerFile );
  normalS = ReadField( headerFileName, NORMAL_S );
  std::cout << NORMAL_S << normalS << std::endl;

  normalA = ReadField( headerFileName, NORMAL_A );
  std::cout << NORMAL_A << normalA << std::endl;    
  
  const double sliceUpData[] = { normalS, normalA, 0 };
  PoistatsFilterType::ArrayType sliceUp( sliceUpData, 3 );
  poistatsFilter->SetSliceUp( sliceUp );

  std::cout << "reading seed volume..." << std::endl;
  // read seed volume
  typedef itk::ImageFileReader< PoistatsFilterType::SeedVolumeType > SeedReaderType;
  SeedReaderType::Pointer seedReader = SeedReaderType::New();
  seedReader->SetFileName( seedRegions );
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

  poistatsFilter->SetNumberOfControlPoints( numberOfControlPoints );
  poistatsFilter->SetInitialSigma( sigma );

  std::cout << "reading sampling volume..." << std::endl;

  // read sampling volume
  typedef itk::ImageFileReader< PoistatsFilterType::SamplingVolumeType > SamplingReaderType;
  SamplingReaderType::Pointer samplingReader = SamplingReaderType::New();
  samplingReader->SetFileName( sample );
  try { 
    samplingReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error reading the series." << std::endl;
    std::cerr << excp << std::endl;
  }
  poistatsFilter->SetSamplingVolume( samplingReader->GetOutput() );
  
  // set number of sample points
  poistatsFilter->SetNumberOfSamplePoints( numberOfSamplePoints );

  // read mask volume if it exists
  if( mask.size() != 0 ) {
    std::cout << "reading mask..." << std::endl;
    typedef itk::ImageFileReader< PoistatsFilterType::MaskVolumeType > MaskReaderType;
    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName( mask );
    try { 
      maskReader->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::cerr << "Error reading the series." << std::endl;
      std::cerr << excp << std::endl;
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
  poistatsFilter->Update();
  
  typedef itk::ImageFileWriter< PoistatsFilterType::OutputImageType > WriterType;  
  WriterType::Pointer writer = WriterType::New();

  // write aggregate densities
  std::string densityFileName = (std::string)outputDirectory + 
    (std::string)"/PathDensity.nii";
  OutputImageType::Pointer pathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::PATH_DENSITY_OUTPUT );
  writer->SetInput( pathDensity );
  writer->SetFileName( densityFileName.c_str() );
  
  std::cout << "writing: " << densityFileName << std::endl;
  writer->Update();  

  std::string optimalDensityFileName = (std::string)outputDirectory + 
    (std::string)"/OptimalPathDensity.nii";
  OutputImageType::Pointer optimalPathDensity = 
    poistatsFilter->GetOutput( PoistatsFilterType::OPTIMAL_PATH_DENSITY_OUTPUT );
  writer->SetInput( optimalPathDensity );
  writer->SetFileName( optimalDensityFileName.c_str() );

  std::cout << "writing: " << optimalDensityFileName << std::endl;
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
    
  PoistatsFilterType::MatrixType replicasProbabilities = 
    poistatsFilter->GetBestPathsProbabilities();
  const std::string bestPathsProbabilitiesFileName( (std::string)outputDirectory + 
    (std::string)"/ReplicasProbabilities.txt" );
  WriteData( bestPathsProbabilitiesFileName, 
    replicasProbabilities.data_array(), replicasProbabilities.rows(), 
    replicasProbabilities.cols() );

  typedef itk::ImageFileWriter< PoistatsFilterType::SegmentationVolumeType > IntWriterType;  
  IntWriterType::Pointer intWriter = IntWriterType::New();

  std::string optimalSegmentationFileName = (std::string)outputDirectory + 
    (std::string)"/OptimalPathSegmentation.nii";
  PoistatsFilterType::SegmentationVolumePointer optimalPathSegmenation = 
    poistatsFilter->GetOptimalSegmentation();
  intWriter->SetInput( optimalPathSegmenation );
  intWriter->SetFileName( optimalSegmentationFileName.c_str() );

  std::cout << "writing: " << optimalSegmentationFileName << std::endl;
  intWriter->Update();    

  return EXIT_SUCCESS;

}
