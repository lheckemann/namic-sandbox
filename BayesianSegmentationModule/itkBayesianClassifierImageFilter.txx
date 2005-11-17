/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/03/08 20:29:20 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkBayesianClassifierImageFilter_txx
#define _itkBayesianClassifierImageFilter_txx

#include "itkBayesianClassifierImageFilter.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

/**
 *  Constructor
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::BayesianClassifierImageFilter()
{
   m_UserProvidedPriors = false;
   m_UserProvidedSmoothingFilter = false;
}




/**
 *  Print Self Method
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "User provided priors =  " << m_UserProvidedPriors << std::endl;
  os << indent << "User provided smooting filter =  " << m_UserProvidedSmoothingFilter << std::endl;
  os << indent << "Smooting filter pointer =  " << m_SmoothingFilter.GetPointer() << std::endl;

}

/**
 * Generate Data method is where the classification (and smoothing) is performed.
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::GenerateData()
{

  // SETUP INPUT IMAGE
  const InputImageType * membershipImage = this->GetInput();

  typename InputImageType::RegionType   imageRegion  = membershipImage->GetBufferedRegion();
  typename InputImageType::SpacingType  imageSpacing = membershipImage->GetSpacing();
  typename InputImageType::PointType    imageOrigin  = membershipImage->GetOrigin();

  InputImageIteratorType                itrMembershipImage( membershipImage, imageRegion );


  // SETUP GENERAL PARAMETERS
  const unsigned int numberOfClasses = membershipImage->GetVectorLength();

  if( numberOfClasses == 0 )
    {
    itkExceptionMacro("The number of components in the input Membership image is Zero !");
    return;
    }

  if( m_UserProvidedPriors )
    {
    if( m_UserProvidedSmoothingFilter )
      {
      this->ComputeWithPriorsWithSmoothing();
      }
    else
      {
      this->ComputeWithPriorsNoSmoothing();
      }
    }
  else
    {
    if( m_UserProvidedSmoothingFilter )
      {
      this->ComputeNoPriorsWithSmoothing();
      }
    else
      {
      this->ComputeNoPriorsNoSmoothing();
      }
    }

#if 0

  // GENERATE MEMBERSHIP DATA IMAGE
  MembershipImageIteratorType itrDataImage( inputImage, imageRegion );
  MembershipPixelType membershipPixel( numberOfClasses );
  MeasurementVectorType mv;
  
  itrDataImage.GoToBegin();
  MembershipputImage.GoToBegin();
  while ( !itrDataImage.IsAtEnd() )
    {
    mv = MembershipputImage.Get();
    for ( unsigned int i = 0; i < numberOfClasses; i++ )
      {
      membershipPixel[i] = m_MembershipFunctions[i]->Evaluate( mv );
      }
    itrDataImage.Set( membershipPixel );
    ++itrInputImage;
    ++itrDataImage;
    }
/* DEBUGGING */
for ( unsigned int i = 0; i < 21639; ++i)
  {
  --itrInputImage;
  --itrDataImage;
  }
std::cout << "Input image in initial section " << itrInputImage.Get() << std::endl; //debugging
std::cout << "Data image in initial section " << itrDataImage.Get() << std::endl; //debugging
/* DEBUGGING */


  // GENERATE POSTERIORS BY APPLYING BAYES' RULE
  PosteriorImagePointer posteriors = PosteriorImageType::New();
  posteriors->SetRegions( imageRegion );
  posteriors->SetSpacing( imageSpacing );
  posteriors->SetOrigin( imageOrigin );
  posteriors->SetVectorLength( numberOfClasses );
  posteriors->Allocate();
  PosteriorImageIteratorType itrPosteriorImage( posteriors, imageRegion );
  PosteriorPixelType posteriorPixel( numberOfClasses );

  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    posteriorPixel[i] = 0.0;
    }
  itrPosteriorImage.GoToBegin();
  while( !itrPosteriorImage.IsAtEnd() )
    {
    itrPosteriorImage.Set( posteriorPixel );
    ++itrPosteriorImage;
    }

  itrDataImage.GoToBegin();
  itrPriorImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  while ( !itrPosteriorImage.IsAtEnd() )
    {
    priorPixel = itrPriorImage.Get();
    membershipPixel = itrDataImage.Get();
    for ( unsigned int i = 0; i < numberOfClasses; ++i )
      {
      posteriorPixel[i] = membershipPixel[i] * priorPixel[i];
      }
    itrPosteriorImage.Set( posteriorPixel );
    ++itrDataImage;
    ++itrPriorImage;
    ++itrPosteriorImage;
    }
/* DEBUGGING */
for ( unsigned int i = 0; i < 21639; ++i)
  {
  --itrPosteriorImage;
  }
std::cout << "Initial Posteriors " << itrPosteriorImage.Get() << std::endl; //debugging
/* DEBUGGING */


  // APPLY MAXIMUM A POSTERIORI RULE
  this->GetOutput()->SetRegions( imageRegion );
  this->GetOutput()->SetSpacing( imageSpacing );
  this->GetOutput()->SetOrigin( imageOrigin );
  AllocateOutputs();
  OutputImageType::Pointer labels = this->GetOutput();
  OutputImageIteratorType itrLabelImage( labels, imageRegion );
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();

  itrLabelImage.GoToBegin();
  itrPosteriorImage.GoToBegin();
  while ( !itrLabelImage.IsAtEnd() )
    {
    itrLabelImage.Set( decisionRule->Evaluate( itrPosteriorImage.Get() ) );
    ++itrLabelImage;
    ++itrPosteriorImage;
    }
/* DEBUGGING */
for ( unsigned int i = 0; i < 21639; ++i)
  {
  --itrLabelImage;
  --itrPosteriorImage;
  }
std::cout << "Label image in initial section " << itrLabelImage.Get() << std::endl; //debugging
std::cout << "Posteriors after decision rule in initial section " << itrPosteriorImage.Get() << std::endl; //debugging
/* DEBUGGING */

#endif
}

/**
 * Compute the labeled map with no priors and no smoothing
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::ComputeNoPriorsNoSmoothing()
{
}


/**
 * Compute the labeled map with priors and no smoothing
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::ComputeWithPriorsNoSmoothing()
{
}


/**
 * Compute the labeled map with no priors and with smoothing
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::ComputeNoPriorsWithSmoothing()
{
}


/**
 * Compute the labeled map with priors and with smoothing
 */
template < class TInputVectorImage, class TLabelType, 
           class TPosteriorPrecisionType, class TPriorPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelType, 
                              TPosteriorPrecisionType, TPriorPrecisionType >
::ComputeWithPriorsWithSmoothing()
{
}


} // end namespace itk

#endif
