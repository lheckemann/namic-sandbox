/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFrontPropagationLabelImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009-07-08 02:32:35 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFrontPropagationLabelImageFilter_txx
#define __itkFrontPropagationLabelImageFilter_txx

#include "itkFrontPropagationLabelImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionExclusionIteratorWithIndex.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TFeatureImage, class TOutputImage>
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::FrontPropagationLabelImageFilter()
{
  this->m_MaximumNumberOfIterations = 10;
  this->m_CurrentIterationNumber = 0;

  this->m_NumberOfPixelsChangedInLastIteration = 0;
  this->m_TotalNumberOfPixelsChanged = 0;

  this->m_SeedArray1 = NULL;
  this->m_SeedArray2 = NULL;

  this->m_OutputImage = NULL;

  this->m_CurrentOperationMode = DescendingFront;

  this->SetNumberOfRequiredInputs(2);

  this->SetBackgroundValue( 0 );
}

/**
 * Destructor
 */
template <class TInputImage, class TFeatureImage, class TOutputImage>
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::~FrontPropagationLabelImageFilter()
{
  SeedArrayMapIterator sitr1 = this->m_SeedArrayMap1.begin();

  while( sitr1 != this->m_SeedArrayMap1.end() )
    {
    delete sitr1->second;
    ++sitr1;
    }

  SeedArrayMapIterator sitr2 = this->m_SeedArrayMap2.begin();

  while( sitr2 != this->m_SeedArrayMap2.end() )
    {
    delete sitr2->second;
    ++sitr2;
    }
}


/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TFeatureImage, class TOutputImage>
void
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}

template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::SetFeatureImage( const TFeatureImage * imageToSegment )
{
  this->ProcessObject::SetNthInput( 1, const_cast< TFeatureImage * >( imageToSegment ) );
}

template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputImageWorkingMemory();

  this->InitializeNeighborhood();

  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();

  this->SetOperationModeToDescendingFront();
  this->IterateFrontPropagations();

  std::cout << "Number of iterations used = ";
  std::cout << this->GetCurrentIterationNumber() << std::endl;
  std::cout << "Number of pixels changed = ";
  std::cout << this->GetTotalNumberOfPixelsChanged() << std::endl;

  this->SetOperationModeToFillingHoles();
  this->IterateFrontPropagations();

  std::cout << "Number of iterations used = ";
  std::cout << this->GetCurrentIterationNumber() << std::endl;
  std::cout << "Number of pixels changed = ";
  std::cout << this->GetTotalNumberOfPixelsChanged() << std::endl;


  this->SetOperationModeToRemovingIsolated();
  this->IterateFrontPropagations();

  std::cout << "Number of iterations used = ";
  std::cout << this->GetCurrentIterationNumber() << std::endl;
  std::cout << "Number of pixels changed = ";
  std::cout << this->GetTotalNumberOfPixelsChanged() << std::endl;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::IterateFrontPropagations()
{
  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  // Progress reporting
  ProgressReporter progress(this, 0, m_MaximumNumberOfIterations);
  
  while( this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations ) 
    {
    this->VisitAllSeedsAndTransitionTheirState();
    this->m_CurrentIterationNumber++;

    progress.CompletedPixel();   // not really a pixel but an iteration

    this->InvokeEvent( IterationEvent() );
    
    if( this->m_NumberOfPixelsChangedInLastIteration ==  0 )
      {
      break;
      }
    }
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::AllocateOutputImageWorkingMemory()
{
  this->m_OutputImage  = this->GetOutput();
  OutputImageRegionType region =  this->m_OutputImage->GetRequestedRegion();

  // Allocate memory for the output image itself.
  this->m_OutputImage->SetBufferedRegion( region );
  this->m_OutputImage->Allocate();
  this->m_OutputImage->FillBuffer( 0 );

  this->m_SeedsMask = SeedMaskImageType::New();
  this->m_SeedsMask->SetRegions( region );
  this->m_SeedsMask->Allocate();
  this->m_SeedsMask->FillBuffer( 0 );

  this->m_FeatureImage = dynamic_cast< const FeatureImageType * >( itk::ProcessObject::GetInput(1) );
}

 
template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::InitializeNeighborhood()
{
  this->m_Neighborhood.SetRadius( this->GetRadius() );

  this->ComputeArrayOfNeighborhoodBufferOffsets();
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{
  const InputImageType * inputImage = this->GetInput();

  OutputImageRegionType region =  inputImage->GetRequestedRegion();

  ConstNeighborhoodIterator< InputImageType >   bit;
  ImageRegionIterator< OutputImageType >        itr;
  ImageRegionIterator< SeedMaskImageType >      mtr;
  
  const InputSizeType & radius = this->GetRadius();

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(inputImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  // Process only the internal face
  fit = faceList.begin();
  
  this->m_InternalRegion = *fit;

  // Mark all the pixels in the boundary of the seed image as visited
  typedef itk::ImageRegionExclusionIteratorWithIndex< 
    SeedMaskImageType > ExclusionIteratorType;
  ExclusionIteratorType exIt( this->m_SeedsMask, region );
  exIt.SetExclusionRegion( this->m_InternalRegion );
  for (exIt.GoToBegin(); !exIt.IsAtEnd(); ++exIt)
    {
    exIt.Set( 255 );
    }

  bit = ConstNeighborhoodIterator<InputImageType>( radius, inputImage, this->m_InternalRegion );
  itr  = ImageRegionIterator<OutputImageType>(    this->m_OutputImage, this->m_InternalRegion );
  mtr  = ImageRegionIterator<SeedMaskImageType>(  this->m_SeedsMask,   this->m_InternalRegion );

  bit.GoToBegin();
  itr.GoToBegin();
  mtr.GoToBegin();
  
  unsigned int neighborhoodSize = bit.Size();

  const InputImagePixelType backgroundValue = this->GetBackgroundValue();
  
  this->m_SeedArrayMap1.clear();
  this->m_SeedArrayMap2.clear();
  this->m_SeedsNewValuesMap.clear();

  while ( ! bit.IsAtEnd() )
    {
    if( bit.GetCenterPixel() != backgroundValue )
      {
      itr.Set( bit.GetCenterPixel() );
      mtr.Set( 255 );
      }
    else
      {
      itr.Set( backgroundValue );
      mtr.Set( 0 );
      
      // Search for foreground pixels in the neighborhood
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
        {
        InputImagePixelType value = bit.GetPixel(i);

        if( value != backgroundValue )
          {
          // If we don't have a container for this label, then create one.
          if( this->m_SeedArrayMap1.find(value) == this->m_SeedArrayMap1.end() )
            {
            this->m_SeedArrayMap1[value] = new SeedArrayType;
            this->m_SeedArrayMap2[value] = new SeedArrayType;
            }

          this->m_SeedArrayMap1[value]->push_back( bit.GetIndex() );
          break;
          }
        }
      }   
    ++bit;
    ++itr;
    ++mtr;
    }


  SeedArrayMapIterator sitr = this->m_SeedArrayMap1.begin();

  while( sitr != this->m_SeedArrayMap1.end() )
    {
    const LabelType label = sitr->first;
    this->m_SeedsNewValuesMap[label] = SeedNewValuesArrayType();
    this->m_SeedsNewValuesMap[label].reserve( this->m_SeedArrayMap1[label]->size() ); 
    ++sitr;
    }
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::VisitAllSeedsAndTransitionTheirState()
{

   SeedArrayMapIterator sitr1 = this->m_SeedArrayMap1.begin();
   SeedArrayMapIterator sitr2 = this->m_SeedArrayMap2.begin();

   while( sitr1 != this->m_SeedArrayMap1.end() )
    {
    this->m_SeedArray1 = sitr1->second;
    this->m_SeedArray2 = sitr2->second;

    const LabelType label = sitr1->first;

    this->VisitAllSeedsAndTransitionTheirState( label );

    sitr1->second = this->m_SeedArray1;
    sitr2->second = this->m_SeedArray2;

    ++sitr1;
    ++sitr2;
    }
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::VisitAllSeedsAndTransitionTheirState(LabelType label)
{
  this->SetForegroundValue( label );

  typedef typename SeedArrayType::const_iterator   SeedIterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  // Clear the array of new values
  this->m_SeedsNewValuesMap[label].clear();

  while( seedItr != this->m_SeedArray1->end() )
    {
    this->m_CurrentPixelIndex = *seedItr;

    if( this->TestForAcceptingCurrentPixel() )
      {
      this->m_SeedsNewValuesMap[label].push_back( label );
      this->PutCurrentPixelNeighborsIntoSeedArray();
      this->m_NumberOfPixelsChangedInLastIteration++;
      }
    else
      {
      this->m_SeedsNewValuesMap[label].push_back( this->GetBackgroundValue() );
      // Keep the seed to try again in the next iteration.
      this->m_SeedArray2->push_back( this->GetCurrentPixelIndex() );
      }

    ++seedItr;
    }

  this->PasteNewSeedValuesToOutputImage(label);
   
  this->m_TotalNumberOfPixelsChanged += this->m_NumberOfPixelsChangedInLastIteration;

  // Now that the values have been copied to the output image, we can empty the
  // array in preparation for the next iteration
  this->m_SeedsNewValuesMap[label].clear();

  this->SwapSeedArrays();
  this->ClearSecondSeedArray();
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::PasteNewSeedValuesToOutputImage( LabelType label )
{
  //
  //  Paste new values into the output image
  //
  typedef typename SeedArrayType::const_iterator   SeedIterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  typedef typename SeedNewValuesArrayType::const_iterator   SeedsNewValuesIterator;

  SeedsNewValuesIterator newValueItr = this->m_SeedsNewValuesMap[label].begin();

  while (seedItr != this->m_SeedArray1->end() )
    {
    this->m_OutputImage->SetPixel( *seedItr, *newValueItr );
    ++seedItr;
    ++newValueItr;
    }
}

template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::SwapSeedArrays()
{
  SeedArrayType * temporary = this->m_SeedArray1;
  this->m_SeedArray1 = this->m_SeedArray2;
  this->m_SeedArray2 = temporary;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::ClearSecondSeedArray()
{
  delete this->m_SeedArray2;
  this->m_SeedArray2 = new SeedArrayType;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::SetOperationModeToDescendingFront()
{
  this->m_CurrentOperationMode = DescendingFront;
  this->SetBirthThreshold( 1 );

  InputSizeType  ballManhattanRadius;
  ballManhattanRadius.Fill( 1 );
  this->SetRadius( ballManhattanRadius );
  this->InitializeNeighborhood();
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::SetOperationModeToFillingHoles()
{
  this->m_CurrentOperationMode = FillingHoles;

  InputSizeType  ballManhattanRadius;
  ballManhattanRadius.Fill( 2 );
  this->SetRadius( ballManhattanRadius );

  this->InitializeNeighborhood();

  const unsigned int neighborhoodSize = this->GetNeighborhoodSize();

  // Take the number of neighbors, discount the central pixel, and take half of them.
  unsigned int threshold = static_cast<unsigned int>( (neighborhoodSize - 1 ) / 2.0 ); 

  // add the majority threshold.
  threshold += this->GetMajorityThreshold();

  // Set that number as the Birth Threshold
  this->SetBirthThreshold( threshold );

}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::SetOperationModeToRemovingIsolated()
{
  InputSizeType  ballManhattanRadius;
  ballManhattanRadius.Fill( 1 );
  this->SetRadius( ballManhattanRadius );

  this->InitializeNeighborhood();

  // Set the Birth Threshold to an unreachable value since we don't
  // want to turn any pixels on in this phase.
  const unsigned int neighborhoodSize = this->GetNeighborhoodSize();

  // Take the number of neighbors, discount the central pixel, and take half of them.
  unsigned int threshold = static_cast<unsigned int>( (neighborhoodSize - 1 ) / 2.0 ); 

  this->SetBirthThreshold( neighborhoodSize + 1 );

  this->SetSurvivalThreshold( threshold );
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
bool 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::TestForAcceptingCurrentPixel() const
{
  switch( this->m_CurrentOperationMode )
    {
    case DescendingFront:
      {
      return this->TestForDescendingFront();
      break;
      }
    case FillingHoles:
      {
      return this->TestQuorumForBirth();
      break;
      }
    case RemovingIsolated:
      {
      return this->TestQuorumForSurvival();
      break;
      }
    }
  return false;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
bool 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::TestQuorumForBirth() const
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset( this->GetCurrentPixelIndex() );

  const OutputImagePixelType  * maskBuffer = this->m_OutputImage->GetBufferPointer();

  const OutputImagePixelType  * currentMaskPixelPointer = maskBuffer + offset;

  const OutputImagePixelType foregroundValue = this->GetForegroundValue();
  const OutputImagePixelType backgroundValue = this->GetBackgroundValue();

  if( *currentMaskPixelPointer != backgroundValue )
    {
    return false;
    }

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  //
  // From that buffer position, visit all other neighbor pixels 
  // and check if they are set to the foreground value.
  //
  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  while( neighborItr != this->m_NeighborBufferOffset.end() )
    {
    const OutputImagePixelType  * neighborMaskPixelPointer    = currentMaskPixelPointer    + *neighborItr;

    if( *neighborMaskPixelPointer == foregroundValue )
      {
      numberOfNeighborsAtForegroundValue++;
      }

    ++neighborItr;
    }

  bool quorum = ( numberOfNeighborsAtForegroundValue > this->GetBirthThreshold() );

  return quorum;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
bool 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::TestQuorumForSurvival() const
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset( this->GetCurrentPixelIndex() );

  const OutputImagePixelType  * maskBuffer = this->m_OutputImage->GetBufferPointer();

  const OutputImagePixelType  * currentMaskPixelPointer = maskBuffer + offset;

  const OutputImagePixelType foregroundValue = this->GetForegroundValue();
  const OutputImagePixelType backgroundValue = this->GetBackgroundValue();

  if( *currentMaskPixelPointer == backgroundValue )
    {
    return false;
    }

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  //
  // From that buffer position, visit all other neighbor pixels 
  // and check if they are set to the foreground value.
  //
  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  while( neighborItr != this->m_NeighborBufferOffset.end() )
    {
    const OutputImagePixelType  * neighborMaskPixelPointer    = currentMaskPixelPointer    + *neighborItr;

    if( *neighborMaskPixelPointer == foregroundValue )
      {
      numberOfNeighborsAtForegroundValue++;
      }

    ++neighborItr;
    }

  bool quorum = ( numberOfNeighborsAtForegroundValue > 2 );

  return quorum;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
bool 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::TestForDescendingFront() const
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset( this->GetCurrentPixelIndex() );

  const InputFeaturePixelType featureValue = this->m_FeatureImage->GetPixel( this->GetCurrentPixelIndex() );

  if( featureValue < this->m_LowerThreshold )
    {
    return false;
    }

  const OutputImagePixelType  * maskBuffer = this->m_OutputImage->GetBufferPointer();
  const InputFeaturePixelType * featureBuffer = this->m_FeatureImage->GetBufferPointer();

  const OutputImagePixelType  * currentMaskPixelPointer = maskBuffer + offset;
  const InputFeaturePixelType * currentFeaturePixelPointer = featureBuffer + offset;

  const OutputImagePixelType foregroundValue = this->GetForegroundValue();
  const OutputImagePixelType backgroundValue = this->GetBackgroundValue();

  if( *currentMaskPixelPointer != backgroundValue )
    {
    return false;
    }

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  //
  // From that buffer position, visit all other neighbor pixels 
  // and check if they are set to the foreground value.
  //
  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  while( neighborItr != this->m_NeighborBufferOffset.end() )
    {
    const OutputImagePixelType  * neighborMaskPixelPointer    = currentMaskPixelPointer    + *neighborItr;
    const InputFeaturePixelType * neighborFeaturePixelPointer = currentFeaturePixelPointer + *neighborItr;

    if( *neighborMaskPixelPointer == foregroundValue  &&
        *neighborFeaturePixelPointer > featureValue )  //  Add an option for (>=) here
      {
      numberOfNeighborsAtForegroundValue++;
      }

    ++neighborItr;
    }

  bool quorum = ( numberOfNeighborsAtForegroundValue > this->GetBirthThreshold() );

  return quorum;
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::PutCurrentPixelNeighborsIntoSeedArray()
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset( this->GetCurrentPixelIndex() );

  const InputImagePixelType * buffer = this->m_OutputImage->GetBufferPointer();

  const InputImagePixelType * currentPixelPointer = buffer + offset;


  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  //
  // Visit the offset of each neighbor in Index as well as buffer space
  // and if they are backgroundValue then insert them as new seeds
  //
  typedef typename NeighborhoodType::OffsetType    NeighborOffsetType;

  typedef typename NeighborOffsetArrayType::const_iterator   NeigborOffsetIterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  const InputImagePixelType backgroundValue = this->GetBackgroundValue();

  for( unsigned int i = 0; i < neighborhoodSize; ++i, ++neighborItr )
    {
    const InputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;

    if( *neighborPixelPointer == backgroundValue )
      {
      NeighborOffsetType neighborOffset = this->m_Neighborhood.GetOffset(i);
      IndexType neighborIndex = this->GetCurrentPixelIndex() + neighborOffset;

      if( this->m_InternalRegion.IsInside( neighborIndex ) )
        {
        if( this->m_SeedsMask->GetPixel( neighborIndex ) == 0 )
          {
          this->m_SeedArray2->push_back( neighborIndex );
          this->m_SeedsMask->SetPixel( neighborIndex, 255 );
          }
        }
      }
    }

}


template <class TInputImage, class TFeatureImage, class TOutputImage>
unsigned int
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::GetNeighborhoodSize() const
{
  return this->m_Neighborhood.Size();
}


template <class TInputImage, class TFeatureImage, class TOutputImage>
void 
FrontPropagationLabelImageFilter<TInputImage, TFeatureImage, TOutputImage>
::ComputeArrayOfNeighborhoodBufferOffsets()
{
  //
  // Copy the offsets from the Input image.
  // We assume that they are the same for the output image.
  //
  const size_t sizeOfOffsetTableInBytes = (InputImageDimension+1)*sizeof(unsigned long);

  memcpy( this->m_OffsetTable, this->m_OutputImage->GetOffsetTable(), sizeOfOffsetTableInBytes );


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

} // end namespace itk

#endif
