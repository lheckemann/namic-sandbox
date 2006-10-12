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
#ifndef _itkBayesianClassificationImageFilter_txx
#define _itkBayesianClassificationImageFilter_txx

#include "itkBayesianClassificationImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"

namespace itk
{

template < class TInputImage, class TMaskImage, class TOutputImage >
BayesianClassificationImageFilter< TInputImage, TMaskImage, TOutputImage >
::BayesianClassificationImageFilter()
  : m_NumberOfClasses( 0 ),
    m_NumberOfSmoothingIterations( 0 )
{
  m_Initializer = BayesianInitializerType::New();
  m_Classifier  = ClassifierFilterType::New();
}

template < class TInputImage, class TMaskImage, class TOutputImage >
void
BayesianClassificationImageFilter< TInputImage, TMaskImage, TOutputImage >
::GenerateData()
{
  // TODO Minipipeline could use a progress accumulator
  m_Initializer->SetInput( this->GetInput() );
  m_Initializer->SetNumberOfClasses( this->GetNumberOfClasses() );
  m_Initializer->SetMaskImage( this->GetMaskImage() );
  m_Initializer->SetMaskValue( this->GetMaskValue() );
  m_Initializer->Update();
  
  m_Classifier->SetInput( m_Initializer->GetOutput() );
  m_Classifier->SetNumberOfSmoothingIterations( 
        this->GetNumberOfSmoothingIterations() );

  // Assume that the smoothing filter is Anisotropic diffusion..
  // make this an option to switch between curvature flow - 
  // anisotropic diffusion etc....
  typedef typename ClassifierFilterType::ExtractedComponentImageType 
                                         ExtractedComponentImageType; 
  typedef itk::GradientAnisotropicDiffusionImageFilter<
    ExtractedComponentImageType, ExtractedComponentImageType >  SmoothingFilterType;
  typename SmoothingFilterType::Pointer smoother = SmoothingFilterType::New();
  smoother->SetNumberOfIterations( 1 );
  smoother->SetTimeStep( 0.125 );
  smoother->SetConductanceParameter( 3 );  
  m_Classifier->SetSmoothingFilter( smoother );

  m_Classifier->GraftOutput( this->GetOutput() );
  m_Classifier->Update();
  this->GraftOutput( m_Classifier->GetOutput() );
}

/**
 *  Print Self Method
 */
template < class TInputImage, class TMaskImage, class TOutputImage >
void
BayesianClassificationImageFilter< TInputImage, TMaskImage, TOutputImage >
::PrintSelf( std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "NumberOfClasses: " << m_NumberOfClasses << std::endl;
  os << indent << "Number of smoothing iterations =  " << m_NumberOfSmoothingIterations << std::endl;
}


} // end namespace itk
  
#endif 
