/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKJetRecursiveGaussianImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/05/06 15:20:16 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkKJetRecursiveGaussianImageFilter_txx
#define _itkKJetRecursiveGaussianImageFilter_txx

#include "itkKJetRecursiveGaussianImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{


/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::KJetRecursiveGaussianImageFilter()
{

  m_NormalizeAcrossScale = false;

  // Create a process accumulator for tracking the progress of this
  // minipipeline
  m_Progress = ProgressAccumulator::New();
  m_Progress->SetMiniPipelineFilter(this);
 
  // Compute the contribution of each filter to the total progress.
  const double weight = 
     1.0 / ( ImageDimension * ( ImageDimension * ( ImageDimension+1 ) / 2 ));


  for( unsigned int i = 0; i<NumberOfSmoothingFilters; i++ )
    {
    GaussianFilterPointer filter = GaussianFilterType::New();
    filter->SetOrder( GaussianFilterType::ZeroOrder );
    filter->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
    filter->ReleaseDataFlagOn();
    m_Progress->RegisterInternalFilter( filter, weight );
    m_SmoothingFilters.push_back( filter );
    }

  m_DerivativeFilterA = DerivativeFilterAType::New();
  m_DerivativeFilterB = DerivativeFilterBType::New();

  m_DerivativeFilterA->SetOrder( DerivativeFilterAType::FirstOrder );
  m_DerivativeFilterA->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
  
  m_DerivativeFilterB->SetOrder( DerivativeFilterBType::FirstOrder );
  m_DerivativeFilterB->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
  
  m_DerivativeFilterA->SetInput( this->GetInput() );
  m_DerivativeFilterB->SetInput( m_DerivativeFilterA->GetOutput() );

  m_Progress->RegisterInternalFilter( m_DerivativeFilterA, weight );
  m_Progress->RegisterInternalFilter( m_DerivativeFilterB, weight );

  // Deal with the 2D case.
  if( NumberOfSmoothingFilters > 0 )
    {
    m_SmoothingFilters[0]->SetInput( m_DerivativeFilterB->GetOutput() );
    }

  for( unsigned int i = 1; i<NumberOfSmoothingFilters; i++ )
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
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::SetSigma( RealType sigma )
{

  for( unsigned int i = 0; i<NumberOfSmoothingFilters; i++ )
    {
    m_SmoothingFilters[ i ]->SetSigma( sigma );
    }
  m_DerivativeFilterA->SetSigma( sigma );
  m_DerivativeFilterB->SetSigma( sigma );

  this->Modified();

}



/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void 
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( bool normalize )
{

  m_NormalizeAcrossScale = normalize;

  for( unsigned int i = 0; i<NumberOfSmoothingFilters; i++ )
    {
    m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( normalize );
    }
  m_DerivativeFilterA->SetNormalizeAcrossScale( normalize );
  m_DerivativeFilterB->SetNormalizeAcrossScale( normalize );

  this->Modified();

}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>::InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
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
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage >
::GenerateData(void)
{

  itkDebugMacro(<< "KJetRecursiveGaussianImageFilter generating data ");

  m_Progress->ResetProgress();

  const typename TInputImage::ConstPointer   inputImage( this->GetInput() );

  m_ImageAdaptor->SetImage( this->GetOutput() );

  m_ImageAdaptor->SetLargestPossibleRegion( 
    inputImage->GetLargestPossibleRegion() );

  m_ImageAdaptor->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  m_ImageAdaptor->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  m_ImageAdaptor->Allocate();

  m_DerivativeFilterA->SetInput( inputImage );
  m_DerivativeFilterB->SetInput( m_DerivativeFilterA->GetOutput() );

  unsigned int element = 3;

  for( unsigned int dima=0; dima < ImageDimension; dima++ )
    {
    for( unsigned int dimb=dima; dimb < ImageDimension; dimb++ )
      {
 std::cout << "at " << element << " element." << std::endl;
      unsigned int i=0; 
      unsigned int j=0;
      while( i < NumberOfSmoothingFilters )
        {
        while( j < ImageDimension )
          {
          if( j != dima && j != dimb ) 
            {
            m_SmoothingFilters[ i ]->SetDirection( j );
            j++;
            break;
            }
          j++;
          }
          i++;
        }

      m_DerivativeFilterA->SetDirection( dima );
      m_DerivativeFilterB->SetDirection( dimb );
      
      typename RealImageType::Pointer derivativeImage; 

      // Deal with the 2D case.
      if( NumberOfSmoothingFilters > 0 )
        {
        GaussianFilterPointer lastFilter = m_SmoothingFilters[ImageDimension-3];
        lastFilter->Update();
        derivativeImage = lastFilter->GetOutput(); 
        }
      else
        {
        m_DerivativeFilterB->Update();
        derivativeImage = m_DerivativeFilterB->GetOutput(); 
        }

      m_Progress->ResetFilterProgressAndKeepAccumulatedProgress();

      // Copy the results to the corresponding component
      // on the output image of vectors
      m_ImageAdaptor->SelectNthElement( element++ );


      ImageRegionIteratorWithIndex< RealImageType > it( 
              derivativeImage, 
              derivativeImage->GetRequestedRegion() );

      ImageRegionIteratorWithIndex< OutputImageAdaptorType > ot( 
        m_ImageAdaptor, 
        m_ImageAdaptor->GetRequestedRegion() );
    
      const RealType spacingA = inputImage->GetSpacing()[ dima ];
      const RealType spacingB = inputImage->GetSpacing()[ dimb ];

      const RealType factor = spacingA * spacingB;

      it.GoToBegin();
      ot.GoToBegin();
      while( !it.IsAtEnd() )
        {
        ot.Set( it.Get() / factor );
        ++it;
        ++ot;
        }

std::cout << " finished with element " <<element-1  << std::endl;
      }//  for dimb = dima:Dimension   loop
        
 std::cout << "at " << dima << " element." << std::endl;

      typename RealImageType::Pointer derivative1Image; 
      derivative1Image = m_DerivativeFilterA->GetOutput(); 
  
 //     m_Progress->ResetFilterProgressAndKeepAccumulatedProgress();

      // Copy the results to the corresponding component
      // on the output image of vectors
      m_ImageAdaptor->SelectNthElement( dima );

      ImageRegionIteratorWithIndex< RealImageType > itg( 
              derivative1Image, 
              derivative1Image->GetRequestedRegion() );

      ImageRegionIteratorWithIndex< OutputImageAdaptorType > otg( 
        m_ImageAdaptor, 
        m_ImageAdaptor->GetRequestedRegion() );
    
      const RealType spacingA = inputImage->GetSpacing()[ dima ];

      itg.GoToBegin();
      otg.GoToBegin();
      while( !itg.IsAtEnd() )
        {
        otg.Set( itg.Get() / spacingA );
        ++itg;
        ++otg;
        }
std::cout << " finished with element " <<dima  << std::endl;
    
    }//  for  dima = 0:Dimension   loop
  

}


template <typename TInputImage, typename TOutputImage>
void
KJetRecursiveGaussianImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "NormalizeAcrossScale: " << m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif
