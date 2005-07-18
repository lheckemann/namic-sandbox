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

  if( argc < 3 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Parameters:  rawDataFileName outputFileName nClasses" << std::endl;
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
  unsigned int nClasses = atoi( argv[3] );
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

  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << " estimated mean : " << estimatedMeans[i] << std::endl;
    }


  // FIND VARIANCES
  typedef KMeansFilterType::OutputImageType                 KMeansImageType;
  typedef itk::ImageRegionConstIterator< RawDataImageType > ConstRawDataIteratorType;
  typedef itk::ImageRegionConstIterator< KMeansImageType >  ConstKMeansIteratorType;
  typedef itk::Array< double >                              CovarianceArrayType;

  ConstRawDataIteratorType itrRawDataImage( readerRawData->GetOutput(), imageRegion );
  ConstKMeansIteratorType itrKMeansImage( kmeansFilter->GetOutput(), imageRegion );

  CovarianceArrayType sumsOfSquares( nClasses );
  CovarianceArrayType sums( nClasses );
  CovarianceArrayType estimatedCovariances( nClasses );
  CovarianceArrayType classCount( nClasses );

  // initialize arrays
  for ( unsigned int i = 0 ; i < nClasses ; ++i )
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
  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    estimatedCovariances[i] =
      (sumsOfSquares[i] / classCount[i]) - ((sums[i] * sums[i]) / (classCount[i] * classCount[i]));
    std::cout << "cluster[" << i << "] ";
    std::cout << " estimated covariance : " << estimatedCovariances[i] << std::endl;
    }


  // INITIALIZE PRIORS
  typedef itk::Array< double >                          PriorArrayPixelType;
  typedef itk::Image< PriorArrayPixelType, Dimension >  PriorImageType;
  typedef PriorImageType::Pointer                       PriorImagePointer;
  typedef itk::ImageRegionIterator< PriorImageType >    PriorImageIteratorType;

  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetOrigin( imageOrigin );
  priors->SetSpacing( imageSpacing );
  priors->Allocate(); 
  PriorImageIteratorType itrPriorImage( priors, imageRegion );

  PriorArrayPixelType priorArrayPixel( nClasses );
  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    priorArrayPixel[i] = (double)1 / nClasses;
    }

  itrPriorImage.GoToBegin();

  while( !itrPriorImage.IsAtEnd() )
    {
    itrPriorImage.Set( priorArrayPixel );
    ++itrPriorImage;
    }


  // CREATE GAUSSIAN DENSITY FUNCTION
  typedef itk::Vector< double, 1 >                          MeasurementVectorType;
  typedef itk::Statistics::GaussianDensityFunction< MeasurementVectorType >
                                                            MembershipFunctionType;
  typedef MembershipFunctionType::Pointer                   MembershipFunctionPointer;

  std::vector< MembershipFunctionPointer >                  membershipFunctions;
  MembershipFunctionType::MeanType                          meanEstimators;
  MembershipFunctionType::CovarianceType                    covarianceEstimators;

  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    membershipFunctions.push_back( MembershipFunctionType::New() );
    }


  // GENERATE DATA IMAGE
  typedef itk::Array< double >                              MembershipArrayPixelType;
  typedef itk::Image< MembershipArrayPixelType, Dimension > MembershipImageType;
  typedef MembershipImageType::Pointer                      MembershipImagePointer;
  typedef itk::ImageRegionIterator< MembershipImageType >   MembershipImageIteratorType;

  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( readerRawData->GetOutput()->GetOrigin() );
  data->SetSpacing( readerRawData->GetOutput()->GetSpacing() );
  data->Allocate();

  MembershipImageIteratorType itrDataImage( data, imageRegion );
  itrDataImage.GoToBegin();

  MembershipArrayPixelType membershipArrayPixel( nClasses );
  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    membershipArrayPixel[i] = 0.0;
    }

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
    for ( unsigned int i = 0 ; i < nClasses ; i++ )
      {
      meanEstimators.Fill( estimatedMeans[i] );
      covarianceEstimators.Fill( estimatedCovariances[i] );
      membershipFunctions[i]->SetMean( &meanEstimators );
      membershipFunctions[i]->SetCovariance( &covarianceEstimators );
      membershipArrayPixel[i] = membershipFunctions[i]->Evaluate( mv );
      }
    itrDataImage.Set( membershipArrayPixel );
    ++itrRawDataImage;
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

  PosteriorArrayPixelType posteriorArrayPixel( nClasses );
  for ( unsigned int i = 0 ; i < nClasses ; ++i )
    {
    posteriorArrayPixel[i] = 0.0;
    }


  PosteriorImageIteratorType itrPosteriorImage( posteriors, imageRegion );

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
    for ( unsigned int i = 0 ; i < nClasses; ++i )
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
    for ( unsigned int i = 0 ; i < nClasses ; ++i )
      {
      tempSum = tempSum + posteriorArrayPixel[i];
      }
    for ( unsigned int i = 0 ; i < nClasses ; ++i )
      {
      posteriorArrayPixel[i] = posteriorArrayPixel[i] / tempSum;
      }
    itrPosteriorImage.Set( posteriorArrayPixel );
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
  
  indexScalarToVectorAdaptor->GetOutput()->SetRegions( imageRegion );
  indexScalarToVectorAdaptor->GetOutput()->GetPixelContainer()->SetImportPointer( 
                                                          posteriors->GetBufferPointer(),
                                                          imageRegion.GetNumberOfPixels(),
                                                          false );

  indexScalarToVectorAdaptor->GetOutput()->Allocate();



  // SETUP SMOOTHING FUNCTION
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<
                            ScalarPosteriorImageType, 
                            ScalarPosteriorImageType > SmoothingFilterType;
  
  float timeStep = 0.1;
  int nIterations = 1;
  
  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( indexVectorToScalarAdaptor->GetOutput() );
  smoothingFilter->SetNumberOfIterations( nIterations );
  smoothingFilter->SetTimeStep( timeStep );
  smoothingFilter->SetConductanceParameter( 3.0 );
  indexVectorToScalarAdaptor->SetInput( posteriors );
  indexScalarToVectorAdaptor->SetInput( smoothingFilter->GetOutput() );
                      
  // PERFORM ITERATIVE SMOOTHING AND RENORMALIZATION OF POSTERIORS
  int nSmoothingIterations = 10;
  for ( unsigned int iSmoothing = 0; iSmoothing < nSmoothingIterations; ++iSmoothing )
    {
    for ( unsigned int i = 0 ; i < nClasses ; ++i )
      {
      indexVectorToScalarAdaptor->SetIndex( i );
      indexScalarToVectorAdaptor->SetIndex( i );
      indexScalarToVectorAdaptor->Update();
      }

    itrPosteriorImage.GoToBegin();
    while ( !itrPosteriorImage.IsAtEnd() )
      {
      posteriorArrayPixel = itrPosteriorImage.Get();
      tempSum = 0;
      for ( unsigned int i = 0 ; i < nClasses ; ++i )
        {
        tempSum = tempSum + posteriorArrayPixel[i];
        }
      for ( unsigned int i = 0 ; i < nClasses ; ++i )
        {
        posteriorArrayPixel[i] = posteriorArrayPixel[i] / tempSum;
        }
      itrPosteriorImage.Set( posteriorArrayPixel );
      ++itrPosteriorImage;
      }
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
  std::vector< double > temporaryHolder(nClasses);  // show this to Luis
  while ( !itrLabelImage.IsAtEnd() )
    {
    for ( unsigned int i = 0 ; i < nClasses ; ++i )
      {
      temporaryHolder[i] = ( itrPosteriorImage.Get()[i] );
      }
    itrLabelImage.Set( decisionRule->Evaluate( temporaryHolder ) );
    ++itrLabelImage;
    ++itrPosteriorImage;
    }
  
  
  // JOIN IMAGE AND LABEL MAP
  typedef itk::JoinImageFilter< RawDataImageType, RawDataImageType > JoinFilterType;
  typedef JoinFilterType::OutputImageType JoinImageType;

  JoinFilterType::Pointer joinFilter = JoinFilterType::New();

  joinFilter->SetInput1( readerRawData->GetOutput() );
  joinFilter->SetInput2( labels );
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
  size[1] = nClasses; // number of bins for the labels = number of labels
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

  std::cout << membershipFunction << std::endl; // DEBUGGING

  // WRITE LABELMAP TO FILE
  typedef itk::ImageFileWriter< LabelOutputImageType > LabelWriterType; 

  LabelWriterType::Pointer labelWriter = LabelWriterType::New();

  const char * labelmapFileName = argv[2];
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
