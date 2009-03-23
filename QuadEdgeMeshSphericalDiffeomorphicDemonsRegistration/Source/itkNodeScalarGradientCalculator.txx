/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeScalarGradientCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNodeScalarGradientCalculator_txx
#define __itkNodeScalarGradientCalculator_txx

#include "itkNodeScalarGradientCalculator.h"


namespace itk
{

/**
 * Constructor
 */
template <class TInputMesh, class TScalar>
NodeScalarGradientCalculator<TInputMesh, TScalar>
::NodeScalarGradientCalculator()
{
}


/**
 * Destructor
 */
template <class TInputMesh, class TScalar>
NodeScalarGradientCalculator<TInputMesh, TScalar>
::~NodeScalarGradientCalculator()
{
}

/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Initialize( void ) const 
{

  if( this->m_InputMesh.IsNull() ) 
    {
    itkExceptionMacro(<<"NodeScalarGradientCalculator Initialize  m_InputMesh is NULL.");
    }
  
  if( this->m_DataContainer.IsNull() ) 
    {
    itkExceptionMacro(<<"NodeScalarGradientCalculator Initialize  m_DataContainer is NULL.");
    }

  if( this->m_BasisSystemList.IsNull() ) 
    {
    itkExceptionMacro(<<"NodeScalarGradientCalculator Initialize  m_BasisSystemList is NULL.");
    }

  return;
}


/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Compute()
{
  this->Initialize(); 

  // Add here computation at every node...
}



/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
typename NodeScalarGradientCalculator<TInputMesh, TScalar>::OutputType
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Evaluate( const InputType& input ) const 
{
  // Add here query from an array
  return OutputType();
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TScalar>
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}

#include <iostream>


} // end namespace itk

#endif
