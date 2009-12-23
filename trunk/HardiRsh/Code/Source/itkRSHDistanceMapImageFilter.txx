
#ifndef __itkRSHDistanceMapImageFilter_txx
#define __itkRSHDistanceMapImageFilter_txx

#include "itkRSHDistanceMapImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkObjectFactory.h"
#include "itkExtractImageFilterRegionCopier.h"
#include "itkProgressReporter.h"


namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputComponentType>
RSHDistanceMapImageFilter<TInputImage,TOutputComponentType>
::RSHDistanceMapImageFilter()
{
  m_Metric=FullL2Metric;
  m_SeedIndex.Fill(0);
}



template <class TInputImage, class TOutputComponentType>
void
RSHDistanceMapImageFilter<TInputImage,TOutputComponentType>
::BeforeThreadedGenerateData()
{
  if (!this->m_IndexSet)
  {
    itkExceptionMacro( << "Please set the Seed Index" );
  }

  if (! this->GetInput(0)->GetLargestPossibleRegion().IsInside(this->m_SeedIndex) )
  {
    itkExceptionMacro( << "Specified Index is outside of supplied Region" );
  }

}

template <class TInputImage, class TOutputComponentType>
void
RSHDistanceMapImageFilter<TInputImage,TOutputComponentType>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  itkDebugMacro("RSHDistanceMapImageFilter::ThreadedGenerateData(  ) ");

  // Get the input and output pointers
  InputImageConstPointer  inputPtr  = this->GetInput(0);
  OutputImagePointer      outputPtr = this->GetOutput(0);

  // Create a progress reporter
  ProgressReporter progress(this, threadId, (outputPtr->GetRequestedRegion().GetNumberOfPixels()) );

  //Iterator typedefs
  typedef ImageRegionConstIterator<TInputImage>     InputIterator;
  typedef ImageRegionIterator<TOutputImage>         OutputIterator;

  typedef typename NumericTraits<OutputPixelType>::RealType
                                                    RealOutputType;
  typedef typename NumericTraits<InputPixelType>::RealType
                                                    RealInputType;

  InputIterator inputIt   = InputIterator(inputPtr, inputPtr->GetRequestedRegion());
  OutputIterator outputIt = OutputIterator(outputPtr, outputPtr->GetRequestedRegion());

  //Compute seed Point and Value
  InputImagePointType seedPoint;
  const RealInputType seedValue = static_cast<RealInputType>
                                      (inputPtr->GetPixel(m_SeedIndex));
  inputPtr->TransformIndexToPhysicalPoint(m_SeedIndex,seedPoint);
  
  inputIt.GoToBegin();
  outputIt.GoToBegin();

  RealOutputType metricValue;
  
  while ( !outputIt.IsAtEnd() )
  {
    metricValue = NumericTraits<RealOutputType>::Zero;

    const RealInputType input = static_cast<RealInputType>
                                      (inputIt.Get());

    if (this->GetMetric() == Self::FullL2Metric)
    {
      for (unsigned int i=0;i<InputPixelType::Dimension;++i)
      {
        metricValue += static_cast<RealOutputType> 
                    ((input[i] - seedValue[i])*(input[i] - seedValue[i])); 
      }
    }
    else
    {
      for (unsigned int i=0;i<InputPixelType::NumberOfOrders;++i)
      {
        RealOutputType seedSum = NumericTraits<RealOutputType>::Zero;
        RealOutputType valueSum = NumericTraits<RealOutputType>::Zero;
        int l = 2*i;
        for (int m = -l;m<=l;m++)
        {
          RealOutputType tmp
            = static_cast<RealOutputType>(input.GetLthMthComponent(l,m));
          valueSum += tmp * tmp;
          
          tmp = static_cast<RealOutputType>(seedValue.GetLthMthComponent(l,m));
          seedSum += tmp * tmp;
        }
        metricValue += static_cast<RealOutputType>
          ( (vcl_sqrt(valueSum)-vcl_sqrt(seedSum)) *
            (vcl_sqrt(valueSum)-vcl_sqrt(seedSum)) ); 
      }
    }
    
    outputIt.Set(metricValue);
    progress.CompletedPixel();
    ++outputIt;
    ++inputIt;
  }

  itkDebugMacro(<< "RSHDistanceMapImageFilter::GenerateData(  ) Complete");

}

} // end namespace itk

#endif
