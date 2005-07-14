/*=========================================================================

Program: Insight Segmentation & Registration Toolkit
Module: $RCSfile: ImageHistogram3.cxx,v $
Language: C++
Date: $Date: 2005/05/07 20:12:37 $
Version: $Revision: 1.8 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "math.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageFileReader.h"
#include "itkJoinImageFilter.h"
#include "itkScalarImageKmeansImageFilter.h"
#include "itkGaussianDensityFunction.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageCastVectorIndexSelectionFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkMaximumDecisionRule.h"
#include "itkHistogramDensityFunction.h"
#include "itkImageToHistogramGenerator.h"
#include "itkImageFileWriter.h" 
    
int main( int argc, char * argv [] )
  {

  if( argc < 4 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage : Histogram inputImageFileName inputLabelImageFileName kmeansLabelmapFileName initialLabelmapFileName" << std::endl;
    return -1;
    }


  // READIN IMAGE FROM FILE
  typedef unsigned short PixelType;
  const unsigned int Dimension = 2;
  typedef itk::Image< PixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType > ReaderType;

  ReaderType::Pointer readerRawData = ReaderType::New();
  ReaderType::Pointer readerLabels = ReaderType::New();

  readerRawData->SetFileName( argv[1] );
  readerLabels->SetFileName( argv[2] );
  try
    {
    readerRawData->Update();
    readerLabels->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while reading image file : "
      << argv[1] << " " << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return -1;
    }

  // when moving to filter, change to GetRequestedRegion()
  InputImageType::RegionType imageRegion = readerRawData->GetOutput()->GetLargestPossibleRegion();

  // K-MEANS CLASSIFICATION
  typedef itk::ScalarImageKmeansImageFilter< InputImageType > KMeansFilterType;

  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
  const unsigned int numberOfInitialClasses = 4;
  const unsigned int useNonContiguousLabels = false;

  kmeansFilter->SetInput( readerRawData->GetOutput() );
  kmeansFilter->SetUseNonContiguousLabels( useNonContiguousLabels );

  for( unsigned k=0; k < numberOfInitialClasses; k++ )
    {
    const double userProvidedInitialMean = k;
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }


  // WRITE KMEANS LABELMAP TO FILE
  typedef KMeansFilterType::OutputImageType KMeansOutputImageType;
  typedef itk::ImageFileWriter< KMeansOutputImageType > KMeansWriterType;

  KMeansWriterType::Pointer kmeansWriter = KMeansWriterType::New();

  const char * kmeansLabelmapFileName = argv[3];
  kmeansWriter->SetInput( kmeansFilter->GetOutput() );
  kmeansWriter->SetFileName( kmeansLabelmapFileName );
  try
    {
    kmeansWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while writing image file : "
      << argv[3] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // GET THE FINAL KMEANS
  KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();

  const unsigned int numberOfClasses = estimatedMeans.Size();

  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << " estimated mean : " << estimatedMeans[i] << std::endl;
    }


  // FIND VARIANCES
  typedef itk::ImageRegionConstIterator< InputImageType > ConstImageIteratorType;
  typedef itk::ImageRegionConstIterator< KMeansOutputImageType > ConstLabelIteratorType;
  typedef itk::Array< double > CovarianceArrayType;

  InputImageType::RegionType inputRegion;
  KMeansOutputImageType::RegionType kmeansOutputRegion;

  InputImageType::SizeType imageSizeType = imageRegion.GetSize();
  KMeansOutputImageType::SizeType labelSizeType =
    kmeansFilter->GetOutput()->GetLargestPossibleRegion().GetSize();

  inputRegion.SetSize( imageSizeType );
  kmeansOutputRegion.SetSize( labelSizeType );

  ConstImageIteratorType itrInputImage( readerRawData->GetOutput(), inputRegion );
  ConstLabelIteratorType itrLabelMap( kmeansFilter->GetOutput(), kmeansOutputRegion );

  CovarianceArrayType sumsOfSquares( numberOfClasses );
  CovarianceArrayType sums( numberOfClasses );
  CovarianceArrayType estimatedCovariances( numberOfClasses );
  CovarianceArrayType count( numberOfClasses );

  itrInputImage.GoToBegin();
  itrLabelMap.GoToBegin();

  // initialize arrays
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    sumsOfSquares[i] = 0;
    sums[i] = 0;
    count[i] = 0;
    }

  // index into image volume using labels. find sumsOfSquares and sums.
  while( !itrInputImage.IsAtEnd() )
    {
    sumsOfSquares[(int)itrLabelMap.Get()] = sumsOfSquares[(int)itrLabelMap.Get()] +
      itrInputImage.Get() * itrInputImage.Get();
    sums[(int)itrLabelMap.Get()] = sums[(int)itrLabelMap.Get()] +
      itrInputImage.Get();
    count[(int)itrLabelMap.Get()] = count[(int)itrLabelMap.Get()] + 1;
    ++itrInputImage;
    ++itrLabelMap;
    }

  // calculate the variance
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    estimatedCovariances[i] =
      (sumsOfSquares[i] / count[i]) - ((sums[i] * sums[i]) / (count[i] * count[i]));
    std::cout << "cluster[" << i << "] ";
    std::cout << " estimated covariance : " << estimatedCovariances[i]
      << "---sumsOfSquares: " << sumsOfSquares[i]
      << "---sums: " << sums[i]
      << "---count: " << count[i] << std::endl;
    }


  // INITIALIZE PRIORS
  typedef itk::Array< double >                         PriorArrayPixelType;
  typedef itk::Image< PriorArrayPixelType, Dimension >  PriorImageType;
  typedef PriorImageType::Pointer                       PriorImagePointer;
  typedef itk::ImageRegionIterator< PriorImageType >    PriorImageIteratorType;

  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetOrigin( readerRawData->GetOutput()->GetOrigin() );
  priors->SetSpacing( readerRawData->GetOutput()->GetSpacing() );
  priors->Allocate(); 
  PriorImageIteratorType itrPriorImage( priors, imageRegion );

  PriorArrayPixelType priorArrayPixel( numberOfClasses );
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    priorArrayPixel[i] = (double)1 / numberOfClasses;
    }
  priors->FillBuffer( priorArrayPixel );


  // CREATE GAUSSIAN DENSITY FUNCTION
  typedef itk::Vector< double, 1 >        MeasurementVectorType;
  typedef itk::Statistics::GaussianDensityFunction< MeasurementVectorType >
                                          MembershipFunctionType;
  typedef MembershipFunctionType::Pointer MembershipFunctionPointer;

  std::vector< MembershipFunctionPointer > membershipFunctions;
  MembershipFunctionType::MeanType meanEstimators;
  MembershipFunctionType::CovarianceType covarianceEstimators;
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    membershipFunctions.push_back( MembershipFunctionType::New() );
    }


  // GENERATE DATA IMAGE
  typedef itk::Array< double >                             MembershipArrayPixelType;
  typedef itk::Image< MembershipArrayPixelType, Dimension > MembershipImageType;
  typedef MembershipImageType::Pointer                      MembershipImagePointer;
  typedef itk::ImageRegionIterator< MembershipImageType >   MembershipImageIteratorType;

  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( readerRawData->GetOutput()->GetOrigin() );
  data->SetSpacing( readerRawData->GetOutput()->GetSpacing() );
  data->Allocate();

  MembershipArrayPixelType membershipArrayPixel( numberOfClasses );
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    membershipArrayPixel[i] = 0.0;
    }
  data->FillBuffer( membershipArrayPixel );

  MembershipImageIteratorType itrDataImage( data, imageRegion );
  MeasurementVectorType mv;

  itrDataImage.GoToBegin();
  itrInputImage.GoToBegin();
  while ( !itrDataImage.IsAtEnd() )
    {
    mv.Fill( itrInputImage.Get() );
    for ( unsigned int i = 0 ; i < numberOfClasses ; i++ )
      {
      meanEstimators.Fill( estimatedMeans[i] );
      covarianceEstimators.Fill( estimatedCovariances[i] );
      membershipFunctions[i]->SetMean( &meanEstimators );
      membershipFunctions[i]->SetCovariance( &covarianceEstimators );
      membershipArrayPixel[i] = membershipFunctions[i]->Evaluate( mv );
      }
    itrDataImage.Set( membershipArrayPixel );
    ++itrInputImage;
    ++itrDataImage;
    }


  // GENERATE POSTERIORS BY APPLYING BAYES' RULE
  typedef itk::Array< double >                              PosteriorArrayPixelType;
  typedef itk::Image< PosteriorArrayPixelType, Dimension >  PosteriorImageType;
  typedef PosteriorImageType::Pointer                       PosteriorImagePointer;
  typedef itk::ImageRegionIterator< PosteriorImageType >    PosteriorImageIteratorType;

  PosteriorImagePointer posteriors = PosteriorImageType::New();
  posteriors->SetRegions( imageRegion );
  posteriors->SetOrigin( readerRawData->GetOutput()->GetOrigin() );
  posteriors->SetSpacing( readerRawData->GetOutput()->GetSpacing() );
  posteriors->Allocate();

  PosteriorArrayPixelType posteriorArrayPixel( numberOfClasses );
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    posteriorArrayPixel[i] = 0.0;
    }
  posteriors->FillBuffer( posteriorArrayPixel );

  MembershipImageIteratorType itrPosteriorImage( posteriors, imageRegion );

  itrDataImage.GoToBegin();
  itrPriorImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  while ( !itrPosteriorImage.IsAtEnd() )
    {
    priorArrayPixel = itrPriorImage.Get();
    membershipArrayPixel = itrDataImage.Get();
    for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
      {
      posteriorArrayPixel[i] = membershipArrayPixel[i] * priorArrayPixel[i];
      }
    itrPosteriorImage.Set( posteriorArrayPixel );
    ++itrDataImage;
    ++itrPriorImage;
    ++itrPosteriorImage;
    }


  // RENORMALIZE POSTERIORS
  float tempSum;
  itrPosteriorImage.GoToBegin();
  while ( !itrPosteriorImage.IsAtEnd() )
    {
    posteriorArrayPixel = itrPosteriorImage.Get();
    tempSum = 0;
    for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
      {
      tempSum = tempSum + posteriorArrayPixel[i];
      }
    for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
      {
      posteriorArrayPixel[i] = posteriorArrayPixel[i] / tempSum;
      }
    itrPosteriorImage.Set( posteriorArrayPixel );
    ++itrDataImage;
    ++itrPriorImage;
    ++itrPosteriorImage;
    }


  // IMAGE ADAPTOR:  VECTOR TO SCALAR IMAGE
  typedef float ScalarPosteriorPixelType;
  typedef itk::Image< ScalarPosteriorPixelType, Dimension > ScalarPosteriorImageType;
  typedef itk::VectorIndexSelectionCastImageFilter<
                                            PosteriorImageType,
                                            ScalarPosteriorImageType >
                                            VectorIndexSelectionCastImageFilterType;
                                            
  VectorIndexSelectionCastImageFilterType::Pointer indexVectorToScalarAdaptor =
                            VectorIndexSelectionCastImageFilterType::New();


  // IMAGE ADAPTOR:  SCALAR TO VECTOR IMAGE
  typedef itk::ImageCastVectorIndexSelectionFilter<
                                            ScalarPosteriorImageType,              
                                            PosteriorImageType >
                                            ImageCastVectorIndexSelectionFilterType;
                                            
  ImageCastVectorIndexSelectionFilterType::Pointer indexScalarToVectorAdaptor =
                            ImageCastVectorIndexSelectionFilterType::New();
  
  indexScalarToVectorAdaptor->GetOutput()->SetRegions( posteriors->GetLargestPossibleRegion() );
  indexScalarToVectorAdaptor->GetOutput()->GetPixelContainer()->SetImportPointer( 
                                                          posteriors->GetBufferPointer(),
                                                          false );
  indexScalarToVectorAdaptor->GetOutput()->Allocate();


  // SMOOTH POSTERIORS
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<
                            ScalarPosteriorImageType, 
                            ScalarPosteriorImageType > SmoothingFilterType;
  
  float timeStep = 0.1;
  int numberOfIterations = 5;
  
  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( indexVectorToScalarAdaptor->GetOutput() );
  smoothingFilter->SetNumberOfIterations( numberOfIterations );
  smoothingFilter->SetTimeStep( timeStep );
  smoothingFilter->SetConductanceParameter( 3.0 );
  indexVectorToScalarAdaptor->SetInput( posteriors );
  indexScalarToVectorAdaptor->SetInput( smoothingFilter->GetOutput() );
                      
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    indexVectorToScalarAdaptor->SetIndex( i );
    indexScalarToVectorAdaptor->SetIndex( i );
    indexScalarToVectorAdaptor->Update();
    }


  // APPLY MAXIMUM A POSTERIORI RULE
  // setup rest of decision rule
  typedef itk::Image< PixelType, Dimension >                LabelOutputImageType;
  typedef itk::ImageRegionIterator< LabelOutputImageType >  LabelImageIteratorType;
  typedef itk::MaximumDecisionRule                          DecisionRuleType;

  LabelOutputImageType::Pointer labels = LabelOutputImageType::New();
  labels->SetRegions( imageRegion );
  labels->SetOrigin( readerRawData->GetOutput()->GetOrigin() );
  labels->SetSpacing( readerRawData->GetOutput()->GetSpacing() );
  labels->Allocate();
  LabelImageIteratorType itrLabelImage( labels, imageRegion );

  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();

  itrLabelImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  std::vector< double > temporaryHolder;  // show this to Luis
  while ( !itrLabelImage.IsAtEnd() )
    {
    for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
      {
      temporaryHolder.push_back( itrPosteriorImage.Get()[i] );
      }
    itrLabelImage.Set( decisionRule->Evaluate( temporaryHolder ) );
    ++itrLabelImage;
    ++itrPosteriorImage;
    }
  
  
  // WRITE INITIAL LABELMAP TO FILE
  typedef itk::ImageFileWriter< LabelOutputImageType > LabelWriterType; 

  LabelWriterType::Pointer labelWriter = LabelWriterType::New();

  const char * initialLabelmapFileName = argv[4];
  labelWriter->SetInput( labels );
  labelWriter->SetFileName( initialLabelmapFileName );
  try
    {
    labelWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while writing image file : "
      << argv[4] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // JOIN IMAGE AND LABEL MAP
  typedef itk::JoinImageFilter< InputImageType, InputImageType > JoinFilterType;
  typedef JoinFilterType::OutputImageType JoinImageType;

  JoinFilterType::Pointer joinFilter = JoinFilterType::New();

  joinFilter->SetInput1( readerRawData->GetOutput() );
  joinFilter->SetInput2( readerLabels->GetOutput() );
  joinFilter->Update();


  // GENERATE HISTOGRAM FROM RAW DATA AND LABEL MAP
  typedef itk::Statistics::ImageToHistogramGenerator<
    JoinImageType > HistogramGeneratorType;
  typedef HistogramGeneratorType::SizeType SizeType;
  typedef HistogramGeneratorType::HistogramType HistogramType;

  HistogramGeneratorType::Pointer histogramGenerator =
    HistogramGeneratorType::New();
  SizeType size;

  size[0] = 256; // number of bins for the gray levels
  size[1] = 4; // number of bins for the labels = number of labels
  histogramGenerator->SetNumberOfBins( size );
  histogramGenerator->SetMarginalScale( 10.0 );
  histogramGenerator->SetInput( joinFilter->GetOutput() );
  histogramGenerator->Compute();

  const HistogramType * histogram = histogramGenerator->GetOutput();
  SizeType histogramSize = histogram->GetSize();

  std::cout << "Histogram row size " << histogramSize[0] << std::endl;
  std::cout << "Histogram column size " << histogramSize[1] << std::endl;

  HistogramType::ConstIterator itr = histogram->Begin();
  HistogramType::ConstIterator end = histogram->End();

  typedef HistogramType::FrequencyType FrequencyType;

  while( itr != end )
    {
    const FrequencyType frequency = itr.GetFrequency();
    //std::cout << frequency << std::endl;
    ++itr;
    }

  std::cout << histogram->GetFrequency( 0, 1 ) << std::endl;
  std::cout << histogram->GetFrequency( 1, 1 ) << std::endl;
  std::cout << histogram->GetFrequency( 2, 1 ) << std::endl;
  std::cout << histogram->GetFrequency( 3, 1 ) << std::endl;


  // MEMBERSHIP FUNCTION
  typedef itk::Statistics::HistogramDensityFunction<
    MeasurementVectorType,
    HistogramType > HistogramDensityFunctionType;

  HistogramDensityFunctionType::Pointer membershipFunction =
    HistogramDensityFunctionType::New();

  membershipFunction->SetHistogram( histogram );

  return 0;
  }
