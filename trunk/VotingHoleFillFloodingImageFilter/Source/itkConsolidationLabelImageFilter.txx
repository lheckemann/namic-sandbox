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
  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();
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
  const InputImagePixelType label = sitr1->first;
  this->ComputeLabelAffinities( label );
  ++sitr1;
  }
}


template <class TInputImage, class TOutputImage>
void 
ConsolidationLabelImageFilter<TInputImage, TOutputImage>
::ComputeLabelAffinities( InputImagePixelType label )
{
  std::cout << "ComputeLabelAffinities( " << label << " ) " << std::endl;
}

} // end namespace itk

#endif
