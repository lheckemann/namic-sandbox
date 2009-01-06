/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLinearInterpolateMeshFunction.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLinearInterpolateMeshFunction_txx
#define __itkLinearInterpolateMeshFunction_txx

#include "itkLinearInterpolateMeshFunction.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputMesh, class TCoordRep>
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::LinearInterpolateMeshFunction()
{
}


/**
 * Destructor
 */
template <class TInputMesh, class TCoordRep>
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::~LinearInterpolateMeshFunction()
{
}



/**
 * Standard "PrintSelf" method
 */
template <class TInputMesh, class TCoordRep>
void
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::PrintSelf( std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );
}


/**
 * Evaluate the mesh at a given point position.
 */
template <class TInputMesh, class TCoordRep>
typename 
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>::OutputType
LinearInterpolateMeshFunction<TInputMesh, TCoordRep>
::Evaluate( const PointType& point ) const
{
  typedef typename Superclass::InstanceIdentifierVectorType InstanceIdentifierVectorType;

  const unsigned int numberOfNeighbors = 3;
  InstanceIdentifierVectorType result;

  this->Search( point, numberOfNeighbors, result );

  //
  // FIXME: This is missing the search for the cell in which the query point is embedded.
  //        a temporary computation is included here, but doesn't really corresponds to
  //        a linear interpolation in a cell yet.
  //
  PixelType pixelValue1 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue2 = itk::NumericTraits< PixelType >::Zero;
  PixelType pixelValue3 = itk::NumericTraits< PixelType >::Zero;

  const PointIdentifier pointId1 = result[0];
  const PointIdentifier pointId2 = result[1];
  const PointIdentifier pointId3 = result[2];

  this->GetPointData( pointId1, &pixelValue1 ); 
  this->GetPointData( pointId2, &pixelValue2 ); 
  this->GetPointData( pointId3, &pixelValue3 ); 

  RealType pixelValueReal1 = static_cast< RealType >( pixelValue1 );
  RealType pixelValueReal2 = static_cast< RealType >( pixelValue2 );
  RealType pixelValueReal3 = static_cast< RealType >( pixelValue3 );

  RealType returnValue = ( pixelValueReal1 + pixelValueReal2 + pixelValueReal3 ) / 3.0;

  return returnValue;
}


} // end namespace itk

#endif
