/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageRegistrationMethod2.h,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:22 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkExpectationMaximizationAlgorithm_txx
#define __itkExpectationMaximizationAlgorithm_txx

#include "itkExpectationMaximizationImageClassification.h"
#include "itkImageRegionIterator.h"

namespace itk {

namespace Statistics {


template < class TImageType >
ExpectationMaximizationImageClassification< TImageType >
::ExpectationMaximizationImageClassification()
{
}



template < class TImageType >
ExpectationMaximizationImageClassification< TImageType >
::~ExpectationMaximizationImageClassification()
{
}




template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::AddIntensityDistributionDensity( 
              const GaussianDensityFunctionType * gaussian,
              ProportionType proportion )
{
   m_Components.push_back( gaussian );
   m_Proportions.push_back( proportion );
}





template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::Update()
{
   this->GenerateData();
}




template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::GenerateData()
{
   this->ComputeExpectation();
   this->ComputeMaximization();
}



 

template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::ComputeMaximization()
{
}


 

template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::ComputeExpectation()
{

   typedef itk::ImageRegionIterator< WeightsImageType >     WeightsIterator;
   typedef itk::ImageRegionConstIterator< InputImageType >     InputImageIterator;
   typedef itk::ImageRegionConstIterator< PriorsImageType >    PriorsImageIterator;

   
   WeightsIterator     witr( m_WeightsImage, m_WeightsImage->GetBufferedRegion() );
   InputImageIterator  iitr( m_InputImage,   m_InputImage->GetBufferedRegion()   );
   PriorsImageIterator pitr( m_PriorImage,   m_PriorImage->GetBufferedRegion()   );

   witr.GoToBegin();
   iitr.GoToBegin();
   pitr.GoToBegin();
   
   while( witr.IsAtEnd() )
     {
     iitr.Get();
     ++witr;
     ++iitr;
     ++pitr;
     }

}



 

template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::InitializeWeightsImage()
{
   m_WeightsImage = WeightsImageType::New();

   m_WeightsImage->CopyInformation( m_InputImage );

   m_WeightsImage->Allocate();
}



 

template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::SetInput( const InputImageType * inputImage )
{
   m_InputImage = inputImage;
}


 

template < class TImageType >
void
ExpectationMaximizationImageClassification< TImageType >
::SetClassPrior( const PriorsImageType * priorImage )
{
   m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

