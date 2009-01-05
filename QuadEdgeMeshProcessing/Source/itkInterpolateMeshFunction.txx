/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkInterpolateMeshFunction.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkInterpolateMeshFunction_txx
#define __itkInterpolateMeshFunction_txx

#include "itkInterpolateMeshFunction.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputMesh, class TCoordRep>
InterpolateMeshFunction<TInputMesh, TCoordRep>
::InterpolateMeshFunction()
{
  this->m_PointLocator = PointLocatorType::New();
}


/**
 * Destructor
 */
template <class TInputMesh, class TCoordRep>
InterpolateMeshFunction<TInputMesh, TCoordRep>
::~InterpolateMeshFunction()
{
}


/**
 * Prepare the internal data structures of the point locator
 */
template <class TInputMesh, class TCoordRep>
void
InterpolateMeshFunction<TInputMesh, TCoordRep>
::Initialize()
{
  this->m_PointLocator->SetPointSet( this->m_Mesh );
  this->Initialize();
}


template <class TInputMesh, class TCoordRep>
void
InterpolateMeshFunction<TInputMesh, TCoordRep>
::Search(const PointType &query,
         unsigned int numberOfNeighborsRequested,
         InstanceIdentifierVectorType& result) const
{
  typename PointLocatorType::PointType point( query );
  this->m_PointLocator->Search( point, numberOfNeighborsRequested, result );
}


template <class TInputMesh, class TCoordRep>
void
InterpolateMeshFunction<TInputMesh, TCoordRep>
::Search(const PointType &query,
         double radius,
         InstanceIdentifierVectorType& result) const
{
  typename PointLocatorType::PointType point( query );
  this->m_PointLocator->Search( point, radius, result );
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TCoordRep>
void
InterpolateMeshFunction<TInputMesh, TCoordRep>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}


/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
typename 
InterpolateMeshFunction<TInputMesh, TCoordRep>::OutputType
InterpolateMeshFunction<TInputMesh, TCoordRep>
::Evaluate( const PointType& point ) const
{
  // FIXME : add here, or in a derived class, the interpolation.
  return 0.0;
}


} // end namespace itk

#endif
