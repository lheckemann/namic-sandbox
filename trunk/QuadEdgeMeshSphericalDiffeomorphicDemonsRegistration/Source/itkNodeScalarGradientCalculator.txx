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
 * Initialize by setting the input mesh
 */
template <class TInputMesh, class TScalar>
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::SetInputMesh( const TInputMesh * ptr )
{
  if( this->m_Mesh != ptr )
    {
    this->m_Mesh = ptr;
    this->Modified();
    }
}

/**
 * Return the input mesh
 */
template <class TInputMesh, class TScalar >
const TInputMesh *
NodeScalarGradientCalculator<TInputMesh, TScalar>
::GetInputMesh() const
{
  return this->m_Mesh;
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
