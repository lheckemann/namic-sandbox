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
  this->m_MaximumNumberOfIterations = 10;
  this->m_CurrentIterationNumber = 0;

  this->m_NumberOfPixelsChangedInLastIteration = 0;
  this->m_TotalNumberOfPixelsChanged = 0;

  this->m_SeedArray1 = new SeedArrayType;
  this->m_SeedArray1 = new SeedArrayType;
}

/**
 * Destructor
 */
template <class TInputImage, class TOutputImage>
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>
::~VotingBinaryHoleFillFloodingImageFilter()
{
  delete this->m_SeedArray1;
  delete this->m_SeedArray2;
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
  
  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();

  while ( (this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations) 
    && ( this->m_NumberOfPixelsChangedInLastIteration > 0 ) )
    {
    this->VisitAllSeedsAndTransitionTheirState();
    }

#ifdef SUGAR
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
#endif
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
  
  bit = ConstNeighborhoodIterator<InputImageType>( radius, inputImage, *fit );
  it  = ImageRegionIterator<OutputImageType>(outputImage, *fit);
  bit.OverrideBoundaryCondition(&nbc);
  bit.GoToBegin();
  
  unsigned int neighborhoodSize = bit.Size();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();
  const InputImagePixelType backgroundValue = this->GetBackgroundValue();
  
  SeedArrayType * seedArray = this->m_SeedArray1;

  seedArray->clear();

  while ( ! bit.IsAtEnd() )
    {
    if( bit.GetCenterPixel() == foregroundValue )
      {
      // it.Set( foregroundValue );
      it.Set( backgroundValue ); // FIXME: this is just for testing
      }
    else
      {
      // Search for foreground pixels in the neighborhood
      bool foundNeighbordSetAtForegroundValue = false;
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
        {
        InputImagePixelType value = bit.GetPixel(i);
        if( value == foregroundValue )
          {
          seedArray->push_back( bit.GetIndex() );
          foundNeighbordSetAtForegroundValue = true;
          break;
          }
        }
      if (foundNeighbordSetAtForegroundValue)
        {
        it.Set( foregroundValue );
        }
      else
        {
        it.Set( backgroundValue );
        }
      }   
    ++bit;
    ++it;
    progress.CompletedPixel();
    }
  std::cout << "Seed array size = " << seedArray->size() << std::endl;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::VisitAllSeedsAndTransitionTheirState()
{
  this->SwapSeedArrays();
  this->ClearSecondSeedArray();

  typedef typename SeedArrayType::const_iterator   SeedIterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  this->m_NumberOfPixelsChangedInLastIteration = 0;

  while (seedItr != this->m_SeedArray2->end() )
    {
    this->SetCurrentPixelIndex( *seedItr );

    if (this->TestForQuorumAtCurrentPixel() )
      {
      this->ConvertCurrentPixelAndPutNeighborsIntoSeedArray();
      }

    ++seedItr;
    }

  this->m_TotalNumberOfPixelsChanged += this->m_NumberOfPixelsChangedInLastIteration;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::SwapSeedArrays()
{
  SeedArrayType * temporary = this->m_SeedArray1;
  this->m_SeedArray1 = this->m_SeedArray2;
  this->m_SeedArray2 = temporary;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ClearSecondSeedArray()
{
  delete this->m_SeedArray2;
  this->m_SeedArray2 = new SeedArrayType;
}


template <class TInputImage, class TOutputImage>
bool 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::TestForQuorumAtCurrentPixel() const
{
   return false;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ConvertCurrentPixelAndPutNeighborsIntoSeedArray()
{
  this->m_SeedArray2->push_back( this->GetCurrentPixelIndex() );
  this->m_NumberOfPixelsChangedInLastIteration++;
}


} // end namespace itk

#endif
