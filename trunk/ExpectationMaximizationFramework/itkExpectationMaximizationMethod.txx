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
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::ExpectationMaximizationMethod()
{
  m_MaximumNumberOfIterations = 1;
  
  this->ProcessObject::SetNthOutput( 0, ParametersType::New().GetPointer() );
  this->ProcessObject::SetNthOutput( 1, UnobservedVariablesPosteriorType::New().GetPointer() );
}

/**
 * MakeOutput
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
DataObject::Pointer
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::MakeOutput(unsigned int output)
{
  if (output == 0)
    {
    return static_cast<DataObject*>(ParametersType::New().GetPointer());
    }
  else if (output == 1)
    {
    return static_cast<DataObject*>(UnobservedVariablesPosteriorType::New().GetPointer());
    }
  else
    {
    return this->ProcessObject::MakeOutput(output);
    }
}


/**
 *  Connect the observations as input.
 */ 
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
void
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::SetObservations( const ObservationsType *observations )
{
  const ObservationsType * currentObservations = 
                dynamic_cast< const ObservationsType * >( this->GetInput(0) );
  
  if( observations && observations != currentObservations )
    {
    this->ProcessObject::SetNthInput( 0, const_cast< ObservationsType * >( observations ) );
    this->Modified();
    }
}


/**
 *  Retrieve the parameters
 */ 
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
const typename ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >::ParametersType *
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::GetParameters() const
{
  const DataObject     * tt = this->GetOutput(0);
  const ParametersType * pp = dynamic_cast< const ParametersType * >( tt );  
  return pp; 
}



/**
 *  Retrieve the parameters
 */ 
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
const typename ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >::UnobservedVariablesPosteriorType *
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::GetUnobservedVariablesPosterior() const
{
  const DataObject     * tt = this->GetOutput(1);
  const UnobservedVariablesPosteriorType * pp = 
              dynamic_cast< const UnobservedVariablesPosteriorType * >( tt );  
  return pp; 
}



/**
 *  Retrieve the Observations
 */ 
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
const typename ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >::ObservationsType *
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::GetObservations() const
{ 
   return this->GeInput(); 
}



/**
 * Initialize before entering the iteration loop in GenerateData().
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
void
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::Initialize()
{
  // This method is intended for being overloaded by any class that requires 
  // to do some initialization before starting the iterations in GenerateData().
}


/**
 * GenerateData
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
void
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::GenerateData()
{

  //
  // Give the subclass a chance to initialize the parameters and
  // perform any other initialization
  //
  this->Initialize();

  unsigned long iteration = 0;

  this->ComputeExpectation();

  while( !this->Converged() && iteration < m_MaximumNumberOfIterations )
    {
    this->ComputeMaximization();
    this->ComputeExpectation();
 
    ++iteration;
    }
}


/**
 * PrintSelf
 */
template <typename TObservationsZ, typename TUnobservedDataY, typename TParametersTheta>
void
ExpectationMaximizationMethod< TObservationsZ, TUnobservedDataY, TParametersTheta >
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Maximum number of iterations: " << m_MaximumNumberOfIterations << std::endl;
}


} // end namespace itk


#endif
