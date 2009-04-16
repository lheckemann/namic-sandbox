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
  this->m_DerivativeList = DerivativeListType::New();
  this->m_PointAreaAccumulatorList = CoordRepListType::New();
  this->m_PointDerivativeAccumulatorList = DerivativeListType::New();

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
 * Check inputs
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::VerifyInputs( ) const
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
}


/**
 * Initialize internal variables
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Initialize( void )
{
  this->m_DerivativeList->Reserve( this->m_InputMesh->GetCells()->Size() );
  this->m_PointAreaAccumulatorList->Reserve( this->m_InputMesh->GetPoints()->Size() );
  this->m_PointDerivativeAccumulatorList->Reserve( this->m_InputMesh->GetPoints()->Size() );

}


/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Compute()
{
  DerivativeType   derivative;
  
  this->VerifyInputs(); 
  this->Initialize(); 

  // Start with gradient computation for each triangle. Uses linear interpolator.

  CellsContainerIterator cellIterator = this->m_InputMesh->GetCells()->Begin();
  CellsContainerIterator cellEnd = this->m_InputMesh->GetCells()->End();

  BasisSystemListIterator basisSystemListIterator;
  basisSystemListIterator = m_BasisSystemList->Begin();
  
  PointIterator pointIterator = this->m_InputMesh->GetPoints()->Begin();
  PointIterator pointEnd = this->m_InputMesh->GetPoints()->End();
  while( pointIterator != pointEnd )
    {
    derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );
    this->m_PointDerivativeAccumulatorList->SetElement(pointIterator.Index(), derivative);
    
    this->m_PointAreaAccumulatorList->SetElement(pointIterator.Index(), 0.0);
    pointIterator++; 
    }

  // Look at all triangular cells, re-use the basis of each, and new scalar values.
  const unsigned int numberOfVerticesInTriangle = 3;
  PixelType pixelValue[numberOfVerticesInTriangle]; 
  PointIdentifier pointIds[numberOfVerticesInTriangle];
  while( cellIterator != cellEnd )
    {
    CellType* cellPointer = cellIterator.Value();


    // Consider current cell. Iterate through its points. 

    PointIdIterator pointIdIterator = cellPointer->PointIdsBegin();
    PointIdIterator pointIdEnd = cellPointer->PointIdsEnd();

    unsigned int i=0; 
    while( pointIdIterator != pointIdEnd )
      {
      pointIds[i]= *pointIdIterator; 
      pixelValue[i]= this->m_DataContainer->GetElement( pointIds[i] ); 
      i++;
      ++pointIdIterator;
      }

    // Michel FIXME: does this change at all? If geometry stays the same,
    // which it should, do this in initialization, and store both
    // cell->area and point->accumulatedArea permanently. 
    
    AreaType area = TriangleType::ComputeArea(
      m_InputMesh->GetPoint( pointIds[0]) ,
      m_InputMesh->GetPoint( pointIds[1]) ,
      m_InputMesh->GetPoint( pointIds[2]) );

    // contribute to accumulated area around each point of triangle
    for( unsigned int i=0; i < numberOfVerticesInTriangle; i++ )
      {
      this->m_PointAreaAccumulatorList->ElementAt( pointIds[i] ) += area;
      }

    VectorType  m_U12;
    VectorType  m_U32;

    m_U12= basisSystemListIterator->Value().GetVector(0); 
    m_U32= basisSystemListIterator->Value().GetVector(1); 
 
    InterpolatorType::GetDerivativeFromPixelsAndBasis(
      pixelValue[0], pixelValue[1], pixelValue[2], m_U12, m_U32, derivative);

    this->m_DerivativeList->push_back( derivative );

    
    // Store at each vertex the value equal to triangle area x
    // derivative. Later, area-based weighting will use the total value
    // and divide by the sum of triangle areas about each vertex.
    // Begin by weighting contribution to point of this triangle by its area.

    derivative *= area;

    for( unsigned int i = 0; i < numberOfVerticesInTriangle; i++ )
      {
      this->m_PointDerivativeAccumulatorList->ElementAt( pointIds[i] ) += derivative;
      }

    ++cellIterator;
    ++basisSystemListIterator;
    }

  pointIterator = this->m_InputMesh->GetPoints()->Begin();
  pointEnd = this->m_InputMesh->GetPoints()->End();

  // Look at all vertices: consider the input edge of each, look at
  // all edges counter-clockwise from that input edge, and consider
  // face formed by consecutive edges. 
  while ( pointIterator != pointEnd )
    {
    // Divide area-weighted derivative by total area assigned to
    // each point. That is: total area of triangles surrounding it.
    this->m_PointDerivativeAccumulatorList->ElementAt( pointIterator.Index() ) /= 
      this->m_PointAreaAccumulatorList->GetElement( pointIterator.Index() );

    pointIterator++;
    }

}


/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
typename NodeScalarGradientCalculator<TInputMesh, TScalar>::OutputType
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Evaluate( const InputType& input  ) const 
{
  return this->m_PointDerivativeAccumulatorList->ElementAt( input );
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
  os << indent << "Input mesh = " << this->m_InputMesh.GetPointer() << std::endl;
  os << indent << "Data Container = " << this->m_DataContainer.GetPointer() << std::endl;
  os << indent << "Basis System List = " << this->m_BasisSystemList.GetPointer() << std::endl;
  os << indent << "Derivative List = " << this->m_DerivativeList.GetPointer() << std::endl;
  os << indent << "Point Area Accumulator = " << this->m_PointAreaAccumulatorList.GetPointer() << std::endl;
  os << indent << "Point Derivative Accumulator = " << this->m_PointDerivativeAccumulatorList.GetPointer() << std::endl;
}

#include <iostream>


} // end namespace itk

#endif
