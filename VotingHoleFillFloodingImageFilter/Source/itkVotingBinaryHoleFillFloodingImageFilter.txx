/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVotingHoleFillingFloodingImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006-06-28 17:06:15 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVotingHoleFillingFloodingImageFilter_txx_
#define __itkVotingHoleFillingFloodingImageFilter_txx_

#include "itkVotingBinaryHoleFillFloodingImageFilter.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkVotingBinaryImageFunction.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>
::VotingBinaryHoleFillFloodingImageFilter()
{
  this->m_SeedList.clear();
}

/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::GenerateData()
{
  InputImageConstPointer  inputImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  // Zero the output
  OutputImageRegionType region =  outputImage->GetRequestedRegion();
  outputImage->SetBufferedRegion( region );
  outputImage->Allocate();
  outputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
  
  typedef VotingBinaryImageFunction<InputImageType, double> FunctionType;
  typedef FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;

  typename FunctionType::Pointer function = FunctionType::New();
  function->SetInputImage ( inputImage );

  ProgressReporter progress(this, 0, region.GetNumberOfPixels());

  IteratorType it ( outputImage, function, m_SeedList );
  it.GoToBegin();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();

  while( !it.IsAtEnd())
    {
    it.Set( foregroundValue );
    ++it;
    progress.CompletedPixel();  // potential exception thrown here
    }
}

template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::AddSeed( const IndexType & seedIndex )
{
}

template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{
  InputImageConstPointer  inputImage  = this->GetInput();
  OutputImagePointer      outputImage = this->GetOutput();

  OutputImageRegionType region =  outputImage->GetRequestedRegion();

  ZeroFluxNeumannBoundaryCondition< InputImageType > nbc;

  ConstNeighborhoodIterator< InputImageType >   bit;
  ImageRegionIterator< OutputImageType >        it;
  
  InputSizeType radius = this->GetRadius();

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(inputImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  ProgressReporter progress(this, 0, region.GetNumberOfPixels());
  
  // Process only the internal face
  fit = faceList.begin();
  
  bit = ConstNeighborhoodIterator<InputImageType>(radius, inputImage, *fit);
  it  = ImageRegionIterator<OutputImageType>(outputImage, *fit);
  bit.OverrideBoundaryCondition(&nbc);
  bit.GoToBegin();
  
  unsigned int neighborhoodSize = bit.Size();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();
  
  while ( ! bit.IsAtEnd() )
    {
    // count the pixels in the neighborhood
    for (unsigned int i = 0; i < neighborhoodSize; ++i)
      {
      InputImagePixelType value = bit.GetPixel(i);
      if( value == foregroundValue )
        {
        this->AddSeed( bit.GetIndex() );
        }
      }
     
    ++bit;
    ++it;
    progress.CompletedPixel();
    }
}


} // end namespace itk

#endif
