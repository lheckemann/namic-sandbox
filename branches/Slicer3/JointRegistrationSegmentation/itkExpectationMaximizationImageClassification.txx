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
#ifndef __itkExpectationMaximizationImageClassification_txx
#define __itkExpectationMaximizationImageClassification_txx

#include "itkExpectationMaximizationImageClassification.h"
#include "itkImageRegionIterator.h"

namespace itk {

namespace Statistics {


template < class TObservationsZImageType, class TParametersTheta >
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::ExpectationMaximizationImageClassification()
{
}



template < class TObservationsZImageType, class TParametersTheta >
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::~ExpectationMaximizationImageClassification()
{
}





template < class TObservationsZImageType, class TParametersTheta >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::GenerateData()
{
  this->Superclass::GenerateData();
  this->ComputeLabelMap();
}



 
template < class TObservationsZImageType, class TParametersTheta >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::ComputeLabelMap()
{
}

 
 
 
template < class TObservationsZImageType, class TParametersTheta >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::ComputeMaximization()
{
  // This method is intended to be overloaded in a derived class 
}

  
 
 
template < class TObservationsZImageType, class TParametersTheta >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::ComputeExpectation()
{
  // This method is intended to be overloaded in a derived class 
}

 
 
 
template < class TObservationsZImageType, class TParametersTheta >
bool
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::Converged() const
{
  // This method is intended to be overloaded in a derived class 
  return false;
}

 




template < class TObservationsZImageType, class TParametersTheta >
void
ExpectationMaximizationImageClassification< TObservationsZImageType, TParametersTheta >
::Initialize()
{

   this->Superclass::Initialize();
   
   const InputImageType * inputImage = 
            dynamic_cast< const InputImageType *>( this->GetObservations() );

   if( !inputImage )
     {
     itkExceptionMacro("Input image has not been connected. Please use SetInput()");
     }

}





} // end namespace Statistics

} // end namespace itk


#endif

