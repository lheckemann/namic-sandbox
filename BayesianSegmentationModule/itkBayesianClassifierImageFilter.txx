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
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::BayesianClassifierImageFilter()
{
   m_UserProvidedPriors = false;
   m_UserProvidedSmoothingFilter = false;
}




/**
 *  Print Self Method
 */
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
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
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::GenerateData()
{
  // Setup input image
  const InputImageType * membershipImage = this->GetInput();

  // Setup general parameters
  const unsigned int numberOfClasses = membershipImage->GetVectorLength();

  if( numberOfClasses == 0 )
    {
    itkExceptionMacro("The number of components in the input Membership image is Zero !");
    return;
    }

  this->AllocateOutputs();

  this->ComputeBayesRule();
  
  if( m_UserProvidedSmoothingFilter )
    {
    this->NormalizeAndSmoothPosteriors();
    }

  this->ClassifyBasedOnPosteriors();

}




/**
 * Compute the labeled map with no priors and no smoothing
 */
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::AllocateOutputs()
{
  const InputImageType * membershipImage = this->GetInput();

  this->GetOutput()->SetRegions( membershipImage->GetBufferedRegion() );
  this->GetOutput()->SetSpacing( membershipImage->GetSpacing() );
  this->GetOutput()->SetOrigin(  membershipImage->GetOrigin() );

  //
  // FIXME : Allocate also the Posteriors.
  // The first output is the Image of Labels, 
  // The second output is the image of Posteriors.
  //
  // FIXME : call SetNumberOfOutputs(2);
  //


  this->Superclass::AllocateOutputs();
}




/**
 * Compute the posteriors using the Bayes rule. If no priors are available,
 * then the posteriors are just a copy of the memberships.  */
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::ComputeBayesRule()
{

  const InputImageType * membershipImage = this->GetInput();

  ImageRegionType   imageRegion  = membershipImage->GetBufferedRegion();

  if( m_UserProvidedPriors )
    {
    const PriorsImageType * priorsImage = 
             dynamic_cast< const PriorsImageType * >( this->GetInput(1) ); 

    if( priorsImage == NULL )
      {
      itkExceptionMacro("Second input type does not correspond to expected Priors Image Type");
      }

    PosteriorsImageType * posteriorsImage = 
             dynamic_cast< PosteriorsImageType * >( this->GetOutput(1) ); 

    if( posteriorsImage == NULL )
      {
      itkExceptionMacro("Second output type does not correspond to expected Posteriors Image Type");
      }


    InputImageIteratorType       itrMembershipImage(  membershipImage, imageRegion );
    PriorsImageIteratorType      itrPriorsImage(      priorsImage,     imageRegion );
    PosteriorsImageIteratorType  itrPosteriorsImage(  posteriorsImage, imageRegion );

    itrMembershipImage.GoToBegin();
    itrPriorsImage.GoToBegin();

    const unsigned int numberOfClasses = membershipImage->GetVectorLength();
    
    while( !itrMembershipImage.IsAtEnd() )
      {
      PosteriorsPixelType posteriors;
      const PriorsPixelType      priors      = itrPriorsImage.Get();
      const MembershipPixelType  memberships = itrMembershipImage.Get();
      for( unsigned int i=0; i<numberOfClasses; i++)
         {
         posteriors[i] =
            static_cast< TPosteriorsPrecisionType >( memberships[i] * priors[i] ); 
         }   
      itrPosteriorsImage.Set( posteriors );
      ++itrMembershipImage;
      ++itrPriorsImage;
      ++itrPosteriorsImage;
      }
    }
  else
    {
    PosteriorsImageType * posteriorsImage = 
             dynamic_cast< PosteriorsImageType * >( this->GetOutput(1) ); 

    if( posteriorsImage == NULL )
      {
      itkExceptionMacro("Second input type does not correspond to expected Posteriors Image Type");
      }

    InputImageIteratorType      itrMembershipImage( membershipImage, imageRegion );
    PosteriorsImageIteratorType itrPosteriorsImage( posteriorsImage, imageRegion );

    itrMembershipImage.GoToBegin();
    itrPosteriorsImage.GoToBegin();
    
    while( !itrMembershipImage.IsAtEnd() )
      {
      itrPosteriorsImage.Set( itrMembershipImage.Get() ); 
      ++itrMembershipImage;
      ++itrPosteriorsImage;
      }

    }

}


/**
 * Normalize the posteriors and smooth them using an user-provided.
 */
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::NormalizeAndSmoothPosteriors()
{
}


/**
 * Compute the labeled map based on the Maximum rule applied to the posteriors.
 */
template < class TInputVectorImage, class TLabelsType, 
           class TPosteriorsPrecisionType, class TPriorsPrecisionType >
void 
BayesianClassifierImageFilter<TInputVectorImage, TLabelsType, 
                              TPosteriorsPrecisionType, TPriorsPrecisionType >
::ClassifyBasedOnPosteriors()
{
  OutputImagePointer labels = this->GetOutput();

  ImageRegionType   imageRegion  = labels->GetBufferedRegion();

  PosteriorsImageType * posteriorsImage = 
           dynamic_cast< PosteriorsImageType * >( this->GetOutput(1) ); 

  if( posteriorsImage == NULL )
    {
    itkExceptionMacro("Second output type does not correspond to expected Posteriors Image Type");
    }

  OutputImageIteratorType      itrLabelsImage(     labels,         imageRegion );
  PosteriorsImageIteratorType  itrPosteriorsImage( posteriorsImage,imageRegion );

  DecisionRulePointer decisionRule = DecisionRuleType::New();

  itrLabelsImage.GoToBegin();
  itrPosteriorsImage.GoToBegin();

  while ( !itrLabelsImage.IsAtEnd() )
    {
    itrLabelsImage.Set( decisionRule->Evaluate( itrPosteriorsImage.Get() ) );
    ++itrLabelsImage;
    ++itrPosteriorsImage;
    }

}



} // end namespace itk

#endif
