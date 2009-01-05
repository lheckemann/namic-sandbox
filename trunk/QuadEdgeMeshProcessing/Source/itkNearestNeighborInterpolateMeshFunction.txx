/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNearestNeighborInterpolateMeshFunction.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNearestNeighborInterpolateMeshFunction_txx
#define __itkNearestNeighborInterpolateMeshFunction_txx

#include "itkNearestNeighborInterpolateMeshFunction.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputMesh, class TCoordRep>
NearestNeighborInterpolateMeshFunction<TInputMesh, TCoordRep>
::NearestNeighborInterpolateMeshFunction()
{
}


/**
 * Destructor
 */
template <class TInputMesh, class TCoordRep>
NearestNeighborInterpolateMeshFunction<TInputMesh, TCoordRep>
::~NearestNeighborInterpolateMeshFunction()
{
}



/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TCoordRep>
void
NearestNeighborInterpolateMeshFunction<TInputMesh, TCoordRep>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}


/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
typename 
NearestNeighborInterpolateMeshFunction<TInputMesh, TCoordRep>::OutputType
NearestNeighborInterpolateMeshFunction<TInputMesh, TCoordRep>
::Evaluate( const PointType& point ) const
{
  typedef typename Superclass::InstanceIdentifierVectorType InstanceIdentifierVectorType;

  const unsigned int numberOfNeighbors = 1;
  InstanceIdentifierVectorType result;

  this->Search( point, numberOfNeighbors, result );

  // FIXME: Use the returned pointId for accessing the PointData entries
  return 0.0;
}


} // end namespace itk

#endif
