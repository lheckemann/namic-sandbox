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


  m_Interpolator.SetInputMesh( m_InputMesh );
  m_Interpolator.Initialize();
  m_DerivativeList.Reserve( this->m_InputMesh->GetCells()->Size() );

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

  // Start with gradient computation for each triangle. Uses linear interpolator.

  CellsContainerIterator cellIterator = this->m_InputMesh->GetCells()->Begin();
  CellsContainerIterator cellEnd = this->m_InputMesh->GetCells()->End();

  BasisSystemListIterator basisSystemListIterator;
  basisSystemListIterator = m_BasisSystemList->Begin();

  // Look at all triangular cells, re-use the basis of each, and new scalar values.
  while ( cellIterator != cellEnd ) {

    CellType* cellPointer = cellIterator.Value();

    //Consider current cell. Iterate through its points. 
    PixelType pixelValue[3]; 
    PointIdentifier pointIds[3];

    PointIdIterator pointIdIterator = cellPointer->PointIdsBegin();
    PointIdIterator pointIdEnd = cellPointer->PointIdsEnd();

    unsigned int i=0; 
    while( pointIdIterator != pointIdEnd )
      {
      pointIds[i]= *pointIdIterator; 
      pixelValue[i]= this->m_DataContainer->GetElement( pointIds[i] ); 
      i++;
    }

    VectorType  m_U12;
    VectorType  m_U32;

    m_U12= basisSystemListIterator->Value().GetVector(0); 
    m_U32= basisSystemListIterator->Value().GetVector(1); 
 
    DerivativeType derivative; 
    m_Interpolator.GetDerivativeFromPixelsAndBasis(pixelValue[0], pixelValue[1], pixelValue[2],
                                                   m_U12, m_U32, derivative);

    m_DerivativeList.push_back( derivative );

    ++cellIterator; ++basisSystemListIterator; 

  }

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
