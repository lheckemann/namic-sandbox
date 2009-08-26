#include "itkLargestForegroundFilledMaskImageFilter.h"

#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageRegionIterator.h>
//#include <itkSimpleFilterWatcher.h>
#include <itkImageRegionIterator.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <vnl/vnl_sample.h>
#include <vnl/vnl_math.h>
#include <itkBrains2MaskImageIOFactory.h>
#include <itkConnectedThresholdImageFilter.h>

#include <itkNumericTraits.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkScalarImageToHistogramGenerator.h>
// Not this:   #include <itkOtsuMultipleThresholdsCalculator.h>
#include <itkOtsuThresholdImageCalculator.h>
#include <itkCastImageFilter.h>

namespace itk
{
template <class TInputImage, class TOutputImage>
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::LargestForegroundFilledMaskImageFilter() :
  m_OtsuPercentileThreshold(0.01),
  m_ThresholdCorrectionFactor(1.0),
  m_ClosingSize(9),
  m_InsideValue(NumericTraits<typename IntegerImageType::PixelType>::One),
  m_OutsideValue(NumericTraits<typename IntegerImageType::PixelType>::Zero)
{
  //   this->m_InsideValue = 
  //     NumericTraits<typename IntegerImageType::PixelType>::One;
  //   this->m_OutsideValue =
  //     NumericTraits<typename IntegerImageType::PixelType>::Zero;
}

template <class TInputImage, class TOutputImage>
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::~LargestForegroundFilledMaskImageFilter()
{
}

  
template <class TInputImage, class TOutputImage>
void 
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "OtsuPercentileThreshold " 
     << m_OtsuPercentileThreshold << " "
     << "ClosingSize " 
     << m_ClosingSize << " "
     << "InsideValue " 
     << m_InsideValue << " "
     << "OutsideValue " 
     << m_OutsideValue << std::endl;
}

template <class TInputImage, class TOutputImage>
void 
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::ImageMinMax(typename TInputImage::PixelType &imageMin,
              typename TInputImage::PixelType &imageMax)
{
  typename MinimumMaximumImageFilter<TInputImage>::Pointer minmaxFilter
    = MinimumMaximumImageFilter<TInputImage>::New();
  minmaxFilter->SetInput(this->GetInput());
  minmaxFilter->Update();
  imageMax = minmaxFilter->GetMaximum();
  imageMin = minmaxFilter->GetMinimum();
}

/**
 *
 *
 * @author hjohnson (8/12/2008)
 *
 * @param low   The intensity value where "OtsuPercentileThreshold" voxels are
 *              below this threshold
 * @param high  The intensity value where "1.0-OtsuPercentileThreshold" voxels
 *              are above this threshold
 * @return Background threshold.
 */
template <class TInputImage, class TOutputImage>
typename TInputImage::PixelType
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::SetLowHigh(typename TInputImage::PixelType & low,
             typename TInputImage::PixelType & high)
             
{
  typename TInputImage::PixelType imageMin;
  typename TInputImage::PixelType imageMax;
  this->ImageMinMax(imageMin, imageMax);

  typedef Statistics::ScalarImageToHistogramGenerator<TInputImage>
    HistogramGeneratorType;
  typename HistogramGeneratorType::Pointer histogramGenerator
    = HistogramGeneratorType::New();
  histogramGenerator->SetInput(this->GetInput());

  //
  int NumberOfBins = static_cast<unsigned int>( imageMax - imageMin + 1 );
  if ( this->m_OtsuPercentileThreshold > 0.0 )
    {
    NumberOfBins = static_cast<int>( 1.0 / this->m_OtsuPercentileThreshold );
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
  HistogramType *histogram = const_cast<HistogramType *>( histogramGenerator->GetOutput() );
  //  If the number of non-zero bins is <= 2, then it is a binary image, and
  // Otsu won't do:
  //
  typename TInputImage::PixelType minQuantile = 
    static_cast<typename TInputImage::PixelType>
    ( histogram->Quantile(0, 0.0F + this->m_OtsuPercentileThreshold) );
  typename TInputImage::PixelType maxQuantile = 
    static_cast<typename TInputImage::PixelType>
    ( histogram->Quantile(0, 1.0F - this->m_OtsuPercentileThreshold) );
  {
  typename HistogramType::Iterator histIt = histogram->Begin();
  int  num = 0;
  bool saw_lowest = false;
  while ( histIt != histogram->End() )
    {
    // walking a 1-dimensional histogram from low to high:
    double measurement(histIt.GetMeasurementVector()[0]);
    if ( histIt.GetFrequency() != 0 )
      {
      ++num;
      high = static_cast<int>( measurement + 0.5 );
      // rounding by chopping
      if ( !saw_lowest )
        {
        low = static_cast<int>( measurement + 0.5 );
        // rounding by chopping
        saw_lowest = true;
        }
      }
    //std::cout << "histogram: " << num << "   " << histIt.GetMeasurementVector()[0] << "    " 
    //  << histIt.GetFrequency() << " / " << numberOfPixels << " = " <<  histIt.GetFrequency() / numberOfPixels 
    //  << std::endl;
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

  typedef OtsuThresholdImageCalculator<TInputImage> OtsuImageCalcType;
  typename OtsuImageCalcType::Pointer OtsuImageCalc = OtsuImageCalcType::New();
  OtsuImageCalc->SetImage(this->GetInput());
  OtsuImageCalc->Compute();
  typename TInputImage::PixelType otsuThreshold = OtsuImageCalc->GetThreshold();
//std::cout << "whole-image-based otsuThreshold was: " << otsuThreshold << std::endl;

  low = minQuantile;
  high = maxQuantile;
  typename TInputImage::PixelType otsuThresholdResult = 
    static_cast<typename TInputImage::PixelType>
    ( m_ThresholdCorrectionFactor * static_cast<double>( otsuThreshold ));
  return otsuThresholdResult;
}

template <class TInputImage, class TOutputImage>
void
LargestForegroundFilledMaskImageFilter<TInputImage,TOutputImage>
::GenerateData()
{
  this->AllocateOutputs();

  typedef BinaryThresholdImageFilter<OutputImageType,
    IntegerImageType>
    InputThresholdFilterType;
  typename InputThresholdFilterType::Pointer threshold
    = InputThresholdFilterType::New();
  threshold->SetInput (this->GetInput());

  threshold->SetInsideValue(this->m_InsideValue);
  threshold->SetOutsideValue(this->m_OutsideValue);

  // These checks guarantee that setLowHigh works with at least two bins.
  // Threshold 0.0 is special:  a pure Otsu on 100 bins.
  if ( !( this->m_OtsuPercentileThreshold >= 0.0 ) )
    {
    itkExceptionMacro(<< "Throwing out worthless PercentileThreshold:  "
                      << this->m_OtsuPercentileThreshold);
    }
  else if ( this->m_OtsuPercentileThreshold >= 1.5 )
    {
    itkExceptionMacro(
                      << "To save the day, PRETENDING an apparently mistaken histogram-trimming"
                      " threshold >= 1.5 really indicates number of histogram bins"
                      " (3.5 rounds up and indicates quartiles, etc.):  "
                      << this->m_OtsuPercentileThreshold);
    }
  else if ( !( this->m_OtsuPercentileThreshold <= 0.5 ) )
    {
    itkExceptionMacro(<< "Trimming back worthless PercentileThreshold"
                      " over the two-tailed maximum of 0.5:  "
                      << this->m_OtsuPercentileThreshold);
    }
  {
  //  std::cout << "Computing otsu thresholds with percentile: "
  //            << this->m_OtsuPercentileThreshold << "." << std::endl;
  typename OutputImageType::PixelType threshold_low, threshold_low_foreground,
    threshold_hi;
  threshold_low_foreground = 
    this->SetLowHigh(threshold_low,threshold_hi);
  threshold->SetLowerThreshold(threshold_low_foreground);
  // threshold->SetUpperThreshold(threshold_hi);
  threshold->SetUpperThreshold( NumericTraits<typename OutputImageType::
                                PixelType>::max() );
// C'mon, guys.  I need to know what's going on.  Leave this output visible for me.  Please?
#if 1
  std::cout << "LowHigh Thresholds: [" << threshold_low << ","
            << threshold_low_foreground << "," << threshold_hi << "]"
            << std::endl;
#endif
  }
  threshold->Update();

  typedef ConnectedComponentImageFilter<IntegerImageType,
    IntegerImageType> FilterType;
  typename FilterType::Pointer labelConnectedComponentsFilter = FilterType::New();
  //  SimpleFilterWatcher watcher(labelConnectedComponentsFilter);
  //  watcher.QuietOn();
  labelConnectedComponentsFilter->SetInput ( threshold->GetOutput() );
  // labelConnectedComponentsFilter->Update();

  typedef RelabelComponentImageFilter<IntegerImageType,
    IntegerImageType> RelabelType;
  typename RelabelType::Pointer relabel = RelabelType::New();
  relabel->SetInput( labelConnectedComponentsFilter->GetOutput() );

  try
    {
    relabel->Update();
    }
  catch ( ExceptionObject & excep )
    {
    std::cerr << "Relabel: exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  typedef BinaryThresholdImageFilter<IntegerImageType,
    IntegerImageType> ThresholdFilterType;
  // unsigned short numObjects = relabel->GetNumberOfObjects();
  // std::cout << "Removed " << numObjects - 1 << " smaller objects." << std::endl;
  typename ThresholdFilterType::Pointer LargestFilter
    = ThresholdFilterType::New();
  LargestFilter->SetInput( relabel->GetOutput() );
  LargestFilter->SetInsideValue(this->m_InsideValue);
  LargestFilter->SetOutsideValue(this->m_OutsideValue);
  LargestFilter->SetLowerThreshold(1);
  LargestFilter->SetUpperThreshold(1);
  LargestFilter->Update();

  typedef BinaryBallStructuringElement<typename IntegerImageType::
    PixelType,
    IntegerImageType::ImageDimension>
    myKernelType;

  myKernelType dilateBall;
  myKernelType erodeBall;
  typename myKernelType::SizeType dilateBallSize;
  typename myKernelType::SizeType erodeBallSize;
  for ( unsigned int d = 0; d < 3; d++ )
    {
    dilateBallSize[d] = m_ClosingSize;
    erodeBallSize[d] = m_ClosingSize;
    }
  dilateBall.SetRadius(dilateBallSize);
  dilateBall.CreateStructuringElement();
  erodeBall.SetRadius(erodeBallSize);
  erodeBall.CreateStructuringElement();

  typedef BinaryDilateImageFilter<IntegerImageType, IntegerImageType,
    myKernelType> DilateFilterType;
  typename DilateFilterType::Pointer DilateFilter = DilateFilterType::New();
  // DilateFilter->SetForegroundValue(1);
  DilateFilter->SetDilateValue(1);
  DilateFilter->SetBackgroundValue(0);
  DilateFilter->SetInput( LargestFilter->GetOutput() );
  DilateFilter->SetKernel( dilateBall );
  DilateFilter->Update();

  typedef BinaryErodeImageFilter<IntegerImageType, IntegerImageType,
    myKernelType> ErodeFilterType;
  typename ErodeFilterType::Pointer ErodeFilter = ErodeFilterType::New();
  // ErodeFilter->SetForegroundValue(1);
  ErodeFilter->SetErodeValue(1);
  ErodeFilter->SetBackgroundValue(0);
  ErodeFilter->SetInput( DilateFilter->GetOutput() );
  ErodeFilter->SetKernel( erodeBall );
  ErodeFilter->Update();

  const typename IntegerImageType::SizeType ImageSize
    = ErodeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  // HACK:  The most robust way to do this would be to find the largest
  // background labeled image, and then choose one of those locations as the
  // seed.
  // For now just choose all the corners as seed points
  typedef ConnectedThresholdImageFilter<IntegerImageType,
    IntegerImageType>
    seededConnectedThresholdFilterType;
  typename seededConnectedThresholdFilterType::Pointer
    seededConnectedThresholdFilter = seededConnectedThresholdFilterType::New();

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

  seededConnectedThresholdFilter->SetReplaceValue(100);
  seededConnectedThresholdFilter->SetUpper(0);
  seededConnectedThresholdFilter->SetLower(0);
  seededConnectedThresholdFilter->SetInput( ErodeFilter->GetOutput() );
  seededConnectedThresholdFilter->Update();

  typename ThresholdFilterType::Pointer FinalThreshold
    = ThresholdFilterType::New();
  FinalThreshold->SetInput ( seededConnectedThresholdFilter->GetOutput() );
  FinalThreshold->SetInsideValue(this->m_OutsideValue);
  FinalThreshold->SetOutsideValue(this->m_InsideValue);
  FinalThreshold->SetLowerThreshold(100);
  FinalThreshold->SetUpperThreshold(100);
  FinalThreshold->Update();
  typedef CastImageFilter<IntegerImageType, OutputImageType> outputCasterType;
  typename outputCasterType::Pointer outputCaster = outputCasterType::New();
  outputCaster->SetInput( FinalThreshold->GetOutput() );
  
  outputCaster->GraftOutput(this->GetOutput());
  outputCaster->Update();
  this->GraftOutput(outputCaster->GetOutput());
  //  typename OutputImageType::Pointer outputMaskImage = outputCaster->GetOutput();
  //  return outputMaskImage;
}

}
