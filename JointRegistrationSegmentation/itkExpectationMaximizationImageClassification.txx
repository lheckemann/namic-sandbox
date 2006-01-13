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


template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ExpectationMaximizationImageClassification()
{
   this->m_ClassProportions = ProportionsContainerType::New();
   this->m_ClassIntensityDistributions = IntensityDistributionContainerType::New();
}



template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::~ExpectationMaximizationImageClassification()
{
}




template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::AddIntensityDistributionDensity( 
              const GaussianDensityFunctionType * gaussian,
              ProportionType proportion )
{
   this->m_ClassIntensityDistributions->InsertElement( 
            this->m_ClassIntensityDistributions->Size(), gaussian );
            
   this->m_ClassProportions->InsertElement( 
            this->m_ClassProportions->Size(), proportion );

}





template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::GenerateData()
{
  this->Superclass::GenerateData();
  this->ComputeLabelMap();
}




 
template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeMaximization()
{

}


 
 
template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
bool
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::Converge() const
{

}


 
template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeLabelMap()
{
}

 
template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeExpectation()
{
}



 

template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::Initialize()
{

   if( !this->m_InputImage )
     {
     itkExceptionMacro("Input image has not been connected. Please use SetInput()");
     }

   if( !this->m_ClassPriorImage )
     {
     itkExceptionMacro("Priors image has not been connected. Please use SetClassPrior()");
     }

   this->m_WeightsImage = WeightsImageType::New();

   const unsigned int numberOfClasses =  this->m_ClassPriorImage->GetVectorLength();

   this->m_WeightsImage->CopyInformation( this->m_InputImage );
   this->m_WeightsImage->SetVectorLength( numberOfClasses ); 
   this->m_WeightsImage->Allocate();


   //
   //   Checking that the number of intensity distribution match 
   //   the number of components of the priors image.
   //
   if( this->m_ClassIntensityDistributions->Size() != numberOfClasses )
     { 
     itkExceptionMacro("Number of provided Intensity Distributions does not match the number of components in the priors");
     }

   if( this->m_ClassProportions->Size() != numberOfClasses )
     { 
     itkExceptionMacro("Number of classes proportions does not match the number of components in the priors");
     }

}



 

template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetInput( const InputImageType * inputImage )
{
   this->m_InputImage = inputImage;
}


 

template < class TObservationsZImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetClassPrior( const PriorsImageType * priorImage )
{
   this->m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

