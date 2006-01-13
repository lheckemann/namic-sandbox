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


template < class TObservationsZImageType >
ExpectationMaximizationImageClassification< TObservationsZImageType >
::ExpectationMaximizationImageClassification()
{
}



template < class TObservationsZImageType >
ExpectationMaximizationImageClassification< TObservationsZImageType >
::~ExpectationMaximizationImageClassification()
{
}





template < class TObservationsZImageType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType >
::GenerateData()
{
  this->Superclass::GenerateData();
  this->ComputeLabelMap();
}



 
template < class TObservationsZImageType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType >
::ComputeLabelMap()
{
}

 
 
 
template < class TObservationsZImageType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType >
::ComputeMaximization()
{
  // This method is intended to be overloaded in a derived class 
}

  
 
 
template < class TObservationsZImageType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType >
::ComputeExpectation()
{
  // This method is intended to be overloaded in a derived class 
}

 
 
 
template < class TObservationsZImageType >
bool
ExpectationMaximizationImageClassification< TObservationsZImageType >
::Converged() const
{
  // This method is intended to be overloaded in a derived class 
}

 




template < class TObservationsZImageType >
void
ExpectationMaximizationImageClassification< TObservationsZImageType >
::Initialize()
{
   this->m_InputImage = dynamic_cast< const InputImageType *>( this->GetObservations() );

   if( !this->m_InputImage )
     {
     itkExceptionMacro("Input image has not been connected. Please use SetInput()");
     }

}





} // end namespace Statistics

} // end namespace itk


#endif

