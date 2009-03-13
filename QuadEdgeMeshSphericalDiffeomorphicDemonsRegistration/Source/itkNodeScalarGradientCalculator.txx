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
template <class TInputMesh, class TCoordRep>
NodeScalarGradientCalculator<TInputMesh, TCoordRep>
::NodeScalarGradientCalculator()
{
    this->m_ScalarArraySize= 0; 
}


/**
 * Destructor
 */
template <class TInputMesh, class TCoordRep>
NodeScalarGradientCalculator<TInputMesh, TCoordRep>
::~NodeScalarGradientCalculator()
{
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TCoordRep>
void
NodeScalarGradientCalculator<TInputMesh, TCoordRep>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}

#include <iostream>


template <class TInputMesh, class TCoordRep>
void
NodeScalarGradientCalculator<TInputMesh, TCoordRep>
::SetScalarArray( RealDataPointer inScalarArray)
{
  if (inScalarArray!=NULL)
  { 
    this->m_ScalarArray = inScalarArray;
    this->m_ScalarArraySize= sizeof(*inScalarArray)/sizeof(RealDataType); 
  }

  //FIXME: does this have to help in the mapping for the data population?

  // Evaluate gradient at each triangle onces, then average these gradients
  // At each point and store in gradient array. 

}

/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
typename 
NodeScalarGradientCalculator<TInputMesh, TCoordRep>::OutputType
NodeScalarGradientCalculator<TInputMesh, TCoordRep>
::Evaluate( unsigned int index ) const
{

  if (index > this->m_ScalarArraySize)
    {
    itkExceptionMacro("NodeScalarGradientCalculator Evaluate index is larger than number of vertices.");  
    }

  // Read gradient at each triangle based on stored value
  return this->m_GradientArray[ index ];
}

} // end namespace itk

#endif
