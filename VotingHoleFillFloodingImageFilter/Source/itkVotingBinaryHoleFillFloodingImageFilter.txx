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
  this->m_SeedArray2 = new SeedArrayType;

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
  this->AllocateOutputImageWorkingMemory();
 
  // Initialize the neighborhood.
  this->m_Neighborhood.SetRadius( this->GetRadius() );

  this->ComputeBirthThreshold();
  this->ComputeArrayOfNeighborhoodBufferOffsets();

  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();

  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  while( this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations ) 
    {
    std::cout << "Iteration " << this->m_CurrentIterationNumber << std::endl;
    this->VisitAllSeedsAndTransitionTheirState();
    this->m_CurrentIterationNumber++;
    if( this->m_NumberOfPixelsChangedInLastIteration ==  0 )
      {
      break;
      }
    }

  this->CopyImageWorkingMemoryToOutputImage();
  this->DeallocateOutputImageWorkingMemory();
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::AllocateOutputImageWorkingMemory()
{
  OutputImageType * outputImage  = this->GetOutput();
  OutputImageRegionType region =  outputImage->GetRequestedRegion();

  // Allocate memory for the output image itself.
  outputImage->SetBufferedRegion( region );
  outputImage->Allocate();

  // Allocate the two output auxiliary images
  this->m_OutputImage1 = OutputImageType::New();
  this->m_OutputImage1->SetBufferedRegion( region );
  this->m_OutputImage1->Allocate();

  this->m_OutputImage2 = OutputImageType::New();
  this->m_OutputImage2->SetBufferedRegion( region );
  this->m_OutputImage2->Allocate();
}

 
template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::DeallocateOutputImageWorkingMemory()
{
  this->m_OutputImage1 = NULL;
  this->m_OutputImage2 = NULL;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{
  const InputImageType * inputImage = this->GetInput();

  OutputImageRegionType region =  inputImage->GetRequestedRegion();

  ZeroFluxNeumannBoundaryCondition< InputImageType > nbc;

  ConstNeighborhoodIterator< InputImageType >   bit;
  ImageRegionIterator< OutputImageType >        it;
  
  const InputSizeType & radius = this->GetRadius();

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(inputImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  ProgressReporter progress(this, 0, region.GetNumberOfPixels());
  
  // Process only the internal face
  fit = faceList.begin();
  
  bit = ConstNeighborhoodIterator<InputImageType>( radius, inputImage, *fit );
  it  = ImageRegionIterator<OutputImageType>(this->m_OutputImage1, *fit);
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
      it.Set( foregroundValue );
      }
    else
      {
      it.Set( backgroundValue );
      
      // Search for foreground pixels in the neighborhood
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
        {
        InputImagePixelType value = bit.GetPixel(i);
        if( value == foregroundValue )
          {
          seedArray->push_back( bit.GetIndex() );
          break;
          }
        }
      }   
    ++bit;
    ++it;
    progress.CompletedPixel();
    }
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::VisitAllSeedsAndTransitionTheirState()
{
  typedef typename SeedArrayType::const_iterator   SeedIterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  this->m_NumberOfPixelsChangedInLastIteration = 0;

  while (seedItr != this->m_SeedArray1->end() )
    {
    this->SetCurrentPixelIndex( *seedItr );

    if( this->TestForQuorumAtCurrentPixel() )
      {
      this->ConvertCurrentPixelAndPutNeighborsIntoSeedArray();
      }

    ++seedItr;
    }

  this->m_TotalNumberOfPixelsChanged += this->m_NumberOfPixelsChangedInLastIteration;

  this->SwapSeedArrays();
  this->SwapOutputImageWorkingMemory();
  this->ClearSecondSeedArray();
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
::SwapOutputImageWorkingMemory()
{
  OutputImagePointer temporary = this->m_OutputImage1;
  this->m_OutputImage1 = this->m_OutputImage2;
  this->m_OutputImage2 = temporary;
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
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage1->ComputeOffset( this->GetCurrentPixelIndex() );

  const InputImagePixelType * buffer = this->m_OutputImage1->GetBufferPointer();

  const InputImagePixelType * currentPixelPointer = buffer + offset;

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  //
  // From that buffer position, visit all other neighbor pixels 
  // and check if they are set to the foreground value.
  //
  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();

  while( neighborItr != this->m_NeighborBufferOffset.end() )
    {
    const InputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;

    if( *neighborPixelPointer == foregroundValue )
      {
      numberOfNeighborsAtForegroundValue++;
      }

    ++neighborItr;
    }

//  std::cout << "Testing Quorum at pixel " << this->GetCurrentPixelIndex() << " ";
//  std::cout << "found " << numberOfNeighborsAtForegroundValue << " foregrounds" << std::endl;
  bool quorum = (numberOfNeighborsAtForegroundValue > this->GetBirthThreshold() );

  return quorum;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ConvertCurrentPixelAndPutNeighborsIntoSeedArray()
{
  this->m_OutputImage1->SetPixel( this->GetCurrentPixelIndex(), this->GetForegroundValue() );
  this->m_SeedArray2->push_back( this->GetCurrentPixelIndex() );
  this->m_NumberOfPixelsChangedInLastIteration++;
}


template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ComputeArrayOfNeighborhoodBufferOffsets()
{
  //
  // Copy the offsets from the Input image.
  // We assume that they are the same for the output image.
  //
  const size_t sizeOfOffsetTableInBytes = (InputImageDimension+1)*sizeof(unsigned long);

  memcpy( this->m_OffsetTable, this->m_OutputImage1->GetOffsetTable(), sizeOfOffsetTableInBytes );


  //
  // Allocate the proper amount of buffer offsets.
  //
  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  this->m_NeighborBufferOffset.resize( neighborhoodSize );


  //
  // Visit the offset of each neighbor in Index space and convert it to linear
  // buffer offsets that can be used for pixel access
  //
  typedef typename NeighborhoodType::OffsetType    NeighborOffsetType;

  for( unsigned int i = 0; i < neighborhoodSize; i++ )
    {
    NeighborOffsetType offset = this->m_Neighborhood.GetOffset(i);

    signed int bufferOffset = 0; // must be a signed number

    for( unsigned int d = 0; d < InputImageDimension; d++ )
      {
      bufferOffset += offset[d] * this->m_OffsetTable[d];
      }
    this->m_NeighborBufferOffset[i] = bufferOffset;
    }
}

template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::ComputeBirthThreshold()
{
  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  // Take the number of neighbors, discount the central pixel, and take half of them.
  unsigned int threshold = static_cast<unsigned int>( (neighborhoodSize - 1 ) / 2.0 ); 

  // add the majority threshold.
  threshold += this->GetMajorityThreshold();

  // Set that number as the Birth Threshold
  this->SetBirthThreshold( threshold );
}

 
template <class TInputImage, class TOutputImage>
void 
VotingBinaryHoleFillFloodingImageFilter<TInputImage,TOutputImage>
::CopyImageWorkingMemoryToOutputImage()
{
  OutputImageType * outputImage = this->GetOutput();

  typedef itk::ImageRegionIterator< OutputImageType >      DestinationIterator;
  typedef itk::ImageRegionConstIterator< OutputImageType > SourceIterator;

  const OutputImageRegionType requestedRegion = outputImage->GetRequestedRegion();
 
  DestinationIterator dst( outputImage, requestedRegion );
  SourceIterator      src( this->m_OutputImage1, requestedRegion );

  dst.GoToBegin();
  src.GoToBegin();

  while( !src.IsAtEnd() )
    {
    dst.Set( src.Get() );
    ++src;
    ++dst;
    }
}


} // end namespace itk

#endif
