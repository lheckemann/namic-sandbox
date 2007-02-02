#ifndef _itkPoistatsFilter_txx
#define _itkPoistatsFilter_txx

#include <itkBSplineInterpolateImageFunction.h>

#include <itkImageRegionIterator.h>

#include <itkNonUniformBSpline.h>

#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_cross.h>

#include <time.h>

#include "itkPoistatsFilter.h"

#include "itkPointSet.h"
#include "itkBSplineScatteredDataPointSetToImageFilter.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
const double
PoistatsFilter<TInputImage, TOutputImage>
::NO_ZERO_SHELL_252[][3] = {
    -0.99419,0,-0.107642,
    -0.985146,-0.130799,0.111264,
    -0.985146,0.130799,0.111264,
    -0.963803,-0.245346,-0.104352,
    -0.963803,0.245346,-0.104352,
    -0.948206,-0.115895,-0.295759,
    -0.948206,0.115895,-0.295759,
    -0.942646,0,0.333793,
    -0.923944,-0.36802,0.104352,
    -0.923944,0.36802,0.104352,
    -0.911958,-0.253082,0.322926,
    -0.911958,0.253082,0.322926,
    -0.894427,0,-0.447214,
    -0.886548,-0.331288,-0.322926,
    -0.886548,0.331288,-0.322926,
    -0.873881,-0.473235,-0.111264,
    -0.873881,0.473235,-0.111264,
    -0.84962,-0.187522,-0.492931,
    -0.84962,0.187522,-0.492931,
    -0.844226,-0.122673,0.52176,
    -0.844226,0.122673,0.52176,
    -0.835236,-0.463581,0.295759,
    -0.835236,0.463581,0.295759,
    -0.804316,-0.58437,0.107642,
    -0.804316,0.58437,0.107642,
    -0.79758,-0.347685,0.492931,
    -0.79758,0.347685,0.492931,
    -0.78869,0,-0.614791,
    -0.762617,-0.554074,-0.333793,
    -0.762617,0.554074,-0.333793,
    -0.755099,-0.396979,-0.52176,
    -0.755099,0.396979,-0.52176,
    -0.723607,-0.525731,0.447214,
    -0.723607,0.525731,0.447214,
    -0.720118,-0.684873,-0.111264,
    -0.720118,0,0.693852,
    -0.720118,0.684873,-0.111264,
    -0.712379,-0.204747,-0.671263,
    -0.712379,0.204747,-0.671263,
    -0.698993,-0.651103,0.295759,
    -0.698993,0.651103,0.295759,
    -0.696674,-0.253082,0.671263,
    -0.696674,0.253082,0.671263,
    -0.638064,-0.463581,0.614791,
    -0.638064,0.463581,0.614791,
    -0.635522,-0.764998,0.104352,
    -0.635522,0.764998,0.104352,
    -0.626112,0,-0.779733,
    -0.610888,-0.595468,-0.52176,
    -0.610888,0.595468,-0.52176,
    -0.589032,-0.740783,-0.322926,
    -0.589032,0.740783,-0.322926,
    -0.582588,-0.423275,-0.693852,
    -0.582588,0.423275,-0.693852,
    -0.577134,-0.651103,0.492931,
    -0.577134,0.651103,0.492931,
    -0.540088,-0.130799,0.831382,
    -0.540088,0.130799,0.831382,
    -0.53117,-0.840815,-0.104352,
    -0.53117,0.840815,-0.104352,
    -0.522506,-0.789117,0.322926,
    -0.522506,0.789117,0.322926,
    -0.513822,-0.211637,-0.831382,
    -0.513822,0.211637,-0.831382,
    -0.506536,-0.36802,0.779733,
    -0.506536,0.36802,0.779733,
    -0.455979,-0.58437,0.671263,
    -0.455979,0.58437,0.671263,
    -0.440891,-0.750089,-0.492931,
    -0.440891,0.750089,-0.492931,
    -0.428824,-0.89651,0.111264,
    -0.428824,0.89651,0.111264,
    -0.417408,0,-0.908719,
    -0.414864,-0.614242,-0.671263,
    -0.414864,0.614242,-0.671263,
    -0.403235,-0.865984,-0.295759,
    -0.403235,0.865984,-0.295759,
    -0.377549,-0.764998,0.52176,
    -0.377549,0.764998,0.52176,
    -0.360059,-0.423275,-0.831382,
    -0.360059,0.423275,-0.831382,
    -0.348337,0,0.937369,
    -0.33769,-0.245346,0.908719,
    -0.33769,0.245346,0.908719,
    -0.307222,-0.945531,-0.107642,
    -0.307222,0.945531,-0.107642,
    -0.291294,-0.89651,0.333793,
    -0.291294,-0.473235,0.831382,
    -0.291294,0.473235,0.831382,
    -0.291294,0.89651,0.333793,
    -0.281811,-0.204747,-0.937369,
    -0.281811,0.204747,-0.937369,
    -0.276393,-0.850651,-0.447214,
    -0.276393,0.850651,-0.447214,
    -0.243719,-0.750089,-0.614791,
    -0.243719,0.750089,-0.614791,
    -0.222529,-0.684873,0.693852,
    -0.222529,0.684873,0.693852,
    -0.197173,0,-0.980369,
    -0.193479,-0.595468,-0.779733,
    -0.193479,0.595468,-0.779733,
    -0.182789,-0.937611,-0.295759,
    -0.182789,0.937611,-0.295759,
    -0.180029,-0.977348,0.111264,
    -0.180029,0.977348,0.111264,
    -0.159516,-0.115895,0.980369,
    -0.159516,0.115895,0.980369,
    -0.144211,-0.840815,0.52176,
    -0.144211,0.840815,0.52176,
    -0.128986,-0.396979,-0.908719,
    -0.128986,0.396979,-0.908719,
    -0.107642,-0.331288,0.937369,
    -0.107642,0.331288,0.937369,
    -0.0842027,-0.865984,-0.492931,
    -0.0842027,0.865984,-0.492931,
    -0.0644931,-0.992447,-0.104352,
    -0.0644931,0.992447,-0.104352,
    -0.0609297,-0.187522,-0.980369,
    -0.0609297,0.187522,-0.980369,
    -0.0424992,-0.554074,0.831382,
    -0.0424992,0.554074,0.831382,
    -0.0411156,-0.945531,0.322926,
    -0.0411156,0.945531,0.322926,
    -0.0254109,-0.740783,-0.671263,
    -0.0254109,0.740783,-0.671263,
    0,0,-1.,
    0,0,1.,
    0.0254109,-0.740783,0.671263,
    0.0254109,0.740783,0.671263,
    0.0411156,-0.945531,-0.322926,
    0.0411156,0.945531,-0.322926,
    0.0424992,-0.554074,-0.831382,
    0.0424992,0.554074,-0.831382,
    0.0609297,-0.187522,0.980369,
    0.0609297,0.187522,0.980369,
    0.0644931,-0.992447,0.104352,
    0.0644931,0.992447,0.104352,
    0.0842027,-0.865984,0.492931,
    0.0842027,0.865984,0.492931,
    0.107642,-0.331288,-0.937369,
    0.107642,0.331288,-0.937369,
    0.128986,-0.396979,0.908719,
    0.128986,0.396979,0.908719,
    0.144211,-0.840815,-0.52176,
    0.144211,0.840815,-0.52176,
    0.159516,-0.115895,-0.980369,
    0.159516,0.115895,-0.980369,
    0.180029,-0.977348,-0.111264,
    0.180029,0.977348,-0.111264,
    0.182789,-0.937611,0.295759,
    0.182789,0.937611,0.295759,
    0.193479,-0.595468,0.779733,
    0.193479,0.595468,0.779733,
    0.197173,0,0.980369,
    0.222529,-0.684873,-0.693852,
    0.222529,0.684873,-0.693852,
    0.243719,-0.750089,0.614791,
    0.243719,0.750089,0.614791,
    0.276393,-0.850651,0.447214,
    0.276393,0.850651,0.447214,
    0.281811,-0.204747,0.937369,
    0.281811,0.204747,0.937369,
    0.291294,-0.89651,-0.333793,
    0.291294,-0.473235,-0.831382,
    0.291294,0.473235,-0.831382,
    0.291294,0.89651,-0.333793,
    0.307222,-0.945531,0.107642,
    0.307222,0.945531,0.107642,
    0.33769,-0.245346,-0.908719,
    0.33769,0.245346,-0.908719,
    0.348337,0,-0.937369,
    0.360059,-0.423275,0.831382,
    0.360059,0.423275,0.831382,
    0.377549,-0.764998,-0.52176,
    0.377549,0.764998,-0.52176,
    0.403235,-0.865984,0.295759,
    0.403235,0.865984,0.295759,
    0.414864,-0.614242,0.671263,
    0.414864,0.614242,0.671263,
    0.417408,0,0.908719,
    0.428824,-0.89651,-0.111264,
    0.428824,0.89651,-0.111264,
    0.440891,-0.750089,0.492931,
    0.440891,0.750089,0.492931,
    0.455979,-0.58437,-0.671263,
    0.455979,0.58437,-0.671263,
    0.506536,-0.36802,-0.779733,
    0.506536,0.36802,-0.779733,
    0.513822,-0.211637,0.831382,
    0.513822,0.211637,0.831382,
    0.522506,-0.789117,-0.322926,
    0.522506,0.789117,-0.322926,
    0.53117,-0.840815,0.104352,
    0.53117,0.840815,0.104352,
    0.540088,-0.130799,-0.831382,
    0.540088,0.130799,-0.831382,
    0.577134,-0.651103,-0.492931,
    0.577134,0.651103,-0.492931,
    0.582588,-0.423275,0.693852,
    0.582588,0.423275,0.693852,
    0.589032,-0.740783,0.322926,
    0.589032,0.740783,0.322926,
    0.610888,-0.595468,0.52176,
    0.610888,0.595468,0.52176,
    0.626112,0,0.779733,
    0.635522,-0.764998,-0.104352,
    0.635522,0.764998,-0.104352,
    0.638064,-0.463581,-0.614791,
    0.638064,0.463581,-0.614791,
    0.696674,-0.253082,-0.671263,
    0.696674,0.253082,-0.671263,
    0.698993,-0.651103,-0.295759,
    0.698993,0.651103,-0.295759,
    0.712379,-0.204747,0.671263,
    0.712379,0.204747,0.671263,
    0.720118,-0.684873,0.111264,
    0.720118,0,-0.693852,
    0.720118,0.684873,0.111264,
    0.723607,-0.525731,-0.447214,
    0.723607,0.525731,-0.447214,
    0.755099,-0.396979,0.52176,
    0.755099,0.396979,0.52176,
    0.762617,-0.554074,0.333793,
    0.762617,0.554074,0.333793,
    0.78869,0,0.614791,
    0.79758,-0.347685,-0.492931,
    0.79758,0.347685,-0.492931,
    0.804316,-0.58437,-0.107642,
    0.804316,0.58437,-0.107642,
    0.835236,-0.463581,-0.295759,
    0.835236,0.463581,-0.295759,
    0.844226,-0.122673,-0.52176,
    0.844226,0.122673,-0.52176,
    0.84962,-0.187522,0.492931,
    0.84962,0.187522,0.492931,
    0.873881,-0.473235,0.111264,
    0.873881,0.473235,0.111264,
    0.886548,-0.331288,0.322926,
    0.886548,0.331288,0.322926,
    0.894427,0,0.447214,
    0.911958,-0.253082,-0.322926,
    0.911958,0.253082,-0.322926,
    0.923944,-0.36802,-0.104352,
    0.923944,0.36802,-0.104352,
    0.942646,0,-0.333793,
    0.948206,-0.115895,0.295759,
    0.948206,0.115895,0.295759,
    0.963803,-0.245346,0.104352,
    0.963803,0.245346,0.104352,
    0.985146,-0.130799,-0.111264,
    0.985146,0.130799,-0.111264,
    0.99419,0,0.107642
  };
  

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
PoistatsFilter<TInputImage, TOutputImage>
::PoistatsFilter()
{

  std::srand( ( unsigned ) time( 0 ) );
  const long seed = std::rand();

  this->m_PoistatsModel = new PoistatsModel( seed );  

  this->SetNumberOfRequiredOutputs( 2 );
      
  this->SetNumberOfSamplePoints( DEFAULT_NUMBER_OF_SAMPLE_POINTS );
  
  this->SetMaxLull( DEFAULT_MAX_LULL );
  this->SetMaxTime( DEFAULT_MAX_TIME );
  
  this->SetCoolFactor( DEFAULT_COOL_FACTOR );
  
  this->SetNumberOfDirections( DEFAULT_NUMBER_OF_DIRECTIONS );

  this->m_OdfLookUpTable = NULL;

  this->m_Polarity = MatrixType( 3, 3 );
  this->m_Polarity.fill( 1 );
  
  m_Replicas = new PoistatsReplicas( this->m_PoistatsModel, 
    DEFAULT_NUMBER_OF_REPLICAS );
    
  m_Replicas->SetNumberOfSteps( DEFAULT_NUMBER_OF_STEPS );
  
  const double smallValue = 1e-200;
  m_InvalidOdf = ArrayType( GetNumberOfDirections() );
  m_InvalidOdf.fill( smallValue );
  
  this->SetReplicaExchangeProbability( DEFAULT_REPLICA_EXCHANGE_PROBABILITY );

  this->SetSigmaTimeConstant( DEFAULT_SIGMA_TIME_CONSTANT );

  this->SetPointsToImageGamma( DEFAULT_POINTS_TO_IMAGE_GAMMA );
  
}

template <class TInputImage, class TOutputImage>
PoistatsFilter<TInputImage, TOutputImage>
::~PoistatsFilter() {
  delete this->m_Replicas;
  delete m_PoistatsModel;  
}

template <class TInputImage, class TOutputImage>
int
PoistatsFilter<TInputImage, TOutputImage>
::GetNumberOfReplicas() {
  return this->m_Replicas->GetNumberOfReplicas();
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetNumberOfReplicas( const int nReplicas ) {
  this->m_Replicas->SetNumberOfReplicas( nReplicas );
}


template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::OdfLookUpTablePointer
PoistatsFilter<TInputImage, TOutputImage>
::GetOdfLookUpTable() {

  if( !this->m_OdfLookUpTable ) {
  
    this->m_OdfLookUpTable = OdfLookUpTableType::New();
          
    RegionType dtiRegion = this->GetInput()->GetLargestPossibleRegion();
    OdfLookUpRegionType odfRegion;
    
    double odfOrigin[ OdfLookUpRegionType::GetImageDimension() ];
    OdfLookUpIndexType odfStart;

    OdfLookUpSizeType odfSize;
    for( int cDim=0; cDim<OdfLookUpRegionType::GetImageDimension(); cDim++ ) {    
      odfSize[ cDim ] = dtiRegion.GetSize()[ cDim ];
      odfOrigin[ cDim ] = this->GetInput()->GetOrigin()[ cDim ];
      odfStart[ cDim ] = 0;
    }
        
    odfRegion.SetSize( odfSize );    
    odfRegion.SetIndex( odfStart );
                    
    this->m_OdfLookUpTable->SetRegions( odfRegion );
    this->m_OdfLookUpTable->SetOrigin( odfOrigin );
    
    this->m_OdfLookUpTable->Allocate();

    // TODO: ask kitware guys about this
    // for some reason, I had linking errors when I tried to do:
    //    this->m_OdfLookUpTable->FillBuffer( INVALID_INDEX )
    const int invalid = INVALID_INDEX;
    this->m_OdfLookUpTable->FillBuffer( invalid );

  }

  return this->m_OdfLookUpTable;
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::AllocateOutputImage( const int outputIndex, OutputImagePointer image ) {
  
  InputImageConstPointer inputImage = this->GetInput();
  
  RegionType dtiRegion = inputImage->GetLargestPossibleRegion();
  OutputSizeType outputSize;
  double outputOrigin[ OutputRegionType::GetImageDimension() ];
  OutputIndexType outputStart;
  OutputSpacingType outputSpacing;

  for( int cDim=0; cDim<OutputRegionType::GetImageDimension(); cDim++ ) {    
    outputSize[ cDim ] = dtiRegion.GetSize()[ cDim ];
    outputOrigin[ cDim ] = this->GetInput()->GetOrigin()[ cDim ];
    outputStart[ cDim ] = 0;
    outputSpacing[ cDim ] = this->GetInput()->GetSpacing()[ cDim ];
  }
      
  OutputRegionType outputRegion;
  outputRegion.SetSize( outputSize );    
  outputRegion.SetIndex( outputStart );
  
  image->SetRegions( outputRegion );
  image->SetOrigin( outputOrigin );
  image->SetSpacing( outputSpacing );  
  
  image->Allocate();

  image->FillBuffer( 0.0 );

  this->SetNthOutput( outputIndex, image );
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter< TInputImage, TOutputImage >
::GetPositiveMinimumInt( MatrixPointer points, const int radius, 
  itk::Array< int >* minimum ) {

  const int rowFloor = 0;
  
  for( int cColumn=0; cColumn<points->cols(); cColumn++ ) {

    vnl_vector< double > column = points->get_column( cColumn );
    int minimumOfRow = static_cast< int >( floor( column.min_value() ) );
    minimumOfRow -= radius;
    
    if( minimumOfRow < rowFloor ) {
      minimumOfRow = rowFloor;
    }
    
    ( *minimum )[ cColumn ] = minimumOfRow;
        
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter< TInputImage, TOutputImage >
::GetPositiveMaximumInt( MatrixPointer points, const int radius, 
  itk::Array< int >* maximum, OutputRegionType pointsRegion ) {

  for( int cColumn=0; cColumn<points->cols(); cColumn++ ) {
    
    vnl_vector< double > column = points->get_column( cColumn );
    int maximumOfRow = static_cast< int >( ceil( column.max_value() ) );
    maximumOfRow += radius;
    
    const int rowCeiling = pointsRegion.GetSize()[ cColumn ] - 1;
    if( maximumOfRow > rowCeiling ) {
      maximumOfRow = rowCeiling;
    }
    
    ( *maximum )[ cColumn ] = maximumOfRow;
    
  }

}



template <class TInputImage, class TOutputImage>
void
PoistatsFilter< TInputImage, TOutputImage >
::ConvertPointsToImage( MatrixPointer points, OutputImagePointer image ) {

/*

mn = floor(min(pnts)-3);
mn = max(1,mn);

mx = ceil(max(pnts)+3);
mx = min(volsize, mx);

[X Y Z] = meshgrid(mn(1):mx(1), mn(2):mx(2), mn(3):mx(3));
X = X(:); Y = Y(:); Z = Z(:);
box = [X Y Z];
wts = sum(exp(-gamma*distmat(box, pnts).^2),2);
wts = wts/sum(wts(:));
idx = int64(sub2ind(volsize, X, Y, Z));

vol = zeros(volsize);
vol(idx) = vol(idx) + wts;
*/

  const int radius = 3;

  itk::Array< int > minimumPoint( 3 );
  GetPositiveMinimumInt( points, radius, &minimumPoint );
  
  itk::Array< int > maximumPoint( 3 );
  GetPositiveMaximumInt( points, radius, &maximumPoint, 
    image->GetLargestPossibleRegion() );

  const int y = 0;
  const int x = 1;
  const int z = 2;
    
  const int columnStart = minimumPoint[ y ];
  const int columnEnd = maximumPoint[ y ];

  const int rowStart = minimumPoint[ x ];
  const int rowEnd = maximumPoint[ x ];
  
  const int sliceStart = minimumPoint[ z ];
  const int sliceEnd = maximumPoint[ z ];

  const double gamma = this->GetPointsToImageGamma();

  itk::Array< double > box( points->cols() );
  OutputIndexType pixelIndex;

  double totalWeights = 0.0;

  // these for loops are inclusive of the ending indices

  for( int cCol=columnStart; cCol<=columnEnd; cCol++ ) {
    for( int cRow=rowStart; cRow<=rowEnd; cRow++ ) {      
      for( int cSlice=sliceStart; cSlice<=sliceEnd; cSlice++ ) {

        box[ y ] = static_cast< double >( cCol );
        box[ x ] = static_cast< double >( cRow );
        box[ z ] = static_cast< double >( cSlice );

        ArrayType distanceMatrix( points->rows() );

        for( int cPoint=0; cPoint<points->rows(); cPoint++ ) {

          vnl_vector< double > point = points->get_row( cPoint );

          const double distance = GetDistance( &point, &box );
          distanceMatrix[ cPoint ] = exp( -gamma * distance * distance );

        }
        
        double sum = distanceMatrix.sum();

        pixelIndex[ 0 ] = cCol; 
        pixelIndex[ 1 ] = cRow; 
        pixelIndex[ 2 ] = cSlice;
        
        image->SetPixel( pixelIndex, sum );
        totalWeights += sum;      
      }
    }  
  }

  // divide each element by the totalWeights
  typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
  IteratorType it( image, image->GetLargestPossibleRegion() );
  
  for( it = it.Begin(); !it.IsAtEnd(); ++it ) {
    double pixelValue = it.Value();
    if( pixelValue != 0.0 ) {
      pixelValue /= totalWeights;
      it.Set( pixelValue );
    } 
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter< TInputImage, TOutputImage >
::GenerateData() {

  this->InvokeEvent( StartEvent() );
    
  itkDebugMacro( << "parsing seed volume" );  
  this->ParseSeedVolume();

  itkDebugMacro( << "initializing paths" );
  this->InitPaths();
  
  this->ConstructOdfList();

  // initialize temperatures to be regularily spaced between 0.05 and 0.1
  const double temperatureFloor = 0.05;
  const double temperatureCeiling = 0.1;
  m_Replicas->SpaceTemperaturesEvenly( temperatureFloor, temperatureCeiling );
  
  this->m_Replicas->FillCurrentMeanEnergies( 0.0 );

  const double maxDouble = std::numeric_limits< double >::max();
  
  this->m_Replicas->FillPreviousMeanEnergies( maxDouble );
  
  this->SetGlobalMinEnergy( maxDouble );

  const int nSpatialDimensions = 3;  
  MatrixType finalBestPath( m_Replicas->GetNumberOfSteps(), 
    nSpatialDimensions );
  
  const double lullEnergyDifferenceThreshold = .5*1e-3;
  
  const bool isMoreThanOneReplica = m_Replicas->GetNumberOfReplicas() > 1;
      
  for( m_CurrentIteration=1, m_CurrentLull=0;
       m_CurrentIteration < this->GetMaxTime() && m_CurrentLull < this->GetMaxLull();
       m_CurrentIteration++ ) {

    clock_t startClock = clock();

    const double sigma = static_cast< double >( this->GetInitialSigma() ) * 
      exp( -static_cast< double >( m_CurrentIteration ) / 
           this->GetSigmaTimeConstant() );
                   
    // reset the number of exchanges that occured...if you're keeping track
    this->SetExchanges( 0 );
    
    for( int cReplica=0; cReplica<this->GetNumberOfReplicas(); cReplica++ ) {
    
      //if time > 1, prevpath{i} = trialpath{i}; end;
      const bool isNotFirst = m_CurrentIteration != 1;
      if( isNotFirst ) {
        m_Replicas->CopyCurrentToPreviousTrialPath( cReplica );
      }

      MatrixPointer currentBasePath = this->m_Replicas->GetBasePath( cReplica );
      MatrixType lowTrialPath( 
        currentBasePath->rows(), currentBasePath->cols() );
      m_Replicas->GetPerturbedBasePath( cReplica, &lowTrialPath, sigma, 
        this->GetStartSeeds(), this->GetEndSeeds() );
        
      // trialpath{i} = rethreadpath(lowtrialpath, steps);

// TODO: peturb the trial path and save the perturbed trial path
//      MatrixPointer perturbedTrialPath = 
//        this->m_Replicas->GetCurrentTrialPath( cReplica );
//      perturbedTrialPath = m_Replicas->RethreadPath(
//        &lowTrialPath, this->GetNumberOfSteps() );
      this->m_Replicas->PerturbCurrentTrialPath( cReplica, &lowTrialPath, 
        this->GetNumberOfSteps() );
      MatrixPointer perturbedTrialPath = 
        this->m_Replicas->GetCurrentTrialPath( cReplica );

      // rpath = round(trialpath{i});
      itk::Array2D< int > roundedPath( perturbedTrialPath->rows(),  
                                       perturbedTrialPath->cols() );
                                       
      RoundPath( &roundedPath, perturbedTrialPath );
      
      ArrayPointer odfs[ this->GetNumberOfSteps() ];
      this->GetOdfsAtPoints( odfs, &roundedPath );
      
      // % calculate path energy      
      // energy(i) = odfpathenergy(trialpath{i}, odfs, geo);
      const double meanPathEnergy = 
        CalculateOdfPathEnergy( perturbedTrialPath, odfs, NULL );

      this->m_Replicas->SetCurrentMeanEnergy( cReplica, meanPathEnergy );
                   
      // % check for Metropolis-Hastings update
      // Delta = (energy(i)-energyprev(i))/temp(i);
      // updateprobability = min(1, exp(-Delta));
      // if rand(1) <= updateprobability;
      
      // update the energy, and always set it initially to the first replica
      if( this->m_Replicas->ShouldUpdateEnergy( cReplica ) || 
        ( cReplica == 0 &&  !isNotFirst ) ) {
      
        // basepath{i} = lowtrialpath; 
        // bestpath{i} = trialpath{i};
        m_Replicas->FoundBestPath( cReplica, &lowTrialPath );
                
        // if energy(i) < globalminenergy
        //   globalbestpath = trialpath{i};          
        //   globalminenergy = energy(i);
        // end
        if( this->m_Replicas->GetCurrentMeanEnergy( cReplica ) < 
          this->GetGlobalMinEnergy() ) {

          PoistatsReplica::CopyPath( perturbedTrialPath, &finalBestPath );

          SetGlobalMinEnergy( 
            this->m_Replicas->GetCurrentMeanEnergy( cReplica ) );
          
        }

      } else {
        // energy(i) = energyprev(i);
        this->m_Replicas->ResetCurrentToPreviousEnergy( cReplica );
      }
      
      // if  time > 1 & rand(1) < replicaexchprob
      const bool shouldExchange = isNotFirst && isMoreThanOneReplica &&
        ( m_PoistatsModel->GetRandomNumber() < this->GetReplicaExchangeProbability() );
            
      if( shouldExchange ) {
      
        const double randomNumber = m_PoistatsModel->GetRandomNumber();

        // get a random replica and the next replica with the next highest mean
        int randomReplicaIndex;
        int followingRandomReplicaIndex;
        this->m_Replicas->GetRandomSortedFirstSecondReplicas( 
          randomReplicaIndex, followingRandomReplicaIndex );
          
        const double probabilityExchange = this->m_Replicas->
          CalculateProbablityExchange( randomReplicaIndex, 
                                       followingRandomReplicaIndex );

        if( m_PoistatsModel->GetRandomNumber() <= probabilityExchange ) {

          this->m_Replicas->ExchangeTemperatures( randomReplicaIndex, 
            followingRandomReplicaIndex );          
          m_Exchanges++;
            
        }
        
      }
          
    }
    
    //denergy = (mean(energy)-mean(energyprev))/mean(energy);        
    m_CurrentEnergyDifference = 
      this->m_Replicas->GetNormalizedMeanCurrentPreviousEnergiesDifference();
      
    //    if abs(denergy) < lulldenergy
    //      lull = lull + 1;
    //    else
    //      lull = 0;
    //    end
    if( fabs( m_CurrentEnergyDifference ) < lullEnergyDifferenceThreshold ) {
      m_CurrentLull++;
    } else {
      m_CurrentLull = 0;
    }
        
    //    energyprev = energy;
    this->m_Replicas->CopyCurrentToPreviousEnergies();
        
    //    temp = coolfactor*temp;     
    this->m_Replicas->CoolTemperatures( this->GetCoolFactor() );

    // fprintf('%3d   lull: %2d  denergy: %+5f  mean: %f  bottom: %f   min: %f   globalmin: %f  exchs: %2d  (%f)\n', ...
    // time, lull, denergy, mean(energy), prctile(energy,10), min(energy), globalminenergy, exchanges, tm);

    clock_t endClock = clock();
    const double elapsedTime = 
      static_cast< double >( endClock - startClock ) / CLOCKS_PER_SEC;
    this->SetElapsedTime( elapsedTime );
    
    this->InvokeEvent( IterationEvent() );            
    
  }

  MatrixPointer rethreadedFinalPath = m_Replicas->RethreadPath( &finalBestPath, 
    this->GetNumberOfSamplePoints() );

  this->SetFinalPath( *rethreadedFinalPath );
  finalBestPath = this->GetFinalPath();
  
  delete rethreadedFinalPath;
  rethreadedFinalPath = NULL;
  
  this->InvokeEvent( GenerateOptimalPathDensitiesEvent() );
  OutputImagePointer optimalPathDensity = OutputImageType::New();
  AllocateOutputImage( OPTIMAL_PATH_DENSITY_OUTPUT, optimalPathDensity );
  this->ConvertPointsToImage( &finalBestPath, optimalPathDensity );  

  this->InvokeEvent( GenerateBestReplicaPathDensitiesStartEvent() );
  OutputImagePointer pathDensities = OutputImageType::New();
  AllocateOutputImage( PATH_DENSITY_OUTPUT, pathDensities );
  MatrixListType bestPaths = m_Replicas->GetBestTrialPaths();
  this->GetAggregateReplicaDensities( bestPaths, pathDensities );
  this->InvokeEvent( GenerateBestReplicaPathDensitiesEndEvent() );
  
  this->InvokeEvent( GenerateFinalPathProbabilitiesEvent() );
  this->CalculateFinalPathProbabilities();
  
//  std::cerr << "calculate best path probabilities for each replica..." << std::endl;
//  this->CalculateBestPathProbabilities();
  
//  std::cerr << "calculating optimal path segmentation..." << std::endl;
//  this->CalculateOptimalPathSegmentation( optimalPathDensity );
  
// TODO: remove this
//  PrintFlippedMatlabMatrix( finalBestPath, "FinalBestPath" );

  this->InvokeEvent( EndEvent() );

}

template <class TInputImage, class TOutputImage>
double 
PoistatsFilter<TInputImage, TOutputImage>
::GetCurrentMeanOfEnergies() const {
  return this->m_Replicas->GetCurrentMeanOfEnergies();
}

template <class TInputImage, class TOutputImage>
double 
PoistatsFilter<TInputImage, TOutputImage>
::GetCurrentMinOfEnergies() const {
  return this->m_Replicas->GetMinimumCurrentEnergy();
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateOptimalPathProbabilitiesVolume( 
  OutputImagePointer optimalPathProbabilities ) {

  typedef itk::ImageRegionIterator< OutputImageType > IteratorType;

  IteratorType optimalSamplesIt( optimalPathProbabilities, 
    optimalPathProbabilities->GetLargestPossibleRegion() );
    
  typedef itk::ImageRegionIterator< SegmentationVolumeType > SegIteratorType;
  SegIteratorType segIt( m_OptimalPathSegmentation, 
    m_OptimalPathSegmentation->GetLargestPossibleRegion() );

  for( 
    optimalSamplesIt = optimalSamplesIt.Begin(), 
    segIt = segIt.Begin();
      
    !optimalSamplesIt.IsAtEnd() && !segIt.IsAtEnd();
    
    ++optimalSamplesIt, ++segIt) {
    
    if( segIt.Value() == 1 ) {

      // 1) Get the pixel index
      SegmentationVolumeIndexType currentIndex = segIt.GetIndex();
      
      // 2) determine which sample point in closest to the pixel index
      SamplingVolumeIndexType closestSamplingIndex;
      double closestDistance = 10000; // this is a big number
      MatrixType finalPathDouble = this->GetFinalPath();
      itk::Array2D< int > finalPath( finalPathDouble.rows(), 
        finalPathDouble.cols() );                                     
      RoundPath( &finalPath, &finalPathDouble );
      
      int closestPoint = 0;
      
      for( int cPoint=0; cPoint<finalPath.rows(); cPoint++ ) {
        
        double currentDistance = 0;
        for( int cDim=0; cDim<3; cDim++ ) {
          const double dimDifference = 
            currentIndex[ cDim ] - finalPath[ cPoint ][ cDim ];
          currentDistance += dimDifference * dimDifference;
        }
        currentDistance = sqrt( currentDistance );
        
        if( currentDistance < closestDistance ) {
          closestDistance = currentDistance;
          closestPoint = cPoint;
        }
        
      }
      
      // 3) set the value
      optimalSamplesIt.Set( this->m_FinalPathProbabilities[ closestPoint ] );
      
    } 
    
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateOptimalPathSamples( OutputImagePointer optimalPathSamples ) {

  typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
  IteratorType optimalSamplesIt( optimalPathSamples, 
    optimalPathSamples->GetLargestPossibleRegion() );
  IteratorType samplingIt( m_SamplingVolume, 
    m_SamplingVolume->GetLargestPossibleRegion() );

  typedef itk::ImageRegionIterator< SegmentationVolumeType > SegIteratorType;
  SegIteratorType segIt( m_OptimalPathSegmentation, 
    m_OptimalPathSegmentation->GetLargestPossibleRegion() );

  for( 
    optimalSamplesIt = optimalSamplesIt.Begin(), 
    segIt = segIt.Begin(),
    samplingIt = samplingIt.Begin();
  
    !optimalSamplesIt.IsAtEnd() && !segIt.IsAtEnd() && !samplingIt.IsAtEnd();
    
    ++optimalSamplesIt, ++segIt, ++samplingIt ) {
    
    if( segIt.Value() == 1 ) {

// TODO: removing this temporarily
//      optimalSamplesIt.Set( samplingIt.Value() );
      
      // 1) Get the pixel index
      SegmentationVolumeIndexType currentIndex = segIt.GetIndex();
      
      // 2) determine which sample point in closest to the pixel index
      SamplingVolumeIndexType closestSamplingIndex;
      double closestDistance = 10000; // this is a big number
      MatrixType finalPathDouble = this->GetFinalPath();
      itk::Array2D< int > finalPath( finalPathDouble.rows(), 
        finalPathDouble.cols() );                                     
      RoundPath( &finalPath, &finalPathDouble );
      
      for( int cPoint=0; cPoint<finalPath.rows(); cPoint++ ) {
        
        double currentDistance = 0;
        for( int cDim=0; cDim<3; cDim++ ) {
          const double dimDifference = 
            currentIndex[ cDim ] - finalPath[ cPoint ][ cDim ];
          currentDistance += dimDifference * dimDifference;
        }
        currentDistance = sqrt( currentDistance );
        
        if( currentDistance < closestDistance ) {
          closestDistance = currentDistance;
          for( int cDim=0; cDim<3; cDim++ ) {
            closestSamplingIndex[ cDim ] = finalPath[ cPoint ][ cDim ];
          }
        }
        
      }
      
      // 3) set the value
      optimalSamplesIt.Set( m_SamplingVolume->GetPixel( closestSamplingIndex ) );
      
    } 
    
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateOptimalPathSegmentation( OutputImagePointer optimalPathDensity ) {

  InputImageConstPointer inputImage = this->GetInput();
  
  RegionType dtiRegion = inputImage->GetLargestPossibleRegion();
  SegmentationVolumeSizeType outputSize;
  double outputOrigin[ SegmentationVolumeRegionType::GetImageDimension() ];
  SegmentationVolumeIndexType outputStart;
  SegmentationVolumeSpacingType outputSpacing;

  for( int cDim=0; cDim<SegmentationVolumeRegionType::GetImageDimension(); cDim++ ) {    
    outputSize[ cDim ] = dtiRegion.GetSize()[ cDim ];
    outputOrigin[ cDim ] = this->GetInput()->GetOrigin()[ cDim ];
    outputStart[ cDim ] = 0;
    outputSpacing[ cDim ] = this->GetInput()->GetSpacing()[ cDim ];
  }
      
  SegmentationVolumeRegionType outputRegion;
  outputRegion.SetSize( outputSize );    
  outputRegion.SetIndex( outputStart );
  
  m_OptimalPathSegmentation = SegmentationVolumeType::New();
  m_OptimalPathSegmentation->SetRegions( outputRegion );
  m_OptimalPathSegmentation->SetOrigin( outputOrigin );
  m_OptimalPathSegmentation->SetSpacing( outputSpacing );  

  m_OptimalPathSegmentation->Allocate();

  m_OptimalPathSegmentation->FillBuffer( 0 );
  
  typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
  IteratorType it( optimalPathDensity, 
    optimalPathDensity->GetLargestPossibleRegion() );

  typedef itk::ImageRegionIterator< SegmentationVolumeType > SegIteratorType;
  SegIteratorType segIt( m_OptimalPathSegmentation, 
    m_OptimalPathSegmentation->GetLargestPossibleRegion() );

//  const double threshold = 0.0001;
  const double threshold = 0.0001;
  
  for( it = it.Begin(), segIt = segIt.Begin();
  
    !it.IsAtEnd() && !segIt.IsAtEnd();
    
    ++it, ++segIt ) {
    const double pixelValue = it.Value();
    if( pixelValue > threshold ) {
      segIt.Set( 1 );
    } 
  }
  
}

template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::MatrixType
PoistatsFilter<TInputImage, TOutputImage>
::GetBestPathsProbabilities() {

// TODO: we're using the number of steps rather than the number of sample points
  MatrixType probabilities( this->m_Replicas->GetNumberOfReplicas(), 
                            this->GetNumberOfSteps() );
//                            this->GetNumberOfSamplePoints() );

  this->m_Replicas->GetBestTrialPathsProbabilities( &probabilities );
  
  return probabilities;

}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateBestPathProbabilities() {

  for( int cReplica = 0; 
    cReplica<this->m_Replicas->GetNumberOfReplicas(); cReplica++ ) {

    MatrixPointer replicaPath = this->m_Replicas->GetBestTrialPath( cReplica );
    
    ArrayType probabilities( replicaPath->rows() );
    this->GetPathProbabilities( replicaPath, &probabilities );
    
    this->m_Replicas->SetBestTrialPathProbabilities( cReplica, &probabilities );
  }  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::ConstructOdfList() {
// MATLAB:
//fprintf('Constructing ODF list ... \n');
//flatd = reshape(dtensortmp, [slices*rows*cols 9]);
//odflist = zeros(length(maskidx),size(geo,1));
//for i = 1:length(maskidx)
//  % every 10,000th element, print something
//  if ~mod(i, 1e4)
//      fprintf('%d ',i); 
//  end;
//  d = reshape(flatd(maskidx(i),:), [3 3]);
//  d = d(order, order) .* polarity;
//  d = R*d*invR;
//  odflist(i,:) = tensor2odf(d, geo, 'angulargaussian')';
//end
//fprintf(' done (%g)\n',toc);

  InputImageConstPointer inputImage  = this->GetInput();
    
  SizeType imageSize = inputImage->GetLargestPossibleRegion().GetSize();
  const int nColumns = imageSize[ 0 ];
  const int nRows = imageSize[ 1 ];
  const int nSlices = imageSize[ 2 ];
  
  int cOdfs = 0;
  
  MatrixType polarity = this->GetPolarity();
  
//% rotation matrix from magnet to slice frame
//% only works for oblique axial
//sliceup = [normal_s normal_a 0];
//R = rot3u2v([1 0 0], sliceup);
//invR = inv(R);  

  MatrixType rotationMatrix( 3, 3 );
  GetMagnetToSliceFrameRotation( &rotationMatrix );
  itkDebugMacro( << "magnet to slice frame rotation:" );
  itkDebugMacro( << rotationMatrix );
  
  const MatrixType inverseRotationMatrix =
    vnl_matrix_inverse< double >( rotationMatrix ).inverse();
  const bool isRotationIdentity = rotationMatrix.is_identity();   
     
  OdfLookUpTablePointer odfLookUpTable = this->GetOdfLookUpTable();
  
  this->InvokeEvent( PoistatsOdfCalculationStartEvent() );
      
  for( int cSlice=0; cSlice<nSlices; cSlice++ ) {
  
    for( int cRow=0; cRow<nRows; cRow++ ) {

      for( int cColumn=0; cColumn<nColumns; cColumn++ ) {
      
        const int nTensorRows = 3;
        const int nTensorColumns = 3;
        itk::Matrix< double, nTensorRows, nTensorColumns > tensor;
        
        bool isPixelMasked = false;
        
        if( this->GetMaskVolume() ) {
          MaskVolumeIndexType maskIndex;
          maskIndex[ 0 ] = cColumn;
          maskIndex[ 1 ] = cRow;
          maskIndex[ 2 ] = cSlice;
          
          MaskType pixel = this->GetMaskVolume()->GetPixel( maskIndex );
          
          if( pixel == 0 ) {            
            isPixelMasked = true;
          }
        }
        
        bool hasZero = false;        
        if( !isPixelMasked ) {
        
          IndexType pixelIndex;
          pixelIndex[ 0 ] = cColumn;
          pixelIndex[ 1 ] = cRow;
          pixelIndex[ 2 ] = cSlice;
          
          PixelType currentTensor = inputImage->GetPixel( pixelIndex );
          
          for( int cTensorRow=0; cTensorRow<nTensorRows; cTensorRow++ ) {
          
            for( int cTensorColumn=0; cTensorColumn<nTensorColumns; cTensorColumn++ ) {
            
              tensor[ cTensorRow ][ cTensorColumn ] = 
                currentTensor( cTensorRow, cTensorColumn);
            
              if( tensor[ cTensorRow ][ cTensorColumn] == 0.0 ) {
                              
                hasZero = true;
              }
              
            }
            
          }
          
        }
                        
        if( !hasZero && !isPixelMasked ) {

// DJ: my attempt at explaining what look is.  I'd like to get rid of the look
// up table at some point:
  // look is actually a 3D matrix, the size of the mask.  At every location that
  // the volume contains a non-masked pixel, look will contain an index that
  // cooresponds to the index that the odf is created in m_Odfs
            
          // MATLAB: look = zeros(size(mask));
          
          OdfLookUpIndexType tableIndex;
          tableIndex[ 0 ] = cColumn;
          tableIndex[ 1 ] = cRow;
          tableIndex[ 2 ] = cSlice;
          odfLookUpTable->SetPixel( tableIndex, cOdfs );
          
          itk::Matrix< double, nTensorRows, nTensorColumns > 
            tensorClone( tensor );

          // MATLAB: d = d(order, order) .* polarity;
          for( int cTensorRow=0; cTensorRow<nTensorRows; cTensorRow++ ) {
          
            for( int cTensorColumn=0; cTensorColumn<nTensorColumns; 
              cTensorColumn++ ) {

              // we want flip the tensors rows and columns backwards (not 
              // transpose though)
              int cFlippedTensorRow = ( nTensorRows - 1 ) - cTensorRow;
              int cTensorFlippedColumn = ( nTensorColumns - 1 ) - cTensorColumn;
              
              double flippedValue = 
                tensorClone[ cFlippedTensorRow ][ cTensorFlippedColumn ];
              double currentPolarity = polarity[ cTensorRow ][ cTensorColumn ];
              
              tensor[ cTensorRow ][ cTensorColumn ] = 
                flippedValue * currentPolarity;
              
            }
          
          }
  
//        MATLAB: d = R*d*invR;
          if( !isRotationIdentity ) {
            tensor = rotationMatrix * tensor.GetVnlMatrix() * inverseRotationMatrix;
          }
  
          ArrayPointer odf = new ArrayType( this->GetNumberOfDirections() );
          CalculateTensor2Odf( &tensor, odf);
                    
          this->m_Odfs.push_back( odf );
          
          if( ( cOdfs % 10000 ) == 0 ) {
            this->InvokeEvent( PoistatsOdfCalculationProgressEvent() );            
          }
          cOdfs++;
                    
        }

      }

    }

  }
  
  this->InvokeEvent( PoistatsOdfCalculationEndEvent() );            
}


template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetMagnetToSliceFrameRotation( MatrixPointer rotation ) {

  // MATLAB:
  //   sliceup = [normal_s normal_a 0];
  //   R = rot3u2v([1 0 0], sliceup);

  double identityValues[] = { 1, 0, 0 };  
  ArrayType identity( identityValues, 3 );

  const double normalS = this->GetInput()->GetDirection()( 2, 2 );
  const double normalA = this->GetInput()->GetDirection()( 1, 2 );
  const double normalR = 0.0;
  ArrayType sliceUp( 3 );
  sliceUp( 0 ) = normalS;
  sliceUp( 1 ) = normalA;
  sliceUp( 2 ) = normalR;
  
  GenerateRotationMatrix3u2v( &identity, &sliceUp, rotation );
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GenerateRotationMatrix3u2v( ArrayPointer u, ArrayPointer v, 
  MatrixPointer outputRotationMatrix ) {

  typedef vnl_matrix< double > VnlMatrixType;
  typedef vnl_vector< double > VnlVectorType;  

  //MATLAB: dotprod = u'*v;
  VnlMatrixType uMatrix( u->size(), 1 );
  uMatrix.set_column( 0, *u );

  VnlMatrixType vMatrix( v->size(), 1 );
  vMatrix.set_column( 0, *v );  
  
  VnlMatrixType dotProductMatrix = uMatrix.transpose() * vMatrix;
  const double dotProduct = dotProductMatrix[ 0 ][ 0 ];
  
// MATLAB:  
//  if abs(dotprod - 1) < 1e-3
//    % if vectors aligned
//    R = eye(3);    
//  else
  MatrixType rotation( u->size(), v->size() );

  const double tiny = 1e-3;
  if( fabs( dotProduct - 1 ) < tiny ) {
    // if vectors are aligned
    rotation.set_identity();
  } else {
// MATLAB:
//   % find rotation axis w
//  if abs(dotprod + 1) < 1e-3
//    % if vectors are anti-aligned
//    % rotation axis is undefined and
//    % so need to find arbitrary orthgonal axis
//    if u(1) == 0 & u(2) == 0
//      v2 = [1 0 0]';
//    else
//      v2 = [u(2) -u(1) 0]';  
//    end
//    v2 = v2/norm(v2);
//    w = cross(u,v2); 
//  else
//    w = cross(u,v); 
//  end

    // find rotation axis w
    if( (dotProduct + 1 ) < tiny ) {
      // if vectors are anti-aligned, rotation axis is undefined and so need 
      //  to find arbitrary orthogonal axis
      
      if( ( uMatrix[ 0 ][ 0 ] == 0 ) && ( uMatrix[ 1 ][ 0 ] == 0 ) ) {
        const double identity[] = { 1, 0, 0 };
        vMatrix.set_column( 0, identity );
      } else {
        const double newV[] = { uMatrix[ 1 ][ 0 ], -uMatrix[ 0 ][ 0 ], 0.0 };
        vMatrix.set_column( 0, newV );
      }
      
      const double twoNorm = vMatrix.array_two_norm();
      vMatrix /= twoNorm;
    }
      
    VnlVectorType uVector = uMatrix.get_column( 0 );
    VnlVectorType vVector = vMatrix.get_column( 0 );

    VnlVectorType wVector = vnl_cross_3d< double >( uVector, vVector );

    // MATLAB:    
    //    w = w/norm(w);
    //    x = w(1); 
    //    y = w(2); 
    //    z = w(3);
    VnlMatrixType wMatrix( 3, 1 );
    wMatrix.set_column( 0, wVector );
    const double twoNorm = wMatrix.array_two_norm();
    wMatrix /= twoNorm;
    
    const double x = wMatrix[ 0 ][ 0 ];
    const double y = wMatrix[ 1 ][ 0 ];
    const double z = wMatrix[ 2 ][ 0 ];

    // MATLAB:  
    //    P = [+0 -z +y;
    //         +z +0 -x;
    //         -y +x +0];         

    const double pValues[] = {
       0, -z,  y,
       z,  0, -x,
      -y,  x,  0
    };
    VnlMatrixType p( pValues, 3, 3 );

    // MATLAB:
    //    theta = acos(u'*v);
    VnlMatrixType angle = uMatrix.transpose() * vMatrix;
    const double theta = acos( angle[ 0 ][ 0 ] );

    // MATLAB: R = eye(3) + P*sin(theta) + P*P*(1-cos(theta));

    VnlMatrixType identity( 3, 3 );
    identity.set_identity();
    
    rotation = identity + p * sin( theta ) + p * p * ( 1 - cos (theta ) );
  }

  // copy the matrix to the output  
  for( int cRow=0; cRow<outputRotationMatrix->rows(); cRow++ ) {
    for( int cCol=0; cCol<outputRotationMatrix->cols(); cCol++ ) {
      ( *outputRotationMatrix )[ cRow ][ cCol ] = rotation[ cRow ][ cCol ];
    }
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::RoundPath( itk::Array2D< int > *outputRoundedArray, 
             itk::Array2D< double >* inputArray ) {
  
  for( int cRow = 0; cRow < inputArray->rows(); cRow++ ) {
    for( int cColumn = 0; cColumn < inputArray->cols(); cColumn++ ) {
    
      ( *outputRoundedArray )[ cRow ][ cColumn] = 
        static_cast< int >( round( ( *inputArray)[ cRow ][ cColumn] ) );
      
    }
  }
  
}

template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::OutputImageType *
PoistatsFilter<TInputImage, TOutputImage>
::GetOptimalDensity() {
  return dynamic_cast<OutputImageType*>( 
    this->ProcessObject::GetOutput(PATH_DENSITY_OUTPUT) );
}


template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::OutputImageType *
PoistatsFilter<TInputImage, TOutputImage>
::GetDensity() {
  return dynamic_cast<OutputImageType*>( 
    this->ProcessObject::GetOutput(OPTIMAL_PATH_DENSITY_OUTPUT) );
}

template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::SegmentationVolumePointer
PoistatsFilter<TInputImage, TOutputImage>
::GetOptimalSegmentation() {
  return m_OptimalPathSegmentation;
}

template <class TInputImage, class TOutputImage>
int
PoistatsFilter<TInputImage, TOutputImage>
::GetNumberOfInitialPoints() const {
  return this->m_Seeds.size();
}

/**
 * Returns the path with evenly spaced sample points.
 */
template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::InitPaths() {
//  % initialize paths
//  fprintf('Initializing paths ...\n');  
//  origpath = rethreadpath(initpoints, ncontrolpoints+2);
//  lowtrialpath = origpath;
//  for i = 1:nreplica
//    basepath{i} = origpath;
//    prevpath{i} = rethreadpath(origpath, steps);
//    trialpath{i} = zeros(steps,3);
//    bestpath{i} = zeros(steps,3);
//  end  

  // TODO: I think that it would be better to have the initial points method create the initial points  
  const int nInitialPoints = this->GetNumberOfInitialPoints();  
  const int spatialDimensions = 3;
  
  MatrixType initialPoints( nInitialPoints, spatialDimensions );
  this->GetInitialPoints( &initialPoints );

  this->m_Replicas->SetInitialPoints( &initialPoints );
  
  this->InvokeEvent( SeedsFoundInitialEvent() );
}

/**
 * Returns the starting end points based on the seed locations.
 */
template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetInitialPoints( MatrixPointer initialPoints ) {

/*
  for idx = 1:nseeds
    [i j k] = ind2sub(size(seeds),find(seeds==seedvalues(idx)));
    X = [i j k];
    com = mean(X,1); % com = center of mass
    % jdx = subscript into voxel distance list of smallest distance
    % between com and each voxel in region
    [null jdx] = min(distmat(com, X));
    % min can return multiples in case of ties. arbitrarily select first tie.
    jdx = jdx(1);
    % initpoints = dim1 x dim2 array of subscripts of voxels closest to coms
    % of each roi. dim1 = number of seeds. dim2 = 3 (image dimension)
    initpoints(idx,:) = X(jdx,:);  
  end
*/

  const int nDimensions = 3;

  ArrayType closestPoint( nDimensions );
  for( int cSeed=0; cSeed<this->m_Seeds.size(); cSeed++ ) {

    MatrixPointer seedRegion = this->m_Seeds[ cSeed ];
    
    GetPointClosestToCenter( seedRegion, &closestPoint );    
    itkDebugMacro( << "closest point: " << closestPoint << std::endl );
    
    if( cSeed < initialPoints->rows() ) {
    
      for( int cDim=0; cDim<nDimensions; cDim++ ) {
        ( *initialPoints )[ cSeed ][ cDim ] = closestPoint[ cDim ];
      }
      
    } else {
    
      itkGenericExceptionMacro (<< "too many seeds");            
      
    }
    
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetPointClosestToCenter( MatrixPointer seeds, ArrayPointer closestSeed ) {

  const int nDimensions=closestSeed->size();
  
  ArrayType center( nDimensions );
  GetCenterOfMass( seeds, &center );
  
  double minDistance = std::numeric_limits< double >::max();
  int indexOfMin = 0;
  
  for( int cSeed=0; cSeed<seeds->rows(); cSeed++ ) {  
    vnl_vector< double > seed = seeds->get_row( cSeed );
    
    double distance = GetDistance( &seed, &center );

    // if this distance is the least, then save it
    if( distance < minDistance ) {
      minDistance = distance;
      indexOfMin = cSeed;
    }
    
  }
  
  // copy the closest seed over
  for( int cDim=0; cDim<nDimensions; cDim++ ) {
    ( *closestSeed )[ cDim ] = ( *seeds )[ indexOfMin ][ cDim ];
  }

}

template <class TInputImage, class TOutputImage>
double
PoistatsFilter<TInputImage, TOutputImage>
::GetDistance( vnl_vector< double >* point1, ArrayPointer point2 ) {

  // subtract them element by element
  vnl_vector< double > distanceVector = *point1 - *point2;
  
  double distance = 0.0;
  
  // square each element
  for( int cDim=0; cDim<distanceVector.size(); cDim++ ) {
    distance += distanceVector[ cDim ] * distanceVector[ cDim ];
  }
  
  distance = sqrt( distance );
  return distance;
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetCenterOfMass( MatrixPointer mass, ArrayPointer center ) {

  const double nSeeds = static_cast< double >( mass->rows() );
  const int nDimensions = mass->cols();
  
  // get the mean along the columns
  for( int cDim=0; cDim<nDimensions; cDim++ ) {

    // initialize
    ( *center )[ cDim ] = 0.0;

    for( int cSeeds=0; cSeeds<mass->rows(); cSeeds++ ) {
        
      ( *center )[ cDim ] += ( *mass )[ cSeeds ][ cDim ];
    
    }
    
    ( *center )[ cDim ] /= nSeeds;
    
  }
  
}

template <class TInputImage, class TOutputImage>
int
PoistatsFilter<TInputImage, TOutputImage>
::GetNumberOfControlPoints() {
  return this->m_Replicas->GetNumberOfControlPoints();
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetNumberOfControlPoints( const int nPoints ) {
  this->m_Replicas->SetNumberOfControlPoints( nPoints );
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetNumberOfSteps( const int nSteps ) {
  this->m_Replicas->SetNumberOfSteps( nSteps );
}

template <class TInputImage, class TOutputImage>
int 
PoistatsFilter<TInputImage, TOutputImage>
::GetNumberOfSteps() {
  return this->m_Replicas->GetNumberOfSteps();
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateTensor2Odf( itk::Matrix< double, 3, 3 > *tensor,
                       itk::Array< double > *odf ) {

  const int numberOfOdfs = this->GetNumberOfDirections();
  const int numberOfTensorAxes = 3;  

  // this is the matlab operations that we're doing
  //    odf = sum((geo*pinv(D)).*geo,2).^(-3/2);

  VnlMatrixType geo = this->GetTensorGeometry();

  vnl_svd< double > tensorSvd( tensor->GetVnlMatrix() );
  VnlMatrixType tensorInverse( tensorSvd.pinverse( 3 ) );

  VnlMatrixType morphedGeo( geo * tensorInverse );
  
  typedef vcl_complex< double > ComplexType;
  typedef itk::Array< ComplexType > ComplexArray;
  ComplexArray complexOdf( odf->size() );
  
  ComplexType odfSum( 0.0, 0.0 );
  for( int cRow=0; cRow<numberOfOdfs; cRow++ ) {
    double rowSum = 0.0;
    for( int cColumn=0; cColumn<numberOfTensorAxes; cColumn++ ) {
      rowSum += morphedGeo[ cRow ][ cColumn ] * geo[ cRow ][ cColumn ];
    }
    
    ComplexType complexRowSum( rowSum, 0 );
    complexOdf[ cRow ] = vcl_pow( complexRowSum, -1.5 );
    
    odfSum += complexOdf[ cRow ];
  }
  
  // normalize the odf
  //  odf = odf/sum(odf);
  for( int cOdf=0; cOdf<numberOfOdfs; cOdf++ ) {    
    ( *odf )[ cOdf ] = vcl_abs( complexOdf[ cOdf ] / odfSum );
  }
  
}

template <class TInputImage, class TOutputImage>
double
PoistatsFilter<TInputImage, TOutputImage>
::CalculateOdfPathEnergy( itk::Array2D< double > *path,
                          itk::Array< double > **odfs,
                          ArrayPointer outputEnergies ) {
  
  const int spatialDimension = path->columns();
  
  itk::Array2D< double > pathDifference( path->rows()-1, spatialDimension );
  PoistatsReplica::CalculatePathVectors( path, &pathDifference );
    
  itk::Array< double > magnitude( path->rows() - 1 );
  PoistatsReplica::CalculateMagnitude( &pathDifference, &magnitude );  

  itk::Array2D< double > normalizedPathVectors( pathDifference );
  // normalize the vectors
  for( int cPath=0; cPath<pathDifference.rows(); cPath++ ) {
    for( int cDimension=0; cDimension<pathDifference.cols(); cDimension++ ) {
      normalizedPathVectors[ cPath ][ cDimension ] /= magnitude[ cPath ];
    }
  }
    
  itk::Array< double > anglesBetweenPathVectors( normalizedPathVectors.rows()-1 );
  CalculateAnglesBetweenVectors( &normalizedPathVectors, 
                                 &anglesBetweenPathVectors );

  const int numberOfOdfs = this->GetNumberOfDirections();
  const int numberOfTensorAxes = 3;
  vnl_matrix< double > geo = this->GetTensorGeometryFlipped();
  
  vnl_matrix< double > dotProductPerGeoDirection( normalizedPathVectors * geo.transpose() );
  for( int cRow=0; cRow<dotProductPerGeoDirection.rows(); cRow++ ) {
    for( int cColumn=0; cColumn<dotProductPerGeoDirection.cols(); cColumn++ ) {
      dotProductPerGeoDirection[ cRow ][ cColumn ] = fabs( dotProductPerGeoDirection[ cRow ][ cColumn ] );
    }
  }

  // calculate the angles between path and geo
  itk::Array2D< double > pathGeoAngles( dotProductPerGeoDirection.rows(), dotProductPerGeoDirection.cols() );
  for( int cRow=0; cRow<pathGeoAngles.rows(); cRow++ ) {
    for( int cColumn=0; cColumn<pathGeoAngles.cols(); cColumn++ ) {
      pathGeoAngles[ cRow ][ cColumn ] = acos( dotProductPerGeoDirection[ cRow ][ cColumn ] );
    }
  }  
  
  MatrixType densityMatrix( pathGeoAngles.rows(), pathGeoAngles.cols() );
  CalculateDensityMatrix( &pathGeoAngles, &densityMatrix );
  
//  odflist = odflist(1:(end-1),:); % n - 1 sets of odfs, because they
//                                % correspond to path segments, not path points.    
//% odfvalues are the baysian conditional posterior probability distribution
//%  sum(A,2) will sum along the rows
//odfvalues = sum(odflist.*dm,2);
  itk::Array< double > odfValues( densityMatrix.rows() );
  for( int cRow=0; cRow<densityMatrix.rows(); cRow++ ) {
    
    double odfListSum = 0.0;
    
    // sums along the rows
    for( int cColumn=0; cColumn<densityMatrix.cols(); cColumn++ ) {
      ArrayPointer odfsAtRow = odfs[ cRow ];
      double odf = ( *odfsAtRow )[ cColumn ] ;
      double density = densityMatrix[ cRow ][ cColumn ];
      odfListSum += odf * density;
    }
    
    odfValues[ cRow ] = odfListSum;
  }
  
//% compute energy
//% the energy is defined as the negative logarithm of the conditional
//% confirmational posterior distribution see Habeck et el Physical Review E
//% 72, 031912, 2005, equation 17
//energies = -log(abs(odfvalues)).*nm;
  ArrayType energies( odfValues.size() );
  for( int cRow=0; cRow<energies.size(); cRow++ ) {
    energies[ cRow ] = -log( fabs( odfValues[ cRow ] ) ) * magnitude[ cRow ];
  }
  
  // if the output energies exist, we should copy them out
  if( outputEnergies ) {
    for( int cRow=0; cRow<energies.size(); cRow++ ) {
      ( *outputEnergies )[ cRow ] = energies[ cRow ];
    }
  }

// we don't want sharp turns, so set those very large
// we don't know why energies doesn't remain an array in this case

// TODO: I'm not sure if this is right and I think that I should check ahead of time
//       for this case
//if max(angles) > pi/3, energies = 1e6; end;

  const double largeAngle = M_PI / 3.0;
    
  double meanEnergy = 0.0;
  
  if( anglesBetweenPathVectors.max_value() > largeAngle ) {
  
    const double maxEnergy = 1e6;
    meanEnergy = maxEnergy / magnitude.sum();

    energies.Fill( maxEnergy );

  } else {
    //meanenergy = sum(energies)/sum(nm);
    meanEnergy = energies.sum() / magnitude.sum();
  }
   
  return meanEnergy;
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateDensityMatrix( itk::Array2D< double > *angles, 
  itk::Array2D< double > *densityMatrix ) {

// dm = rownormalize(exp(-8*angles.^2));

  for( int cRow=0; cRow<angles->rows(); cRow++ ) {

    double rowSum = 0.0;

    for( int cColumn=0; cColumn<angles->cols(); cColumn++ ) {

      double angle = ( *angles )[ cRow ][ cColumn ];
      double currentDensity = exp( -8.0 * angle * angle );
      
      ( *densityMatrix )[ cRow ][ cColumn ] = currentDensity;
      rowSum += currentDensity;

    }
    
    // normalize based on the row
    for( int cColumn=0; cColumn<angles->cols(); cColumn++ ) {
      ( *densityMatrix )[ cRow ][ cColumn ] /= rowSum;
    }
    
  }
}

//% angles are the angles between adjacent path vectors, v
//%   ie. v1 dot v2 = cos(theta12)
//angles = acos(abs(sum((v.*circshift(v,[-1 0])),2)));
template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateAnglesBetweenVectors( itk::Array2D< double > *vectors, 
  itk::Array< double > *angles ) {

  for( int cRow=0; cRow<vectors->rows()-1; cRow++ ) {

    double rowSum = 0.0;

    for( int cColumn=0; cColumn<vectors->cols(); cColumn++ ) {

      const double currentCell = ( *vectors )[ cRow ][ cColumn ];
      const double nextCell = ( *vectors )[ cRow+1 ][ cColumn ];
      rowSum += currentCell * nextCell;

    }

    ( *angles )[ cRow ] = acos( fabs( rowSum ) );

  }  

}

template <class TInputImage, class TOutputImage>
void 
PoistatsFilter<TInputImage, TOutputImage>
::GetOdfsAtPoints( itk::Array< double >** outputOdfs, 
                   itk::Array2D< int >* inputPoints ) {
//  idx = inboundsidx(rpath, size(mask));
      
//  % now get the indices that aren't masked
//  odfidx = look(idx);
//  goodindices = odfidx~=0;     
//  odfs(goodindices,:) = abs(odflist(odfidx(goodindices),:));      

  InputImageConstPointer inputImage = this->GetInput();

  // TODO: these will be the dimensions of the mask, rather than the image
  SizeType imageSize = this->GetInput()->GetLargestPossibleRegion().GetSize();

  for( int cPoint=0; cPoint<inputPoints->rows(); cPoint++ ) {

    OdfLookUpIndexType index;

    index[ 0 ] = ( *inputPoints )[ cPoint ][ 0 ];
    index[ 1 ] = ( *inputPoints )[ cPoint ][ 1 ];
    index[ 2 ] = ( *inputPoints )[ cPoint ][ 2 ];
    
    bool isValidIndex = true;                
    for( int cDim=0; cDim<OdfLookUpRegionType::GetImageDimension(); cDim++ ) {
      if( index[ cDim ] < 0 ) {
        isValidIndex = false;
      } else if( index[ cDim ] >= imageSize[ cDim ] ) {
        isValidIndex = false;
      }
    }
    
    OdfLookUpTablePointer table = this->GetOdfLookUpTable();
    int odfIndex = INVALID_INDEX;
    if( isValidIndex ) {
      odfIndex = table->GetPixel( index );
    }
    
    if( odfIndex > INVALID_INDEX ) {    
      outputOdfs[ cPoint ] = this->m_Odfs[ odfIndex ];            
    } else {
      outputOdfs[ cPoint ] = &m_InvalidOdf;
    }
  }

}

template <class TInputImage, class TOutputImage>
void 
PoistatsFilter<TInputImage, TOutputImage>
::GetSortedUniqueSeedValues( SeedVolumePointer volume, 
  std::vector< std::pair< SeedType, int > > *seedValues ) {

  typedef std::pair< SeedType, int > SeedValueCountPairType;
  typedef std::vector< SeedValueCountPairType > SeedPairList;

  itk::ImageRegionConstIterator< SeedVolumeType > seedIt(
    volume, volume->GetLargestPossibleRegion() );

  // get the unique values
  for ( seedIt = seedIt.Begin(); !seedIt.IsAtEnd(); ++seedIt ) {
    
    const SeedType pixelValue = seedIt.Value();
    
    if( pixelValue != 0 ) {
 
      bool isUnique = true;
      
      // where the seed value should be inserted to maintain sorted order
      SeedPairList::iterator insertionIndex = seedValues->begin();
  
      for( SeedPairList::iterator valuesIt = seedValues->begin();
          valuesIt != seedValues->end(); valuesIt++ ) {
        
        const SeedType seedValue = ( *valuesIt ).first;
        
        if( pixelValue == seedValue ) {
          isUnique = false;
          // increment the count
          ( *valuesIt ).second++; 
        } else if( pixelValue > seedValue ) {
          insertionIndex = valuesIt;
        }
        
      }
      
      if( isUnique ) {
        const int size = 1;
        SeedValueCountPairType seedPair( pixelValue, size );
        seedValues->insert( insertionIndex, seedPair );
      }

    }
  }
  
}

template <class TInputImage, class TOutputImage>
itk::Array2D< double >*
PoistatsFilter<TInputImage, TOutputImage>
::GetStartSeeds() {

  return this->m_Seeds[ 0 ];
}

template <class TInputImage, class TOutputImage>
itk::Array2D< double >*
PoistatsFilter<TInputImage, TOutputImage>
::GetEndSeeds() {
  return this->m_Seeds[ this->m_Seeds.size()-1 ];
}

template <class TInputImage, class TOutputImage>
itk::Array< double >
PoistatsFilter<TInputImage, TOutputImage>
::GetSamples() {
  
  typedef itk::BSplineInterpolateImageFunction< SamplingVolumeType, double, double > InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetInputImage( this->m_SamplingVolume );
  interpolator->SetSplineOrder( 3 );

  MatrixType finalPath = this->GetFinalPath();
        
  ArrayType samples( finalPath.rows() );  
  typedef InterpolatorType::ContinuousIndexType ContinuousIndexType;  
  ContinuousIndexType index;
  // evaluate at the final path points
  for( int cPoint=0; cPoint<finalPath.rows(); cPoint++ ) {
    
    for( int cDim=0; cDim<finalPath.cols(); cDim++ ) {
      index[ cDim ] = finalPath[ cPoint ][ cDim ];
    }
    
    samples[ cPoint ] = interpolator->EvaluateAtContinuousIndex( index );
  }
      
  return samples;
}

/**
 * Return in seeds1 the union of seeds1 and seeds2.
 */
template <class TInputImage, class TOutputImage>
void 
PoistatsFilter<TInputImage, TOutputImage>
::TakeUnionOfSeeds( std::vector< std::pair< SeedType, int > > *seeds1,
                    std::vector< SeedType > *seeds2 ) {

  typedef std::pair< SeedType, int > SeedValueCountPairType;
  typedef std::vector< SeedValueCountPairType > SeedPairList;
  
  SeedPairList seedUnion;
  
  for( std::vector< SeedType >::iterator values2It = seeds2->begin();
    values2It != seeds2->end(); ++values2It ) {
        
    const SeedType value2 = ( *values2It );
    
    for( SeedPairList::iterator values1It = seeds1->begin();
      values1It != seeds1->end(); ++values1It ) {
  
      const SeedType value1 = ( *values1It ).first;

      if( value1 == value2 ) {
        seedUnion.push_back( *values1It );
      }

    }
            
  }
  
  seeds1->clear();
  *seeds1 = seedUnion;

}


/**
 * Set the seed volume and parses the volume for the number of different seeds
 * and the pixel values for those seeds.
 */
template <class TInputImage, class TOutputImage>
void 
PoistatsFilter<TInputImage, TOutputImage>
::SetSeedVolume( SeedVolumePointer volume ) {

  this->m_SeedVolume = volume;

}


template <class TInputImage, class TOutputImage>
void 
PoistatsFilter<TInputImage, TOutputImage>
::ParseSeedVolume() {
  
  typedef std::pair< SeedType, int > SeedValueCountPairType;
  typedef std::vector< SeedValueCountPairType > SeedPairList;
  SeedPairList seedValueCountPairs;
  GetSortedUniqueSeedValues( this->m_SeedVolume, &seedValueCountPairs );
  
  if( this->m_SeedValuesToUse.empty() ) {
    this->InvokeEvent( SeedsUsingAllEvent() );
  } else {
    TakeUnionOfSeeds( &seedValueCountPairs, &this->m_SeedValuesToUse );
  }
  
  itk::ImageRegionConstIterator< SeedVolumeType > seedIt(
    this->m_SeedVolume, this->m_SeedVolume->GetLargestPossibleRegion() );

  for( SeedPairList::iterator valuesIt = seedValueCountPairs.begin();
    valuesIt != seedValueCountPairs.end(); valuesIt++ ) {
        
    const SeedType seedValue = ( *valuesIt ).first;
    const int nCurrentSeed = ( *valuesIt ).second;
    
    itkDebugMacro( << "  ( seed value, number of seeds ): (" << seedValue << ", " << nCurrentSeed << " )" );

    MatrixPointer currentSeeds = 
      new MatrixType( nCurrentSeed, SeedVolumeIndexType::GetIndexDimension() );

    int cCurrentSeeds = 0;

    this->m_Seeds.push_back( currentSeeds );

    for ( seedIt = seedIt.Begin(); !seedIt.IsAtEnd(); ++seedIt ) {    

      SeedType pixelValue = seedIt.Value();

      if( pixelValue == seedValue ) {
        SeedVolumeIndexType seedIndex = seedIt.GetIndex();
        for( int cIndex=0; cIndex<SeedVolumeIndexType::GetIndexDimension(); 
          cIndex++ ) {
          
          ( *currentSeeds )[ cCurrentSeeds  ][ cIndex  ] = seedIndex[ cIndex ];
          
        }

        cCurrentSeeds++;
        
      }
      
    }
    
  }

}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::CalculateFinalPathProbabilities() {

  ArrayType probabilities( this->GetNumberOfSamplePoints() );

  MatrixType finalPath = this->GetFinalPath();

  this->GetPathProbabilities( &finalPath, &probabilities );
  
  this->m_FinalPathProbabilities = probabilities;

}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetPathProbabilities( MatrixPointer path, ArrayPointer probabilities ) {

/*
odfs = (1e-200)*ones(steps, length(geo));
rpath = round(globalbestpath);      
idx = inboundsidx(rpath, size(mask));
odfidx = look(idx);
goodindices = odfidx~=0;     
odfs(goodindices,:) = abs(odflist(odfidx(goodindices),:));
[energy energies] = odfpathenergy(globalbestpath, odfs, geo);             
pathprobabilities = exp(-energies);
pathprobabilities = csapi(1:length(pathprobabilities), pathprobabilities, ...
                          linspace(1,length(pathprobabilities),nsamplepoints));
*/

  ArrayPointer odfs[ path->rows() ];
                   
  itk::Array2D< int > roundedPath( path->rows(), path->cols() );                                 

  RoundPath( &roundedPath, path );

  this->GetOdfsAtPoints( odfs, &roundedPath );
  
  ArrayType energies( path->rows()-1 );

  CalculateOdfPathEnergy( path, odfs, &energies );
  
  MatrixType rawProbablities( energies.size(), 3 );
  rawProbablities.Fill( 0.0 );

  for( int cProbability=0; cProbability<rawProbablities.rows(); 
    cProbability++ ) {
    
    rawProbablities[ cProbability ][ 0 ] = exp( -energies[ cProbability ] );
      
  }
  
  const double gridFloor = 0.0;
  const double gridCeiling = 1.0;
  ArrayType originalPathGrid( rawProbablities.rows() );
  PoistatsReplica::SpaceEvenly( &originalPathGrid, gridFloor, gridCeiling );  
  
  MatrixPointer outputProbabilities = m_Replicas->CubicSplineInterpolation( 
    &rawProbablities,
    &originalPathGrid,
    probabilities->size() );
    
  for( int cProbability=0; cProbability<probabilities->size(); 
    cProbability++ ) {
    
    ( *probabilities )[ cProbability ] = 
      ( *outputProbabilities )[ cProbability ][ 0 ];
  
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetRandomSeed( const long seed ) {

  this->m_PoistatsModel->SetRandomSeed( seed );

}


template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::GetAggregateReplicaDensities( 
  MatrixListType replicaPaths,
  OutputImagePointer aggregateDensities ) {

/*
ps = exp(-energy); ps = ps/sum(ps);
density = zeros(size(mask));
for i = 1:nreplica
  if ~mod(i,5), fprintf('%d ',i);end;
  if ps(i) > (1e-1/nreplica) % if replica contributes
    density = density + ps(i)*pnts2vol(bestpath{i}, size(density),.5);
  end
end
density = density / sum(density(:));
*/

  ArrayType probability( m_Replicas->GetNumberOfReplicas() );
  for( int cProbability=0; cProbability<probability.size(); cProbability++ ) {
    probability[ cProbability ] = 
      exp( -m_Replicas->GetCurrentMeanEnergy( cProbability ) );
  }  
  probability /= probability.sum();

  aggregateDensities->FillBuffer( 0.0 );
  
  OutputSizeType size;
  OutputIndexType start;

  for( int cDim=0; cDim<OutputRegionType::GetImageDimension(); cDim++ ) {

    size[ cDim ] = this->GetInput()->GetLargestPossibleRegion().GetSize( cDim );
    start[ cDim ] = 0;
  
  }
  
  OutputRegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  
  typedef itk::ImageRegionIterator< OutputImageType > DensityIteratorType;

  DensityIteratorType aggregateIterator(
    aggregateDensities, aggregateDensities->GetLargestPossibleRegion() );
  
  const double minimumReplicaContributation = 
    1e-1 / static_cast< double >( replicaPaths.size() );
    
  int cReplica = 0;
  for( MatrixListType::iterator replicaIterator = replicaPaths.begin(); 
    replicaIterator != replicaPaths.end(); replicaIterator++ ) {

    this->InvokeEvent( GenerateBestReplicaPathDensitiesProgressEvent() );
    const MatrixPointer currentReplicaPath = ( *replicaIterator );
        
    OutputImagePointer currentReplicaDensity = OutputImageType::New();
    currentReplicaDensity->SetRegions( region );
    currentReplicaDensity->Allocate();
    currentReplicaDensity->FillBuffer( 0 );
    
    const double currentReplicaProbablity = probability[ cReplica ];
    
    if( currentReplicaProbablity > minimumReplicaContributation ) {
  
      this->ConvertPointsToImage( currentReplicaPath, currentReplicaDensity);
      DensityIteratorType currentReplicaIterator(
        currentReplicaDensity,
        currentReplicaDensity->GetLargestPossibleRegion() );

      for(
        currentReplicaIterator = currentReplicaIterator.Begin(),
        aggregateIterator = aggregateIterator.Begin();
        
        !currentReplicaIterator.IsAtEnd() &&
        !aggregateIterator.IsAtEnd();
        
        ++currentReplicaIterator,
        ++aggregateIterator ) {
        
        const double currentReplicaContribution = 
          currentReplicaIterator.Value() * currentReplicaProbablity;

        aggregateIterator.Set( aggregateIterator.Value() + 
          currentReplicaContribution );
        
      }
    
    }
    
    cReplica++;
    
  }

  // get the sum of the image
  double aggregateSum = 0.0;  
  for( aggregateIterator = aggregateIterator.Begin(); 
    !aggregateIterator.IsAtEnd(); ++aggregateIterator ) {
    aggregateSum += aggregateIterator.Value();
  }

  // normalize the image
  const double inverseAggregateSum = 1 / aggregateSum;
  for( aggregateIterator = aggregateIterator.Begin(); 
    !aggregateIterator.IsAtEnd(); ++aggregateIterator ) {
    aggregateIterator.Set( aggregateIterator.Value() * inverseAggregateSum );
  }
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetMaskVolume( MaskVolumePointer volume ) {

  this->m_MaskVolume = volume;
  
  typedef itk::ImageRegionIterator< MaskVolumeType > MaskIteratorType;
  MaskIteratorType maskIterator(
    this->m_MaskVolume, this->m_MaskVolume->GetLargestPossibleRegion() );

  const MaskType validValue = 1;
  const MaskType invalidValue = 0;
  
  // go through all pixels and assign value of 1 for all non-zero
  for( maskIterator = maskIterator.Begin(); !maskIterator.IsAtEnd(); ++maskIterator ) {
    
    MaskType value = maskIterator.Value();
    
    if( value != invalidValue && value != validValue ) {
      maskIterator.Set( 1 );
    }
    
  }

}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::SetNextSeedValueToUse( int seedValue ) {

  this->m_SeedValuesToUse.push_back( seedValue );

}

template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::VnlMatrixType
PoistatsFilter<TInputImage, TOutputImage>
::GetTensorGeometry() {

  if( this->m_TensorGeometry.empty() ) {
  
    // TODO: I think that this is being reached more than once...
  
    const int numberOfOdfs = this->GetNumberOfDirections();
    const int numberOfTensorAxes = 3;  

    this->m_TensorGeometry = VnlMatrixType( *NO_ZERO_SHELL_252,
      numberOfOdfs, numberOfTensorAxes );

    // TODO: right now, the shell is loaded in with the first and last columns
    //       switched, so we need to swap them...maybe, but I think they're symmetric, so maybe not...?
//    for( int cPoint=0; cPoint<this->m_TensorGeometry.rows(); cPoint++ ) {  
//    
//      std::cerr << "*** NOTE: flipped tensor geometry..." << std::endl;
//    
//      const double tmp = this->m_TensorGeometry[ cPoint ][ 0 ];
//      this->m_TensorGeometry[ cPoint ][ 0 ] = this->m_TensorGeometry[ cPoint ][ 2 ];
//      this->m_TensorGeometry[ cPoint ][ 2 ] = tmp;      
//    }
    
  }
  
  return this->m_TensorGeometry;
  
}


// TODO: this is a hack to get the energy to be calculated correctly.  I feel like I need to study this a little more...
template <class TInputImage, class TOutputImage>
typename PoistatsFilter<TInputImage, TOutputImage>::VnlMatrixType
PoistatsFilter<TInputImage, TOutputImage>
::GetTensorGeometryFlipped() {

  if( m_TensorGeometryFlipped.empty() ) {
    const int numberOfOdfs = this->GetNumberOfDirections();
    const int numberOfTensorAxes = 3;  

    this->m_TensorGeometryFlipped = VnlMatrixType( *NO_ZERO_SHELL_252,
      numberOfOdfs, numberOfTensorAxes );
      
    // TODO: right now, the shell is loaded in with the first and last columns
    //       switched, so we need to swap them...maybe, but I think they're symmetric, so maybe not...?
    for( int cPoint=0; cPoint<this->m_TensorGeometryFlipped.rows(); cPoint++ ) {  
      const double tmp = this->m_TensorGeometryFlipped[ cPoint ][ 0 ];
      this->m_TensorGeometryFlipped[ cPoint ][ 0 ] = this->m_TensorGeometryFlipped[ cPoint ][ 2 ];
      this->m_TensorGeometryFlipped[ cPoint ][ 2 ] = tmp;      
    }
    
  }
  
  return m_TensorGeometryFlipped;
  
}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::PrintFlippedMatlabMatrixCombo( itk::Array2D< double > lowTrialPath, 
  itk::Array2D< double > rethreadedPath ){

  std::string matrixName( "MyLowTrialPath" );
  PrintFlippedMatlabMatrix( lowTrialPath, matrixName );
  std::cout << "figure;";
  std::cout << "plot3( " << matrixName << "(:,1), " << matrixName << "(:,2), " << matrixName << "(:,3),'ro' )" << std::endl;
  
  matrixName = "MyRethreadedPath";
  PrintFlippedMatlabMatrix( rethreadedPath, matrixName );
  std::cout << "hold on;" << std::endl;  
  std::cout << "plot3( " << matrixName << "(:,1), " << matrixName << "(:,2), " << matrixName << "(:,3),'bx' )" << std::endl;
  std::cout << "hold on;" << std::endl;  
  std::cout << "plot3( " << matrixName << "(:,1), " << matrixName << "(:,2), " << matrixName << "(:,3) )" << std::endl;

}

template <class TInputImage, class TOutputImage>
void
PoistatsFilter<TInputImage, TOutputImage>
::PrintFlippedMatlabMatrix( itk::Array2D< double > matrix, 
  std::string matrixName ){

  std::cout << matrixName << " = [" << std::endl;

  for( int cRow=0; cRow<matrix.rows(); cRow++ ) {
    for( int cCol=0; cCol<matrix.cols(); cCol++ ) {      
      std::cout << matrix[ cRow ][ matrix.cols()-cCol-1 ] + 1 << "   ";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "];" << std::endl;  

}

} // end namespace itk

#endif
