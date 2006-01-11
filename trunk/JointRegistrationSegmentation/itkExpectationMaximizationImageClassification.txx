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


template < class TImageType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TImageType >
::ExpectationMaximizationImageClassification()
{
}



template < class TImageType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TImageType >
::~ExpectationMaximizationImageClassification()
{
}




template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::AddIntensityDistributionDensity( 
              const GaussianDensityFunctionType * gaussian,
              ProportionType proportion )
{
   m_ClassIntensityDistributions.push_back( gaussian );
   m_ClassProportions.push_back( proportion );
}





template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::Update()
{
   this->GenerateData();
}




template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::GenerateData()
{
   this->ComputeExpectation();
   this->ComputeMaximization();
}



 

template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::ComputeMaximization()
{
}


 

template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::ComputeExpectation()
{

  typedef itk::ImageRegionIterator< WeightsImageType >        WeightsIterator;
  typedef itk::ImageRegionConstIterator< PriorsImageType >    PriorsImageIterator;
  typedef itk::ImageRegionConstIterator< CorrectedImageType > CorrectedImageIterator;

   
  WeightsIterator        witr( m_WeightsImage, m_WeightsImage->GetBufferedRegion() );
  PriorsImageIterator    pitr( m_PriorImage,   m_PriorImage->GetBufferedRegion()   );
  CorrectedImageIterator citr( m_CorrectedImage, m_CorrectedImage->GetBufferedRegion()   );

  witr.GoToBegin();
  pitr.GoToBegin();
  citr.GoToBegin();
   
  while( witr.IsAtEnd() )
    {
    index = witr.GetIndex();
    
    typename WeightsImageType::PointType inputPoint;
    m_WeightsImage->TransformIndexToPhysicalPoint( index, inputPoint );

    typename Superclass::OutputPointType transformedPoint = 
                    this->m_Transform->TransformPoint( inputPoint );

    if( !this->m_PriorImage->IsInside( transformedPoint ) )
      {

      if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
        {
        const RealType priors  = this->m_Interpolator->Evaluate( transformedPoint );
        citr.Get();
        witr.Set();
        }

      }

    ++witr;
    ++pitr;
    ++citr;
    }

}



 

template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::InitializeWeightsImage()
{
   m_WeightsImage = WeightsImageType::New();

   m_WeightsImage->CopyInformation( m_InputImage );

   m_WeightsImage->Allocate();
}



 

template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::SetInput( const InputImageType * inputImage )
{
   m_InputImage = inputImage;
}


 

template < class TImageType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType >
::SetClassPrior( const PriorsImageType * priorImage )
{
   m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

