/*=========================================================================

Program: Insight Segmentation & Registration Toolkit
Module: $RCSfile: BayesianSegmentation.cxx,v $
Language: C++
Date: $Date: 2005/07/25 20:12:37 $
Version: $Revision: 1.0 $

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
#include "itkImageToHistogramGenerator.h"
#include "itkHistogramDensityFunction.h"
#include "itkImageFileWriter.h"

int main( int argc, char * argv [] )
  {

  if( argc < 2 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Parameters: rawDataFileName nClasses" << std::endl;
    return -1;
    }


  // READIN IMAGE FROM FILE
  typedef unsigned short PixelType;
  const unsigned int Dimension = 2;
  typedef itk::Image< PixelType, Dimension > RawDataImageType;
  typedef itk::ImageFileReader< RawDataImageType > RawDataReaderType;

  RawDataReaderType::Pointer readerRawData = RawDataReaderType::New();

  readerRawData->SetFileName( argv[1] );

  try
    {
    readerRawData->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while reading raw data file : "
      << argv[1] << std::endl;
    std::cerr << excp << std::endl;
    return -1;
    }


  // SETUP GENERAL IMAGE PARAMETERS
  // when moving to filter, change to GetRequestedRegion()
  RawDataImageType::RegionType imageRegion =
    readerRawData->GetOutput()->GetLargestPossibleRegion();
  RawDataImageType::SizeType imageSizeType = imageRegion.GetSize();
  const RawDataImageType::PointType& imageOrigin = readerRawData->GetOutput()->GetOrigin();
  RawDataImageType::SpacingType imageSpacing = readerRawData->GetOutput()->GetSpacing();


  // K-MEANS CLASSIFICATION
  unsigned int nClasses = atoi( argv[2] );
  const unsigned int useNonContiguousLabels = false;
  typedef itk::ScalarImageKmeansImageFilter< RawDataImageType > KMeansFilterType;

  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();

  kmeansFilter->SetInput( readerRawData->GetOutput() );
  kmeansFilter->SetUseNonContiguousLabels( useNonContiguousLabels );

  for( unsigned k=0; k < nClasses; k++ )
    {
    const double userProvidedInitialMean = k;
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }
  kmeansFilter->Update();


  // GET THE FINAL KMEANS
  KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();

  nClasses = estimatedMeans.Size();


  // FIND VARIANCES
  typedef KMeansFilterType::OutputImageType KMeansImageType;
  typedef itk::ImageRegionConstIterator< RawDataImageType > ConstRawDataIteratorType;
  typedef itk::ImageRegionConstIterator< KMeansImageType > ConstKMeansIteratorType;
  typedef itk::Array< double > CovarianceArrayType;

  ConstRawDataIteratorType itrRawDataImage( readerRawData->GetOutput(), imageRegion );
  ConstKMeansIteratorType itrKMeansImage( kmeansFilter->GetOutput(), imageRegion );

  CovarianceArrayType sumsOfSquares( nClasses );
  CovarianceArrayType sums( nClasses );
  CovarianceArrayType estimatedCovariances( nClasses );
  CovarianceArrayType classCount( nClasses );

  // initialize arrays
  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    sumsOfSquares[i] = 0;
    sums[i] = 0;
    classCount[i] = 0;
    }

  // index into image volume using labels. find sumsOfSquares and sums.
  itrRawDataImage.GoToBegin();
  itrKMeansImage.GoToBegin();
  while( !itrRawDataImage.IsAtEnd() )
    {
    sumsOfSquares[(int)itrKMeansImage.Get()] = sumsOfSquares[(int)itrKMeansImage.Get()] +
      itrRawDataImage.Get() * itrRawDataImage.Get();
    sums[(int)itrKMeansImage.Get()] = sums[(int)itrKMeansImage.Get()] +
      itrRawDataImage.Get();
    classCount[(int)itrKMeansImage.Get()] = classCount[(int)itrKMeansImage.Get()] + 1;
    ++itrRawDataImage;
    ++itrKMeansImage;
    }

  // calculate the variance
  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    estimatedCovariances[i] =
      (sumsOfSquares[i] / classCount[i]) - ((sums[i] * sums[i]) / (classCount[i] * classCount[i]));

    // print means and covariances
    std::cout << "cluster[" << i << "]-- " << std::endl;
    std::cout << " estimated mean : " << estimatedMeans[i] << std::endl;
    std::cout << " estimated covariance : " << estimatedCovariances[i] << std::endl;
    }


  // INITIALIZE PRIORS
  typedef itk::Array< double > PriorArrayPixelType;
  typedef itk::Image< PriorArrayPixelType, Dimension > PriorImageType;
  typedef PriorImageType::Pointer PriorImagePointer;
  typedef itk::ImageRegionIterator< PriorImageType > PriorImageIteratorType;

  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetOrigin( imageOrigin );
  priors->SetSpacing( imageSpacing );
  priors->Allocate();
  PriorArrayPixelType priorArrayPixel( nClasses );
  PriorImageIteratorType itrPriorImage( priors, imageRegion );

  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    priorArrayPixel[i] = (double)1 / nClasses;
    }
  itrPriorImage.GoToBegin();
  while( !itrPriorImage.IsAtEnd() )
    {
    itrPriorImage.Set( priorArrayPixel );
    ++itrPriorImage;
    }
--itrPriorImage;
std::cout << "Prior image in initial section " << itrPriorImage.Get() << std::endl; //debugging


  // CREATE GAUSSIAN MEMBERSHIP FUNCTIONS
  typedef itk::Vector< double, 1 > MeasurementVectorType;
  typedef itk::Statistics::GaussianDensityFunction< MeasurementVectorType >
    GaussianMembershipFunctionType;
  typedef GaussianMembershipFunctionType::Pointer GaussianMembershipFunctionPointer;

  std::vector< GaussianMembershipFunctionPointer > gaussianMembershipFunctions;
  GaussianMembershipFunctionType::MeanType meanEstimators;
  GaussianMembershipFunctionType::CovarianceType covarianceEstimators;

  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    gaussianMembershipFunctions.push_back( GaussianMembershipFunctionType::New() );
    }


  // GENERATE GAUSSIAN DATA IMAGE
  typedef itk::Array< double > MembershipArrayPixelType;
  typedef itk::Image< MembershipArrayPixelType, Dimension > MembershipImageType;
  typedef MembershipImageType::Pointer MembershipImagePointer;
  typedef itk::ImageRegionIterator< MembershipImageType > MembershipImageIteratorType;

  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( imageOrigin );
  data->SetSpacing( imageSpacing );
  data->Allocate();
  MembershipArrayPixelType membershipArrayPixel( nClasses );
  MembershipImageIteratorType itrDataImage( data, imageRegion );

  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    membershipArrayPixel[i] = 0.0;
    }
  itrDataImage.GoToBegin();
  while( !itrDataImage.IsAtEnd() )
    {
    itrDataImage.Set( membershipArrayPixel );
    ++itrDataImage;
    }

  MeasurementVectorType mv;

  itrDataImage.GoToBegin();
  itrRawDataImage.GoToBegin();
  while ( !itrDataImage.IsAtEnd() )
    {
    mv.Fill( itrRawDataImage.Get() );
    for ( unsigned int i = 0; i < nClasses; i++ )
      {
      meanEstimators.Fill( estimatedMeans[i] );
      covarianceEstimators.Fill( estimatedCovariances[i] );
      gaussianMembershipFunctions[i]->SetMean( &meanEstimators );
      gaussianMembershipFunctions[i]->SetCovariance( &covarianceEstimators );
      membershipArrayPixel[i] = gaussianMembershipFunctions[i]->Evaluate( mv );
      }
    itrDataImage.Set( membershipArrayPixel );
    ++itrRawDataImage;
    ++itrDataImage;
    }
--itrRawDataImage;
std::cout << "RawData image in initial section " << itrRawDataImage.Get() << std::endl; //debugging
--itrDataImage;
std::cout << "Data image in initial section " << itrDataImage.Get() << std::endl; //debugging


  // GENERATE POSTERIORS BY APPLYING BAYES' RULE
  typedef itk::Array< double > PosteriorArrayPixelType;
  typedef itk::Image< PosteriorArrayPixelType, Dimension > PosteriorImageType;
  typedef PosteriorImageType::Pointer PosteriorImagePointer;
  typedef itk::ImageRegionIterator< PosteriorImageType > PosteriorImageIteratorType;

  PosteriorImagePointer posteriors = PosteriorImageType::New();
  posteriors->SetRegions( imageRegion );
  posteriors->SetOrigin( imageOrigin );
  posteriors->SetSpacing( imageSpacing );
  posteriors->Allocate();
  PosteriorArrayPixelType posteriorArrayPixel( nClasses );
  PosteriorImageIteratorType itrPosteriorImage( posteriors, imageRegion );

  for ( unsigned int i = 0; i < nClasses; ++i )
    {
    posteriorArrayPixel[i] = 0.0;
    }
  itrPosteriorImage.GoToBegin();
  while( !itrPosteriorImage.IsAtEnd() )
    {
    itrPosteriorImage.Set( posteriorArrayPixel );
    ++itrPosteriorImage;
    }

  itrDataImage.GoToBegin();
  itrPriorImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  while ( !itrPosteriorImage.IsAtEnd() )
    {
    priorArrayPixel = itrPriorImage.Get();
    membershipArrayPixel = itrDataImage.Get();
    for ( unsigned int i = 0; i < nClasses; ++i )
      {
      posteriorArrayPixel[i] = membershipArrayPixel[i] * priorArrayPixel[i];
      }
    itrPosteriorImage.Set( posteriorArrayPixel );
    ++itrDataImage;
    ++itrPriorImage;
    ++itrPosteriorImage;
    }
--itrPosteriorImage;
std::cout << "Initial Posteriors " << itrPosteriorImage.Get() << std::endl; //debugging

  // RENORMALIZE POSTERIORS
  float renormSum;
  itrPosteriorImage.GoToBegin();
  while ( !itrPosteriorImage.IsAtEnd() )
    {
    posteriorArrayPixel = itrPosteriorImage.Get();
    renormSum = 0;
    for ( unsigned int i = 0; i < nClasses; ++i )
      {
      renormSum = renormSum + posteriorArrayPixel[i];
      }
    for ( unsigned int i = 0; i < nClasses; ++i )
      {
      posteriorArrayPixel[i] = posteriorArrayPixel[i] / renormSum;
      }
    itrPosteriorImage.Set( posteriorArrayPixel );
    ++itrPosteriorImage;
    }
--itrPosteriorImage;
std::cout << "After renormalizing in initial section " << itrPosteriorImage.Get() << std::endl; //debugging


  // IMAGE ADAPTOR: VECTOR TO SCALAR IMAGE
  typedef float ScalarPosteriorPixelType;
  typedef itk::Image< ScalarPosteriorPixelType, Dimension > ScalarPosteriorImageType;
  typedef itk::VectorIndexSelectionCastImageFilter<
    PosteriorImageType,
    ScalarPosteriorImageType >
    VectorIndexSelectionCastImageFilterType;

  VectorIndexSelectionCastImageFilterType::Pointer indexVectorToScalarAdaptor =
    VectorIndexSelectionCastImageFilterType::New();


  // IMAGE ADAPTOR: SCALAR TO VECTOR IMAGE
  typedef itk::ImageCastVectorIndexSelectionFilter<
    ScalarPosteriorImageType,
    PosteriorImageType >
    ImageCastVectorIndexSelectionFilterType;

  ImageCastVectorIndexSelectionFilterType::Pointer indexScalarToVectorAdaptor =
    ImageCastVectorIndexSelectionFilterType::New();

  indexScalarToVectorAdaptor->GetOutput()->SetRegions( imageRegion );
  indexScalarToVectorAdaptor->GetOutput()->GetPixelContainer()->SetImportPointer(
    posteriors->GetBufferPointer(),
    imageRegion.GetNumberOfPixels(),
    false );
  indexScalarToVectorAdaptor->GetOutput()->Allocate();


  // SETUP SMOOTHING FUNCTION
  int nIterations = 1; // always leave this at 1.
  float timeStep = 0.1; // USER VARIABLE (DEFAULT = 0.1)
  float conductance = 3.0; // USER VARIABLE (DEFAULT = 3.0)
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<
    ScalarPosteriorImageType,
    ScalarPosteriorImageType > SmoothingFilterType;

  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( indexVectorToScalarAdaptor->GetOutput() );
  smoothingFilter->SetNumberOfIterations( nIterations );
  smoothingFilter->SetTimeStep( timeStep );
  smoothingFilter->SetConductanceParameter( 3.0 );
  indexVectorToScalarAdaptor->SetInput( posteriors );
  indexScalarToVectorAdaptor->SetInput( smoothingFilter->GetOutput() );


  // PERFORM ITERATIVE SMOOTHING AND RENORMALIZATION OF POSTERIORS
  int nSmoothingIterations = 3; // USER VARIABLE (DEFAULT = 10)
  for ( unsigned int iSmoothing = 0; iSmoothing < nSmoothingIterations; ++iSmoothing )
    {
    for ( unsigned int i = 0; i < nClasses; ++i )
      {
      indexVectorToScalarAdaptor->SetIndex( i );
      indexScalarToVectorAdaptor->SetIndex( i );
      indexScalarToVectorAdaptor->Update();
      }

    itrPosteriorImage.GoToBegin();
    while ( !itrPosteriorImage.IsAtEnd() )
      {
      posteriorArrayPixel = itrPosteriorImage.Get();
      renormSum = 0;
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        renormSum = renormSum + posteriorArrayPixel[i];
        }
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        posteriorArrayPixel[i] = posteriorArrayPixel[i] / renormSum;
        }
      itrPosteriorImage.Set( posteriorArrayPixel );
      ++itrPosteriorImage;
      }
    }
--itrPosteriorImage;
std::cout << "Posteriors after smoothing in initial section " << itrPosteriorImage.Get() << std::endl; //debugging


  // APPLY MAXIMUM A POSTERIORI RULE
  // setup rest of decision rule
  typedef itk::Image< PixelType, Dimension > LabelOutputImageType;
  typedef itk::ImageRegionIterator< LabelOutputImageType > LabelImageIteratorType;
  typedef itk::MaximumDecisionRule DecisionRuleType;

  LabelOutputImageType::Pointer labels = LabelOutputImageType::New();
  labels->SetRegions( imageRegion );
  labels->SetOrigin( imageOrigin );
  labels->SetSpacing( imageSpacing );
  labels->Allocate();
  LabelImageIteratorType itrLabelImage( labels, imageRegion );
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();

  itrLabelImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  while ( !itrLabelImage.IsAtEnd() )
    {
    itrLabelImage.Set( decisionRule->Evaluate( itrPosteriorImage.Get() ) );
    ++itrLabelImage;
    ++itrPosteriorImage;
    }
--itrLabelImage;
std::cout << "Label image in initial section " << itrLabelImage.Get() << std::endl; //debugging
--itrPosteriorImage;
std::cout << "Posteriors after decision rule in initial section " << itrPosteriorImage.Get() << std::endl; //debugging


  // SETUP FILE WRITER
  typedef itk::ImageFileWriter< LabelOutputImageType > LabelWriterType;

  LabelWriterType::Pointer labelWriter = LabelWriterType::New();


  // WRITE LABELMAP TO FILE
  char * labelmapFileName = "z://src/BayesianSegmentationModule/labelsInitial.png";
  labelWriter->SetInput( labels );
  labelWriter->SetFileName( labelmapFileName );
  try
    {
    labelWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while writing image file : "
      << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // SETUP ITERATIVE STATISTICAL REFINEMENT
  int nStatRefineIterations = 4; // USER VARIABLE (DEFAULT = 1)

  // SETUP JOIN IMAGE FILTER
  typedef itk::JoinImageFilter< RawDataImageType, RawDataImageType > JoinFilterType;
  typedef JoinFilterType::OutputImageType JoinImageType;

  JoinFilterType::Pointer joinFilter = JoinFilterType::New();

  // SETUP IMAGE TO HISTOGRAM GENERATOR
  typedef itk::Statistics::ImageToHistogramGenerator<
    JoinImageType >
    HistogramGeneratorType;
  typedef HistogramGeneratorType::HistogramType HistogramType;
  typedef HistogramGeneratorType::SizeType HistogramSizeType;

  HistogramGeneratorType::Pointer histogramGenerator =
    HistogramGeneratorType::New();
  const HistogramType * histogram;
  HistogramSizeType histogramSize;
  HistogramType::ConstIterator itrHistogram;
  HistogramType::ConstIterator endHistogram;

  // SETUP HISTOGRAM DENSITY FUNCTIONS
  typedef itk::Statistics::HistogramDensityFunction<
    MeasurementVectorType,
    HistogramType >
    HistogramMembershipFunctionType;
  typedef HistogramMembershipFunctionType::Pointer HistogramMembershipFunctionPointer;

  HistogramMembershipFunctionPointer histogramMembershipFunction;

  histogramMembershipFunction = HistogramMembershipFunctionType::New();


  // BEGIN ITERATIVE STATISTICAL REFINEMENT
  for ( unsigned int j = 0; j < nStatRefineIterations; ++j )
    {
    // JOIN IMAGE AND LABEL MAP
    joinFilter->SetInput1( readerRawData->GetOutput() );
    joinFilter->SetInput2( labels );
    joinFilter->Update();


    // GENERATE HISTOGRAM FROM RAW DATA AND LABEL MAP
    histogram = histogramGenerator->GetOutput();
    histogramSize[0] = 256; // number of bins for the gray levels
    histogramSize[1] = nClasses; // number of bins for the labels = number of labels
    histogramGenerator->SetNumberOfBins( histogramSize );
    histogramGenerator->SetMarginalScale( 10.0 );
    histogramGenerator->SetInput( joinFilter->GetOutput() );
    histogramGenerator->Compute();
    itrHistogram = histogram->Begin();
    endHistogram = histogram->End();


    // CREATE HISTOGRAM MEMBERSHIP FUNCTIONS
    histogramMembershipFunction->SetHistogram( histogram );


    // GENERATE HISTOGRAM DATA IMAGE
    itrDataImage.GoToBegin();
    itrRawDataImage.GoToBegin();
    itrLabelImage.GoToBegin();
    while ( !itrDataImage.IsAtEnd() )
      {
      mv.Fill( itrRawDataImage.Get() );
      for ( unsigned int i = 0; i < nClasses; i++ )
        {
        histogramMembershipFunction->SetClass( i );
        membershipArrayPixel[i] = histogramMembershipFunction->Evaluate( mv );
        }
      itrDataImage.Set( membershipArrayPixel );
      ++itrLabelImage;
      ++itrRawDataImage;
//std::cout << "Data in statistical loop " << itrDataImage.Get() << std::endl; //debugging
      ++itrDataImage;
      }
--itrLabelImage;
std::cout << "Label image in statistical loop " << itrLabelImage.Get() << std::endl; //debugging
--itrRawDataImage;
std::cout << "RawData image in statistical loop " << itrRawDataImage.Get() << std::endl; //debugging
--itrDataImage;
std::cout << "Data terms in statistical loop " << itrDataImage.Get() << std::endl; //debugging


    // GENERATE POSTERIORS BY APPLYING BAYES' RULE
    itrDataImage.GoToBegin();
    itrPosteriorImage.GoToBegin();
    while ( !itrPosteriorImage.IsAtEnd() )
      {
      posteriorArrayPixel = itrPosteriorImage.Get();
      membershipArrayPixel = itrDataImage.Get();
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        posteriorArrayPixel[i] = membershipArrayPixel[i] * posteriorArrayPixel[i];
        }
      itrPosteriorImage.Set( posteriorArrayPixel );
      ++itrDataImage;
      ++itrPosteriorImage;
      }
--itrPosteriorImage;
std::cout << "Posterior image in statistical loop " << itrPosteriorImage.Get() << std::endl; //debugging


    // RENORMALIZE POSTERIORS
    itrPosteriorImage.GoToBegin();
    while ( !itrPosteriorImage.IsAtEnd() )
      {
      posteriorArrayPixel = itrPosteriorImage.Get();
      renormSum = 0;
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        renormSum = renormSum + posteriorArrayPixel[i];
        }
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        posteriorArrayPixel[i] = posteriorArrayPixel[i] / renormSum;
        }
      itrPosteriorImage.Set( posteriorArrayPixel );
      ++itrPosteriorImage;
      }
--itrPosteriorImage;
std::cout << "Posteriors after renomalize in statistical loop " << itrPosteriorImage.Get() << std::endl; //debugging


    // PERFORM ITERATIVE SMOOTHING AND RENORMALIZATION OF POSTERIORS
    for ( unsigned int iSmoothing = 0; iSmoothing < nSmoothingIterations; ++iSmoothing )
      {
      for ( unsigned int i = 0; i < nClasses; ++i )
        {
        indexVectorToScalarAdaptor->SetIndex( i );
        indexScalarToVectorAdaptor->SetIndex( i );
        indexScalarToVectorAdaptor->Update();
        }

      itrPosteriorImage.GoToBegin();
      while ( !itrPosteriorImage.IsAtEnd() )
        {
        posteriorArrayPixel = itrPosteriorImage.Get();
        renormSum = 0;
        for ( unsigned int i = 0; i < nClasses; ++i )
          {
          renormSum = renormSum + posteriorArrayPixel[i];
          }
        for ( unsigned int i = 0; i < nClasses; ++i )
          {
          posteriorArrayPixel[i] = posteriorArrayPixel[i] / renormSum;
          }
        itrPosteriorImage.Set( posteriorArrayPixel );
        ++itrPosteriorImage;
        }
      }
--itrPosteriorImage;
std::cout << "Posteriors after smoothing in statistical loop " << itrPosteriorImage.Get() << std::endl; //debugging


    // APPLY MAXIMUM A POSTERIORI RULE
    itrLabelImage.GoToBegin();
    itrPosteriorImage.GoToBegin();
    while ( !itrLabelImage.IsAtEnd() )
      {
      itrLabelImage.Set( decisionRule->Evaluate( itrPosteriorImage.Get() ) );
      ++itrLabelImage;
      ++itrPosteriorImage;
      }
--itrPosteriorImage;
std::cout << "Posteriors after decision rule in statistical loop " << itrPosteriorImage.Get() << std::endl; //debugging
--itrLabelImage;
std::cout << "Labels after decision rule in statistical loop " << itrLabelImage.Get() << std::endl; //debugging
    }


  // WRITE LABELMAP TO FILE
  labelmapFileName = "z://src/BayesianSegmentationModule/labelsFinal.png";
  labelWriter->SetInput( labels );
  labelWriter->SetFileName( labelmapFileName );
  try
    {
    labelWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encoutered while writing image file : "
      << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return 0;
  }
