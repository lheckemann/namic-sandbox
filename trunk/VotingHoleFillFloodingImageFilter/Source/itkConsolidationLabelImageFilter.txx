/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkConsolidationLabelImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009-07-08 02:32:35 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkConsolidationLabelImageFilter_txx
#define __itkConsolidationLabelImageFilter_txx

#include "itkConsolidationLabelImageFilter.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::ConsolidationLabelImageFilter()
{
}

/**
 * Destructor
 */
template <class TInputImage, class TOutputImage>
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::~ConsolidationLabelImageFilter()
{
}


/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <class TInputImage, class TOutputImage>
void 
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputImageWorkingMemory();
  this->InitializeNeighborhood();
  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds2();
  this->ComputeLabelAffinities();
}


template <class TInputImage, class TOutputImage>
void 
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::ComputeLabelAffinities()
{
 const SeedArrayMapType & seedArrayMap = this->GetSeedArrayMap1();
 SeedArrayMapConstIterator sitr1 = seedArrayMap.begin();

 while( sitr1 != seedArrayMap.end() )
  {
  this->ComputeLabelAffinities( sitr1->first, sitr1->second );

  const LabelType label = sitr1->first;
  typedef typename NumericTraits<InputImagePixelType>::PrintType PrintType;
  std::cout << "Histogram for Label= " << static_cast<PrintType>( label ) << std::endl;
  const NumberOfPixelsArrayMapType & currentLabelHistogram = this->m_NeigborLabelsHistogram[label];

  typename NumberOfPixelsArrayMapType::const_iterator currentLabelHistogramItr = currentLabelHistogram.begin();
  while( currentLabelHistogramItr != currentLabelHistogram.end() )
    {
    std::cout << "Label = " << currentLabelHistogramItr->first << " = " << currentLabelHistogramItr->second << std::endl;
    ++currentLabelHistogramItr;
    }
  std::cout << std::endl;

  ++sitr1;
  }
}


template <class TInputImage, class TOutputImage>
void 
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::ComputeLabelAffinities( InputImagePixelType label, const SeedArrayType * seedArray )
{
  typedef typename NumericTraits<InputImagePixelType>::PrintType PrintType;
  std::cout << "ComputeLabelAffinities( " << static_cast<PrintType>( label ) << " ) " << std::endl;

  //
  //  If there is no histogram for this label yet, create one.
  //
  if( this->m_NeigborLabelsHistogram.find(label) == this->m_NeigborLabelsHistogram.end() )
    {
    this->m_NeigborLabelsHistogram[label] = NumberOfPixelsArrayMapType();
    }

  NumberOfPixelsArrayMapType & currentLabelHistogram = this->m_NeigborLabelsHistogram[label];

  const OutputImageType * outputImage = this->GetOutputImage();
  const OutputImagePixelType backgroundValue = this->GetBackgroundValue();

  typedef typename SeedArrayType::const_iterator   SeedIterator;

  SeedIterator seedItr = seedArray->begin();

  while( seedItr != seedArray->end() )
    {
    const OutputImagePixelType pixelValue = outputImage->GetPixel( *seedItr );

    if( pixelValue != backgroundValue )
      {
      //
      // If the current label doesn't exist, create one
      //
      if( currentLabelHistogram.find(pixelValue) == currentLabelHistogram.end() )
        {
        currentLabelHistogram[pixelValue] = NumericTraits<SizeValueType>::Zero;
        }

      currentLabelHistogram[pixelValue]++;
      }

    ++seedItr;
    }
}


} // end namespace itk

#endif
