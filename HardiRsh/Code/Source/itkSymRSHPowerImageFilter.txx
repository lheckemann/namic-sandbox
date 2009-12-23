
#ifndef __itkSymRSHPowerImageFilter_txx
#define __itkSymRSHPowerImageFilter_txx

#include "itkSymRSHPowerImageFilter.h"
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
SymRSHPowerImageFilter<TInputImage,TOutputComponentType>
::SymRSHPowerImageFilter()
{
  m_Normalize=false;
}

template <class TInputImage, class TOutputComponentType>
void
SymRSHPowerImageFilter<TInputImage,TOutputComponentType>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  itkDebugMacro("SymRSHPowerImageFilter::ThreadedGenerateData(  ) ");

  // Get the input and output pointers
  InputImageConstPointer  inputPtr  = this->GetInput(0);
  OutputImagePointer      outputPtr = this->GetOutput(0);

  // Create a progress reporter
  ProgressReporter progress(this, threadId, (outputPtr->GetRequestedRegion().GetNumberOfPixels()) );


  //Iterator typedefs
  typedef ImageRegionConstIterator<TInputImage>     InputIterator;
  typedef ImageRegionIterator<TOutputImage>         OutputIterator;

//  typedef typename OutputPixelType::RealValueType   RealValueType;
  typedef typename OutputPixelType::ValueType   RealValueType;

  InputIterator inputIt   = InputIterator(inputPtr, inputPtr->GetRequestedRegion());
  OutputIterator outputIt = OutputIterator(outputPtr, outputPtr->GetRequestedRegion());

  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while ( !outputIt.IsAtEnd() )
  {
    unsigned int numberOfOrders = InputPixelType::NumberOfOrders;
    OutputPixelType vec;
    vec.Fill(0);

    InputPixelType input = inputIt.Get();

    for (unsigned int i=0;i<numberOfOrders;++i)
    {
      RealValueType sum = NumericTraits<RealValueType>::Zero;
      int l = 2*i;
      for (int m = -l;m<=l;m++)
      {
        RealValueType tmp
          = static_cast<RealValueType>(input.GetLthMthComponent(l,m));

        if ( isnan(tmp) || isinf(tmp) ) ///TODO Something is wrong upstream. Fix it later
          tmp = 0;

        sum+= tmp * tmp;
      }

      vec[i] = static_cast<typename OutputPixelType::ValueType>(sqrt(sum));
    }
    //normalize
    if (m_Normalize)
    {
      if (vec.GetNorm() > 0.0)
        vec.Normalize();
    }


    outputIt.Set(vec);
    ++outputIt;
    ++inputIt;
  }

  itkDebugMacro(<< "SymRSHPowerImageFilter::GenerateData(  ) Complete");

}

} // end namespace itk

#endif
