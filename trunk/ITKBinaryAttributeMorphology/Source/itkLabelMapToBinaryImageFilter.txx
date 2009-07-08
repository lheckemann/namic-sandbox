/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelMapToBinaryImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelMapToBinaryImageFilter_txx
#define __itkLabelMapToBinaryImageFilter_txx

#include "itkLabelMapToBinaryImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

namespace itk {

template <class TInputImage, class TOutputImage>
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::LabelMapToBinaryImageFilter()
{
  this->m_OutputBackgroundValue = NumericTraits<OutputImagePixelType>::NonpositiveMin();
  this->m_OutputForegroundValue = NumericTraits<OutputImagePixelType>::max();
}

template <class TInputImage, class TOutputImage>
void 
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // We need all the input.
  InputImagePointer input = const_cast<InputImageType *>(this->GetInput());

  if( input )
    { 
    input->SetRequestedRegion( input->GetLargestPossibleRegion() );
    }
}


template <class TInputImage, class TOutputImage>
void 
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}


template<class TInputImage, class TOutputImage>
void
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData()
{
  unsigned long numberOfThreads = this->GetNumberOfThreads();
  if( itk::MultiThreader::GetGlobalMaximumNumberOfThreads() != 0 )
    {
    numberOfThreads = std::min( this->GetNumberOfThreads(), itk::MultiThreader::GetGlobalMaximumNumberOfThreads() );
    }

  // number of threads can be constrained by the region size, so call the
  // SplitRequestedRegion to get the real number of threads which will be used
  typename TOutputImage::RegionType splitRegion;  // dummy region - just to call the following method

  numberOfThreads = this->SplitRequestedRegion(0, numberOfThreads, splitRegion);

  m_Barrier = Barrier::New();

  m_Barrier->Initialize( numberOfThreads );

  this->Superclass::BeforeThreadedGenerateData();
}


template<class TInputImage, class TOutputImage>
void
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData( const OutputImageRegionType& outputRegionForThread, int threadId )
{
  OutputImageType * output = this->GetOutput();

  // fill the output with background value - they will be overridden with the
  // foreground value later, if there is some objects
  if( this->GetNumberOfInputs() == 2 )
    {
    // fill the background with the background values from the background image
    ImageRegionConstIterator< OutputImageType > bgIt( this->GetBackgroundImage(), outputRegionForThread );
    ImageRegionIterator< OutputImageType > oIt( output, outputRegionForThread );

    bgIt.GoToBegin();
    oIt.GoToBegin();

    while( !oIt.IsAtEnd() )
      {
      const OutputImagePixelType & bg = bgIt.Get();
      if( bg != this->m_OutputForegroundValue )
        {
        oIt.Set( bg );
        }
      else
        {
        oIt.Set( this->m_OutputBackgroundValue );
        }
      ++oIt;
      ++bgIt;
      }
    }
  else
    {
    // fill the background with the background value
    ImageRegionIterator< OutputImageType > oIt( output, outputRegionForThread );
    oIt.GoToBegin();

    while( !oIt.IsAtEnd() )
      {
      oIt.Set( this->m_OutputBackgroundValue );
      ++oIt;
      }
    }

  // wait for the other threads to complete that part
  m_Barrier->Wait();

  // and delegate to the superclass implementation to use the thread support for the label objects
  Superclass::ThreadedGenerateData( outputRegionForThread, threadId );

}


template<class TInputImage, class TOutputImage>
void
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData( LabelObjectType * labelObject )
{
  OutputImageType * output = this->GetOutput();

  typedef typename LabelObjectType::LineContainerType   LineContainerType;

  typename LineContainerType::const_iterator lit;
  LineContainerType & lineContainer = labelObject->GetLineContainer();

  for( lit = lineContainer.begin(); lit != lineContainer.end(); lit++ )
    {
    IndexType idx = lit->GetIndex();

    unsigned long length = lit->GetLength();

    for( unsigned int i=0; i<length; i++)
      {
      output->SetPixel( idx, this->m_OutputForegroundValue );
      idx[0]++;
      }
    }
}


template< class TInputImage, class TOutputImage >
void
LabelMapToBinaryImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "OutputForegroundValue: "
     << static_cast<typename NumericTraits<OutputImagePixelType>::PrintType>(this->m_OutputForegroundValue) << std::endl;
  os << indent << "OutputBackgroundValue: "
     << static_cast<typename NumericTraits<OutputImagePixelType>::PrintType>(this->m_OutputBackgroundValue) << std::endl;
  os << indent << "Barrier object: " << this->m_Barrier.GetPointer() << std::endl;
}


} // end namespace itk

#endif
