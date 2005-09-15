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
 *
 */
template <class TInputImage, class TOutputImage>
BayesianClassifierImageFilter<TInputImage, TOutputImage>
::BayesianClassifierImageFilter()
{

}


/**
 * Add a membership function
 */
template <class TInputImage, class TOutputImage>
void
BayesianClassifierImageFilter<TInputImage, TOutputImage>
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
BayesianClassifierImageFilter<TInputImage, TOutputImage>
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
BayesianClassifierImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();  

  const unsigned int numberOfClasses = m_MembershipFunctions.size();
  if( numberOfClasses == 0 )
    {
    itkExceptionMacro("No membership functions have been set up.  Please call AddMembershipFunction() first");
    return;
    }


  const InputImageType * inputImage = this->GetInput();

  typename InputImageType::RegionType   imageRegion  = inputImage->GetBufferedRegion();
  typename InputImageType::SpacingType  imageSpacing = inputImage->GetSpacing();
  typename InputImageType::PointType    imageOrigin  = inputImage->GetOrigin();

  PosteriorImagePointer posteriors = PosteriorImageType::New();
  posteriors->SetRegions( imageRegion );
  posteriors->SetSpacing( imageSpacing );
  posteriors->SetOrigin( imageOrigin );
  posteriors->SetVectorLength( numberOfClasses );
  posteriors->Allocate();

  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetSpacing( imageSpacing );
  priors->SetOrigin( imageOrigin );
  priors->SetVectorLength( numberOfClasses );
  priors->Allocate();

  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( imageOrigin );
  data->SetSpacing( imageSpacing );
  data->SetVectorLength( numberOfClasses );
  data->Allocate();

  typedef ImageRegionConstIterator< InputImageType >  ItertatorType;

  ItertatorType it( this->GetInput(), 
                    this->GetInput()->GetRequestedRegion() );

  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    it.Get();
    ++it;
    }


}


} // end namespace itk

#endif
