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
  this->m_Interpolator = InterpolatorType::New();
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
  this->m_Interpolator->SetInputMesh( this->m_InputMesh );
  this->m_Interpolator->Initialize();

  //Michel: this is in constructor, no? this->m_DerivativeList = DerivativeListType::New();
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
  DerivativeType derivative, oldDerivative, reallyOldDerivative; 
  CoordRepType  area;
  
  this->VerifyInputs(); 
  this->Initialize(); 

  // Start with gradient computation for each triangle. Uses linear interpolator.

  CellsContainerIterator cellIterator = this->m_InputMesh->GetCells()->Begin();
  CellsContainerIterator cellEnd = this->m_InputMesh->GetCells()->End();

  BasisSystemListIterator basisSystemListIterator;
  basisSystemListIterator = m_BasisSystemList->Begin();
  typedef typename PointType::CoordRepType        CoordRepType;
  
  PointIterator pointIterator = this->m_InputMesh->GetPoints()->Begin();
  PointIterator pointEnd = this->m_InputMesh->GetPoints()->End();
  while( pointIterator != pointEnd )
    {
    derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );
    this->m_PointDerivativeAccumulatorList->SetElement(pointIterator.Index(), derivative);
    
    this->m_PointAreaAccumulatorList->SetElement(pointIterator.Index(), 0.0);
    pointIterator++; 

    }

  int cellIndex=0; 
  // Look at all triangular cells, re-use the basis of each, and new scalar values.
  PointType fixedPoint; 
  PixelType pixelValue[3]; 
  PointIdentifier pointIds[3];
  while ( cellIterator != cellEnd )
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

    //Michel FIXME: does this change at all? If geometry stays the same,
    //which it should, do this in initialization, and store both
    //cell->area and point->accumulatedArea permanently. 
    
    area =static_cast< CoordRepType >(
       TriangleType::ComputeArea(m_InputMesh->GetPoint( pointIds[0]) ,
                                 m_InputMesh->GetPoint( pointIds[1]) ,
                                 m_InputMesh->GetPoint( pointIds[2])));

    for (int i=0; i<3; i++)
      {
      // contribute to accumulated area around each point of triangle
      CoordRepType  accumulatedArea= this->m_PointAreaAccumulatorList->GetElement( pointIds[i] );

      accumulatedArea += area;
      
      this->m_PointAreaAccumulatorList->SetElement(pointIds[i], accumulatedArea);
      }

    VectorType  m_U12;
    VectorType  m_U32;

    m_U12= basisSystemListIterator->Value().GetVector(0); 
    m_U32= basisSystemListIterator->Value().GetVector(1); 
 
    this->m_Interpolator->GetDerivativeFromPixelsAndBasis(
      pixelValue[0], pixelValue[1], pixelValue[2], m_U12, m_U32, derivative);

    this->m_DerivativeList->push_back( derivative );

    
    //Store at each vertex the value equal to triangle area x
    //derivative. Later, area-based weighting will use the total value
    //and divide by the sum of triangle areas about each vertex.
    //Begin by weighting contribution to point of this triangle by its area.

    derivative *= area;

    for (int i=0; i<3; i++)
      {
      oldDerivative= this->m_PointDerivativeAccumulatorList->GetElement(pointIds[i]);

      oldDerivative += derivative;  

      this->m_PointDerivativeAccumulatorList->SetElement(pointIds[i], oldDerivative);
      }

    ++cellIterator;
    ++basisSystemListIterator;
    ++cellIndex; 
    }

  pointIterator = this->m_InputMesh->GetPoints()->Begin();
  pointEnd = this->m_InputMesh->GetPoints()->End();

  // Look at all vertices: consider the input edge of each, look at
  // all edges counter-clockwise from that input edge, and consider
  // face formed by consecutive edges. 
  int pointIndex=0; 
  while ( pointIterator != pointEnd )
    {

    fixedPoint.CastFrom( pointIterator.Value() );
    derivative = this->m_PointDerivativeAccumulatorList->GetElement(pointIterator.Index());
    area = this->m_PointAreaAccumulatorList->ElementAt(pointIterator.Index());

    //Divide area-weighted derivative by total area assigned to
    //each point, i.e.: total area of triangles surrounding it.
     
    derivative /= area; 

    this->m_PointDerivativeAccumulatorList->SetElement(pointIterator.Index(), derivative);

    pointIterator++;
    pointIndex++; 
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
  OutputType result;
  
  result= this->m_PointDerivativeAccumulatorList->GetElement(input);

  return result;
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
