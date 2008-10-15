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

  this->m_InputImage = NULL;
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
  this->m_InputImage   = this->GetInput();
  this->m_OutputImage  = this->GetOutput();

  // Zero the output
  OutputImageRegionType region =  this->m_OutputImage->GetRequestedRegion();
  this->m_OutputImage->SetBufferedRegion( region );
  this->m_OutputImage->Allocate();
  this->m_OutputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
  
  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  this->ComputeArrayOfNeighborhoodOffsets();
  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();

  while ( (this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations) 
    && ( this->m_NumberOfPixelsChangedInLastIteration > 0 ) )
    {
    this->VisitAllSeedsAndTransitionTheirState();
    }
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{
  OutputImageRegionType region =  this->m_OutputImage->GetRequestedRegion();

  ZeroFluxNeumannBoundaryCondition< InputImageType > nbc;

  ConstNeighborhoodIterator< InputImageType >   bit;
  ImageRegionIterator< OutputImageType >        it;
  
  const InputSizeType & radius = this->GetRadius();

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(this->m_InputImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  ProgressReporter progress(this, 0, region.GetNumberOfPixels());
  
  // Process only the internal face
  fit = faceList.begin();
  
  bit = ConstNeighborhoodIterator<InputImageType>( radius, this->m_InputImage, *fit );
  it  = ImageRegionIterator<OutputImageType>(this->m_OutputImage, *fit);
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

    if( this->TestForQuorumAtCurrentPixel() )
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
  const OffsetValueType offset = this->m_InputImage->ComputeOffset( this->m_CurrentPixelIndex );

  const InputImagePixelType * buffer = this->m_InputImage->GetBufferPointer();

  const InputImagePixelType * currentPixelPointer = buffer + offset;

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborOffset.begin();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();

  while( neighborItr != this->m_NeighborOffset.end() )
    {
    const InputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;
    if( *neighborPixelPointer == foregroundValue )
      {
      numberOfNeighborsAtForegroundValue++;
      }
    ++neighborItr;
    }

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


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ComputeArrayOfNeighborhoodOffsets()
{
  //
  // Copy the offsets from the Input image.
  // We assume that they are the same for the output image.
  //
  const size_t sizeOfOffsetTableInBytes = (InputImageDimension+1)*sizeof(unsigned long);

  memcpy( this->m_OffsetTable, this->m_InputImage->GetOffsetTable(), sizeOfOffsetTableInBytes );


  //
  // Instantiate a helper Neighborhood object
  //
  typedef itk::Neighborhood< InputImagePixelType, InputImageDimension >  NeighborhoodType;

  NeighborhoodType neighborhood;

  neighborhood.SetRadius( this->GetRadius() );

  this->m_NeighborOffset.resize( neighborhood.Size() );


  //
  // Visit the offset of each neighbor in Index space and convert it to linear
  // buffer offsets that can be used for pixel access
  //
  typedef typename NeighborhoodType::OffsetType    NeighborOffsetType;

  std::cout << "Neighborhood helper size = " << neighborhood.Size() << std::endl;

  for( unsigned int i = 0; i < neighborhood.Size(); i++ )
    {
    NeighborOffsetType offset = neighborhood.GetOffset(i);

    std::cout << "Offset[" << i << "] = " << offset << " ";

    signed int bufferOffset = 0; // must be a signed number

    for( unsigned int d = 0; d < InputImageDimension; d++ )
      {
      bufferOffset += offset[d] * this->m_OffsetTable[d];
      }
    this->m_NeighborOffset[i] = bufferOffset;
    std::cout << bufferOffset << std::endl;
    }
}


} // end namespace itk

#endif
