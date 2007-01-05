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
#ifndef __itkExpectationMaximizationImageClassificationWithPriors_txx
#define __itkExpectationMaximizationImageClassificationWithPriors_txx

#include "itkExpectationMaximizationImageClassification.h"
#include "itkImageRegionIterator.h"

namespace itk {

namespace Statistics {


template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::ExpectationMaximizationImageClassificationWithPriors()
{
   this->m_ClassProportions = ProportionsContainerType::New();
   this->m_ClassIntensityDistributions = IntensityDistributionContainerType::New();
}



template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::~ExpectationMaximizationImageClassificationWithPriors()
{
}




template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::AddIntensityDistributionDensity( 
              const GaussianDensityFunctionType * gaussian,
              ProportionType proportion )
{
   this->m_ClassIntensityDistributions->InsertElement( 
            this->m_ClassIntensityDistributions->Size(), gaussian );
            
   this->m_ClassProportions->InsertElement( 
            this->m_ClassProportions->Size(), proportion );

}





template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::GenerateData()
{
  this->Superclass::GenerateData();
}




 
template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::ComputeMaximization()
{
   // This method is inteded to be overloaded in a derived class
}


 
 
template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
bool
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::Converged() const
{
   // This method is inteded to be overloaded in a derived class
   return false;
}


 
template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::ComputeLabelMap()
{
}

 
template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::ComputeExpectation()
{
}


 


template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
unsigned int
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::GetNumberOfClasses() const
{
   const unsigned int numberOfClasses =  this->m_ClassPriorImage->GetVectorLength();
   return numberOfClasses;
}



 

template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::Initialize()
{

   this->Superclass::Initialize();

   if( !this->m_ClassPriorImage )
     {
     itkExceptionMacro("Priors image has not been connected. Please use SetClassPriors()");
     }


   const unsigned int numberOfClasses =  this->m_ClassPriorImage->GetVectorLength();

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




 

template < class TObservationsZImageType, class TParametersTheta, class TPriorPixelComponentType >
void
ExpectationMaximizationImageClassificationWithPriors< TObservationsZImageType, TParametersTheta, TPriorPixelComponentType >
::SetClassPriors( const PriorsImageType * priorImage )
{
   this->m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

