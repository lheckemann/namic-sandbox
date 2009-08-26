#ifndef __FindLargestForgroundFilledMask_h__
#define __FindLargestForgroundFilledMask_h__
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkSimpleFilterWatcher.h"
#include "itkImageRegionIterator.h"
#include <itkBinaryBallStructuringElement.h>
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "vnl/vnl_sample.h"
#include "itkBrains2MaskImageIOFactory.h"
#include "itkConnectedThresholdImageFilter.h"

#include <itkNumericTraits.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkOtsuMultipleThresholdsCalculator.h>
#include <itkCastImageFilter.h>

template <class SImageType>
void ImageMinMax(typename SImageType::Pointer image,
  typename SImageType::PixelType *imageMin,
  typename SImageType::PixelType *imageMax)
{
  typename itk::MinimumMaximumImageFilter<SImageType>::Pointer minmaxFilter
    = itk::MinimumMaximumImageFilter<SImageType>::New();
  minmaxFilter->SetInput(image);
  minmaxFilter->Update();
  *imageMax = minmaxFilter->GetMaximum();
  *imageMin = minmaxFilter->GetMinimum();
}

/**
 *
 *
 * @author hjohnson (8/12/2008)
 *
 * @param SImageType The image type templated over
 * @param image  the itk image to be used to compute the
 *               histograms
 * @param low   The intensity value where "percent" voxels are
 *              above this threshold
 * @param high  The intensity value where "1.0-percent" voxels
 *              are below this threshold
 * @param percent A value between 0.0 and 100.0 representing the
 *                Quantile percentages to be eliminated. NOTE:
 *                This value will be divided by 100.0, so
 *                100.0=100%, and 1.0=.01%.
 * @return Background threshold.
 */
template <class SImageType>
typename SImageType::PixelType
setLowHigh(typename SImageType::Pointer & image,
  typename SImageType::PixelType & low,
  typename SImageType::PixelType & high,
  const float percent)
{
  typename SImageType::PixelType imageMin;
  typename SImageType::PixelType imageMax;
  ImageMinMax<SImageType>(image, &imageMin, &imageMax);

  typedef itk::Statistics::ScalarImageToHistogramGenerator<SImageType>
  HistogramGeneratorType;
  typename HistogramGeneratorType::Pointer histogramGenerator
    = HistogramGeneratorType::New();
  histogramGenerator->SetInput(image);

  //
  int NumberOfBins = static_cast<unsigned int>( imageMax - imageMin + 1 );
  if ( percent > 0.0 )
    {
    NumberOfBins = static_cast<int>( 1.0 / percent );
    // well-behaved when 0 < percent <= 0.5 has been guaranteed elsewhere.
    }
  histogramGenerator->SetNumberOfBins( NumberOfBins );
  histogramGenerator->SetMarginalScale(1.0);
#ifndef ITK_USE_REVIEW_STATISTICS
  histogramGenerator->SetHistogramMin( imageMin );
  histogramGenerator->SetHistogramMax( imageMax );
#endif
  histogramGenerator->Compute();
  typedef typename HistogramGeneratorType::HistogramType HistogramType;
  const HistogramType *histogram = histogramGenerator->GetOutput();

  //  If the number of non-zero bins is <= 2, then it is a binary image, and
  // Otsu won't do:
  //
    {
    typename HistogramType::ConstIterator histIt = histogram->Begin();
    int  num = 0;
    bool saw_lowest = false;
    while ( histIt != histogram->End() )
      {
      // walking a 1-dimensional histogram from low to high:
      if ( histIt.GetFrequency() != 0 )
        {
        ++num;
        high = static_cast<int>( histIt.GetMeasurementVector()[0] + 0.5 );
             // rounding by chopping
        if ( !saw_lowest )
          {
          low = static_cast<int>( histIt.GetMeasurementVector()[0] + 0.5 );
              // rounding by chopping
          saw_lowest = true;
          }
        }
      ++histIt;
      }
    // std::cout << "low " << low << " high " << high << std::endl;
    // std::cout << "Is it binary? N=" << num << " would give no more than two."
    // << std::endl;
    if ( num <= 2 ) // then it is a binary image, and Otsu won't do:
      {
      std::cout
     <<
      "Image handled with only two catgegories; effectively, binary thresholding."
     << std::endl;
      return high;
      }
    }

  typedef itk::OtsuMultipleThresholdsCalculator<HistogramType> OtsuCalcType;
  typename OtsuCalcType::Pointer OtsuCalc = OtsuCalcType::New();
  OtsuCalc->SetInputHistogram(histogram);
  OtsuCalc->SetNumberOfThresholds(1);
  OtsuCalc->Update();
  typename OtsuCalcType::OutputType otsuThresholds = OtsuCalc->GetOutput();

  low
    = static_cast<typename SImageType::PixelType>( histogram->Quantile(0, 0.0F
                                                    +
                                                    percent) );
  high
    = static_cast<typename SImageType::PixelType>( histogram->Quantile(0, 1.0F
                                                    -
                                                    percent) );
  return static_cast<typename SImageType::PixelType>( otsuThresholds[0] );
}

template <class SImageType>
typename SImageType::Pointer
FindLargestForgroundFilledMask(typename SImageType::Pointer readerOutput,
  const double otsuPercentileThreshold,
  const unsigned int closingSize)
{
  typedef itk::Image<unsigned short,
    SImageType::ImageDimension> IntegerImageType;

  typedef itk::BinaryThresholdImageFilter<SImageType,
    IntegerImageType>
  InputThresholdFilterType;
  typename InputThresholdFilterType::Pointer threshold
    = InputThresholdFilterType::New();
  threshold->SetInput (readerOutput);
  threshold->SetInsideValue(itk::NumericTraits<typename IntegerImageType::
        PixelType>::One);
  threshold->SetOutsideValue(itk::NumericTraits<typename IntegerImageType::
        PixelType>::Zero);

  // These checks guarantee that setLowHigh works with at least two bins.
  // Threshold 0.0 is special:  a pure Otsu on 100 bins.
  if ( !( otsuPercentileThreshold >= 0.0 ) )
    {
    std::cout << "Throwing out worthless PercentileThreshold:  "
              << otsuPercentileThreshold << std::endl;
    exit(-1);
    }
  else if ( otsuPercentileThreshold >= 1.5 )
    {
    std::cout
   <<
    "To save the day, PRETENDING an apparently mistaken histogram-trimming threshold >= 1.5 really indicates number of histogram bins (3.5 rounds up and indicates quartiles, etc.):  "
   << otsuPercentileThreshold << std::endl;
    exit(-1);
    }
  else if ( !( otsuPercentileThreshold <= 0.5 ) )
    {
    std::cout
   <<
    "Trimming back worthless PercentileThreshold over the two-tailed maximum of 0.5:  "
   << otsuPercentileThreshold << std::endl;
    exit(-1);
    }
    {
    std::cout << "Computing otsu thresholds with percentile: "
              << otsuPercentileThreshold << "." << std::endl;
    typename SImageType::PixelType threshold_low, threshold_low_foreground,
    threshold_hi;
    threshold_low_foreground = setLowHigh<SImageType>(readerOutput,
                                                      threshold_low,
                                                      threshold_hi,
                                                      otsuPercentileThreshold);
    threshold->SetLowerThreshold(threshold_low_foreground);
    // threshold->SetUpperThreshold(threshold_hi);
    threshold->SetUpperThreshold( itk::NumericTraits<typename SImageType::
          PixelType>::max() );
    std::cout << "LowHigh Thresholds: [" << threshold_low << ","
              << threshold_low_foreground << "," << threshold_hi << "]"
              << std::endl;
    }
  threshold->Update();
  // #define __MAKE_DEBUG_IMAGES__
#ifdef __MAKE_DEBUG_IMAGES__
  typedef itk::ImageFileWriter<SImageType> WriterType;
  bool makeDebugImages = true;
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( threshold->GetOutput() );
      writer->SetFileName( "LINE157ThresholdImage.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif

  typedef itk::ConnectedComponentImageFilter<IntegerImageType,
    IntegerImageType> FilterType;
  typename FilterType::Pointer labelConnectedComponentsFilter = FilterType::New();
  itk::SimpleFilterWatcher watcher(labelConnectedComponentsFilter);
  watcher.QuietOn();
  labelConnectedComponentsFilter->SetInput ( threshold->GetOutput() );
  labelConnectedComponentsFilter->Update();
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( labelConnectedComponentsFilter->GetOutput() );
      writer->SetFileName( "LINE179labelConnectedComponent.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif

  typedef itk::RelabelComponentImageFilter<IntegerImageType,
    IntegerImageType> RelabelType;
  typename RelabelType::Pointer relabel = RelabelType::New();
  relabel->SetInput( labelConnectedComponentsFilter->GetOutput() );
  try
    {
    relabel->Update();
    }
  catch ( itk::ExceptionObject & excep )
    {
    std::cerr << "Relabel: exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( relabel->GetOutput() );
      writer->SetFileName( "LINE207_relabel.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif

  typedef itk::BinaryThresholdImageFilter<IntegerImageType,
    IntegerImageType> ThresholdFilterType;
  unsigned short numObjects = relabel->GetNumberOfObjects();
  std::cout << "Removed " << numObjects - 1 << " smaller objects." << std::endl;
  typename ThresholdFilterType::Pointer LargestFilter
    = ThresholdFilterType::New();
  LargestFilter->SetInput( relabel->GetOutput() );
  LargestFilter->SetInsideValue(itk::NumericTraits<typename IntegerImageType::
        PixelType>::One);
  LargestFilter->SetOutsideValue(itk::NumericTraits<typename IntegerImageType::
        PixelType>::Zero);
  LargestFilter->SetLowerThreshold(1);
  LargestFilter->SetUpperThreshold(1);
  LargestFilter->Update();
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( LargestFilter->GetOutput() );
      writer->SetFileName( "LINE232LargestFilter.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif

  typedef itk::BinaryBallStructuringElement<typename IntegerImageType::
      PixelType,
    IntegerImageType::ImageDimension>
  myKernelType;

  myKernelType dilateBall;
  myKernelType erodeBall;
  typename myKernelType::SizeType dilateBallSize;
  typename myKernelType::SizeType erodeBallSize;
  for ( unsigned int d = 0; d < 3; d++ )
    {
    dilateBallSize[d] = closingSize;
    erodeBallSize[d] = closingSize;
    }
  dilateBall.SetRadius(dilateBallSize);
  dilateBall.CreateStructuringElement();
  erodeBall.SetRadius(erodeBallSize);
  erodeBall.CreateStructuringElement();

  typedef itk::BinaryDilateImageFilter<IntegerImageType, IntegerImageType,
    myKernelType> DilateFilterType;
  typename DilateFilterType::Pointer DilateFilter = DilateFilterType::New();
  // DilateFilter->SetForegroundValue(1);
  DilateFilter->SetDilateValue(1);
  DilateFilter->SetBackgroundValue(0);
  DilateFilter->SetInput( LargestFilter->GetOutput() );
  DilateFilter->SetKernel( dilateBall );
  DilateFilter->Update();
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( DilateFilter->GetOutput() );
      writer->SetFileName( "LINE267Dilate.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif
  typedef itk::BinaryErodeImageFilter<IntegerImageType, IntegerImageType,
    myKernelType> ErodeFilterType;
  typename ErodeFilterType::Pointer ErodeFilter = ErodeFilterType::New();
  // ErodeFilter->SetForegroundValue(1);
  ErodeFilter->SetErodeValue(1);
  ErodeFilter->SetBackgroundValue(0);
  ErodeFilter->SetInput( DilateFilter->GetOutput() );
  ErodeFilter->SetKernel( erodeBall );
  ErodeFilter->Update();
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    typename WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( ErodeFilter->GetOutput() );
      writer->SetFileName( "LINE287Erode.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif
  const typename IntegerImageType::SizeType ImageSize
    = ErodeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  // HACK:  The most robust way to do this would be to find the largest
  // background labeled image, and then choose one of those locations as the
  // seed.
  // For now just choose all the corners as seed points
  typedef itk::ConnectedThresholdImageFilter<IntegerImageType,
    IntegerImageType>
  seededConnectedThresholdFilterType;
  typename seededConnectedThresholdFilterType::Pointer
  seededConnectedThresholdFilter = seededConnectedThresholdFilterType::New();
    {
      {
      const typename IntegerImageType::IndexType SeedLocation = { { 0, 0, 0}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { ImageSize[0] - 1, 0, 0}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { 0, ImageSize[1] - 1, 0}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { ImageSize[0] - 1, ImageSize[1] - 1, 0}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { 0, 0, ImageSize[2] - 1}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { ImageSize[0] - 1, 0, ImageSize[2] - 1}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { 0, ImageSize[1] - 1, ImageSize[2] - 1}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
      {
      const typename IntegerImageType::IndexType SeedLocation
        = { { ImageSize[0] - 1, ImageSize[1] - 1, ImageSize[2] - 1}};
      seededConnectedThresholdFilter->SetSeed(SeedLocation);
      }
    }
  seededConnectedThresholdFilter->SetReplaceValue(100);
  seededConnectedThresholdFilter->SetUpper(0);
  seededConnectedThresholdFilter->SetLower(0);
  seededConnectedThresholdFilter->SetInput( ErodeFilter->GetOutput() );
  seededConnectedThresholdFilter->Update();
#ifdef __MAKE_DEBUG_IMAGES__
  if ( makeDebugImages )
    {
    WriterType::Pointer writer = WriterType::New();
    try
      {
      writer->SetInput ( seededConnectedThresholdFilter->GetOutput() );
      writer->SetFileName( "LINE346seededConnectedThreshold.nii.gz" );
      writer->Update();
      }
    catch ( itk::ExceptionObject & excep )
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
      }
    }
#endif

  typename ThresholdFilterType::Pointer FinalThreshold
    = ThresholdFilterType::New();
  FinalThreshold->SetInput ( seededConnectedThresholdFilter->GetOutput() );
  FinalThreshold->SetInsideValue(itk::NumericTraits<typename IntegerImageType::
        PixelType>::Zero);
  FinalThreshold->SetOutsideValue(itk::NumericTraits<typename IntegerImageType
        ::PixelType>::One);
  FinalThreshold->SetLowerThreshold(100);
  FinalThreshold->SetUpperThreshold(100);
  FinalThreshold->Update();
  typedef itk::CastImageFilter<IntegerImageType, SImageType> outputCasterType;
  typename outputCasterType::Pointer outputCaster = outputCasterType::New();
  outputCaster->SetInput( FinalThreshold->GetOutput() );
  outputCaster->Update();
  typename SImageType::Pointer outputMaskImage = outputCaster->GetOutput();
  return outputMaskImage;
}

#endif // __FindLargestForgroundFilledMask_h__
