/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeScalarGradientCalculator.h,v $
  Language:  C++
  Date:      $Date: 2008-05-14 09:26:05 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkResampleDestinationPointsQuadEdgeMeshFilter_txx
#define __itkResampleDestinationPointsQuadEdgeMeshFilter_txx

#include "itkResampleDestinationPointsQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkVersor.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
ResampleDestinationPointsQuadEdgeMeshFilter< TInputMesh, TReferenceMesh, TOutputMesh >
::ResampleDestinationPointsQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
ResampleDestinationPointsQuadEdgeMeshFilter< TInputMesh, TReferenceMesh, TOutputMesh >
::~ResampleDestinationPointsQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
void
ResampleDestinationPointsQuadEdgeMeshFilter< TInputMesh, TReferenceMesh, TOutputMesh >
::SetReferenceMesh( const ReferenceMeshType * mesh )
{
  itkDebugMacro("setting input ReferenceMesh to " << mesh);
  if( mesh != static_cast<const ReferenceMeshType *>(this->ProcessObject::GetInput( 1 )) )
    {
    this->ProcessObject::SetNthInput(1, const_cast< ReferenceMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
const typename ResampleDestinationPointsQuadEdgeMeshFilter<TInputMesh, TReferenceMesh, TOutputMesh>::ReferenceMeshType *
ResampleDestinationPointsQuadEdgeMeshFilter< TInputMesh, TReferenceMesh, TOutputMesh >
::GetReferenceMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const ReferenceMeshType * referenceMesh = 
    static_cast<const ReferenceMeshType *>( surrogate->ProcessObject::GetInput(1) );
  return referenceMesh;
}


template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
void
ResampleDestinationPointsQuadEdgeMeshFilter< TInputMesh, TReferenceMesh, TOutputMesh >
::GenerateData()
{

  OutputMeshPointer outputMesh = this->GetOutput();

  //
  // Visit all nodes of the Mesh 
  //
  OutputPointsContainerPointer points = outputMesh->GetPoints();

  if( points.IsNull() )
    {
    itkExceptionMacro("Mesh has NULL PointData");
    }

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  ProgressReporter progress(this, 0, numberOfPoints);

  std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

  // Initialize the internal point locator structure 
  this->m_Interpolator->SetInputMesh( this->GetReferenceMesh() );
  this->m_Interpolator->Initialize();

  typedef typename OutputMeshType::PointsContainer::ConstIterator    PointIterator;
  typedef typename OutputMeshType::PointDataContainer::Iterator      PointDataIterator;

  PointIterator pointItr = points->Begin();
  PointIterator pointEnd = points->End();

  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    MappedPointType;

  OutputPointType  inputPoint;
  OutputPointType  pointToEvaluate;

  while( pointItr != pointEnd )
    {
    inputPoint.CastFrom( pointItr.Value() );

    MappedPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    pointToEvaluate.CastFrom( transformedPoint );
//    outputPointItr.Value() = this->m_Interpolator->Evaluate( pointToEvaluate );

    progress.CompletedPixel();

    ++pointItr;
    }

}

} // end namespace itk

#endif
