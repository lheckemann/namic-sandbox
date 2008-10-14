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

#include "itkVotingHoleFillingFloodingImageFilter.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
VotingHoleFillingFloodingImageFilter<TInputImage, TOutputImage>
::VotingHoleFillingFloodingImageFilter()
{
  this->m_SeedList.clear();
}

/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
VotingHoleFillingFloodingImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <class TInputImage, class TOutputImage>
void 
VotingHoleFillingFloodingImageFilter<TInputImage,TOutputImage>
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

  while( !it.IsAtEnd())
    {
    it.Set(m_ForegroundValue);
    ++it;
    progress.CompletedPixel();  // potential exception thrown here
    }
}


} // end namespace itk

#endif
