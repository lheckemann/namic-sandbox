/*=========================================================================

author: Bryn Lloyd, Computational Radiology Laborotory (CRL), Brigham and Womans
  date: 06/30/2005

=========================================================================*/
#ifndef _itkDiadicProductOfGradientRecursiveGaussianImageFilter_txx
#define _itkDiadicProductOfGradientRecursiveGaussianImageFilter_txx

#include "itkDiadicProductOfGradientRecursiveGaussianImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{


/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::DiadicProductOfGradientRecursiveGaussianImageFilter()
{
  m_NormalizeAcrossScale = false;

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ] = GaussianFilterType::New();
    m_SmoothingFilters[ i ]->SetOrder( GaussianFilterType::ZeroOrder );
    m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
    m_SmoothingFilters[ i ]->ReleaseDataFlagOn();
    }

  m_DerivativeFilter = DerivativeFilterType::New();
  m_DerivativeFilter->SetOrder( DerivativeFilterType::FirstOrder );
  m_DerivativeFilter->SetNormalizeAcrossScale( m_NormalizeAcrossScale );

  m_DerivativeFilter->SetInput( this->GetInput() );

  m_SmoothingFilters[0]->SetInput( m_DerivativeFilter->GetOutput() );

  for( unsigned int i = 1; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetInput(
      m_SmoothingFilters[i-1]->GetOutput() );
    }

  m_ImageAdaptor = OutputImageAdaptorType::New();

  this->SetSigma( 1.0 );

}


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::SetSigma( RealType sigma )
{

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetSigma( sigma );
    }
  m_DerivativeFilter->SetSigma( sigma );

  this->Modified();

}



/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( bool normalize )
{

  m_NormalizeAcrossScale = normalize;

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( normalize );
    }
  m_DerivativeFilter->SetNormalizeAcrossScale( normalize );

  this->Modified();

}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>::InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if (out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}

/**
 * Compute filter for Gaussian kernel
 */
template <typename TInputImage, typename TOutputImage >
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage >
::GenerateData(void)
{
 // Create a process accumulator for tracking the progress of this
  // minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Compute the contribution of each filter to the total progress.
  //
  // Correct this part!!!!! blloyd
  //
  const double weight = 1.0 / ( ImageDimension * ImageDimension );

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    progress->RegisterInternalFilter( m_SmoothingFilters[i], weight );
    }

  progress->RegisterInternalFilter( m_DerivativeFilter, weight );
  progress->ResetProgress();

  const typename TInputImage::ConstPointer   inputImage( this->GetInput() );

  m_ImageAdaptor->SetImage( this->GetOutput() );

  m_ImageAdaptor->SetLargestPossibleRegion(
    inputImage->GetLargestPossibleRegion() );

  m_ImageAdaptor->SetBufferedRegion(
    inputImage->GetBufferedRegion() );

  m_ImageAdaptor->SetRequestedRegion(
    inputImage->GetRequestedRegion() );

  m_ImageAdaptor->Allocate();

  m_DerivativeFilter->SetInput( inputImage );

  for( unsigned int dim=0; dim < ImageDimension; dim++ )
    {
    unsigned int i=0;
    unsigned int j=0;
    while(  i< ImageDimension)
      {
      if( i == dim )
        {
        j++;
        }
      m_SmoothingFilters[ i ]->SetDirection( j );
      i++;
      j++;
      }
    m_DerivativeFilter->SetDirection( dim );

    GaussianFilterPointer lastFilter = m_SmoothingFilters[ImageDimension-2];

    lastFilter->Update();

    progress->ResetFilterProgressAndKeepAccumulatedProgress();

    // Copy the results to the corresponding component
    // on the output image of vectors
    m_ImageAdaptor->SelectNthElement( dim );

    typename RealImageType::Pointer derivativeImage =
      lastFilter->GetOutput();

    ImageRegionIteratorWithIndex< RealImageType > it(
      derivativeImage,
      derivativeImage->GetRequestedRegion() );

    ImageRegionIteratorWithIndex< OutputImageAdaptorType > ot(
      m_ImageAdaptor,
      m_ImageAdaptor->GetRequestedRegion() );

    const RealType spacing = inputImage->GetSpacing()[ dim ];

    it.GoToBegin();
    ot.GoToBegin();
    while( !it.IsAtEnd() )
      {
      ot.Set( it.Get() / spacing );
      ++it;
      ++ot;
      }

    }

// NEW ************************************

  // Finally calculate the outer (diadic) product of the gradient.
  ImageRegionIteratorWithIndex< OutputImageType > ottensor(
    this->GetOutput(),
    this->GetOutput()->GetRequestedRegion() );

  ImageRegionIteratorWithIndex< OutputImageType > itgradient(
    this->GetOutput(),
    this->GetOutput()->GetRequestedRegion() );

  unsigned int numberTensorElements = (ImageDimension*(ImageDimension+1))/2;
  PixelType tmp[numberTensorElements];

  ottensor.GoToBegin();
  itgradient.GoToBegin();
  while( !itgradient.IsAtEnd() )
    {
    unsigned int count = 0;
    for (int j = 0; j < ImageDimension; ++j)
      {
      for (int k = j; k < ImageDimension; ++k)
        {
        tmp[count++] = itgradient.Get()[j]*itgradient.Get()[k];
        }
      }
    for (int j = 0; j < numberTensorElements; ++j)
      {
      ottensor.Value()[j] = tmp[j];
      }

//std::cout << ottensor.Value() << std::endl;
    ++itgradient;
    ++ottensor;
    }

// END NEW *************************************


}


template <typename TInputImage, typename TOutputImage>
void
DiadicProductOfGradientRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "NormalizeAcrossScale: " << m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif