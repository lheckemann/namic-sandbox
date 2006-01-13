/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExpectationMaximizationMethod.txx,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:23 $
  Version:   $Revision: 1.23 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkExpectationMaximizationMethod_txx
#define _itkExpectationMaximizationMethod_txx

#include "itkExpectationMaximizationMethod.h"


namespace itk
{
/*
 * Constructor
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersThesta>
ExpectationMaximizationMethod
::ExpectationMaximizationMethod()
{
  this->ProcessObject::SetNthOutput( 0, ParametersType::New().GetPointer() );
  this->ProcessObject::SetNthOutput( 1, UnobservedDataType::New().GetPointer() );
}

/**
 * MakeOutput
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
DataObject::Pointer
ExpectationMaximizationMethod
::MakeOutput(unsigned int output)
{
  if (output == 0)
    {
    return static_cast<DataObject*>(ParametersType::New().GetPointer());
    }
  else if (output == 1)
    {
    return static_cast<DataObject*>(UnobservedDataType::New().GetPointer());
    }
  else
    {
    return this->ProcessObject::MakeOutput(output);
    }
}

/**
 * MakeOutput
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
void
ExpectationMaximizationMethod
::GenerateData()
{
  // Give the subclass a chance to initialize the parameters and
  // perform any other initialization
  this->Initialize();

  long iteration = 0;

  this->ComputeExpectation();
  while( !this->Converged() && iteration < m_MaximumIterations)
   {
   this->ComputeMaximization();
   this->ComputeExpectation();

   ++iteration;
   }
}



} // end namespace itk


#endif
