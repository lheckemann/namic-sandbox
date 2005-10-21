/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierInitializationImageFilter.txx,v $
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
#ifndef _itkBayesianClassifierInitializationImageFilter_txx
#define _itkBayesianClassifierInitializationImageFilter_txx

#include "itkBayesianClassifierInitializationImageFilter.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputImage>
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::BayesianClassifierInitializationImageFilter()
{
}


/**
 * Add a membership function
 */
template <class TInputImage, class TOutputImage>
void
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::AddMembershipFunction( const MembershipFunctionType * newFunction )
{
   MembershipFunctionConstPointer functionSmartPointer = newFunction;
   m_MembershipFunctions.push_back( functionSmartPointer );
}


/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Number of membership functions = " << m_MembershipFunctions.size() << std::endl;
  for( unsigned int i=0; i< m_MembershipFunctions.size(); i++)
    {
    os << indent << m_MembershipFunctions[i].GetPointer() << std::endl;
    }

}

/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::GenerateData()
{

  // SETUP INPUT IMAGE
  const InputImageType * inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetBufferedRegion();
  typename InputImageType::SpacingType  imageSpacing = inputImage->GetSpacing();
  typename InputImageType::PointType    imageOrigin  = inputImage->GetOrigin();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );


  // SETUP GENERAL PARAMETERS
  const unsigned int numberOfClasses = m_MembershipFunctions.size();
  if( numberOfClasses == 0 )
    {
    itkExceptionMacro("No membership functions have been set up.  Please call AddMembershipFunction() first");
    return;
    }


  // INITIALIZE PRIORS TO DEFAULT UNIFORMITY
  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetSpacing( imageSpacing );
  priors->SetOrigin( imageOrigin );
  priors->SetVectorLength( numberOfClasses );
  priors->Allocate();
  PriorImageIteratorType itrPriorImage( priors, imageRegion );
  PriorPixelType priorPixel( numberOfClasses );

  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    priorPixel[i] = (double)1 / numberOfClasses;
    }
  itrPriorImage.GoToBegin();
  while( !itrPriorImage.IsAtEnd() )
    {
    itrPriorImage.Set( priorPixel );
    ++itrPriorImage;
    }
/* DEBUGGING */
for ( unsigned int i = 0; i < 21639; ++i)
  {
  --itrPriorImage;
  }
std::cout << "Prior image in initial section " << itrPriorImage.Get() << std::endl; //debugging
/* DEBUGGING */


  // GENERATE MEMBERSHIP DATA IMAGE
  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( imageOrigin );
  data->SetSpacing( imageSpacing );
  data->SetVectorLength( numberOfClasses );
  data->Allocate();
  MembershipImageIteratorType itrDataImage( data, imageRegion );
  MembershipPixelType membershipPixel( numberOfClasses );
  MeasurementVectorType mv;
  
  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    membershipPixel[i] = 0.0;
    }
  itrDataImage.GoToBegin();
  while( !itrDataImage.IsAtEnd() )
    {
    itrDataImage.Set( membershipPixel );
    ++itrDataImage;
    }

  itrDataImage.GoToBegin();
  itrInputImage.GoToBegin();
  while ( !itrDataImage.IsAtEnd() )
    {
    mv = itrInputImage.Get();
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
}
} // end namespace itk

#endif
