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
#include "itkVersor.h"

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

  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;
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
  this->m_DerivativeList->Reserve( this->m_InputMesh->GetNumberOfCells() );
  this->m_PointAreaAccumulatorList->Reserve( this->m_InputMesh->GetNumberOfPoints() );
  this->m_PointDerivativeAccumulatorList->Reserve( this->m_InputMesh->GetNumberOfPoints() );
}


/**
 * Compute the function
 */
template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::Compute()
{
  this->VerifyInputs(); 
  this->Initialize(); 

  // Start with gradient computation for each triangle. Uses linear interpolator.
  const CellsContainer * cells =  this->m_InputMesh->GetCells();

  CellsContainerConstIterator cellIterator = cells->Begin();
  CellsContainerConstIterator cellEnd = cells->End();

  BasisSystemListIterator basisSystemListIterator;
  basisSystemListIterator = m_BasisSystemList->Begin();
  
  const PointsContainer * points = this->m_InputMesh->GetPoints();

  PointIterator pointIterator = points->Begin();
  PointIterator pointEnd = points->End();

  //
  // Set all derivatives and area accumulators to Null vectors.
  //
  DerivativeType   derivative;
  derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType   parallelTransportedDerivative;

  while( pointIterator != pointEnd )
    {
    this->m_PointDerivativeAccumulatorList->SetElement( pointIterator.Index(), derivative);
    this->m_PointAreaAccumulatorList->SetElement(pointIterator.Index(), 0.0);
    pointIterator++; 
    }

  //
  // Look at all triangular cells, re-use the basis of each, and new scalar values.
  //
  const unsigned int numberOfVerticesInTriangle = 3;
  PixelType pixelValue[numberOfVerticesInTriangle]; 
  PointIdentifier pointIds[numberOfVerticesInTriangle];
  PointType point[numberOfVerticesInTriangle];

  while( cellIterator != cellEnd )
    {
    CellType* cellPointer = cellIterator.Value();

    // Consider current cell. Iterate through its points. 

    PointIdIterator pointIdIterator = cellPointer->PointIdsBegin();
    PointIdIterator pointIdEnd = cellPointer->PointIdsEnd();

    unsigned int i=0; 
    while( pointIdIterator != pointIdEnd )
      {
      const PointIdentifier pointId = *pointIdIterator; 
      pointIds[i]= pointId;
      pixelValue[i]= this->m_DataContainer->GetElement( pointId ); 
      point[i] = m_InputMesh->GetPoint( pointId );
      i++;
      ++pointIdIterator;
      }


    //
    // FIXME: does this change at all? If geometry stays the same,
    // which it should, do this in initialization, and store both
    // cell->area and point->accumulatedArea permanently. 
    //
    const AreaType area = TriangleType::ComputeArea( point[0], point[1], point[3] );


    //
    // contribute to accumulated area around each point of triangle
    //
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

    //
    //  Compute the coordinates of the point at the center of the cell.
    //
    const double equalWeight = 1.0 / 3.0;
    PointType cellCenter;
    cellCenter.SetToBarycentricCombination(
      point[0], point[1], point[2], equalWeight, equalWeight );

    VectorType vectorToCenter = cellCenter - this->m_SphereCenter;
    vectorToCenter *= this->m_SphereRadius / vectorToCenter.GetNorm();

    const PointType cellCenterProjectedInSphere = this->m_SphereCenter + vectorToCenter;
 
    //
    // Project derivative in the plane tangent to the sphere at the projected
    // point of the center cell
    //
    VectorType vectorToCenterNormalized = vectorToCenter;
    vectorToCenterNormalized.Normalize();

    const double radialComponent = derivative * vectorToCenterNormalized;

    DerivativeType  projectedDerivative;
    for( unsigned int k = 0; k < MeshDimension; k++ )
      {
      projectedDerivative[k] = derivative[k] - vectorToCenterNormalized[k] * radialComponent;
      }
        

    // 
    // Store the derivative for that cell.
    //
    this->m_DerivativeList->push_back( projectedDerivative );

    
    // Store at each vertex the value equal to triangle area x
    // derivative. Later, area-based weighting will use the total value
    // and divide by the sum of triangle areas about each vertex.
    // Begin by weighting contribution to point of this triangle by its area.

    projectedDerivative *= area;

   

    for( unsigned int i = 0; i < numberOfVerticesInTriangle; i++ )
      {
      // 
      // Parallel transport the derivative vector to each neighbor point
      //
      this->ParalelTransport( cellCenterProjectedInSphere, point[i], 
        projectedDerivative, parallelTransportedDerivative );

      //
      // then accumulate them there.
      //
      this->m_PointDerivativeAccumulatorList->ElementAt( pointIds[i] ) += parallelTransportedDerivative;
      }

    ++cellIterator;
    ++basisSystemListIterator;
    }

  pointIterator = points->Begin();
  pointEnd = points->End();

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

template <class TInputMesh, class TScalar >
void
NodeScalarGradientCalculator<TInputMesh, TScalar>
::ParalelTransport( 
    const PointType src, const PointType dst,
    const DerivativeType & inputVector, 
    DerivativeType & transportedVector ) const
{
  VectorType vsrc = src - this->m_SphereCenter;
  VectorType vdst = dst - this->m_SphereCenter;

  VectorType axis = CrossProduct( vsrc, vdst );

  const double scaledSinus   = axis.GetNorm();
  const double scaledCosinus = vsrc * vdst;

  double angle = vcl_atan2( scaledSinus, scaledCosinus );
  
  typedef Versor< double > VersorType;

  VersorType versor;
  versor.Set( axis, angle );

  transportedVector = versor.Transform( inputVector );
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
